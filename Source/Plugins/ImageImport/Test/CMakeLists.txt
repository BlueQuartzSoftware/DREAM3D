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

project(ImageImportTest)

include_directories(${ImageImportTest_SOURCE_DIR})

# --------------------------------------------------------------------------
macro(ConvertPathToHeaderCompatible INPUT)
    if(WIN32)
      STRING(REPLACE "\\" "\\\\" ${INPUT} ${${INPUT}} )
      STRING(REPLACE "/" "\\\\" ${INPUT} ${${INPUT}}  )
    endif()
endmacro()
include_directories(${ImageImportTest_BINARY_DIR})
set(TEST_TEMP_DIR ${ImageImportTest_BINARY_DIR}/Temp)
file(MAKE_DIRECTORY ${TEST_TEMP_DIR})

#-- Convert all '\' to '\\' so that they are properly escaped in the header file
set(DATAIMPORT_TEST_XMLH5TIFF_OUTPUT_FILE "${TEST_TEMP_DIR}DataImportTest_XMLH5Tiff.h5")


configure_file(${ImageImportTest_SOURCE_DIR}/TestFileLocations.h.in
               ${ImageImportTest_BINARY_DIR}/TestFileLocations.h @ONLY IMMEDIATE)

include_directories(${ImageImportTest_BINARY_DIR})

include_directories(${DREAM3DProj_SOURCE_DIR}/Source/Plugins)
include_directories(${DREAM3DProj_SOURCE_DIR}/Source/QtSupport)




macro(ADD_DREAM3D_TEST testname LIBRARIES)
  add_executable( ${testname} "${ImageImportTest_SOURCE_DIR}/${testname}.cpp")
  target_link_libraries( ${testname} "${LIBRARIES}")
  set_target_properties( ${testname} PROPERTIES FOLDER ${PLUGIN_NAME}Plugin/Test)
  add_test(${testname} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${testname})
endmacro()




ADD_DREAM3D_TEST(SaveImagesTest "DREAM3DLib;Qt5::Core;Qt5::Gui")
