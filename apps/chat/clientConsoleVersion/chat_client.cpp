#include "chat_client.h"
#include "LanceNet/base/StringPiece.h"
#include "LanceNet/net/EventLoop.h"
#include <LanceNet/base/Logging.h>
#include <google/protobuf/message.h>
#include "LanceNet/net/TcpClient.h"
#include "../codec/msg.pb.h"
#include <cassert>
#include <iostream>

using namespace std::placeholders;
ChatClient::ChatClient(EventLoop* loop, const StringPiece& server_addr, int port)
  : m_loop(loop),
    m_client(port, server_addr.as_string(), loop)
{

    m_client.setOnConnectionCallback(std::bind(&ChatClient::OnNewConnection, this, _1, _2, _3));

    m_client.setOnDisconnectedCb(std::bind(&ChatClient::OnDisConnection, this, _1, _2, _3));
    
    m_client.setOnMessageCallback([this](const TcpConnectionPtr& conn, Buffer* buf,TimeStamp ts){
        m_codec.OnRawBytes(conn, buf, ts);
    });
    m_codec.setOnProtobufMessageCallback([this](const LanceNet::net::TcpConnectionPtr &conn, const google::protobuf::Message &message, LanceNet::TimeStamp ts){
        this->OnMessage(conn, message, ts);
    });
}

void ChatClient::OnNewConnection(const TcpConnectionPtr& tcpConnPtr, int conn_fd, const SA_IN* peer_addr)
{
    m_connection = tcpConnPtr;
    if(m_onNewConnectionCb){
        m_onNewConnectionCb(tcpConnPtr, conn_fd, peer_addr);
    }
}

void ChatClient::OnDisConnection(const TcpConnectionPtr& tcpConnPtr, int conn_fd, const SA_IN* peer_addr)
{
    m_connection.reset();
}

void ChatClient::OnMessage(const TcpConnectionPtr& conn, const google::protobuf::Message& message ,LanceNet::TimeStamp ts)
{
    if(message.GetDescriptor() == DialogMessage::GetDescriptor()){
        const DialogMessage *dialog = dynamic_cast<const DialogMessage*>(&message);
        LOG_INFO << "username: " << dialog->username() << "\n" 
                 << "msg type: " << dialog->type() << "\n" 
                 << "content: " << dialog->content();
    }
    else{
        LOG_WARNC << "unrecognized message type";
    }
}

void ChatClient::send(const google::protobuf::Message& message)
{
    // connection not established yet
    assert(m_connection);
    m_codec.send(m_connection, message);
}

void ChatClient::start()
{
    m_client.start();
}
