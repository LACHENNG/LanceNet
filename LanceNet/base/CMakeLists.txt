set(LIB_NAME LanceNet_base)

set(SRC_LIST
    Condition.cpp
    CountDownLatch.cpp
    MutexLock.cpp
    MutexLockGuard.cpp
    ThisThread.cpp
    Thread.cpp
    unix_wrappers.cpp
    StringPiece.cpp
    Time.cpp

    ColorText.cpp
    LogStream.cpp
    Logging.cpp
)

add_library(
    ${LIB_NAME}
    ${SRC_LIST}
)

# target_include_directories(${PROJECT_NAME}
#     INTERFACE
#     ${CMAKE_CURRENT_SOURCE_DIR}
# )

target_link_libraries(
    ${LIB_NAME}
    PUBLIC
    pthread
    PRIVATE
    LanceNet_net
)


# add_subdirectory(logging)


