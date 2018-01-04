set(RefManual_Docs_AuxiliaryTools
    DevHelper
    PipelineRunner
  )

set(RefManual_Docs_Developer
    CodeGuidelines
    CompilerSuite
    CreatingAFilter
    Linux_Configuring_and_Building_DREAM3D
    OSX_Configuring_and_Building_DREAM3D
    RepositoryLayout
    SDKInstall
    SourceDownload
    Windows_Configuring_and_Building_DREAM3D
    WritingAFilter
  )

set(RefManual_Docs_ReleaseNotes
    ReleaseNotes_6_0_0
    ReleaseNotes_6_1_0
    ReleaseNotes_6_2_0
    ReleaseNotes_6_3_0
    ReleaseNotes_6_4_0
  )

set(RefManual_Docs_SupportedFileFormats
    H5EbsdFile
    MiscFileFormats
    NativeDREAM3DFileFormat
    XdmfForVisualization
  )

set(RefManual_Docs_Tutorials
    CroppingData
    EBSDReconstruction
    SurfaceMeshing
    SyntheticSinglePhase 
  )

set(RefManual_Docs_UsingDREAM3D
    Acknowledgements
    Contributors
    CreatingPipeline
    DataStructure
    ImportExportPipelines
    UserInterface
  )

set(RefManual_Groups 
      AuxiliaryTools 
      Developer 
      ReleaseNotes 
      SupportedFileFormats 
      Tutorials 
      UsingDREAM3D
    )


# ------------------------------------------------------------------------------
# This is an experimental tool that will convert all the markdown documents to
# html with styling provided by CSS. This is an experimental way of removing the
# DOxygen generated docs with it's insecure jquery.js file.
option(SIMPL_DISCOUNT_DOCUMENTATION "Use Discount to generate the Documentation" OFF)
if(SIMPL_DISCOUNT_DOCUMENTATION)

  find_package(discount)
  if(discount_FOUND)
    message(STATUS "Discount Markdown->HTML library found.")

    # -----------------------------------------------------------------------------
    # This is example code that loops over all the DocGroups and each .md file within
    # that group.
    set(PluginJsonFile "${DREAM3DDocRoot}/ReferenceManual/PluginIndex.json")
    file(WRITE "${PluginJsonFile}" "{\n")
    file(APPEND "${PluginJsonFile}" "  \"Plugins\": {\n")

    get_property(DREAM3DDoc_GROUPS GLOBAL PROPERTY DREAM3DDoc_GROUPS)
    set(NEED_COMMA_DOC_GROUP 0)
    foreach(DocGroup ${DREAM3DDoc_GROUPS})
      #message(STATUS "  DocGroup: ${${DocGroup}_SOURCE_DIR}" )
      
      if(NEED_COMMA_DOC_GROUP)
        file(APPEND "${PluginJsonFile}" " ,\n")
      endif()
      if(NOT NEED_COMMA_DOC_GROUP)
        set(NEED_COMMA_DOC_GROUP 1)
      endif()
      file(APPEND "${PluginJsonFile}" "  \"${DocGroup}\": { \n")
      file(APPEND "${PluginJsonFile}" "    \"SOURCE_DIR\": \"${${DocGroup}_SOURCE_DIR}\",\n")
      file(APPEND "${PluginJsonFile}" "    \"DOC_DIR\": \"Documentation/${DocGroup}Filters\",\n")
      if(EXISTS "${${DocGroup}_SOURCE_DIR}/Documentation/${DocGroup}Filters/Images")
        file(APPEND "${PluginJsonFile}" "    \"IMAGES_DIR\": \"Documentation/${DocGroup}Filters/Images\",\n")
      endif()
      file(APPEND "${PluginJsonFile}" "    \"FILTERS\": [\n")

      get_property(DocGroupList GLOBAL PROPERTY DREAM3DDoc_${DocGroup}Filters)
      set(NEED_COMMA_DOC_ITEM 0)
      foreach(docListItem ${DocGroupList})
        if(NEED_COMMA_DOC_ITEM)
          file(APPEND "${PluginJsonFile}" "  ,\n")
        endif()
        if(NOT NEED_COMMA_DOC_ITEM)
          set(NEED_COMMA_DOC_ITEM 1)
        endif()

        if(${DocGroup} STREQUAL "Core" OR ${DocGroup} STREQUAL "Test")
          string(REPLACE "/${DocGroup}Filters" "/Documentation/SIMPLibFilters" tempstring ${${DocGroup}_SOURCE_DIR})
          string(REPLACE "${tempstring}" "Plugins" tempstring ${docListItem})
          string(REPLACE "Filters" "" tempstring ${tempstring})
          string(REPLACE "Plugins/${DocGroup}/" "" filterName ${tempstring})
          string(REPLACE ".md" "" filterName ${filterName})
          string(REPLACE ".md" ".html" tempstring ${tempstring})

          file(APPEND "${PluginJsonFile}" "        \"${filterName}\"")
        else()
          string(REPLACE "${${DocGroup}_SOURCE_DIR}/Documentation/" "" tempstring ${docListItem})
          string(REPLACE "Filters" "" tempstring ${tempstring})
          string(REPLACE "${DocGroup}/" "" filterName ${tempstring})
          string(REPLACE ".md" "" filterName ${filterName})
          string(REPLACE ".md" ".html" tempstring ${tempstring})
          file(APPEND "${PluginJsonFile}" "        \"${filterName}\"")
        endif()
      endforeach()
      file(APPEND "${PluginJsonFile}" "    ]\n")
      file(APPEND "${PluginJsonFile}" "\n  }")
    endforeach()
    file(APPEND "${PluginJsonFile}" "\n  } ,")
    #------------------------------------------------------------------------------
    # This section of code generates the JSON file that has everything else from the
    # documentation set
    file(APPEND "${PluginJsonFile}" "\n  \"ReferenceManual\": {\n")
    set(NEED_COMMA_DOC_GROUP 0)
    foreach(DocGroup ${RefManual_Groups})
      if(NEED_COMMA_DOC_GROUP)
        file(APPEND "${PluginJsonFile}" " ,\n")
      endif()
      if(NOT NEED_COMMA_DOC_GROUP)
        set(NEED_COMMA_DOC_GROUP 1)
      endif()
      file(APPEND "${PluginJsonFile}" "    \"${DocGroup}\": { \n")
      file(APPEND "${PluginJsonFile}" "      \"SOURCE_DIR\": \"${DREAM3DProj_SOURCE_DIR}/Documentation/ReferenceManual\",\n")
      file(APPEND "${PluginJsonFile}" "      \"DOC_DIR\": \"${DocGroup}\",\n")
      if(EXISTS "${DREAM3DProj_SOURCE_DIR}/Documentation/ReferenceManual/${DocGroup}/Images")
        file(APPEND "${PluginJsonFile}" "      \"IMAGES_DIR\": \"${DocGroup}/Images\",\n")
      endif()
      file(APPEND "${PluginJsonFile}" "      \"FILTERS\": [\n")
      
      set(NEED_COMMA_DOC_ITEM 0)
      foreach(docListItem ${RefManual_Docs_${DocGroup}})
        if(NEED_COMMA_DOC_ITEM)
          file(APPEND "${PluginJsonFile}" "  ,\n")
        endif()
        if(NOT NEED_COMMA_DOC_ITEM)
          set(NEED_COMMA_DOC_ITEM 1)
        endif()
        file(APPEND "${PluginJsonFile}" "        \"${docListItem}\"")
      endforeach()
      file(APPEND "${PluginJsonFile}" "    ]\n")
      file(APPEND "${PluginJsonFile}" "  }")
    endforeach()
    #file(APPEND "${PluginJsonFile}" "\n  }")

    file(APPEND "${PluginJsonFile}" "   ,\n")
    file(APPEND "${PluginJsonFile}" "  \"Index\": { \n")
    file(APPEND "${PluginJsonFile}" "      \"SOURCE_DIR\": \"${DREAM3DProj_SOURCE_DIR}/Documentation/ReferenceManual\",\n")
    file(APPEND "${PluginJsonFile}" "      \"DOC_DIR\": \"\",\n")
    file(APPEND "${PluginJsonFile}" "      \"IMAGES_DIR\": \"Images\",\n")
    file(APPEND "${PluginJsonFile}" "      \"FILTERS\": [\n")
    file(APPEND "${PluginJsonFile}" "        \"Index\"")
    file(APPEND "${PluginJsonFile}" "       ]\n")
    file(APPEND "${PluginJsonFile}" "    }")
    file(APPEND "${PluginJsonFile}" "\n }")
    # This Bit of JSON is the last part of the file
    file(APPEND "${PluginJsonFile}" "\n}\n")

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



    if(APPLE)
      set(HTML_OUTPUT_DIR "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${DREAM3D_PACKAGE_DEST_PREFIX}Resources/Help/DREAM3D")
    else()
      set(HTML_OUTPUT_DIR "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Help/DREAM3D")
    endif()


    configure_file( ${DREAM3DProj_SOURCE_DIR}/Documentation/Tools/ToolConfiguration.h.in
                    ${DREAM3DProj_BINARY_DIR}/Documentation/ReferenceManual/DocToolsConfiguration.h)


    add_executable(GenerateHTMLDocs
      ${DREAM3DProj_SOURCE_DIR}/Documentation/Tools/GenerateHTMLDocs.cpp 
      ${DREAM3DProj_BINARY_DIR}/Documentation/Tools/${PROJECT_NAME}Configuration.h
    )
    target_include_directories(GenerateHTMLDocs INTERFACE  discount::libmarkdown)
    target_link_libraries(GenerateHTMLDocs Qt5::Core discount::libmarkdown SIMPLib)
    #-- Set the Debug Suffix for the application
    set_target_properties( GenerateHTMLDocs
                PROPERTIES
                DEBUG_OUTPUT_NAME GenerateHTMLDocs${EXE_DEBUG_EXTENSION}
                RELEASE_OUTPUT_NAME GenerateHTMLDocs
    )

    add_custom_target(GenerateDiscountDocs ALL
      COMMAND GenerateHTMLDocs
      DEPENDS DREAM3D
      COMMENT "Using Discount to generate HTML Documentation"
      )

  else()
    set(SIMPL_DISCOUNT_DOCUMENTATION OFF)
    message(STATUS "Discount package was NOT found. Documentation will NOT be generated")
  endif()

endif()

# -----------------------------------------------------------------------------
# Use DOXYGEN to Build the documentation
if(SIMPL_DOXYGEN_DOCUMENTATION)
  
  set(DOX_GENERATE_RTF "NO")
  option(SIMPL_GENERATE_RTF "Generate RTF Documentation" OFF)
  if(SIMPL_GENERATE_RTF STREQUAL "ON")
    set(DOX_GENERATE_RTF "YES")
  endif()

  # By default we generate the Documentation in HTML format
  set(DOX_GENERATE_HTML "NO")
  option(SIMPL_GENERATE_HTML "Generate HTML Documentation" ON)
  if(SIMPL_GENERATE_HTML STREQUAL "ON")
    set(DOX_GENERATE_HTML "YES")
  endif()

  #most windows machines do NOT have LATEX built in so default this to OFF
  set(DOX_GENERATE_LATEX "NO")
  option(SIMPL_GENERATE_LATEX "Generate PDF Documentation" OFF)
  if(SIMPL_GENERATE_LATEX STREQUAL "ON")
    set(DOX_GENERATE_LATEX "YES")
    # Check for existence of LaTex executable files
    include( ${DOCUMENTATION_RESOURCES_DIR}/CMake/UseLATEX.cmake)
    if(APPLE)
      option(AUTO_OPEN_PDF "Automatically open generated PDF files with default PDF reading application" ON)
    endif()
  endif()

  PROJECT(DREAM3DReferenceManual)
  cmake_minimum_required(VERSION 3.8.0)

  if(NOT DEFINED PROJECT_RESOURCES_DIR)
      set(PROJECT_RESOURCES_DIR ${PROJECT_SOURCE_DIR}/Resources)
  endif()

  set(DOCUMENTATION_SOURCE_FILES \"${PROJECT_SOURCE_DIR}/Index.dox\")
  set(DOXYGEN_IMAGE_DIRS "")

  foreach(DocGroup ${RefManual_Groups})
    list(APPEND DOCUMENTATION_SOURCE_FILES \"${PROJECT_SOURCE_DIR}/${DocGroup}/${DocGroup}.dox\" )
    if(EXISTS "${PROJECT_SOURCE_DIR}/${DocGroup}/Images")
      list(APPEND DOXYGEN_IMAGE_DIRS \"${PROJECT_SOURCE_DIR}/${DocGroup}/Images\")
    endif()    
    foreach(docListItem ${RefManual_Docs_${DocGroup}})
      #message(STATUS "docListItem: ${docListItem}")
      list(APPEND DOCUMENTATION_SOURCE_FILES \"${PROJECT_SOURCE_DIR}/${DocGroup}/${docListItem}.md\" )
    endforeach()

  endforeach()


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
    # message(STATUS "=======>   DOxygen Group: ${group}")

    string(TOLOWER ${group} filterGroup_Lower)
    #file(STRINGS ${DREAM3DDocRoot}/DREAM3DDoc_${group} filter_path_list)
    get_property(filter_path_list GLOBAL PROPERTY DREAM3DDoc_${group}Filters)
    # message(STATUS "filter_path_list: ${filter_path_list}")
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
