cmake_minimum_required(VERSION 3.10)  
project(fuzzer)                    

set(CMAKE_CXX_STANDARD 17)             
set(CMAKE_CXX_STANDARD_REQUIRED ON)  
set(CMAKE_CXX_FLAGS_DEBUG "-g -O3 -DDEBUG -Wall -Wextra -Wswitch-enum")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
# set(CMAKE_BUILD_TYPE Debug)

file(GLOB_RECURSE SRCS "src/*.cpp")

include_directories(include)

add_executable(fuzzer ${SRCS})

