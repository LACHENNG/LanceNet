
cmake_minimum_required(VERSION 3.0)

# set project name and version 
project(base VERSION 1.0)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)


set(SRC_LIST
    Condition.cpp
    CountDownLatch.cpp
    MutexLock.cpp
    MutexLockGuard.cpp
    ThisThread.cpp
    Thread.cpp
    unix_wrappers.cpp
)

add_library(${PROJECT_NAME} 
    ${SRC_LIST})

# target_include_directories(${PROJECT_NAME} 
#     INTERFACE
#     ${CMAKE_CURRENT_SOURCE_DIR}
# )

target_link_libraries(
    ${PROJECT_NAME}
    pthread
)

add_subdirectory(logging)
