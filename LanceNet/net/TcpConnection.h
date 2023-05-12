// Author : Lance @ nwpu
#ifndef LanceNet_NET_TCP_CONNECTION_H
#define LanceNet_NET_TCP_CONNECTION_H

#include "LanceNet/base/noncopyable.h"
#include "LanceNet/base/unix_wrappers.h"
#include "LanceNet/net/TcpServer.h"

#include <functional>
#include <memory>
#include <sys/socket.h>
#include <string>

namespace LanceNet
{
namespace net
{

class EventLoop;
class FdChannel;
// TcpConnection class manage conn_fd and Represents a session with a client
// It is usually mamnage by shared_ptr due to it ambiguous life cycle
// It use a FdChannel to monitor the read or write and other events
class TcpConnection: public noncopyable,
                     public std::                      enable_shared_from_this<TcpConnection>
{
public:
    using TcpConnectionPtr = std::shared_ptr<TcpConnection>;
    using OnNewConnectionEstablishedCb = std::function<void(TcpConnectionPtr connPtr, int conn_fd, const SA_IN* peer_addr)>;
    using OnMessageCb = std::function<void(const char* buf, size_t len)>;
    using OnDisconnectCb = OnNewConnectionEstablishedCb;
    using OnCloseConnectionCb = std::function<void(TcpConnectionPtr conn_ptr)>;

    TcpConnection(EventLoop* loop, int connfd,
            std::string name,
            const SA_IN* peer_addr);
    // FIXME outline dtor
    ~TcpConnection();

    void setOnConnectionEstablishedCb(const OnNewConnectionEstablishedCb& cb);
    void setOnMessageCb(const OnMessageCb& cb);
    void setOnDisconnectCb(const OnDisconnectCb& cb);

    // use internally by TcpServer or TcpClient
    // to notify them to remove the TcpConnectionPtr they hold
    void setOnCloseCb(const OnCloseConnectionCb& cb);

    std::string name() { return name_ ;}

private:
    // callback is called when the data in FdChannel arrives
    // Not thread safe
    // but in loop
    void handleRead();
    // read return 0 , so close the TcpConnection
    // Not thread safe
    // but in loop

    EventLoop* owner_loop_;
    std::unique_ptr<FdChannel> talkChannel_;

    std::string name_;
    // peer address info
    SA_IN peer_addr_;

    OnNewConnectionEstablishedCb conn_established_cb_;
    OnMessageCb on_message_cb_;
    OnDisconnectCb disconnect_cb_;
    OnCloseConnectionCb on_close_cb_;
};

} // namespace net
} // namespace LanceNet

#endif // LanceNet_NET_TCP_CONNECTION_H
