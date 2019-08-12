cmake_minimum_required(VERSION 3.5.1 FATAL_ERROR)

project(simslides)

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

install(DIRECTORY
  models
  DESTINATION share/${PROJECT_NAME}/
)

install(DIRECTORY
  worlds
  DESTINATION share/${PROJECT_NAME}/
)

configure_file(
  "setup.sh.in"
  "${CMAKE_CURRENT_BINARY_DIR}/share/setup.sh" @ONLY
)

install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/share/setup.sh
  DESTINATION share/${PROJECT_NAME}
)

configure_file(
  "simslides.sh.in"
  "${CMAKE_CURRENT_BINARY_DIR}/share/simslides" @ONLY
)

install(PROGRAMS
  ${CMAKE_CURRENT_BINARY_DIR}/share/simslides
  DESTINATION bin
)

