cmake_minimum_required(VERSION 2.8)
project(Lambdas)

#
# g++ needs to be told to compile for C++11
#
if (CMAKE_COMPILER_IS_GNUCXX)
	set(CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")
endif()

#
# Define the main project
#
add_executable(Lambdas MainCode.cpp)
