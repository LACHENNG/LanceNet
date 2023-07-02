#include "LanceNet/base/unix_wrappers.h"
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
    peer_addr_(*peer_addr),
    state_(kConnecting)
{
    talkChannel_->setReadCallback(std::bind(&TcpConnection::handleRead, this, std::placeholders::_1));
    talkChannel_->setWriteCallback(std::bind(&TcpConnection::handleWrite, this));
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

void TcpConnection::setOnWriteCompleteCb(const OnWriteCompleteCb& cb)
{
    on_writecomplete_cb_ = cb;
}
void TcpConnection::setCloseCallback(const CloseCallback& cb)
{
    closeCallback_ = cb;
}

void TcpConnection::connectionEstablished()
{
    setState(kConnected);
    if(on_conn_established_cb_){
        on_conn_established_cb_(shared_from_this(), talkChannel_->fd(), &peer_addr_);
    }
}

void TcpConnection::sendInLoop(const void* message, size_t len)
{
    owner_loop_->assertInEventLoopThread();

    if(state_ != kConnected){
        return ;
    }
    // try to send directly and append the remains to OutputBuffer
    int nWrote = 0;
    if(outputBuffer_.readableBytes() == 0 )
    {
        int n = Write(talkChannel_->fd(), message, len);
        nWrote += n;
    }
    // FIXME: this may have the problem that the client receive data too slow
    // the buffer may grow too fast in server
    if(nWrote < static_cast<int>(len)){
        outputBuffer_.append(static_cast<const char*>(message) + nWrote, len - nWrote);
        if(!talkChannel_->isWriteEnabled()){
            talkChannel_->enableWriting();
        }
    }
    if(nWrote == static_cast<int>(len) && on_writecomplete_cb_){
        on_writecomplete_cb_(shared_from_this());
    }
}

void TcpConnection::shutdownInLoop()
{
    owner_loop_->assertInEventLoopThread();
    if(!talkChannel_->isWriteEnabled())
    {
        // no more data is waiting to be sent, so we can shutdown WR directly
        Shutdown(talkChannel_->fd(), SHUT_WR);
    }
}

void TcpConnection::send(const void* message, size_t len)
{
    if(state_ == kDisConnecting)
        return;
    if(owner_loop_->isInEventLoopThread()){
        sendInLoop(message, len);
    }else{
        std::string str(static_cast<const char*>(message), static_cast<const char*>(message) + len);
        owner_loop_->pendInLoop([p=shared_from_this(), message = std::move(str)](){
            p->sendInLoop(message.data(), message.size());
        });
    }
}

void TcpConnection::send(const std::string& message)
{
    if(state_ == kDisConnecting)
        return;
    send(message.data(), message.size());
}

void TcpConnection::shutdown()
{
    if(state_ == kConnected){
        setState(kDisConnecting);
        owner_loop_->runInLoop(std::bind(&TcpConnection::shutdownInLoop, this));
    }
}

void TcpConnection::setState(StateE s)
{
    state_ = s;
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
    owner_loop_->assertInEventLoopThread();
    assert(state_ == kDisConnecting || state_ == kConnected);
    owner_loop_->remove(conn->talkChannel_.get());
    setState(kDisConnected);
}

// try to send the remain data in output buffer
// if the state_ is kDisConnecting, the actual shutdown is called after all the data is sent
void TcpConnection::handleWrite()
{
    owner_loop_->assertInEventLoopThread();
    if(!talkChannel_->isWriteEnabled()){
        LOG_INFOC << "Connection is down, no more writing";
        return ;
    }
    int nreadable = outputBuffer_.readableBytes();
    int nWrote = ::Write(talkChannel_->fd(),
                        outputBuffer_.peek(),
                        outputBuffer_.readableBytes());
    outputBuffer_.retrieve(nWrote);
    assert(static_cast<int>(outputBuffer_.readableBytes()) == nreadable - nWrote);

    // unregister writing as we are using level triggle
    assert(talkChannel_->isWriteEnabled());
    if(nWrote == nreadable){
        talkChannel_->disableWriting();
        if(on_writecomplete_cb_){
            on_writecomplete_cb_(shared_from_this());
        }
        if(state_ == kDisConnected){
            shutdownInLoop();
        }
        
    }
}

void TcpConnection::handleClose()
{
    owner_loop_->assertInEventLoopThread();
    assert(state_ == kConnected || state_ == kDisConnecting);
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

