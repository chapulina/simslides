cmake_minimum_required(VERSION 3.5.1 FATAL_ERROR)

project(simslides)

find_package(gazebo)
find_package(SDFormat REQUIRED)
find_package(Protobuf REQUIRED)

# Common
add_subdirectory(common)

# Gazebo-classic
if (${gazebo_FOUND})
  message (STATUS "Gazebo-classic found")
  add_subdirectory(classic)
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

configure_file(
  "simslides.sh.in"
  "${CMAKE_CURRENT_BINARY_DIR}/share/simslides" @ONLY
)

install(PROGRAMS
  ${CMAKE_CURRENT_BINARY_DIR}/share/simslides
  DESTINATION bin
)

