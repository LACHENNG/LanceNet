// Author : Lance @ nwpu
#ifndef LanceNet_NET_SOCK_CHANNEL_H
#define LanceNet_NET_SOCK_CHANNEL_H
#include "LanceNet/base/Time.h"
#include "LanceNet/base/noncopyable.h"
#include <functional>

namespace LanceNet
{
namespace net
{

class EventLoop;

// Socket Event dispatch for every single fd
// Every single FdChannel is owned by a IO thread(EventLoop thread)
// It basically represents IO handlers
// Contains a socket fd but do not own it, the owner loop is responsible for closing it
// Must unregister itself when dtor
//
// @brief: Channel class expresses the relationship between a file descriptor and its interested IO events, as well as the callback functions that should be executed when these events occur.
class FdChannel : noncopyable
{
public:
    using EventCallback = std::function<void (TimeStamp)>;

    explicit FdChannel(EventLoop* loop, int fd);
    ~FdChannel();

    // invoke user callbacks
    void handleEvents(TimeStamp ts);

    void setReadCallback(const EventCallback& cb) { onReadCb_ = cb; }
    void setWriteCallback(const EventCallback& cb) { onWriteCb_ = cb; }

    // getter
    int fd() const { return fd_; }
    short events() const { return events_; }
    short revents() const { return revents_; }
    int index() const { return index_; } // the index of fd_ in poolfd

    bool isWriteEnabled(){ return static_cast<bool>(events_ & kWriteEvent); }
    bool isReadEnabled() { return static_cast<bool>(events_ & kReadEvent); }

    bool isNoneEvent() { return events_ == kNoneEvent; }

    // setter
    // set events that interested in
    void events(short newInterestedEvent) { events_ = newInterestedEvent; }
    // set return events actually happend
    void revents(short revents) { revents_ = revents; }
    // set index in pollfd
    void index(int newIndex) { index_ = newIndex; }

    // cares on readable events
    void enableReading() { registInterestedEvent(kReadEvent); }
    // care on writeable events 
    void enableWriting() { registInterestedEvent(kWriteEvent); }

    // not care on writeable events 
    void disableWriting() { unregisterEvent(kWriteEvent); }
    // not care on any events
    void disableAll();

private:
    // change interested events in fd
    void registInterestedEvent(short event);
    void unregisterEvent(short event);

    // interact with EventLoop, let the fd event take effect
    void update();

public:
    static const short kReadEvent;
    static const short kWriteEvent;
    static const short kNoneEvent;
    static const short kErrorEvent;
private:
    // assert that Channel is not deconstruct during handing events
    bool eventHandling;
    const int fd_;
    // record index in poolfd in IOMultiplexer class
    int index_;

    // registered events we are interested in
    short events_;
    // acutally occured events
    short revents_;

    EventLoop* owner_loop_;

    // user callbacks on events
    EventCallback onReadCb_;
    EventCallback onWriteCb_;
    EventCallback onErrorCb_;
};

} // net
} // namespace LanceNet

#endif // LanceNet_NET_SOCK_CHANNEL_H

