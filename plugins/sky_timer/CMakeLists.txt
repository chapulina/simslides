cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project(sky_timer)

# Find packages

find_package(gazebo REQUIRED)

list(APPEND CMAKE_CXX_FLAGS "${GAZEBO_CXX_FLAGS}")

# include appropriate directories
include_directories(${GAZEBO_INCLUDE_DIRS})
link_directories(${GAZEBO_LIBRARY_DIRS})

# Create libraries and executables

add_library(sky_timer SHARED SkyTimer.cc)
target_link_libraries(sky_timer ${GAZEBO_LIBRARIES})

