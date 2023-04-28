// Author : Lance @ nwpu
//
#ifndef LanceNet_NET_TIMER_QUEQUE_H
#define LanceNet_NET_TIMER_QUEQUE_H
#include "LanceNet/base/Time.h"
#include <vector>
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
class TimerQueue
{
public:
    using TimerUPtr = std::unique_ptr<Timer>;

    TimerQueue(EventLoop* loop);
    ~TimerQueue();

    // add a timer to queue
    // thread safe
    void addTimer(TimerUPtr timer, double delay);

    // get the timer that has expired so far
    // thread safe
    std::vector<TimerUPtr> getExpired(TimeStamp now);

private:
    using Callback = std::function<void ()>;
    using Entry = std::pair<TimeStamp, TimerUPtr>;

    int createTimerfd();

    void handleTimeouts();

    // add a new timer, return if it is the earliest one that expiration
    bool insertTimer(TimerUPtr timer, double delaySeconds);

    // reset Timerfd to alarm at newAlarmTime
    void resetTimerfd(TimeStamp newAlarmTime);
    // reset timerfd according to the given expired timers.
    void reset(const std::vector<Entry>& expiredTimers);

    itimerspec getAbsTimeSpec(TimeStamp now, TimeStamp future);

private:
    // sorted by expiration, managing timer and can easily get expired timers in lg(N) time
    std::set<Entry> timerList_;

    int timerfd_;
    // used to observe timerfd events
    std::unique_ptr<FdChannel> fdChannelPtr_;

     // for cancel()
    std::set<TimerId>  cancelingTimers_;
};

} // namespace net
}// namespace LanceNet
 //
#endif // LanceNet_NET_TIMER_QUEQUE_H
