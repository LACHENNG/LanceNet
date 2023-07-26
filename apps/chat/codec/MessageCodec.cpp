#include "MessageCodec.h"
#include "LanceNet/base/Logging.h"
#include <bits/stdint-uintn.h>
#include <cstdint>
#include <netinet/in.h>
#include <cassert>

MessageCodec::MessageCodec(const StringMessageCallback& cb)
{
    m_messageCallback = cb;
}

void MessageCodec::send(const LanceNet::net::TcpConnectionPtr& conn, 
          const LanceNet::StringPiece& message)
{
    LanceNet::net::Buffer buffer;
    buffer.append(message.data(), message.size());
    int32_t len = static_cast<int32_t>(message.size());
    int32_t len_be32 = htonl(len);
    LOG_INFOC << "preprend origin: " << len << " ";
    buffer.preappend(len_be32);
    LOG_INFOC << "preprend be32: " << len_be32 << " ";
    assert(buffer.readableBytes() == kHeaderLen + message.size());
    conn->send(buffer.retrieveAllAsString());

}

void MessageCodec::OnMessage(const LanceNet::net::TcpConnectionPtr& conn,
               LanceNet::net::Buffer* buf,
               LanceNet::TimeStamp receiveTime)
{
    while(buf->readableBytes() >= kHeaderLen) 
    {

        LOG_TRACE << "msg be32len " << buf->peekInt32();
        int msg_len = ntohl(buf->peekInt32());
        LOG_TRACE << "decoded data len " << msg_len;
        // FIXME: current limit msg len to ushort_max 
        if (msg_len < 0 || msg_len > UINT16_MAX) {
            LOG_WARNC << "Invalid Length with header len = " << msg_len;
            conn->shutdown();
            break;
        }
        if (buf->readableBytes() + kHeaderLen < static_cast<size_t>(msg_len)) {
             break;
        }
        buf->retrieveInt32();
        std::string message = buf->retrieveAsString(msg_len);

        LOG_DEBUGC << "decoded msg len " << message.size();
        // received a complete msg 
        if(m_messageCallback){
            m_messageCallback(conn, message, receiveTime);
        }else{
            LOG_WARNC << "MessageCallback not set, MessageCodec discard received message";
        }
    }
}
