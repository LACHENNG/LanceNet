set(src memallocator.cpp memmanager.cpp alloc.cpp)
# add_definitions(-DDEBUG)
add_library(memorypool ${src})

add_executable(test test.cpp)

target_link_libraries(test memorypool wrappers)