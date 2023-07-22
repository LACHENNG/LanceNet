
// Author : Lance @ nwpu
//
#ifndef LanceNet_BASE_POLLERX_H
#define LanceNet_BASE_POLLERX_H
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
namespace LanceNet
{
namespace net
{

class EventLoop;
class FdChannel;

// IO multiplexing class
//  PollerX class is a optimized version of Poller that base on 
//  performace analysis using Callgrind
//  the unordered_map is repalce with a fixed size c array with the pollerFdLimit parameter of ctor
class PollerX : public IOMultiplexer
{
public:
    const static int kMaxPollFds= 10000;

    PollerX(EventLoop* loop, int pollerFdLimit = kMaxPollFds);
    ~PollerX() = default;

    // Polls the I/O events
    // must be called in EvnetLoop thread
    TimeStamp poll(FdChannelList* activeChannels, int timeout) override;

    void updateFdChannel(FdChannel* activeChannes) override;
    void removeFdChannel(FdChannel* sockChannel) override;
    void disableAllEvent(FdChannel* targetChannel) override;

    int maxFdLimit() const { return m_maxPollerFd; }
private:
    // helper function for Poller::poll()
    void fillActiveChannels(int numActiveEvents, FdChannelList* activeChannels);

private:
    using PollFdList = std::vector<struct pollfd>;

    // FdChannelList sockChannelList_;
    PollFdList fdlist_;

    const int m_maxPollerFd;
    using FdMap = std::vector<FdChannel*>;
    FdMap fdMap_;
};

} // net
} // LanceNet

#endif // LanceNet_BASE_POLLEROPTIMIZED_H
