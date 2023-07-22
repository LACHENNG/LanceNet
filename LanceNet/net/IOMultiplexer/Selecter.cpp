#include "LanceNet/base/Time.h"
#include "LanceNet/net/EventLoop.h"
#include <LanceNet/net/IOMultiplexer/Selecter.h>
#include <LanceNet/base/Logging.h>
#include <LanceNet/net/FdChannel.h>
#include <LanceNet/base/unix_wrappers.h>
#include <strings.h>
#include <assert.h>
#include <sys/select.h>
namespace
{
    const int kNew= -1;
    const int kAdded = 1;
    const int kDeleted= 2;
}
namespace LanceNet
{
namespace net
{
Selecter::Selecter(EventLoop* loop)
  : IOMultiplexer(loop),
    maxfdsofar_(0)
{
    FD_ZERO(&readset_);
    FD_ZERO(&writeset_);
    FD_ZERO(&readsetCopy_);
    FD_ZERO(&writesetCopy_);
    LOG_WARNC << "Your are using select Poller which can monitor only file descriptors numbers that are less than `FD_SETSIZE`, we suggest that you had better not use it";
}

TimeStamp Selecter::poll(FdChannelList* activeChannels, int timeoutMs)
{
    assertInEventLoopThread();
    struct timeval tv = getTimeVal(timeoutMs);
    struct timeval* timeoutp = timeoutMs == -1 ? NULL : &tv;
    readsetCopy_ = readset_;
    writesetCopy_ = writeset_;
    //  nfds  should  be  set  to  the highest-numbered file descriptor in any of the three sets, plus 1.
    int nready = Select(maxfdsofar_ + 1, &readsetCopy_, &writesetCopy_, nullptr, timeoutp);
    TimeStamp now = TimeStamp::now();
    fillActiveChannels(nready, activeChannels);
    return now;
}

void Selecter::updateFdChannel(FdChannel* channel)
{
    assertInEventLoopThread();
    int fd = channel->fd();
    bool allreadyExists = (fdMap_.find(fd) != fdMap_.end());
    int status = channel->index();
    if(allreadyExists){
        assert(status == kAdded || status == kDeleted);
        assert(fdMap_[fd] == channel);
    }else{
        assert(status == kNew);
        fdMap_[fd] = channel;
    }
    if(channel->isReadEnabled()){
        FD_SET(fd, &readset_);
    }
    if(channel->isWriteEnabled()){
        FD_SET(fd, &writeset_);
    }
    maxfdsofar_ = std::max(maxfdsofar_, fd);
    channel->index(kAdded);
}

void Selecter::removeFdChannel(FdChannel* channel)
{
    assertInEventLoopThread();
    assert(channel->isNoneEvent());
    int status = channel->index();
    assert(status == kAdded || status == kDeleted);
    int fd = channel->fd();
    assert(fdMap_.find(fd) != fdMap_.end());
    assert(fdMap_[fd] == channel);
    int n = fdMap_.erase(fd);
    assert(n == 1); (void)n;

    if(status == kAdded){
        if(channel->isReadEnabled()) FD_CLR(fd, &readset_);
        if(channel->isWriteEnabled()) FD_CLR(fd, &writeset_);
    }

    channel->index(kNew);
}

void Selecter::disableAllEvent(FdChannel* channel)
{
    assertInEventLoopThread();
    int fd = channel->fd();
    bool allreadyExists = (fdMap_.find(fd) != fdMap_.end());
    assert(allreadyExists);
    (void)allreadyExists;
    assert(fdMap_[fd] == channel);

    FD_CLR(fd, &readset_);
    FD_CLR(fd, &writeset_);
    // use to set status in this context
    channel->index(kDeleted);
}

void Selecter::fillActiveChannels(int numActiveEvents, FdChannelList* activeChannels)
{
    for(int i = 3; i <= maxfdsofar_ && numActiveEvents > 0; i++){
        bool readable = FD_ISSET(i, &readsetCopy_);
        bool writeable = FD_ISSET(i, &writesetCopy_);
        if(!readable && !writeable) 
            continue; 
        FdChannel* channel = nullptr;
        if(readable || writeable){
            assert(fdMap_.find(i) != fdMap_.end());
            channel = fdMap_[i];
            numActiveEvents--;
        }
        if(readable) channel->revents(FdChannel::kReadEvent);
        if(writeable) channel->revents(FdChannel::kWriteEvent);

        activeChannels->push_back(channel);

    }
}
struct timeval Selecter::getTimeVal(int timeoutMs)
{
    static const int kMsPerSec = 1e3;
    struct timeval tv;
    bzero(&tv, sizeof(tv));
    if(timeoutMs < 0){
        return tv;
    }
    tv.tv_sec = timeoutMs / kMsPerSec;
    tv.tv_usec = timeoutMs % kMsPerSec;
    return tv;
}

} // net
} // LanceNet

