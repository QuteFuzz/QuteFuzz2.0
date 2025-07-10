cmake_minimum_required(VERSION 3.10)  
project(fuzzer)
                

set(CMAKE_CXX_STANDARD 20)             
set(CMAKE_CXX_STANDARD_REQUIRED ON)  
set(CMAKE_CXX_FLAGS_DEBUG "-g -O3 -DDEBUG -Wall -Wextra -Wswitch-enum")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")



file(GLOB_RECURSE SRCS "src/*.cpp")

add_executable(fuzzer ${SRCS})

target_include_directories(fuzzer PRIVATE
    ${CMAKE_SOURCE_DIR}/include/ast_builder
    ${CMAKE_SOURCE_DIR}/include/frontend_parsers
    ${CMAKE_SOURCE_DIR}/include/grammar_parser
    ${CMAKE_SOURCE_DIR}/include/qig
    ${CMAKE_SOURCE_DIR}/include/utils
    ${CMAKE_SOURCE_DIR}/include
)

