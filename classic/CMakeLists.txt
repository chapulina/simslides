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
  ImportDialog.hh
  InsertActorDialog.hh
  LoadDialog.hh
  PresentMode.hh
  Simslides.hh
)

QT5_WRAP_CPP(headers_MOC ${qt_headers})

add_library(simslides SHARED
  ${client_src}
  ${headers_MOC}
)
target_link_libraries(simslides
  ${GAZEBO_LIBRARIES}
  ${Qt5Core_LIBRARIES}
  ${Qt5Test_LIBRARIES}
  ${Qt5Widgets_LIBRARIES}
  ${PROTOBUF_LIBRARIES}
)

install(TARGETS simslides
  DESTINATION lib
)

