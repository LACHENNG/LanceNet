/* a wrapper of connected fd `fdsession` which represents a specific communication context with clients
 * Author: Lang@nwpu
 * Date: 2023/3/11
*/

#pragma once 
#include <memory>
#include <stdio.h>

#define BUF_SIZE 4096
#define MSG_BUF_SIZE BUF_SIZE*10

class ClientEvent{
public:
    virtual void OnMessage() = 0;
    virtual void OnExit() = 0;
};

class ITaskType{
public:
    virtual  void doWork() = 0;
};

class ClientSession : public ClientEvent, public ITaskType{
public:
    typedef int element_type;
    ClientSession(int clientfd, void * pServer);

    virtual void doWork() override;
    void OnMessage() override;
    void OnExit() override;

    int getFd() const;
    void * getServerPtr();

    bool operator<(const ClientSession* rhs){
        return m_clientfd < rhs->getFd();
    }

private:
    void* m_pServer;
    int m_clientfd;
    /* buffer of client messages */
    char m_szBuf[BUF_SIZE];
    char m_MsgBuf[MSG_BUF_SIZE]; 
    /* the next avaliable mem pos in msg buf*/
    size_t _msgPosEnd{0};
    /* the next byte to be processed */
    size_t _msgPosCur{0};
};

