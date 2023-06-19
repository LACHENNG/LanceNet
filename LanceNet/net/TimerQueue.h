// Author : Lance @ nwpu
//
#ifndef LanceNet_NET_TIMER_QUEQUE_H
#define LanceNet_NET_TIMER_QUEQUE_H
#include "LanceNet/base/Time.h"
#include "LanceNet/base/noncopyable.h"
#include <memory>
#include <vector>
#include <functional>
#include <queue>
#include <functional>
#include <set>

namespace LanceNet
{
namespace net
{

class Timer;
class FdChannel;
class EventLoop;
class TimerId;
//
// A timer queue includes some timers and corresponding callbacks at expiration that should be invoke by eventloop using a single timer fd notifying mechanism
// It use a fdChannel to observe the readable events of tiemrfd_
// the member function is transfer(if necessary) its execution to its owner Eventloop(IO thread), so no mutex lock is needed
//
// Impl detail : each time a Timer expires or a new timer add in or
// cancled,  the timefd is properly reset by timerfd_settime to
// guarantee that the timerfd is notified by the time next Timer expires but,
// so that every Timer is properly dealed with

// But no guarantee that the callback of the `Timer` is handled on time at its expiration,
// however the timerfd is notified by the kernel in time as it is usually reset
// by timerfd_settime to the next expiration of the next unexpired `Timer`s
//
class TimerQueue : noncopyable
{
public:
    // Note: I not use unique_ptr as it not work well when 
    // using std::bind to get a std::function<void()> functor as 
    // std::function only support CopyConstructible callable
    using TimerPtr = std::shared_ptr<Timer>;

    TimerQueue(EventLoop* loop);
    ~TimerQueue();

    // add a timer to queue
    // thread safe
    TimerId addTimer(TimerPtr timer);

    // cancel a timer
    // thread safe
    void cancelTimer(TimerId timerid);

private:
    using Callback = std::function<void (Timer*)>;
    using Entry = std::pair<TimeStamp, TimerPtr>;
    using EntryRaw = std::pair<TimeStamp, Timer*>;

    void _addTimer(TimerPtr timer);
    void _cancelTimer(TimerId timerid);

    // get the timer that has expired so far
    std::vector<TimerPtr> getExpired(TimeStamp now);

    // callback on alarms of timerfd_
    void handleTimeouts();
    void handleRead();
    int createTimerfd();
    // add a new timer, return if it is the earliest one that expiration
    bool insertTimer(TimerPtr timer);

    // reset Timerfd to alarm at newAlarmTime
    void resetTimerfd(TimeStamp newAlarmTime);

    // reset timerfd according to the given expired timers.
    void reset(std::vector<TimerPtr>& expiredTimers);

    // helper to get a itimerspec struct of difference of two TimeStamp
    itimerspec getAbsTimeSpec(TimeStamp now, TimeStamp future);

private:
    EventLoop* owner_loop_;

    std::priority_queue<Entry, std::vector<Entry>, std::greater<Entry>> timer_que_;

    int timerfd_;
    // used to observe timerfd events
    std::unique_ptr<FdChannel> fdChannelPtr_;

     // for cancel()
    std::set<TimerId>  cancelingTimers_;
    std::set<TimerId> activeTimers_;
};


}  // namespace net
}  // namespace LanceNet

#endif // LanceNet_NET_TIMER_QUEQUE_H
