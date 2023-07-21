
#include <LanceNet/net/EventLoopThread.h>
#include <LanceNet/net/EventLoop.h>
#include <LanceNet/base/Logging.h>

namespace LanceNet
{
namespace net
{

EventLoopThread::EventLoopThread()
  : loopThread_(std::bind(&EventLoopThread::createEventLoopAndLaunch, this)),
    // EventLoopPtr_(std::make_unique<EventLoop>()), // leave it empty is by design
    latch_(1)
{
}

EventLoopThread::~EventLoopThread()
{
    // FIXME: quit or leave it along ?
    EventLoopPtr_->quit();
    loopThread_.join();
}

void EventLoopThread::createEventLoopAndLaunch()
{
    EventLoop* p = new EventLoop();

    LOG_DEBUGC << "-----------EventLoop p =  " << p;
    EventLoopPtr_.reset(p);
    // notify that eventloop is create and ready
    latch_.countdown();

    EventLoopPtr_->StartLoop();
}

std::shared_ptr<EventLoop> EventLoopThread::StartLoop()
{
    // launch a thread that create evnetloop and startloop
    loopThread_.start();

    // wait the eventloop is ready
    latch_.wait();
    assert(EventLoopPtr_);
    return EventLoopPtr_;
}

} // namespace net
} // namespace LanceNet
