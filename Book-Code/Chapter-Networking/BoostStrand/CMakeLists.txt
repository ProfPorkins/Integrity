cmake_minimum_required(VERSION 2.8)
project(BoostStrand)

#
# Prepare the Boost include headers and libraries
#
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
set(Boost_DEBUG OFF)
find_package(Boost COMPONENTS system date_time regex)

if (Boost_FOUND)
	include_directories(${Boost_INCLUDE_DIRS})
	link_directories(${Boost_LIBRARY_DIRS})
	#
	# Boost likes to have this defined when compiling on Windows systems
	#
	if (CMAKE_GENERATOR MATCHES "Visual Studio")
		add_definitions(-D_WIN32_WINNT=0x0501)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc /Zi")
	endif()
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
	add_executable(BoostStrand MainCode.cpp IRange.hpp)
	target_link_libraries(BoostStrand ${Boost_LIBRARIES})
endif()
