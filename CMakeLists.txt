cmake_minimum_required(VERSION 3.20)
project(OpenCVTesting)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package( OpenCV REQUIRED ) # locate OpenCV in system
include_directories( ${OpenCV_INCLUDE_DIRS} ) # provide library headersmessage(STATUS "OpenCV library status:")

add_executable(OpenCVTesting main.cpp)
target_link_libraries( OpenCVTesting ${OpenCV_LIBS} )
