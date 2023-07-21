#include "LanceNet/net/EventLoopPool.h"
#include "LanceNet/base/Logging.h"
#include "LanceNet/net/EventLoopThread.h"
#include "LanceNet/net/EventLoop.h"

namespace LanceNet
{
namespace net
{

EventLoopPool::EventLoopPool(EventLoop* mainloop)
  : started_(false),
    threadNum_(0),
    next_(0),
    mainLoop_(mainloop)
{

}

EventLoopPool::~EventLoopPool()
{
    // default dtor is ok
}

void EventLoopPool::setThreadNum(int numThreads)
{
    threadNum_ = numThreads;
}

void EventLoopPool::start()
{
    assert(!started_);

    for(int _ = 0; _ < threadNum_; _++)
    {
        threads_.emplace_back(std::make_shared<EventLoopThread>());
        auto loopPtr = threads_.back()->StartLoop();
        loops_.push_back(loopPtr.get());
    }
    started_ = true;
}

EventLoop* EventLoopPool::getNextLoop()
{
    assert(started_);
    if(threadNum_ == 0)
    {
        return mainLoop_;
    }
    // round rabin
    auto nextLoop = loops_[next_];
    next_ = next_ + 1;
    if(next_ >= threadNum_){
        next_ = 0;
    }
    return nextLoop;
}

} // namespace LanceNet
} // namespace net
