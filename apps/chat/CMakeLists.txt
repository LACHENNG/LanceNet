cmake_minimum_required(VERSION 3.0)

find_package(Protobuf 3.20.0 REQUIRED)
include_directories(${PROTOBUF_INCLUDE_DIR})

set(SRC_LIST
    codec/MessageCodec.cpp
    codec/msg.pb.cc
    codec/Packet.cpp
   
    clientConsoleVersion/chat_client.cpp
    server/chat_server.cpp
)


add_executable(chat_server server/main_chat_server.cpp ${SRC_LIST} )
add_executable(chat_client clientConsoleVersion/main_chat_client.cpp ${SRC_LIST} )

target_link_libraries(chat_server PRIVATE
                                  LanceNet_base LanceNet_net ${PROTOBUF_LIBRARY})
target_link_libraries(chat_client LanceNet_base LanceNet_net ${PROTOBUF_LIBRARY})

add_executable(packet_unittest ./codec/test/Packet_unittest.cpp ${SRC_LIST}  )
target_link_libraries(packet_unittest LanceNet_base LanceNet_net ${PROTOBUF_LIBRARY}  boost_unit_test_framework)
