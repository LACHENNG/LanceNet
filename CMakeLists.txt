
cmake_minimum_required(VERSION 3.0)
project(WebServer CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_INCLUDE_PATH ${CMAKE_SOURCE_DIR}/bin)

include_directories(${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/apps/echo)


add_subdirectory(${CMAKE_SOURCE_DIR}/apps/test)
add_subdirectory(${CMAKE_SOURCE_DIR}/src)
add_subdirectory(${CMAKE_SOURCE_DIR}/apps/echo)



# install(TARGETS cltest )