# Set the minimum version of CMake that can be used
# To find the cmake version run
# $ cmake --version
cmake_minimum_required(VERSION 3.5)

# Set the project name
project(HttpServer)

# Create a sources variable with a link to all cpp files to compile
set(SOURCES
    ${PROJECT_SOURCE_DIR}/src/Main.cpp
    ${PROJECT_SOURCE_DIR}/src/Channel.cpp
    ${PROJECT_SOURCE_DIR}/src/Epoller.cpp
    ${PROJECT_SOURCE_DIR}/src/EventLoop.cpp
    ${PROJECT_SOURCE_DIR}/src/EventLoopThread.cpp
    ${PROJECT_SOURCE_DIR}/src/EventLoopThreadPool.cpp
    ${PROJECT_SOURCE_DIR}/src/HttpData.cpp
    ${PROJECT_SOURCE_DIR}/src/Server.cpp
    ${PROJECT_SOURCE_DIR}/src/Util.cpp
    ${PROJECT_SOURCE_DIR}/src/log/format.cc
    ${PROJECT_SOURCE_DIR}/src/log/logger.cc
    ${PROJECT_SOURCE_DIR}/src/timer/Timer.cpp
)
# find a Threads library
find_package(Threads)

# check if boost was found
if(Threads_FOUND)
    message ("Thread found")
else()
    message (FATAL_ERROR "Cannot find threads")
endif()

# set the C++ standard to C++ 11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# add flag
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -g -Wall -O3 -fno-strict-aliasing")

# Add an executable
add_executable(${PROJECT_NAME}  ${SOURCES})


# Set the directories that should be included in the build command for this target
# when running g++ these will be included as -I/directory/path/
target_include_directories(${PROJECT_NAME} 
    PRIVATE
        ${PROJECT_SOURCE_DIR}/webserver_lzh_v2
        ${PROJECT_SOURCE_DIR}/webserver_lzh_v2/log
        ${PROJECT_SOURCE_DIR}/webserver_lzh_v2/timer
)

# link against the thread libraries
target_link_libraries( ${PROJECT_NAME} 
    PRIVATE
        ${CMAKE_THREAD_LIBS_INIT}
)
