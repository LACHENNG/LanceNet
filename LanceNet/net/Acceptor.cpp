#include "LanceNet/base/unix_wrappers.h"
#include "LanceNet/net/EventLoop.h"
#include "LanceNet/net/FdChannel.h"
#include <LanceNet/net/Acceptor.h>
#include <LanceNet/base/Logging.h>
#include <sys/socket.h>
#include <assert.h>

namespace LanceNet
{
namespace net
{

Acceptor::Acceptor(EventLoop* loop, int listenPort)
  : owner_loop_(loop),
    listen_port_(listenPort),
    listen_fd_(Open_listenfd(listenPort)),
    listenChannel(std::make_unique<FdChannel>(loop, listen_fd_)),
    listening_(false)
{
    LOG_INFOC << "Server: Listening at port " << listen_port_;
    listenChannel->setReadCallback(std::bind(&Acceptor::handleRead, this));
}
bool Acceptor::startListen()
{
    owner_loop_->assertInEventLoopThread();
    assert(!listening());
    listenChannel->enableReading();
    listening_ = true;
    return true;
}

void Acceptor::handleRead()
{
    struct sockaddr_storage peer_addr;
    socklen_t peerlen = sizeof(peer_addr);

    int conn_fd = Accept4(listen_fd_, reinterpret_cast<struct sockaddr*>(&peer_addr), &peerlen, SOCK_NONBLOCK | SOCK_CLOEXEC);

    // current only support ipv4
    if(peer_addr.ss_family != AF_INET){
        LOG_WARN << "Peer not use IPV4, this library currently only support ipv4";
        return ;
    }
    if(onConnectCb_){
        onConnectCb_(conn_fd, reinterpret_cast<const SA_IN*>(&peer_addr));
    }
    else{
        LOG_WARN << "OnConnection Callback is not set";
    }
}


void Acceptor::setOnConnectionCallback(const OnConnectCallback& onConnetCb)
{
    onConnectCb_ = onConnetCb;
}


} // namespace net
} // namespace LanceNet

