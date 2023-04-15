
find_package(Boost COMPONENTS system filesystem unit_test_framework REQUIRED)
# include_directories ($ {Boost_INCLUDE_DIRS})

message("====================================")
message("${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}")
message("====================================")

set(SRC_LIST
    Buffer.cpp
)

add_library(LanceNet_net ${SRC_LIST})

add_executable(buffer_test Buffer_test.cpp)

# target_link_libraries(buffer_test LanceNet_net LanceNet_base ${Boost_Unit_TEST_FRAMEWORK_LIBRARY})
target_link_libraries(buffer_test PRIVATE 
    LanceNet_net
    LanceNet_base
    ${Boost_FILESYSTEM_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
    ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
)
# target_link_libraries(buffer_test LanceNet_net LanceNet_base Boost::test)
# target_link_libraries(net
#    PRIVATE
#    base)
