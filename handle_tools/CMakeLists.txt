cmake_minimum_required(VERSION 2.8)
PROJECT(HandleTools)
message(STATUS "project HandleTools")
set(CMAKE_BUILD_TYPE DEBUG)
set(CMAKE_C_FLAGS_DEBUG "-g -Wall")

option(FOR_ARM "option for build for arm" OFF)
message("FOR_ARM option is ${FOR_ARM}")
if(${FOR_ARM} STREQUAL "ON")
	message(STATUS "start to build ARM platform.")
	SET(CMAKE_SYSTEM_NAME Linux_4418)                                                           
	SET(CMAKE_C_COMPILER "arm-cortex_a9-linux-gnueabi-gcc")
	SET(CMAKE_CXX_COMPILER "arm-cortex_a9-linux-gnueabi-g++")
else()
	message(STATUS "start to build PC platform.")
endif()
add_subdirectory(src)

