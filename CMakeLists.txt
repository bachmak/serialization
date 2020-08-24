cmake_minimum_required(VERSION 2.8)

include_directories("include")

file(GLOB SOURCES "src/*.cpp")
add_executable(main ${SOURCES})