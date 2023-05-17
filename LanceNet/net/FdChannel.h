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

class FdChannel : noncopyable
{
public:
    using EventCallback = std::function<void (TimeStamp)>;

    explicit FdChannel(EventLoop* loop, int fd);
    ~FdChannel();

    // invoke user callbacks
    void handleEvents(TimeStamp ts);

    void setReadCallback(EventCallback cb);
    void setWriteCallback(EventCallback cb);

    // getter
    int fd();
    short events();
    short revents();
    int index(); // the index of fd_ in poolfd

    // setter
    void events(short newevents); // set events like in pollfd.events
    void revents(short newEvent);// set revents like in pollfd.revent
    void index(int newIndex);    // set index in pollfd

    // let the interested events take effect.
    // (change interested events)
    void enableReading();
    void enableWriting();
    void disableWriting();
    void disableAll();
    
    bool isNoneEvent();

private:
    // change interested events in fd
    void registInterestedEvent(short event);
    void unregisterEvent(short event);

    // interact with EventLoop, let the fd event take effect
    void update();

private:
    static const short kReadEvent;
    static const short kWriteEvent;
    static const short kNoneEvent;
    static const short kErrorEvent;
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

