

add_executable(echo_server ./echo_server.cpp)
add_executable(echo_client ./echo_client.cpp)


target_link_libraries(echo_client PRIVATE
    LanceNet_base
    LanceNet_net
)

target_link_libraries(echo_server PRIVATE
    LanceNet_base
    LanceNet_net
)
