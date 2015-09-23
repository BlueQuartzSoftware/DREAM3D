#--////////////////////////////////////////////////////////////////////////////
#--  Copyright (c) 2009, 2010 Michael A. Jackson. BlueQuartz Software
#--  All rights reserved.
#--  BSD License: http://www.opensource.org/licenses/bsd-license.html
#--////////////////////////////////////////////////////////////////////////////

include (CMakeParseArguments)
# include(${CMP_OSX_TOOLS_SOURCE_DIR}/OSX_BundleTools.cmake)
# include(${CMP_OSX_TOOLS_SOURCE_DIR}/ToolUtilities.cmake)

#-------------------------------------------------------------------------------
macro (cmp_IDE_GENERATED_PROPERTIES SOURCE_PATH HEADERS SOURCES)
    STRING(REPLACE "/" "\\\\" source_group_path ${SOURCE_PATH}  )
    source_group(${source_group_path} FILES ${HEADERS} ${SOURCES})

  #-- The following is needed if we ever start to use OS X Frameworks but only
  #--  works on CMake 2.6 and greater
  #set_property(SOURCE ${HEADERS}
  #             PROPERTY MACOSX_PACKAGE_LOCATION Headers/${NAME}
  #)

endmacro (cmp_IDE_GENERATED_PROPERTIES SOURCE_PATH HEADERS SOURCES)

#-------------------------------------------------------------------------------

macro (cmp_IDE_SOURCE_PROPERTIES SOURCE_PATH HEADERS SOURCES INSTALL_FILES)
    if(${INSTALL_FILES} EQUAL "1")
        INSTALL (FILES ${HEADERS}
                 DESTINATION "include/${SOURCE_PATH}"
                 COMPONENT Headers
        )
    endif()
    STRING(REPLACE "/" "\\\\" source_group_path "${SOURCE_PATH}"  )
    source_group("${source_group_path}" FILES ${HEADERS} ${SOURCES})

  #-- The following is needed if we ever start to use OS X Frameworks but only
  #--  works on CMake 2.6 and greater
  #set_property(SOURCE ${HEADERS}
  #             PROPERTY MACOSX_PACKAGE_LOCATION Headers/${NAME}
  #)

endmacro (cmp_IDE_SOURCE_PROPERTIES NAME HEADERS SOURCES INSTALL_FILES)

#-------------------------------------------------------------------------------
# This macro will set all the variables necessary to have a "good" OS X Application
# bundle. The variables are as follows:
#  TARGET_NAME - which can be taken from the ${TARGET_NAME} variable is needed
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
    MACOSX_BUNDLE_INFO_STRING "${PROJECT_NAME}${DBG_EXTENSION} Version ${VERSION_STRING}, Copyright 2015 BlueQuartz Software."
    MACOSX_BUNDLE_ICON_FILE ${ICON_FILE_NAME}
    MACOSX_BUNDLE_GUI_IDENTIFIER "${PROJECT_NAME}${DBG_EXTENSION}"
    MACOSX_BUNDLE_LONG_VERSION_STRING "${PROJECT_NAME}${DBG_EXTENSION} Version ${VERSION_STRING}"
    MACOSX_BUNDLE_BUNDLE_NAME ${PROJECT_NAME}${DBG_EXTENSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${VERSION_STRING}
    MACOSX_BUNDLE_BUNDLE_VERSION ${VERSION_STRING}
    MACOSX_BUNDLE_COPYRIGHT "Copyright 2015, BlueQuartz Software, LLC All Rights Reserved."
    MACOSX_BUNDLE_INFO_PLIST ${CMP_OSX_TOOLS_SOURCE_DIR}/MacOSXBundleInfo.plist.in
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
#  TARGET The name of the Target to use in the add_executable() commnad
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
    set(multiValueArgs SOURCES LINK_LIBRARIES LIB_SEARCH_DIRS QT5_MODULES QT_PLUGINS OTHER_PLUGINS)
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
        endif(QT_MAC_USE_COCOA)
        list(APPEND QAB_SOURCES ${qt_menu_nib_sources})
    elseif(WIN32)
        SET(GUI_TYPE WIN32)
        FILE (WRITE "${CMAKE_CURRENT_BINARY_DIR}/Icon.rc"
          "// Icon with lowest ID value placed first to ensure application icon\n"
          "// remains consistent on all systems.\n"
          "IDI_ICON1 ICON \"${QAB_ICON_FILE}\"")
        SET(QAB_ICON_FILE "${CMAKE_CURRENT_BINARY_DIR}/Icon.rc")
        cmp_IDE_GENERATED_PROPERTIES("${TARGET}/Generated/QrcFiles" "${QAB_ICON_FILE}" "")
    endif(APPLE)

#-- Append the Icon file/Image/Resource file to the list of Sources to compile
    list(APPEND QAB_SOURCES ${QAB_ICON_FILE})

    foreach(qt5module ${QAB_QT5_MODULES})
      set(QAB_LINK_LIBRARIES ${QAB_LINK_LIBRARIES} Qt5::${qt5module})
    endforeach()

#-- Add and Link our executable
    add_executable( ${QAB_TARGET} ${GUI_TYPE} ${QAB_SOURCES} )
    target_link_libraries( ${QAB_TARGET}
                            ${QAB_LINK_LIBRARIES}
                             )

#-- Make sure we have a proper bundle icon. This must occur AFTER the add_executable command
    if(APPLE)
      ConfigureMacOSXBundlePlist( "${QAB_TARGET}" "${QAB_DEBUG_EXTENSION}" "${QAB_ICON_FILE}"
                                     "${${QAB_TARGET}_VERSION_MAJOR}.${${QAB_TARGET}_VERSION_MINOR}.${${QAB_TARGET}_VERSION_PATCH}" )
    endif(APPLE)

#-- Set the Debug Suffix for the application
    SET_TARGET_PROPERTIES( ${QAB_TARGET}
                PROPERTIES
                DEBUG_OUTPUT_NAME ${QAB_TARGET}${QAB_DEBUG_EXTENSION}
                RELEASE_OUTPUT_NAME ${QAB_TARGET}
    )
    if(CMAKE_SYSTEM_NAME MATCHES "Linux")
        SET_TARGET_PROPERTIES( ${QAB_TARGET}
                PROPERTIES
                INSTALL_RPATH \$ORIGIN/../lib
    )
    endif()
#-- Create install rules for any Qt Plugins that are needed
    set(pi_dest ${QAB_INSTALL_DEST}/Plugins)
    # if we are on OS X then we set the plugin installation location to inside the App bundle
    if(APPLE)
        set(pi_dest ${QAB_TARGET}.app/Contents/Plugins)
        set(osx_app_name ${QAB_TARGET})
        if(CMAKE_BUILD_TYPE MATCHES "Debug")
            set(pi_dest ${QAB_TARGET}${QAB_DEBUG_EXTENSION}.app/Contents/Plugins)
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
    install(TARGETS ${QAB_TARGET}
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

        #set(OPTIMIZE_BUNDLE_SHELL_SCRIPT
        #    "${QAB_BINARY_DIR}/OSX_Scripts/${QAB_TARGET}_OptimizeBundle.sh")

        configure_file("${CMP_OSX_TOOLS_SOURCE_DIR}/CompleteBundle.cmake.in"
                "${OSX_MAKE_STANDALONE_BUNDLE_CMAKE_SCRIPT}" @ONLY IMMEDIATE)

        #set(PROJECT_INSTALL_DIR ${osx_app_name}.app)
        #configure_file("${CMP_OSX_TOOLS_SOURCE_DIR}/ThinAndShareLibraries.sh.in"
        #        "${OPTIMIZE_BUNDLE_SHELL_SCRIPT}" @ONLY IMMEDIATE)

        install(SCRIPT "${OSX_MAKE_STANDALONE_BUNDLE_CMAKE_SCRIPT}" COMPONENT ${QAB_COMPONENT})
    endif(APPLE)

#-- This should be called when we are on Linux
    if(CMAKE_SYSTEM_NAME MATCHES "Linux")
      set(linux_app_name ${QAB_TARGET})
      set(LINUX_MAKE_STANDALONE_LAUNCH_SCRIPT
                  "${QAB_BINARY_DIR}/LINUX_Scripts/${QAB_TARGET}.sh")
      set(lib_suffix "")
      set(build_type "${CMAKE_BUILD_TYPE}")
      if("${build_type}" STREQUAL "Debug")
          set(lib_suffix "_debug")
      endif()

      set(LINUX_INSTALL_LIBS_CMAKE_SCRIPT
              "${QAB_BINARY_DIR}/LINUX_Scripts/${QAB_TARGET}_CompleteBundle.cmake")
      set(OPTIMIZE_BUNDLE_SHELL_SCRIPT
              "${QAB_BINARY_DIR}/LINUX_Scripts/${QAB_TARGET}_InstallLibraries.sh")

      configure_file("${CMP_LINUX_TOOLS_SOURCE_DIR}/CompleteBundle.cmake.in"
                    "${LINUX_INSTALL_LIBS_CMAKE_SCRIPT}" @ONLY IMMEDIATE)
      set(PROJECT_INSTALL_DIR ${linux_app_name})

      configure_file("${CMP_LINUX_TOOLS_SOURCE_DIR}/InstallLibraries.sh.in"
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
#  TARGET The name of the Target to use in the add_executable() commnad
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
    add_executable( ${QAB_TARGET} ${GUI_TYPE} ${QAB_SOURCES} )
    target_link_libraries( ${QAB_TARGET}
                            ${QAB_LINK_LIBRARIES} )

#-- Set the Debug Suffix for the application
    SET_TARGET_PROPERTIES( ${QAB_TARGET}
                PROPERTIES
                DEBUG_OUTPUT_NAME ${QAB_TARGET}${QAB_DEBUG_EXTENSION}
                RELEASE_OUTPUT_NAME ${QAB_TARGET}
    )
    if(CMAKE_SYSTEM_NAME MATCHES "Linux")
      SET_TARGET_PROPERTIES( ${QAB_TARGET}
            PROPERTIES
            INSTALL_RPATH \$ORIGIN/../lib )
    endif()
    if(NOT "${QAB_SOLUTION_FOLDER}" STREQUAL "")
      SET_TARGET_PROPERTIES(${QAB_TARGET}
                          PROPERTIES FOLDER ${QAB_SOLUTION_FOLDER})
    endif()

  if( NOT ${QAB_INSTALL_DEST} STREQUAL "")
  #-- Create an Install Rule for the main app bundle target
    install(TARGETS ${QAB_TARGET}
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

        set(PROJECT_INSTALL_DIR "bin")
        configure_file("${CMP_OSX_TOOLS_SOURCE_DIR}/CompleteTool.cmake.in"
                "${OSX_MAKE_STANDALONE_BUNDLE_CMAKE_SCRIPT}" @ONLY IMMEDIATE)

        configure_file("${CMP_OSX_TOOLS_SOURCE_DIR}/CompleteTool.sh.in"
                "${OPTIMIZE_BUNDLE_SHELL_SCRIPT}" @ONLY IMMEDIATE)

        install(SCRIPT "${OSX_MAKE_STANDALONE_BUNDLE_CMAKE_SCRIPT}" COMPONENT ${QAB_COMPONENT})
    endif(APPLE)
  endif()
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
        install(TARGETS ${EXE_NAME}
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
        # use, i.e. don't skip the full RPATH for the build tree
        SET(CMAKE_SKIP_BUILD_RPATH  FALSE)

        # when building, don't use the install RPATH already
        # (but later on when installing)
        SET(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE) 

        SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")

        # add the automatically determined parts of the RPATH
        # which point to directories outside the build tree to the install RPATH
        SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

    endif(APPLE)

    if(CMAKE_SYSTEM_NAME MATCHES "Linux")
      set(CMAKE_INSTALL_RPATH "\$ORIGIN/../lib")
      SET_TARGET_PROPERTIES( ${targetName}
                PROPERTIES
                INSTALL_RPATH \$ORIGIN/../lib)
    endif()



   endif( BUILD_SHARED_LIBS)

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
    endif(WIN32 AND NOT MINGW)


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
            install(TARGETS ${targetName}
                    DESTINATION ./Plugins
                    CONFIGURATIONS ${btype}
                    COMPONENT Applications)
        endforeach()
    endif()
endmacro()

# --------------------------------------------------------------------
#-- Copy all the dependent DLLs into the current build directory so that the test
#-- can run.
macro(CMP_COPY_DEPENDENT_LIBRARIES _libraryList)
#  message(STATUS "#--------------------------------------------")
#  message(STATUS "CMP_COPY_DEPENDENT_LIBRARIES: ${_libraryList}")
  set(SUPPORT_LIB_OPTION 1)
  if(MSVC_IDE)
    set(SUPPORT_LIB_OPTION 0)
  elseif(APPLE) # Apple systems do NOT need this so just skip this entirely
    set(SUPPORT_LIB_OPTION 2)
  elseif(UNIX AND NOT MSVC)
    set(SUPPORT_LIB_OPTION 3)
  endif()

  set(_libraryList ${_libraryList})
  set(TYPES Debug Release)
  if( ${SUPPORT_LIB_OPTION} EQUAL 1)
    set(TYPES ${CMAKE_BUILD_TYPE})
  endif()

  if(SUPPORT_LIB_OPTION EQUAL 0 OR SUPPORT_LIB_OPTION EQUAL 1)
    # Make all the necessary intermediate directories for Visual Studio
    if(MSVC_IDE)
      file(MAKE_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug)
      file(MAKE_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release)
      file(MAKE_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/MinSizeRel)
      file(MAKE_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/RelWithDebInfo)
    endif()
    FOREACH(lib ${_libraryList})

      STRING(TOUPPER ${lib} upperlib)
      # message(STATUS "upperlib: ${upperlib}")
      # message(STATUS "${upperlib}_IS_SHARED: ${${upperlib}_IS_SHARED}")
      if(${upperlib}_IS_SHARED)
        FOREACH(BTYPE ${TYPES})
          # Smessage(STATUS "Looking for ${BTYPE} DLL Version of ${lib}")
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
          # message(STATUS "${upperlib}_LIBRARY_DLL_${TYPE}: ${${upperlib}_LIBRARY_DLL_${TYPE}}")
          mark_as_advanced(${upperlib}_LIBRARY_DLL_${TYPE})
          if( ${${upperlib}_LIBRARY_DLL_${TYPE}} STREQUAL  "${upperlib}_LIBRARY_DLL_${TYPE}-NOTFOUND")
            message(FATAL_ERROR "According to how ${upperlib}_LIBRARY_${TYPE} was found the library should"
                                " have been built as a DLL but no .dll file can be found. I looked in the "
                                " following locations:\n  ${lib_path}\n  ${lib_path}/..\n  ${lib_path}/../bin\n  ${${upperlib}_BIN_DIR}")
          endif()

         # SET(${upperlib}_LIBRARY_DLL_${TYPE} "${${upperlib}_LIBRARY_DLL_${TYPE}}/${lib_name}.dll" CACHE FILEPATH "The path to the DLL Portion of the library" FORCE)
         # message(STATUS "${upperlib}_LIBRARY_DLL_${TYPE}: ${${upperlib}_LIBRARY_DLL_${TYPE}}")
          #message(STATUS "Generating Copy Rule for DLL: ${${upperlib}_LIBRARY_DLL_${TYPE}}")
          if(SUPPORT_LIB_OPTION EQUAL 1)
            set(BTYPE ".")
          endif()
          ADD_CUSTOM_TARGET(ZZ_${upperlib}_DLL_${TYPE}-Copy ALL
                          COMMAND ${CMAKE_COMMAND} -E copy_if_different ${${upperlib}_LIBRARY_DLL_${TYPE}}
                          ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${BTYPE}/
                          COMMENT "  Copy: ${${upperlib}_LIBRARY_DLL_${TYPE}}\n    To: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${BTYPE}/")
          set_target_properties(ZZ_${upperlib}_DLL_${TYPE}-Copy PROPERTIES FOLDER ZZ_COPY_FILES)
        ENDFOREACH(BTYPE ${TYPES})
      endif(${upperlib}_IS_SHARED)
    ENDFOREACH(lib ${_libraryList})
  endif()
endmacro()

# --------------------------------------------------------------------
# This macro generates install rules for Visual Studio builds so that
# dependent DLL libraries (HDF5, Tiff, Expat, DataModel) will be
# properly installed with your project.
# --------------------------------------------------------------------
macro(CMP_LIBRARIES_INSTALL_RULES _libraryList destination)
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
              install(FILES ${${upperlib}_LIBRARY_DLL_${TYPE}}
                DESTINATION ${destination}
                CONFIGURATIONS ${BTYPE}
                COMPONENT Applications)
          endif()

        ENDFOREACH(BTYPE ${TYPES})
    ENDFOREACH(lib ${_libraryList})
  endif(MSVC)

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
              install(FILES ${${upperlib}_LIBRARY_SO_${TYPE}}
                DESTINATION "lib"
                CONFIGURATIONS ${BTYPE}
                COMPONENT Applications)
          endif()
      ENDFOREACH(lib ${_libraryList})
    endif()
ENDmacro()

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

function(cmpGenerateVersionString)
  set(options)
  set(oneValueArgs GENERATED_HEADER_FILE_PATH GENERATED_SOURCE_FILE_PATH
                   NAMESPACE PROJECT_NAME EXPORT_MACRO )
  cmake_parse_arguments(GVS "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )

  if(0)
    message(STATUS "--------------------------------------------")
    message(STATUS "GVS_NAMESPACE: ${GVS_NAMESPACE}")
    message(STATUS "GVS_PROJECT_NAME: ${GVS_PROJECT_NAME}")
    message(STATUS "GVS_GENERATED_HEADER_FILE_PATH: ${GVS_GENERATED_HEADER_FILE_PATH}")
    message(STATUS "GVS_GENERATED_SOURCE_FILE_PATH: ${GVS_GENERATED_SOURCE_FILE_PATH}")
    message(STATUS "GVS_PROJECT_SOURCE_DIR: ${GVS_PROJECT_SOURCE_DIR}")
    message(STATUS "GVS_PROJECT_VERSION_MAJOR: ${GVS_PROJECT_VERSION_MAJOR}")
    message(STATUS "${GVS_PROJECT_NAME}_BUILD_DATE: ${${GVS_PROJECT_NAME}_BUILD_DATE}")
  endif()

  set(PROJECT_PREFIX "${GVS_PROJECT_NAME}")
  set(VERSION_GEN_NAME "${GVS_PROJECT_NAME}")
  set(VERSION_GEN_NAMESPACE "${GVS_NAMESPACE}")
  set(VERSION_GEN_NAMESPACE_EXPORT "${GVS_EXPORT_MACRO}")
  set(VERSION_GEN_VER_MAJOR  ${${GVS_PROJECT_NAME}_VERSION_MAJOR})
  set(VERSION_GEN_VER_MINOR  ${${GVS_PROJECT_NAME}_VERSION_MINOR})
  set(VERSION_GEN_VER_PATCH "0")
  set(VERSION_GEN_VER_REVISION "0")
  set(VERSION_BUILD_DATE ${${GVS_PROJECT_NAME}_BUILD_DATE})
  set(VERSION_GEN_HEADER_FILE_NAME ${GVS_GENERATED_HEADER_FILE_PATH})

  set(${GVS_PROJECT_NAME}_VERSION_PATCH "${VERSION_GEN_VER_PATCH}" PARENT_SCOPE)
  set(${GVS_PROJECT_NAME}_VERSION_TWEAK "${VERSION_GEN_VER_REVISION}" PARENT_SCOPE)

  cmpConfigureFileWithMD5Check( GENERATED_FILE_PATH        ${${GVS_PROJECT_NAME}_BINARY_DIR}/${GVS_GENERATED_HEADER_FILE_PATH}
                                CONFIGURED_TEMPLATE_PATH   ${CMP_CONFIGURED_FILES_SOURCE_DIR}/cmpVersion.h.in )

  cmpConfigureFileWithMD5Check( GENERATED_FILE_PATH        ${${GVS_PROJECT_NAME}_BINARY_DIR}/${GVS_GENERATED_SOURCE_FILE_PATH}
                                CONFIGURED_TEMPLATE_PATH   ${CMP_CONFIGURED_FILES_SOURCE_DIR}/cmpVersion.cpp.in )

endfunction()

#-------------------------------------------------------------------------------
# This function generates a file ONLY if the MD5 between the "to be" generated file
# and the current file are different. This will help reduce recompiles based on
# the generation of files that are really the same.
#
function(cmpConfigureFileWithMD5Check)
    set(options)
    set(oneValueArgs CONFIGURED_TEMPLATE_PATH GENERATED_FILE_PATH )
    cmake_parse_arguments(GVS "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )

 #   message(STATUS "   GVS_CONFIGURED_TEMPLATE_PATH: ${GVS_CONFIGURED_TEMPLATE_PATH}")
 #   message(STATUS "   GVS_GENERATED_FILE_PATH: ${GVS_GENERATED_FILE_PATH}")

    # Only Generate a file if it is different than what is already there.
    if(EXISTS ${GVS_GENERATED_FILE_PATH} )
        file(MD5 ${GVS_GENERATED_FILE_PATH} VERSION_HDR_MD5)
        configure_file(${GVS_CONFIGURED_TEMPLATE_PATH}   ${GVS_GENERATED_FILE_PATH}_tmp  )

        file(MD5 ${GVS_GENERATED_FILE_PATH}_tmp VERSION_GEN_HDR_MD5)
        #message(STATUS "  File Exists, doing MD5 Comparison")

        # Compare the MD5 checksums. If they are different then configure the file into the proper location
        if(NOT "${VERSION_HDR_MD5}" STREQUAL "${VERSION_GEN_HDR_MD5}")
            #message(STATUS "   ${VERSION_GEN_HDR_MD5}")
            #message(STATUS "   ${VERSION_HDR_MD5}")
            #message(STATUS "  Files differ: Replacing with newly generated file")
            configure_file(${GVS_CONFIGURED_TEMPLATE_PATH}  ${GVS_GENERATED_FILE_PATH} )
        else()
            #message(STATUS "  NO Difference in Files")
        endif()
        file(REMOVE ${GVS_GENERATED_FILE_PATH}_tmp)
    else()
      # message(STATUS "  File does NOT Exist, Generating one...")
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
# This function will attempt to generate a build date/time string.
#
#-------------------------------------------------------------------------------
function(cmpGenerateBuildDate)
  set(oneValueArgs PROJECT_NAME )
  cmake_parse_arguments(GVS "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )

  IF (WIN32)
      EXECUTE_PROCESS(COMMAND "cmd" " /C date /T" OUTPUT_VARIABLE RESULT)
      string(REPLACE  " " ";" RESULT ${RESULT})
      list(GET RESULT 1 RESULT)
      string(REPLACE "/" ";" RESULT ${RESULT})
      list(GET RESULT 2 YEAR)
      list(GET RESULT 0 MONTH)
      list(GET RESULT 1 DAY)
      set(${GVS_PROJECT_NAME}_BUILD_DATE "${YEAR}/${MONTH}/${DAY}" PARENT_SCOPE)
      #message(STATUS "${GVS_PROJECT_NAME}_BUILD_DATE: ${${GVS_PROJECT_NAME}_BUILD_DATE}")
  ELSEIF(UNIX)
      EXECUTE_PROCESS(COMMAND "date" "+%Y/%m/%d/" OUTPUT_VARIABLE RESULT)
      string(REPLACE "/" ";" RESULT ${RESULT})
      list(GET RESULT 0 YEAR)
      list(GET RESULT 1 MONTH)
      list(GET RESULT 2 DAY)
      set(${GVS_PROJECT_NAME}_BUILD_DATE "${YEAR}/${MONTH}/${DAY}" PARENT_SCOPE)
      #message(STATUS "${GVS_PROJECT_NAME}_BUILD_DATE: ${${GVS_PROJECT_NAME}_BUILD_DATE}")
  ELSE (WIN32)
      MESSAGE(SEND_ERROR "date for this operating system not implemented")
      set(${GVS_PROJECT_NAME}_BUILD_DATE "NO_DATE" PARENT_SCOPE)
  ENDIF (WIN32)

endfunction()



#-------------------------------------------------------------------------------
# We are going to use Git functionality to create a version number for our package
# The specific functionality we are going to use is the 'git describe' function
# which should return the latest tag, the number commits since that tag and the
# SHA1 of that commit. If we fail to find git then we fall back to a manually
# entered version number.
function(cmpGitRevisionString)
  set(options)
  set(oneValueArgs GENERATED_HEADER_FILE_PATH GENERATED_SOURCE_FILE_PATH
                   NAMESPACE PROJECT_NAME EXPORT_MACRO )
  cmake_parse_arguments(GVS "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )

  if(0)
    message(STATUS "--------------------------------------------")
    message(STATUS "GVS_NAMESPACE: ${GVS_NAMESPACE}")
    message(STATUS "GVS_PROJECT_NAME: ${GVS_PROJECT_NAME}")
    message(STATUS "GVS_GENERATED_HEADER_FILE_PATH: ${GVS_GENERATED_HEADER_FILE_PATH}")
    message(STATUS "GVS_GENERATED_SOURCE_FILE_PATH: ${GVS_GENERATED_SOURCE_FILE_PATH}")
    message(STATUS "GVS_PROJECT_SOURCE_DIR: ${GVS_PROJECT_SOURCE_DIR}")
    message(STATUS "GVS_PROJECT_VERSION_MAJOR: ${GVS_PROJECT_VERSION_MAJOR}")
    message(STATUS "GVS_EXPORT_MACRO: ${GVS_EXPORT_MACRO}")
    message(STATUS "${GVS_PROJECT_NAME}_BUILD_DATE: ${${GVS_PROJECT_NAME}_BUILD_DATE}")
  endif()


  # Run 'git describe' to get our tag offset
  execute_process(COMMAND ${GIT_EXECUTABLE} describe --long
                  OUTPUT_VARIABLE DVERS
                  RESULT_VARIABLE did_run
                  ERROR_VARIABLE git_error
                  WORKING_DIRECTORY ${${GVS_PROJECT_NAME}_SOURCE_DIR} )

  # message(STATUS "DVERS: ${DVERS}")
  set(PROJECT_PREFIX "${GVS_PROJECT_NAME}")
  set(VERSION_GEN_NAME "${GVS_PROJECT_NAME}")
  set(VERSION_GEN_NAMESPACE "${GVS_NAMESPACE}")
  set(VERSION_GEN_NAMESPACE_EXPORT "${GVS_EXPORT_MACRO}")
  set(VERSION_GEN_VER_MAJOR  ${${GVS_PROJECT_NAME}_VERSION_MAJOR})
  set(VERSION_GEN_VER_MINOR  ${${GVS_PROJECT_NAME}_VERSION_MINOR})
  set(VERSION_GEN_VER_PATCH "0")
  set(VERSION_GEN_VER_REVISION "0")
  set(VERSION_BUILD_DATE ${${GVS_PROJECT_NAME}_BUILD_DATE})
  set(VERSION_GEN_HEADER_FILE_NAME ${GVS_GENERATED_HEADER_FILE_PATH})

  #-- Make sure that actually worked and if not just generate some dummy values
  if(DVERS STREQUAL "")
    message(STATUS "DVERS was Empty. Generating Default version strings")
  else()
    string(STRIP ${DVERS} DVERS)
    string(REPLACE  "-" ";" VERSION_LIST ${DVERS})
    list(LENGTH VERSION_LIST VERSION_LIST_LENGTH)

    list(GET VERSION_LIST 1 VERSION_GEN_VER_PATCH)
    list(GET VERSION_LIST 2 VERSION_GEN_VER_REVISION)


    string(SUBSTRING ${VERSION_GEN_VER_REVISION} 1 -1 VERSION_GEN_VER_REVISION)

  endif()

  set(${GVS_PROJECT_NAME}_VERSION_PATCH "${VERSION_GEN_VER_PATCH}" PARENT_SCOPE)
  set(${GVS_PROJECT_NAME}_VERSION_TWEAK "${VERSION_GEN_VER_REVISION}" PARENT_SCOPE)


  cmpConfigureFileWithMD5Check( GENERATED_FILE_PATH        ${${GVS_PROJECT_NAME}_BINARY_DIR}/${GVS_GENERATED_HEADER_FILE_PATH}
                                CONFIGURED_TEMPLATE_PATH   ${CMP_CONFIGURED_FILES_SOURCE_DIR}/cmpVersion.h.in )

  cmpConfigureFileWithMD5Check( GENERATED_FILE_PATH        ${${GVS_PROJECT_NAME}_BINARY_DIR}/${GVS_GENERATED_SOURCE_FILE_PATH}
                                CONFIGURED_TEMPLATE_PATH   ${CMP_CONFIGURED_FILES_SOURCE_DIR}/cmpVersion.cpp.in )
endfunction()


#-------------------------------------------------------------------------------
#
#
#
function(cmpRevisionString)
  set(options)
  set(oneValueArgs GENERATED_HEADER_FILE_PATH GENERATED_SOURCE_FILE_PATH
                   NAMESPACE PROJECT_NAME EXPORT_MACRO )
  cmake_parse_arguments(GVS "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )

  # Generate our Build date in the Form of YYYY/MM/DD
  cmpGenerateBuildDate(PROJECT_NAME ${GVS_PROJECT_NAME})

  if(GIT_FOUND)
    cmpGitRevisionString( GENERATED_HEADER_FILE_PATH "${GVS_GENERATED_HEADER_FILE_PATH}"
                          GENERATED_SOURCE_FILE_PATH "${GVS_GENERATED_SOURCE_FILE_PATH}"
                          NAMESPACE "${GVS_NAMESPACE}"
                          PROJECT_NAME "${GVS_PROJECT_NAME}"
                          EXPORT_MACRO "${GVS_EXPORT_MACRO}")
  else()
    cmpGenerateVersionString( GENERATED_HEADER_FILE_PATH "${GVS_GENERATED_HEADER_FILE_PATH}"
                              GENERATED_SOURCE_FILE_PATH "${GVS_GENERATED_SOURCE_FILE_PATH}"
                              NAMESPACE "${GVS_NAMESPACE}"
                              PROJECT_NAME "${GVS_PROJECT_NAME}"
                              EXPORT_MACRO "${GVS_EXPORT_MACRO}")
  endif()

  # We have to "set" our variable into the parent scope. What a pain. Pass by reference would be really nice about now
  set(${GVS_PROJECT_NAME}_VERSION_PATCH "${${GVS_PROJECT_NAME}_VERSION_PATCH}" PARENT_SCOPE)
  set(${GVS_PROJECT_NAME}_VERSION_TWEAK "${${GVS_PROJECT_NAME}_VERSION_TWEAK}" PARENT_SCOPE)
  set(${GVS_PROJECT_NAME}_BUILD_DATE "${${GVS_PROJECT_NAME}_BUILD_DATE}" PARENT_SCOPE)

#  message(STATUS "${GVS_PROJECT_NAME}_VERSION_MAJOR: ${${GVS_PROJECT_NAME}_VERSION_MAJOR}")
#  message(STATUS "${GVS_PROJECT_NAME}_VERSION_MINOR: ${${GVS_PROJECT_NAME}_VERSION_MINOR}")
#  message(STATUS "${GVS_PROJECT_NAME}_VERSION_PATCH: ${${GVS_PROJECT_NAME}_VERSION_PATCH}")
#  message(STATUS "${GVS_PROJECT_NAME}_VERSION_TWEAK: ${${GVS_PROJECT_NAME}_VERSION_TWEAK}")
#  message(STATUS "${GVS_PROJECT_NAME}_BUILD_DATE: ${${GVS_PROJECT_NAME}_BUILD_DATE}")
endfunction()

#-------------------------------------------------------------------------------
# Function COMPILE_TOOL to help alleviate lots of extra code below for adding
# simple command line tools that just need one or two source files
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



#-------------------------------------------------------------------------------
#
#
#
function(echo_target_property tgt prop)
  # v for value, d for defined, s for set
  get_property(v TARGET ${tgt} PROPERTY ${prop})
  get_property(d TARGET ${tgt} PROPERTY ${prop} DEFINED)
  get_property(s TARGET ${tgt} PROPERTY ${prop} SET)

  # only produce output for values that are set
  if(s)
    message("tgt='${tgt}' prop='${prop}'")
    message("  value='${v}'")
    message("  defined='${d}'")
    message("  set='${s}'")
    message("")
  endif()
endfunction()

function(echo_target tgt)
  if(NOT TARGET ${tgt})
    message("There is no target named '${tgt}'")
    return()
  endif()

  set(props
    DEBUG_OUTPUT_NAME
    DEBUG_POSTFIX
    RELEASE_OUTPUT_NAME
    RELEASE_POSTFIX
    ARCHIVE_OUTPUT_DIRECTORY
    ARCHIVE_OUTPUT_DIRECTORY_DEBUG
    ARCHIVE_OUTPUT_DIRECTORY_RELEASE
    ARCHIVE_OUTPUT_NAME
    ARCHIVE_OUTPUT_NAME_DEBUG
    ARCHIVE_OUTPUT_NAME_RELEASE
    AUTOMOC
    AUTOMOC_MOC_OPTIONS
    BUILD_WITH_INSTALL_RPATH
    BUNDLE
    BUNDLE_EXTENSION
    COMPILE_DEFINITIONS
    COMPILE_DEFINITIONS_DEBUG
    COMPILE_DEFINITIONS_RELEASE
    COMPILE_FLAGS
    DEBUG_POSTFIX
    RELEASE_POSTFIX
    DEFINE_SYMBOL
    ENABLE_EXPORTS
    EXCLUDE_FROM_ALL
    EchoString
    FOLDER
    FRAMEWORK
    Fortran_FORMAT
    Fortran_MODULE_DIRECTORY
    GENERATOR_FILE_NAME
    GNUtoMS
    HAS_CXX
    IMPLICIT_DEPENDS_INCLUDE_TRANSFORM
    IMPORTED
    IMPORTED_CONFIGURATIONS
    IMPORTED_IMPLIB
    IMPORTED_IMPLIB_DEBUG
    IMPORTED_IMPLIB_RELEASE
    IMPORTED_LINK_DEPENDENT_LIBRARIES
    IMPORTED_LINK_DEPENDENT_LIBRARIES_DEBUG
    IMPORTED_LINK_DEPENDENT_LIBRARIES_RELEASE
    IMPORTED_LINK_INTERFACE_LANGUAGES
    IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG
    IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE
    IMPORTED_LINK_INTERFACE_LIBRARIES
    IMPORTED_LINK_INTERFACE_LIBRARIES_DEBUG
    IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE
    IMPORTED_LINK_INTERFACE_MULTIPLICITY
    IMPORTED_LINK_INTERFACE_MULTIPLICITY_DEBUG
    IMPORTED_LINK_INTERFACE_MULTIPLICITY_RELEASE
    IMPORTED_LOCATION
    IMPORTED_LOCATION_DEBUG
    IMPORTED_LOCATION_RELEASE
    IMPORTED_NO_SONAME
    IMPORTED_NO_SONAME_DEBUG
    IMPORTED_NO_SONAME_RELEASE
    IMPORTED_SONAME
    IMPORTED_SONAME_DEBUG
    IMPORTED_SONAME_RELEASE
    IMPORT_PREFIX
    IMPORT_SUFFIX
    INCLUDE_DIRECTORIES
    INSTALL_NAME_DIR
    INSTALL_RPATH
    INSTALL_RPATH_USE_LINK_PATH
    INTERPROCEDURAL_OPTIMIZATION
    INTERPROCEDURAL_OPTIMIZATION_DEBUG
    INTERPROCEDURAL_OPTIMIZATION_RELEASE
    LABELS
    LIBRARY_OUTPUT_DIRECTORY
    LIBRARY_OUTPUT_DIRECTORY_DEBUG
    LIBRARY_OUTPUT_DIRECTORY_RELEASE
    LIBRARY_OUTPUT_NAME
    LIBRARY_OUTPUT_NAME_DEBUG
    LIBRARY_OUTPUT_NAME_RELEASE
    LINKER_LANGUAGE
    LINK_DEPENDS
    LINK_FLAGS
    LINK_FLAGS_DEBUG
    LINK_FLAGS_RELEASE
    LINK_INTERFACE_LIBRARIES
    LINK_INTERFACE_LIBRARIES_DEBUG
    LINK_INTERFACE_LIBRARIES_RELEASE
    LINK_INTERFACE_MULTIPLICITY
    LINK_INTERFACE_MULTIPLICITY_DEBUG
    LINK_INTERFACE_MULTIPLICITY_RELEASE
    LINK_SEARCH_END_STATIC
    LINK_SEARCH_START_STATIC
    LOCATION
    LOCATION_DEBUG
    LOCATION_RELEASE
    MACOSX_BUNDLE
    MACOSX_BUNDLE_INFO_PLIST
    MACOSX_FRAMEWORK_INFO_PLIST
    MAP_IMPORTED_CONFIG_DEBUG
    MAP_IMPORTED_CONFIG_RELEASE
    OSX_ARCHITECTURES
    OSX_ARCHITECTURES_DEBUG
    OSX_ARCHITECTURES_RELEASE
    OUTPUT_NAME
    OUTPUT_NAME_DEBUG
    OUTPUT_NAME_RELEASE
    POST_INSTALL_SCRIPT
    PREFIX
    PRE_INSTALL_SCRIPT
    PRIVATE_HEADER
    PROJECT_LABEL
    PUBLIC_HEADER
    RESOURCE
    RULE_LAUNCH_COMPILE
    RULE_LAUNCH_CUSTOM
    RULE_LAUNCH_LINK
    RUNTIME_OUTPUT_DIRECTORY
    RUNTIME_OUTPUT_DIRECTORY_DEBUG
    RUNTIME_OUTPUT_DIRECTORY_RELEASE
    RUNTIME_OUTPUT_NAME
    RUNTIME_OUTPUT_NAME_DEBUG
    RUNTIME_OUTPUT_NAME_RELEASE
    SKIP_BUILD_RPATH
    SOURCES
    SOVERSION
    STATIC_LIBRARY_FLAGS
    STATIC_LIBRARY_FLAGS_DEBUG
    STATIC_LIBRARY_FLAGS_RELEASE
    SUFFIX
    TYPE
    VERSION
    VS_DOTNET_REFERENCES
    VS_GLOBAL_WHATEVER
    VS_GLOBAL_KEYWORD
    VS_GLOBAL_PROJECT_TYPES
    VS_KEYWORD
    VS_SCC_AUXPATH
    VS_SCC_LOCALPATH
    VS_SCC_PROJECTNAME
    VS_SCC_PROVIDER
    VS_WINRT_EXTENSIONS
    VS_WINRT_REFERENCES
    WIN32_EXECUTABLE
    XCODE_ATTRIBUTE_WHATEVER
    )

  message("======================== ${tgt} ========================")
  foreach(p ${props})
    echo_target_property("${t}" "${p}")
  endforeach()
  message("")
endfunction()


function(echo_targets)
  set(tgts ${ARGV})
  foreach(t ${tgts})
    echo_target("${t}")
  endforeach()
endfunction()

# If you need to print out all the properties of a target then you can use the
# following code to do this.
#
# set(targets  DREAM3DLib OrientationLib  )
# echo_targets(${targets})

