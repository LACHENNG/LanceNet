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
#include "msg.h"



ClientSession::ClientSession(int clientfd, void * pServer): 
    m_pServer(pServer), m_clientfd(clientfd)

{

}

void ClientSession::doWork(){
    OnMessage();
}

void ClientSession::OnMessage(){
    int n = read(m_clientfd, m_szBuf, BUF_SIZE); 
    /* for nonblocking socket m_clientfd, n == 0 mean peer closed the connection*/
    if(n == 0 || (n == -1 && errno == ECONNRESET)){
        OnExit();
        return;
    }
  
    memcpy(m_MsgBuf + _msgPosEnd, m_szBuf, n);
    _msgPosEnd += n;

    /* update statistics to server */
    // auto addBytesToServer = std::mem_fn(&EchoServer::addRecvBytes);
    // addBytesToServer((EchoServer*)m_pServer, n);

    /* parse zero, one or more messages */
    while(1){       
        assert(_msgPosEnd >= _msgPosCur);
        size_t data_len = _msgPosEnd - _msgPosCur;     
        if(data_len < sizeof(MsgHeader)){
            break;
        }
        else if(data_len >= sizeof(MsgHeader)){
            MsgHeader* header = (MsgHeader*) (m_szBuf + _msgPosCur);

            size_t szPackage = header->getPackSize();
            /* no enough data to form a message */
            if(data_len < szPackage){
                break;
            }
            else if(data_len >= szPackage){
                /* process message at buf[_msgPosCur, _msgPosCur + szPackage]*/
                switch (header->getMsgType())
                {
                case LOGIN_REQUEST:
                {
                    MsgLoginReq* login = (MsgLoginReq*) header;
                    /* code */
                    printf("\033[37;32m User: %s Pwd: %s Login\n", login->uName, login->pwd);
                    break;
                }
                   
                case LOGOUT_REQUEST:
                case LOGIN_SUCCESS:
                case LOGOUT_SUCCESS:
                default: 
                    assert(0);
                    break;
                }
                _msgPosCur += szPackage;
                /* memory move data to the begin of the buffer */
                if(_msgPosCur > MSG_BUF_SIZE * 0.8){
                    memmove(m_MsgBuf, m_MsgBuf + _msgPosCur, _msgPosEnd - _msgPosCur);
                    _msgPosEnd -= _msgPosCur;
                    _msgPosCur = 0;   
                    printf("\033[37;41m INFO\033[0m: memory moved\n");
                }
            }
        }
    }
}
void ClientSession::OnExit(){
    auto pOnExit = std::mem_fn(&EchoServer::OnClientExit);
    pOnExit((EchoServer*)m_pServer, this);
}

int ClientSession::getFd() const{
    return m_clientfd;
}
void * ClientSession::getServerPtr(){
    return m_pServer;
}
