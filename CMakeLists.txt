# cmake_minimum_required(VERSION <specify CMake version here>)
project(ConcurrentLab4)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -openmp:llvm -openmp:experimental")

add_executable(ConcurrentLab4 main.cpp)