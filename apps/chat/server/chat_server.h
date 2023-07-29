// Author : Lance @ npuw 

#ifndef LANCENET_CHATSERVER
#define LANCENET_CHATSERVER

#include "LanceNet/base/StringPiece.h"
#include "LanceNet/base/noncopyable.h"
#include <LanceNet/net/TcpServer.h>
#include <LanceNet/net/EventLoop.h>
#include <LanceNet/net/TcpConnection.h>
#include <unordered_set>


namespace google{
namespace protobuf
{
class Message;
}
}


namespace LanceNet {
namespace net{


// ChatServer implements a chat room that boardcast meessage which acts like a
// boardcast service in application layer
class ProtobufCodec;
class ChatServer : LanceNet::noncopyable
{
public:
    ChatServer(EventLoop* loop, int listen_port);
    ~ChatServer(); // force out-line to make unique_ptr happy

    void OnNewConnection(const TcpConnectionPtr& tcpConnPtr, int conn_fd, const SA_IN* peer_addr);
    void OnDisConnection(const TcpConnectionPtr& tcpConnPtr, int conn_fd, const SA_IN* peer_addr);
    void OnMessage(const TcpConnectionPtr& conn, const google::protobuf::Message& message ,LanceNet::TimeStamp);

    void start();
    void send(const TcpConnectionPtr& conn, const google::protobuf::Message& message);

private:
    using OnNewConnectionCb = std::function<void(TcpConnectionPtr tcpConnPtr, int conn_fd, const SA_IN* peer_addr)>;
    using OnMessageCb = std::function<void (const LanceNet::net::TcpConnectionPtr&, const google::protobuf::Message& ,LanceNet::TimeStamp)>;
    using ConnectionList = std::unordered_set<TcpConnectionPtr>;
    using ProtobufCodecUPtr = std::unique_ptr<ProtobufCodec>;

    EventLoop* m_loop;
    TcpServer m_server;
    ProtobufCodecUPtr m_codec;
    ConnectionList m_connections;
};

} // namespace LanceNet
} // namespace  net
  //
#endif // LANCENET_CHATSERVER
