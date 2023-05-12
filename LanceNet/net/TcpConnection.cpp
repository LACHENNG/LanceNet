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
    conn_established_cb_ = cb;
}

void TcpConnection::setOnMessageCb(const OnMessageCb& cb)
{
    on_message_cb_ = cb;
}

void TcpConnection::setOnDisconnectCb(const OnDisconnectCb& cb)
{
    disconnect_cb_ = cb;
}

void TcpConnection::setOnCloseCb(const OnCloseConnectionCb& cb)
{
    on_close_cb_ = cb;
}

void TcpConnection::handleRead()
{
    owner_loop_->assertInEventLoopThread();
    char buf[65535];
    auto nread = Read(talkChannel_->fd(), buf, sizeof(buf));

    // FIXME peer closed the connection
    if(nread == 0){
        // close
        on_close_cb_(shared_from_this());
        return ;
    }else if(nread > 0){
        if(on_message_cb_){
            on_message_cb_(buf, nread);
        }else{
            LOG_WARNC << "OnMessageCb is not set";
        }
    }else{
        //FIXME handle error
    }
}

} // namespace net
} // namespace LanceNet

