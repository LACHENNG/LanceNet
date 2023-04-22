#include "LanceNet/base/ThisThread.h"
#include "LanceNet/net/SockChannel.h"
#include <LanceNet/net/EventLoop.h>
#include <LanceNet/net/IOMultiplexer.h>
#include <LanceNet/base/Logging.h>
#include <assert.h>
#include <cstdlib>

namespace LanceNet
{
namespace net
{

__thread EventLoop* tl_eventLoopPtrOfThisThread;

//definition of static var
// std::unordered_map<pid_t, EventLoop*> EventLoop::s_umap;

EventLoop::EventLoop()
  : tid_(ThisThread::Gettid()),
    running_(false),
    multiplexer_(new IOMultiplexer(this))
{
    // only one instances is allowed in IO thread(EventLoop thread)
    assertCanCreateNewLoop();
    tl_eventLoopPtrOfThisThread = this;
}

void EventLoop::StartLoop(){
    // StartLoop can only be called at EventLoop thread
    assertInEventLoopThread();
    assert(!running_);
    running_ = true;

    while(running_){
        activeSockChannels_.clear();

        // a negative timeout means infinite timeout
        int timeout = -1;
        multiplexer_->poll(&activeSockChannels_, timeout);

        for(auto SockChannelPtr : activeSockChannels_){
            SockChannelPtr->handleEvents();
        }
    }
}

void EventLoop::assertInEventLoopThread()
{
    if(tid_ != ThisThread::Gettid()){
        LOG_FATALC << "assertInEventLoopThread failed, EventLoop is created at thread with id : " << tid_ << " but invoke at another thread with id : " << ThisThread::Gettid();
        exit(EXIT_FAILURE);
    }
}

void EventLoop::assertCanCreateNewLoop()
{
    if(tl_eventLoopPtrOfThisThread != NULL)
    {
        LOG_FATALC << "assertCanCreateNewLoop failed,there is allreay one EvnetLoop at thread : " << ThisThread::Gettid();
        exit(EXIT_FAILURE);
    }
}

EventLoop* EventLoop::getEventLoopOfThisThread()
{
    static std::unordered_map<pid_t, EventLoop*> s_umap;

    return s_umap[ThisThread::Gettid()];
}

void EventLoop::update(SockChannel* sockChannel)
{
    multiplexer_->updateSockChannel(sockChannel);
}

void EventLoop::quit()
{
    running_ = false;
}

// I use thread local to optimize usage
// std::unordered_map<pid_t, EventLoop*>& EventLoop::getTidToEventLoopMap()
// {
//     static std::unordered_map<pid_t, EventLoop*> s_umap;
//     return s_umap;
//
// }


} // namespace net
} // namespace LanceNet

