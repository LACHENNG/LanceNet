// Author : Lance @ nwpu
#ifndef LanceNet_NET_EVENTLOOP_H
#define LanceNet_NET_EVENTLOOP_H

#include "LanceNet/base/ThisThread.h"
namespace LanceNet
{
namespace net
{
// A thread have at most one EventLoop, making it an IO thread
//
// the interface design makes it clear which member function is thread safe
// which can only be called from IO thread
//
// Multi-instances is one thread is not allow

class EventLoop
{
public:
    EventLoop();
    ~EventLoop() = default;

    // start event loop
    void StartLoop();

    // get EventLoop of current thread (NULL may return if no EventLoop there)
    static EventLoop* getEventLoopOfCurrentThread();

    // assert if is in Loop thread;
    void assertInEventLoopThread();
private:
    pid_t tid_;
};






} // namespace net
} // namespace LanceNet

#endif
