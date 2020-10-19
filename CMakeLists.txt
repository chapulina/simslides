cmake_minimum_required(VERSION 3.5.1 FATAL_ERROR)

project(simslides)

find_package(ignition-common3 REQUIRED)
set(IGN_COMMON_VER ${ignition-common3_VERSION_MAJOR})

find_package(ignition-gazebo4 REQUIRED COMPONENTS gui)
set(IGN_GAZEBO_VER ${ignition-gazebo4_VERSION_MAJOR})

find_package(ignition-rendering4 REQUIRED)
set(IGN_RENDERING_VER ${ignition-rendering4_VERSION_MAJOR})

find_package(ignition-gui4 REQUIRED)
set(IGN_GUI_VER ${ignition-gui4_VERSION_MAJOR})

find_package(sdformat10 REQUIRED)
set(SDF_VER ${sdformat10_VERSION_MAJOR})

add_subdirectory(common)
add_subdirectory(ignition)

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
