#--////////////////////////////////////////////////////////////////////////////
#-- Copyright (c) 2009, Michael A. Jackson. BlueQuartz Software
#-- All rights reserved.
#-- BSD License: http://www.opensource.org/licenses/bsd-license.html
#-- This code was partly written under US Air Force Contract FA8650-07-D-5800
#--////////////////////////////////////////////////////////////////////////////
# -----------------------------------------------------------------------------
#
# -----------------------------------------------------------------------------
project (DREAM3DDocumentation)
cmake_minimum_required(VERSION 2.8)



if (NOT DEFINED PROJECT_RESOURCES_DIR)
set (PROJECT_RESOURCES_DIR ${PROJECT_SOURCE_DIR}/Resources)
endif()

set(DOCUMENTATION_RESOURCES_DIR ${PROJECT_RESOURCES_DIR}/Documentation)

FIND_PACKAGE(Doxygen)
if (NOT DOXYGEN_FOUND)
  message(FATAL_ERROR "Doxygen is needed to build the documentation but was not found on your system. Please install it correctly and rerun cmake.")
endif()

SET (DOX_GENERATE_RTF "NO")
option(EIM_GENERATE_RTF "Generate RTF Documentation" ON)
if (EIM_GENERATE_RTF STREQUAL "ON")
    SET (DOX_GENERATE_RTF "YES")
endif()
  

SET (DOX_GENERATE_HTML "NO")
option(EIM_GENERATE_HTML "Generate HTML Documentation" ON)
if (EIM_GENERATE_HTML STREQUAL "ON")
    SET (DOX_GENERATE_HTML "YES")
endif()


SET (DOX_GENERATE_LATEX "NO")
option(EIM_GENERATE_LATEX "Generate HTML Documentation" ON)
if (EIM_GENERATE_LATEX STREQUAL "ON")
    SET (DOX_GENERATE_LATEX "YES")
    # Check for existence of LaTex executable files
    INCLUDE( ${DOCUMENTATION_RESOURCES_DIR}/CMake/UseLATEX.cmake)
endif()

if (APPLE)
    option(AUTO_OPEN_PDF "Automatically open generated PDF files with default PDF reading application" ON)
endif()



include ( ${DOCUMENTATION_RESOURCES_DIR}/CMake/Macros.cmake)

set (LATEX_TITLE "DREAM.3D:\\\\Documentation")
set (LATEX_AUTHOR "Many and \\\\ Varied")

# The order of the files as they appear here is the order they will appear in the
# Latex/PDF output
set (DOCUMENTATION_SOURCE_FILES
    ${PROJECT_SOURCE_DIR}/AIMRep.dox
    ${PROJECT_SOURCE_DIR}/AngFileImport.dox
    ${PROJECT_SOURCE_DIR}/Reconstruction.dox
    ${PROJECT_SOURCE_DIR}/SyntheticBuilder.dox
    ${PROJECT_SOURCE_DIR}/SurfaceMeshing.dox
    ${PROJECT_SOURCE_DIR}/StatsGenerator.dox
    ${PROJECT_SOURCE_DIR}/MicrostructurePresets.dox
    ${PROJECT_SOURCE_DIR}/StructureReaders.dox
)

set (LATEX_EPS_FILES 
 ${PROJECT_SOURCE_DIR}/Images/imagequality.png
 ${PROJECT_SOURCE_DIR}/Images/minimagequality.png
 ${PROJECT_SOURCE_DIR}/Images/oimFileImport.png
 ${PROJECT_SOURCE_DIR}/Images/reconstruction_gui.png
 ${PROJECT_SOURCE_DIR}/Images/StatsGenerator_1.png
 ${PROJECT_SOURCE_DIR}/Images/StatsGenerator_10.png
 ${PROJECT_SOURCE_DIR}/Images/StatsGenerator_2.png
 ${PROJECT_SOURCE_DIR}/Images/StatsGenerator_3.png
 ${PROJECT_SOURCE_DIR}/Images/StatsGenerator_4.png
 ${PROJECT_SOURCE_DIR}/Images/StatsGenerator_6.png
 ${PROJECT_SOURCE_DIR}/Images/StatsGenerator_9.png
)

if (DOX_GENERATE_LATEX)
ADD_DOXYGEN_TARGETS (PDF_FILENAME ${PROJECT_NAME}
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
                     IMAGE_DIR "${PROJECT_SOURCE_DIR}/Images")
endif()

set (DOCUMENTATION_SOURCE_FILES
        ${PROJECT_SOURCE_DIR}/index.dox
        ${DOCUMENTATION_SOURCE_FILES})


if (DOX_GENERATE_HTML)
ADD_DOXYGEN_TARGETS (PDF_FILENAME ${PROJECT_NAME}
                     PROJECT_NAME  "${PROJECT_NAME}_HTML" 
                     PROJECT_BINARY_DIR "${PROJECT_BINARY_DIR}" 
                     SOURCE_FILES "${DOCUMENTATION_SOURCE_FILES}" 
                     EPS_FILES "${LATEX_EPS_FILES}"
                     TITLE "${LATEX_TITLE}"
                     AUTHOR "${LATEX_AUTHOR}"
                     GENERATE_HTML "${DOX_GENERATE_LATEX}"
                     GENERATE_LATEX "NO"
                     GENERATE_RTF "NO"
                     TEMPLATE_DIR "${DOCUMENTATION_RESOURCES_DIR}"
                     IMAGE_DIR "${PROJECT_SOURCE_DIR}/Images")
endif()

if (DOX_GENERATE_RTF)
ADD_DOXYGEN_TARGETS (PDF_FILENAME ${PROJECT_NAME}
                     PROJECT_NAME  "${PROJECT_NAME}_RTF" 
                     PROJECT_BINARY_DIR "${PROJECT_BINARY_DIR}" 
                     SOURCE_FILES "${DOCUMENTATION_SOURCE_FILES}" 
                     EPS_FILES "${LATEX_EPS_FILES}"
                     TITLE "${LATEX_TITLE}"
                     AUTHOR "${LATEX_AUTHOR}"
                     GENERATE_HTML "NO"
                     GENERATE_LATEX "NO"
                     GENERATE_RTF "${DOX_GENERATE_RTF}"
                     TEMPLATE_DIR "${DOCUMENTATION_RESOURCES_DIR}"
                     IMAGE_DIR "${PROJECT_SOURCE_DIR}/Images")
endif()
