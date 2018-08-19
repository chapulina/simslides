cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

find_package(Qt5Widgets REQUIRED)
find_package(Qt5Core REQUIRED)
find_package(Protobuf REQUIRED)
find_package(gazebo REQUIRED)

add_definitions(${QT_DEFINITIONS})

include_directories(SYSTEM
  ${GAZEBO_INCLUDE_DIRS}
  ${Qt5Core_INCLUDE_DIRS}
)
link_directories(${GAZEBO_LIBRARY_DIRS})

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GAZEBO_CXX_FLAGS}")

set (client_src
  Common.cc
  ImportDialog.cc
  InsertActorDialog.cc
  Keyframe.cc
  LoadDialog.cc
  PresentMode.cc
  Simslides.cc
)

set (qt_headers
  Common.hh
  ImportDialog.hh
  InsertActorDialog.hh
  Keyframe.hh
  LoadDialog.hh
  PresentMode.hh
  Simslides.hh
)

QT5_WRAP_CPP(headers_MOC ${qt_headers})

add_library(simslides SHARED ${client_src} ${headers_MOC})
#add_library(simslides SHARED ${headers_MOC}
 # Simslides.cc)
target_link_libraries(simslides ${GAZEBO_LIBRARIES}
    ${Qt5Core_LIBRARIES}
    ${Qt5Widgets_LIBRARIES}
    ${PROTOBUF_LIBRARIES}
)
