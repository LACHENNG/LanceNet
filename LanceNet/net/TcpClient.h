// Author : Lance @ nwpu
#ifndef LanceNet_NET_TCPCLIENT_H
#define LanceNet_NET_TCPCLIENT_H

#include "LanceNet/base/StringPiece.h"
#include "LanceNet/base/Time.h"
#include "LanceNet/base/noncopyable.h"
#include "LanceNet/base/unix_wrappers.h"
#include "LanceNet/net/Buffer.h"
#include <functional>

namespace LanceNet
{
namespace net
{

class EventLoop;
class Connector;
class TcpConnection;

class TcpClient : noncopyable
{
public:
    using TcpConnectionPtr = std::shared_ptr<TcpConnection>;
    using OnMessageCb = std::function<void(const TcpConnectionPtr&, Buffer* buf, TimeStamp ts)>;
    using OnConnectionEstablishedCb = std::function<void(TcpConnectionPtr connPtr, int conn_fd, const SA_IN* peer_addr)>;
    using OnDisconnectedCb = OnConnectionEstablishedCb;

    using OnCloseCb = std::function<void(TcpConnectionPtr conn_ptr)>;

    TcpClient(int port, const std::string& serverName, EventLoop* loop);
    ~TcpClient();

    void start();

    void setOnConnectionCallback(OnConnectionEstablishedCb onconnectcb);
    void setOnMessageCallback(OnMessageCb onmessagecb);
    void setOnDisconnectedCb(OnDisconnectedCb ondisconnectedcb);

private:
    using ConnectorPtr = std::unique_ptr<Connector>;
    // callback used by Connector
    void OnRawConnection(int conn_sock);
    // used by TcpConnection 
    void removeConnection(const TcpConnectionPtr& conn);

    int port_;
    std::string serverName_;

    OnMessageCb messagecb_;
    OnConnectionEstablishedCb connectioncb_;
    OnDisconnectedCb disconnectcb_;
    OnCloseCb closecb_;

    EventLoop* loop_;
    ConnectorPtr connectorPtr_;
    TcpConnectionPtr tcpConnectionPtr_;
};

} // net
} // LanceNet

#endif // LanceNet_NET_TCPCLIENT_H
