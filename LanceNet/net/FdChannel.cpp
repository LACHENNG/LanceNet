#include "LanceNet/base/Time.h"
#include <LanceNet/net/FdChannel.h>
#include <LanceNet/net/EventLoop.h>
#include <LanceNet/base/Logging.h>
#include <LanceNet/base/unix_wrappers.h>
#include <assert.h>
#include <sys/poll.h>

namespace LanceNet
{
namespace net
{

const short FdChannel::kReadEvent = POLLIN | POLLPRI | POLLRDHUP;
const short FdChannel::kWriteEvent = POLLOUT;
const short FdChannel::kErrorEvent = POLLERR | POLLNVAL;
const short FdChannel::kNoneEvent = 0;

FdChannel::FdChannel(EventLoop* loop, int fd)
  : eventHandling(false),
    fd_(fd),
    index_(-1),
    events_(0),
    revents_(0),
    owner_loop_(loop)
{
}

// unregister channel in ower_looper
FdChannel::~FdChannel()
{
    assert(!eventHandling);
    Close(fd_);
    LOG_DEBUGC << "~FdChannel() with fd[" << fd_ << "]" << "closed";
}

void FdChannel::handleEvents(TimeStamp ts)
{
    eventHandling = true;
    owner_loop_->assertInEventLoopThread();
    // invalid request : fd not open
    if(revents_ & kErrorEvent){
        if(onErrorCb_) onErrorCb_(ts);
    }
    // read event
    if(revents_ &  kReadEvent){
        if(onReadCb_) onReadCb_(ts);
    }

    // write event
    if(revents_ & kWriteEvent){
        if(onWriteCb_) onWriteCb_(ts);
    }
    eventHandling = false;
}


void FdChannel::disableAll()
{
    // WRONG: only set event = 0 can not mask all event like POLLERR
    events_ = 0;
    owner_loop_->disableAllEvent(this);
}

void FdChannel::registInterestedEvent(short event)
{
    events_ |= event;
    this->update();
}

void FdChannel::unregisterEvent(short event)
{
    events_ &= (~event);
    this->update();
}

void FdChannel::update()
{
    owner_loop_->update(this);
}

} // namespace net
} // namespace LanceNet

