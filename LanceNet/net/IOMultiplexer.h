// Author : Lance @ nwpu
// A IO multiplexing class that monitoring interested IO events
//
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
class FdChannel;

// IO multiplexing class
// This poll should noly be invoked by EventLoop (IO) thread
// It manage pollfds and mapping from fd to FdChannel*
//
class IOMultiplexer
{
public:
    using FdChannelList= std::vector<FdChannel*>;
    using FdMap = std::unordered_map<int, FdChannel*>;

    IOMultiplexer(EventLoop* loop);
    ~IOMultiplexer() = default;

    // Polls the I/O events
    // must be called in EvnetLoop thread
    TimeStamp poll(FdChannelList* activeChannels, int timeout);

    void updateFdChannel(FdChannel* activeChannes);
    void removeFdChannel(FdChannel* sockChannel);

private:
    void assertInEventLoopThread();
    // helper function for IOMultiplexer::poll()
    void fillActiveChannels(int numActiveEvents, FdChannelList* activeChannels);

private:
    using PollFdList = std::vector<struct pollfd>;

    EventLoop* owner_loop_;

    // FdChannelList sockChannelList_;
    PollFdList fdlist_;

    // Mapping from fd to FdChannel*
    FdMap fdMap_;
};

} // net
} // LanceNet

#endif // LanceNet_BASE_POLLER_H
