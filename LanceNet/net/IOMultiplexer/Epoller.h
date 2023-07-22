// Author : Lance @ nwpu
// An epoll IO multiplexing class that monitoring interested IO events
#ifndef LanceNet_BASE_EPOLLER_H
#define LanceNet_BASE_EPOLLER_H

#include <LanceNet/base/noncopyable.h>
#include <LanceNet/net/IOMultiplexer.h>
#include <LanceNet/base/Time.h>

#include <vector>
#include <unordered_map>
#include <sys/epoll.h>

namespace LanceNet
{
namespace net
{

class EventLoop;
class FdChannel;

// An epoll(level trigger, LT mode) IO multiplexing class
class Epoller : public IOMultiplexer
{
public:
    Epoller(EventLoop* loop);
    ~Epoller() = default;

    // Polls the I/O events
    // must be called in EvnetLoop thread
    TimeStamp poll(FdChannelList* activeChannels, int timeout) override;

    void updateFdChannel(FdChannel* activeChannes) override;
    void removeFdChannel(FdChannel* sockChannel) override;
    void disableAllEvent(FdChannel* targetChannel) override;

private:
    using EpollEventList = std::vector<struct epoll_event>;

    void fillActiveChannels(int numActiveEvents, FdChannelList* activeChannels, EpollEventList& available_events);

private:
    static const int kDefaultEventListSize = 8;

    int epfd_;
    // used by ::epoll_wait etc.
    EpollEventList epoll_events_;

    using FdMap = std::unordered_map<int, FdChannel*>;
    FdMap fdMap_;
};

} // net
} // LanceNet

#endif // LanceNet_BASE_EPOLLER_H
