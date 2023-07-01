#include <LanceNet/net/EventLoop.h>
#include <LanceNet/net/FdChannel.h>
#include <LanceNet/net/Acceptor.h>
#include <LanceNet/base/Logging.h>
#include <LanceNet/net/TcpServer.h>
#include <LanceNet/net/TcpConnection.h>
#include <LanceNet/net/EventLoopPool.h>

#include <functional>
#include <memory>
#include <assert.h>

namespace LanceNet
{
namespace net
{
using namespace std::placeholders;

TcpServer::TcpServer(EventLoop* loop, int listen_port)
  : owner_loop_(loop),
    acceptor_(std::make_unique<Acceptor>(loop, listen_port)),
    name("TcpConnection "),
    totalCreatedTcpConnections_(0),
    eventloopPool_(std::make_unique<EventLoopPool>(owner_loop_))
{
}

TcpServer::~TcpServer()
{
}

void TcpServer::start()
{
    acceptor_->setOnConnectionCallback(
        std::bind(&TcpServer::onNewConnection, this, _1, _2));
}

void TcpServer::setNewConnectionCb(const OnNewConnectionCb &cb)
{
    onNewConnCb_ = cb;
}

void TcpServer::setMessageCb(const OnMessageCb& cb)
{
    onMessageCb_ = cb;
}

void TcpServer::onNewConnection(int conn_fd, const SA_IN* peer_addr)
{
    owner_loop_->assertInEventLoopThread();
    ++totalCreatedTcpConnections_;

    auto conn_name = name + itoa_s(totalCreatedTcpConnections_);
    auto ioLoop = eventloopPool_->getNextLoop();
    auto tcpConnPtr = std::make_shared<TcpConnection>(ioLoop,
            conn_fd, conn_name, peer_addr);

    tcpConnPtr->setOnConnectionEstablishedCb(onNewConnCb_);
    tcpConnPtr->setOnMessageCb(onMessageCb_);
    tcpConnPtr->setCloseCallback(std::bind(&TcpServer::removeConnection, this, _1));
    ioLoop->runInLoop(std::bind(&TcpConnection::connectionEstablished, tcpConnPtr));
    // tcpConnPtr->connectionEstablished();
    tcp_connections_[conn_name] = tcpConnPtr;
}

void TcpServer::removeConnectionInLoop(const TcpConnectionPtr& conn)
{
    owner_loop_->assertInEventLoopThread();
    LOG_INFOC << "TcpServer::removeConnection [" << conn->name() << "]";
    auto nErased = tcp_connections_.erase(conn->name());
    assert(nErased != 0);
    // normally TcpConnectionPtr now is destoryed
    // so does the FdChannel it contains, but at the time removeConnection is a callback called by FdChannel::handleEvent
    // we can not destory FdChannel when we are still using it

    // So must extend the lifetime of TcpConnection
    // otherwise after closeCallback_ with unregister itself
    // from TcpServer and cause TcpConnection deconstruct
    //owner_loop_->runInLoop(std::bind(&TcpConnection::destoryedConnection, conn.get(), conn));
    auto ioLoop = conn->getLoop();
    ioLoop->pendInLoop([conn](){
        LOG_INFOC << "connection destoryed in Loop with connection name = " << "[" <<  conn->name() << "]";
        conn->destoryedConnection(conn);
    });

}

void TcpServer::removeConnection(const TcpConnectionPtr& conn)
{
    // FIXME: unsafe
    // auto ioLoop = conn->getLoop();
    // ioLoop->runInLoop(std::bind(&TcpServer::removeConnectionInLoop, this, conn));
    removeConnectionInLoop(conn);
}

} // namespace net
} // namespace LanceNet
