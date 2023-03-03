
cmake_minimum_required(VERSION 3.0)

project(WebServer CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_INCLUDE_PATH ${CMAKE_SOURCE_DIR}/bin)
include_directories(${CMAKE_SOURCE_DIR}/src)

add_subdirectory(${CMAKE_SOURCE_DIR}/src)

add_executable(cltest apps/test_threadpool_sbuf.cpp)
# echo server/client without IO multiplexing
add_executable(echo_server apps/echo/server.cpp)
add_executable(echo_client apps/echo/client.cpp)
# echo server/client with select
add_executable(client_select apps/echo/client_select.cpp)
add_executable(server_select apps/echo/server_select.cpp)



target_link_libraries(cltest wrappers pthread)
target_link_libraries(echo_server wrappers pthread)
target_link_libraries(echo_client wrappers pthread)
target_link_libraries(client_select wrappers pthread)
target_link_libraries(server_select wrappers pthread)
# target_link_directories(main PRIVATE ${CMAKE_BINARY_DIR}/src

# install(TARGETS cltest )