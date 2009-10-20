#///////////////////////////////////////////////////////////////////////////////
#//
#//  Copyright (c) 2008, Michael A. Jackson. BlueQuartz Software
#//  All rights reserved.
#//  BSD License: http://www.opensource.org/licenses/bsd-license.html
#//
#///////////////////////////////////////////////////////////////////////////////

# Copy the needed Qt libraries into the Build directory. Also add installation
# and CPack code to support installer generation.
# this is a complete hack for Visual Studio to copy the Qt libraries. This file
# needs to be changed, the 'TYPE' variable needs to be set for either Debug or
# Release builds. What a pain.
if (AIM_BUILD_QT_APPS AND NOT Q_WS_MAC)
    if (DEFINED QT_QMAKE_EXECUTABLE)
        SET (QTLIBLIST QtCore QtGui)
        
        IF (MSVC)
            set(TYPE "d")
            FOREACH(qtlib ${QTLIBLIST})
              IF (WIN32)
                GET_FILENAME_COMPONENT(QT_DLL_PATH_tmp ${QT_QMAKE_EXECUTABLE} PATH)
                INSTALL(FILES ${QT_DLL_PATH_tmp}/${qtlib}${type}d4.dll 
                    DESTINATION ./
                    CONFIGURATIONS Debug
                    COMPONENT Applications)
                INSTALL(FILES ${QT_DLL_PATH_tmp}/${qtlib}4.dll 
                    DESTINATION ./
                    CONFIGURATIONS Release
                    COMPONENT Applications)
                add_custom_target(${qtlib}-Debug-Copy ALL
                            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QT_DLL_PATH_tmp}/${qtlib}${TYPE}4.dll
                            ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug/ 
                            COMMENT "Copying ${qtlib}${TYPE}4.dll to ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug/")
                add_custom_target(${qtlib}-Release-Copy ALL
                            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QT_DLL_PATH_tmp}/${qtlib}4.dll
                            ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release/ 
                            COMMENT "Copying ${qtlib}4.dll to ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release/")
              ENDIF (WIN32)
            ENDFOREACH(qtlib)
                
        endif()
    endif(DEFINED QT_QMAKE_EXECUTABLE)
endif()

# Install MinGW support libraries if those are needed
IF (MINGW)
    INCLUDE ( ${MXA_CMAKE_DIR}/MXAFindMinGW.cmake )
    IF (MINGW_FOUND)
        INSTALL(FILES ${MINGW_LIBRARIES}
                DESTINATION ./
                COMPONENT Applications)
        add_custom_target(MINGW-Copy  ALL
                        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${MINGW_LIBRARY}
                        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_CFG_INTDIR}/ 
                        COMMENT "Copying ${MINGW_LIBRARY} to ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_CFG_INTDIR}/")
    ENDIF (MINGW_FOUND)
    
   if (DEFINED QT_QMAKE_EXECUTABLE)
        SET (QTLIBLIST QtCore QtGui)
        set(TYPE "")
        IF (CMAKE_BUILD_TYPE STREQUAL "Debug")
            set(TYPE "d")
        endif()
        FOREACH(qtlib ${QTLIBLIST})
          IF (WIN32)
            GET_FILENAME_COMPONENT(QT_DLL_PATH_tmp ${QT_QMAKE_EXECUTABLE} PATH)
            INSTALL(FILES ${QT_DLL_PATH_tmp}/${qtlib}${type}d4.dll 
                DESTINATION ./
                CONFIGURATIONS Debug
                COMPONENT Applications)
            INSTALL(FILES ${QT_DLL_PATH_tmp}/${qtlib}4.dll 
                DESTINATION ./
                CONFIGURATIONS Release
                COMPONENT Applications)
            add_custom_target(${qtlib}-Copy ALL
                        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QT_DLL_PATH_tmp}/${qtlib}${TYPE}4.dll
                        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_CFG_INTDIR}/ 
                        COMMENT "Copying ${qtlib}${TYPE}4.dll to ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_CFG_INTDIR}/")

          ENDIF (WIN32)
        ENDFOREACH(qtlib)
    endif(DEFINED QT_QMAKE_EXECUTABLE)
    
ENDIF (MINGW)

#-- Copy all the MXADataModel dependent DLLs into the current build directory so 
#-- that the programs can run.
if (WIN32)
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(TYPE "DEBUG")
    else()
        set(TYPE "RELEASE")
    endif()
    
    SET (mxa_lib_list tiff expat hdf5 MXADataModel)
    FOREACH(lib ${mxa_lib_list})
      STRING(TOUPPER ${lib} upperlib)
      if (HAVE_${upperlib}_DLL)
        GET_FILENAME_COMPONENT(lib_path ${${upperlib}_LIBRARY_${TYPE}} PATH)
        get_filename_component(lib_name ${${upperlib}_LIBRARY_${TYPE}} NAME_WE)
        add_custom_target(${lib}-Copy  ALL
                      COMMAND ${CMAKE_COMMAND} -E copy_if_different ${${upperlib}_BIN_DIR}/${lib_name}.dll
                      ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_CFG_INTDIR}/ 
                      COMMENT "Copying ${lib_name}.dll to ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_CFG_INTDIR}/")
      endif()
    ENDFOREACH()
endif()





# Add any extra files/resources that need to be installed
SET (AIM_INSTALLED_RESOURCES
       
)
# Install the RoboMet MXADataModel XML file.
INSTALL (FILES ${AIM_INSTALLED_RESOURCES} 
        DESTINATION Resources
        COMPONENT Resources)
        
#-- Install the generated Doxygen documentation
if(AIM_BUILD_API_DOCS)
  INSTALL(DIRECTORY ${AIMRepresentation_BINARY_DIR}/Documentation/html
            DESTINATION Documentation
            COMPONENT Documentation )
endif()


macro(_FIND_MSVC_REDIST VCVERS)
  set(SDKVERS "2.0") 
  if(${VCVERS} EQUAL 8)
    set(SDKVERS "2.0")
  endif()
  if(${VCVERS} EQUAL 9)
    set(SDKVERS "3.5")
  endif()
  IF(MSVC${VCVERS}0)
    FIND_PROGRAM(MSVC_REDIST NAMES vcredist_${CMAKE_MSVC_ARCH}/vcredist_${CMAKE_MSVC_ARCH}.exe
      PATHS
      "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\VCExpress\\${VCVERS}.0;InstallDir]/../../SDK/v${SDKVERS}/BootStrapper/Packages/"
      "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\VisualStudio\\${VCVERS}.0;InstallDir]/../../SDK/v${SDKVERS}/BootStrapper/Packages/"
      "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Wow6432Node\\Microsoft\\VisualStudio\\${VCVERS}.0;InstallDir]/../../SDK/v${SDKVERS}/BootStrapper/Packages/"
      )
    if (MSVC_REDIST)
      GET_FILENAME_COMPONENT(vcredist_name "${MSVC_REDIST}" NAME)
      INSTALL(PROGRAMS ${MSVC_REDIST} COMPONENT SupportFiles DESTINATION bin)
      SET(CPACK_NSIS_EXTRA_INSTALL_COMMANDS "ExecWait '\\\"$INSTDIR\\\\bin\\\\${vcredist_name}\\\"'")
    endif()
    #message(STATUS "MSVC_REDIST: ${MSVC_REDIST}")
  ENDIF(MSVC${VCVERS}0)
endmacro()


# ------------------------------------------------------------------------------ 
# This CMake code sets up for CPack to be used to generate native installers
# ------------------------------------------------------------------------------
#INCLUDE (InstallRequiredSystemLibraries)
INCLUDE (${AIMRepresentation_RESOURCES_DIR}/CMake/InstallMSVCLibraries.cmake)
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "AIM Software Tools")
SET(CPACK_PACKAGE_VENDOR "BlueQuartz Software, Michael A. Jackson")
SET(CPACK_PACKAGE_DESCRIPTION_FILE "${AIMRepresentation_SOURCE_DIR}/ReadMe.txt")
SET(CPACK_RESOURCE_FILE_LICENSE "${AIMRepresentation_SOURCE_DIR}/License.txt")
SET(CPACK_PACKAGE_VERSION_MAJOR ${AIMRepresentation_VER_MAJOR})
SET(CPACK_PACKAGE_VERSION_MINOR ${AIMRepresentation_VER_MINOR})
SET(CPACK_PACKAGE_VERSION_PATCH ${AIMRepresentation_VER_PATCH})
SET(CPACK_PACKAGE_VERSION ${AIMRepresentation_VERSION})
SET(CPACK_COMPONENTS_ALL Applications Resources Unspecified SupportFiles Documentation)
set(CPACK_COMPONENT_APPLICATIONS_DISPLAY_NAME "Applications")
set(CPACK_COMPONENT_APPLICATIONS_DESCRIPTION  "The Gui Versions of the AIMRepresentation Software Tools Suite")
set(CPACK_COMPONENT_APPLICATIONS_REQUIRED 1)
set(CPACK_COMPONENT_SUPPORTFILES_DISPLAY_NAME "Tools")
set(CPACK_COMPONENT_SUPPORTFILES_DESCRIPTION  "The Command Line versions of the AIMRepresentation Software Tools Suite")
set(CPACK_COMPONENT_SUPPORTFILES_REQUIRED 1)
set(CPACK_COMPONENT_RESOURCES_DISPLAY_NAME "Resources")
set(CPACK_COMPONENT_RESOURCES_DESCRIPTION  "Contains resource files to aid in converting Zeiss Mosaic Directories into HDF5 Files.")
set(CPACK_COMPONENT_RESOURCES_REQUIRED 1)
set(CPACK_COMPONENT_DOCUMENTATION_DISPLAY_NAME "Documentation")
set(CPACK_COMPONENT_DOCUMENTATION_DESCRIPTION  "Contains the AIMRepresentation API Documentation and User manuals in HTML format.")
set(CPACK_PACKAGE_EXECUTABLES AIMFourFoldSymmetry AIMFourFoldSymmetry
                                FourFoldSymmetry FourFoldSymmetry
                                AIMMountMaker AIMMountMaker)
    
IF(WIN32 AND NOT UNIX)
  # There is a bug in NSI that does not handle full unix paths properly. Make
  # sure there is at least one set of four (4) backlasshes.
# SET(CPACK_PACKAGE_ICON "${AIMRepresentation_SOURCE_DIR}/Resources/InstallerIcon.bmp")
# SET(CPACK_NSIS_INSTALLED_ICON_NAME "bin\\\\MyExecutable.exe")
  SET(CPACK_NSIS_DISPLAY_NAME "AIM Software Tools")
  SET(CPACK_NSIS_HELP_LINK "http:\\\\\\\\www.bluequartz.net")
  SET(CPACK_NSIS_URL_INFO_ABOUT "http:\\\\\\\\www.bluequartz.net")
  SET(CPACK_NSIS_CONTACT "mike.jackson@bluequartz.net")
  SET(CPACK_NSIS_MODIFY_PATH ON)
  SET(CPACK_SOURCE_GENERATOR "ZIP")
  SET(CPACK_GENERATOR "NSIS")
  SET(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "AIM Software Tools")

if(MSVC80)
#_FIND_MSVC_REDIST(8)
endif()

if(MSVC90)
#_FIND_MSVC_REDIST(9)
endif()
  
ELSE(WIN32 AND NOT UNIX)
SET(CPACK_BINARY_BUNDLE "OFF")
SET(CPACK_BINARY_CYGWIN "OFF")
SET(CPACK_BINARY_DEB "OFF")
SET(CPACK_BINARY_DRAGNDROP "OFF")
SET(CPACK_BINARY_NSIS "OFF")
SET(CPACK_BINARY_OSXX11 "OFF")
SET(CPACK_BINARY_PACKAGEMAKER "OFF")
SET(CPACK_BINARY_RPM "OFF")
SET(CPACK_BINARY_STGZ "OFF")
SET(CPACK_BINARY_TBZ2 "OFF")
SET(CPACK_BINARY_TGZ "ON")
SET(CPACK_BINARY_TZ "OFF")
SET(CPACK_BINARY_ZIP "OFF")
ENDIF(WIN32 AND NOT UNIX)

SET(CPACK_SOURCE_GENERATOR "ZIP")
SET(CPACK_SOURCE_PACKAGE_FILE_NAME "AIMRepresentation-${AIMRepresentation_VERSION}-Source")
SET(CPACK_SOURCE_TOPLEVEL_TAG "Source")
SET(CPACK_IGNORE_FILES "/VS2008/;/zRel/;/Build/;/\\\\.git/;\\\\.*project")
SET(CPACK_SOURCE_IGNORE_FILES "/VS2008/;/zRel/;/Build/;/\\\\.git/;\\\\.*project")



# THIS MUST BE THE LAST LINE OF THIS FILE BECAUSE ALL THE CPACK VARIABLES MUST BE
# DEFINED BEFORE CPack IS INCLUDED
INCLUDE(CPack)

