#include <LanceNet/net/Buffer.h>
#include <boost/test/tools/old/interface.hpp>
#include <google/protobuf/message.h>
#include "../Packet.h"
#include "../msg.pb.h"
#include "apps/chat/codec/MessageCodec.h"
#include <string>

//#define BOOST_TEST_MODULE BufferTest
#define BOOST_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

using namespace LanceNet;
using namespace net;
using LanceNet::net::Buffer;
using namespace std;

BOOST_AUTO_TEST_CASE(testPacketSerializeDeserialize)
{
    string username = "Lance@nwpu";
    string content = "I am lance";
    LanceNet::DialogMessage message;
    message.set_username(username);
    message.set_type(1);
    message.set_content(content);
    int protoDatalen = message.ByteSizeLong();
    LOG_INFOC << "message.ByteSizeLong() " << protoDatalen;
    int packetLen = Packet::kCheckSumLen + Packet::kHeaderLen + Packet::kMsgNameLen + message.GetTypeName().size() + 1 + protoDatalen;

    
    // serialize message  with failure
    Buffer buffer(1, 16);
    buffer.ensureWriteableSpace(packetLen - 1);
    BOOST_CHECK_GE(buffer.writeableBytes(), packetLen - 1);
    BOOST_CHECK_EQUAL(message.GetTypeName(), "LanceNet.DialogMessage");
    bool f =Packet::packProtoMessageToCachedSizeArray(const_cast<char*>(buffer.peek()), buffer.writeableBytes(), message);
    BOOST_CHECK_EQUAL(f, false);

    // serialize message with success
    buffer.ensureWriteableSpace(packetLen);
    BOOST_CHECK_EQUAL(buffer.writeableBytes(), packetLen);
    BOOST_CHECK_EQUAL(message.GetTypeName(), "LanceNet.DialogMessage");
    f = Packet::packProtoMessageToCachedSizeArray(const_cast<char*>(buffer.peek()), buffer.writeableBytes(), message);
    BOOST_CHECK_EQUAL(f, true);
    if(f) buffer.hasWritten(packetLen);

    BOOST_CHECK_EQUAL(buffer.readableBytes(), packetLen);


    // deserialized packet
    //
    Packet packet = Packet::parseFromArray(buffer.peek(), buffer.readableBytes());
    BOOST_CHECK_EQUAL(packet.headerLen(), packetLen - Packet::kHeaderLen);
    BOOST_CHECK_EQUAL(packet.typeNameLen(), message.GetTypeName().size());
    BOOST_CHECK_EQUAL(packet.payloadLen(), message.ByteSizeLong());
    BOOST_CHECK_EQUAL(packet.byteSizeAll(), packetLen);
    BOOST_CHECK_EQUAL(string(packet.messageTypeName()), message.GetTypeName());
    
    // deserialized message
    //
    const char* protodata = packet.payload();
    const int len = packet.payloadLen();
    google::protobuf::Message* message2 = createProtoMessage(message.GetTypeName(), protodata, len);

    BOOST_CHECK_EQUAL(message2->GetTypeName(), message.GetTypeName());
    BOOST_CHECK_EQUAL(message2->GetDescriptor(), message.GetDescriptor());
    DialogMessage *dialogMessage2 = dynamic_cast<DialogMessage*>(message2);
    BOOST_CHECK_EQUAL(dialogMessage2->username(), message.username());
    BOOST_CHECK_EQUAL(dialogMessage2->type(), message.type());
    BOOST_CHECK_EQUAL(dialogMessage2->content(), message.content());
}

