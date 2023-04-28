// Author : Lance @ nwpu
//
#ifndef LanceNet_NET_TIMERID_H
#define LanceNet_NET_TIMERID_H

#include "LanceNet/base/Time.h"
#include "LanceNet/base/copyable.h"
#include <functional>

namespace LanceNet
{
namespace net
{
class Timer;

// Identifier of a timer, make the cancling easier
//
class TimerId : public copyable
{
public:
    TimerId();

    TimerId(Timer* timer, int64_t id);

    friend class TimerQueue;

private:
    Timer* timer_;
    int64_t id_;
};



} // namespace net
} // namespace LanceNet

#endif // LanceNet_NET_TIMERID_H
