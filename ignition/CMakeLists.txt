set(library_name SimSlidesIgn)

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

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${IGNITION-GUI_CXX_FLAGS} -std=c++17 -lstdc++fs")

QT5_ADD_RESOURCES(resources_RCC ${library_name}.qrc)
QT5_WRAP_CPP(headers_MOC ${qt_headers})

add_library(${library_name} SHARED
  ${client_src}
  ${headers_MOC}
  ${resources_RCC}
)

target_link_libraries(${library_name}
  stdc++fs
  simslides_common
  ignition-common${IGN_COMMON_VER}::ignition-common${IGN_COMMON_VER}
  ignition-gazebo${IGN_GAZEBO_VER}::ignition-gazebo${IGN_GAZEBO_VER}
  ignition-gui${IGN_GUI_VER}::ignition-gui${IGN_GUI_VER}
  ignition-rendering${IGN_RENDERING_VER}::ignition-rendering${IGN_RENDERING_VER}
)

include(GNUInstallDirs)
install(TARGETS ${library_name}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

configure_file(
  "simslides_ignition.sh.in"
  "${CMAKE_CURRENT_BINARY_DIR}/share/simslides_ignition" @ONLY
)

install(PROGRAMS
  ${CMAKE_CURRENT_BINARY_DIR}/share/simslides_ignition
  DESTINATION bin
)

