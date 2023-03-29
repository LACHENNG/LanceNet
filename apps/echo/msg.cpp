/*
 * Description：A simple message protocol in application layer suport Client login message and
                Logout message and Server  Login response Msg and LogOut Response Msg
                Also a general msg type to test 
 * Author: Lang@nwpu
 */
#include "msg.h"

class MsgHeader{
public:
    MsgHeader(size_t _msgLen, MSG_Type mtype) : msgLen(_msgLen){

    }
    size_t msgLen;
    MSG_Type msgtype;
};

class MsgLogin : public MsgHeader{
public:
    MsgLogin() : MsgHeader(sizeof(MsgLogin), LOGIN_REQUEST){

    }
    char uName[32]{0};
    char pwd[32]{0};
    char data[DATA_BUF_SIZE];
};

class MsgLogout : public MsgHeader{
public:
    MsgLogout() : MsgHeader(sizeof(MsgLogin), LOGOUT_REQUEST){

    }
    char uName[32]{0};
    char pwd[32]{0};
};


class MsgLoginSucResponse : MsgHeader{
public:
    MsgLoginSucResponse() : MsgHeader(sizeof(MsgLoginSucResponse), LOGIN_SUCCESS){

    }
    char data[DATA_BUF_SIZE];
};

class MsgLogoutSucResponse : MsgHeader{
public:
    MsgLogoutSucResponse() : MsgHeader(sizeof(MsgLogoutSucResponse), LOGOUT_SUCCESS){

    }
    char data[DATA_BUF_SIZE];
};


