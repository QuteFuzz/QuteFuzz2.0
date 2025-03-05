cmake_minimum_required(VERSION 3.10)  
project(fuzzer)                    

set(CMAKE_CXX_STANDARD 17)             
set(CMAKE_CXX_STANDARD_REQUIRED ON)    

file(GLOB_RECURSE SRCS "src/*.cpp")

include_directories(include)

add_executable(fuzzer ${SRCS})

