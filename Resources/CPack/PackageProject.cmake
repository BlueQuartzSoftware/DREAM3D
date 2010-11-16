#///////////////////////////////////////////////////////////////////////////////
#//
#//  Copyright (c) 2009, Michael A. Jackson. BlueQuartz Software
#//  All rights reserved.
#//  BSD License: http://www.opensource.org/licenses/bsd-license.html
#//
#///////////////////////////////////////////////////////////////////////////////


# ------------------------------------------------------------------------------ 
# This CMake code sets up for CPack to be used to generate native installers
# ------------------------------------------------------------------------------
INCLUDE (${CMP_INSTALLATION_SUPPORT_SOURCE_DIR}/InstallMSVCLibraries.cmake)
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "GUI Application that runs various image processing algorithms through a plugin interface.")
SET(CPACK_PACKAGE_VENDOR "BlueQuartz Software, Michael A. Jackson")
SET(CPACK_PACKAGE_DESCRIPTION_FILE "${PROJECT_BINARY_DIR}/ReadMe.txt")
SET(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_BINARY_DIR}/License.txt")
SET(CPACK_PACKAGE_VERSION_MAJOR ${IPHelper_VER_MAJOR})
SET(CPACK_PACKAGE_VERSION_MINOR ${IPHelper_VER_MINOR})
SET(CPACK_PACKAGE_VERSION_PATCH ${IPHelper_VER_PATCH})
SET(CPACK_PACKAGE_VERSION ${IPHelper_VERSION})
SET(CPACK_COMPONENTS_ALL Applications)
set(CPACK_COMPONENT_APPLICATIONS_DISPLAY_NAME "Applications")
set(CPACK_COMPONENT_APPLICATIONS_DESCRIPTION  "The Gui Versions of the IPHelper Software Tools Suite")
set(CPACK_COMPONENT_APPLICATIONS_REQUIRED 1)
set(CPACK_PACKAGE_EXECUTABLES
    IPHelper IPHelper
)

IF (APPLE)
  set(CPACK_PACKAGE_FILE_NAME "${PROJECT_NAME}-OSX")
elseif(WIN32)
	if ( ${CMAKE_SIZEOF_VOID_P} EQUAL 8)
	
		set(CPACK_PACKAGE_FILE_NAME "${PROJECT_NAME}-Win64")
	else()
		set(CPACK_PACKAGE_FILE_NAME "${PROJECT_NAME}-Win32")
	endif()
else()
  set(CPACK_PACKAGE_FILE_NAME "${PROJECT_NAME}-${CMAKE_SYSTEM_NAME}")
endif()
# Create an NSID based installer for Windows Systems
IF(WIN32 AND NOT UNIX)
  # There is a bug in NSI that does not handle full unix paths properly. Make
  # sure there is at least one set of four (4) backlasshes.
# SET(CPACK_PACKAGE_ICON "${EmMpmProj_SOURCE_DIR}/Resources/InstallerIcon.bmp")
# SET(CPACK_NSIS_INSTALLED_ICON_NAME "bin\\\\MyExecutable.exe")
  SET(CPACK_NSIS_DISPLAY_NAME "IPHelper Software Tools")
  SET(CPACK_NSIS_HELP_LINK "http:\\\\\\\\www.bluequartz.net")
  SET(CPACK_NSIS_URL_INFO_ABOUT "http:\\\\\\\\www.bluequartz.net")
  SET(CPACK_NSIS_CONTACT "mike.jackson@bluequartz.net")
  SET(CPACK_NSIS_MODIFY_PATH ON)
  SET(CPACK_GENERATOR "ZIP")
  SET(CPACK_BINARY_ZIP "ON")
  SET(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "IPHelper Software Tools")
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
    SET(CPACK_BINARY_TGZ "OFF")
    SET(CPACK_BINARY_TZ "OFF")
    SET(CPACK_BINARY_ZIP "ON")
ENDIF(WIN32 AND NOT UNIX)

SET(CPACK_SOURCE_GENERATOR "ZIP")
SET(CPACK_SOURCE_PACKAGE_FILE_NAME "IPHelper-${IPHelper_VERSION}-Source")
SET(CPACK_SOURCE_TOPLEVEL_TAG "Source")
SET(CPACK_IGNORE_FILES "/i386/;/x64/;/VS2008/;/zRel/;/Build/;/\\\\.git/;\\\\.*project")
SET(CPACK_SOURCE_IGNORE_FILES "/i386/;/x64/;/VS2008/;/zRel/;/Build/;/\\\\.git/;\\\\.*project")



# THIS MUST BE THE LAST LINE OF THIS FILE BECAUSE ALL THE CPACK VARIABLES MUST BE
# DEFINED BEFORE CPack IS INCLUDED
INCLUDE(CPack)

