// Author : Lance @ nwpu
#ifndef LanceNet_NET_TCP_CONNECTION_H
#define LanceNet_NET_TCP_CONNECTION_H

#include "LanceNet/base/noncopyable.h"
#include "LanceNet/base/unix_wrappers.h"
#include "LanceNet/net/Buffer.h"
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
    using OnMessageCb = std::function<void(const TcpConnectionPtr&, Buffer* buf, TimeStamp ts)>;

    using OnCloseConnectionCb = OnNewConnectionEstablishedCb;
    using CloseCallback = std::function<void(TcpConnectionPtr conn_ptr)>;

    TcpConnection(EventLoop* loop, int connfd,
            std::string name,
            const SA_IN* peer_addr);
    ~TcpConnection();

    // registered user callback
    // not thread safe
    void setOnConnectionEstablishedCb(const OnNewConnectionEstablishedCb& cb);
    void setOnMessageCb(const OnMessageCb& cb);
    void setOnDisconnectCb(const OnCloseConnectionCb& cb);

    // use internally by TcpServer or TcpClient
    // usually it is bind to TcpServer::removeConnection
    void setCloseCallback(const CloseCallback& cb);

    // called by TcpServer
    // not thread safe
    // but in loop
    void connectionEstablished();

    // thread safe
    void send(const void* message, size_t len);
    // thread safe
    void send(const std::string& message);
    // thread safe
    void shutdown();

    // runInRoop
    void destoryedConnection(TcpConnectionPtr conn);
    std::string name() { return name_ ;}

    EventLoop* getLoop() const{ return owner_loop_;}

private:
    enum StateE{kConnecting, kConnected, kDisConnecting, kDisConnected};

    void setState(StateE s);
    // Not thread safe
    // but in loop
    void handleRead(TimeStamp ts);
    // callback when talkChannel_->fd() is writeable
    // Not thread safe
    // but in loop
    void handleWrite();

    void handleClose();
    void handleError();

    void pendSendInLoop(std::string message);
    void sendInLoop(const void* message, size_t len);
    void shutdownInLoop();

    EventLoop* owner_loop_;
    std::unique_ptr<FdChannel> talkChannel_;

    std::string name_;
    // peer address info
    SA_IN peer_addr_;

    // user registered callbacks
    OnNewConnectionEstablishedCb on_conn_established_cb_;
    OnMessageCb on_message_cb_;
    OnCloseConnectionCb on_close_cb_;
    // bind to TcpServer::removeConnection
    CloseCallback closeCallback_;

    // read,write buffer
    Buffer inputBuffer_;
    Buffer outputBuffer_;

    StateE state_;
};

} // namespace net
} // namespace LanceNet

#endif // LanceNet_NET_TCP_CONNECTION_H
