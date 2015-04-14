#--////////////////////////////////////////////////////////////////////////////
#--
#--  Copyright (c) 2013, Michael A. Jackson. BlueQuartz Software
#--  Copyright (c) 2013, Michael Groeber, US Air Force Research Laboratory
#--  All rights reserved.
#--  BSD License: http://www.opensource.org/licenses/bsd-license.html
#--
#-- This code was partly written under US Air Force Contract FA8650-07-D-5800
#--
#--////////////////////////////////////////////////////////////////////////////

project(EbsdLibTest)

include(${DREAM3DProj_SOURCE_DIR}/Test/DREAM3DTestFunctions.cmake)

include_directories(${${PROJECT_NAME}_SOURCE_DIR})
include_directories(${${PROJECT_NAME}_BINARY_DIR})
include_directories(${EbsdLibTest_BINARY_DIR})
include_directories(${DREAM3DProj_SOURCE_DIR}/Test)

set(TEST_TEMP_DIR ${${PROJECT_NAME}_BINARY_DIR}/Temp)
# Make sure the directory is created during CMake time
file(MAKE_DIRECTORY ${TEST_TEMP_DIR})

configure_file(${EbsdLibTest_SOURCE_DIR}/EbsdTestFileLocation.h.in
               ${EbsdLibTest_BINARY_DIR}/EbsdLibTestFileLocation.h @ONLY IMMEDIATE)


set(TEST_TEMP_DIR ${${PROJECT_NAME}_BINARY_DIR}/Temp)
# Make sure the directory is created during CMake time
file(MAKE_DIRECTORY ${TEST_TEMP_DIR})


AddDREAM3DUnitTest(TESTNAME AngImportTest 
                    SOURCES ${EbsdLibTest_SOURCE_DIR}/AngImportTest.cpp
                    FOLDER "Test/EbsdLib" 
                    LINK_LIBRARIES Qt5::Core EbsdLib)

AddDREAM3DUnitTest(TESTNAME CtfReaderTest 
                    SOURCES ${EbsdLibTest_SOURCE_DIR}/CtfReaderTest.cpp
                    FOLDER "Test/EbsdLib" 
                    LINK_LIBRARIES Qt5::Core EbsdLib)

AddDREAM3DUnitTest(TESTNAME EdaxOIMReaderTest
                    SOURCES ${${PROJECT_NAME}_SOURCE_DIR}/EdaxOIMReaderTest.cpp
                    FOLDER "Test/EbsdLib" 
                    LINK_LIBRARIES Qt5::Core H5Support EbsdLib)

AddDREAM3DUnitTest(TESTNAME HedmReaderTest 
                    SOURCES ${EbsdLibTest_SOURCE_DIR}/HedmReaderTest.cpp
                    FOLDER "Test/EbsdLib" 
                    LINK_LIBRARIES Qt5::Core EbsdLib)

