#ifndef LanceNet_BASE_POLLER_H
#define LanceNet_BASE_POLLER_H
#include <vector>
#include <unordered_map>
#include <sys/poll.h>

#include "LanceNet/base/Time.h"
// struct pollfd {
//     int   fd;         /* file descriptor */
//     short events;     /* requested events */
//     short revents;    /* returned events */
// };

// FIXME: why forward-declearation is not working when use a unique_ptr<IOMultiplexer>
// struct pollfd;

namespace LanceNet
{
namespace net
{

class EventLoop;
class SockChannel;

// IO multiplexing class
// This poll should noly be invoked by EventLoop (IO) thread
// It manage pollfds and mapping from fd to SockChannel*
//
class IOMultiplexer
{
public:
    using SockChannelList= std::vector<SockChannel*>;
    using FdMap = std::unordered_map<int, SockChannel*>;

    IOMultiplexer(EventLoop* loop);
    ~IOMultiplexer() = default;

    // Polls the I/O events
    // must be called in EvnetLoop thread
    TimeStamp poll(SockChannelList* activeChannels, int timeout);

    void updateSockChannel(SockChannel* activeChannes);


private:
    void assertInEventLoopThread();
    // helper function for IOMultiplexer::poll()
    void fillActiveChannels(int numActiveEvents, SockChannelList* activeChannels);

private:
    using PollFdList = std::vector<struct pollfd>;

    EventLoop* owner_loop_;

    // SockChannelList sockChannelList_;
    PollFdList fdlist_;

    // Mapping from fd to SockChannel*
    FdMap fdMap_;
};

} // net
} // LanceNet

#endif // LanceNet_BASE_POLLER_H
