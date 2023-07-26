#include "chat_client.h"
#include "LanceNet/base/StringPiece.h"
#include "LanceNet/net/EventLoop.h"
#include <LanceNet/base/Logging.h>

#include "LanceNet/net/TcpClient.h"
#include <cassert>
#include <iostream>

using namespace std::placeholders;
ChatClient::ChatClient(EventLoop* loop, const StringPiece& server_addr, int port)
  : m_loop(loop),
    m_client(port, server_addr.as_string(), loop),
    m_codec(std::bind(&ChatClient::OnStringMessage, this, _1, _2, _3))
{

    m_client.setOnMessageCallback(std::bind(&MessageCodec::OnMessage, &m_codec, _1, _2, _3));
    m_client.setOnConnectionCallback(std::bind(&ChatClient::OnNewConnection, this, _1, _2, _3));

    m_client.setOnDisconnectedCb(std::bind(&ChatClient::OnDisConnection, this, _1, _2, _3));
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

void ChatClient::OnStringMessage(const LanceNet::net::TcpConnectionPtr& conn,const std::string& msg,LanceNet::TimeStamp)
{
    assert(m_connection == conn);
    std::cout << msg << std::endl;
}

void ChatClient::send(const StringPiece& msg)
{
    // connection not established yet
    assert(m_connection);
    m_codec.send(m_connection, msg);
}

void ChatClient::start()
{
    m_client.start();
}
