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
find_package(ignition-common3 REQUIRED)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${IGNITION-GUI_CXX_FLAGS} -std=c++17 -lstdc++fs")

QT5_ADD_RESOURCES(resources_RCC ${library_name}.qrc)
QT5_WRAP_CPP(headers_MOC ${qt_headers})

include_directories(SYSTEM
  ${IGNITION-COMMON_INCLUDE_DIRS}
  ${IGNITION-GUI_INCLUDE_DIRS}
  ${Qt5Core_INCLUDE_DIRS}
  ${Qt5Qml_INCLUDE_DIRS}
  ${Qt5Quick_INCLUDE_DIRS}
  ${Qt5QuickControls2_INCLUDE_DIRS}
)

link_directories(
  ${IGNITION-COMMON_LIBRARY_DIRS}
  ${IGNITION-GUI_LIBRARY_DIRS}
)

add_library(${library_name} SHARED
  ${client_src}
  ${headers_MOC}
  ${resources_RCC}
)
target_link_libraries(${library_name}
  stdc++fs
  simslides_common
  ${IGNITION-GUI_LIBRARIES}
  ${Qt5Core_LIBRARIES}
  ${Qt5Qml_LIBRARIES}
  ${Qt5Quick_LIBRARIES}
  ${Qt5QuickControls2_LIBRARIES}
  ${PROTOBUF_LIBRARIES}
)

include(GNUInstallDirs)
install(TARGETS ${library_name}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

