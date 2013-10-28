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

if("${DREAM3D_DATA_DIR}" STREQUAL "")
 message(FATAL_ERROR "The DREAM3D_DATA_DIR variable is not set and it needs to be set to a location where the test data is located")
endif()

# --------------------------------------------------------------------------
macro(ConvertPathToHeaderCompatible INPUT)
  #message(STATUS "INPUT: ${INPUT}  ${${INPUT}}")
    if(WIN32)
      STRING(REPLACE "\\" "\\\\" ${INPUT} ${${INPUT}} )
      STRING(REPLACE "/" "\\\\" ${INPUT} ${${INPUT}}  )
    endif()
endmacro()

include_directories(${EbsdLibTest_BINARY_DIR})
set(TEST_TEMP_DIR ${EbsdLibTest_BINARY_DIR}/Temp)
file(MAKE_DIRECTORY ${TEST_TEMP_DIR})



configure_file(${EbsdLibTest_SOURCE_DIR}/EbsdTestFileLocation.h.in
               ${EbsdLibTest_BINARY_DIR}/EbsdTestFileLocation.h @ONLY IMMEDIATE)


include_directories(${EbsdLibTest_BINARY_DIR})
include_directories(${DREAM3DProj_SOURCE_DIR}/Test)

# --------------------------------------------------------------------------
# ANG Data File Testing
# --------------------------------------------------------------------------
add_executable(AngImportTest ${EbsdLibTest_SOURCE_DIR}/AngImportTest.cpp ${EbsdLibTest_BINARY_DIR}/EbsdTestFileLocation.h )
target_link_libraries(AngImportTest ${QT_QTCORE_LIBRARY} EbsdLib)
set_target_properties(AngImportTest PROPERTIES FOLDER Test)
add_test(AngImportTest ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/AngImportTest)

# --------------------------------------------------------------------------
# CTF Data File Testing
# --------------------------------------------------------------------------
add_executable(CtfReaderTest ${EbsdLibTest_SOURCE_DIR}/CtfReaderTest.cpp ${EbsdLibTest_BINARY_DIR}/EbsdTestFileLocation.h )
target_link_libraries(CtfReaderTest EbsdLib)
set_target_properties(CtfReaderTest PROPERTIES FOLDER Test)
add_test(CtfReaderTest ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/CtfReaderTest)


# --------------------------------------------------------------------------
# HEDM Tests
# --------------------------------------------------------------------------
add_executable(HedmReaderTest HedmReaderTest.cpp ${EbsdLibTest_BINARY_DIR}/EbsdTestFileLocation.h )
target_link_libraries(HedmReaderTest EbsdLib)
set_target_properties(HedmReaderTest PROPERTIES FOLDER Test)
add_test(HedmReaderTest ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/HedmReaderTest)



