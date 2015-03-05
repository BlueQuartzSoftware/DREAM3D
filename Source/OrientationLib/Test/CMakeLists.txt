#--////////////////////////////////////////////////////////////////////////////
#--
#--  Copyright (c) 2009, Michael A. Jackson. BlueQuartz Software
#--  Copyright (c) 2009, Michael Groeber, US Air Force Research Laboratory
#--  All rights reserved.
#--  BSD License: http://www.opensource.org/licenses/bsd-license.html
#--
#-- This code was partly written under US Air Force Contract FA8650-07-D-5800
#--
#--////////////////////////////////////////////////////////////////////////////

project(OrientationLibTest)


include_directories(${${PROJECT_NAME}_SOURCE_DIR})
include_directories(${${PROJECT_NAME}_BINARY_DIR})


set(TEST_TEMP_DIR ${${PROJECT_NAME}_BINARY_DIR}/Temp)
# Make sure the directory is created during CMake time
file(MAKE_DIRECTORY ${TEST_TEMP_DIR})


configure_file(${${PROJECT_NAME}_SOURCE_DIR}/TestFileLocations.h.in
               ${${PROJECT_NAME}_BINARY_DIR}/${PROJECT_NAME}FileLocations.h @ONLY IMMEDIATE)

set(OrientationLib_Link_Libs Qt5::Core H5Support DREAM3DLib OrientationLib)

AddDREAM3DUnitTest(TESTNAME QuaternionMathTest SOURCES ${${PROJECT_NAME}_SOURCE_DIR}/QuaternionMathTest.cpp LINK_LIBRARIES ${OrientationLib_Link_Libs})
AddDREAM3DUnitTest(TESTNAME OrientationMathTest SOURCES ${${PROJECT_NAME}_SOURCE_DIR}/OrientationMathTest.cpp LINK_LIBRARIES ${OrientationLib_Link_Libs})
AddDREAM3DUnitTest(TESTNAME TextureTest SOURCES ${${PROJECT_NAME}_SOURCE_DIR}/TextureTest.cpp LINK_LIBRARIES ${OrientationLib_Link_Libs})
AddDREAM3DUnitTest(TESTNAME ODFTest SOURCES ${${PROJECT_NAME}_SOURCE_DIR}/ODFTest.cpp LINK_LIBRARIES ${OrientationLib_Link_Libs})
AddDREAM3DUnitTest(TESTNAME RotArrayTest SOURCES ${${PROJECT_NAME}_SOURCE_DIR}/RotArrayTest.cpp LINK_LIBRARIES ${OrientationLib_Link_Libs})


