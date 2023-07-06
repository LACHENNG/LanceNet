#include <LanceNet/net/IOMultiplexer/Epoller.h>
#include <LanceNet/base/Time.h>
#include <LanceNet/base/Logging.h>
#include <LanceNet/net/EventLoop.h>
#include <LanceNet/base/unix_wrappers.h>
#include <LanceNet/net/FdChannel.h>
#include <strings.h>
#include <sys/epoll.h>
#include <sys/poll.h>
#include <assert.h>

// On Linux, the constants of poll(2) and epoll(4)
// are expected to be the same.
static_assert(EPOLLIN == POLLIN,        "epoll uses same flag values as poll");
static_assert(EPOLLPRI == POLLPRI,      "epoll uses same flag values as poll");
static_assert(EPOLLOUT == POLLOUT,      "epoll uses same flag values as poll");
static_assert(EPOLLRDHUP == POLLRDHUP,  "epoll uses same flag values as poll");
static_assert(EPOLLERR == POLLERR,      "epoll uses same flag values as poll");
static_assert(EPOLLHUP == POLLHUP,      "epoll uses same flag values as poll");

namespace
{
const int knew = -1;
const int kadded = 1;
const int kDeleted = 2;
}

namespace LanceNet
{
namespace net
{

Epoller::Epoller(EventLoop* loop)
  : IOMultiplexer(loop),
    epfd_(Epoll_create1(EPOLL_CLOEXEC)),
    epoll_events_(kDefaultEventListSize)
{
    LOG_INFO << "IOMultiplexing Method : Epoller";
}


TimeStamp Epoller::poll(FdChannelList* activeChannels, int timeout)
{
    assertInEventLoopThread();

    EpollEventList available_events(epoll_events_.size());
    int nready = ::Epoll_wait(epfd_, available_events.data(), available_events.size(), timeout);

    int cur_size = epoll_events_.size();
    if(nready >= cur_size){
        epoll_events_.resize(2 * cur_size);
    }

    TimeStamp ts(TimeStamp::now());

    if(nready > 0)
    {
        fillActiveChannels(nready, activeChannels, available_events);
    }
    else if(nready == 0)
    {
        LOG_INFOC << " no events happend";
    }
    else
    {
        LOG_FATALC << " system call error at Epoller::poll()";
    }

    return ts;
}

void Epoller::updateFdChannel(FdChannel* channel)
{
    assertInEventLoopThread();
    // pollfd
    int fd = channel->fd();
    bool alreadyExists= (fdMap_.find(fd) != fdMap_.end());

    struct epoll_event event;
    bzero(&event, sizeof(event));
    event.events = channel->events();
    event.data.fd = channel->fd();

    fdMap_[fd] = channel;
    if(alreadyExists)
    {
        epoll_ctl(epfd_, EPOLL_CTL_MOD, fd, &event);
    }
    else{
        epoll_ctl(epfd_, EPOLL_CTL_ADD, fd, &event);
        channel->index(kadded);
    }
}

void Epoller::removeFdChannel(FdChannel *sockChannel)
{
    assertInEventLoopThread();
    assert(sockChannel->isNoneEvent());
    int status = sockChannel->index();
    assert(status == kadded || status == kDeleted);
    int fd = sockChannel->fd();
    assert(fdMap_.find(fd) != fdMap_.end());
    assert(fdMap_[fd] == sockChannel);
    size_t n = fdMap_.erase(fd);
    (void)n;
    assert(n == 1);

    if(sockChannel->index() == kadded){
        epoll_ctl(epfd_, EPOLL_CTL_DEL, fd, NULL);
    }
    sockChannel->index(knew);
}

void Epoller::disableAllEvent(FdChannel *targetChannel)
{
    assert(targetChannel->isNoneEvent());
    assert(targetChannel->index() == kadded);

    epoll_ctl(epfd_, EPOLL_CTL_DEL, targetChannel->fd(), NULL);
    targetChannel->index(kDeleted);
}

// find all active events and push its corresponding FdChannel to @parameter activeChannels
// current active revents will be stored into FdChannel
void Epoller::fillActiveChannels(int numEvents, FdChannelList* activeChannels, EpollEventList& available_events)
{
    assert(static_cast<size_t>(numEvents) <= epoll_events_.size());
    for(int i = 0; i < numEvents; i++)
    {
        auto &pollfd = available_events[i];
        int fd = pollfd.data.fd;
        auto iter = fdMap_.find(fd);

        if(iter != fdMap_.end())
        {
            assert(fdMap_[fd]->fd() == fd);
            fdMap_[fd]->revents(pollfd.events);
            activeChannels->push_back(fdMap_[fd]);
        }
        else
        {
            LOG_FATAL << "fdMap_.find(fd) == end()";
            exit(EXIT_FAILURE);
        }
    }
}


} // namespace net
} // namespace LanceNet


