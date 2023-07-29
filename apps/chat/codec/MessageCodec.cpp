#include "MessageCodec.h"
#include "LanceNet/base/Logging.h"
#include "apps/chat/codec/Packet.h"
#include "apps/chat/codec/msg.pb.h"
#include <cstdint>
#include <google/protobuf/descriptor.h>
#include <cassert>
#include <google/protobuf/message.h>

namespace LanceNet{
namespace net{

ProtobufCodec::ProtobufCodec()
{
}

void ProtobufCodec::send(const LanceNet::net::TcpConnectionPtr &conn, const google::protobuf::Message& message)
{
    LanceNet::net::Buffer buffer;
    encodeToEmptyBuffer(&buffer, message);
    conn->send(&buffer);
}

void ProtobufCodec::OnRawBytes(const TcpConnectionPtr& conn, Buffer* buf, TimeStamp receiveTime)
{
    using namespace google::protobuf;
    while(Packet::canParseFromArray(buf->peek(), buf->readableBytes())){
        Packet packet = packet.parseFromArray(buf->peek(), buf->readableBytes());

        // get message type and  protoMesData
        const char* msgType = packet.messageTypeName();
        const char* protoData = packet.payload();
        Message* message = reflectAndFillProtoMessage(msgType, protoData, packet.payloadLen());
        if(m_OnprotobufMessageCb){
            m_OnprotobufMessageCb(conn, *message, receiveTime);
        }else{
            LOG_WARNC << "MessageCallback not set, ProtobufCodec discard received message";
        }

        buf->retrieve(packet.byteSizeAll());
    }
}

bool ProtobufCodec::encodeToEmptyBuffer(Buffer* dest_buffer, const google::protobuf::Message& message)
{
    assert(dest_buffer && dest_buffer->readableBytes() == 0);
    dest_buffer->ensureWriteableSpace(Packet::bytesAllToPack(message));

    bool suc = Packet::packProtoMessageToCachedSizeArray(const_cast<char*>(dest_buffer->peek()), dest_buffer->writeableBytes(), message);
    if(suc) dest_buffer->hasWritten(Packet::bytesAllToPack(message));
    return suc;
}

} // namespace LanceNet
} // namespace net
