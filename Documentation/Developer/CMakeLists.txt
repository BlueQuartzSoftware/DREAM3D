#--////////////////////////////////////////////////////////////////////////////
#-- Copyright (c) 2009, Michael A. Jackson. BlueQuartz Software
#-- All rights reserved.
#-- BSD License: http://www.opensource.org/licenses/bsd-license.html
#-- This code was partly written under US Air Force Contract FA8650-07-D-5800
#--////////////////////////////////////////////////////////////////////////////
# -----------------------------------------------------------------------------
#
# -----------------------------------------------------------------------------
project(DeveloperManual)
cmake_minimum_required(VERSION 2.8.9)

if(NOT DEFINED PROJECT_RESOURCES_DIR)
  set(PROJECT_RESOURCES_DIR ${PROJECT_SOURCE_DIR}/Resources)
endif()

set(DOCUMENTATION_RESOURCES_DIR ${PROJECT_RESOURCES_DIR}/DocConfigFiles)


include(${DOCUMENTATION_RESOURCES_DIR}/CMake/Macros.cmake)


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

