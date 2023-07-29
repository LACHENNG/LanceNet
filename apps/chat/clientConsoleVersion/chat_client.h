
// Author : Lance @ npuw

#ifndef LANCENET_CHATCLIENT_H
#define LANCENET_CHATCLIENT_H

#include "LanceNet/base/noncopyable.h"
#include <LanceNet/net/TcpClient.h>
#include <LanceNet/net/EventLoop.h>
#include <LanceNet/net/TcpConnection.h>
#include <unordered_set>
#include "../codec/MessageCodec.h"

using namespace LanceNet;
using namespace LanceNet::net;

// ChatClient enter the chat room and send message to all the online-users 
//
class ChatClient : LanceNet::noncopyable
{
public:
    using OnNewConnectionCb = std::function<void(TcpConnectionPtr connPtr, int conn_fd, const SA_IN* peer_addr)>;

    ChatClient(EventLoop* loop, const StringPiece& server_addr, int port);

    void OnNewConnection(const TcpConnectionPtr& tcpConnPtr, int conn_fd, const SA_IN* peer_addr);
    void OnDisConnection(const TcpConnectionPtr& tcpConnPtr, int conn_fd, const SA_IN* peer_addr);

    void OnMessage(const TcpConnectionPtr& conn, const google::protobuf::Message& message, LanceNet::TimeStamp);

    void setOnConnectedCb(const OnNewConnectionCb& cb){ m_onNewConnectionCb = cb;}

    // MT-safe
    void send(const google::protobuf::Message& message);
    // unsafe 
    void start();

    TcpConnectionPtr connection() const {return m_connection;}

private:
    EventLoop* m_loop;
    TcpClient m_client;
    ProtobufCodec m_codec;
    TcpConnectionPtr m_connection;
    OnNewConnectionCb m_onNewConnectionCb;
};

#endif // LANCENET_CHATCLIENT_H
