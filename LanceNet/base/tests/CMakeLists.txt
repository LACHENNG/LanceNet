


add_executable(logging_test ./Logging_test.cpp)
add_executable(monitor_test ./monitorTest.cpp)

add_executable(logstream_test ./logstream_test.cpp)

target_link_libraries( logging_test
    LanceNet_base
    LanceNet_net
)

target_link_libraries( logstream_test 
    LanceNet_base
    LanceNet_net
)
target_link_libraries( monitor_test
    LanceNet_base
    LanceNet_net
)
