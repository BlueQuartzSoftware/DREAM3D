#--////////////////////////////////////////////////////////////////////////////
#--
#--  Copyright (c) 2015, BlueQuartz Software
#--  All rights reserved.
#--  BSD License: http://www.opensource.org/licenses/bsd-license.html
#--
#-- This code was partly written under US Air Force Contract FA8650-07-D-5800
#--
#--////////////////////////////////////////////////////////////////////////////

# project(OrientationAnalysisTest)
set(${PLUGIN_NAME}Test_SOURCE_DIR ${${PLUGIN_NAME}_SOURCE_DIR}/Test)
set(${PLUGIN_NAME}Test_BINARY_DIR ${${PLUGIN_NAME}_BINARY_DIR}/Test)

include(${DREAM3DProj_SOURCE_DIR}/Test/DREAM3DTestFunctions.cmake)


include_directories(${${PLUGIN_NAME}_SOURCE_DIR})
include_directories(${${PLUGIN_NAME}_BINARY_DIR})


set(TEST_TEMP_DIR ${${PLUGIN_NAME}Test_BINARY_DIR}/Temp)
# Make sure the directory is created during CMake time
file(MAKE_DIRECTORY ${TEST_TEMP_DIR})


configure_file(${${PLUGIN_NAME}_SOURCE_DIR}/Test/TestFileLocations.h.in
               ${${PLUGIN_NAME}_BINARY_DIR}/Test/${PLUGIN_NAME}TestFileLocations.h @ONLY IMMEDIATE)


set(${PLUGIN_NAME}_Link_Libs Qt5::Core H5Support SIMPLib OrientationLib)

AddDREAM3DUnitTest(TESTNAME AngCachingTest 
          SOURCES ${${PLUGIN_NAME}_SOURCE_DIR}/Test/AngCachingTest.cpp
          FOLDER "${PLUGIN_NAME}Plugin/Test"
          LINK_LIBRARIES ${${PLUGIN_NAME}_Link_Libs})

AddDREAM3DUnitTest(TESTNAME CtfCachingTest 
          SOURCES ${${PLUGIN_NAME}_SOURCE_DIR}/Test/CtfCachingTest.cpp
          FOLDER "${PLUGIN_NAME}Plugin/Test"
          LINK_LIBRARIES ${${PLUGIN_NAME}_Link_Libs})

AddDREAM3DUnitTest(TESTNAME AngleFileIOTest 
          SOURCES ${${PLUGIN_NAME}_SOURCE_DIR}/Test/AngleFileIOTest.cpp
          FOLDER "${PLUGIN_NAME}Plugin/Test"
          LINK_LIBRARIES ${${PLUGIN_NAME}_Link_Libs})

AddDREAM3DUnitTest(TESTNAME OrientationUtilityTest 
                    SOURCES ${${PLUGIN_NAME}Test_SOURCE_DIR}/OrientationUtilityTest.cpp 
                    FOLDER "${PLUGIN_NAME}Plugin/Test" 
                    LINK_LIBRARIES ${${PLUGIN_NAME}_Link_Libs})

