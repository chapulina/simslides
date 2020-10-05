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
  message (STATUS "Gazebo classic found")
  add_subdirectory(classic)
else()
  message (STATUS "Gazebo classic not found, skipping this simulator")
endif()

# Ignition
if (${ignition-gui3_FOUND})
  message (STATUS "Ignition found")

  find_package(ignition-common3 REQUIRED)
  set(IGN_COMMON_VER ${ignition-common3_VERSION_MAJOR})

  find_package(ignition-gazebo3 REQUIRED COMPONENTS gui)
  set(IGN_GAZEBO_VER ${ignition-gazebo3_VERSION_MAJOR})

  find_package(ignition-rendering3 REQUIRED)
  set(IGN_RENDERING_VER ${ignition-rendering3_VERSION_MAJOR})

  set(IGN_GUI_VER ${ignition-gui3_VERSION_MAJOR})

  add_subdirectory(ignition)
else()
  message (STATUS "Ignition not found, skipping this simulator")
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

