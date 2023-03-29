/*
 * Description：A simple message protocol in application layer suport Client login message and
                Logout message and Server  Login response Msg and LogOut Response Msg
                Also a general msg type to test 
 * Author: Lang@nwpu
 */
#pragma once 
#include <stdlib.h>
#define DATA_BUF_SIZE 1024

enum MSG_Type{
    LOGIN_REQUEST = 0,
    LOGOUT_REQUEST, 
    LOGIN_SUCCESS,
    LOGOUT_SUCCESS,
};

class MsgHeader{
public:
    MsgHeader(size_t _msgLen, MSG_Type mtype) : msgLen(_msgLen){

    }
    size_t getPackSize(){
        return msgLen;
    }
    MSG_Type getMsgType(){
        return msgtype;
    }
private:
    size_t msgLen;
    MSG_Type msgtype;
};

class MsgLoginReq : public MsgHeader{
public:
    MsgLoginReq() : MsgHeader(sizeof(MsgLoginReq), LOGIN_REQUEST){

    }
    char uName[32]{0};
    char pwd[32]{0};
    char data[DATA_BUF_SIZE];
};

class MsgLogoutReq : public MsgHeader{
public:
    MsgLogoutReq() : MsgHeader(sizeof(MsgLogoutReq), LOGOUT_REQUEST){

    }
    char uName[32]{0};
    char pwd[32]{0};
};


class MsgLoginSucResponse : MsgHeader{
public:
    MsgLoginSucResponse();
    char data[DATA_BUF_SIZE];
};

class MsgLogoutSucResponse : MsgHeader{
public:
    MsgLogoutSucResponse() ;
    char data[DATA_BUF_SIZE];
};


