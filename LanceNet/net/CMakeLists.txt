
find_package(Boost COMPONENTS system filesystem unit_test_framework REQUIRED)
# include_directories ($ {Boost_INCLUDE_DIRS})

message("====================================")
message("${boost_unit_test_framework_library}")
message("====================================")

set(SRC_LIST
    Buffer.cpp
    EventLoop.cpp
    EventLoopThread.cpp
    IOMultiplexer.cpp

    IOMultiplexer/DefaultIOMultiplexer.cpp
    IOMultiplexer/Epoller.cpp
    IOMultiplexer/Poller.cpp
    IOMultiplexer/Selecter.cpp
    IOMultiplexer/PollerX.cpp
    FdChannel.cpp

    Timer.cpp
    TimerId.cpp
    TimerQueue.cpp

    Acceptor.cpp
    TcpServer.cpp
    TcpConnection.cpp

    EventLoopPool.cpp

    Connector.cpp
    TcpClient.cpp
)

add_library(LanceNet_net ${SRC_LIST})
target_link_libraries(LanceNet_net LanceNet_base)

add_subdirectory(tests)

