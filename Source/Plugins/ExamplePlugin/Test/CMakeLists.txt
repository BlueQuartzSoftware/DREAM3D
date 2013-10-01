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

project(ExamplePluginTest)

set(DREAM3D_DATA_DIR ""  CACHE PATH "The directory where to find test data files")

# --------------------------------------------------------------------------
macro(ConvertPathToHeaderCompatible INPUT)
#message(STATUS "INPUT: ${INPUT} ${${INPUT}} ")
    if(WIN32)
      STRING(REPLACE "\\" "\\\\" ${INPUT} ${${INPUT}} )
      STRING(REPLACE "/" "\\\\" ${INPUT} ${${INPUT}}  )
    endif()
endmacro()

set(DIR_SEP "/")
if(WIN32)
  set(DIR_SEP "\\\\")
endif()
#message(STATUS "DREAM3DTest_BINARY_DIR: ${DREAM3DTest_BINARY_DIR}")
include_directories(${DREAM3DTest_BINARY_DIR})
set(TEST_TEMP_DIR ${DREAM3DTest_BINARY_DIR}/Temp/)

GET_FILENAME_COMPONENT(MXA_TEST_SRC_DIR ${CMAKE_CURRENT_LIST_FILE} PATH)
FILE(TO_NATIVE_PATH "${CMAKE_BINARY_DIR}/DataImportTest.xml" DATAIMPORT_TEST_IMPORT_XML_FILE)
file(TO_NATIVE_PATH "${DREAM3DTest_BINARY_DIR}/" DREAM3DTest_BUILD_DIR_NATIVE)
file(TO_NATIVE_PATH "${DREAM3DProj_BINARY_DIR}/" DREAM3DProj_BUILD_DIR_NATIVE)
file(TO_NATIVE_PATH "${DREAM3DProj_BINARY_DIR}/Bin/" DREAM3DProj_BIN_DIR_NATIVE)
file(TO_NATIVE_PATH "${TEST_TEMP_DIR}/" TEST_TEMP_DIR_NATIVE)

#-- Convert all '\' to '\\' so that they are properly escaped in the header file
set(DATAIMPORT_TEST_XMLH5TIFF_OUTPUT_FILE "${TEST_TEMP_DIR}DataImportTest_XMLH5Tiff.h5")
ConvertPathToHeaderCompatible(TEST_TEMP_DIR)
ConvertPathToHeaderCompatible(DATAIMPORT_TEST_XMLH5TIFF_OUTPUT_FILE)
ConvertPathToHeaderCompatible(DATAIMPORT_TEST_IMPORT_XML_FILE)
ConvertPathToHeaderCompatible(DREAM3DTest_BUILD_DIR_NATIVE)
ConvertPathToHeaderCompatible(DREAM3DProj_BIN_DIR_NATIVE)
ConvertPathToHeaderCompatible(DREAM3DProj_BUILD_DIR_NATIVE)
ConvertPathToHeaderCompatible(TEST_TEMP_DIR_NATIVE)



#configure_file(${PROJECT_RESOURCES_DIR}/EbsdTestFiles/EbsdTestFileLocation.h.in
#               ${DREAM3DTest_BINARY_DIR}/EbsdTestFileLocation.h @ONLY IMMEDIATE)
configure_file(${ExamplePluginTest_SOURCE_DIR}/ExamplePluginTestFileLocations.h.in
               ${ExamplePluginTest_BINARY_DIR}/ExamplePluginTestFileLocations.h @ONLY IMMEDIATE)

include_directories(${DREAM3DTest_BINARY_DIR})
include_directories(${DREAM3DTest_SOURCE_DIR})
include_directories(${DREAM3DProj_SOURCE_DIR}/Source/Applications)
include_directories(${DREAM3DProj_SOURCE_DIR}/Source/Plugins)
include_directories(${DREAM3DProj_SOURCE_DIR}/Source/QtSupport)
include_directories(${DREAM3DProj_SOURCE_DIR}/Source/FilterWidgets)
include_directories(${ExamplePluginTest_BINARY_DIR})

# --------------------------------------------------------------------------
#
# --------------------------------------------------------------------------
macro(DREAM3D_ADD_TEST testname Exename)
    if(NOT APPLE)
        ADD_TEST(${testname} ${Exename})
    else()
        if(NOT ${CMAKE_OSX_ARCHITECTURES} STREQUAL "")
        foreach(arch ${CMAKE_OSX_ARCHITECTURES})
            ADD_TEST( ${arch}_${testname} ${CMP_OSX_TOOLS_SOURCE_DIR}/UBTest.sh ${arch} ${Exename} )
        endforeach()
        else()
            ADD_TEST(${testname} ${Exename})
        endif()
    endif()
endmacro()


# --------------------------------------------------------------------------
# FilterParametersRW Test
# --------------------------------------------------------------------------
set(FilterParametersRWTest_SRCS
  ${ExamplePluginTest_BINARY_DIR}/ExamplePluginTestFileLocations.h
  ${ExamplePluginTest_SOURCE_DIR}/FilterParametersRWTest.cpp
  ${ExamplePlugin_SOURCE_DIR}/ExamplePluginFilters/GenericExample.h
  ${ExamplePlugin_SOURCE_DIR}/ExamplePluginFilters/GenericExample.cpp
  ${ExamplePlugin_SOURCE_DIR}/ExamplePluginFilters/ArraySelectionExample.cpp
  ${ExamplePlugin_SOURCE_DIR}/ExamplePluginFilters/ArraySelectionExample.h
  ${ExamplePlugin_SOURCE_DIR}/ExamplePluginFilters/ThresholdExample.cpp
  ${ExamplePlugin_SOURCE_DIR}/ExamplePluginFilters/ThresholdExample.h
  ${ExamplePluginTest_SOURCE_DIR}/TestFilters/TestFilters.h
  ${ExamplePluginTest_SOURCE_DIR}/TestFilters/TestFilters.cpp
)
add_executable(FilterParametersRWTest ${FilterParametersRWTest_SRCS} )
target_link_libraries(FilterParametersRWTest DREAM3DLib )
set_target_properties(FilterParametersRWTest PROPERTIES FOLDER Test)
add_test(FilterParametersRWTest ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/FilterParametersRWTest)

