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

project(OrientationAnalysisTest)

include_directories(${OrientationAnalysisTest_SOURCE_DIR})

# --------------------------------------------------------------------------
macro(ConvertPathToHeaderCompatible INPUT)
    if(WIN32)
      STRING(REPLACE "\\" "\\\\" ${INPUT} ${${INPUT}} )
      STRING(REPLACE "/" "\\\\" ${INPUT} ${${INPUT}}  )
    endif()
endmacro()
include_directories(${OrientationAnalysisTest_BINARY_DIR})
set(TEST_TEMP_DIR ${OrientationAnalysisTest_BINARY_DIR}/Temp)
file(MAKE_DIRECTORY ${TEST_TEMP_DIR})

#-- Convert all '\' to '\\' so that they are properly escaped in the header file
set(DATAIMPORT_TEST_XMLH5TIFF_OUTPUT_FILE "${TEST_TEMP_DIR}DataImportTest_XMLH5Tiff.h5")


configure_file(${OrientationAnalysisTest_SOURCE_DIR}/TestFileLocations.h.in
               ${OrientationAnalysisTest_BINARY_DIR}/TestFileLocations.h @ONLY IMMEDIATE)

include_directories(${OrientationAnalysisTest_BINARY_DIR})
include_directories(${DREAM3DProj_SOURCE_DIR}/Source/Applications)
include_directories(${DREAM3DProj_SOURCE_DIR}/Source/Plugins)
include_directories(${DREAM3DProj_SOURCE_DIR}/Source/QtSupport)
include_directories(${DREAM3DProj_SOURCE_DIR}/Source/FilterWidgets)


# --------------------------------------------------------------------------
#
# --------------------------------------------------------------------------
macro(DREAM3D_ADD_UNIT_TEST testname Exename)
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



macro(ADD_DREAM3D_TEST testname LIBRARIES)
  add_executable( ${testname} "${OrientationAnalysisTest_SOURCE_DIR}/${testname}.cpp")
  target_link_libraries( ${testname} "${LIBRARIES}")
  set_target_properties( ${testname} PROPERTIES FOLDER ${PLUGIN_NAME}Plugin/Test)
  add_test(${testname} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${testname})
endmacro()

macro(ADD_DREAM3D_TEST_2 testname LIBRARIES SOURCES)
  add_executable( ${testname} "${OrientationAnalysisTest_SOURCE_DIR}/${testname}.cpp" ${SOURCES})
  target_link_libraries( ${testname} "${LIBRARIES}")
  set_target_properties( ${testname} PROPERTIES FOLDER ${PLUGIN_NAME}Plugin/Test)
  add_test(${testname} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${testname})
endmacro()


ADD_DREAM3D_TEST(AngCachingTest  "DREAM3DLib")
