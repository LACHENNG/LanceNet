
find_package(Boost COMPONENTS system filesystem unit_test_framework REQUIRED)
# include_directories ($ {Boost_INCLUDE_DIRS})

message("====================================")
message("${boost_unit_test_framework_library}")
message("====================================")

set(SRC_LIST
    Buffer.cpp
    EventLoop.cpp
    IOMultiplexer.cpp
    SockChannel.cpp
)

add_library(LanceNet_net ${SRC_LIST})
target_link_libraries(LanceNet_net LanceNet_base)

add_subdirectory(tests)

