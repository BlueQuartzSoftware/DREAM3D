#--////////////////////////////////////////////////////////////////////////////
#--  Copyright (c) 2009, 2010 Michael A. Jackson. BlueQuartz Software
#--  All rights reserved.
#--  BSD License: http://www.opensource.org/licenses/bsd-license.html
#--////////////////////////////////////////////////////////////////////////////

include(${CMP_OSX_TOOLS_SOURCE_DIR}/OSX_BundleTools.cmake)
include(${CMP_OSX_TOOLS_SOURCE_DIR}/ToolUtilities.cmake)

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
    if (${INSTALL_FILES} EQUAL "1")
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


# ------------------------------------------------------------------------------ 
# This CMake code installs the needed support libraries
# ------------------------------------------------------------------------------ 
macro(cmp_InstallationSupport EXE_NAME EXE_DEBUG_EXTENSION EXE_BINARY_DIR 
                                appNeedsPlugins installFiles comp dest searchDirs)

message(STATUS "EXE_NAME: ${EXE_NAME}")
message(STATUS "EXE_DEBUG_EXTENSION: ${EXE_DEBUG_EXTENSION}")
message(STATUS "EXE_BINARY_DIR: ${EXE_BINARY_DIR}")
message(STATUS "appNeedsPlugins: ${appNeedsPlugins}")
message(STATUS "installFiles: ${installFiles}")
message(STATUS "comp: ${comp}")
message(STATUS "dest: ${dest}")
message(STATUS "searchDirs: ${searchDirs}")



    SET_TARGET_PROPERTIES( ${EXE_NAME} 
                PROPERTIES
                DEBUG_OUTPUT_NAME ${EXE_NAME}${EXE_DEBUG_EXTENSION}
                RELEASE_OUTPUT_NAME ${EXE_NAME}
    )
    if ( ${installFiles} EQUAL 1 )
        INSTALL(TARGETS ${EXE_NAME} 
            COMPONENT ${comp}
            RUNTIME DESTINATION ${dest}
            LIBRARY DESTINATION ${dest} 
            ARCHIVE DESTINATION ${dest}        
            BUNDLE DESTINATION ${dest}
        )   
        
        
        # --------------------------------------------------------------------
        # Get the plugin list from the plugin file
        if ( ${appNeedsPlugins})
          file(READ ${CMP_PLUGIN_LIST_FILE} CMP_COMPLETE_PLUGIN_LIST)
          file(READ ${CMP_PLUGIN_SEARCHDIR_FILE} CMP_PLUGIN_SEARCH_DIRS)
        endif()
        
        if (APPLE)
            # --- If we are on OS X copy all the embedded libraries to the app bundle
            # message(STATUS "Creating Install CMake file for GUI application ${EXE_NAME}")
            set (PLUGIN_SEARCH_DIRS "searchDirs")
            if(${GUI_TYPE} STREQUAL "MACOSX_BUNDLE")
                include (${CMP_OSX_TOOLS_SOURCE_DIR}/OSX_BundleTools.cmake)
                if(CMAKE_BUILD_TYPE MATCHES "Debug")
                    MakeOSXBundleApp( "${EXE_NAME}${EXE_DEBUG_EXTENSION}" 
                                ${EXE_BINARY_DIR}
                                ${CMP_OSX_TOOLS_SOURCE_DIR} )
                else (CMAKE_BUILD_TYPE MATCHES "Debug")
                    MakeOSXBundleApp(${EXE_NAME} 
                             ${EXE_BINARY_DIR}
                             ${CMP_OSX_TOOLS_SOURCE_DIR} )
                endif()
            endif()
        else()    
            #------------------------------------------------------------------------------
            # Add install rules for required system runtimes such as MSVCRxx.dll
            SET (CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_SKIP ON)
            SET (CMAKE_INSTALL_DEBUG_LIBRARIES ON)
            INCLUDE(InstallRequiredSystemLibraries)
            IF (CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS)
              INSTALL(FILES ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS}
                DESTINATION ./
                PERMISSIONS OWNER_WRITE OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ
                COMPONENT Runtime)
            ENDIF (CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS)                
            
            set(_extension)
            set(_dir "")
            
            IF(WIN32)
              set(_extension ".exe")
              set(_dir "")
            ENDIF(WIN32)
            set(APPS "\${CMAKE_INSTALL_PREFIX}/${dest}/${EXE_NAME}${EXE_DEBUG_EXTENSION}${_extension}")  # paths to executables
            INSTALL(CODE "
               include(\"${CMAKE_ROOT}/Modules/BundleUtilities.cmake\")
               fixup_bundle(\"${APPS}\"   \"\"   \"${searchDirs}\")
               " COMPONENT ${comp})
            
        endif(APPLE)
    endif()
endmacro()

# --------------------------------------------------------------------
#
# --------------------------------------------------------------------
macro(cmp_ToolInstallationSupport EXE_NAME EXE_DEBUG_EXTENSION EXE_BINARY_DIR installFiles comp dest)

    SET_TARGET_PROPERTIES( ${EXE_NAME} 
        PROPERTIES
        DEBUG_OUTPUT_NAME ${EXE_NAME}${EXE_DEBUG_EXTENSION}
        RELEASE_OUTPUT_NAME ${EXE_NAME}
    )
    IF (${installFiles} EQUAL 1)
        INSTALL(TARGETS ${EXE_NAME} 
            COMPONENT ${comp}
            LIBRARY DESTINATION ${dest} 
            ARCHIVE DESTINATION lib
            RUNTIME DESTINATION ${dest}
            BUNDLE DESTINATION ./
        )   
    
        #   message(STATUS "Creating Install CMake file for tool application ${EXE_NAME}")
        if (APPLE)
            if(CMAKE_BUILD_TYPE MATCHES "Debug")
                MakeOSXTool( "${EXE_NAME}${EXE_DEBUG_EXTENSION}" 
                                    ${EXE_BINARY_DIR}
                                    ${CMP_OSX_TOOLS_SOURCE_DIR} )
            else (CMAKE_BUILD_TYPE MATCHES "Debug")
                MakeOSXTool(${EXE_NAME} 
                                 ${EXE_BINARY_DIR}
                                 ${CMP_OSX_TOOLS_SOURCE_DIR} )
            endif()
        endif(APPLE)
    endif()
endmacro()

# --------------------------------------------------------------------

macro(LibraryProperties targetName DEBUG_EXTENSION)
    if ( NOT BUILD_SHARED_LIBS AND MSVC)
      SET_TARGET_PROPERTIES( ${targetName} 
        PROPERTIES
        DEBUG_OUTPUT_NAME lib${targetName}
        RELEASE_OUTPUT_NAME lib${targetName}  )
    endif()
    
    #-- Set the Debug and Release names for the libraries
    SET_TARGET_PROPERTIES( ${targetName} 
        PROPERTIES
        DEBUG_POSTFIX ${DEBUG_EXTENSION} )
    
    IF (BUILD_SHARED_LIBS)
      if (APPLE)
          OPTION (CMP_BUILD_WITH_INSTALL_NAME "Build Libraries with the install_name set to the installation prefix. This is good if you are going to run from the installation location" OFF)
          IF(CMP_BUILD_WITH_INSTALL_NAME)
          
              SET_TARGET_PROPERTIES(${MXADATAMODEL_LIB_NAME}
                 PROPERTIES
                 LINK_FLAGS "-current_version ${${CMP_PROJECT_NAME}_VERSION} -compatibility_version ${${CMP_PROJECT_NAME}_VERSION}"
                 INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/lib"
                 BUILD_WITH_INSTALL_RPATH ${CMP_BUILD_WITH_INSTALL_NAME}
              )
         ENDIF(CMP_BUILD_WITH_INSTALL_NAME)
     endif(APPLE)
#     INSTALL(TARGETS ${targetName} 
#        COMPONENT Applications
#        RUNTIME DESTINATION ./
#        LIBRARY DESTINATION ./ 
#        ARCHIVE DESTINATION ./        
#        BUNDLE DESTINATION ./
#      )   
   ENDIF( BUILD_SHARED_LIBS)

endmacro(LibraryProperties DEBUG_EXTENSION)

# --------------------------------------------------------------------
macro(StaticLibraryProperties targetName )
    if (WIN32 AND NOT MINGW)
        SET (DEBUG_EXTENSION "_d")
    else()
        SET (DEBUG_EXTENSION "_debug")
    endif()
    
    
    IF (WIN32 AND NOT MINGW)
        SET(LIBRARY_RELEASE_NAME "lib${targetName}" CACHE INTERNAL "" FORCE)
        SET(LIBRARY_DEBUG_NAME "lib${targetName}${DEBUG_EXTENSION}" CACHE INTERNAL "" FORCE)
    ELSE (WIN32 AND NOT MINGW)
        SET(LIBRARY_RELEASE_NAME "${targetName}" CACHE INTERNAL "" FORCE)
        SET(LIBRARY_DEBUG_NAME "${targetName}${DEBUG_EXTENSION}" CACHE INTERNAL "" FORCE)
    ENDIF(WIN32 AND NOT MINGW)
    
    
    #-- Set the Debug and Release names for the libraries
    SET_TARGET_PROPERTIES( ${targetName} 
        PROPERTIES
        DEBUG_OUTPUT_NAME ${LIBRARY_DEBUG_NAME}
        RELEASE_OUTPUT_NAME ${LIBRARY_RELEASE_NAME}
    )

endmacro(StaticLibraryProperties)

# --------------------------------------------------------------------
#
# --------------------------------------------------------------------
macro(PluginProperties targetName DEBUG_EXTENSION projectVersion binaryDir)
    if ( NOT BUILD_SHARED_LIBS AND MSVC)
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
    
    IF (APPLE AND BUILD_SHARED_LIBS)
      SET_TARGET_PROPERTIES(${targetName}
         PROPERTIES
         LINK_FLAGS "-current_version ${projectVersion} -compatibility_version ${projectVersion}"
      )
    ENDIF (APPLE AND BUILD_SHARED_LIBS)
    
    # Add the plugin to our list of plugins that will need to be installed
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        file(APPEND ${binaryDir}/plugins.txt "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/lib${targetName}${DEBUG_EXTENSION}.plugin;")
    else()
        file(APPEND ${binaryDir}/plugins.txt "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/lib${targetName}.plugin;")
    endif()
    
    if (NOT APPLE)
        set (BUILD_TYPES "Debug;Release")
        foreach(btype ${BUILD_TYPES})
            INSTALL(TARGETS ${targetName}
                    DESTINATION ./plugins
                    CONFIGURATIONS ${btype}
                    COMPONENT Applications)
        endforeach()
    endif()
endmacro(PluginProperties DEBUG_EXTENSION)

#-------------------------------------------------------------------------------
# Finds plugins from the Qt installation. The pluginlist argument should be
# something like "qgif;qjpeg;qtiff"
#-------------------------------------------------------------------------------
macro (FindQt4Plugins pluginlist pluginfile libdirsearchfile plugintype)
  set (qt_plugin_list ${pluginlist})
  set (qt_plugin_types "Debug;Release")
  if (WIN32)
    set(qt_plugin_prefix "")
    set(qt_plugin_DEBUG_suffix "d4")
    set(qt_plugin_RELEASE_suffix "4")
  else ()
    set(qt_plugin_prefix "lib")
    set(qt_plugin_DEBUG_suffix "_debug")
    set(qt_plugin_RELEASE_suffix "")
  endif()

  #message(STATUS "qt_plugin_debug_suffix: ${qt_plugin_debug_suffix}")
  set (QTPLUGINS_RELEASE "")
  set (QTPLUGINS_DEBUG   "")
  set (QTPLUGINS "")

  # Loop through all the Build Types and all the plugins to find each one.
  foreach(build_type ${qt_plugin_types})
    string(TOUPPER ${build_type} BTYPE)
      foreach(plugin ${qt_plugin_list})
        STRING(TOUPPER ${plugin} PLUGIN)
        # message(STATUS "|-- Looking for ${plugin}${qt_plugin_${BTYPE}_suffix}")
        FIND_LIBRARY( QT_IMAGEFORMAT_PLUGIN_${PLUGIN}_${BTYPE} 
                      NAMES ${plugin}${qt_plugin_${BTYPE}_suffix} 
                      PATHS ${QT_PLUGINS_DIR}/${plugintype} 
                      DOC "Library Path for ${plugin}"
                      NO_DEFAULT_PATH NO_CMAKE_PATH NO_SYSTEM_ENVIRONMENT_PATH NO_CMAKE_SYSTEM_PATH)
                      
        if (MSVC)
            #  message(STATUS "QT_PLUGINS_DIR: ${QT_PLUGINS_DIR}")
            #  message(STATUS " QT_IMAGEFORMAT_PLUGIN_${PLUGIN}_${BTYPE}: ${QT_IMAGEFORMAT_PLUGIN_${PLUGIN}_${BTYPE}}")
            get_filename_component(lib_path ${QT_IMAGEFORMAT_PLUGIN_${PLUGIN}_${BTYPE}} PATH)
            get_filename_component(lib_name ${QT_IMAGEFORMAT_PLUGIN_${PLUGIN}_${BTYPE}} NAME_WE)
            #  message(STATUS "lib_path: ${lib_path}")
            #  message(STATUS "lib_name: ${lib_name}")
            set (QT_IMAGEFORMAT_PLUGIN_${PLUGIN}_${BTYPE} ${lib_path}/${lib_name}.dll CACHE PATH "" FORCE)
            if ( ${QT_IMAGEFORMAT_PLUGIN_${PLUGIN}_${BTYPE}} STREQUAL  "QT_IMAGEFORMAT_PLUGIN_${PLUGIN}_${BTYPE}-NOTFOUND")
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
                    COMPONENT Runtime)
            endif()
        elseif (UNIX AND NOT APPLE)
            INSTALL(FILES ${QT_IMAGEFORMAT_PLUGIN_${PLUGIN}_${BTYPE}}
                DESTINATION ./plugins/${plugintype} 
                CONFIGURATIONS ${BTYPE} 
                COMPONENT Runtime)

        endif()             
                      
                      
                      
        mark_as_advanced(QT_IMAGEFORMAT_PLUGIN_${PLUGIN}_${BTYPE})
        # message(STATUS "|--  QT_IMAGEFORMAT_PLUGIN_${PLUGIN}_${BTYPE}: ${QT_IMAGEFORMAT_PLUGIN_${PLUGIN}_${BTYPE}}")
        LIST(APPEND QTPLUGINS_${BTYPE} ${QT_IMAGEFORMAT_PLUGIN_${PLUGIN}_${BTYPE}})
      endforeach()
    endforeach()

    # Assign either the debug or release plugin list to the QTPLUGINS variable on NON msvc platforms.
    if (NOT MSVC)
      if ( NOT DEFINED CMAKE_BUILD_TYPE )
        if ( ${CMAKE_BUILD_TYPE} STREQUAL "Debug")
            set (QTPLUGINS ${QTPLUGINS_DEBUG})
        else()
            set (QTPLUGINS ${QTPLUGINS_RELEASE})
        endif()
      else()
        set (QTPLUGINS ${QTPLUGINS_RELEASE})
      endif()
    else()
        # Create the qt.conf file so that the image plugins will be loaded correctly
        FILE(WRITE ${PROJECT_BINARY_DIR}/qt.conf "[Paths]\nPlugins = plugins")
        INSTALL(FILES ${PROJECT_BINARY_DIR}/qt.conf
                DESTINATION .
                COMPONENT Runtime)
    endif()
    file(APPEND ${pluginfile} "${QTPLUGINS};")
    file(APPEND ${libdirsearchfile} "${QT_PLUGINS_DIR}/imageformats;")
    
endmacro(FindQt4Plugins pluginlist)

# --------------------------------------------------------------------
#-- Copy all the Qt4 dependent DLLs into the current build directory so that
#-- one can debug an application or library that depends on Qt4 libraries.
macro (CMP_COPY_QT4_RUNTIME_LIBRARIES QTLIBLIST)
    #message(STATUS "CMP_COPY_QT4_RUNTIME_LIBRARIES")
    if (MSVC)
        if (DEFINED QT_QMAKE_EXECUTABLE)
            set(TYPE "d")
            FOREACH(qtlib ${QTLIBLIST})
                GET_FILENAME_COMPONENT(QT_DLL_PATH_tmp ${QT_QMAKE_EXECUTABLE} PATH)
                add_custom_target(ZZ_${qtlib}-Debug-Copy ALL
                            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QT_DLL_PATH_tmp}/${qtlib}${TYPE}4.dll
                            ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug/ 
                            COMMENT "Copying ${qtlib}${TYPE}4.dll to ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug/")
                add_custom_target(ZZ_${qtlib}-Release-Copy ALL
                            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QT_DLL_PATH_tmp}/${qtlib}4.dll
                            ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release/ 
                            COMMENT "Copying ${qtlib}4.dll to ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release/")
          
            ENDFOREACH(qtlib)
        endif(DEFINED QT_QMAKE_EXECUTABLE)
    endif()
endmacro()

# --------------------------------------------------------------------
#
#
macro (CMP_QT_LIBRARIES_INSTALL_RULES QTLIBLIST destination)
   # message(STATUS "CMP_COPY_QT4_RUNTIME_LIBRARIES")
    if (MSVC)
        if (DEFINED QT_QMAKE_EXECUTABLE)
            set(TYPE "d")
            FOREACH(qtlib ${QTLIBLIST})
                
                GET_FILENAME_COMPONENT(QT_DLL_PATH_tmp ${QT_QMAKE_EXECUTABLE} PATH)
                INSTALL(FILES ${QT_DLL_PATH_tmp}/${qtlib}${type}d4.dll 
                    DESTINATION "${destination}"
                    CONFIGURATIONS Debug
                    COMPONENT Applications)
                INSTALL(FILES ${QT_DLL_PATH_tmp}/${qtlib}4.dll 
                    DESTINATION "${destination}"
                    CONFIGURATIONS Release
                    COMPONENT Applications)   
                message(STATUS "Generating Install Rule for DLL Library ${QT_DLL_PATH_tmp}/${qtlib}4.dll")
                message(STATUS "Generating Install Rule for DLL Library ${QT_DLL_PATH_tmp}/${qtlib}d4.dll")       
            ENDFOREACH(qtlib)
        endif(DEFINED QT_QMAKE_EXECUTABLE)
    endif()
endmacro()

# --------------------------------------------------------------------
#-- Copy all the dependent DLLs into the current build directory so that the test
#-- can run.
MACRO (CMP_COPY_DEPENDENT_LIBRARIES _libraryList)
 # message(STATUS "#--------------------------------------------")
 # message(STATUS "CMP_COPY_DEPENDENT_LIBRARIES: ${_libraryList}")
  set (_libraryList ${_libraryList})
  SET (TYPES Debug Release)
  if (MSVC)
    # Make all the necessary intermediate directories for Visual Studio
    file(MAKE_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug)
    file(MAKE_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release)
    file(MAKE_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/MinSizeRel)
    file(MAKE_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/RelWithDebInfo)
    FOREACH(lib ${_libraryList})
    
      STRING(TOUPPER ${lib} upperlib)
    #  message(STATUS "upperlib: ${upperlib}")
    #  message(STATUS "${upperlib}_IS_SHARED: ${${upperlib}_IS_SHARED}")
      if (${upperlib}_IS_SHARED)
        FOREACH(BTYPE ${TYPES} )
      #    message(STATUS "Looking for ${BTYPE} DLL Version of ${lib}")
          STRING(TOUPPER ${BTYPE} TYPE)        
          get_filename_component(lib_path ${${upperlib}_LIBRARY_${TYPE}} PATH)
          get_filename_component(lib_name ${${upperlib}_LIBRARY_${TYPE}} NAME_WE)
       #   message(STATUS "lib_path: ${lib_path}")
       #   message(STATUS "lib_name: ${lib_name}")
          
          find_file(${upperlib}_LIBRARY_DLL_${TYPE}
                        NAMES ${lib_name}.dll
                        PATHS  ${lib_path}/../bin ${lib_path}/.. ${lib_path}/
                        NO_DEFAULT_PATH )
      #    message(STATUS "${upperlib}_LIBRARY_DLL_${TYPE}: ${${upperlib}_LIBRARY_DLL_${TYPE}}")
          mark_as_advanced(${upperlib}_LIBRARY_DLL_${TYPE})
          if ( ${${upperlib}_LIBRARY_DLL_${TYPE}} STREQUAL  "${upperlib}_LIBRARY_DLL_${TYPE}-NOTFOUND")
            message(FATAL_ERROR "According to how ${upperlib}_LIBRARY_${TYPE} was found the library should"
                                " have been built as a DLL but no .dll file can be found. I looked in the "
                                " following locations:  ${lib_path}\n  ${lib_path}/..\n  ${lib_path}/../bin")
          endif()

         # SET(${upperlib}_LIBRARY_DLL_${TYPE} "${${upperlib}_LIBRARY_DLL_${TYPE}}/${lib_name}.dll" CACHE FILEPATH "The path to the DLL Portion of the library" FORCE)
         # message(STATUS "${upperlib}_LIBRARY_DLL_${TYPE}: ${${upperlib}_LIBRARY_DLL_${TYPE}}")
          message(STATUS "Generating Copy Rule for DLL Library ${${upperlib}_LIBRARY_DLL_${TYPE}}")
          ADD_CUSTOM_TARGET(ZZ_${upperlib}_DLL_${TYPE}-Copy ALL 
                      COMMAND ${CMAKE_COMMAND} -E copy_if_different ${${upperlib}_LIBRARY_DLL_${TYPE}}
                      ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${BTYPE}/ 
                      COMMENT "  Copy: ${${upperlib}_LIBRARY_DLL_${TYPE}}\n    To: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${BTYPE}/")

        ENDFOREACH(BTYPE ${TYPES})
      ENDIF(${upperlib}_IS_SHARED)
    ENDFOREACH(lib ${_libraryList})
  ENDIF(MSVC)  
endmacro()

# --------------------------------------------------------------------
# This macro generates install rules for Visual Studio builds so that
# dependent DLL libraries (HDF5, Tiff, Expat, MXADataModel) will be
# properly installed with your project.
# --------------------------------------------------------------------
MACRO (CMP_LIBRARIES_INSTALL_RULES _libraryList destination)
  #  message(STATUS "CMP_LIBRARIES_INSTALL_RULES")
  set (_libraryList ${_libraryList})
  SET (TYPES Debug Release)
  if (MSVC)
   
    FOREACH(lib ${_libraryList})
        STRING(TOUPPER ${lib} upperlib)
      
        FOREACH(BTYPE ${TYPES} )
          STRING(TOUPPER ${BTYPE} TYPE)        
          get_filename_component(lib_path ${${upperlib}_LIBRARY_${TYPE}} PATH)
          get_filename_component(lib_name ${${upperlib}_LIBRARY_${TYPE}} NAME_WE)
          
          find_file(${upperlib}_LIBRARY_DLL_${TYPE}
                        NAMES ${lib_name}.dll
                        PATHS  ${lib_path}/../bin ${lib_path}/.. ${lib_path}/
                        NO_DEFAULT_PATH )
         # message(STATUS "${upperlib}_LIBRARY_DLL_${TYPE}: ${${upperlib}_LIBRARY_DLL_${TYPE}}")
          mark_as_advanced(${upperlib}_LIBRARY_DLL_${TYPE})
          if ( ${${upperlib}_LIBRARY_DLL_${TYPE}} STREQUAL  "${upperlib}_LIBRARY_DLL_${TYPE}-NOTFOUND")
             message(STATUS "A Companion DLL for ${upperlib}_LIBRARY_${TYPE} was NOT found which usually means"
                                " that the library was NOT built as a DLL. I looked in the "
                                " following locations:  ${lib_path}\n  ${lib_path}/..\n  ${lib_path}/../bin")
          else()
             # set(${upperlib}_LIBRARY_DLL_${TYPE}  ${${upperlib}_LIBRARY_DLL_${TYPE}}/${lib_name}.dll)
           #   message(STATUS "${upperlib}_LIBRARY_DLL_${TYPE}: ${${upperlib}_LIBRARY_DLL_${TYPE}}")
              message(STATUS "Generating Install Rule for DLL Library ${${upperlib}_LIBRARY_DLL_${TYPE}}")
              INSTALL(FILES ${${upperlib}_LIBRARY_DLL_${TYPE}}
                DESTINATION ${destination} 
                CONFIGURATIONS ${BTYPE} 
                COMPONENT Applications)
          endif()
        
        ENDFOREACH(BTYPE ${TYPES})
    ENDFOREACH(lib ${_libraryList})
  ENDIF(MSVC) 
ENDMACRO()

#-------------------------------------------------------------------------------
# This macro will attempt a try_run command in order to compile and then 
# generate a version string based on today's date. The output string should be
# of the form YYYY.MM.DD. 
#  Required CMake variables to be set are:
#   EmInit_CMAKE_DIR - The path to the MXA CMake directory
#  The following variables are set, all of which should have been already
#  initialized to a default value
#   ${CMP_PROJECT_NAME}_VERSION
#   ${CMP_PROJECT_NAME}_VER_MAJOR
#   ${CMP_PROJECT_NAME}_VER_MINOR
#   ${CMP_PROJECT_NAME}_VER_PATCH
#
#-------------------------------------------------------------------------------
macro(cmpGenerateVersionString GENERATED_FILE_PATH NAMESPACE cmpProjectName)
    INCLUDE (${CMAKE_ROOT}/Modules/CheckSymbolExists.cmake)
    
    if ( CMP_HAVE_TIME_GETTIMEOFDAY )
      set ( VERSION_COMPILE_FLAGS "-DHAVE_TIME_GETTIMEOFDAY")
    endif()
    
    if ( CMP_HAVE_SYS_TIME_GETTIMEOFDAY )
        set ( VERSION_COMPILE_FLAGS "-DHAVE_SYS_TIME_GETTIMEOFDAY")
    endif()
    
    if (NOT CMP_HAVE_TIME_GETTIMEOFDAY AND NOT CMP_HAVE_SYS_TIME_GETTIMEOFDAY)
      set (VERSION_GEN_VER_MAJOR "0")
      set (VERSION_GEN_VER_MINOR "0")
      set (VERSION_GEN_VER_PATCH "1")
      set (VERSION_GEN_COMPLETE "0.0.1" )
      set (VERSION_GEN_NAME "${CMP_PROJECT_NAME}")
      set (VERSION_GEN_NAMESPACE "${NAMESPACE}")
      set (${cmpProjectName}_VERSION   ${VERSION_GEN_COMPLETE}  CACHE STRING "Complete Version String")
      set (${cmpProjectName}_VER_MAJOR ${VERSION_GEN_VER_MAJOR} CACHE STRING "Major Version String")
      set (${cmpProjectName}_VER_MINOR ${VERSION_GEN_VER_MINOR} CACHE STRING "Minor Version String")
      set (${cmpProjectName}_VER_PATCH ${VERSION_GEN_VER_PATCH} CACHE STRING "Patch Version String")
    else()
      # message(STATUS "Running version generation executable")
      try_run(VERSION_RUN_RESULT VERSION_COMPILE_RESULT 
              ${CMAKE_CURRENT_BINARY_DIR} ${CMP_CORE_TESTS_SOURCE_DIR}/cmpGenerateVersionString.cpp
              COMPILE_DEFINITIONS ${VERSION_COMPILE_FLAGS}
              COMPILE_OUTPUT_VARIABLE VERSION_COMPILE_OUTPUT
              RUN_OUTPUT_VARIABLE VERSION_RUN_OUTPUT )
      set(VERSION_RUN_RESULT "1" CACHE INTERNAL "")
  
                 
      if (NOT VERSION_RUN_OUTPUT) 
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
    
      set (VERSION_GEN_COMPLETE ${VERSION_RUN_OUTPUT} )
      set (VERSION_GEN_NAME "${cmpProjectName}")
      set (VERSION_GEN_NAMESPACE "${NAMESPACE}")
      set (${cmpProjectName}_VERSION   ${VERSION_RUN_OUTPUT}    CACHE STRING "Complete Version String")
      set (${cmpProjectName}_VER_MAJOR ${VERSION_GEN_VER_MAJOR} CACHE STRING "Major Version String")
      set (${cmpProjectName}_VER_MINOR ${VERSION_GEN_VER_MINOR} CACHE STRING "Minor Version String")
      set (${cmpProjectName}_VER_PATCH ${VERSION_GEN_VER_PATCH} CACHE STRING "Patch Version String")
      mark_as_advanced(${cmpProjectName}_VERSION ${cmpProjectName}_VER_MAJOR ${cmpProjectName}_VER_MINOR ${cmpProjectName}_VER_PATCH)

    endif()
    set (PROJECT_PREFIX "${cmpProjectName}")
    configure_file(${CMP_CONFIGURED_FILES_SOURCE_DIR}/cmpVersion.h.in   ${GENERATED_FILE_PATH}  )
    MARK_AS_ADVANCED(${CMP_PROJECT_NAME}_VERSION ${CMP_PROJECT_NAME}_VER_MAJOR ${CMP_PROJECT_NAME}_VER_MINOR ${CMP_PROJECT_NAME}_VER_PATCH)
endmacro()


