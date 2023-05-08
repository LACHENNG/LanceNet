#include "LanceNet/base/Mutex.h"
#include "LanceNet/base/Time.h"
#include "LanceNet/base/unix_wrappers.h"
#include "LanceNet/base/ThisThread.h"
#include <LanceNet/net/TimerQueue.h>
#include "LanceNet/net/Timer.h"
#include "LanceNet/net/FdChannel.h"
#include <LanceNet/net/EventLoop.h>
#include <LanceNet/net/IOMultiplexer.h>
#include <LanceNet/base/Logging.h>

#include <assert.h>
#include <cstdint>
#include <cstdlib>
#include <sys/eventfd.h>

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
    multiplexer_(std::make_unique<IOMultiplexer>(this)),
    runInLoopImpl_(std::make_unique<RunInLoopImpl>(this)),
    timerQueueUptr_(std::make_unique<TimerQueue>(this))
{
    // only one instances is allowed in IO thread(EventLoop thread)
    assertCanCreateNewLoop();
    tl_eventLoopPtrOfThisThread = this;
}

EventLoop::~EventLoop()
{
}

void EventLoop::StartLoop(){

    // StartLoop can only be called at EventLoop thread
    assertInEventLoopThread();
    assert(!running_);
    running_ = true;

    //LOG_INFOC << "Eventloop::StartLoop at thread : " << ThisThread::Gettid();

    while(running_){
        activeFdChannels_.clear();

        // a negative timeout means infinite timeout
        int timeout = -1;
        multiplexer_->poll(&activeFdChannels_, timeout);

        for(auto FdChannelPtr : activeFdChannels_){
            FdChannelPtr->handleEvents();
        }
    }
}

void EventLoop::assertInEventLoopThread()
{
    if(tid_ != ThisThread::Gettid()){
        LOG_FATALC << "assertInEventLoopThread failed, EventLoop is created at thread with id : " << tid_ << " but this assertion is invoked at thread with id : " << ThisThread::Gettid();
        exit(EXIT_FAILURE);
    }
}

bool EventLoop::isInEventLoopThread()
{
    return tid_ == ThisThread::Gettid();
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
    //static std::unordered_map<pid_t, EventLoop*> s_umap;

    //return s_umap[ThisThread::Gettid()];
    return tl_eventLoopPtrOfThisThread;
}

void EventLoop::update(FdChannel* sockChannel)
{
    multiplexer_->updateFdChannel(sockChannel);
}

void EventLoop::remove(FdChannel* sockChannel)
{
    multiplexer_->removeFdChannel(sockChannel);
}

void EventLoop::quit()
{
    running_ = false;
}

void EventLoop::runInLoop(PendFunction functor)
{
    // If the current thread is EventLoop thread, call the function synchronously, otherwise pend
    if(isInEventLoopThread()){
        functor();
    }
    else{
        //if(!running_) LOG_WARNC << "the EventLoop is not started yet";
        runInLoopImpl_->pend(functor);
    }
}

TimerId EventLoop::runAt(TimeStamp when, Callback what, double delaySecs)
{
    //auto addtimer = [=](){
    //    auto timer = std::make_unique<Timer>(when + delaySecs, what);
    //    timerQueueUptr_->addTimer(std::move(timer));
    //};
    //runInLoop(addtimer);
    auto newWhen = when + delaySecs;
    auto timer = std::make_shared<Timer>(newWhen + delaySecs, what);
    auto timerid = timerQueueUptr_->addTimer(timer);
    return timerid;
}

TimerId EventLoop::runEvery(TimeStamp start, double interval, Callback whatFunc)
{
    auto timer = std::make_shared<Timer>(start , whatFunc, true, interval);
    auto timerid = timerQueueUptr_->addTimer(timer);
    return timerid;
}

EventLoop::RunInLoopImpl::RunInLoopImpl(EventLoop* owner_loop)
  : eventfd_(makeEventfd()),
    eventfdChannelUPtr_(std::make_unique<FdChannel>(owner_loop, eventfd_)),
    ower_loop_(owner_loop)
{
    eventfdChannelUPtr_->setReadCallback(std::bind(&EventLoop::RunInLoopImpl::doPendingFunctors, this));
    eventfdChannelUPtr_->enableReading();
}

EventLoop::RunInLoopImpl::~RunInLoopImpl()
{
    // the eventfd is auto closed by dtor of FdChannel
    // the fdChannel is auto removed from owner_loop_;
}

void EventLoop::RunInLoopImpl::doPendingFunctors()
{
    ower_loop_->assertInEventLoopThread();

    // swap to locals can reduce the critical zone and avoid dead lock as the pending functions may call runInLoop() too.
    std::vector<PendFunction> functions;
    {
        MutexLockGuard lock(mutex_);
        functions.swap(pendingFuncs_);
    }

    for(auto& pendingfunc : functions)
    {
        pendingfunc();
    }
    readEventfdOnce();
}

void EventLoop::RunInLoopImpl::pend(EventLoop::PendFunction func)
{
    MutexLockGuard lock(mutex_);
    pendingFuncs_.push_back(func);
    writeEventfd();
}

void EventLoop::RunInLoopImpl::writeEventfd()
{
    // wirte to eventfd so that it will become readable
    //LOG_INFOC << "RunInLoopImpl::writeEventfd()";
    uint64_t __ = 1;
    Write(eventfd_, &__, sizeof(uint64_t));
}

void EventLoop::RunInLoopImpl::readEventfdOnce()
{
    //LOG_INFOC << "RunInLoopImpl::clearnotify()";
    uint64_t __;
    int n = Read(eventfd_, &__, sizeof(uint64_t));
    assert(sizeof(uint64_t) == n);
}

int EventLoop::RunInLoopImpl::makeEventfd()
{
    int fd = eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK);
    return fd;
}

} // namespace net
} // namespace LanceNet
