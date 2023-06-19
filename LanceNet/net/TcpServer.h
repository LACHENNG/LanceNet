// Author : Lance @ nwpu
#ifndef LanceNet_NET_TCPSERVER_H
#define LanceNet_NET_TCPSERVER_H

#include "LanceNet/base/Time.h"
#include "LanceNet/base/noncopyable.h"
#include "LanceNet/net/Buffer.h"
#include "LanceNet/base/unix_wrappers.h"

#include <functional>
#include <unordered_map>
#include <memory>
#include <sys/socket.h>

namespace LanceNet
{
namespace net
{

class EventLoop;
class Acceptor;
class TcpConnection;
class EventLoopPool;
// TcpServer class manage the TcpConnection obtained by accept(2).
// It is a class that is directly used by the user. The lifecycle is controlled by the user
class TcpServer: noncopyable
{
public:
    using TcpConnectionPtr = std::shared_ptr<TcpConnection>;
    using OnNewConnectionCb = std::function<void(TcpConnectionPtr tcpConnPtr, int conn_fd, const SA_IN* peer_addr)>;
    using OnMessageCb = std::function<void(const TcpConnectionPtr&, Buffer* buf, TimeStamp ts)>;

    TcpServer(EventLoop* loop, int listen_port);
    ~TcpServer(); // force out-line dtor, for unique_ptr members

    // set new conntion callback
    // not thread safe
    void setNewConnectionCb(const OnNewConnectionCb& cb);

    // set message callback
    // not thread safe
    void setMessageCb(const OnMessageCb& cb);

    void start();

private:
    // Accepting new connections
    // not thread save
    // but in loop
    void onNewConnection(int conn_fd, const SA_IN* peer_addr);
    EventLoop* owner_loop_;
    std::unique_ptr<Acceptor> acceptor_; // avoid revealing Acceptor

    // identifier for each new TcpConnection
    std::string name;
    int totalCreatedTcpConnections_;  // in loop

    // not thread safe
    // but in loop
    void removeConnectionInLoop(const TcpConnectionPtr& conn);
    // thread safe
    void removeConnection(const TcpConnectionPtr& conn);

    OnNewConnectionCb onNewConnCb_;
    OnMessageCb onMessageCb_;

    // managing TcpConnections
    std::unordered_map<std::string, TcpConnectionPtr> tcp_connections_;

    std::unique_ptr<EventLoopPool> threadPool_;
};

} // namespace net
} // namespace LanceNet

#endif // LanceNet_NET_TCPSERVER_H
