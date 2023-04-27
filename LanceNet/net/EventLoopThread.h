// Author : Lance @ nwpu
//
#ifndef LanceNet_EVENT_LOOP_THREAD_H
#define LanceNet_EVENT_LOOP_THREAD_H
#include "LanceNet/base/CountDownLatch.h"
#include "LanceNet/base/Thread.h"
#include <memory>
namespace LanceNet
{
namespace net
{

class EventLoop;
class FdChannel;

// This class acts like a EventLoop class, but launched in
// another thread
class EventLoopThread
{
public:

    EventLoopThread();
    ~EventLoopThread();

    // start event loop, and return if the eventloop is really and started
    std::shared_ptr<EventLoop> StartLoop();

private:
    void createEventLoopAndLaunch();

    base::Thread loopThread_;
    // use shared_ptr as the client use EventLoop returned by `StartLoop()` too.
    std::shared_ptr<EventLoop> EventLoopPtr_;
    CountDownLatch latch_;
};



} // namespace net
} // namespace LanceNet





#endif // LanceNet_EVENT_LOOP_THREAD_H

