cmake_minimum_required(VERSION 3.10)

project(Log VERSION 2.0)


# Make sure to add the CMAKE_CXX_STANDARD declarations above the call to add_executable
# specify the c++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_COMPILER "g++")

set(SOURCES
    log_test3.cpp
    format.cc
)
find_package(Threads)

add_executable(${PROJECT_NAME} ${SOURCES})
# target_link_libaries after add_executable

target_link_libraries(${PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT})


