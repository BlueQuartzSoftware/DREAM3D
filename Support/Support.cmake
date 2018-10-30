
#-------------------------------------------------------------------------------
#- This copies all the Prebuilt Pipeline files into the Build directory so the help
#- works from the Build Tree
add_custom_target(PrebuiltPipelinesCopy ALL
            COMMAND ${CMAKE_COMMAND} -E copy_directory ${DREAM3D_SUPPORT_DIR}/PrebuiltPipelines
            ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${CMAKE_CFG_INTDIR}/PrebuiltPipelines/
            COMMENT "Copying Prebuilt Pipelines into Binary Directory")
set_target_properties(PrebuiltPipelinesCopy PROPERTIES FOLDER ZZ_COPY_FILES)

# Make Sure this target as part of the DREAM3D Build
# add_dependencies(DREAM3D PrebuiltPipelinesCopy)

#- This installs all the prebuilt pipeline files.
if(APPLE)
  set(INSTALL_DESTINATION "${DREAM3D_PACKAGE_DEST_PREFIX}/Resources")
else()
  set(INSTALL_DESTINATION "${DREAM3D_PACKAGE_DEST_PREFIX}")
endif()
install(DIRECTORY ${DREAM3D_SUPPORT_DIR}/PrebuiltPipelines
        DESTINATION ${INSTALL_DESTINATION}
        COMPONENT Applications )


#-------------------------------------------------------------------------------
# Copy the Data Folder to the build directory
set(DATA_DEST_DIR "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${CMAKE_CFG_INTDIR}/Data/")
# if(NOT APPLE AND NOT WIN32)
#   set(DATA_DEST_DIR "${DREAM3DProj_BINARY_DIR}/Data/")
# endif()
add_custom_target(DataFolderCopy ALL
            COMMAND ${CMAKE_COMMAND} -E copy_directory ${DREAM3D_DATA_DIR}/Data
            ${DATA_DEST_DIR}
            COMMENT "Copying Data Folder into Binary Directory")
set_target_properties(DataFolderCopy PROPERTIES FOLDER ZZ_COPY_FILES)


set(DREAM3D_DATA_DIRECTORIES
  ${DREAM3D_DATA_DIR}/Data/SmallIN100 
  ${DREAM3D_DATA_DIR}/Data/Image 
  ${DREAM3D_DATA_DIR}/Data/Textured_Copper
  ${DREAM3D_DATA_DIR}/Data/Models
)

foreach(data_dir ${DREAM3D_DATA_DIRECTORIES})
  if(EXISTS ${data_dir})
    set(INSTALL_DESTINATION "Data")
    install(DIRECTORY 
          ${data_dir}
          DESTINATION ${INSTALL_DESTINATION}
          COMPONENT Applications)
  endif()
endforeach()



