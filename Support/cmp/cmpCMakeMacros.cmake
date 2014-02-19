#--////////////////////////////////////////////////////////////////////////////
#--  Copyright (c) 2009, 2010 Michael A. Jackson. BlueQuartz Software
#--  All rights reserved.
#--  BSD License: http://www.opensource.org/licenses/bsd-license.html
#--////////////////////////////////////////////////////////////////////////////

include (CMakeParseArguments)
# include(${CMP_OSX_TOOLS_SOURCE_DIR}/OSX_BundleTools.cmake)
# include(${CMP_OSX_TOOLS_SOURCE_DIR}/ToolUtilities.cmake)

#-------------------------------------------------------------------------------
MACRO (cmp_IDE_GENERATED_PROPERTIES SOURCE_PATH HEADERS SOURCES)
    STRING(REPLACE "/" "\\\\" source_group_path ${SOURCE_PATH}  )
    source_group(${source_group_path} FILES ${HEADERS} ${SOURCES})

  #-- The following is needed if we ever start to use OS X Frameworks but only
  #--  works on CMake 2.6 and greater
  #set_property(SOURCE ${HEADERS}
  #             PROPERTY MACOSX_PACKAGE_LOCATION Headers/${NAME}
  #)

ENDMACRO (cmp_IDE_GENERATED_PROPERTIES SOURCE_PATH HEADERS SOURCES)

#-------------------------------------------------------------------------------

MACRO (cmp_IDE_SOURCE_PROPERTIES SOURCE_PATH HEADERS SOURCES INSTALL_FILES)
    if(${INSTALL_FILES} EQUAL "1")
        INSTALL (FILES ${HEADERS}
                 DESTINATION include/${SOURCE_PATH}
                 COMPONENT Headers
        )
    endif()
    STRING(REPLACE "/" "\\\\" source_group_path ${SOURCE_PATH}  )
    source_group(${source_group_path} FILES ${HEADERS} ${SOURCES})

  #-- The following is needed if we ever start to use OS X Frameworks but only
  #--  works on CMake 2.6 and greater
  #set_property(SOURCE ${HEADERS}
  #             PROPERTY MACOSX_PACKAGE_LOCATION Headers/${NAME}
  #)

ENDMACRO (cmp_IDE_SOURCE_PROPERTIES NAME HEADERS SOURCES INSTALL_FILES)

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
macro(ConfigureMacOSXBundlePlist TARGET_NAME DEBUG_EXTENSION ICON_FILE_PATH VERSION_STRING)
  # message(STATUS "ConfigureMacOSXBundlePlist for ${PROJECT_NAME} ")
  if(CMAKE_BUILD_TYPE MATCHES "Release")
    SET(DBG_EXTENSION "")
  else()
    set(DBG_EXTENSION ${DEBUG_EXTENSION})
  endif()
  get_filename_component(ICON_FILE_NAME "${ICON_FILE_PATH}" NAME)

 set_target_properties(${TARGET_NAME} PROPERTIES
     MACOSX_BUNDLE_INFO_STRING "${PROJECT_NAME}${DBG_EXTENSION} Version ${VERSION_STRING}, Copyright 2009 BlueQuartz Software."
     MACOSX_BUNDLE_ICON_FILE ${ICON_FILE_NAME}
     MACOSX_BUNDLE_GUI_IDENTIFIER "${PROJECT_NAME}${DBG_EXTENSION}"
     MACOSX_BUNDLE_LONG_VERSION_STRING "${PROJECT_NAME}${DBG_EXTENSION} Version ${VERSION_STRING}"
     MACOSX_BUNDLE_BUNDLE_NAME ${PROJECT_NAME}${DBG_EXTENSION}
     MACOSX_BUNDLE_SHORT_VERSION_STRING ${VERSION_STRING}
     MACOSX_BUNDLE_BUNDLE_VERSION ${VERSION_STRING}
     MACOSX_BUNDLE_COPYRIGHT "Copyright 2011, BlueQuartz Software. All Rights Reserved."
 )

 SET(${PROJECT_NAME}_PROJECT_SRCS ${${PROJECT_NAME}_PROJECT_SRCS} ${ICON_FILE_PATH})
 SET_SOURCE_FILES_PROPERTIES(${ICON_FILE_PATH} PROPERTIES
                             MACOSX_PACKAGE_LOCATION Resources)
endmacro()

# --------------------------------------------------------------------
# This function should be able to correctly create an Application bundle
# based on the Qt Frameworks for any platform. There are specific
# sections to ensure that plugins and other libraries and resources are
# correctly copied into the Application Bundle. On other platforms these
# items are copied into the installation directory.
# Arguments:
#  TARGET The name of the Target to use in the Add_Executable() commnad
#  DEBUG_EXTENSION The file name suffix extension that Debug builds will have
#  ICON_FILE The path to the proper icon file for this platform (icns for OS X, ico for windows)
#  VERSION_MAJOR The Major version
#  VERSION_MINOR The Minor version
#  VERSION_PATCH The Patch version
#  BINARY_DIR    The binary directory where some files are created for this application
#  COMPONENT     The name of the component that is used during the packaging
#  INSTALL_DEST  The destination directory inside of the CMAKE_INSTALL_PREFIX to install everything
#
#  SOURCES   All the source files that are needed to compile the code
#  LINK_LIBRARIES Dependent libraries that are needed to properly link the executable
#  LIB_SEARCH_DIRS  A list of directories where certain dependent libraries or plugins can be found
#  QT_PLUGINS A List of Qt Plugins that this project needs
#  OTHER_PLUGINS A list of other plugins that are needed by this Application. These can be those built
#     by this project or located somewhere else.
function(BuildQtAppBundle)
    set(options )
    set(oneValueArgs TARGET DEBUG_EXTENSION ICON_FILE VERSION_MAJOR VERSION_MINOR VERSION_PATCH
                     BINARY_DIR COMPONENT INSTALL_DEST PLUGIN_LIST_FILE)
    set(multiValueArgs SOURCES LINK_LIBRARIES LIB_SEARCH_DIRS QT_PLUGINS OTHER_PLUGINS)
    cmake_parse_arguments(QAB "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )

    # Default GUI type is blank
    set(GUI_TYPE "")

    #-- Configure the OS X Bundle Plist
    if(APPLE)
        SET(GUI_TYPE MACOSX_BUNDLE)
        #-- Make sure the qt_menu.nib is copied if we are using Qt Cocoa by setting the
        # source files properties of the qt_menu.nib package
        if(QT_MAC_USE_COCOA)
            GET_FILENAME_COMPONENT(qt_menu_nib
              "${QT_QTGUI_LIBRARY_RELEASE}/Resources/qt_menu.nib"
              REALPATH)
            set(qt_menu_nib_sources
              "${qt_menu_nib}/classes.nib"
              "${qt_menu_nib}/info.nib"
              "${qt_menu_nib}/keyedobjects.nib"
              )
            SET_SOURCE_FILES_PROPERTIES(
              ${qt_menu_nib_sources}
              PROPERTIES
              MACOSX_PACKAGE_LOCATION Resources/qt_menu.nib
            )
        ELSE(QT_MAC_USE_COCOA)
            set(qt_menu_nib_sources)
        ENDif(QT_MAC_USE_COCOA)

#-- Write out a qt.conf file to place in our App bundle
#        set(qt_conf_file ${${QAB_TARGET}_BINARY_DIR}/qt.conf)
#        file(WRITE ${qt_conf_file})
#        set_source_files_properties(${qt_conf_file}
#                                PROPERTIES
#                                MACOSX_PACKAGE_LOCATION Resources)
#
#        list(APPEND QAB_SOURCES ${qt_menu_nib_sources} ${qt_conf_file})
         list(APPEND QAB_SOURCES ${qt_menu_nib_sources})
    elseif(WIN32)
        #SET(GUI_TYPE WIN32)
        FILE (WRITE "${CMAKE_CURRENT_BINARY_DIR}/Icon.rc"
          "// Icon with lowest ID value placed first to ensure application icon\n"
          "// remains consistent on all systems.\n"
          "IDI_ICON1 ICON \"${QAB_ICON_FILE}\"")
        SET(QAB_ICON_FILE "${CMAKE_CURRENT_BINARY_DIR}/Icon.rc")
        cmp_IDE_GENERATED_PROPERTIES("${TARGET}/Generated/QrcFiles" "${QAB_ICON_FILE}" "")
    endif(APPLE)

#-- Append the Icon file/Image/Resource file to the list of Sources to compile
    list(APPEND QAB_SOURCES ${QAB_ICON_FILE})

#-- Add and Link our executable
    ADD_EXECUTABLE( ${QAB_TARGET} ${GUI_TYPE} ${QAB_SOURCES} )
    TARGET_LINK_LIBRARIES( ${QAB_TARGET}
                        ${QAB_LINK_LIBRARIES} )

#-- Make sure we have a proper bundle icon. This must occur AFTER the ADD_EXECUTABLE command
    if(APPLE)
        ConfigureMacOSXBundlePlist( ${QAB_TARGET} ${QAB_DEBUG_EXTENSION} ${QAB_ICON_FILE}
                                "${QAB_VERSION_MAJOR}.${QAB_VERSION_MINOR}.${QAB_VERSION_PATCH}" )
    endif(APPLE)

#-- Set the Debug Suffix for the application
    SET_TARGET_PROPERTIES( ${QAB_TARGET}
                PROPERTIES
                DEBUG_OUTPUT_NAME ${QAB_TARGET}${QAB_DEBUG_EXTENSION}
                RELEASE_OUTPUT_NAME ${QAB_TARGET}
    )

#-- Create install rules for any Qt Plugins that are needed
    set(pi_dest ${QAB_INSTALL_DEST}/plugins)
    # if we are on OS X then we set the plugin installation location to inside the App bundle
    if(APPLE)
        set(pi_dest ${QAB_TARGET}.app/Contents/plugins)
        set(osx_app_name ${QAB_TARGET})
        if(CMAKE_BUILD_TYPE MATCHES "Debug")
            set(pi_dest ${QAB_TARGET}${QAB_DEBUG_EXTENSION}.app/Contents/plugins)
            set(osx_app_name ${QAB_TARGET}${QAB_DEBUG_EXTENSION})
        endif()
    endif()


    set(app_plugin_list "")
    set(lib_search_dirs "")
    set(write_qt_plugins_in_qtconf "")
#-- It is important as you build up the list to modify the path to the Qt Plugin
#-- to point to the plugin that will appear in the Application bundle and NOT
#-- the path to your Qt installation. If you do NOT do this step properly AND you
#-- have write privs on your Qt Installation CMake will most likely "fixup" your
#-- Qt installation files which really isn't good at all. Also when generating the
#-- list it is important to have Absolute Paths to these plugins otherwise
#-- fixup_bundle() can not find the libraries.
    foreach(pi ${QAB_QT_PLUGINS})
        set(write_qt_plugins_in_qtconf "1")
        get_filename_component(qt_plugin_name "${pi}" NAME)
        get_filename_component(qt_plugin_type_path "${pi}" PATH)
        get_filename_component(qt_plugin_type "${qt_plugin_type_path}" NAME)
        #install(PROGRAMS ${pi}
        #        DESTINATION "${pi_dest}/${qt_plugin_type}"
        #        COMPONENT ${QAB_COMPONENT}
        #)
        list(APPEND app_plugin_list "\${CMAKE_INSTALL_PREFIX}/${pi_dest}/${qt_plugin_type}/${qt_plugin_name}")
    endforeach()
    list(REMOVE_DUPLICATES lib_search_dirs)

#
#-- Create install rules for our own plugins that are targets in the build system which
#-- is only needed on Apple systems to make sure we get them installed into the bundle.
#-- On other platforms the standard installation rules are used instead.
    if(APPLE)
        foreach(pi ${QAB_OTHER_PLUGINS})
            get_filename_component(plugin_name "${pi}" NAME)
            install(PROGRAMS ${pi}
                    DESTINATION "${pi_dest}"
                    COMPONENT ${QAB_COMPONENT}
            )
            list(APPEND app_plugin_list "\${CMAKE_INSTALL_PREFIX}/${pi_dest}/${plugin_name}")
        endforeach()
    endif(APPLE)

    #-- Create an Install Rule for the main app bundle target
    INSTALL(TARGETS ${QAB_TARGET}
        COMPONENT ${QAB_COMPONENT}
        RUNTIME DESTINATION ${QAB_INSTALL_DEST}
        LIBRARY DESTINATION ${QAB_INSTALL_DEST}
        ARCHIVE DESTINATION ${QAB_INSTALL_DEST}
        BUNDLE DESTINATION ${QAB_INSTALL_DEST}
    )

#-- Create last install rule that will run fixup_bundle() on OS X Machines. Other platforms we
#-- are going to create the install rules elsewhere
    if(APPLE)
        list(APPEND lib_search_dirs "${QAB_LIB_SEARCH_DIRS}")

        set(OSX_MAKE_STANDALONE_BUNDLE_CMAKE_SCRIPT
                    "${QAB_BINARY_DIR}/OSX_Scripts/${QAB_TARGET}_CompleteBundle.cmake")

        set(OPTIMIZE_BUNDLE_SHELL_SCRIPT
            "${QAB_BINARY_DIR}/OSX_Scripts/${QAB_TARGET}_OptimizeBundle.sh")

        CONFIGURE_FILE("${CMP_OSX_TOOLS_SOURCE_DIR}/CompleteBundle.cmake.in"
                "${OSX_MAKE_STANDALONE_BUNDLE_CMAKE_SCRIPT}" @ONLY IMMEDIATE)

        set(PROJECT_INSTALL_DIR ${osx_app_name}.app)
        CONFIGURE_FILE("${CMP_OSX_TOOLS_SOURCE_DIR}/ThinAndShareLibraries.sh.in"
                "${OPTIMIZE_BUNDLE_SHELL_SCRIPT}" @ONLY IMMEDIATE)

        install(SCRIPT "${OSX_MAKE_STANDALONE_BUNDLE_CMAKE_SCRIPT}" COMPONENT ${QAB_COMPONENT})
    endif(APPLE)

#-- This should be called when we are on Linux
    if(NOT APPLE AND UNIX)
        set(linux_app_name ${QAB_TARGET})
        set(LINUX_MAKE_STANDALONE_LAUNCH_SCRIPT
                    "${QAB_BINARY_DIR}/LINUX_Scripts/${QAB_TARGET}.sh")
        CONFIGURE_FILE("${CMP_LINUX_TOOLS_SOURCE_DIR}/launch_script.sh.in"
                "${LINUX_MAKE_STANDALONE_LAUNCH_SCRIPT}" @ONLY IMMEDIATE)
        install(PROGRAMS "${LINUX_MAKE_STANDALONE_LAUNCH_SCRIPT}"
                DESTINATION "bin"
                COMPONENT ${QAB_COMPONENT})
        set(lib_suffix "")
        set(build_type "@CMAKE_BUILD_TYPE@")
        if(${build_type} STREQUAL "Debug")
            set(lib_suffix "_debug")
        endif()

      set(LINUX_INSTALL_LIBS_CMAKE_SCRIPT
                "${QAB_BINARY_DIR}/LINUX_Scripts/${QAB_TARGET}_CompleteBundle.cmake")
        set(OPTIMIZE_BUNDLE_SHELL_SCRIPT
                "${QAB_BINARY_DIR}/LINUX_Scripts/${QAB_TARGET}_InstallLibraries.sh")

      CONFIGURE_FILE("${CMP_LINUX_TOOLS_SOURCE_DIR}/CompleteBundle.cmake.in"
      "${LINUX_INSTALL_LIBS_CMAKE_SCRIPT}" @ONLY IMMEDIATE)
        set(PROJECT_INSTALL_DIR ${linux_app_name}.app)
        CONFIGURE_FILE("${CMP_LINUX_TOOLS_SOURCE_DIR}/InstallLibraries.sh.in"
                "${OPTIMIZE_BUNDLE_SHELL_SCRIPT}" @ONLY IMMEDIATE)

        install(SCRIPT "${LINUX_INSTALL_LIBS_CMAKE_SCRIPT}" COMPONENT ${QAB_COMPONENT})
    endif()


endfunction()

# --------------------------------------------------------------------
# This function should be able to correctly create an Application bundle
# based on the Qt Frameworks for any platform. There are specific
# sections to ensure that plugins and other libraries and resources are
# correctly copied into the Application Bundle. On other platforms these
# items are copied into the installation directory.
# Arguments:
#  TARGET The name of the Target to use in the Add_Executable() commnad
#  DEBUG_EXTENSION The file name suffix extension that Debug builds will have
#  VERSION_MAJOR The Major version
#  VERSION_MINOR The Minor version
#  VERSION_PATCH The Patch version
#  BINARY_DIR    The binary directory where some files are created for this application
#  COMPONENT     The name of the component that is used during the packaging
#  INSTALL_DEST  The destination directory inside of the CMAKE_INSTALL_PREFIX to install everything
#  SOURCES   All the source files that are needed to compile the code
#  LINK_LIBRARIES Dependent libraries that are needed to properly link the executable
#  LIB_SEARCH_DIRS  A list of directories where certain dependent libraries or plugins can be found
#
# Notes: If we were to base a tool off of Qt and NOT just system/3rd party libraries
#  then we would probably have to get some of the features of the "BuildQtAppBunlde"
#  back in this function in order to copy in the Qt frameworks, plugins and other
#  stuff like that. For now none of our 'tools' require Qt.
function(BuildToolBundle)
    set(options )
    set(oneValueArgs TARGET DEBUG_EXTENSION VERSION_MAJOR VERSION_MINOR VERSION_PATCH
                     BINARY_DIR COMPONENT INSTALL_DEST SOLUTION_FOLDER)
    set(multiValueArgs SOURCES LINK_LIBRARIES LIB_SEARCH_DIRS)
    cmake_parse_arguments(QAB "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )

    # Default GUI type is blank
    set(GUI_TYPE "")

    if(APPLE)
        set(osx_app_name ${QAB_TARGET})
        if(CMAKE_BUILD_TYPE MATCHES "Debug")
            set(osx_app_name ${QAB_TARGET}${QAB_DEBUG_EXTENSION})
        endif()
    endif()

#-- Add and Link our executable
    ADD_EXECUTABLE( ${QAB_TARGET} ${GUI_TYPE} ${QAB_SOURCES} )
    TARGET_LINK_LIBRARIES( ${QAB_TARGET}
                            ${QAB_LINK_LIBRARIES} )

#-- Set the Debug Suffix for the application
    SET_TARGET_PROPERTIES( ${QAB_TARGET}
                PROPERTIES
                DEBUG_OUTPUT_NAME ${QAB_TARGET}${QAB_DEBUG_EXTENSION}
                RELEASE_OUTPUT_NAME ${QAB_TARGET}
    )
    if(NOT "${QAB_SOLUTION_FOLDER}" STREQUAL "")
    SET_TARGET_PROPERTIES(${QAB_TARGET}
                          PROPERTIES FOLDER ${QAB_SOLUTION_FOLDER})

  endif()

#-- Create an Install Rule for the main app bundle target
    INSTALL(TARGETS ${QAB_TARGET}
        COMPONENT ${QAB_COMPONENT}
        RUNTIME DESTINATION ${QAB_INSTALL_DEST}
        LIBRARY DESTINATION ${QAB_INSTALL_DEST}
        ARCHIVE DESTINATION ${QAB_INSTALL_DEST}
        BUNDLE DESTINATION ${QAB_INSTALL_DEST}
    )


#-- Create last install rule that will run fixup_bundle() on OS X Machines. Other platforms we
#-- are going to create the install rules elsewhere
    if(APPLE)
        list(APPEND lib_search_dirs "${QAB_LIB_SEARCH_DIRS}")

        set(OSX_MAKE_STANDALONE_BUNDLE_CMAKE_SCRIPT
            "${QAB_BINARY_DIR}/OSX_Scripts/${QAB_TARGET}_CompleteTool.cmake")
        set(OPTIMIZE_BUNDLE_SHELL_SCRIPT
            "${QAB_BINARY_DIR}/OSX_Scripts/${QAB_TARGET}_OptimizeTool.sh")

        set(PROJECT_INSTALL_DIR "tools")
        CONFIGURE_FILE("${CMP_OSX_TOOLS_SOURCE_DIR}/CompleteTool.cmake.in"
                "${OSX_MAKE_STANDALONE_BUNDLE_CMAKE_SCRIPT}" @ONLY IMMEDIATE)

        CONFIGURE_FILE("${CMP_OSX_TOOLS_SOURCE_DIR}/CompleteTool.sh.in"
                "${OPTIMIZE_BUNDLE_SHELL_SCRIPT}" @ONLY IMMEDIATE)

        install(SCRIPT "${OSX_MAKE_STANDALONE_BUNDLE_CMAKE_SCRIPT}" COMPONENT ${QAB_COMPONENT})
    endif(APPLE)
endfunction()

# --------------------------------------------------------------------
#
# --------------------------------------------------------------------
macro(cmp_ToolInstallationSupport_old EXE_NAME EXE_DEBUG_EXTENSION EXE_BINARY_DIR installFiles
                                  comp dest lib_search_dirs)
if(0)
    message(STATUS "EXE_NAME: ${EXE_NAME}")
    message(STATUS "EXE_DEBUG_EXTENSION: ${EXE_DEBUG_EXTENSION}")
    message(STATUS "EXE_BINARY_DIR: ${EXE_BINARY_DIR}")
    message(STATUS "appNeedsPlugins: ${appNeedsPlugins}")
    message(STATUS "installFiles: ${installFiles}")
    message(STATUS "comp: ${comp}")
    message(STATUS "dest: ${dest}")
    message(STATUS "lib_search_dirs: ${lib_search_dirs}")
endif()

    SET_TARGET_PROPERTIES( ${EXE_NAME}
        PROPERTIES
        DEBUG_OUTPUT_NAME ${EXE_NAME}${EXE_DEBUG_EXTENSION}
        RELEASE_OUTPUT_NAME ${EXE_NAME}
    )
    if(${installFiles} EQUAL 1)
        INSTALL(TARGETS ${EXE_NAME}
            COMPONENT ${comp}
            RUNTIME DESTINATION ${dest}
            LIBRARY DESTINATION ${dest}
            ARCHIVE DESTINATION ${dest}
            BUNDLE DESTINATION  ${dest}
        )

        #   message(STATUS "Creating Install CMake file for tool application ${EXE_NAME}")
        if(APPLE)
            if(CMAKE_BUILD_TYPE MATCHES "Debug")
                MakeOSXTool( "${EXE_NAME}${EXE_DEBUG_EXTENSION}"
                            ${EXE_BINARY_DIR}
                            ${CMP_OSX_TOOLS_SOURCE_DIR}
                            "${dest}"
                            "${lib_search_dirs}")
            else(CMAKE_BUILD_TYPE MATCHES "Debug")
                MakeOSXTool(${EXE_NAME}
                             ${EXE_BINARY_DIR}
                             ${CMP_OSX_TOOLS_SOURCE_DIR}
                             "${dest}"
                             "${lib_search_dirs}")
            endif()
        endif(APPLE)
    endif()
endmacro()

# --------------------------------------------------------------------

macro(LibraryProperties targetName DEBUG_EXTENSION)
    if( NOT BUILD_SHARED_LIBS AND MSVC)
      SET_TARGET_PROPERTIES( ${targetName}
        PROPERTIES
        DEBUG_OUTPUT_NAME lib${targetName}
        RELEASE_OUTPUT_NAME lib${targetName}  )
    endif()

    #-- Set the Debug and Release names for the libraries
    SET_TARGET_PROPERTIES( ${targetName}
        PROPERTIES
        DEBUG_POSTFIX ${DEBUG_EXTENSION} )

    if(BUILD_SHARED_LIBS)
      if(APPLE)
          OPTION (CMP_BUILD_WITH_INSTALL_NAME "Build Libraries with the install_name set to the installation prefix. This is good if you are going to run from the installation location" OFF)
          if(CMP_BUILD_WITH_INSTALL_NAME)
              SET_TARGET_PROPERTIES(${targetName}
                 PROPERTIES
                 LINK_FLAGS "-current_version ${${CMP_PROJECT_NAME}_VERSION} -compatibility_version ${${CMP_PROJECT_NAME}_VERSION}"
                 INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/lib"
                 BUILD_WITH_INSTALL_RPATH ${CMP_BUILD_WITH_INSTALL_NAME}
              )
         ENDif(CMP_BUILD_WITH_INSTALL_NAME)
     endif(APPLE)

   ENDif( BUILD_SHARED_LIBS)

endmacro(LibraryProperties DEBUG_EXTENSION)

# --------------------------------------------------------------------
macro(StaticLibraryProperties targetName )
    if(WIN32 AND NOT MINGW)
        set(DEBUG_EXTENSION "_d")
    else()
        set(DEBUG_EXTENSION "_debug")
    endif()


    if(WIN32 AND NOT MINGW)
        SET(LIBRARY_RELEASE_NAME "lib${targetName}" CACHE INTERNAL "" FORCE)
        SET(LIBRARY_DEBUG_NAME "lib${targetName}${DEBUG_EXTENSION}" CACHE INTERNAL "" FORCE)
    else(WIN32 AND NOT MINGW)
        SET(LIBRARY_RELEASE_NAME "${targetName}" CACHE INTERNAL "" FORCE)
        SET(LIBRARY_DEBUG_NAME "${targetName}${DEBUG_EXTENSION}" CACHE INTERNAL "" FORCE)
    ENDif(WIN32 AND NOT MINGW)


    #-- Set the Debug and Release names for the libraries
    SET_TARGET_PROPERTIES( ${targetName}
        PROPERTIES
        DEBUG_OUTPUT_NAME ${LIBRARY_DEBUG_NAME}
        RELEASE_OUTPUT_NAME ${LIBRARY_RELEASE_NAME}
    )

endmacro(StaticLibraryProperties)

#-------------------------------------------------------------------------------
# This is used if you are creating a plugin that needs to be installed
#-------------------------------------------------------------------------------
macro(PluginProperties targetName DEBUG_EXTENSION projectVersion binaryDir pluginfile)
    if( NOT BUILD_SHARED_LIBS AND MSVC)
      SET_TARGET_PROPERTIES( ${targetName}
        PROPERTIES
        DEBUG_OUTPUT_NAME lib${targetName}
        RELEASE_OUTPUT_NAME lib${targetName}  )
    endif()

    #-- Set the Debug and Release names for the libraries
    SET_TARGET_PROPERTIES( ${targetName}
        PROPERTIES
        DEBUG_POSTFIX ${DEBUG_EXTENSION}
        SUFFIX ".plugin" )

    # Add the plugin to our list of plugins that will need to be installed
    if(CMAKE_BUILD_TYPE STREQUAL "Debug" AND NOT MSVC)
        file(APPEND ${pluginfile} "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/lib${targetName}${DEBUG_EXTENSION}.plugin;")
    else()
        file(APPEND ${pluginfile} "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/lib${targetName}.plugin;")
    endif()

    if(NOT APPLE)
        set(BUILD_TYPES "Debug;Release")
        foreach(btype ${BUILD_TYPES})
            INSTALL(TARGETS ${targetName}
                    DESTINATION ./plugins
                    CONFIGURATIONS ${btype}
                    COMPONENT Applications)
        endforeach()
    endif()
endmacro()

#-------------------------------------------------------------------------------
# Finds plugins from the Qt installation. The pluginlist argument should be
# something like "qgif;qjpeg;qtiff"
#-------------------------------------------------------------------------------
macro (FindQt4Plugins pluginlist pluginfile libdirsearchfile plugintype)
  set(qt_plugin_list ${pluginlist})
  set(qt_plugin_types "Debug;Release")
  if(WIN32)
    set(qt_plugin_prefix "")
    set(qt_plugin_DEBUG_suffix "d4")
    set(qt_plugin_RELEASE_suffix "4")
  else ()
    set(qt_plugin_prefix "lib")
    set(qt_plugin_DEBUG_suffix "_debug")
    set(qt_plugin_RELEASE_suffix "")
  endif()

  #message(STATUS "qt_plugin_debug_suffix: ${qt_plugin_debug_suffix}")
  set(QTPLUGINS_RELEASE "")
  set(QTPLUGINS_DEBUG   "")
  set(QTPLUGINS "")

  # Loop through all the Build Types and all the plugins to find each one.
  foreach(build_type ${qt_plugin_types})
    string(TOUPPER ${build_type} BTYPE)
      foreach(plugin ${qt_plugin_list})
        STRING(TOUPPER ${plugin} PLUGIN)
        #message(STATUS "|-- Looking for ${plugin}${qt_plugin_${BTYPE}_suffix}")
        FIND_LIBRARY( QT_IMAGEFORMAT_PLUGIN_${PLUGIN}_${BTYPE}
                      NAMES ${plugin}${qt_plugin_${BTYPE}_suffix}
                      PATHS ${QT_PLUGINS_DIR}/${plugintype}
                      DOC "Library Path for ${plugin}"
                      NO_DEFAULT_PATH NO_CMAKE_PATH NO_SYSTEM_ENVIRONMENT_PATH NO_CMAKE_SYSTEM_PATH)

        if(MSVC)
            #  message(STATUS "QT_PLUGINS_DIR: ${QT_PLUGINS_DIR}")
            #  message(STATUS " QT_IMAGEFORMAT_PLUGIN_${PLUGIN}_${BTYPE}: ${QT_IMAGEFORMAT_PLUGIN_${PLUGIN}_${BTYPE}}")
            get_filename_component(lib_path ${QT_IMAGEFORMAT_PLUGIN_${PLUGIN}_${BTYPE}} PATH)
            get_filename_component(lib_name ${QT_IMAGEFORMAT_PLUGIN_${PLUGIN}_${BTYPE}} NAME_WE)
            #  message(STATUS "lib_path: ${lib_path}")
            #  message(STATUS "lib_name: ${lib_name}")
            set(QT_IMAGEFORMAT_PLUGIN_${PLUGIN}_${BTYPE} ${lib_path}/${lib_name}.dll CACHE PATH "" FORCE)
            if( ${QT_IMAGEFORMAT_PLUGIN_${PLUGIN}_${BTYPE}} STREQUAL  "QT_IMAGEFORMAT_PLUGIN_${PLUGIN}_${BTYPE}-NOTFOUND")
              message(STATUS "A Companion DLL for ${upperlib}_LIBRARY_${TYPE} was NOT found which usually means"
                                " that the library was NOT built as a DLL. I looked in the "
                                " following locations:  ${lib_path}\n  ${lib_path}/..\n  ${lib_path}/../bin")
            else()
                #  set(${upperlib}_LIBRARY_DLL_${TYPE}  ${${upperlib}_LIBRARY_DLL_${TYPE}}/${lib_name}.dll)
                #  message(STATUS "${upperlib}_LIBRARY_DLL_${TYPE}: ${${upperlib}_LIBRARY_DLL_${TYPE}}")
                #  message(STATUS "Generating Install Rule for DLL File for QT_IMAGEFORMAT_PLUGIN_${PLUGIN}_${BTYPE}\n  ${QT_IMAGEFORMAT_PLUGIN_${PLUGIN}_${BTYPE}}")
                INSTALL(FILES ${QT_IMAGEFORMAT_PLUGIN_${PLUGIN}_${BTYPE}}
                    DESTINATION ./plugins/${plugintype}
                    CONFIGURATIONS ${BTYPE}
                    COMPONENT Applications)
            endif()
        elseif(UNIX AND NOT APPLE)
            INSTALL(FILES ${QT_IMAGEFORMAT_PLUGIN_${PLUGIN}_${BTYPE}}
                DESTINATION ./plugins/${plugintype}
                CONFIGURATIONS ${BTYPE}
                COMPONENT Applications)
        endif()

        mark_as_advanced(QT_IMAGEFORMAT_PLUGIN_${PLUGIN}_${BTYPE})
        #message(STATUS "|--  QT_IMAGEFORMAT_PLUGIN_${PLUGIN}_${BTYPE}: ${QT_IMAGEFORMAT_PLUGIN_${PLUGIN}_${BTYPE}}")
        LIST(APPEND QTPLUGINS_${BTYPE} ${QT_IMAGEFORMAT_PLUGIN_${PLUGIN}_${BTYPE}})
      endforeach()
    endforeach()

    # Assign either the debug or release plugin list to the QTPLUGINS variable on NON msvc platforms.
    if(NOT MSVC)
      if( NOT DEFINED CMAKE_BUILD_TYPE )
        if( ${CMAKE_BUILD_TYPE} STREQUAL "Debug")
            set(QTPLUGINS ${QTPLUGINS_DEBUG})
        else()
            set(QTPLUGINS ${QTPLUGINS_RELEASE})
        endif()
      else()
        set(QTPLUGINS ${QTPLUGINS_RELEASE})
      endif()
    else()
        # Create the qt.conf file so that the image plugins will be loaded correctly
        FILE(WRITE ${PROJECT_BINARY_DIR}/qt.conf "[Paths]\nPlugins = plugins")
        INSTALL(FILES ${PROJECT_BINARY_DIR}/qt.conf
                DESTINATION .
                COMPONENT Applications)
    endif()
    file(APPEND ${pluginfile} "${QTPLUGINS};")
    file(APPEND ${libdirsearchfile} "${QT_PLUGINS_DIR}/${plugintype};")

endmacro(FindQt4Plugins pluginlist)

# --------------------------------------------------------------------
#-- Copy all the Qt4 dependent DLLs into the current build directory so that
#-- one can debug an application or library that depends on Qt4 libraries.
macro (CMP_COPY_QT4_RUNTIME_LIBRARIES QTLIBLIST)
    #message(STATUS "CMP_COPY_QT4_RUNTIME_LIBRARIES")
    if(MSVC)

        if(DEFINED QT_QMAKE_EXECUTABLE)
            set(TYPE "d")
            FOREACH(qtlib ${QTLIBLIST})
                GET_FILENAME_COMPONENT(QT_DLL_PATH_tmp ${QT_QMAKE_EXECUTABLE} PATH)

                if( ${CMAKE_BUILD_TOOL} STREQUAL "nmake")
          if(NOT ${CMAKE_BUILD_TYPE} STREQUAL "Debug")
            set(TYPE "")
          endif()
          add_custom_target(ZZ_${qtlib}-Debug-Copy ALL
                              COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QT_DLL_PATH_tmp}/${qtlib}${TYPE}4.dll
                              ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/
                              COMMENT "Copying ${qtlib}${TYPE}4.dll to ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
          set_target_properties(ZZ_${qtlib}-Debug-Copy PROPERTIES FOLDER ZZ_COPY_FILES)
                else()
          add_custom_target(ZZ_${qtlib}-Debug-Copy ALL
                              COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QT_DLL_PATH_tmp}/${qtlib}${TYPE}4.dll
                              ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug/
                              COMMENT "Copying ${qtlib}${TYPE}4.dll to ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug/")
          set_target_properties(ZZ_${qtlib}-Debug-Copy PROPERTIES FOLDER ZZ_COPY_FILES)
        #   message(STATUS "Generating Copy Rule for Qt Release DLL Library ${QT_DLL_PATH_tmp}/${qtlib}d4.dll")
          add_custom_target(ZZ_${qtlib}-Release-Copy ALL
                              COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QT_DLL_PATH_tmp}/${qtlib}4.dll
                              ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release/
                              COMMENT "Copying ${qtlib}4.dll to ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release/")
          set_target_properties(ZZ_${qtlib}-Release-Copy PROPERTIES FOLDER ZZ_COPY_FILES)
                endif()
            ENDFOREACH(qtlib)
        endif(DEFINED QT_QMAKE_EXECUTABLE)
    endif()
    if(MINGW)
        if(DEFINED QT_QMAKE_EXECUTABLE)
            set(TYPE "")
            if( ${CMAKE_BUILD_TYPE} STREQUAL "Debug")
                set(TYPE "d")
            endif()
            FOREACH(qtlib ${QTLIBLIST})
                GET_FILENAME_COMPONENT(QT_DLL_PATH_tmp ${QT_QMAKE_EXECUTABLE} PATH)
                message(STATUS "Generating Copy Rule for Qt DLL Library ${QT_DLL_PATH_tmp}/${qtlib}d4.dll")
                add_custom_target(ZZ_${qtlib}-Debug-Copy ALL
                            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QT_DLL_PATH_tmp}/${qtlib}${TYPE}4.dll
                            ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/
                            COMMENT "Copying ${qtlib}${TYPE}4.dll to ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/")
        set_target_properties(ZZ_${qtlib}-Debug-Copy PROPERTIES FOLDER ZZ_COPY_FILES)
            ENDFOREACH(qtlib)
        endif(DEFINED QT_QMAKE_EXECUTABLE)
    endif()
endmacro()

# --------------------------------------------------------------------
#
#
macro (CMP_QT_LIBRARIES_INSTALL_RULES QTLIBLIST destination)
   # message(STATUS "CMP_QT_LIBRARIES_INSTALL_RULES")
    if(MSVC)
        if(DEFINED QT_QMAKE_EXECUTABLE)
            set(TYPE "d")
            FOREACH(qtlib ${QTLIBLIST})

                GET_FILENAME_COMPONENT(QT_DLL_PATH_tmp ${QT_QMAKE_EXECUTABLE} PATH)
           #     message(STATUS "Generating Install Rule for Qt Debug DLL Library ${QT_DLL_PATH_tmp}/${qtlib}d4.dll")
                INSTALL(FILES ${QT_DLL_PATH_tmp}/${qtlib}${type}d4.dll
                    DESTINATION "${destination}"
                    CONFIGURATIONS Debug
                    COMPONENT Applications)
           #     message(STATUS "Generating Install Rule for Qt Release DLL Library ${QT_DLL_PATH_tmp}/${qtlib}4.dll")
                INSTALL(FILES ${QT_DLL_PATH_tmp}/${qtlib}4.dll
                    DESTINATION "${destination}"
                    CONFIGURATIONS Release
                    COMPONENT Applications)
            ENDFOREACH(qtlib)
        endif(DEFINED QT_QMAKE_EXECUTABLE)
    endif()

#-- This will create install rules for the dylibs on linux hopefully creating
#-- a stand alone .zip or .tgz file
    if(UNIX AND NOT APPLE)
        if(DEFINED QT_QMAKE_EXECUTABLE)
            GET_FILENAME_COMPONENT(QT_LIB_PATH ${QT_QMAKE_EXECUTABLE} PATH)
            GET_FILENAME_COMPONENT(QT_LIB_PATH ${QT_LIB_PATH} PATH)
            set(QT_LIB_PATH ${QT_LIB_PATH}/lib)
            FOREACH(qtlib ${QTLIBLIST})
               FILE(GLOB libFiles ${QT_LIB_PATH}/lib${qtlib}.so*)
               INSTALL(FILES ${libFiles}
                    DESTINATION "lib"
                    CONFIGURATIONS ${CMAKE_BUILD_TYPE}
                    COMPONENT Applications)
            ENDFOREACH(qtlib)
        endif()
    endif(UNIX AND NOT APPLE)
endmacro()

# --------------------------------------------------------------------
#-- Copy all the dependent DLLs into the current build directory so that the test
#-- can run.
MACRO (CMP_COPY_DEPENDENT_LIBRARIES _libraryList)
#  message(STATUS "#--------------------------------------------")
#  message(STATUS "CMP_COPY_DEPENDENT_LIBRARIES: ${_libraryList}")
  set(_libraryList ${_libraryList})
  set(TYPES Debug Release)
  if(${CMAKE_BUILD_TOOL} STREQUAL "nmake")
    set(TYPES ${CMAKE_BUILD_TYPE})
  endif()
  if(MSVC)
    # Make all the necessary intermediate directories for Visual Studio
    file(MAKE_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug)
    file(MAKE_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release)
    file(MAKE_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/MinSizeRel)
    file(MAKE_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/RelWithDebInfo)
    FOREACH(lib ${_libraryList})

      STRING(TOUPPER ${lib} upperlib)
     # message(STATUS "upperlib: ${upperlib}")
     # message(STATUS "${upperlib}_IS_SHARED: ${${upperlib}_IS_SHARED}")
      if(${upperlib}_IS_SHARED)
        FOREACH(BTYPE ${TYPES})
        #  message(STATUS "Looking for ${BTYPE} DLL Version of ${lib}")
          STRING(TOUPPER ${BTYPE} TYPE)
          get_filename_component(lib_path ${${upperlib}_LIBRARY_${TYPE}} PATH)
          get_filename_component(lib_name ${${upperlib}_LIBRARY_${TYPE}} NAME_WE)
          #message(STATUS "lib_path: ${lib_path}")
          #message(STATUS "lib_name: ${lib_name}")
      #message(STATUS "${upperlib}_BIN_DIR: ${${upperlib}_BIN_DIR}")

          find_file(${upperlib}_LIBRARY_DLL_${TYPE}
                        NAMES ${lib_name}.dll
                        PATHS  ${lib_path}/../bin ${lib_path}/.. ${lib_path}/ ${${upperlib}_BIN_DIR}
                        NO_DEFAULT_PATH )
      #    message(STATUS "${upperlib}_LIBRARY_DLL_${TYPE}: ${${upperlib}_LIBRARY_DLL_${TYPE}}")
          mark_as_advanced(${upperlib}_LIBRARY_DLL_${TYPE})
          if( ${${upperlib}_LIBRARY_DLL_${TYPE}} STREQUAL  "${upperlib}_LIBRARY_DLL_${TYPE}-NOTFOUND")
            message(FATAL_ERROR "According to how ${upperlib}_LIBRARY_${TYPE} was found the library should"
                                " have been built as a DLL but no .dll file can be found. I looked in the "
                                " following locations:\n  ${lib_path}\n  ${lib_path}/..\n  ${lib_path}/../bin\n  ${${upperlib}_BIN_DIR}")
          endif()

         # SET(${upperlib}_LIBRARY_DLL_${TYPE} "${${upperlib}_LIBRARY_DLL_${TYPE}}/${lib_name}.dll" CACHE FILEPATH "The path to the DLL Portion of the library" FORCE)
         # message(STATUS "${upperlib}_LIBRARY_DLL_${TYPE}: ${${upperlib}_LIBRARY_DLL_${TYPE}}")
         # message(STATUS "Generating Copy Rule for ${BTYPE} DLL Library ${${upperlib}_LIBRARY_DLL_${TYPE}}")
          if(${CMAKE_BUILD_TOOL} STREQUAL "nmake")
            set(BTYPE ".")
          endif()
      ADD_CUSTOM_TARGET(ZZ_${upperlib}_DLL_${TYPE}-Copy ALL
                      COMMAND ${CMAKE_COMMAND} -E copy_if_different ${${upperlib}_LIBRARY_DLL_${TYPE}}
                      ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${BTYPE}/
                      COMMENT "  Copy: ${${upperlib}_LIBRARY_DLL_${TYPE}}\n    To: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${BTYPE}/")
      set_target_properties(ZZ_${upperlib}_DLL_${TYPE}-Copy PROPERTIES FOLDER ZZ_COPY_FILES)
        ENDFOREACH(BTYPE ${TYPES})
      ENDif(${upperlib}_IS_SHARED)
    ENDFOREACH(lib ${_libraryList})
  ENDif(MSVC)
endmacro()

# --------------------------------------------------------------------
# This macro generates install rules for Visual Studio builds so that
# dependent DLL libraries (HDF5, Tiff, Expat, DataModel) will be
# properly installed with your project.
# --------------------------------------------------------------------
MACRO (CMP_LIBRARIES_INSTALL_RULES _libraryList destination)
#  message(STATUS "CMP_LIBRARIES_INSTALL_RULES")
  set(_libraryList ${_libraryList})
  set(TYPES Debug Release)
  if(MSVC)

    FOREACH(lib ${_libraryList})
        STRING(TOUPPER ${lib} upperlib)

        FOREACH(BTYPE ${TYPES} )
          STRING(TOUPPER ${BTYPE} TYPE)
          get_filename_component(lib_path ${${upperlib}_LIBRARY_${TYPE}} PATH)
          get_filename_component(lib_name ${${upperlib}_LIBRARY_${TYPE}} NAME_WE)

          find_file(${upperlib}_LIBRARY_DLL_${TYPE}
                        NAMES ${lib_name}.dll
                        PATHS  ${lib_path}/../bin ${lib_path}/.. ${lib_path}/ ${${upperlib}_BIN_DIR}
                        NO_DEFAULT_PATH )
         # message(STATUS "${upperlib}_LIBRARY_DLL_${TYPE}: ${${upperlib}_LIBRARY_DLL_${TYPE}}")
          mark_as_advanced(${upperlib}_LIBRARY_DLL_${TYPE})
          if( ${${upperlib}_LIBRARY_DLL_${TYPE}} STREQUAL  "${upperlib}_LIBRARY_DLL_${TYPE}-NOTFOUND")
             message(STATUS "A Companion DLL for ${upperlib}_LIBRARY_${TYPE} was NOT found which usually means\n"
                                " that the library was NOT built as a DLL. I looked in the \n"
                                " following locations:\n  ${lib_path}\n  ${lib_path}/..\n  ${lib_path}/../bin\n  ${${upperlib}_BIN_DIR}")
          else()
             # set(${upperlib}_LIBRARY_DLL_${TYPE}  ${${upperlib}_LIBRARY_DLL_${TYPE}}/${lib_name}.dll)
           #   message(STATUS "${upperlib}_LIBRARY_DLL_${TYPE}: ${${upperlib}_LIBRARY_DLL_${TYPE}}")
           #   message(STATUS "Generating Install Rule for DLL Library ${${upperlib}_LIBRARY_DLL_${TYPE}}")
              INSTALL(FILES ${${upperlib}_LIBRARY_DLL_${TYPE}}
                DESTINATION ${destination}
                CONFIGURATIONS ${BTYPE}
                COMPONENT Applications)
          endif()

        ENDFOREACH(BTYPE ${TYPES})
    ENDFOREACH(lib ${_libraryList})
  ENDif(MSVC)

#-- This will create install rules for the dylibs on linux hopefully creating
#-- a stand alone .zip or .tgz file
    if(UNIX AND NOT APPLE)
      FOREACH(lib ${_libraryList})
        STRING(TOUPPER ${lib} upperlib)

        set(BTYPE "RELEASE" )
          STRING(TOUPPER ${BTYPE} TYPE)
          get_filename_component(lib_path ${${upperlib}_LIBRARY_${TYPE}} PATH)
          get_filename_component(lib_name ${${upperlib}_LIBRARY_${TYPE}} NAME_WE)

          find_file(${upperlib}_LIBRARY_SO_${TYPE}
                        NAMES ${lib_name}.so
                        PATHS  ${lib_path}/../bin ${lib_path}/.. ${lib_path}/ ${${upperlib}_BIN_DIR}
                        NO_DEFAULT_PATH )
         # message(STATUS "${upperlib}_LIBRARY_SO_${TYPE}: ${${upperlib}_LIBRARY_SO_${TYPE}}")
          mark_as_advanced(${upperlib}_LIBRARY_SO_${TYPE})
          if( "${${upperlib}_LIBRARY_SO_${TYPE}}" STREQUAL  "${upperlib}_LIBRARY_SO_${TYPE}-NOTFOUND")
             message(STATUS "A shared library for ${upperlib}_LIBRARY_${TYPE} was NOT found which usually means\n"
                                " that the library was NOT built as a .so. I looked in the \n"
                                " following locations:\n  ${lib_path}\n  ${lib_path}/..\n  ${lib_path}/../bin\n  ${${upperlib}_BIN_DIR}")
          else()
           #   message(STATUS "${upperlib}_LIBRARY_SO_${TYPE}: ${${upperlib}_LIBRARY_SO_${TYPE}}")
           #   message(STATUS "Generating Install Rule for .so Library ${${upperlib}_LIBRARY_SO_${TYPE}}")
              INSTALL(FILES ${${upperlib}_LIBRARY_SO_${TYPE}}
                DESTINATION "lib"
                CONFIGURATIONS ${BTYPE}
                COMPONENT Applications)
          endif()
      ENDFOREACH(lib ${_libraryList})
    endif()
ENDMACRO()

#-------------------------------------------------------------------------------
# This macro will attempt a try_run command in order to compile and then
# generate a version string based on today's date. The output string should be
# of the form YYYY.MM.DD.
#  Required CMake variables to be set are:
#   Project_CMAKE_DIR - The path to the Project CMake directory
#  The following variables are set, all of which should have been already
#  initialized to a default value
#   ${CMP_PROJECT_NAME}_VERSION
#   ${CMP_PROJECT_NAME}_VER_MAJOR
#   ${CMP_PROJECT_NAME}_VER_MINOR
#   ${CMP_PROJECT_NAME}_VER_PATCH
#
#-------------------------------------------------------------------------------
macro(cmpGenerateVersionString GENERATED_HEADER_PATH GENERATED_SOURCE_PATH NAMESPACE cmpProjectName)
    INCLUDE (${CMAKE_ROOT}/Modules/CheckSymbolExists.cmake)

    if( CMP_HAVE_TIME_GETTIMEOFDAY )
      set( VERSION_COMPILE_FLAGS "-DHAVE_TIME_GETTIMEOFDAY")
    endif()

    if( CMP_HAVE_SYS_TIME_GETTIMEOFDAY )
        set( VERSION_COMPILE_FLAGS "-DHAVE_SYS_TIME_GETTIMEOFDAY")
    endif()

    if(NOT CMP_HAVE_TIME_GETTIMEOFDAY AND NOT CMP_HAVE_SYS_TIME_GETTIMEOFDAY)
      set(VERSION_GEN_VER_MAJOR "0")
      set(VERSION_GEN_VER_MINOR "0")
      set(VERSION_GEN_VER_PATCH "1")
      set(VERSION_GEN_COMPLETE "0.0.1" )
      set(VERSION_GEN_NAME "${CMP_PROJECT_NAME}")
      set(VERSION_GEN_NAMESPACE "${NAMESPACE}")
      set(${cmpProjectName}_VERSION   ${VERSION_GEN_COMPLETE}  CACHE STRING "Complete Version String")
      set(${cmpProjectName}_VER_MAJOR ${VERSION_GEN_VER_MAJOR} CACHE STRING "Major Version String")
      set(${cmpProjectName}_VER_MINOR ${VERSION_GEN_VER_MINOR} CACHE STRING "Minor Version String")
      set(${cmpProjectName}_VER_PATCH ${VERSION_GEN_VER_PATCH} CACHE STRING "Patch Version String")
    else()
      # message(STATUS "Running version generation executable")
      try_run(VERSION_RUN_RESULT VERSION_COMPILE_RESULT
              ${CMAKE_CURRENT_BINARY_DIR} ${CMP_CORE_TESTS_SOURCE_DIR}/cmpGenerateVersionString.cpp
              COMPILE_DEFINITIONS ${VERSION_COMPILE_FLAGS}
              COMPILE_OUTPUT_VARIABLE VERSION_COMPILE_OUTPUT
              RUN_OUTPUT_VARIABLE VERSION_RUN_OUTPUT )
      set(VERSION_RUN_RESULT "1" CACHE INTERNAL "")


      if(NOT VERSION_RUN_OUTPUT)
          message(STATUS "VERSION_COMPILE_OUTPUT: ${VERSION_COMPILE_OUTPUT}")
          message(STATUS "VERSION_RUN_OUTPUT: ${VERSION_RUN_OUTPUT}")
          FILE(APPEND ${CMAKE_BINARY_DIR}/CMakeFiles/CMakeError.log
              "Attempting to Generate a Version Number from a GetTimeofDay() function failed with the following output\n"
              "----------- COMPILE OUTPUT ---------------------------------------------------\n"
              "${VERSION_COMPILE_OUTPUT}\n"
              "----------- RUN OUTPUT ---------------------------------------------------\n"
              "${VERSION_RUN_OUTPUT}\n"
              "--------------------------------------------------------------\n" )
           message(FATAL_ERROR "The program to generate a version was not able to be run. Are we cross compiling? Do we have the GetTimeOfDay() function?")

      endif()

      # and now the version string given by qmake
      STRING(REGEX REPLACE "^([0-9]+)\\.[0-9]+\\.[0-9]+.*" "\\1" VERSION_GEN_VER_MAJOR "${VERSION_RUN_OUTPUT}")
      STRING(REGEX REPLACE "^[0-9]+\\.([0-9]+)\\.[0-9]+.*" "\\1" VERSION_GEN_VER_MINOR "${VERSION_RUN_OUTPUT}")
      STRING(REGEX REPLACE "^[0-9]+\\.[0-9]+\\.([0-9]+).*" "\\1" VERSION_GEN_VER_PATCH "${VERSION_RUN_OUTPUT}")

      set(VERSION_GEN_COMPLETE ${VERSION_RUN_OUTPUT} )
      set(VERSION_GEN_NAME "${cmpProjectName}")
      set(VERSION_GEN_NAMESPACE "${NAMESPACE}")
      set(${cmpProjectName}_VERSION   ${VERSION_RUN_OUTPUT}    CACHE STRING "Complete Version String")
      set(${cmpProjectName}_VER_MAJOR ${VERSION_GEN_VER_MAJOR} CACHE STRING "Major Version String")
      set(${cmpProjectName}_VER_MINOR ${VERSION_GEN_VER_MINOR} CACHE STRING "Minor Version String")
      set(${cmpProjectName}_VER_PATCH ${VERSION_GEN_VER_PATCH} CACHE STRING "Patch Version String")
      mark_as_advanced(${cmpProjectName}_VERSION ${cmpProjectName}_VER_MAJOR ${cmpProjectName}_VER_MINOR ${cmpProjectName}_VER_PATCH)

    endif()
    set(PROJECT_PREFIX "${cmpProjectName}")
    configure_file(${CMP_CONFIGURED_FILES_SOURCE_DIR}/cmpVersion.h.in   ${GENERATED_HEADER_PATH}  )
    configure_file(${CMP_CONFIGURED_FILES_SOURCE_DIR}/cmpVersion.cpp.in   ${GENERATED_SOURCE_PATH}  )
    MARK_AS_ADVANCED(${CMP_PROJECT_NAME}_VERSION ${CMP_PROJECT_NAME}_VER_MAJOR ${CMP_PROJECT_NAME}_VER_MINOR ${CMP_PROJECT_NAME}_VER_PATCH)
endmacro()

#-------------------------------------------------------------------------------
# This function generates a file ONLY if the MD5 between the "to be" generated file
# and the current file are different. This will help reduce recompiles based on
# the generation of files that are really the same.
#
function(cmpConfigureFileWithMD5Check)
    set(options)
    set(oneValueArgs CONFIGURED_TEMPLATE_PATH GENERATED_FILE_PATH )
    cmake_parse_arguments(GVS "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )

  #  message(STATUS "GVS_CONFIGURED_TEMPLATE_PATH: ${GVS_CONFIGURED_TEMPLATE_PATH}")
  #  message(STATUS "GVS_GENERATED_FILE_PATH: ${GVS_GENERATED_FILE_PATH}")

    # Only Generate a file if it is different than what is already there.
    if(EXISTS ${GVS_GENERATED_FILE_PATH} )
        file(MD5 ${GVS_GENERATED_FILE_PATH} VERSION_HDR_MD5)
        #configure_file(${CMP_CONFIGURED_FILES_SOURCE_DIR}/cmpVersion.h.in   ${GVS_GENERATED_HEADER_FILE_PATH}_tmp.h  )
        configure_file(${GVS_CONFIGURED_TEMPLATE_PATH}   ${GVS_GENERATED_FILE_PATH}_tmp  )
        file(MD5 ${GVS_GENERATED_FILE_PATH}_tmp VERSION_GEN_HDR_MD5)
    #    message(STATUS "  File Exists, doing MD5 Comparison")
        # Compare the MD5 checksums. If they are different then configure the file into the proper location
        if(NOT "${VERSION_HDR_MD5}" STREQUAL "${VERSION_GEN_HDR_MD5}")
     #       message(STATUS "  Files differ: Replacing with newly generated file")
            configure_file(${GVS_CONFIGURED_TEMPLATE_PATH}  ${GVS_GENERATED_FILE_PATH} )
        else()
     #       message(STATUS "  NO Difference in Files")
        endif()
        file(REMOVE ${GVS_GENERATED_FILE_PATH}_tmp)
    else()
     #   message(STATUS "  File does NOT Exist, Generating one...")
        configure_file(${GVS_CONFIGURED_TEMPLATE_PATH} ${GVS_GENERATED_FILE_PATH} )
    endif()

endfunction()

#-------------------------------------------------------------------------------
# This function generates a file ONLY if the MD5 between the "to be" generated file
# and the current file are different. This will help reduce recompiles based on
# the generation of files that are really the same.
#
function(cmpReplaceFileIfDifferent)
    set(options)
    set(oneValueArgs NEW_FILE_PATH OLD_FILE_PATH )
    cmake_parse_arguments(GVS "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )

   # message(STATUS "GVS_NEW_FILE_PATH: ${GVS_NEW_FILE_PATH}")
   # message(STATUS "GVS_OLD_FILE_PATH: ${GVS_OLD_FILE_PATH}")

    # Only Generate a file if it is different than what is already there.
    if(EXISTS ${GVS_OLD_FILE_PATH} )
      #  message(STATUS "  File Exists, doing MD5 Comparison")
        file(MD5 ${GVS_OLD_FILE_PATH} VERSION_HDR_MD5)
        file(MD5 ${GVS_NEW_FILE_PATH} VERSION_GEN_HDR_MD5)

        # Compare the MD5 checksums. If they are different then configure the file into the proper location
        if(NOT "${VERSION_HDR_MD5}" STREQUAL "${VERSION_GEN_HDR_MD5}")
        #    message(STATUS "  Files differ: Replacing with newly generated file")
            file(REMOVE ${GVS_OLD_FILE_PATH})
            file(RENAME ${GVS_NEW_FILE_PATH} ${GVS_OLD_FILE_PATH})

        else()
         #   message(STATUS "  NO Difference in Files")
            file(REMOVE ${GVS_NEW_FILE_PATH})
        endif()

    else()
       # message(STATUS "  File does NOT Exist, Generating one...")
        file(RENAME ${GVS_NEW_FILE_PATH} ${GVS_OLD_FILE_PATH})
    endif()

endfunction()




#-------------------------------------------------------------------------------
# We are going to use Git functionality to create a version number for our package
# The specific functionality we are going to use is the 'git describe' function
# which should return the latest tag, the number commits since that tag and the
# SHA1 of that commit. If we fail to find git then we fall back to a manually
# entered version number.
#
function(cmpVersionStringsFromGit)
    set(options)
    set(oneValueArgs GENERATED_HEADER_FILE_PATH GENERATED_SOURCE_FILE_PATH NAMESPACE cmpProjectName)
    cmake_parse_arguments(GVS "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )
    if(0)
      message(STATUS "--------------------------------------------")
      message(STATUS "GVS_NAMESPACE: ${GVS_NAMESPACE}")
      message(STATUS "GVS_cmpProjectName: ${GVS_cmpProjectName}")
      message(STATUS "GVS_GENERATED_HEADER_FILE_PATH: ${GVS_GENERATED_HEADER_FILE_PATH}")
      message(STATUS "GVS_GENERATED_SOURCE_FILE_PATH: ${GVS_GENERATED_SOURCE_FILE_PATH}")
    endif()

    # Find Git executable
    Find_package(Git)
    # Run 'git describe' to get our tag offset
    if(GIT_FOUND)
        execute_process(COMMAND ${GIT_EXECUTABLE} describe
        OUTPUT_VARIABLE DVERS
        RESULT_VARIABLE did_run
        ERROR_VARIABLE git_error
        WORKING_DIRECTORY ${DREAM3DProj_SOURCE_DIR}
    )

        # message(STATUS "DVERS: ${DVERS}")

        #-- Make sure that actually worked and if not just generate some dummy values
        if(DVERS STREQUAL "")
            cmpGenerateVersionString( ${GVS_GENERATED_HEADER_FILE_PATH} ${GVS_GENERATED_SOURCE_FILE_PATH} ${GVS_NAMESPACE} ${GVS_cmpProjectName} )
        else()
            string(STRIP ${DVERS} DVERS)
            string(REPLACE  "-" ";" VERSION_LIST ${DVERS})
               # message(STATUS "VERSION_LIST: ${VERSION_LIST}")
               # list(GET VERSION_LIST 0 VERSION_GEN_VER_MAJOR)
            set(VERSION_GEN_VER_MAJOR ${CMP_VERSION_MAJOR})
            list(GET VERSION_LIST 1 VERSION_GEN_VER_MINOR)
            list(GET VERSION_LIST 2 VERSION_GEN_VER_PATCH)

            set(VERSION_GEN_NAMESPACE "${GVS_NAMESPACE}")
            set(VERSION_GEN_NAME "${GVS_cmpProjectName}")
              #  message(STATUS "VERSION_GEN_VER_MAJOR: ${VERSION_GEN_VER_MAJOR}")
              #  message(STATUS "VERSION_GEN_VER_MINOR: ${VERSION_GEN_VER_MINOR}")
              #  message(STATUS "VERSION_GEN_VER_PATCH: ${VERSION_GEN_VER_PATCH}")
            string(SUBSTRING ${VERSION_GEN_VER_PATCH} 1 -1 VERSION_GEN_VER_PATCH)

            set(${GVS_cmpProjectName}_VER_MAJOR ${CMP_VERSION_MAJOR} CACHE STRING "" FORCE)
            set(${GVS_cmpProjectName}_VER_MINOR ${VERSION_GEN_VER_MINOR} CACHE STRING "" FORCE)
            set(${GVS_cmpProjectName}_VER_PATCH ${VERSION_GEN_VER_PATCH} CACHE STRING "" FORCE)
            set(VERSION_GEN_COMPLETE "${VERSION_GEN_VER_MAJOR}.${VERSION_GEN_VER_MINOR}.${VERSION_GEN_VER_PATCH}" )
               # message(STATUS "VERSION_GEN_COMPLETE: ${VERSION_GEN_COMPLETE}")
              #  message(STATUS "${GVS_cmpProjectName}_VER_MAJOR: ${${GVS_cmpProjectName}_VER_MAJOR}")
               # message(STATUS "${GVS_cmpProjectName}_VER_MINOR: ${${GVS_cmpProjectName}_VER_MINOR}")
               # message(STATUS "${GVS_cmpProjectName}_VER_PATCH: ${${GVS_cmpProjectName}_VER_PATCH}")

            set(${GVS_cmpProjectName}_VERSION "${${GVS_cmpProjectName}_VER_MAJOR}.${${GVS_cmpProjectName}_VER_MINOR}.${${GVS_cmpProjectName}_VER_PATCH}" CACHE STRING "Full Version Number" FORCE)
               # message(STATUS "${GVS_cmpProjectName}_VERSION: ${${GVS_cmpProjectName}_VERSION}")
            mark_as_advanced( ${GVS_cmpProjectName}_VER_MAJOR ${GVS_cmpProjectName}_VER_MINOR ${GVS_cmpProjectName}_VER_PATCH)
            set(PROJECT_PREFIX "${GVS_cmpProjectName}")

            cmpConfigureFileWithMD5Check( GENERATED_FILE_PATH        ${GVS_GENERATED_HEADER_FILE_PATH}
                                         CONFIGURED_TEMPLATE_PATH   ${CMP_CONFIGURED_FILES_SOURCE_DIR}/cmpVersion.h.in )

            cmpConfigureFileWithMD5Check( GENERATED_FILE_PATH        ${GVS_GENERATED_SOURCE_FILE_PATH}
                                         CONFIGURED_TEMPLATE_PATH   ${CMP_CONFIGURED_FILES_SOURCE_DIR}/cmpVersion.cpp.in )

            # MARK_AS_ADVANCED(${CMP_PROJECT_NAME}_VERSION ${CMP_PROJECT_NAME}_VER_MAJOR ${CMP_PROJECT_NAME}_VER_MINOR ${CMP_PROJECT_NAME}_VER_PATCH)

        endif()
    else()
       cmpGenerateVersionString( ${GVS_GENERATED_HEADER_FILE_PATH} ${GVS_GENERATED_SOURCE_FILE_PATH} ${GVS_NAMESPACE} ${GVS_cmpProjectName})
    endif()

endfunction()

#-------------------------------------------------------------------------------
# Function COMPILE_TOOL to help alleviate lots of extra code below for adding
# simple command line tools that just need one or two source files
#
function(COMPILE_TOOL)
    set(options)
    set(oneValueArgs TARGET DEBUG_EXTENSION BINARY_DIR COMPONENT INSTALL_DEST DEFINITION)
    set(multiValueArgs SOURCES LINK_LIBRARIES)
    cmake_parse_arguments(D3DTOOL "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )

    if( ${D3DTOOL_DEFINITION} )
    add_definitions(-D${DEFINITION})
    endif()

    BuildToolBundle(
        TARGET ${D3DTOOL_TARGET}
        SOURCES ${D3DTOOL_SOURCES}
        DEBUG_EXTENSION ${D3DTOOL_DEBUG_EXTENSION}
        VERSION_MAJOR ${D3DTOOL_DREAM3D_VER_MAJOR}
        VERSION_MINOR ${D3DTOOL_DREAM3D_VER_MINOR}
        VERSION_PATCH ${D3DTOOL_DREAM3D_VER_PATCH}
        BINARY_DIR    ${D3DTOOL_BINARY_DIR}
        LINK_LIBRARIES ${D3DTOOL_LINK_LIBRARIES}
        LIB_SEARCH_DIRS ${CMAKE_LIBRARY_OUTPUT_DIRECTORY} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
        COMPONENT     Applications
        INSTALL_DEST  "${D3DTOOL_INSTALL_DEST}"
    )

endfunction()

