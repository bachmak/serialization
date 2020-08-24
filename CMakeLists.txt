cmake_minimum_required(VERSION 2.8)

include_directories("include")

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
else()
    set(CXX_STANDARD 17)
endif()

file(GLOB SOURCES "src/*.cpp")
add_executable(main ${SOURCES})