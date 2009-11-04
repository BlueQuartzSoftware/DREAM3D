# -------- Function to build OS X Stand Alone Bundles -----------------
macro(MakeOSXBundleApp target binary_dir osx_tools_dir)
    set(target ${target})
    set(binary_dir ${binary_dir})
    set(osx_tools_dir ${osx_tools_dir})

  SET (OSX_MAKE_STANDALONE_BUNDLE_CMAKE_SCRIPT "${binary_dir}/OS_X_Scripts/${target}_OSX_MakeStandAloneBundle.cmake")
  SET (OSX_MAKE_STANDALONE_BUNDLE_BASH_SCRIPT "${binary_dir}/OS_X_Scripts/${target}_OSX_MakeStandAloneBundle.sh")

  CONFIGURE_FILE("${osx_tools_dir}/CompleteBundle.cmake.in"
    "${OSX_MAKE_STANDALONE_BUNDLE_CMAKE_SCRIPT}" @ONLY IMMEDIATE)
 
  install(SCRIPT "${OSX_MAKE_STANDALONE_BUNDLE_CMAKE_SCRIPT}")
                    
endmacro(MakeOSXBundleApp)


# -------- Function to build OS X Stand Alone Bundles -----------------
macro(MakeOSXTool target binary_dir osx_tools_dir)
    set(target ${target})
    set(binary_dir ${binary_dir})
    set(osx_tools_dir ${osx_tools_dir})

  SET (OSX_MAKE_STANDALONE_TOOL_CMAKE_SCRIPT "${binary_dir}/OS_X_Scripts/${target}_OSX_MakeStandAloneTool.cmake")

  CONFIGURE_FILE("${osx_tools_dir}/CompleteTool.cmake.in"
    "${OSX_MAKE_STANDALONE_TOOL_CMAKE_SCRIPT}" @ONLY IMMEDIATE)
 
  install(SCRIPT "${OSX_MAKE_STANDALONE_TOOL_CMAKE_SCRIPT}")
                    
endmacro(MakeOSXTool)




#-------------------------------------------------------------------------------
# This macro will set all the variables necessary to have a "good" OS X Application
# bundle. The variables are as follows:
#  PROJECT_NAME - which can be taken from the ${PROJECT_NAME} variable is needed
#  DEBUG_EXTENSION - The extension used to denote a debug built Application. Typically
#   this is '_debug'
#  ICON_FILE_PATH - The complete path to the bundle icon file
#  VERSION_STRING - The version string that you wish to use for the bundle. For OS X
#   this string is usually XXXX.YY.ZZ in type. Look at the Apple docs for more info
#-------------------------------------------------------------------------------
macro(ConfigureMacOSXBundlePlist PROJECT_NAME DEBUG_EXTENSION ICON_FILE_PATH VERSION_STRING)
  IF(CMAKE_BUILD_TYPE MATCHES "Release")
    SET(DBG_EXTENSION "")
  else()
    set(DBG_EXTENSION ${DEBUG_EXTENSION})
  endif()
  get_filename_component(ICON_FILE_NAME ${ICON_FILE_PATH} NAME)

 #CFBundleGetInfoString
 SET(MACOSX_BUNDLE_INFO_STRING "${PROJECT_NAME}${DBG_EXTENSION} Version ${VERSION_STRING}, Copyright 2009 BlueQuartz Software.")
 SET(MACOSX_BUNDLE_ICON_FILE ${ICON_FILE_NAME})
 SET(MACOSX_BUNDLE_GUI_IDENTIFIER "${PROJECT_NAME}${DBG_EXTENSION}")
 #CFBundleLongVersionString
 SET(MACOSX_BUNDLE_LONG_VERSION_STRING "${PROJECT_NAME}${DBG_EXTENSION} Version ${VERSION_STRING}")
 SET(MACOSX_BUNDLE_BUNDLE_NAME ${PROJECT_NAME}${DBG_EXTENSION})
 SET(MACOSX_BUNDLE_SHORT_VERSION_STRING ${VERSION_STRING})
 SET(MACOSX_BUNDLE_BUNDLE_VERSION ${VERSION_STRING})
 SET(MACOSX_BUNDLE_COPYRIGHT "Copyright 2009, BlueQuartz Software. All Rights Reserved.")
 
 SET(${PROJECT_NAME}_PROJECT_SRCS ${${PROJECT_NAME}_PROJECT_SRCS} ${ICON_FILE_PATH})
 SET_SOURCE_FILES_PROPERTIES(${ICON_FILE_PATH} PROPERTIES
                             MACOSX_PACKAGE_LOCATION Resources)
endmacro()

