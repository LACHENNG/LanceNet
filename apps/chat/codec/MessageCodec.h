// Author : Lance @ nwpu
//
// MessageCodec : an intermediate layer that performs the transformation of
// 'data arrive' event in lanceNet network lib
// to 'message arrive' event in application layer
//
// Deal with Packet Splitting and enconding automatically
//
#ifndef LanceNet_MESSAGECODEC_H
#define  LanceNet_MESSAGECODEC_H

#include "LanceNet/base/StringPiece.h"
#include "LanceNet/base/Time.h"
#include "LanceNet/base/noncopyable.h"
#include "LanceNet/net/TcpConnection.h"
#include "LanceNet/net/TcpServer.h"
#include <LanceNet/net/Buffer.h>

#include <functional>
#include <string>


class MessageCodec : LanceNet::noncopyable
{
public:
    using StringMessageCallback =
        std::function<void (const LanceNet::net::TcpConnectionPtr&,const std::string&,LanceNet::TimeStamp)>;

    explicit MessageCodec(const StringMessageCallback& cb);

    void send(const LanceNet::net::TcpConnectionPtr& conn,
              const LanceNet::StringPiece& message);

    void OnMessage(const LanceNet::net::TcpConnectionPtr& ,
                  LanceNet::net::Buffer*,
                  LanceNet::TimeStamp receiveTime);

private:
    StringMessageCallback m_messageCallback;
    const static size_t kHeaderLen = sizeof(int32_t);
};




#endif // LanceNet_MESSAGECODEC_H
