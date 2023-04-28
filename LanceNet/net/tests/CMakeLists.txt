
add_executable(buffer_test Buffer_test.cpp)

add_executable(loop_test event001.cpp)
add_executable(loop_test2 event002.cpp)
add_executable(loop_test3 event003.cpp)
add_executable(fdChanneltest fdChanneltest.cpp)
add_executable(eventLooptest eventLooptestwithTimerfd.cpp)
add_executable(runinlooptest runInLoop_test.cpp)
add_executable(EventLoopThreadtest EventLoopThreadtest.cpp)
#add_executable(loop_test event001.cpp)



target_link_libraries(buffer_test PRIVATE
    LanceNet_net
    LanceNet_base
    ${Boost_FILESYSTEM_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
    ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
)

target_link_libraries(loop_test PRIVATE
    # LanceNet_net
    # LanceNet_base
    # LanceNet_log
    # LanceNet_net
    # LanceNet_base

    LanceNet_net
    LanceNet_base
)

target_link_libraries(loop_test2 PRIVATE
    LanceNet_net
    LanceNet_base
)

target_link_libraries(loop_test3 PRIVATE
    LanceNet_net
    LanceNet_base
)

target_link_libraries(fdChanneltest PRIVATE
    LanceNet_net
    LanceNet_base
)

target_link_libraries(eventLooptest PRIVATE
    LanceNet_net
    LanceNet_base
)

target_link_libraries(runinlooptest PRIVATE
    LanceNet_net
    LanceNet_base
)

target_link_libraries(EventLoopThreadtest PRIVATE
    LanceNet_net
    LanceNet_base
)


