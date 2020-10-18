set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -lstdc++fs")

set (common_src
  Common.cc
  Keyframe.cc
)

include_directories(SYSTEM
  ${SDFormat_INCLUDE_DIRS}
)
link_directories(${SDFormat_LIBRARY_DIRS})

set (LIB_NAME SimSlidesCommon)
add_library(${LIB_NAME} SHARED
  ${common_src}
)

target_include_directories(${LIB_NAME}
  PUBLIC
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_compile_features(${LIB_NAME} PRIVATE cxx_std_17)

target_link_libraries(${LIB_NAME}
  PUBLIC
    ${SDFormat_LIBRARIES}
)

include(GNUInstallDirs)
install(TARGETS ${LIB_NAME}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
