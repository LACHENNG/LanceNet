// Author : Lance @ npuw 

#ifndef LANCENET_CHATSERVER
#define LANCENET_CHATSERVER

#include "LanceNet/base/StringPiece.h"
#include "LanceNet/base/noncopyable.h"
#include <LanceNet/net/TcpServer.h>
#include <LanceNet/net/EventLoop.h>
#include <LanceNet/net/TcpConnection.h>
#include <unordered_set>
#include "../codec/MessageCodec.h"

using namespace LanceNet;
using namespace LanceNet::net;

// ChatServer implements a chat room that boardcast meessage which acts like a
// boardcast service in application layer
//
class ChatServer : LanceNet::noncopyable
{
public:
    using TcpConnectionPtr = TcpConnection::TcpConnectionPtr;
    using OnNewConnectionCb = std::function<void(TcpConnectionPtr tcpConnPtr, int conn_fd, const SA_IN* peer_addr)>;
    using OnStringMessageCb = MessageCodec::StringMessageCallback; 

    ChatServer(EventLoop* loop, int listen_port);

    void OnNewConnection(const TcpConnectionPtr& tcpConnPtr, int conn_fd, const SA_IN* peer_addr);
    void OnDisConnection(const TcpConnectionPtr& tcpConnPtr, int conn_fd, const SA_IN* peer_addr);
    void OnStringMessage(const LanceNet::net::TcpConnectionPtr&,const std::string&,LanceNet::TimeStamp);

    void start();
    void send(const StringPiece& msg) {}

private:
    using ConnectionList = std::unordered_set<TcpConnectionPtr>;
    EventLoop* m_loop;
    TcpServer m_server;
    MessageCodec codec;
    ConnectionList m_connections;
};

#endif // LANCENET_CHATSERVER
