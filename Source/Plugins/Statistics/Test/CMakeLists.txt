#--////////////////////////////////////////////////////////////////////////////
#--
#--  Copyright (c) 2015 BlueQuartz Software
#--  All rights reserved.
#--  BSD License: http://www.opensource.org/licenses/bsd-license.html
#--
#-- This code was partly written under US Air Force Contract FA8650-07-D-5800
#--
#--////////////////////////////////////////////////////////////////////////////

project(StatisticsTest)

include(${DREAM3DProj_SOURCE_DIR}/Test/DREAM3DTestFunctions.cmake)


include_directories(${${PROJECT_NAME}_SOURCE_DIR})
include_directories(${${PROJECT_NAME}_BINARY_DIR})


set(TEST_TEMP_DIR ${${PROJECT_NAME}_BINARY_DIR}/Temp)
# Make sure the directory is created during CMake time
file(MAKE_DIRECTORY ${TEST_TEMP_DIR})


configure_file(${${PROJECT_NAME}_SOURCE_DIR}/TestFileLocations.h.in
               ${${PROJECT_NAME}_BINARY_DIR}/${PROJECT_NAME}FileLocations.h @ONLY IMMEDIATE)

AddDREAM3DUnitTest(TESTNAME CalculateArrayHistogramTest
                  SOURCES ${${PROJECT_NAME}_SOURCE_DIR}/CalculateArrayHistogramTest.cpp
                  FOLDER "${PLUGIN_NAME}Plugin/Test"
                  LINK_LIBRARIES Qt5::Core H5Support SIMPLib)




AddDREAM3DUnitTest(TESTNAME FindDifferenceMapTest SOURCES ${${PROJECT_NAME}_SOURCE_DIR}/FindDifferenceMapTest.cpp FOLDER "${PLUGIN_NAME}Plugin/Test" LINK_LIBRARIES Qt5::Core H5Support SIMPLib)

