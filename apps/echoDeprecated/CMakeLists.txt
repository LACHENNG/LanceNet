
aux_source_directory(. src)
message("echo : ${src}")


add_executable(client client.cpp)
add_executable(client_with_msg client_with_msg.cpp)
add_executable(server server.cpp  echoserver.cpp  clientsession.cpp)

target_link_libraries(client wrappers pthread)
target_link_libraries(server wrappers pthread)
target_link_libraries(client_with_msg wrappers pthread)