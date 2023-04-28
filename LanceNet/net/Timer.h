// Author : Lance @ nwpu
//
#ifndef LanceNet_NET_TIMER_H
#define LanceNet_NET_TIMER_H

#include "LanceNet/base/Time.h"
#include "LanceNet/base/noncopyable.h"
#include "LanceNet/net/TimerId.h"

#include <atomic>
#include <functional>
namespace LanceNet
{
namespace net
{
//
// A timer that hold a expiration time and a callback to be
// called by TimerQueue at alarms of the timer
//

class Timer : noncopyable
{
public:
    using Callback = std::function<void ()>;
    explicit Timer(TimeStamp expiration, Callback onAlarmCb, bool repeat = false, double intervalSecs = 0);
    ~Timer();

    void OnExpiration();

    // getter
    TimeStamp expiration() const;
    bool isRepeat() const;
    double intervalSecs() const;

    // set the time to expire at a specific time
    void resetExpiration(TimeStamp newExpiration);
    // restart timer, which reset the expiration base on time now
    void restartTimer();

    TimerId getMyTimerId();

    static int64_t getNumTimerCreated();

private:
    TimeStamp timeExpiration_;
    Callback onAlarmCb_;
    // is reapeat ?
    bool repeat_;
    // the interval
    double intervalSecs_;

    static std::atomic_int64_t numTimersCreated;
};

} // namespace net

} // namespace LanceNet


#endif // LanceNet_NET_TIMER_H
