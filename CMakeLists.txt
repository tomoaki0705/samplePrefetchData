cmake_minimum_required(VERSION 2.8)

project(samplePrefetch)

add_executable(measurePrefetch main.cpp)


if(CMAKE_SYSTEM_PROCESSOR MATCHES "^(arm.*|ARM.*)")
	set(ARM 1)
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(aarch64)")
	set(AARCH64 1)
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpermissive -std=c++11")

if(ARM)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mfpu=neon-fp16")
endif(ARM)
