cmake_minimum_required(VERSION 3.5.1 FATAL_ERROR)

project(simslides)

find_package(ignition-common4 REQUIRED)
set(IGN_COMMON_VER ${ignition-common4_VERSION_MAJOR})

find_package(ignition-gazebo5 REQUIRED COMPONENTS gui)
set(IGN_GAZEBO_VER ${ignition-gazebo5_VERSION_MAJOR})

find_package(ignition-rendering5 REQUIRED)
set(IGN_RENDERING_VER ${ignition-rendering5_VERSION_MAJOR})

find_package(ignition-gui5 REQUIRED)
set(IGN_GUI_VER ${ignition-gui5_VERSION_MAJOR})

find_package(sdformat11 REQUIRED)
set(SDF_VER ${sdformat11_VERSION_MAJOR})

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
