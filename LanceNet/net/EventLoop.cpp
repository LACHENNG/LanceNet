#include <LanceNet/net/EventLoop.h>
#include <LanceNet/base/Logging.h>

namespace LanceNet
{
namespace net
{

EventLoop::EventLoop()
{
    tid_ = ThisThread::Gettid();
}

void EventLoop::StartLoop(){
    // StartLoop can only be called at EventLoop thread
    assertInEventLoopThread();

    sleep(5);
}

void EventLoop::assertInEventLoopThread()
{
    if(tid_ != ThisThread::Gettid()){
        LOG_FATALC << "assertInEventLoopThread failed, calling thread != EventLoop thread";
    }
}


} // namespace net

} // namespace LanceNet



