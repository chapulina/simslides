set (client_src
  Helpers.cc
  ImportDialog.cc
  InsertActorDialog.cc
  LoadDialog.cc
  PresentMode.cc
  SimSlides.cc
)

set (qt_headers
  ImportDialog.hh
  InsertActorDialog.hh
  LoadDialog.hh
  PresentMode.hh
  SimSlides.hh
)

find_package(Qt5
  COMPONENTS
    Core
    Widgets
  REQUIRED
)

add_definitions(${QT_DEFINITIONS})

include_directories(SYSTEM
  ${GAZEBO_INCLUDE_DIRS}
  ${Qt5Core_INCLUDE_DIRS}
)
link_directories(${GAZEBO_LIBRARY_DIRS})

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GAZEBO_CXX_FLAGS} -std=c++17 -lstdc++fs")

QT5_WRAP_CPP(headers_MOC ${qt_headers})

set (LIB_NAME SimSlidesClassic)

add_library(${LIB_NAME} SHARED
  ${client_src}
  ${headers_MOC}
)
target_link_libraries(${LIB_NAME}
  stdc++fs
  SimSlidesCommon
  ${GAZEBO_LIBRARIES}
  ${Qt5Core_LIBRARIES}
  ${Qt5Test_LIBRARIES}
  ${Qt5Widgets_LIBRARIES}
  ${PROTOBUF_LIBRARIES}
)

include(GNUInstallDirs)
install(TARGETS ${LIB_NAME}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

configure_file(
  "simslides_classic.sh.in"
  "${CMAKE_CURRENT_BINARY_DIR}/share/simslides_classic" @ONLY
)

install(PROGRAMS
  ${CMAKE_CURRENT_BINARY_DIR}/share/simslides_classic
  DESTINATION bin
)

