set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -lstdc++fs")

set (common_src
  Common.cc
  Keyframe.cc
)

include_directories(SYSTEM
  ${SDFormat_INCLUDE_DIRS}
)
link_directories(${SDFormat_LIBRARY_DIRS})

add_library(simslides_common SHARED
  ${common_src}
)

target_include_directories(simslides_common
  PUBLIC
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_compile_features(simslides_common PRIVATE cxx_std_17)

target_link_libraries(simslides_common
  PUBLIC
    ${SDFormat_LIBRARIES}
)

include(GNUInstallDirs)
install(TARGETS simslides_common
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
