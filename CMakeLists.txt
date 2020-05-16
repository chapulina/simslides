cmake_minimum_required(VERSION 3.5.1 FATAL_ERROR)

project(simslides)

find_package(gazebo QUIET)
find_package(ignition-gui3 QUIET)
find_package(sdformat9 REQUIRED)
find_package(Protobuf REQUIRED)

# Common
add_subdirectory(common)

# Gazebo-classic
if (${gazebo_FOUND})
  message (STATUS "Gazebo-classic found")
  add_subdirectory(classic)
endif()

# Ignition
if (${ignition-gui3_FOUND})
  message (STATUS "Ignition found")
  add_subdirectory(ignition)
endif()


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

