#///////////////////////////////////////////////////////////////////////////////
#
#  Copyright (c) 2011, Michael A. Jackson. BlueQuartz Software
#  All rights reserved.
#  BSD License: http://www.opensource.org/licenses/bsd-license.html
#
#///////////////////////////////////////////////////////////////////////////////
project(emmpm)

set(emmpm_SRCS
    ${emmpm_SOURCE_DIR}/main.cpp
)

set(emmpm_HDRS
)

set(emmpm_PROJECT_SRCS
  ${emmpm_SRCS} ${emmpm_HDRS}
)
cmp_IDE_SOURCE_PROPERTIES("emmpm/apps" "${emmpm_HDRS}" "${emmpm_SRCS}"  "${PROJECT_INSTALL_HEADERS}")

# --------------------------------------------------------------------
# Setup the install rules for the various platforms
set(install_dir "tools")
if (WIN32)
    set (install_dir ".")
endif()

BuildToolBundle(
    TARGET ${PROJECT_NAME}
    SOURCES ${${PROJECT_NAME}_PROJECT_SRCS}
    DEBUG_EXTENSION ${EXE_DEBUG_EXTENSION}
    VERSION_MAJOR ${EMMPM_VER_MAJOR}
    VERSION_MINOR ${EMMPM_VER_MINOR}
    VERSION_PATCH ${EMMPM_VER_PATCH}
    BINARY_DIR    ${${PROJECT_NAME}_BINARY_DIR}
    LINK_LIBRARIES EMMPMLib
    LIB_SEARCH_DIRS ${CMAKE_LIBRARY_OUTPUT_DIRECTORY} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    COMPONENT     Applications
    INSTALL_DEST  "${install_dir}"
)
