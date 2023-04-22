// Author : Lance @ nwpu
#ifndef LanceNet_NET_EVENTLOOP_H
#define LanceNet_NET_EVENTLOOP_H

#include "LanceNet/base/ThisThread.h"
#include "LanceNet/base/noncopyable.h"
#include "LanceNet/net/IOMultiplexer.h"
#include <atomic>
#include <unordered_map>
#include <vector>
#include <memory>

namespace LanceNet
{
namespace net
{

class EventLoop;
class SockChannel;
// class IOMultiplexer;

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

    EventLoop();
    ~EventLoop() = default;

    // start event loop
    void StartLoop();

    // get EventLoop of current thread (NULL may return if no EventLoop there)
    static selfPtr getEventLoopOfThisThread();
    static void assertCanCreateNewLoop();

    // assert if is in Loop thread;
    void assertInEventLoopThread();

    void update(SockChannel* sockChannel);

    void quit();

private:
    // used thread local to simplify the implement
    // static std::unordered_map<pid_t, selfPtr>& getTidToEventLoopMap();

    // the thread id of this EventLoop
    pid_t tid_;
    std::atomic_bool running_;

    // active SockChannel that has events to be handled
    std::vector<SockChannel*> activeSockChannels_;

    // Poller to get active Sockchannels that have events to handle with;
    // IOMultiplexer* multiplexer_;
    std::unique_ptr<IOMultiplexer> multiplexer_;
};

} // namespace net
} // namespace LanceNet

#endif
