#include <LanceNet/net/EventLoop.h>
#include <LanceNet/net/TcpConnection.h>
#include <LanceNet/net/FdChannel.h>
#include <LanceNet/base/Logging.h>
#include <memory>
#include <assert.h>

namespace LanceNet
{
namespace net
{

TcpConnection::TcpConnection(EventLoop* loop, int connfd, std::string name, const SA_IN* peer_addr)
  : owner_loop_(loop),
    talkChannel_(std::make_unique<FdChannel>(loop, connfd)),
    name_(name),
    peer_addr_(*peer_addr)
{
    talkChannel_->setReadCallback(std::bind(&TcpConnection::handleRead, this));
    talkChannel_->enableReading();
}

TcpConnection::~TcpConnection()
{
    LOG_INFOC << "~TcpConnection() Fd = " << talkChannel_->fd();
}

void TcpConnection::setOnConnectionEstablishedCb(const OnNewConnectionEstablishedCb& cb)
{
    on_conn_established_cb_ = cb;
}

void TcpConnection::setOnMessageCb(const OnMessageCb& cb)
{
    on_message_cb_ = cb;
}

void TcpConnection::setOnDisconnectCb(const OnCloseConnectionCb& cb)
{
    on_close_cb_ = cb;
}

void TcpConnection::setCloseCallback(const CloseCallback& cb)
{
    closeCallback_ = cb;
}

void TcpConnection::connectionEstablished()
{
    on_conn_established_cb_(shared_from_this(), talkChannel_->fd(), &peer_addr_);
}

void TcpConnection::handleRead()
{
    owner_loop_->assertInEventLoopThread();
    char buf[65535];
    auto nread = Read(talkChannel_->fd(), buf, sizeof(buf));

    if(nread == 0){
        handleClose();
    }else if(nread > 0){
        if(on_message_cb_){
            on_message_cb_(buf, nread);
        }else{
            LOG_WARNC << "OnMessageCb is not set";
        }
    }else{
        handleError();
    }
}

void TcpConnection::handleWrite()
{
}

void TcpConnection::handleClose()
{
    owner_loop_->assertInEventLoopThread();
    LOG_INFOC << "TcpConnection::handleClose()";
    //FIXME disable talkChannel_
    assert(closeCallback_); // the internally used close callback must be set
    closeCallback_(shared_from_this());
}

void TcpConnection::handleError()
{
    LOG_WARNC << "TcpConnection::handleError() connection name [Fd = " << talkChannel_->fd() << "]";
}

} // namespace net
} // namespace LanceNet

