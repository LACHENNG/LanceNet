// Author : Lance @ nwpu
#ifndef LanceNet_NET_SOCK_CHANNEL_H
#define LanceNet_NET_SOCK_CHANNEL_H
#include "LanceNet/base/noncopyable.h"
#include <functional>

namespace LanceNet
{
namespace net
{

class EventLoop;
class SockChannel;

// Socket Event Dispather
// have sock fd but do not own it, the owner loop is responsible for closing it
//

class SockChannel : noncopyable
{
public:
    using EventCallback = std::function<void ()>;

    SockChannel(EventLoop* loop, int fd);
    ~SockChannel() = default;

    // invoke user callbacks
    void handleEvents();

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
    // (change interested evnets)
    void enableReading();

private:
    // change interested events in fd
    void registInterestedEvent(short event);

    // let the fd event take effect
    void update();

private:
    static const short kReadEvent;
    static const short kWriteEvent;
    static const short kNoneEvent;
    static const short kErrorEvent;

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

