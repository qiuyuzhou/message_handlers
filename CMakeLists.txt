cmake_minimum_required(VERSION 2.8.7)
project(message_handlers)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include(conanbuildinfo.cmake)
conan_basic_setup()

add_subdirectory(tests)

