#--////////////////////////////////////////////////////////////////////////////
#-- Copyright (c) 2009, Michael A. Jackson. BlueQuartz Software
#-- All rights reserved.
#-- BSD License: http://www.opensource.org/licenses/bsd-license.html
#-- This code was partly written under US Air Force Contract FA8650-07-D-5800
#--////////////////////////////////////////////////////////////////////////////
# -----------------------------------------------------------------------------
#
# -----------------------------------------------------------------------------
PROJECT(DREAM3DAPIDocs)
cmake_minimum_required(VERSION 3.1.0)

if(NOT DEFINED PROJECT_RESOURCES_DIR)
set(PROJECT_RESOURCES_DIR ${PROJECT_SOURCE_DIR}/Resources)
endif()




#set(DOXY_OUTPUT_DIR ${DREAM3DProj_BINARY_DIR}/Documentation/API)
#configure_file(${PROJECT_RESOURCES_DIR}/DREAM3DLib_API.doxyfile.in
#               ${PROJECT_BINARY_DIR}/Doxyfile  @ONLY IMMEDIATE)
#add_custom_target(APIDocs COMMAND ${DOXYGEN_EXECUTABLE} ${PROJECT_BINARY_DIR}/Doxyfile)

#-------------------------------------------------------------------------------
# Create the initial list of Files for DOxygen to process
set(DOCUMENTATION_SOURCE_FILES
  ${DREAM3DProj_SOURCE_DIR}/Source
)

include(${DOCUMENTATION_RESOURCES_DIR}/CMake/Macros.cmake)

#---- Set some LATEX Varaibles
set(LATEX_TITLE "DREAM3D\\\\[0.45cm] Developers API Documentation")
set(LATEX_AUTHOR "DREAM3D Development Team")


set(LATEX_EPS_FILES )


set(DOXYGEN_IMAGE_DIRS )

if(DOX_GENERATE_LATEX)
  ADD_DOXYGEN_TARGETS(PDF_FILENAME "DREAM3D Developers API Documentation"
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
    ${DOCUMENTATION_SOURCE_FILES}
)
# Include the SIMPLib doc files
if(NOT ${SIMPLProj_SOURCE_DIR} STREQUAL "")
    set(DOCUMENTATION_SOURCE_FILES ${DOCUMENTATION_SOURCE_FILES}
        ${SIMPLProj_SOURCE_DIR}/Source
    )
endif()

#include the SIMPLView doc files
if(NOT ${SIMPLViewProj_SOURCE_DIR} STREQUAL "")
    set(DOCUMENTATION_SOURCE_FILES ${DOCUMENTATION_SOURCE_FILES}
        ${SIMPLViewProj_SOURCE_DIR}/Source
    )
endif()


if(DOX_GENERATE_HTML)
  ADD_DOXYGEN_TARGETS(PDF_FILENAME "DREAM3D Developers API Documentation"
                     PROJECT_NAME  "${PROJECT_NAME}_HTML"
                     PROJECT_BINARY_DIR "${PROJECT_BINARY_DIR}_HTML"
                     SOURCE_FILES "${DOCUMENTATION_SOURCE_FILES}"
                     EPS_FILES "${LATEX_EPS_FILES}"
                     TITLE "${LATEX_TITLE}"
                     AUTHOR "${LATEX_AUTHOR}"
                     GENERATE_HTML "YES"
                     GENERATE_LATEX "NO"
                     GENERATE_RTF "NO"
                     INCLUDE_MARKDOWN_FILES "NO"
                     TEMPLATE_DIR "${DOCUMENTATION_RESOURCES_DIR}"
                     IMAGE_DIR "${DOXYGEN_IMAGE_DIRS}"
                     OUTPUT_DIR "${PROJECT_BINARY_DIR}_HTML"
                     OUTPUT_SUB_DIR "html")
endif()


