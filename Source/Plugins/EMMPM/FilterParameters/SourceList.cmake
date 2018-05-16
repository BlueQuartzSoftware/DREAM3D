set(SUBDIR_NAME FilterParameters)

set(${PLUGIN_NAME}_FilterParameters_Moc_HDRS
  
)

# --------------------------------------------------------------------
# Run Qts automoc program to generate some source files that get compiled
# QT5_WRAP_CPP( ${PLUGIN_NAME}_FilterParameters_Generated_MOC_SRCS ${${PLUGIN_NAME}_FilterParameters_Moc_HDRS})
set_source_files_properties( ${${PLUGIN_NAME}_FilterParameters_Generated_MOC_SRCS} PROPERTIES GENERATED TRUE)
set_source_files_properties( ${${PLUGIN_NAME}_FilterParameters_Generated_MOC_SRCS} PROPERTIES HEADER_FILE_ONLY TRUE)


set(${PLUGIN_NAME}_FilterParameters_HDRS
  ${EMMPM_SOURCE_DIR}/${SUBDIR_NAME}/EMMPMFilterParameter.h
)

set(${PLUGIN_NAME}_FilterParameters_SRCS
  ${EMMPM_SOURCE_DIR}/${SUBDIR_NAME}/EMMPMFilterParameter.cpp
)

cmp_IDE_SOURCE_PROPERTIES( "${SUBDIR_NAME}" "${${PLUGIN_NAME}_FilterParameters_HDRS};${${PLUGIN_NAME}_FilterParameters_Moc_HDRS}" "${${PLUGIN_NAME}_FilterParameters_SRCS}" "${PROJECT_INSTALL_HEADERS}")
cmp_IDE_SOURCE_PROPERTIES( "Generated/${SUBDIR_NAME}" "" "${${PLUGIN_NAME}_FilterParameters_Generated_MOC_SRCS}" "0")


set(${PLUGIN_NAME}_FilterParameters_HDRS
  ${${PLUGIN_NAME}_FilterParameters_HDRS}
  ${${PLUGIN_NAME}_FilterParameters_Moc_HDRS}  # Add the headers that get Moc'ed here so they show up in solutions/IDEs/Project files
)

set(${PLUGIN_NAME}_FilterParameters_SRCS
  ${${PLUGIN_NAME}_FilterParameters_SRCS}
  ${${PLUGIN_NAME}_FilterParameters_Generated_MOC_SRCS}
)


if( ${PROJECT_INSTALL_HEADERS} EQUAL 1 )
    INSTALL (FILES ${${PLUGIN_NAME}_FilterParameters_HDRS}
                   ${${PLUGIN_NAME}_FilterParameters_Moc_HDRS}
            DESTINATION include/EMMPM/${SUBDIR_NAME}
            COMPONENT Headers   )
endif()
