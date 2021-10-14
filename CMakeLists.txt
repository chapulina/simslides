cmake_minimum_required(VERSION 3.5.1 FATAL_ERROR)

project(simslides)

# If Fortress is chosen, skip Gazebo classic and require Fortress
if("$ENV{IGNITION_VERSION}" STREQUAL "fortress")

  set(IGN_GUI_VER 6)
  find_package(ignition-gui${IGN_GUI_VER} REQUIRED)

  set(SDFORMAT_VER 12)
  find_package(sdformat${SDFORMAT_VER} REQUIRED)

  set(IGN_COMMON_VER 4)
  find_package(ignition-common${IGN_COMMON_VER} REQUIRED)

  set(IGN_GAZEBO_VER 6)
  find_package(ignition-gazebo${IGN_GAZEBO_VER} REQUIRED COMPONENTS gui)

  set(IGN_RENDERING_VER 6)
  find_package(ignition-rendering${IGN_RENDERING_VER} REQUIRED)

  set(IGNITION_FOUND true)
  message(STATUS "Compiling against Ignition Fortress")

# If Edifice is chosen, skip Gazebo classic and require Edifice
elseif("$ENV{IGNITION_VERSION}" STREQUAL "edifice")

  set(IGN_GUI_VER 5)
  find_package(ignition-gui${IGN_GUI_VER} REQUIRED)

  set(SDFORMAT_VER 11)
  find_package(sdformat${SDFORMAT_VER} REQUIRED)

  set(IGN_COMMON_VER 4)
  find_package(ignition-common${IGN_COMMON_VER} REQUIRED)

  set(IGN_GAZEBO_VER 5)
  find_package(ignition-gazebo${IGN_GAZEBO_VER} REQUIRED COMPONENTS gui)

  set(IGN_RENDERING_VER 5)
  find_package(ignition-rendering${IGN_RENDERING_VER} REQUIRED)

  set(IGNITION_FOUND true)
  message(STATUS "Compiling against Ignition Edifice")

# Default to Citadel + Gazebo 11
# If one simulator isn't found, only use the other
else()

  find_package(gazebo QUIET)
  find_package(Protobuf REQUIRED)

  set(IGN_GUI_VER 3)
  find_package(ignition-gui${IGN_GUI_VER} QUIET)

  set(SDFORMAT_VER 9)
  find_package(sdformat${SDFORMAT_VER} REQUIRED)

  if (${ignition-gui${IGN_GUI_VER}_FOUND})

    set(IGN_COMMON_VER 3)
    find_package(ignition-common${IGN_COMMON_VER} REQUIRED)

    set(IGN_GAZEBO_VER 3)
    find_package(ignition-gazebo${IGN_GAZEBO_VER} REQUIRED COMPONENTS gui)

    set(IGN_RENDERING_VER 3)
    find_package(ignition-rendering${IGN_RENDERING_VER} REQUIRED)

    set(IGNITION_FOUND true)
    message(STATUS "Compiling against Ignition Citadel")

  else()
    message (STATUS "Ignition not found, skipping this simulator")
  endif()

endif()

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
if (${IGNITION_FOUND})
  message (STATUS "Ignition found")
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
