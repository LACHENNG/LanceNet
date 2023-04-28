#include <LanceNet/base/Time.h>
#include <LanceNet/net/TimerQueue.h>
#include <LanceNet/net/FdChannel.h>
#include <LanceNet/net/EventLoop.h>
#include <LanceNet/net/Timer.h>
#include <LanceNet/net/TimerId.h>
#include <LanceNet/base/unix_wrappers.h>

#include <bits/types/struct_itimerspec.h>
#include <chrono>
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
  : timerfd_(createTimerfd()),
    fdChannelPtr_(std::make_unique<FdChannel>(loop, timerfd_))
{
    // let the fdChannel take effect
    fdChannelPtr_->setReadCallback(std::bind(&TimerQueue::handleTimeouts, this));
    fdChannelPtr_->enableReading();
}

TimerQueue::~TimerQueue()
{
    Close(timerfd_);
    // Note fdChannel is auto unregistered by its own dtor
}

void TimerQueue::addTimer(TimerUPtr timer, double delaySeconds)
{
    // move calls to EventLoop thread
    // fdChannelPtr_->runInLoop(std::bind());

    // if the inserted timer is the earliest one that to will be expired, adjust the timefd
    bool earilyChanged = insertTimer(std::move(timer), delaySeconds);
    if(earilyChanged){
        TimeStamp earliestExpireTime = timerList_.begin()->first;
        resetTimerfd(earliestExpireTime);
    }
}

std::vector<TimerQueue::TimerUPtr> TimerQueue::getExpired(TimeStamp now)
{
    std::vector<Entry> expiredEntry;
    std::vector<TimerUPtr> expiredTimers;

    // O(lg N)
    auto gtriter = timerList_.lower_bound(std::make_pair(now, TimerUPtr()));

    std::move(timerList_.begin(), gtriter, std::back_inserter(expiredTimers));
    timerList_.erase(timerList_.begin(), gtriter);

    // check if the expired Timers is repeatable, and properly reset the timerfd
    reset(expiredEntry);

    for(auto& entry : expiredEntry)
    {
        expiredTimers.push_back(std::move(entry.second));
    }
    return expiredTimers;
}

int TimerQueue::createTimerfd()
{
    int timerfd = Timerfd_create(CLOCK_MONOTONIC,
                                   TFD_NONBLOCK | TFD_CLOEXEC);
    return timerfd;
}

void TimerQueue::handleTimeouts()
{
    std::vector<TimerUPtr> TimerList =  getExpired(TimeStamp::now());
    for(auto& timerUptr : TimerList){
        timerUptr->OnExpiration();
    }
}

bool TimerQueue::insertTimer(TimerUPtr timer, double delaySeconds)
{
    // the new insert timer is the earliest to expire?
    bool insertOneIsTheEarliest = false;

    delaySeconds = std::max(0.0, delaySeconds);

    auto newTimestamp = timer->expiration() + delaySeconds;
    auto previousTimeStamp = timerList_.begin()->first;

    if(newTimestamp < previousTimeStamp){
        insertOneIsTheEarliest = true;
    }

    timer->resetExpiration(newTimestamp);
    timerList_.insert(std::make_pair(newTimestamp,std::move(timer)));

   return insertOneIsTheEarliest;
}

void TimerQueue::resetTimerfd(TimeStamp newAlarmTime)
{
    // you can not expire in the past
    auto timenow = TimeStamp::now();
    assert(timenow < newAlarmTime);

     struct itimerspec newtimespec = getAbsTimeSpec(timenow, newAlarmTime);

    Timerfd_settime(timerfd_, TFD_TIMER_ABSTIME, &newtimespec, NULL);

}

void TimerQueue::reset(const std::vector<Entry>& expiredTimers)
{

    for(auto& timerEntry : expiredTimers){
        auto stamp = timerEntry.first;
        auto& timerUPtr = timerEntry.second;
        TimerId timerid = timerUPtr->getMyTimerId();

        if(timerUPtr->isRepeat() &&
          cancelingTimers_.find(timerid) == cancelingTimers_.end())
        {
            // the timer expired is a repeat timer, restart and add to timerlist
            timerUPtr->restartTimer();
            insertTimer(std::move(timerUPtr), 0);
        }
        else{
            // the timer that not used will automatically deleted by unique_ptr as living the scope of local expiredTimers variable
        }

    }
    TimeStamp nextEarliestExpiration;
    if(!timerList_.empty())
    {
        nextEarliestExpiration = timerList_.begin()->first;
    }

    if(nextEarliestExpiration.isValid())
    {
        resetTimerfd(nextEarliestExpiration);
    }
}

itimerspec TimerQueue::getAbsTimeSpec(TimeStamp now, TimeStamp future)
{
    const int kSec2NanoSec = 1e9;
    // get abs time difference
    double seconds = TimeStamp::getDiffInSeconds(now, future);
    int64_t sec= (int) seconds;
    int64_t nanosec = (seconds - sec) * kSec2NanoSec;

    itimerspec spec;
    bzero(&spec, sizeof(itimerspec));
    spec.it_value.tv_sec = sec;
    spec.it_value.tv_nsec = nanosec;
    return spec;
}



} // namespace net
} // namespace LanceNet



