/* a wrapper of connected fd `fdsession` which represents a specific communication context with clients
 * Author: Lang@nwpu
 * Date: 2023/3/11
*/

#pragma once 
#include <memory>
#include <stdio.h>

#define BUF_SIZE 4096
#define MSG_BUF BUF_SIZE*10

class ClientEvent{
public:
    virtual void OnMessage() = 0;
    virtual void OnExit() = 0;
};



class ClientSession : public ClientEvent{
public:
    typedef int element_type;
    ClientSession(int clientfd, void * pServer);
    virtual void OnMessage();
    virtual void OnExit();

    int getFd() const;
    void * getPServer();

    bool operator<(const ClientSession* rhs){
        return m_clientfd < rhs->getFd();
    }

private:
    void * m_pServer;
    int m_clientfd;
    char m_upszBuf[BUF_SIZE];
    char m_upMsgBuf[MSG_BUF]; 
};

