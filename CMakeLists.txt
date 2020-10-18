cmake_minimum_required(VERSION 3.5.1 FATAL_ERROR)

project(simslides)

find_package(gazebo REQUIRED)
find_package(Qt5 COMPONENTS Core Widgets Test REQUIRED)
find_package(Protobuf REQUIRED)

# Common
add_subdirectory(common)

# Gazebo classic
add_subdirectory(classic)

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
