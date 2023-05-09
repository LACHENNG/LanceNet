
// Author : Lance @ nwpu
#ifndef LanceNet_NET_ACCEPTOR_H
#define LanceNet_NET_ACCEPTOR_H

// #include "LanceNet/base/ThisThread.h"
// #include "LanceNet/base/Time.h"
#include "LanceNet/base/noncopyable.h"
#include "LanceNet/base/unix_wrappers.h"
#include <functional>
#include <memory>
#include <sys/socket.h>
// #include "LanceNet/base/Mutex.h"
// #include "LanceNet/net/IOMultiplexer.h"
// #include "LanceNet/net/TimerId.h"


namespace LanceNet
{
namespace net
{

class EventLoop;
class FdChannel;

class Acceptor : noncopyable
{
public:
    using OnConnectCallback = std::function<void (int conn_socket, const SA_IN* peer_sockaddr)>;

    Acceptor(EventLoop* ownerLoop, int listenPort);

    void setOnConnectionCallback(const OnConnectCallback& onConnetCb);

private:
    void handleRead();
    
    int listen_port_;
    int listen_fd_;
    std::unique_ptr<FdChannel> listenChannel;
    OnConnectCallback onConnectCb_;
};

} // namespace net
} // namespace LanceNet

#endif // LanceNet_NET_ACCEPTOR_H
