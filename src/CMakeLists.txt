# set(CMAKE_INCLUDE_CURRENT_DIR ON)
# set(CMAKE_CXX_STANDARD 11)
# include_directories(${CMAKE_CURRENT_SOURCE_DIR})
aux_source_directory(. DIR_LIB_SRCS)

message("DIR_LIB_SRCS : ${DIR_LIB_SRCS}")
# message("src folder: CMAKE_CURRENT_SOURCE_DIR: ${CMAKE_CURRENT_SOURCE_DIR}")

add_library(wrappers ${DIR_LIB_SRCS})
# set_target_properties(threadpool PROPERTIES LINKER_LANGUAGE CXX)

# message("CMAKE_BINARY_DIR : ${CMAKE_BINARY_DIR}")
# message("CMAKE_CURRENT_BINARY_DIR : ${CMAKE_CURRENT_BINARY_DIR}")