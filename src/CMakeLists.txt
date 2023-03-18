# set(CMAKE_INCLUDE_CURRENT_DIR ON)
# set(CMAKE_CXX_STANDARD 11)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/MemoryPool)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/ThreadPool)

aux_source_directory(. DIR_LIB_SRCS)

message("DIR_LIB_SRCS : ${DIR_LIB_SRCS}")

add_library(wrappers ${DIR_LIB_SRCS})

add_subdirectory(MemoryPool)
add_subdirectory(ObjectPool)