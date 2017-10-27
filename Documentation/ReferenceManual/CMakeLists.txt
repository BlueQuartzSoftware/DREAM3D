#--////////////////////////////////////////////////////////////////////////////
#-- Copyright (c) 2009, Michael A. Jackson. BlueQuartz Software
#-- All rights reserved.
#-- BSD License: http://www.opensource.org/licenses/bsd-license.html
#-- This code was partly written under US Air Force Contract FA8650-07-D-5800
#--////////////////////////////////////////////////////////////////////////////
# -----------------------------------------------------------------------------

set(DREAM3D_USE_DOXYGEN ON)
set(DREAM3D_USE_MKDOCS OFF)

# -----------------------------------------------------------------------------
# Use MKDOCS to build the docs
if(DREAM3D_USE_MKDOCS)

  get_property(DREAM3DDocRoot GLOBAL PROPERTY DREAM3DDocRoot)
  add_custom_target(REFMANUAL_DOC_FOLDER_COPY ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${DREAM3DProj_SOURCE_DIR}/Documentation/ReferenceManual/"
    "${DREAM3DDocRoot}/ReferenceManual/"
    COMMENT "REFMANUAL_DOC_FOLDER_COPY: Copying Documentation to build directory")
  set_target_properties(REFMANUAL_DOC_FOLDER_COPY PROPERTIES FOLDER ZZ_COPY_FILES)


  configure_file(${DREAM3DProj_SOURCE_DIR}/Documentation/mkdocs.yml
    ${DREAM3DDocRoot}/mkdocs.yml 
    )

  configure_file("${BrandedSIMPLView_DIR}/icons/DREAM3D (PNG)/128x128.png"
    ${DREAM3DDocRoot}/ReferenceManual/logo.png COPYONLY
    )


  get_filename_component(WORKSPACE_DIR "${DREAM3DProj_SOURCE_DIR}" DIRECTORY)  
  if(EXISTS "${WORKSPACE_DIR}/DREAM3DSuperbuild")
    add_custom_target(SUPERBUILD_DOC_FOLDER_COPY ALL
      COMMAND ${CMAKE_COMMAND} -E copy_directory "${WORKSPACE_DIR}/DREAM3DSuperbuild/docs"
      "${DREAM3DDocRoot}/ReferenceManual/Developer/"
      COMMENT "SUPERBUILD_DOC_FOLDER_COPY: Copying Documentation to build directory")
    set_target_properties(SUPERBUILD_DOC_FOLDER_COPY PROPERTIES FOLDER ZZ_COPY_FILES)

  endif()

endif()

# -----------------------------------------------------------------------------
# Use DOXYGEN to Build the documentation
if(DREAM3D_USE_DOXYGEN)

  PROJECT(DREAM3DReferenceManual)
  cmake_minimum_required(VERSION 3.6.0)

  if(NOT DEFINED PROJECT_RESOURCES_DIR)
  set(PROJECT_RESOURCES_DIR ${PROJECT_SOURCE_DIR}/Resources)
  endif()

  set(DOCUMENTATION_RESOURCES_DIR ${PROJECT_RESOURCES_DIR}/DocConfigFiles)


  #-------------------------------------------------------------------------------
  # Create the initial list of Files for DOxygen to process
  set(DOCUMENTATION_SOURCE_FILES
    \"${PROJECT_SOURCE_DIR}/Index.dox\"
    \"${PROJECT_SOURCE_DIR}/Acknowledgements.md\"
    \"${PROJECT_SOURCE_DIR}/DataStructure.md\"
    \"${PROJECT_SOURCE_DIR}/UsingDREAM3D/UsingDREAM3D.dox\"
    \"${PROJECT_SOURCE_DIR}/UsingDREAM3D/UserInterface.md\"
    \"${PROJECT_SOURCE_DIR}/UsingDREAM3D/CreatingPipeline.md\"
    \"${PROJECT_SOURCE_DIR}/UsingDREAM3D/ImportExportPipelines.md\"
    \"${PROJECT_SOURCE_DIR}/SupportedFileFormats/SupportedFileFormats.dox\"
    \"${PROJECT_SOURCE_DIR}/SupportedFileFormats/NativeDREAM3DFileFormat.md\"
    \"${PROJECT_SOURCE_DIR}/SupportedFileFormats/XdmfForVisualization.md\"
    \"${PROJECT_SOURCE_DIR}/SupportedFileFormats/MiscFileFormats.md\"
    \"${PROJECT_SOURCE_DIR}/SupportedFileFormats/H5EbsdFile.md\"
    \"${PROJECT_SOURCE_DIR}/Tutorials/Tutorials.dox\"
    \"${PROJECT_SOURCE_DIR}/Tutorials/EBSDReconstruction.md\"
    \"${PROJECT_SOURCE_DIR}/Tutorials/SurfaceMeshing.md\"
    \"${PROJECT_SOURCE_DIR}/Tutorials/SyntheticSinglePhase.md\"
    \"${PROJECT_SOURCE_DIR}/Tutorials/CroppingData.md\"
    \"${PROJECT_SOURCE_DIR}/AuxiliaryTools/DevHelper.md\"
    \"${PROJECT_SOURCE_DIR}/AuxiliaryTools/PipelineRunner.md\"
    \"${PROJECT_SOURCE_DIR}/AuxiliaryTools/AuxiliaryTools.dox\"
    \"${PROJECT_SOURCE_DIR}/ReleaseNotes/ReleaseNotes.dox\"
    \"${PROJECT_SOURCE_DIR}/ReleaseNotes/ReleaseNotes_6_0_0.md\"
    \"${PROJECT_SOURCE_DIR}/ReleaseNotes/ReleaseNotes_6_1_0.md\"
    \"${PROJECT_SOURCE_DIR}/ReleaseNotes/ReleaseNotes_6_2_0.md\"
    \"${PROJECT_SOURCE_DIR}/ReleaseNotes/ReleaseNotes_6_3_0.md\"
    \"${PROJECT_SOURCE_DIR}/ReleaseNotes/ReleaseNotes_6_4_0.md\"
  )

  option(DREAM3D_INCLUDE_DEVELOPER_DOCS "Include the Developer Documentation in the DOxygen Generated output." ON)
  if(DREAM3D_INCLUDE_DEVELOPER_DOCS)
    set(DOCUMENTATION_SOURCE_FILES "${DOCUMENTATION_SOURCE_FILES}"
      \"${PROJECT_SOURCE_DIR}/Developer/CodeGuidelines.md\"
      \"${PROJECT_SOURCE_DIR}/Developer/CompilerSuite.md\"
      \"${PROJECT_SOURCE_DIR}/Developer/CreatingAFilter.md\"
      \"${PROJECT_SOURCE_DIR}/Developer/Developer.dox\"
      \"${PROJECT_SOURCE_DIR}/Developer/Linux_Configuring_and_Building_DREAM3D.md\"
      \"${PROJECT_SOURCE_DIR}/Developer/OSX_Configuring_and_Building_DREAM3D.md\"
      \"${PROJECT_SOURCE_DIR}/Developer/RepositoryLayout.md\"
      \"${PROJECT_SOURCE_DIR}/Developer/SDKInstall.md\"
      \"${PROJECT_SOURCE_DIR}/Developer/SourceDownload.md\"
      \"${PROJECT_SOURCE_DIR}/Developer/Windows_Configuring_and_Building_DREAM3D.md\"
      \"${PROJECT_SOURCE_DIR}/Developer/WritingAFilter.md\"
    )
  endif()

  set(DOXYGEN_IMAGE_DIRS
    \"${PROJECT_SOURCE_DIR}/Images\"
    \"${PROJECT_SOURCE_DIR}/SupportedFileFormats/Images\"
    \"${PROJECT_SOURCE_DIR}/UsingDREAM3D/Images\"
    \"${PROJECT_SOURCE_DIR}/Tutorials/Images\"
    \"${PROJECT_SOURCE_DIR}/Tutorials/CroppingImages\"
    \"${PROJECT_SOURCE_DIR}/AuxiliaryTools/Images\"
    \"${PROJECT_SOURCE_DIR}/AuxiliaryTools/Images/DevHelper\"
    \"${PROJECT_SOURCE_DIR}/Developer/Images\"
    )

  #-------------------------------------------------------------------------------
  # Read the list of Groups back into CMake
  get_property(DREAM3DDocRoot GLOBAL PROPERTY DREAM3DDocRoot)
  get_property(DREAM3D_DOC_GROUPS_FILE_LIST GLOBAL PROPERTY DREAM3DDoc_GROUPS)


  set(temp_string "/*!\n\n@page filterdocumentation Filter Documentation\n\n")

  foreach( group ${DREAM3D_DOC_GROUPS_FILE_LIST} )
    #message(STATUS "=======>   DOxygen Group: ${group}")
    string(TOLOWER ${group} filterGroup_Lower)
    set(temp_string "${temp_string}@subpage ${filterGroup_Lower}\n\n")
  endforeach()
  set(temp_string "${temp_string}\n*/\n\n")
  file(WRITE ${DREAM3DReferenceManual_BINARY_DIR}/FilterDocumentation.dox "${temp_string}")


  #-------------------------------------------------------------------------------
  # Now Append the list of filters for each Group into the same file
  foreach( group ${DREAM3D_DOC_GROUPS_FILE_LIST} )
    string(TOLOWER ${group} filterGroup_Lower)
    #file(STRINGS ${DREAM3DDocRoot}/DREAM3DDoc_${group} filter_path_list)
    get_property(filter_path_list GLOBAL PROPERTY DREAM3DDoc_${group})

    file(APPEND ${DREAM3DReferenceManual_BINARY_DIR}/FilterDocumentation.dox "/*!\n@page ${filterGroup_Lower} ${group}\n\n")
    # Read the summary for the group if it exists
    if(EXISTS ${DREAM3DReferenceManual_SOURCE_DIR}/Filters/${group}/${group}.md)
      file(READ ${DREAM3DReferenceManual_SOURCE_DIR}/Filters/${group}/${group}.md Group_Summary)
      file(APPEND ${DREAM3DReferenceManual_BINARY_DIR}/FilterDocumentation.dox ${Group_Summary})
    endif()
    foreach(filterPath ${filter_path_list})
      get_filename_component(filter  ${filterPath} NAME_WE)
      get_filename_component(group_path ${filterPath} PATH)
      if(EXISTS "${group_path}/Images")
        list(APPEND DOXYGEN_IMAGE_DIRS "\"${group_path}/Images\"")
      endif()
      string(TOLOWER ${filter} filter_Lower)
      file(APPEND ${DREAM3DReferenceManual_BINARY_DIR}/FilterDocumentation.dox "@subpage ${filter_Lower}\n\n")
      list(APPEND DOCUMENTATION_SOURCE_FILES "\"${filterPath}\"")
    endforeach()
    list(REMOVE_DUPLICATES DOXYGEN_IMAGE_DIRS)
    file(APPEND ${DREAM3DReferenceManual_BINARY_DIR}/FilterDocumentation.dox "*/\n\n")
  endforeach()

  set(DOCUMENTATION_SOURCE_FILES
      "${DOCUMENTATION_SOURCE_FILES}"
       "\"${DREAM3DReferenceManual_BINARY_DIR}/FilterDocumentation.dox\""
  )



  include(${DOCUMENTATION_RESOURCES_DIR}/CMake/Macros.cmake)

  #---- Set some LATEX Varaibles
  set(LATEX_TITLE "DREAM.3D v6.0\\\\[0.45cm]User Manual and Filter Reference")
  set(LATEX_AUTHOR "DREAM3D Development Team")


  set(LATEX_EPS_FILES

  )



  if(DOX_GENERATE_LATEX)
    ADD_DOXYGEN_TARGETS(PDF_FILENAME "DREAM3D_v6_User_Manual"
                       PROJECT_NAME  "${PROJECT_NAME}_LATEX"
                       PROJECT_BINARY_DIR "${PROJECT_BINARY_DIR}_LATEX"
                       SOURCE_FILES "${DOCUMENTATION_SOURCE_FILES}"
                       EPS_FILES "${LATEX_EPS_FILES}"
                       TITLE "${LATEX_TITLE}"
                       AUTHOR "${LATEX_AUTHOR}"
                       GENERATE_HTML "NO"
                       GENERATE_LATEX "YES"
                       GENERATE_RTF "NO"
                       TEMPLATE_DIR "${DOCUMENTATION_RESOURCES_DIR}"
                       IMAGE_DIR "${DOXYGEN_IMAGE_DIRS}"
                       OUTPUT_DIR "${PROJECT_BINARY_DIR}_LATEX"
                       OUTPUT_SUB_DIR "latex")
  endif()

  set(DOCUMENTATION_SOURCE_FILES
          \"${PROJECT_SOURCE_DIR}/Index.dox\"
          ${DOCUMENTATION_SOURCE_FILES})


  if(DOX_GENERATE_HTML)
    if(APPLE)
      set(OUTPUT_DIR "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${DREAM3D_PACKAGE_DEST_PREFIX}/Resources/Help")
      install(DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${DREAM3D_PACKAGE_DEST_PREFIX}/Resources/Help/DREAM3D
          DESTINATION ${DREAM3D_PACKAGE_DEST_PREFIX}Resources/Help
          COMPONENT Applications )
    else()
      set(OUTPUT_DIR "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Help")
    endif()

    ADD_DOXYGEN_TARGETS(PDF_FILENAME "DREAM.3D v6 User Manual"
                       PROJECT_NAME  "${PROJECT_NAME}_HTML"
                       PROJECT_BINARY_DIR "${PROJECT_BINARY_DIR}"
                       SOURCE_FILES "${DOCUMENTATION_SOURCE_FILES}"
                       EPS_FILES "${LATEX_EPS_FILES}"
                       TITLE "${LATEX_TITLE}"
                       AUTHOR "${LATEX_AUTHOR}"
                       GENERATE_HTML "YES"
                       GENERATE_LATEX "NO"
                       GENERATE_RTF "NO"
                       TEMPLATE_DIR "${DOCUMENTATION_RESOURCES_DIR}"
                       IMAGE_DIR "${DOXYGEN_IMAGE_DIRS}"
                       OUTPUT_DIR ${OUTPUT_DIR}
                       OUTPUT_SUB_DIR "DREAM3D")
  endif()

  if(DOX_GENERATE_RTF)
    ADD_DOXYGEN_TARGETS(PDF_FILENAME ${PROJECT_NAME}
                       PROJECT_NAME  "${PROJECT_NAME}_RTF"
                       PROJECT_BINARY_DIR "${PROJECT_BINARY_DIR}"
                       SOURCE_FILES "${DOCUMENTATION_SOURCE_FILES}"
                       EPS_FILES "${LATEX_EPS_FILES}"
                       TITLE "${LATEX_TITLE}"
                       AUTHOR "${LATEX_AUTHOR}"
                       GENERATE_HTML "NO"
                       GENERATE_LATEX "NO"
                       GENERATE_RTF "YES"
                       TEMPLATE_DIR "${DOCUMENTATION_RESOURCES_DIR}"
                       IMAGE_DIR "${DOXYGEN_IMAGE_DIRS}"
                       OUTPUT_DIR "${PROJECT_BINARY_DIR}/DREAM3D_RTF")
  endif()


  #-------------------------------------------------------------------------------
  # Make Sure this target is built so that the DREAM3D Application has access to
  # the HTML help files
  if (TARGET DREAM3DReferenceManual_HTML AND TARGET DREAM3D)
    add_dependencies(DREAM3D DREAM3DReferenceManual_HTML)
  endif()


  #- This installs all the help files. Note that we assume a flat directory structure
  #- in the "Help" folder, ie, all html files, images, etc are all in the "Help"
  #- folder and NOT in any subfolders beyond that.
  file(MAKE_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Help/DREAM3D)
  if(NOT APPLE)
    install(DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Help/DREAM3D
          DESTINATION ${INSTALL_DIR_PREFIX}Help/
          COMPONENT Applications )
  endif()

endif()
