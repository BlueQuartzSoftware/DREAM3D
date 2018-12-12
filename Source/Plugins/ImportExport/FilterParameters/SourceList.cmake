
set (${PLUGIN_NAME}_FilterParameters_SRCS "")

set (${PLUGIN_NAME}_FilterParameters_HDRS "")

set(_FilterParameters
  # SomeNewFilterParameter
  )

foreach(f ${_FilterParameters} )
  set (${PLUGIN_NAME}_FilterParameters_SRCS
      ${${PLUGIN_NAME}_FilterParameters_SRCS}
  )

  set (${PLUGIN_NAME}_FilterParameters_HDRS
      ${${PLUGIN_NAME}_FilterParameters_HDRS}
  )
endforeach()


cmp_IDE_SOURCE_PROPERTIES( "FilterParameters" "${${PLUGIN_NAME}_FilterParameters_HDRS}" "${${PLUGIN_NAME}_FilterParameters_SRCS}" "${PROJECT_INSTALL_HEADERS}")
