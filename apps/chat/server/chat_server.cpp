#include "chat_server.h"
#include "LanceNet/net/Buffer.h"
#include "LanceNet/net/EventLoop.h"
#include "LanceNet/base/Logging.h"
#include "apps/chat/codec/MessageCodec.h"
#include <boost/test/tools/assertion.hpp>
#include <functional>
#include <cassert>
#include <google/protobuf/message.h>
#include <netdb.h>
#include "../codec/msg.pb.h"

using namespace std::placeholders;

namespace LanceNet {
namespace net {

ChatServer::ChatServer(EventLoop* loop, int listen_port)
  : m_loop(loop),
    m_server(loop, listen_port),
    m_codec(new ProtobufCodec())
{
    m_server.setOnDissconnectionCb(std::bind(&ChatServer::OnDisConnection, this, _1, _2, _3));
    m_server.setOnNewConnectionCb(std::bind(&ChatServer::OnNewConnection, this, _1, _2, _3));

    m_server.setOnMessageCb([this](const TcpConnectionPtr& conn, Buffer* buf,TimeStamp ts){
        m_codec->OnRawBytes(conn, buf, ts);
    });
    m_codec->setOnProtobufMessageCallback([this](const TcpConnectionPtr &conn, const google::protobuf::Message &message, TimeStamp ts){
       this->OnMessage(conn, message, ts);
    });
}

ChatServer::~ChatServer()
{
}

void ChatServer::OnMessage(const TcpConnectionPtr& conn,const google::protobuf::Message& message, TimeStamp ts)
{
    for(const auto &client : m_connections){
        // if(client != conn)
        if(message.GetDescriptor() == DialogMessage::GetDescriptor())
        {
            const DialogMessage *dialog = dynamic_cast<const DialogMessage*>(&message);
            LOG_INFO << "username: " << dialog->username() << "\n" 
                     << "msg type: " << dialog->type() << "\n" 
                     << "content: " << dialog->content();
            m_codec->send(client, message);
        }
        else{
            LOG_WARNC << "unrecognized message type";
        }
    }
}

void ChatServer::start()
{
    m_server.start();
}


void ChatServer::send(const TcpConnectionPtr& conn, const google::protobuf::Message& message)
{
    m_codec->send(conn, message);
}

void ChatServer::OnNewConnection(const TcpConnectionPtr& tcpConnPtr, int conn_fd, const SA_IN* peer_addr)
{
    LOG_DEBUGC << "got new connection fd =  " << conn_fd;
    assert(m_connections.find(tcpConnPtr) == m_connections.end());
    m_connections.insert(tcpConnPtr);
}

void ChatServer::OnDisConnection(const TcpConnectionPtr &tcpConnPtr, int conn_fd, const SA_IN* peer_addr)
{
    LOG_DEBUG << "disconnection fd =  " << conn_fd;
    assert(m_connections.find(tcpConnPtr) != m_connections.end());
    m_connections.erase(tcpConnPtr);
}


} // namespace LanceNet
} // namespace net


