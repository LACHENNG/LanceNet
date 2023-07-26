#include "chat_server.h"
#include "LanceNet/net/EventLoop.h"
#include "LanceNet/base/Logging.h"
#include <functional>
#include <cassert>
#include <netdb.h>

using namespace std::placeholders;

ChatServer::ChatServer(EventLoop* loop, int listen_port)
  : m_loop(loop),
    m_server(loop, listen_port),
    codec(std::bind(&ChatServer::OnStringMessage, this, _1,_2,_3))
{
    m_server.setOnDissconnectionCb(std::bind(&ChatServer::OnDisConnection, this, _1, _2, _3));
    m_server.setOnNewConnectionCb(std::bind(&ChatServer::OnNewConnection, this, _1, _2, _3));
    m_server.setOnMessageCb(std::bind(&MessageCodec::OnMessage, &codec, _1, _2,_3));
}

void ChatServer::OnStringMessage(const LanceNet::net::TcpConnectionPtr& conn,const std::string& msg ,LanceNet::TimeStamp)
{
   // LOG_INFOC << "server received form " << conn->name() << " msg : " << msg << " connections.size() = " << m_connections.size();
    for(const auto &client : m_connections){
        // do not send back to message sender itself
        if(client != conn)
            codec.send(client, msg);
    }
}

void ChatServer::start()
{
    m_server.start(); 
}
void ChatServer::OnNewConnection(const TcpConnectionPtr& tcpConnPtr, int conn_fd, const SA_IN* peer_addr)
{

    LOG_INFOC << "got new connection fd =  " << conn_fd;
    assert(m_connections.find(tcpConnPtr) == m_connections.end());
    m_connections.insert(tcpConnPtr);
}

void ChatServer::OnDisConnection(const TcpConnectionPtr &tcpConnPtr, int conn_fd, const SA_IN* peer_addr)
{
    LOG_INFOC << "disconnection fd =  " << conn_fd;
    assert(m_connections.find(tcpConnPtr) != m_connections.end());
    m_connections.erase(tcpConnPtr);
}


