// Author : Lance @ nwpu
//
// ProtobufCodec : an intermediate layer that performs the transformation of
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
#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>
#include <memory>
#include <string>

namespace google
{
namespace protobuf
{
class Message;
}
}

namespace LanceNet
{
namespace net
{
typedef std::shared_ptr<google::protobuf::Message> MessagePtr;


class ProtobufCodec : public LanceNet::noncopyable
{
public:
    const static size_t kHeaderLen = sizeof(int32_t);
    
    using OnProtobufMessageCallback =
       std::function<void (const TcpConnectionPtr&, const google::protobuf::Message& ,TimeStamp)>;

    ProtobufCodec();

    void setOnProtobufMessageCallback(const OnProtobufMessageCallback& cb)
    { m_OnprotobufMessageCb = cb; }

    // send protobuf::Message
    void send(const LanceNet::net::TcpConnectionPtr& conn,
              const google::protobuf::Message& message);

    void OnRawBytes(const LanceNet::net::TcpConnectionPtr& ,
                  LanceNet::net::Buffer*,
                  LanceNet::TimeStamp receiveTime);
private:
    // encode message as a packet
    bool encodeToEmptyBuffer(Buffer* dest_buffer, const google::protobuf::Message& message);

    OnProtobufMessageCallback m_OnprotobufMessageCb;
};

// create (reflect) a message by TypeName and decode the 
// binary data(which previously serialized by protobuf::Message::serialzexxx) to fill the Message
// at protoData with lenght `size`
inline google::protobuf::Message* reflectAndFillProtoMessage(const StringPiece& msgType, const char* protoData, size_t size){
    using namespace google::protobuf;
    // using protobuf reflect to create Message
    google::protobuf::Message* message = nullptr;
    const auto descriptor =  google::protobuf::DescriptorPool::generated_pool()->FindMessageTypeByName(msgType.begin());
    if(descriptor){
        const Message* prototype =
            google::protobuf::MessageFactory::generated_factory()->GetPrototype(descriptor);
        if(prototype) message = prototype->New();
    }

    if(!message){
        return nullptr;
    }
    // fill Mesage
    bool f = message->ParseFromArray(protoData, size); 
    assert(f == true); (void) f;
    return message;
}

} // namespace LanceNet
} // namespace net

#endif // LanceNet_MESSAGECODEC_H
