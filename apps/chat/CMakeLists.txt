cmake_minimum_required(VERSION 3.0)

set(SRC_LIST
    codec/MessageCodec.cpp
    server/chat_server.cpp
    ./client/chat_client.cpp
)


add_executable(chat_server server/main_chat_server.cpp ${SRC_LIST} )
add_executable(chat_client client/main_chat_client.cpp ${SRC_LIST} )

target_link_libraries(chat_server LanceNet_base LanceNet_net )
target_link_libraries(chat_client LanceNet_base LanceNet_net )


