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
    if(WIN32)
      STRING(REPLACE "\\" "\\\\" ${INPUT} ${${INPUT}} )
      STRING(REPLACE "/" "\\\\" ${INPUT} ${${INPUT}}  )
    endif()
endmacro()

include_directories(${DREAM3DTest_BINARY_DIR})




#GET_FILENAME_COMPONENT(ExamplePluginTest_TEST_SRC_DIR ${CMAKE_CURRENT_LIST_FILE} PATH)
#-- Convert all '\' to '\\' so that they are properly escaped in the header file
FILE(TO_NATIVE_PATH "${ExamplePluginTest_BINARY_DIR}/" ExamplePluginTest_BINARY_DIR_NATIVE)
FILE(TO_NATIVE_PATH "${ExamplePluginTest_SOURCE_DIR}/" ExamplePluginTest_SOURCE_DIR_NATIVE)
FILE(TO_NATIVE_PATH "${ExamplePluginTest_SOURCE_DIR}/Pipelines/" ExamplePluginTest_PIPELINE_DIR_NATIVE)

ConvertPathToHeaderCompatible(ExamplePluginTest_BINARY_DIR_NATIVE)
ConvertPathToHeaderCompatible(ExamplePluginTest_SOURCE_DIR_NATIVE)
ConvertPathToHeaderCompatible(ExamplePluginTest_PIPELINE_DIR_NATIVE)

configure_file(${ExamplePluginTest_SOURCE_DIR}/TestFileLocation.h.in
               ${ExamplePluginTest_BINARY_DIR_NATIVE}/ExamplePluginTestFileLocation.h @ONLY IMMEDIATE)


include_directories(${ExamplePluginTest_BINARY_DIR})
include_directories(${DREAM3DTest_SOURCE_DIR})
include_directories(${DREAM3DProj_SOURCE_DIR})
#include_directories(${DREAM3DProj_SOURCE_DIR}/Source/Plugins)
#include_directories(${DREAM3DProj_SOURCE_DIR}/Source/QtSupport)
#include_directories(${DREAM3DProj_SOURCE_DIR}/Source/FilterWidgets)
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


# --------------------------------------------------------------------------
# QSettings Based Filter Parameters Reader and Writer Tests
# --------------------------------------------------------------------------
add_executable(QFilterParametersIOTest QFilterParametersIOTest.cpp )
target_link_libraries(QFilterParametersIOTest ${QT_QTCORE_LIBRARY} DREAM3DLib )
set_target_properties(QFilterParametersIOTest PROPERTIES FOLDER Test)
add_test(QFilterParametersIOTest ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/QFilterParametersIOTest)
