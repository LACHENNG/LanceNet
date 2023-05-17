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
    talkChannel_->setReadCallback(std::bind(&TcpConnection::handleRead, this, std::placeholders::_1));
    talkChannel_->setWriteCallback(std::bind(&TcpConnection::handleWrite, this, std::placeholders::_1));
    talkChannel_->enableReading();
    // talkChannel_->enableWriting();
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

void TcpConnection::handleRead(TimeStamp ts)
{
    owner_loop_->assertInEventLoopThread();

    int nread = inputBuffer_.readFd(talkChannel_->fd());
    if(nread == 0){
        handleClose();
    }else if(nread > 0){
        if(on_message_cb_){
            on_message_cb_(shared_from_this(), &inputBuffer_, ts);
        }else{
            LOG_WARNC << "OnMessageCb is not set";
        }
    }else{
        handleError();
    }
}

void TcpConnection::destoryedConnection(TcpConnectionPtr conn)
{
    owner_loop_->remove(conn->talkChannel_.get());
}

void TcpConnection::handleWrite(TimeStamp ts)
{

}

void TcpConnection::handleClose()
{
    owner_loop_->assertInEventLoopThread();
    LOG_INFOC << "TcpConnection::handleClose()";
    talkChannel_->disableAll();
    assert(closeCallback_);
    closeCallback_(shared_from_this());
}

void TcpConnection::handleError()
{
    LOG_WARNC << "TcpConnection::handleError() connection name [Fd = " << talkChannel_->fd() << "]" << strerror(errno);
}

} // namespace net
} // namespace LanceNet

