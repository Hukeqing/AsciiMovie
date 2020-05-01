cmake_minimum_required(VERSION 3.15)
project(AsciiMovie)

set(CMAKE_CXX_STANDARD 11)

add_executable(AsciiMovie main.cpp image.h stb_image.h stb_image_write.h stb_image_resize.h)

add_executable(Show amShow.cpp)

add_definitions(-DDEBUG)