#--////////////////////////////////////////////////////////////////////////////
#--
#--  Copyright (c) 2015, BlueQuartz Software
#--  All rights reserved.
#--  BSD License: http://www.opensource.org/licenses/bsd-license.html
#--
#-- This code was partly written under US Air Force Contract FA8650-07-D-5800
#--
#--////////////////////////////////////////////////////////////////////////////

# project(H5SupportTest)
set(PLUGIN_NAME "H5Support")
set(${PLUGIN_NAME}Test_SOURCE_DIR ${${PLUGIN_NAME}_SOURCE_DIR}/Test/Test)
set(${PLUGIN_NAME}Test_BINARY_DIR ${${PLUGIN_NAME}_BINARY_DIR}/Test)

include(${DREAM3DProj_SOURCE_DIR}/Test/DREAM3DTestFunctions.cmake)

include_directories(${${PLUGIN_NAME}_SOURCE_DIR}/Test)
include_directories(${${PLUGIN_NAME}_BINARY_DIR}/Test)


set(TEST_TEMP_DIR ${${PLUGIN_NAME}Test_BINARY_DIR}/Test/Temp)
# Make sure the directory is created during CMake time
file(MAKE_DIRECTORY ${TEST_TEMP_DIR})

configure_file(${${PLUGIN_NAME}_SOURCE_DIR}/Test/TestFileLocations.h.in
               ${${PLUGIN_NAME}_BINARY_DIR}/Test/${PLUGIN_NAME}TestFileLocations.h @ONLY IMMEDIATE)


AddDREAM3DUnitTest(TESTNAME H5LiteTest 
                    SOURCES ${${PLUGIN_NAME}_SOURCE_DIR}/Test/H5LiteTest.cpp
                    FOLDER "Test/H5Support" 
                    LINK_LIBRARIES Qt5::Core H5Support )
AddDREAM3DUnitTest(TESTNAME H5UtilitiesTest 
                    SOURCES ${${PLUGIN_NAME}_SOURCE_DIR}/Test/H5UtilitiesTest.cpp 
                    FOLDER "Test/H5Support" 
                    LINK_LIBRARIES Qt5::Core H5Support )


if(0)
# THis is just a quick test to make sure that the latest HDF5 can actually write data
# sets that are larger than 4GB in size
add_executable(BigHDF5DatasetTest ${DREAM3DTest_SOURCE_DIR}/BigHDF5DatasetTest.cpp)
target_link_libraries(BigHDF5DatasetTest Qt5::Core H5Support )
set_target_properties(BigHDF5DatasetTest PROPERTIES FOLDER Test)
add_test(BigHDF5DatasetTest ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/BigHDF5DatasetTest)


endif()
