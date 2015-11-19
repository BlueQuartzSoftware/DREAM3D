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

include(${DREAM3DProj_SOURCE_DIR}/Test/DREAM3DTestFunctions.cmake)



set(TEST_TEMP_DIR ${DREAM3DTest_BINARY_DIR}/Temp)
# Make sure the directory is created during CMake time
file(MAKE_DIRECTORY ${TEST_TEMP_DIR})


configure_file(${DREAM3DTest_SOURCE_DIR}/TestFileLocations.h.in
               ${DREAM3DTest_BINARY_DIR}/DREAM3DTestFileLocations.h @ONLY IMMEDIATE)

include_directories(${DREAM3DTest_BINARY_DIR})


AddDREAM3DUnitTest(TESTNAME ConditionalSetValueTest
  SOURCES ${DREAM3DTest_SOURCE_DIR}/ConditionalSetValueTest.cpp
  FOLDER "SIMPLibProj/Test"
  LINK_LIBRARIES Qt5::Core H5Support SIMPLib)

AddDREAM3DUnitTest(TESTNAME CombineAttributeMatricesTest
  SOURCES ${DREAM3DTest_SOURCE_DIR}/CombineAttributeMatricesTest.cpp
  FOLDER "SIMPLibProj/Test"
  LINK_LIBRARIES Qt5::Core H5Support SIMPLib)

AddDREAM3DUnitTest(TESTNAME DataContainerTest
  SOURCES ${DREAM3DTest_SOURCE_DIR}/DataContainerTest.cpp
  FOLDER "SIMPLibProj/Test"
  LINK_LIBRARIES Qt5::Core H5Support SIMPLib)

AddDREAM3DUnitTest(TESTNAME ReplaceValueTest
  SOURCES ${DREAM3DTest_SOURCE_DIR}/ReplaceValueTest.cpp
  FOLDER "SIMPLibProj/Test"
  LINK_LIBRARIES Qt5::Core H5Support SIMPLib)

AddDREAM3DUnitTest(TESTNAME StringDataArrayTest
  SOURCES ${DREAM3DTest_SOURCE_DIR}/StringDataArrayTest.cpp
  FOLDER "SIMPLibProj/Test"
  LINK_LIBRARIES Qt5::Core H5Support SIMPLib)

AddDREAM3DUnitTest(TESTNAME StructArrayTest
  SOURCES ${DREAM3DTest_SOURCE_DIR}/StructArrayTest.cpp
  FOLDER "SIMPLibProj/Test"
  LINK_LIBRARIES Qt5::Core H5Support SIMPLib)

AddDREAM3DUnitTest(TESTNAME FilterPipelineTest
  SOURCES ${DREAM3DTest_SOURCE_DIR}/FilterPipelineTest.cpp
  FOLDER "SIMPLibProj/Test"
  LINK_LIBRARIES Qt5::Core H5Support SIMPLib)

AddDREAM3DUnitTest(TESTNAME DataArrayTest
  SOURCES ${DREAM3DTest_SOURCE_DIR}/DataArrayTest.cpp
  FOLDER "SIMPLibProj/Test"
  LINK_LIBRARIES Qt5::Core H5Support SIMPLib)

AddDREAM3DUnitTest(TESTNAME ErrorMessageTest
  SOURCES ${DREAM3DTest_SOURCE_DIR}/ErrorMessageTest.cpp
  FOLDER "SIMPLibProj/Test"
  LINK_LIBRARIES Qt5::Core H5Support SIMPLib)

AddDREAM3DUnitTest(TESTNAME DataContainerBundleTest
  SOURCES ${DREAM3DTest_SOURCE_DIR}/DataContainerBundleTest.cpp
  FOLDER "SIMPLibProj/Test"
  LINK_LIBRARIES Qt5::Core H5Support SIMPLib)

AddDREAM3DUnitTest(TESTNAME FilterParametersRWTest
  SOURCES ${DREAM3DTest_SOURCE_DIR}/FilterParametersRWTest.cpp
  FOLDER "SIMPLibProj/Test"
  LINK_LIBRARIES Qt5::Core H5Support SIMPLib)

AddDREAM3DUnitTest(TESTNAME CreateDataArrayTest
  SOURCES ${DREAM3DTest_SOURCE_DIR}/CreateDataArrayTest.cpp
   FOLDER "SIMPLibProj/Test"
 LINK_LIBRARIES Qt5::Core H5Support SIMPLib)

AddDREAM3DUnitTest(TESTNAME RemoveComponentFromArrayTest
  SOURCES ${DREAM3DTest_SOURCE_DIR}/RemoveComponentFromArrayTest.cpp
  FOLDER "SIMPLibProj/Test"
  LINK_LIBRARIES Qt5::Core H5Support SIMPLib)

 AddDREAM3DUnitTest(TESTNAME QuaternionMathTest
   SOURCES ${DREAM3DTest_SOURCE_DIR}/QuaternionMathTest.cpp
   FOLDER "SIMPLibProj/Test"
   LINK_LIBRARIES Qt5::Core H5Support SIMPLib)

if(0)
QT5_WRAP_CPP( RemoveArraysObserver_MOC  "${DREAM3DTest_SOURCE_DIR}/RemoveArraysObserver.h")
set_source_files_properties(${DREAM3DTest_SOURCE_DIR}/RemoveArraysObserver.h PROPERTIES HEADER_FILE_ONLY TRUE)
AddDREAM3DUnitTest(TESTNAME MoveDataTest
                   SOURCES ${DREAM3DTest_SOURCE_DIR}/MoveDataTest.cpp ${DREAM3DTest_SOURCE_DIR}/RemoveArraysObserver.h ${RemoveArraysObserver_MOC}
                   FOLDER "SIMPLibProj/Test"
                   LINK_LIBRARIES Qt5::Core H5Support SIMPLib  )
endif()

QT5_WRAP_CPP( PreflightVerify_MOC  "${DREAM3DTest_SOURCE_DIR}/PreflightVerify.h")
#set_source_files_properties(${DREAM3DTest_SOURCE_DIR}/PreflightVerify.h PROPERTIES HEADER_FILE_ONLY TRUE)
set(PreFlightTest_SOURCES ${DREAM3DTest_SOURCE_DIR}/PreFlightTest.cpp
                          ${DREAM3DTest_SOURCE_DIR}/PreflightVerify.h
                          ${DREAM3DTest_SOURCE_DIR}/PreflightVerify.cpp
                          ${PreflightVerify_MOC}
                          )
AddDREAM3DUnitTest(TESTNAME PreFlightTest
                   SOURCES ${PreFlightTest_SOURCES}
                   FOLDER "SIMPLibProj/Test"
                   LINK_LIBRARIES Qt5::Core H5Support SIMPLib  )

#----------------------------------------------------------------------------
# Here we are trying to get something together that will run all the PrebuiltPipelines
# pipelines as a sanity check
# Reconstruction Pipelines First
set(PREBUILT_PIPELINES_DIR "PrebuiltPipelines/Workshop")
set(TEST_PIPELINE_LIST_FILE ${DREAM3DTest_BINARY_DIR}/PipelineRunnerTest.txt)
set(TEST_DATA_DIR ${DREAM3D_DATA_DIR})

set(PREBUILT_PIPELINE_NAMES
 "Reconstruction/(01) SmallIN100 Import"
 "Reconstruction/(02) SmallIN100 Initial Visualization"
 "Reconstruction/(03) SmallIN100 Threshold"
 "Reconstruction/(04) SmallIN100 Alignment"
 "Reconstruction/(05) SmallIN100 Alignment 2"
 "Reconstruction/(06) SmallIN100 CleanUp"
 "Reconstruction/(07) SmallIN100 Segmentation"
 "Reconstruction/(08) SmallIN100 Merge Twins"
 "Reconstruction/(09) SmallIN100 Minimum Size"
 "Reconstruction/(10) SmallIN100 Minimum Neighbors"
 "Reconstruction/(11) SmallIN100 Fill Bad Data"
 "Reconstruction/(12) SmallIN100 Close Bad Data"
 "Reconstruction/(13) Image Initial Visualization"
 "Reconstruction/(14) Image Segmentation"
 "Reconstruction/(15) SmallIN100 Full Reconstruction"
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

add_executable(PipelineRunnerTest ${DREAM3DTest_SOURCE_DIR}/PipelineRunnerTest.cpp ${DREAM3DTest_BINARY_DIR}/PipelineRunnerTest.h)
target_link_libraries(PipelineRunnerTest Qt5::Core EbsdLib SIMPLib)
set_target_properties(PipelineRunnerTest PROPERTIES FOLDER "SIMPLibProj/Test")
add_test(PipelineRunnerTest ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/PipelineRunnerTest)

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
  set(DREAM3D_PIPELINE_FILE "${DREAM3D_SUPPORT_DIR}/${PREBUILT_PIPELINES_DIR}/${f}.json")
  FILE(APPEND ${TEST_PIPELINE_LIST_FILE} "${DREAM3D_PIPELINE_FILE}\n")
endforeach()

