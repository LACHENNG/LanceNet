

add_executable(echo_server echo/echo_server.cpp)
add_executable(echo_client echo/echo_client.cpp)
add_executable(daytime_server ./daytime.cpp)


target_link_libraries(echo_client PRIVATE
    LanceNet_base
    LanceNet_net
)

target_link_libraries(echo_server PRIVATE
    LanceNet_base
    LanceNet_net
)

target_link_libraries(daytime_server PRIVATE
    LanceNet_base
    LanceNet_net
)

add_subdirectory(./chat/)

