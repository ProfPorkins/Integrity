cmake_minimum_required(VERSION 2.8)
project(BoostIoService)

#
# Prepare the Boost include headers and libraries
#
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
set(Boost_DEBUG OFF)
find_package(Boost 1.54.0 COMPONENTS system date_time regex)

if (Boost_FOUND)
	include_directories(${Boost_INCLUDE_DIR})
	#
	# Boost likes to have this defined when compiling on Windows systems
	#
	if (CMAKE_GENERATOR MATCHES "Visual Studio")
		add_definitions(-D_WIN32_WINNT=0x0501)
	endif()	
	#
	# Define the main project
	#
	add_executable(BoostIoService MainCode.cpp IRange.hpp)
	target_link_libraries(BoostIoService ${Boost_LIBRARIES})
endif()
