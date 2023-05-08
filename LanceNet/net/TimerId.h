// Author : Lance @ nwpu
//
#ifndef LanceNet_NET_TIMERID_H
#define LanceNet_NET_TIMERID_H

#include "LanceNet/base/Time.h"
#include "LanceNet/base/copyable.h"
#include "LanceNet/net/TimerQueue.h"
#include <functional>
#include <memory>
#include <atomic>

namespace LanceNet
{
namespace net
{
class Timer;

// Identifier of a timer, make the cancling easier
// a TimerId contains Timer* and its serialnum which is used to indicate the number of the class object that is created
// Dont`t warry about the raw pointer Timer*, the life time of Timer is usually manage by the user visible TimerQueue class.
class TimerId : public copyable
{
public:

    TimerId();

    TimerId(Timer* timer, int64_t id);
    int64_t id() const;

    friend class TimerQueue;

    static int64_t getAndIncrementId();

private:
    Timer* timer_;
    int64_t id_;
};

inline bool operator<(const TimerId lhs, const TimerId rhs)
{
    return lhs.id() < rhs.id();
}

} // namespace net
} // namespace LanceNet

#endif // LanceNet_NET_TIMERID_H
