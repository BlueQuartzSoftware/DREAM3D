#--////////////////////////////////////////////////////////////////////////////
#--
#--  Copyright (c) 2015, BlueQuartz Software
#--  All rights reserved.
#--  BSD License: http://www.opensource.org/licenses/bsd-license.html
#--
#-- This code was partly written under US Air Force Contract FA8650-07-D-5800
#--
#--////////////////////////////////////////////////////////////////////////////

set(DREAM3DTest_SOURCE_DIR ${DREAM3DProj_SOURCE_DIR}/Test)
set(DREAM3DTest_BINARY_DIR ${DREAM3DProj_BINARY_DIR}/Test)

include_directories(${DREAM3DTest_SOURCE_DIR})

#------------------------------------------------------------------------------
# Include this file from the SIMPL Project
include(${SIMPLProj_SOURCE_DIR}/Test/SIMPLTestFunctions.cmake)

set(TEST_TEMP_DIR ${DREAM3DTest_BINARY_DIR}/Temp)
# Make sure the directory is created during CMake time
file(MAKE_DIRECTORY ${TEST_TEMP_DIR})


configure_file(${DREAM3DTest_SOURCE_DIR}/TestFileLocations.h.in
               ${DREAM3DTest_BINARY_DIR}/DREAM3DTestFileLocations.h @ONLY IMMEDIATE)

include_directories(${DREAM3DTest_BINARY_DIR})
include_directories(${SIMPLProj_SOURCE_DIR}/Source)
include_directories(${SIMPLProj_BINARY_DIR})

AddSIMPLUnitTest(TESTNAME FilterParameterCallbackExample
                   SOURCES ${DREAM3DTest_SOURCE_DIR}/FilterParameterCallbackExample.cpp
                   FOLDER "DREAM3D/Test"
                   LINK_LIBRARIES Qt5::Core H5Support SIMPLib  )



QT5_WRAP_CPP( PreflightVerify_MOC  "${DREAM3DTest_SOURCE_DIR}/PreflightVerify.h")
#set_source_files_properties(${DREAM3DTest_SOURCE_DIR}/PreflightVerify.h PROPERTIES HEADER_FILE_ONLY TRUE)
set(PreFlightTest_SOURCES ${DREAM3DTest_SOURCE_DIR}/PreFlightTest.cpp
                          ${DREAM3DTest_SOURCE_DIR}/PreflightVerify.h
                          ${DREAM3DTest_SOURCE_DIR}/PreflightVerify.cpp
                          ${PreflightVerify_MOC}
                          )
AddSIMPLUnitTest(TESTNAME PreFlightTest
                   SOURCES ${PreFlightTest_SOURCES}
                   FOLDER "DREAM3D/Test"
                   LINK_LIBRARIES Qt5::Core H5Support SIMPLib  )

#----------------------------------------------------------------------------
# Here we are trying to get something together that will run all the PrebuiltPipelines
# pipelines as a sanity check
# Reconstruction Pipelines First
set(PREBUILT_PIPELINES_DIR "PrebuiltPipelines/Workshop")
set(TEST_PIPELINE_LIST_FILE ${DREAM3DTest_BINARY_DIR}/PipelineRunnerTest.txt)
set(TEST_DATA_DIR ${DREAM3D_DATA_DIR})

set(PREBUILT_PIPELINE_NAMES
"Reconstruction/(01) SmallIN100 Archive"
"Reconstruction/(02) SmallIN100 Initial Visualization"
"Reconstruction/(03) SmallIN100 Alignment"
"Reconstruction/(04) SmallIN100 Presegmentation Processing"
"Reconstruction/(05) SmallIN100 Segmentation"
"Reconstruction/(06) SmallIN100 Postsegmentation Processing"
"Reconstruction/(07) SmallIN100 Final Processing"
"Reconstruction/(08) Image Initial Visualization"
"Reconstruction/(09) Image Segmentation"
"Reconstruction/(10) SmallIN100 Full Reconstruction"
"Statistics/(01) SmallIN100 Morphological Statistics"
"Statistics/(02) SmallIN100 Export Statistics"
"Statistics/(03) SmallIN100 Transfer Field Data"
"Statistics/(04) SmallIN100 Field Threshold"
"Statistics/(05) SmallIN100 Crystallographic Statistics"
"SurfaceMeshing/(01) SmallIN100 Quick Mesh"
"SurfaceMeshing/(02) SmallIN100 Smooth Mesh"
"SurfaceMeshing/(03) SmallIN100 Mesh Statistics"
"SurfaceMeshing/(04) SmallIN100 GBCD"
"Synthetic/(01) Single Cubic Phase Equiaxed"
"Synthetic/(02) Single Hexagonal Phase Equiaxed"
"Synthetic/(03) Single Cubic Phase Rolled"
"Synthetic/(04) Two Phase Cubic Hexagonal Particles Equiaxed"
"Synthetic/(05) Composite"
"Synthetic/(06) SmallIN100 Synthetic"
)

configure_file(${DREAM3DTest_SOURCE_DIR}/PipelineRunnerTest.h.in
               ${DREAM3DTest_BINARY_DIR}/PipelineRunnerTest.h)

add_executable(PipelineRunnerTest 
                ${DREAM3DTest_SOURCE_DIR}/PipelineRunnerTest.cpp ${DREAM3DTest_BINARY_DIR}/PipelineRunnerTest.h)
target_link_libraries(PipelineRunnerTest Qt5::Core EbsdLib SIMPLib)
set_target_properties(PipelineRunnerTest PROPERTIES FOLDER "SIMPLibProj/Test")
add_test(NAME PipelineRunnerTest 
          COMMAND ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/PipelineRunnerTest
          WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

FILE(WRITE ${TEST_PIPELINE_LIST_FILE} )

foreach(f ${PREBUILT_PIPELINE_NAMES} )
  set(DREAM3D_PIPELINE_FILE "${DREAM3D_SUPPORT_DIR}/${PREBUILT_PIPELINES_DIR}/${f}.json")
  FILE(APPEND ${TEST_PIPELINE_LIST_FILE} "${DREAM3D_PIPELINE_FILE}\n")
endforeach()

# Some more Testing Only pipelines that can be put here
set(PREBUILT_PIPELINES_DIR "TestPipelines")
set(TEST_PIPELINE_LIST_FILE ${DREAM3DTest_BINARY_DIR}/PipelineRunnerTest.txt)
set(TEST_DATA_DIR ${DREAM3D_DATA_DIR})
set(PREBUILT_PIPELINE_NAMES

)

foreach(f ${PREBUILT_PIPELINE_NAMES} )
  set(DREAM3D_PIPELINE_FILE "${DREAM3D_SUPPORT_DIR}/${PREBUILT_PIPELINES_DIR}/${f}")
  FILE(APPEND ${TEST_PIPELINE_LIST_FILE} "${DREAM3D_PIPELINE_FILE}\n")
endforeach()

