cmake_minimum_required(VERSION 2.8)
project(Scalability)

#
# Need to tell Visual Studio to use unicode because we are making a Win32 application
if (CMAKE_GENERATOR MATCHES "Visual Studio")
	add_definitions(-DUNICODE -D_UNICODE)
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
add_executable(Scalability WIN32
	ConcurrentMultiqueue.hpp
	IRange.hpp
	Mandelbrot.cpp
	Mandelbrot.hpp
	MandelImageTask.cpp
	MandelImageTask.hpp
	MandelPartTask.cpp
	MandelPartTask.hpp
	NextPrimeTask.cpp
	NextPrimeTask.hpp
	ScalabilityApp.cpp
	ScalabilityApp.hpp
	Task.hpp
	ThreadPool.cpp
	ThreadPool.hpp
	WinMain.cpp
	WorkerThread.cpp
	WorkerThread.hpp
	)
