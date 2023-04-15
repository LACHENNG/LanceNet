
cmake_minimum_required(VERSION 3.0)

# set project name and version
project(LanceNet LANGUAGES CXX VERSION 1.0)

# specify teh c++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Debug Purpose Only
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb")
# add_definitions(-DDEBUG)

include_directories(${PROJECT_SOURCE_DIR})
message("PROJECT_SOURCE_DIR : ${PROJECT_SOURCE_DIR}")

####################################
### vim-lsp support to tell lsp how the project is build by cmake
### generate compile_commands.json to help language server clangd understand your project
####################################
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(${CMAKE_SOURCE_DIR}/LanceNet)

# install(TARGETS cltest )
