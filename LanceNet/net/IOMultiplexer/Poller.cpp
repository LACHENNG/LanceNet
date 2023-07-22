#include <string>
#include <sys/poll.h>
#include <assert.h>
#include <cstdlib>

#include <LanceNet/net/FdChannel.h>
#include <LanceNet/net/IOMultiplexer/Poller.h>
#include <LanceNet/base/Time.h>
#include <LanceNet/base/Logging.h>
#include <LanceNet/net/EventLoop.h>

namespace LanceNet
{
namespace net
{

Poller::Poller(EventLoop* loop)
  :  IOMultiplexer(loop)
{
    LOG_INFO << "Using IOMultiplexer: poll";
}


TimeStamp Poller::poll(FdChannelList* activeChannels, int timeout)
{
    assertInEventLoopThread();
    // int poll(struct pollfd *fds, nfds_t nfds, int timeout);

    int numEvents = ::poll(fdlist_.data(), fdlist_.size(), timeout);
    TimeStamp ts(TimeStamp::now());

    if(numEvents > 0)
    {
        fillActiveChannels(numEvents, activeChannels);
    }
    else if(numEvents == 0)
    {
        LOG_INFOC << " no events happend";
    }
    else
    {
        LOG_FATALC << " system call error at Poller::poll()";
    }

    return ts;
}

// using the new FdChannel which contains user interested events and update to corresponding pollfd.events

// update the internal events from @para channel
// insert a new or update an existing channel takes O(1) time
// also, update a existing one take O(1) time  as @para channel remembered the index in pollfds_;
//
void Poller::updateFdChannel(FdChannel* channel)
{
    assertInEventLoopThread();

    // pollfd
    int fd = channel->fd();
    auto iter = fdMap_.find(fd);
    // insert a new one
    if(iter == fdMap_.end())
    {
        int index = fdlist_.size();
        channel->index(index);

        fdMap_[fd] = channel;
        fdlist_.push_back(pollfd{fd, channel->events(), 0});

    }
    else{
        // update an existing one
        int index = channel->index();
        assert(index >= 0);
        assert(index < static_cast<int>(fdlist_.size()));
        assert(fdlist_[index].fd == channel->fd());
        fdlist_[index].events = channel->events();
    }
}

void Poller::removeFdChannel(FdChannel *sockChannel)
{
    assertInEventLoopThread();
    int fd = sockChannel->fd();
    auto iter = fdMap_.find(fd);
    assert(iter != fdMap_.end());
    assert(fdMap_[fd] == sockChannel);

    int index = sockChannel->index();
    assert(index >= 0 && index < static_cast<int>(fdlist_.size()));

    assert(fdlist_[index].fd == -sockChannel->fd()-1);
    // delte from fdlist_ in O(1) time
    int backFd = fdlist_.back().fd;
    if(backFd < 0)
    { 
        backFd = -backFd - 1;
    }
    std::swap(fdlist_[index], fdlist_.back());
    fdMap_[backFd]->index(index);

    fdlist_.pop_back();
    fdMap_.erase(iter);
}

void Poller::disableAllEvent(FdChannel *targetChannel)
{
    int index = targetChannel->index();
    assert(targetChannel->isNoneEvent());
    assert(index >= 0 && index < static_cast<int>(fdlist_.size()));
    assert(fdlist_[index].fd != -targetChannel->fd() - 1);
    fdlist_[index].fd = -targetChannel->fd() - 1;
    assert(fdlist_[index].fd < 0);
}



// find all active events and push its corresponding FdChannel to @parameter activeChannels
// current active revents will be stored into FdChannel
void Poller::fillActiveChannels(int numEvents, FdChannelList* activeChannels)
{
    for(int i = 0; i < static_cast<int>(fdlist_.size()) && numEvents > 0; i++)
    {
        auto& pollfd = fdlist_[i];
        if(pollfd.revents > 0)
        {
            numEvents--;

            int fd = pollfd.fd;
            auto iter = fdMap_.find(fd);
            FdChannel* channel = iter->second;
            if(iter != fdMap_.end())
            {
                assert(channel->fd() == fd);
                // set revents of FdChannel so that the event can be handled
                channel->revents(pollfd.revents);
                activeChannels->push_back(channel); // faster than emplace back in this case base on performace analysis
            }
            else
            {
                LOG_FATAL << "fdMap_.find(fd) == end()";
                exit(EXIT_FAILURE);
            }
        }
    }
}


} // namespace net
} // namespace LanceNet


