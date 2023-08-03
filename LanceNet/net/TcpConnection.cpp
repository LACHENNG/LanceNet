#include "LanceNet/base/Time.h"
#include "LanceNet/base/unix_wrappers.h"
#include <LanceNet/net/EventLoop.h>
#include <LanceNet/net/TcpConnection.h>
#include <LanceNet/net/FdChannel.h>
#include <LanceNet/base/Logging.h>
#include <cstdio>
#include <memory>
#include <assert.h>
#include <netdb.h>

namespace LanceNet
{
namespace net
{

TcpConnection::TcpConnection(EventLoop* loop, int connfd, std::string name, const SA_IN* peer_addr)
  : owner_loop_(loop),
    talkChannel_(std::make_unique<FdChannel>(loop, connfd)),
    name_(name),
    peer_addr_(*peer_addr),
    state_(kConnecting),
    talkChannelfd_(connfd)
{
    // talkChannel_->setReadCallback(std::bind(&TcpConnection::handleRead, this, std::placeholders::_1));
    // talkChannel_->setWriteCallback(std::bind(&TcpConnection::handleWrite, this));
    talkChannel_->setReadCallback([this](TimeStamp ts){
        this->handleRead(ts);
    });

    talkChannel_->setWriteCallback([this](TimeStamp ts){
        this->handleWrite();
    });
}

TcpConnection::~TcpConnection()
{
    // it`s ok to use default manner
}

void TcpConnection::setOnConnectionEstablishedCb(const OnNewConnectionEstablishedCb& cb)
{
    on_conn_established_cb_ = cb;
}

void TcpConnection::setOnMessageCb(const OnMessageCb& cb)
{
    on_message_cb_ = cb;
}

void TcpConnection::setOnDisconnectCb(const OnDisConnectionCb& cb)
{
    on_disconnt_cb_ = cb;
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
    owner_loop_->assertInEventLoopThread();
    setState(kConnected);
    if(on_conn_established_cb_){
        on_conn_established_cb_(shared_from_this(), talkChannel_->fd(), &peer_addr_);
    }
    talkChannel_->enableReading();
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
        int n = ::write(fdOfTalkChannel(), message, len);
        if(n < 0 && (n != EAGAIN && n != EWOULDBLOCK))
        {
            if(errno == EAGAIN || errno == EWOULDBLOCK){
                nWrote = 0;
            }
            else handle_error("write()");
        }
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

void TcpConnection::send(const std::string& msg)
{
    if(state_ == kDisConnecting)
        return;
    if(owner_loop_->isInEventLoopThread()){
        sendInLoop(msg.data(), msg.size());
    }else{
        std::string str(msg);
        owner_loop_->pendInLoop([p=shared_from_this(), message = std::move(str)](){
            p->sendInLoop(message.data(), message.size());
        });
    }
}

void TcpConnection::send(Buffer* buf)
{
    if(state_ == kDisConnecting)
        return;

    if(owner_loop_->isInEventLoopThread())
    {
        sendInLoop(buf->peek(), buf->readableBytes());
        buf->retrieveAll();
    }
    else{
        Buffer data(buf->initSize(), buf->prependSize());
        data.swap(*buf);
        LOG_DEBUGC << "++++++++++ before send buf: " << data.readableBytes() << " bytes";
        owner_loop_->pendInLoop([p=shared_from_this(), mybuf = std::move(data)](){
            p->sendInLoop(mybuf.peek(), mybuf.readableBytes());
        });
    }
}

void TcpConnection::shutdown()
{
    if(state_ == kConnected){
        setState(kDisConnecting);
        // owner_loop_->runInLoop(std::bind(&TcpConnection::shutdownInLoop, this));
        owner_loop_->runInLoop([this](){
            this->shutdownInLoop();
        });
    }
}

void TcpConnection::setState(StateE s)
{
    state_ = s;
}

void TcpConnection::handleRead(TimeStamp ts)
{
    owner_loop_->assertInEventLoopThread();

    int nread = inputBuffer_.readFdFast(fdOfTalkChannel());
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

void TcpConnection::destoryedConnection(const TcpConnectionPtr& conn)
{
    owner_loop_->assertInEventLoopThread();
    assert(state_ == kDisConnecting || state_ == kConnected);
    setState(kDisConnected);
    if(on_disconnt_cb_){
        on_disconnt_cb_(shared_from_this(), talkChannel_->fd(), &peer_addr_);
    }
    // at this time the only possible reference to TcpConnection is this function itself or user client code or scheduled TcpServer/TcpClient::removeChannel which often be execute soon in next round of eventloop
    // all reference in LanceNet lib is released, so if the use count not equal to 1 means client may still hold a copy to conn
    //
    // TcpServer/TcpClient::removeChannel is scheduling to run in next round of loop, so release of conn may
    // take some time, so after two secs(most likly TcpServer or TcpClient release its reference to conn), check
    // again to see any leak in user client code
    getLoop()->runAt(TimeStamp::now(), [conn]()
      {
        if(conn.use_count() != 1){
            LOG_WARNC << "TcpConnectionPtr [ fd = " 
                      << conn->fdOfTalkChannel() << " ]" 
                      << " should be destoryed but it is still referenced by your client code, do you forget to release it in your client code on dissconnection? (eg. on OnDissConnection UserCallback or so.)";
         }
       }, 2);

    owner_loop_->remove(conn->talkChannel_.get());
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
    int nWrote = ::Write(fdOfTalkChannel(),
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
    talkChannel_->disableAll();
    assert(closeCallback_);
    closeCallback_(shared_from_this());
}

void TcpConnection::handleError()
{
    LOG_DEBUG << "TcpConnection::handleError() connection name [ Fd = " << talkChannel_->fd() << " ] \" error: " << strerror(errno) << "\" this connection will be closed";
    handleClose();
}

std::string TcpConnection::peerIpPort() const
{
    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &peer_addr_.sin_addr, ip, sizeof(ip));
    int port = ntohs(peer_addr_.sin_port);
    std::string ipPort = std::string(ip) + ":" + std::to_string(port);
    return ipPort;

}

} // namespace net
} // namespace LanceNet

