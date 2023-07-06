// Author : Lance @ nwpu
// A IO multiplexing class that monitoring interested IO events
//
#ifndef LanceNet_BASE_POLLER_H
#define LanceNet_BASE_POLLER_H
#include <vector>
#include <unordered_map>
#include <sys/poll.h>

#include "LanceNet/base/Time.h"
#include "LanceNet/net/IOMultiplexer.h"
// struct pollfd {
//     int   fd;         /* file descriptor */
//     short events;     /* requested events */
//     short revents;    /* returned events */
// };

// FIXME: why forward-declearation is not working when use a unique_ptr<Poller>
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
class Poller : public IOMultiplexer
{
public:
    Poller(EventLoop* loop);
    ~Poller() = default;

    // Polls the I/O events
    // must be called in EvnetLoop thread
    TimeStamp poll(FdChannelList* activeChannels, int timeout) override;

    void updateFdChannel(FdChannel* activeChannes) override;
    void removeFdChannel(FdChannel* sockChannel) override;
    void disableAllEvent(FdChannel* targetChannel) override;

private:
    // helper function for Poller::poll()
    void fillActiveChannels(int numActiveEvents, FdChannelList* activeChannels);

private:
    using PollFdList = std::vector<struct pollfd>;

    // FdChannelList sockChannelList_;
    PollFdList fdlist_;
};

} // net
} // LanceNet

#endif // LanceNet_BASE_POLLER_H
