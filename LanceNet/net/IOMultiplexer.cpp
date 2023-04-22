#include <sys/poll.h>
#include <assert.h>
#include <cstdlib>

#include <LanceNet/net/SockChannel.h>
#include <LanceNet/net/IOMultiplexer.h>
#include <LanceNet/base/Time.h>
#include <LanceNet/base/Logging.h>
#include <LanceNet/net/EventLoop.h>

namespace LanceNet
{
namespace net
{

IOMultiplexer::IOMultiplexer(EventLoop* loop)
  :  owner_loop_(loop)
{
}


TimeStamp IOMultiplexer::poll(SockChannelList* activeChannels, int timeout)
{
    assertInEventLoopThread();
    // int poll(struct pollfd *fds, nfds_t nfds, int timeout);

    int numEvents = ::poll(fdlist_.data(), fdlist_.size(), timeout);
    TimeStamp ts(TimeStamp::now());

    if(numEvents > 0)
    {
        LOG_INFOC << numEvents << " events happended";
        fillActiveChannels(numEvents, activeChannels);
    }
    else if(numEvents == 0)
    {
        LOG_INFOC << " no events happend";
    }
    else
    {
        LOG_FATALC << " system call error at IOMultiplexer::poll()";
    }

    return ts;
}

// using the new SockChannel which contains user interested events and update to corresponding pollfd.events

// update the internal events from @para channel
// insert a new or update an existing channel takes O(1) time
// also, update a existing one take O(1) time  as @para channel remembered the index in pollfds_;
//
void IOMultiplexer::updateSockChannel(SockChannel* channel)
{
    // FIXME: is this right ?
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
        fdlist_[index].events = channel->events();
    }
}

void IOMultiplexer::assertInEventLoopThread()
{
    owner_loop_->assertInEventLoopThread();
}

// find all active events and push its corresponding SockChannel to @parameter activeChannels
// current active revents will be stored into SockChannel
void IOMultiplexer::fillActiveChannels(int numEvents, SockChannelList* activeChannels)
{
    for(int i = 0; i < static_cast<int>(fdlist_.size()) && numEvents > 0; i++)
    {
        auto pollfd = fdlist_[i];
        if(pollfd.revents > 0)
        {
            numEvents--;

            int fd = pollfd.fd;
            auto iter = fdMap_.find(fd);

            if(iter != fdMap_.end())
            {
                assert(fdMap_[fd]->fd() == fd);
                // set revents of SockChannel so that the event can be handled
                fdMap_[fd]->revents(pollfd.revents);
                activeChannels->push_back(fdMap_[fd]);
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


