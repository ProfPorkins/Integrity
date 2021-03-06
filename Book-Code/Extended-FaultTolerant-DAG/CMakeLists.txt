cmake_minimum_required(VERSION 2.8)
project(FaultTolerantDAG)

set(PROTOBUF_SRC_ROOT_FOLDER "${PROJECT_SOURCE_DIR}/../ProtocolBuffers/protobuf-2.6.0")
find_package(Protobuf REQUIRED)
if (PROTOBUF_FOUND)
	message(STATUS "Google Protocol Buffers: " ${PROTOBUF_INCLUDE_DIRS})

	set(Shared_Messages_Protos
		Shared/Messages/DAGExample.proto
		Shared/Messages/DAGExampleResult.proto
		Shared/Messages/Mandel.proto
		Shared/Messages/MandelFinished.proto
		Shared/Messages/MandelFinishedResult.proto
		Shared/Messages/MandelResult.proto
		Shared/Messages/NextPrime.proto
		Shared/Messages/NextPrimeResult.proto
		Shared/Messages/TaskRequest.proto
		Shared/Messages/TaskStatus.proto
		Shared/Messages/TerminateCommand.proto
		)
	PROTOBUF_GENERATE_CPP(Proto_Message_Sources Proto_Message_Headers ${Shared_Messages_Protos})
endif()

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
find_package(Boost COMPONENTS system date_time regex REQUIRED)

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
	Client/FaultTolerantApp.cpp
	Client/FaultTolerantApp.hpp
	Client/Mandelbrot.cpp
	Client/Mandelbrot.hpp
	Client/MandelMisc.hpp
	Client/WinMain.cpp
	)

#
# Define the Server project
add_executable(Server 
	Server/ComputeServer.cpp
	Server/ComputeServer.hpp
	Server/ServerMain.cpp
	)

#
# Define the shared code messages
set(Shared_Messages_Headers
	Shared/Messages/DAGExample.hpp
	Shared/Messages/DAGExampleResult.hpp
	Shared/Messages/MandelFinished.hpp
	Shared/Messages/MandelFinishedResult.hpp
	Shared/Messages/MandelMessage.hpp
	Shared/Messages/MandelResult.hpp
	Shared/Messages/Message.hpp
	Shared/Messages/MessagePBMixIn.hpp
	Shared/Messages/MessageTypes.hpp
	Shared/Messages/NextPrime.hpp
	Shared/Messages/NextPrimeResult.hpp
	Shared/Messages/ResultMessage.hpp
	Shared/Messages/TerminateCommand.hpp
	Shared/Messages/TaskMessage.hpp
	Shared/Messages/TaskRequest.hpp
	Shared/Messages/TaskStatus.hpp
	)
set(Shared_Messages_Sources
	Shared/Messages/Message.cpp
	)
	

source_group("Messages\\Header Files" FILES ${Shared_Messages_Headers})
source_group("Messages\\Source Files" FILES ${Shared_Messages_Sources})
source_group("Messages\\Proto\\Files" FILES ${Shared_Messages_Protos})
source_group("Messages\\Proto\\Header Files" FILES ${Proto_Message_Headers})
source_group("Messages\\Proto\\Source Files" FILES ${Proto_Message_Sources})

set(Shared_Threading_Headers
	Shared/Threading/ConcurrentDAG.hpp
	Shared/Threading/ConcurrentQueue.hpp
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
	Shared/Tasks/DAGExampleTask.hpp
	Shared/Tasks/MandelFinishedTask.hpp
	Shared/Tasks/MandelTask.hpp
	Shared/Tasks/NextPrimeTask.hpp
	Shared/Tasks/Task.hpp
	)
set(Shared_Tasks_Sources
	Shared/Tasks/DAGExampleTask.cpp
	Shared/Tasks/MandelFinishedTask.cpp
	Shared/Tasks/MandelTask.cpp
	Shared/Tasks/NextPrimeTask.cpp
	Shared/Tasks/Task.cpp
	)
source_group("Tasks\\Header Files" FILES ${Shared_Tasks_Headers})
source_group("Tasks\\Source Files" FILES ${Shared_Tasks_Sources})

set(Shared_Framework_Headers
	Shared/AssignedTask.hpp
	Shared/FaultTolerantFramework.hpp
	Shared/Server.hpp
	Shared/ServerSet.hpp
	Shared/TaskRequestQueue.hpp
	Shared/TaskStatusTool.hpp
	)
set(Shared_Framework_Sources
	Shared/AssignedTask.cpp
	Shared/FaultTolerantFramework.cpp
	Shared/ServerSet.cpp
	Shared/TaskRequestQueue.cpp
	Shared/TaskStatusTool.cpp
	)
source_group("Framework\\Header Files" FILES ${Shared_Framework_Headers})
source_group("Framework\\Source Files" FILES ${Shared_Framework_Sources})

set(Shared_Sources
	Shared/IRange.hpp
	${Shared_Framework_Headers}
	${Shared_Framework_Sources}
	${Shared_Messages_Headers}
	${Shared_Messages_Sources}
	${Shared_Threading_Headers}
	${Shared_Threading_Sources}
	${Shared_Tasks_Headers}
	${Shared_Tasks_Sources}
	${Shared_Messages_Protos}
	${Proto_Message_Headers}
	${Proto_Message_Sources}
	)

#
# This adds the Shared code as a static library project
add_library(Shared STATIC ${Shared_Sources})

#
# Give everything access to the Shared code include folders
set_property(TARGET Client APPEND PROPERTY INCLUDE_DIRECTORIES "${PROJECT_SOURCE_DIR}")
set_property(TARGET Server APPEND PROPERTY INCLUDE_DIRECTORIES "${PROJECT_SOURCE_DIR}")
set_property(TARGET Shared APPEND PROPERTY INCLUDE_DIRECTORIES "${PROJECT_SOURCE_DIR}")

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
	set_property(TARGET Client APPEND PROPERTY INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIRS})
	set_property(TARGET Server APPEND PROPERTY INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIRS})
	set_property(TARGET Shared APPEND PROPERTY INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIRS})
	link_directories(${Boost_LIBRARY_DIRS})
	
	target_link_libraries(Client ${Boost_LIBRARIES})
	target_link_libraries(Server ${Boost_LIBRARIES})
endif()

if (PROTOBUF_FOUND)
	set_property(TARGET Client APPEND PROPERTY INCLUDE_DIRECTORIES ${PROTOBUF_INCLUDE_DIRS})
	set_property(TARGET Server APPEND PROPERTY INCLUDE_DIRECTORIES ${PROTOBUF_INCLUDE_DIRS})
	set_property(TARGET Shared APPEND PROPERTY INCLUDE_DIRECTORIES ${PROTOBUF_INCLUDE_DIRS})
	
	set(ProtobufGeneratedMessages ${CMAKE_CURRENT_BINARY_DIR} CACHE INTERNAL "Path to generated protbuf files.")
	set_property(TARGET Client APPEND PROPERTY INCLUDE_DIRECTORIES ${ProtobufGeneratedMessages})
	set_property(TARGET Server APPEND PROPERTY INCLUDE_DIRECTORIES ${ProtobufGeneratedMessages})
	set_property(TARGET Shared APPEND PROPERTY INCLUDE_DIRECTORIES ${ProtobufGeneratedMessages})
	
	target_link_libraries(Client ${PROTOBUF_LIBRARIES})
	target_link_libraries(Server ${PROTOBUF_LIBRARIES})
endif()


#
# Specify the Shared project to be linked with the Client and Server projects
target_link_libraries(Client Shared)
target_link_libraries(Server Shared)
