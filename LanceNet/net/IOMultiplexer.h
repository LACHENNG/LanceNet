// Author : Lance @ nwpu
// A IO multiplexing class that monitoring interested IO events
//
#ifndef LanceNet_BASE_IOMULTIPLEXER_H
#define LanceNet_BASE_IOMULTIPLEXER_H
#include <vector>
#include <unordered_map>
#include <sys/poll.h>
#include <LanceNet/base/noncopyable.h>
#include "LanceNet/base/Time.h"

namespace LanceNet
{
namespace net
{

class EventLoop;
class FdChannel;
class EventLoop;
// IO multiplexing base class
// This poll should noly be invoked by EventLoop (IO) thread
// It manage pollfds and mapping from fd to FdChannel*
//
class IOMultiplexer : noncopyable
{
public:
    using FdChannelList= std::vector<FdChannel*>;

    IOMultiplexer(EventLoop* loop);
    virtual ~IOMultiplexer();

    // Polls the I/O events
    // must be called in EvnetLoop thread
    virtual TimeStamp poll(FdChannelList* activeChannels, int timeout) = 0;

    virtual void updateFdChannel(FdChannel* activeChannes) = 0;
    virtual void removeFdChannel(FdChannel* sockChannel) = 0;
    virtual void disableAllEvent(FdChannel* targetChannel) = 0;

    void assertInEventLoopThread();

    static IOMultiplexer* getDefaultIOMultiplexer(EventLoop* loop);
private:
    EventLoop* owner_loop_;

};

} // net
} // LanceNet

#endif // LanceNet_BASE_IOMULTIPLEXER_H
