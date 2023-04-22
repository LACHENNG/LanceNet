
add_executable(buffer_test Buffer_test.cpp)

add_executable(loop_test event001.cpp)
add_executable(loop_test2 event002.cpp)
add_executable(loop_test3 event003.cpp)
add_executable(loop_test4 event004.cpp)
add_executable(loop_test5 event005.cpp)
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

target_link_libraries(loop_test4 PRIVATE
    LanceNet_net
    LanceNet_base
)

target_link_libraries(loop_test5 PRIVATE
    LanceNet_net
    LanceNet_base
)


