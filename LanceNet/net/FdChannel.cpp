#include "LanceNet/base/Time.h"
#include <LanceNet/net/FdChannel.h>
#include <LanceNet/net/EventLoop.h>
#include <LanceNet/base/Logging.h>
#include <LanceNet/base/unix_wrappers.h>
#include <sys/poll.h>

namespace LanceNet
{
namespace net
{

const short FdChannel::kReadEvent = POLLIN | POLLPRI | POLLRDHUP;
const short FdChannel::kWriteEvent = POLLOUT;
const short FdChannel::kErrorEvent = POLLERR | POLLNVAL;

FdChannel::FdChannel(EventLoop* loop, int fd)
  : fd_(fd),
    index_(-1),
    events_(0),
    revents_(0),
    owner_loop_(loop)
{
}

// unregister channel in ower_looper
FdChannel::~FdChannel()
{
    owner_loop_->remove(this);
    Close(fd_);
    LOG_INFOC << "FdChannel[fd=" << fd_ << "]" << " unregistered and closed";
}

void FdChannel::handleEvents(TimeStamp ts)
{
    owner_loop_->assertInEventLoopThread();
    // invalid request : fd not open
    if(revents_ & kErrorEvent){
        LOG_WARNC << "FdChannel::handleAllEvents() POLLERR | POLLINVAL";
        onErrorCb_(ts);
    }
    // read event
    if(revents_ &  kReadEvent){
        if(onReadCb_) onReadCb_(ts);
    }

    // write event
    if(revents_ & kWriteEvent){
        if(onWriteCb_) onWriteCb_(ts);
    }
}

void FdChannel::enableReading()
{
    LOG_INFOC << "registered reading event for fd = " << fd();
    registInterestedEvent(kReadEvent);
}

void FdChannel::registInterestedEvent(short event)
{
    events_ |= event;
    this->update();
}

void FdChannel::update()
{
    owner_loop_->update(this);
}


void FdChannel::setReadCallback(EventCallback cb)
{
    onReadCb_ = cb;
}

void FdChannel::setWriteCallback(EventCallback cb)
{
    onWriteCb_ = cb;
}


// getter
int FdChannel::fd()
{
    return fd_;
}

short FdChannel::events()
{
    return events_;
}

short FdChannel::revents()
{
    return revents_;
}

int FdChannel::index()
{
    return index_;
}

// setter
void FdChannel::events(short newEvents)
{
    events_ = newEvents;
}

void FdChannel::revents(short revents)
{
    revents_ = revents;
}

void FdChannel::index(int newIndex)
{
    index_ = newIndex;
}




} // namespace net
} // namespace LanceNet

