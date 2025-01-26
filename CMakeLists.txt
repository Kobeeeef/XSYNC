cmake_minimum_required(VERSION 3.10)

# Project name
project(XSync)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include directories
include_directories(${PROJECT_SOURCE_DIR}/include)

# Find and link the ZeroMQ library
find_package(PkgConfig REQUIRED)
pkg_check_modules(ZeroMQ REQUIRED libzmq)
include_directories(${ZeroMQ_INCLUDE_DIRS})
link_directories(${ZeroMQ_LIBRARY_DIRS})

# Suppress ZeroMQ deprecation warnings
add_definitions(-DZEROMQ_DEPRECATED)

# Source files
set(SOURCES
        ${PROJECT_SOURCE_DIR}/XSyncClient.cpp
        ${PROJECT_SOURCE_DIR}/XSyncServer.cpp
)

# Add the executable targets
add_executable(XSyncClient ${PROJECT_SOURCE_DIR}/XSyncClient.cpp)
add_executable(XSyncServer ${PROJECT_SOURCE_DIR}/XSyncServer.cpp)

# Link libraries
target_link_libraries(XSyncClient ${ZeroMQ_LIBRARIES})
target_link_libraries(XSyncServer ${ZeroMQ_LIBRARIES})
