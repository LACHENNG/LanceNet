#include "LanceNet/base/unix_wrappers.h"
#include "LanceNet/net/TcpConnection.h"
#include <LanceNet/net/TcpClient.h>
#include <LanceNet/net/Connector.h>
#include <LanceNet/net/EventLoop.h>
#include <LanceNet/net/Buffer.h>
#include <LanceNet/base/Time.h>
#include <LanceNet/base/Logging.h>

#include <assert.h>
#include <functional>
#include <memory>

namespace LanceNet
{
namespace net
{

TcpClient::TcpClient(int port, const std::string& serverName, EventLoop* loop)
  : port_(port),
    serverName_(serverName),
    loop_(loop),
    connectorPtr_(std::make_unique<Connector>(loop, port, serverName))
{
    connectorPtr_->setConnectionCallback(
        std::bind(&TcpClient::OnRawConnection, this, std:: placeholders::_1)
    );
}

TcpClient::~TcpClient()
{
    // default ctor is okay
}

void TcpClient::start()
{
    connectorPtr_->start();
}

void TcpClient::setOnMessageCallback(OnMessageCb messagecb)
{
    messagecb_ = messagecb;
}

void TcpClient::setOnDisconnectedCb(OnDisconnectedCb ondisconnectedcb)
{
    disconnectcb_ = ondisconnectedcb;
}

void TcpClient::setOnConnectionCallback(OnConnectionEstablishedCb connectcb)
{
    connectioncb_ = connectcb;
}

void TcpClient::OnRawConnection(int conn_sock)
{
    auto sa_in = getPeerAddr(conn_sock);
    tcpConnectionPtr_.reset(
            new TcpConnection(loop_,
                              conn_sock,
                              "TcpConnection",
                              &sa_in));
    // tcpConnectionPtr_->setCloseCallback(
    //    std::bind(&TcpClient::removeConnection, this, std::placeholders::_1));
    tcpConnectionPtr_->setCloseCallback([this](const TcpConnectionPtr &conn){
        this->removeConnection(conn);
    });

    tcpConnectionPtr_->setOnMessageCb(messagecb_);
    LOG_DEBUG << "last line in TcpClient::OnRawConnection conn use count before user on connection cb " << tcpConnectionPtr_.use_count();
    tcpConnectionPtr_->setOnConnectionEstablishedCb(connectioncb_);
    tcpConnectionPtr_->setOnDisconnectCb(disconnectcb_);

    tcpConnectionPtr_->connectionEstablished();
    LOG_DEBUG << "last line in TcpClient::OnRawConnection conn use count " << tcpConnectionPtr_.use_count();
}

void TcpClient::removeConnection(const TcpConnectionPtr& conn)
{
    // FIXME: unsafe
    auto ioLoop = conn->getLoop();
    tcpConnectionPtr_.reset();
    ioLoop->pendInLoop([conn](){
        LOG_INFOC << "connection destoryed in Loop with connection name = " << "[" <<  conn->name() << "]" << " use count " << conn.use_count();
        conn->destoryedConnection(conn);
    });
}

} // namespace net
} // namespace LanceNet
