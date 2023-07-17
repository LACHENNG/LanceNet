

add_executable(timer test_timer.cpp)
add_executable(thread test_thread.cpp)
add_executable(atomic test_atomic.cpp)
add_executable(order test_initorder.cpp)
add_executable(cltest test_threadpool_sbuf.cpp)


target_link_libraries(timer wrappers pthread)
target_link_libraries(thread wrappers pthread)
target_link_libraries(atomic pthread)
target_link_libraries(order pthread)
target_link_libraries(cltest pthread)