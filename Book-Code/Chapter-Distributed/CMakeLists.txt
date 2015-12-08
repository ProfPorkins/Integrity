cmake_minimum_required(VERSION 2.8)
project(Scalability)

#
# Need to tell Visual Studio to use unicode because we are making a Win32/64 application
if (CMAKE_GENERATOR MATCHES "Visual Studio")
	add_definitions(-DUNICODE -D_UNICODE)
endif()

#
# Prepare the Boost include headers and libraries
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
set(Boost_DEBUG OFF)
find_package(Boost COMPONENTS system date_time regex)

#
# g++ needs to be told to compile for C++11, and we also have to 
# tell it to link against the pthread library for std::thread to
# work correctly.
if (CMAKE_COMPILER_IS_GNUCXX)
	set(CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS} -pthread")
endif()

#
# Define the Client project
add_executable(Client WIN32
	Client/DistributedApp.cpp
	Client/DistributedApp.hpp
	Client/Mandelbrot.cpp
	Client/Mandelbrot.hpp
	Client/Server.hpp
	Client/ServerSet.cpp
	Client/ServerSet.hpp
	Client/WinMain.cpp
	Client/TaskRequestQueue.cpp
	Client/TaskRequestQueue.hpp
	)

#
# Define the Server project
add_executable(Server 
	Server/ComputeServer.cpp
	Server/ComputeServer.hpp
	Server/ServerMain.cpp
	)

#
# Define the shared code project
set(Shared_Messages_Headers
	Shared/Messages/MandelMessage.hpp
	Shared/Messages/MandelResultMessage.hpp
	Shared/Messages/Message.hpp
	Shared/Messages/MessageTypes.hpp
	Shared/Messages/NextPrimeMessage.hpp
	Shared/Messages/NextPrimeResultMessage.hpp
	Shared/Messages/ResultMessage.hpp
	Shared/Messages/TerminateCommand.hpp
	Shared/Messages/TaskMessage.hpp
	Shared/Messages/TaskRequest.hpp
	)
set(Shared_Messages_Sources
	Shared/Messages/MandelMessage.cpp
	Shared/Messages/MandelResultMessage.cpp
	Shared/Messages/Message.cpp
	Shared/Messages/NextPrimeMessage.cpp
	Shared/Messages/NextPrimeResultMessage.cpp
	)
source_group("Messages\\Header Files" FILES ${Shared_Messages_Headers})
source_group("Messages\\Source Files" FILES ${Shared_Messages_Sources})

set(Shared_Threading_Headers
	Shared/Threading/ConcurrentPriorityQueue.hpp
	Shared/Threading/ThreadPool.hpp
	Shared/Threading/WorkerThread.hpp
	)
set(Shared_Threading_Sources
	Shared/Threading/ThreadPool.cpp
	Shared/Threading/WorkerThread.cpp
	)
source_group("Threading\\Header Files" FILES ${Shared_Threading_Headers})
source_group("Threading\\Source Files" FILES ${Shared_Threading_Sources})
	
set(Shared_Tasks_Headers
	Shared/Tasks/MandelTask.hpp
	Shared/Tasks/NextPrimeTask.hpp
	Shared/Tasks/Task.hpp
	)
set(Shared_Tasks_Sources
	Shared/Tasks/MandelTask.cpp
	Shared/Tasks/NextPrimeTask.cpp
	Shared/Tasks/Task.cpp
	)
source_group("Tasks\\Header Files" FILES ${Shared_Tasks_Headers})
source_group("Tasks\\Source Files" FILES ${Shared_Tasks_Sources})

set(Shared_Sources
	Shared/IRange.hpp
	Shared/MandelMisc.hpp
	${Shared_Messages_Headers}
	${Shared_Messages_Sources}
	${Shared_Threading_Headers}
	${Shared_Threading_Sources}
	${Shared_Tasks_Headers}
	${Shared_Tasks_Sources}
	)

#
# This adds the Shared code as a static library project
add_library(Shared STATIC ${Shared_Sources})

#
# Give everything access to the Shared code include folders
if (${CMAKE_VERSION} VERSION_GREATER "2.8.7")
	set_property(TARGET Client APPEND PROPERTY INCLUDE_DIRECTORIES "${PROJECT_SOURCE_DIR}")
	set_property(TARGET Server APPEND PROPERTY INCLUDE_DIRECTORIES "${PROJECT_SOURCE_DIR}")
	set_property(TARGET Shared APPEND PROPERTY INCLUDE_DIRECTORIES "${PROJECT_SOURCE_DIR}")
endif()

#
# Setup boost for the various projects
if (Boost_FOUND)
	#
	# Boost likes to have this defined when compiling on Windows systems
	if (MSVC)
		add_definitions(-D_WIN32_WINNT=0x0501)
	endif()
	#
	# Interface and Test need access to the boost headers & libraries
	if (${CMAKE_VERSION} VERSION_GREATER "2.8.7")
		set_property(TARGET Client APPEND PROPERTY INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIRS})
		set_property(TARGET Server APPEND PROPERTY INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIRS})
		set_property(TARGET Shared APPEND PROPERTY INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIRS})
	else()
		include_directories(${Boost_INCLUDE_DIRS})
	endif()
	link_directories(${Boost_LIBRARY_DIRS})
	
	target_link_libraries(Client ${Boost_LIBRARIES})
	target_link_libraries(Server ${Boost_LIBRARIES})
endif()

#
# Specify the Shared project to be linked with the Client and Server projects
target_link_libraries(Client Shared)
target_link_libraries(Server Shared)
