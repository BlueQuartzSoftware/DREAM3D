#--////////////////////////////////////////////////////////////////////////////
#--
#--  Copyright (c) 2014, Michael A. Jackson. BlueQuartz Software
#--  All rights reserved.
#--  BSD License: http://www.opensource.org/licenses/bsd-license.html
#--
#-- This code was partly written under US Air Force Contract FA8650-07-D-5800
#--
#--////////////////////////////////////////////////////////////////////////////

project(ImageImportTest)

include_directories(${ImageImportTest_SOURCE_DIR})
include_directories(${ImageImportTest_BINARY_DIR})


# --------------------------------------------------------------------------
# Get some Macros that we need
# --------------------------------------------------------------------------
macro(ADD_DREAM3D_TEST testname LIBRARIES)
  add_executable( ${testname} "${DREAM3DTest_SOURCE_DIR}/${testname}.cpp")
  target_link_libraries( ${testname} "${LIBRARIES}")
  set_target_properties( ${testname} PROPERTIES FOLDER Test)
  add_test(${testname} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${testname})
endmacro()

macro(ADD_DREAM3D_TEST_2 testname LIBRARIES SOURCES)
  add_executable( ${testname} "${DREAM3DTest_SOURCE_DIR}/${testname}.cpp" ${SOURCES})
  target_link_libraries( ${testname} "${LIBRARIES}")
  set_target_properties( ${testname} PROPERTIES FOLDER Test)
  add_test(${testname} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${testname})
endmacro()


#----------------------------------------------------------------------------
# Here we are trying to get something together that will run all the PrebuiltPipelines
# pipelines as a sanity check
# Reconstruction Pipelines First
set(PREBUILT_PIPELINES_DIR "PrebuiltPipelines/Workshop")
set(TEST_PIPELINE_LIST_FILE ${ImageImportTest_BINARY_DIR}/PipelineRunnerTest.txt)
set(TEST_DATA_DIR ${DREAM3D_DATA_DIR})

set(PREBUILT_PIPELINE_NAMES
  "Reconstruction/(01) SmallIN100 Import"
)

configure_file(${DREAM3DTest_SOURCE_DIR}/PipelineRunnerTest.h.in
               ${ImageImportTest_BINARY_DIR}/PipelineRunnerTest.h)

add_executable(ImageImportTestPipelineRunnerTest ${DREAM3DTest_SOURCE_DIR}/PipelineRunnerTest.cpp ${ImageImportTest_BINARY_DIR}/PipelineRunnerTest.h)
target_link_libraries(ImageImportTestPipelineRunnerTest ${QT_QTCORE_LIBRARY} EbsdLib DREAM3DLib)

FILE(WRITE ${TEST_PIPELINE_LIST_FILE} )

foreach(f ${PREBUILT_PIPELINE_NAMES} )
  set(DREAM3D_PIPELINE_FILE "${DREAM3D_SUPPORT_DIR}/${PREBUILT_PIPELINES_DIR}/${f}.txt")
  FILE(APPEND ${TEST_PIPELINE_LIST_FILE} "${DREAM3D_PIPELINE_FILE}\n")
endforeach()
