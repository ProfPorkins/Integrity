cmake_minimum_required(VERSION 2.8)
project(BoostThreadedSendReceive)

#
# Define the main project
#
add_executable(Server ServerMain.cpp message.proto)
add_executable(Client ClientMain.cpp IRange.hpp message.proto)

#
# Prepare the Boost include headers and libraries
#
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
set(Boost_DEBUG OFF)
find_package(Boost COMPONENTS system date_time regex REQUIRED)

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

	target_link_libraries(Server ${Boost_LIBRARIES})
	target_link_libraries(Client ${Boost_LIBRARIES})
endif()

set(PROTOBUF_SRC_ROOT_FOLDER "${PROJECT_SOURCE_DIR}/../ProtocolBuffers/protobuf-2.6.0")
find_package(Protobuf REQUIRED)
if (PROTOBUF_FOUND)
	message(STATUS "Google Protocol Buffers: Found")
	message(STATUS ${PROTOBUF_INCLUDE_DIRS})

	set(ProtobufGenerateMessages ${CMAKE_CURRENT_BINARY_DIR} CACHE INTERNAL "Path to generated protbuf files.")
	include_directories(${ProtobufGenerateMessages})
	include_directories(${PROTOBUF_INCLUDE_DIRS})

	set(protos "message.proto")
	PROTOBUF_GENERATE_CPP(ProtoSources ProtoHeaders ${protos})
	add_library(messages STATIC ${ProtoSources} ${ProtoHeaders})

	target_link_libraries(Client messages)
	target_link_libraries(Server messages)
	target_link_libraries(Client ${PROTOBUF_LIBRARIES})
	target_link_libraries(Server ${PROTOBUF_LIBRARIES})
endif()
