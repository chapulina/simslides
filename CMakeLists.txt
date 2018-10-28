cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

find_package(gazebo REQUIRED)
find_package(Qt5 COMPONENTS Core Widgets Test REQUIRED)
find_package(Protobuf REQUIRED)

add_definitions(${QT_DEFINITIONS})

include_directories(SYSTEM
  ${GAZEBO_INCLUDE_DIRS}
  ${Qt5Core_INCLUDE_DIRS}
)
link_directories(${GAZEBO_LIBRARY_DIRS})

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GAZEBO_CXX_FLAGS}")

add_subdirectory(src)

