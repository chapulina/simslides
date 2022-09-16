set(library_name SimSlidesGz)

set(client_src
  ${library_name}.cc
)

set (qt_headers
  ${library_name}.hh
)

find_package(Qt5
  COMPONENTS
    Core
    Quick
    QuickControls2
  REQUIRED
)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GZ-GUI_CXX_FLAGS} -std=c++17 -lstdc++fs")

QT5_ADD_RESOURCES(resources_RCC ${library_name}.qrc)
QT5_WRAP_CPP(headers_MOC ${qt_headers})

add_library(${library_name} SHARED
  ${client_src}
  ${headers_MOC}
  ${resources_RCC}
)

target_link_libraries(${library_name}
  stdc++fs
  SimSlidesCommon
  gz-common${GZ_COMMON_VER}::gz-common${GZ_COMMON_VER}
  gz-sim${GZ_SIM_VER}::gz-sim${GZ_SIM_VER}
  gz-gui${GZ_GUI_VER}::gz-gui${GZ_GUI_VER}
  gz-rendering${GZ_RENDERING_VER}::gz-rendering${GZ_RENDERING_VER}
)

include(GNUInstallDirs)
install(TARGETS ${library_name}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/${PROJECT_NAME}/gz-gui/
)

configure_file(
  "simslides_gazebo.sh.in"
  "${CMAKE_CURRENT_BINARY_DIR}/share/simslides_gazebo" @ONLY
)

install(PROGRAMS
  ${CMAKE_CURRENT_BINARY_DIR}/share/simslides_gazebo
  DESTINATION bin
)
