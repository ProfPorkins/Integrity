cmake_minimum_required(VERSION 2.8)
project(DAG)

#
# g++ needs to be told to compile for C++11, and we also have to 
# tell it to link against the pthread library for std::thread to
# work correctly.
#
if (CMAKE_COMPILER_IS_GNUCXX)
	set(CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS} -pthread")
endif()

#
# Define the main project
#
add_executable(DAG MainCode.cpp)
