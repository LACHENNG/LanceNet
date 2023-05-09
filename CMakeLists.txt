
cmake_minimum_required(VERSION 3.0)

# set project name and version
project(LanceNet LANGUAGES CXX VERSION 1.0)

# specify teh c++ standard
set(CMAKE_CXX_STANDARD 17) # Heterogeneous Lookup require C++14 , set.extract requires C++17
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_BUILD_TYPE Debug)
# Debug Purpose Only
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb")

set(CXX_FLAGS
 -g
 # -DVALGRIND
 -DCHECK_PTHREAD_RETURN_VALUE
 -D_FILE_OFFSET_BITS=64
 -Wall
 -Wextra
 -Werror
 -Wconversion
 -Wno-unused-parameter
 -Wold-style-cast
 -Woverloaded-virtual
 -Wpointer-arith
 -Wshadow
 -Wwrite-strings
 -march=native
 # -MMD
 -std=c++11
 -rdynamic
 )

# add_definitions(-DDEBUG)

include_directories(${PROJECT_SOURCE_DIR})
message("PROJECT_SOURCE_DIR : ${PROJECT_SOURCE_DIR}")

####################################
### vim-lsp support to tell lsp how the project is build by cmake
### generate compile_commands.json to help language server clangd understand your project
####################################
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(${CMAKE_SOURCE_DIR}/LanceNet)

# binary executable 
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
# install(TARGETS cltest )
