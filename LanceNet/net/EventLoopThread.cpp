
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
    EventLoopPtr_->quit();
    loopThread_.join();
}

void EventLoopThread::createEventLoopAndLaunch()
{
    LOG_INFOC << "create new eventloop and launched at thread: " << ThisThread::Gettid();
    EventLoopPtr_.reset(new EventLoop());
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
