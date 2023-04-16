
add_executable(buffer_test Buffer_test.cpp)
add_executable(loop_test event001.cpp)



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

