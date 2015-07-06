#--////////////////////////////////////////////////////////////////////////////
#-- Copyright (c) 2009, Michael A. Jackson. BlueQuartz Software
#-- All rights reserved.
#-- BSD License: http://www.opensource.org/licenses/bsd-license.html
#-- This code was partly written under US Air Force Contract FA8650-07-D-5800
#--////////////////////////////////////////////////////////////////////////////
# -----------------------------------------------------------------------------
#
# -----------------------------------------------------------------------------
project (DeveloperManual)
cmake_minimum_required(VERSION 3.1.0)

if(NOT DEFINED PROJECT_RESOURCES_DIR)
set(PROJECT_RESOURCES_DIR ${PROJECT_SOURCE_DIR}/Resources)
endif()

set(DOCUMENTATION_RESOURCES_DIR ${PROJECT_RESOURCES_DIR}/DocConfigFiles)

FIND_PACKAGE(Doxygen)
if(NOT DOXYGEN_FOUND)
  message(FATAL_ERROR "Doxygen is needed to build the documentation but was not found on your system. Please install it correctly and rerun cmake.")
endif()

set(DOX_GENERATE_RTF "NO")
option(DREAM3D_GENERATE_RTF "Generate RTF Documentation" OFF)
if(DREAM3D_GENERATE_RTF STREQUAL "ON")
    set(DOX_GENERATE_RTF "YES")
endif()


set(DOX_GENERATE_HTML "YES")
option(DREAM3D_GENERATE_HTML "Generate HTML Documentation" ON)
if(DREAM3D_GENERATE_HTML STREQUAL "ON")
    set(DOX_GENERATE_HTML "YES")
endif()


set(DOX_GENERATE_LATEX "NO")
option(DREAM3D_GENERATE_LATEX "Generate HTML Documentation" OFF)
if(DREAM3D_GENERATE_LATEX STREQUAL "ON")
    set(DOX_GENERATE_LATEX "YES")
    # Check for existence of LaTex executable files
    INCLUDE( ${DOCUMENTATION_RESOURCES_DIR}/CMake/UseLATEX.cmake)
endif()

if(APPLE)
    option(AUTO_OPEN_PDF "Automatically open generated PDF files with default PDF reading application" ON)
endif()



include ( ${DOCUMENTATION_RESOURCES_DIR}/CMake/Macros.cmake)



set(LATEX_TITLE "DREAM.3D:\\\\Developer Documentation")
set(LATEX_AUTHOR "DREAM3D Development Team")

# The order of the files as they appear here is the order they will appear in the
# Latex/PDF output
set(DOCUMENTATION_SOURCE_FILES
  ${DREAM3DProj_SOURCE_DIR}/ToDo.md
  ${PROJECT_SOURCE_DIR}/Compiling.md
  ${PROJECT_SOURCE_DIR}/AddingFilters.md
  ${PROJECT_SOURCE_DIR}/AddingPrebuiltPipelines.md
  ${PROJECT_SOURCE_DIR}/H5EbsdFile.md
  ${PROJECT_SOURCE_DIR}/DataContainerStrategies.md
  ${PROJECT_SOURCE_DIR}/WritingPlugin.md
  ${PROJECT_SOURCE_DIR}/CodeGuidelines.md
  ${PROJECT_SOURCE_DIR}/DevelopmentNotes.md
)

set(LATEX_EPS_FILES

)

if(DOX_GENERATE_LATEX)
ADD_DOXYGEN_TARGETS (PDF_FILENAME "DREAM.3D_Developer_Reference"
                     PROJECT_NAME  "${PROJECT_NAME}_LATEX"
                     PROJECT_BINARY_DIR "${PROJECT_BINARY_DIR}"
                     SOURCE_FILES "${DOCUMENTATION_SOURCE_FILES}"
                     EPS_FILES "${LATEX_EPS_FILES}"
                     TITLE "${LATEX_TITLE}"
                     AUTHOR "${LATEX_AUTHOR}"
                     GENERATE_HTML "NO"
                     GENERATE_LATEX "YES"
                     GENERATE_RTF "NO"
                     TEMPLATE_DIR "${DOCUMENTATION_RESOURCES_DIR}"
                     IMAGE_DIR "${PROJECT_SOURCE_DIR}/Images"
                     OUTPUT_DIR "${PROJECT_BINARY_DIR}")
endif()

set(DOCUMENTATION_SOURCE_FILES
        ${PROJECT_SOURCE_DIR}/index.dox
        ${DOCUMENTATION_SOURCE_FILES})


if(DOX_GENERATE_HTML)
ADD_DOXYGEN_TARGETS (PDF_FILENAME ${PROJECT_NAME}
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
                     IMAGE_DIR "${PROJECT_SOURCE_DIR}/Images"
                     OUTPUT_DIR "${PROJECT_BINARY_DIR}/Developer_Reference")
endif()

if(DOX_GENERATE_RTF)
ADD_DOXYGEN_TARGETS (PDF_FILENAME ${PROJECT_NAME}
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
                     IMAGE_DIR "${PROJECT_SOURCE_DIR}/Images"
                     OUTPUT_DIR "${PROJECT_BINARY_DIR}/Developer_Reference")
endif()

