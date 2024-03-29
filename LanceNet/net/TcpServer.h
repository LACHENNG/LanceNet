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
    using OnNewConnectionCb = std::function<void(const TcpConnectionPtr& tcpConnPtr, int conn_fd, const SA_IN* peer_addr)>;
    using OnDissconnectionCb = OnNewConnectionCb;
    using OnMessageCb = std::function<void(const TcpConnectionPtr&, Buffer* buf, TimeStamp ts)>;
    
    // numThreads = 0 means use main thread as IO thread
    TcpServer(EventLoop* loop, int listen_port, int numThreads = 0);
    ~TcpServer(); // force out-line dtor, for unique_ptr members

    // set new conntion callback
    // not thread safe
    void setOnNewConnectionCb(const OnNewConnectionCb& cb);
    void setOnDissconnectionCb(const OnDissconnectionCb& cb);
    // set message callback
    // not thread safe
    void setOnMessageCb(const OnMessageCb& cb);

    void start();

    EventLoop* mainLoop() const { return main_loop_;}

private:
    // Accepting new connections
    // not thread save
    // but in loop
    void onNewConnection(int conn_fd, const SA_IN* peer_addr);
    EventLoop* main_loop_;
    std::unique_ptr<Acceptor> acceptor_; // avoid revealing Acceptor

    // identifier for each new TcpConnection
    std::string name;
    int totalCreatedTcpConnections_;  // in loop

    // not thread safe
    // but in main loop
    void removeConnectionInLoop(const TcpConnectionPtr& conn);
    // not thread safe
    // but in main loop
    void removeConnection(const TcpConnectionPtr& conn);

    OnNewConnectionCb onNewConnCb_;
    OnMessageCb onMessageCb_;
    OnDissconnectionCb onDissconCb_;
    // managing TcpConnections
    std::unordered_map<std::string, TcpConnectionPtr> tcp_connections_;

    std::unique_ptr<EventLoopPool> eventloopPool_;
};

} // namespace net
} // namespace LanceNet

#endif // LanceNet_NET_TCPSERVER_H
