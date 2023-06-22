#include <LanceNet/base/Time.h>
#include <LanceNet/net/TimerQueue.h>
#include <LanceNet/net/FdChannel.h>
#include <LanceNet/net/EventLoop.h>
#include <LanceNet/net/Timer.h>
#include <LanceNet/net/TimerId.h>
#include <LanceNet/base/unix_wrappers.h>
#include <LanceNet/base/Logging.h>

#include <chrono>
#include <cstdint>
#include <iterator>
#include <utility>
#include <algorithm>
#include <vector>
#include <assert.h>
#include <algorithm>
#include <functional>
#include <ctime>
#include <sys/timerfd.h>

namespace LanceNet
{
namespace net
{

TimerQueue::TimerQueue(EventLoop* loop)
  : owner_loop_(loop),
    timerfd_(createTimerfd()),
    fdChannelPtr_(std::make_unique<FdChannel>(loop, timerfd_))
{
    LOG_INFOC << "made timer fd = " << timerfd_;
    // let the fdChannel take effect
    fdChannelPtr_->setReadCallback(std::bind(&TimerQueue::handleTimeouts, this));
    fdChannelPtr_->enableReading();
}

TimerQueue::~TimerQueue()
{
    // default dtor is ok
    // Note fdChannel is auto unregistered by its own dtor
}

TimerId TimerQueue::addTimer(TimerPtr timer)
{
    TimerId timerid(timer.get(), timer->id());
    //auto func = [this, timer]()
    //{
    //    _addTimer(std::move(timer));
    //};
    owner_loop_->runInLoop(
      std::bind(&TimerQueue::_addTimer, this, timer)
    );

    return timerid;
}

void TimerQueue::_addTimer(TimerPtr timer)
{
    owner_loop_->assertInEventLoopThread();
    // if the inserted timer is the earliest one that to will be expired, adjust the timefd
    bool earilyChanged = insertTimer(timer);
    if(earilyChanged){
        TimeStamp earliestExpireTime = timer_que_.top().first;
        LOG_INFOC << "resetTimerfd to earliestExpiretime = " << earliestExpireTime.toFmtString();
        resetTimerfd(earliestExpireTime);
    }
    //LOG_INFO << "sleep for 2s in _addTimer";
    //sleep(2);
    //LOG_INFO << "sleeped ";
}

void TimerQueue::cancelTimer(TimerId timerid)
{
    owner_loop_->runInLoop(
        std::bind(&TimerQueue::_cancelTimer, this, timerid)
    );
}

void TimerQueue::_cancelTimer(TimerId timerid)
{
    owner_loop_->assertInEventLoopThread();
    auto rv =
        cancelingTimers_.insert(timerid);
    assert(rv.second); (void)rv;
}

std::vector<TimerQueue::TimerPtr> TimerQueue::getExpired(TimeStamp now)
{
    std::vector<Entry> expiredEntries;
    TimeStamp sentry(TimeStamp::now());
    while(!timer_que_.empty() && timer_que_.top().first <= sentry){
        auto timer = timer_que_.top();
        expiredEntries.emplace_back(timer);

        timer_que_.pop();
        activeTimers_.erase(TimerId(timer.second.get(), timer.second->id()));
    }

    assert(timer_que_.size() == activeTimers_.size());
    std::vector<TimerPtr> expiredTimers;
    auto ExtractTimerPtrFunc = [&](auto& entry){
        expiredTimers.push_back(entry.second);
    };
    std::for_each(expiredEntries.begin(),
                  expiredEntries.end(),
                  ExtractTimerPtrFunc);
    return expiredTimers;
}

int TimerQueue::createTimerfd()
{
    int timerfd = Timerfd_create(CLOCK_MONOTONIC,
                                   TFD_NONBLOCK | TFD_CLOEXEC);
    return timerfd;
}

void TimerQueue::handleRead()
{
    uint64_t tmp;
    Read(timerfd_, &tmp, sizeof(tmp));
}

void TimerQueue::handleTimeouts()
{
    handleRead();
    std::vector<TimerPtr> expiredTimers =  getExpired(TimeStamp::now());
    for(auto& timerPtr : expiredTimers ){
        timerPtr->OnExpiration();
    }
    // check if the expired Timers is repeatable, and properly reset the timerfd
    reset(expiredTimers);
}

bool TimerQueue::insertTimer(TimerPtr timer)
{
    owner_loop_->assertInEventLoopThread();
    assert(activeTimers_.size() == timer_que_.size());

    // the new insert timer is the earliest to expire?
    bool isTheEarliest = false;
    if(timer_que_.empty() || timer->expiration() < timer_que_.top().first)
    {
        isTheEarliest = true;
    }
    {
        timer_que_.push({timer->expiration(), timer});
        auto rv =
            activeTimers_.insert(TimerId(timer.get(), timer->id()));
        assert(rv.second); (void)rv; // supress unused warn
    }

    assert(activeTimers_.size() == timer_que_.size());
    return isTheEarliest;
}

void TimerQueue::resetTimerfd(TimeStamp newAlarmTime)
{
    auto timenow = TimeStamp::now();

    struct itimerspec newtimespec = getAbsTimeSpec(timenow, newAlarmTime);
    // TFD_TIMER_ABSTIME(abs time) or 0 (relative time)
    Timerfd_settime(timerfd_, 0, &newtimespec, NULL);

}

void TimerQueue::reset(std::vector<TimerPtr>& expiredTimers)
{
    assert(activeTimers_.size() == timer_que_.size());

    if(expiredTimers.empty())
        return;

    std::vector<TimerPtr> freelist;
    for(auto& timerPtr : expiredTimers){
        TimerId timerid(timerPtr.get(), timerPtr->id());

        if(timerPtr->isRepeat() &&
          cancelingTimers_.find(timerid) == cancelingTimers_.end())
        {
            LOG_INFOC << "restart timer id = " << timerPtr->id();
            // the timer expired is a repeat timer, restart and add to timerlist
            timerPtr->restartTimer();
            insertTimer(timerPtr);
        }
        else{
            // the timer that not used will automatically deleted by unique_ptr as living the scope of local expiredTimers variable
            assert(timerPtr.use_count() == 1);
            freelist.push_back(timerPtr);
        }

    }
    TimeStamp nextEarliestExpiration;
    if(!timer_que_.empty())
    {
        nextEarliestExpiration = timer_que_.top().first;
    }

    if(nextEarliestExpiration.isValid())
    {
        LOG_INFO << "resetTimerfd with new expiration = " << nextEarliestExpiration.toFmtString();
        resetTimerfd(nextEarliestExpiration);
    }
    assert(activeTimers_.size() == timer_que_.size());
    // item in free list is auto released
}

itimerspec TimerQueue::getAbsTimeSpec(TimeStamp now, TimeStamp future)
{
    auto nanoSeconds = TimeStamp::getDiffInNanoSeconds(now, future);
    // set at least 100 microseconds
    if(nanoSeconds < 100*1e3) nanoSeconds = 100 * 1e3;

    itimerspec spec;
    bzero(&spec, sizeof(itimerspec));
    spec.it_value.tv_sec = nanoSeconds / TimeStamp::NanoSecondsPerSecond;
    spec.it_value.tv_nsec = nanoSeconds % TimeStamp::NanoSecondsPerSecond;
    return spec;
}

} // namespace net
} // namespace LanceNet



