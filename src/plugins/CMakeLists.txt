set (plugins
  ResetModel
)

foreach (plugin ${plugins})
  add_library(${plugin} SHARED
    ${plugin}.cc
  )
  target_link_libraries(${plugin}
    ${GAZEBO_LIBRARIES}
  )
  install(TARGETS ${plugin}
    DESTINATION lib
  )
endforeach (plugin ${plugins})

