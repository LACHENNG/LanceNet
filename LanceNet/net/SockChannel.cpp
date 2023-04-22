#include <LanceNet/net/SockChannel.h>
#include <LanceNet/net/EventLoop.h>
#include <LanceNet/base/Logging.h>
#include <sys/poll.h>

namespace LanceNet
{
namespace net
{

const short SockChannel::kReadEvent = POLLIN | POLLPRI | POLLRDHUP;
const short SockChannel::kWriteEvent = POLLOUT;
const short SockChannel::kErrorEvent = POLLERR | POLLNVAL;

SockChannel::SockChannel(EventLoop* loop, int fd)
  : fd_(fd),
    index_(-1),
    events_(0),
    revents_(0),
    owner_loop_(loop)
{
}

void SockChannel::handleEvents()
{
    owner_loop_->assertInEventLoopThread();
    // invalid request : fd not open
    if(revents_ & kErrorEvent){
        LOG_WARNC << "SockChannel::handleAllEvents() POLLERR | POLLINVAL";
        onErrorCb_();
    }
    // read event
    if(revents_ &  kReadEvent){
        if(onReadCb_) onReadCb_();
    }

    // write event
    if(revents_ & kWriteEvent){
        if(onWriteCb_) onWriteCb_();
    }
}

void SockChannel::enableReading()
{
    registInterestedEvent(kReadEvent);
}

void SockChannel::registInterestedEvent(short event)
{
    events_ |= event;
    this->update();
}

void SockChannel::update()
{
    owner_loop_->update(this);
}


void SockChannel::setReadCallback(EventCallback cb)
{
    onReadCb_ = cb;
}

void SockChannel::setWriteCallback(EventCallback cb)
{
    onWriteCb_ = cb;
}


// getter
int SockChannel::fd()
{
    return fd_;
}

short SockChannel::events()
{
    return events_;
}

short SockChannel::revents()
{
    return revents_;
}

int SockChannel::index()
{
    return index_;
}

// setter
void SockChannel::events(short newEvents)
{
    events_ = newEvents;
}

void SockChannel::revents(short revents)
{
    revents_ = revents;
}

void SockChannel::index(int newIndex)
{
    index_ = newIndex;
}




} // namespace net
} // namespace LanceNet

