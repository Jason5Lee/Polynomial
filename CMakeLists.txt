cmake_minimum_required(VERSION 3.7)
project(Project3)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp polynomial.cpp polynomial.h storage.h)
add_executable(Project3 ${SOURCE_FILES})