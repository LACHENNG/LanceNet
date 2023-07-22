// Author : Lance @ nwpu
// A IO multiplexing class that monitoring interested IO events
//
#ifndef LanceNet_BASE_SELECTER_H
#define LanceNet_BASE_SELECTER_H
#include <vector>
#include <unordered_map>
#include <sys/poll.h>

#include "LanceNet/base/Time.h"
#include "LanceNet/net/IOMultiplexer.h"

namespace LanceNet
{
namespace net
{

class EventLoop;
class FdChannel;

// IO multiplexing class using ::select
//
class Selecter : public IOMultiplexer
{
public:
    Selecter(EventLoop* loop);
    ~Selecter() = default;

    // Polls the I/O events
    // must be called in EvnetLoop thread
    TimeStamp poll(FdChannelList* activeChannels, int timeoutMs) override;

    void updateFdChannel(FdChannel* activeChannes) override;
    void removeFdChannel(FdChannel* sockChannel) override;
    void disableAllEvent(FdChannel* targetChannel) override;

private:
    // helper function for Selecter::poll()
    void fillActiveChannels(int numActiveEvents, FdChannelList* activeChannels);

    struct timeval getTimeVal(int timeoutMs);
private:
    using FdMap = std::unordered_map<int, FdChannel*>;
    FdMap fdMap_;

    int maxfdsofar_;
    fd_set readset_;
    fd_set writeset_;
    fd_set readsetCopy_;
    fd_set writesetCopy_;
};

} // net
} // LanceNet

#endif
