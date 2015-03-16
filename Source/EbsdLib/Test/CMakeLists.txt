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
# include_directories(${EbsdLibTest_BINARY_DIR})
# include_directories(${DREAM3DProj_SOURCE_DIR}/Test)

set(TEST_TEMP_DIR ${${PROJECT_NAME}_BINARY_DIR}/Temp)
# Make sure the directory is created during CMake time
file(MAKE_DIRECTORY ${TEST_TEMP_DIR})

configure_file(${EbsdLibTest_SOURCE_DIR}/EbsdTestFileLocation.h.in
               ${EbsdLibTest_BINARY_DIR}/EbsdLibTestFileLocation.h @ONLY IMMEDIATE)


# --------------------------------------------------------------------------
# ANG Data File Testing
# --------------------------------------------------------------------------
AddDREAM3DUnitTest(TESTNAME AngImportTest
                    SOURCES ${${PROJECT_NAME}_SOURCE_DIR}/AngImportTest.cpp
                    LINK_LIBRARIES Qt5::Core H5Support EbsdLib)

AddDREAM3DUnitTest(TESTNAME EdaxOIMReaderTest
                    SOURCES ${${PROJECT_NAME}_SOURCE_DIR}/EdaxOIMReaderTest.cpp
                    LINK_LIBRARIES Qt5::Core H5Support EbsdLib)

# --------------------------------------------------------------------------
# CTF Data File Testing
# --------------------------------------------------------------------------
AddDREAM3DUnitTest(TESTNAME CtfReaderTest
                    SOURCES ${${PROJECT_NAME}_SOURCE_DIR}/CtfReaderTest.cpp
                    LINK_LIBRARIES Qt5::Core H5Support EbsdLib)


# --------------------------------------------------------------------------
# HEDM Tests
# --------------------------------------------------------------------------
AddDREAM3DUnitTest(TESTNAME HedmReaderTest
                    SOURCES ${${PROJECT_NAME}_SOURCE_DIR}/HedmReaderTest.cpp
                    LINK_LIBRARIES Qt5::Core H5Support EbsdLib)


