cmake_minimum_required(VERSION 2.8)

project(samplePrefetch)

add_executable(measurePrefetch main.cpp)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mfpu=neon-fp16 -fpermissive")
