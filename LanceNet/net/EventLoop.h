// Author : Lance @ nwpu
#ifndef LanceNet_NET_EVENTLOOP_H
#define LanceNet_NET_EVENTLOOP_H

#include "LanceNet/base/ThisThread.h"
#include "LanceNet/base/Time.h"
#include "LanceNet/base/noncopyable.h"
#include "LanceNet/base/Mutex.h"
#include "LanceNet/net/IOMultiplexer.h"
#include "LanceNet/net/TimerId.h"

#include <atomic>
#include <unordered_map>
#include <functional>
#include <vector>
#include <memory>

namespace LanceNet
{
namespace net
{

class EventLoop;
class FdChannel;
class TimerQueue;
// class IOMultiplexer;
//

extern __thread EventLoop* tl_eventLoopPtrOfThisThread;
// Multi-instances is one thread is not allow

// EventLoop is the core of a Reactor
// The IO events get by IO multiplexing are dispatched to event handles of each file descriptor(fd)
//
// A thread have at most one EventLoop, making it an IO thread
//
// the interface design makes it clear which member function is thread safe
// which can only be called from IO thread
//
class EventLoop : noncopyable
{
public:
    using self = EventLoop;
    using selfPtr = EventLoop*;
    using Callback = std::function<void()>;
    using PendFunction = Callback;

    EventLoop();
    ~EventLoop();

    // start event loop
    void StartLoop();

    // get EventLoop of current thread (NULL may return if no EventLoop there)
    static selfPtr getEventLoopOfThisThread();
    static void assertCanCreateNewLoop();

    // assert if is in Loop thread;
    void assertInEventLoopThread();
    bool isInEventLoopThread();

    void update(FdChannel* fdChannel);
    void remove(FdChannel* fdChannel);
    void disableAllEvent(FdChannel* fdChannel);

    void quit();

    // Transfers function execution from another thread to the current eventloop thread
    void runInLoop(PendFunction pendingfunc); //sematic
    void pendInLoop(PendFunction pendingfunc); //sematic

    // A more useful wrapper to use TimerQueue
    TimerId runAt(TimeStamp when , Callback whatFunc, double delaySecs = 0);
    TimerId runEvery(TimeStamp start, double interval, Callback whatFunc);

private:
    // Transfer exection from other to EventLoop by eventfd + callback + pendingFuncions queue mechanism
    class RunInLoopImpl
    {
    public:
        explicit RunInLoopImpl(EventLoop* owner_loop);
        ~RunInLoopImpl();

        void pend(PendFunction func);

    private:
        // can only called in EventLoop thread
        void doPendingFunctors();

        // read and write eventfd
        void writeEventfd();
        void readEventfdOnce();

        int makeEventfd();

        // eventfd for notifiying
        int eventfd_;
        std::unique_ptr<FdChannel> eventfdChannelUPtr_;

        EventLoop* ower_loop_;
        // pending functions
        std::vector<PendFunction> pendingFuncs_; // Mutex Guarded
        MutexLock mutex_;
    };

    // the thread id of this EventLoop
    pid_t tid_;
    std::atomic_bool running_;

    // active FdChannel that has events to be handled
    std::vector<FdChannel*> activeFdChannels_;

    // Poller to get active Sockchannels that have events to handle with;
    // IOMultiplexer* multiplexer_;
    std::unique_ptr<IOMultiplexer> multiplexer_;

    std::unique_ptr<RunInLoopImpl> runInLoopImpl_;

    std::unique_ptr<TimerQueue> timerQueueUptr_;
};

} // namespace net
} // namespace LanceNet

#endif
