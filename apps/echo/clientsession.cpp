/* a wrapper of connected m_clientfd `m_clientfdsession` which represents a specific 
   communication context with clients
*/
#include "clientsession.h"
#include "echoserver.h"
#include <unistd.h>
#include <thread>
#include <iostream>
#include <stdio.h>
#include <functional>



ClientSession::ClientSession(int clientfd, void * pServer): 
    m_clientfd(clientfd), m_pServer(pServer)

{

}
void ClientSession::OnMessage(){
    
    int n = read(m_clientfd, m_upszBuf, BUF_SIZE);
    /* for nonblocking socket m_clientfd, n == 0 mean peer closed the connection*/
    if(n == 0){
        OnExit();
    }
    else if(n == -1){
        if(errno==ECONNRESET){
            OnExit();
        }

    }else if(n > 0){
        auto addBytesToServer = std::mem_fn(&EchoServer::addRecvBytes);
        addBytesToServer((EchoServer*)m_pServer, n);
        // fprintf(stdout, "from %d : %s\n", m_clientfd, m_upszBuf);
        // int rv =write(m_clientfd, m_upszBuf, n); /* issue: race condition  */ 
        // if(rv == -1){
        //     perror("write");
        // }
    }      
}
void ClientSession::OnExit(){
    auto pOnExit = std::mem_fn(&EchoServer::OnClientExit);
    pOnExit((EchoServer*)m_pServer, this);
}

int ClientSession::getFd() const{
    return m_clientfd;
}
void * ClientSession::getPServer(){
    return m_pServer;
}
