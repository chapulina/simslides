cmake_minimum_required(VERSION 3.5.1 FATAL_ERROR)

project(simslides)

find_package(gz-common5 REQUIRED)
set(GZ_COMMON_VER ${gz-common5_VERSION_MAJOR})

find_package(gz-sim7 REQUIRED COMPONENTS gui)
set(GZ_SIM_VER ${gz-sim7_VERSION_MAJOR})

find_package(gz-rendering7 REQUIRED)
set(GZ_RENDERING_VER ${gz-rendering7_VERSION_MAJOR})

find_package(gz-gui7 REQUIRED)
set(GZ_GUI_VER ${gz-gui7_VERSION_MAJOR})

find_package(sdformat13 REQUIRED)
set(SDF_VER ${sdformat13_VERSION_MAJOR})

add_subdirectory(common)
add_subdirectory(gazebo)

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
