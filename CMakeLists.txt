cmake_minimum_required(VERSION 3.20)
project(inverse_md5_calc)

set(CMAKE_CXX_STANDARD 20)

add_executable(inverse_md5_calc main.cpp md5.hpp md5.cpp md5_ptr.cpp md5_utils.hpp utils.cpp)
