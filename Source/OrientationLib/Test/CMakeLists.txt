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

include_directories(${OrientationLibTest_SOURCE_DIR})

# --------------------------------------------------------------------------
macro(ConvertPathToHeaderCompatible INPUT)
    if(WIN32)
      STRING(REPLACE "\\" "\\\\" ${INPUT} ${${INPUT}} )
      STRING(REPLACE "/" "\\\\" ${INPUT} ${${INPUT}}  )
    endif()
endmacro()
include_directories(${OrientationLibTest_BINARY_DIR})
set(TEST_TEMP_DIR ${OrientationLibTest_BINARY_DIR}/Temp)
file(MAKE_DIRECTORY ${TEST_TEMP_DIR})

configure_file(${OrientationLibTest_SOURCE_DIR}/TestFileLocations.h.in
               ${OrientationLibTest_BINARY_DIR}/TestFileLocations.h @ONLY IMMEDIATE)

include_directories(${OrientationLibTest_BINARY_DIR})
include_directories(${DREAM3DProj_SOURCE_DIR}/Source/Applications)
include_directories(${DREAM3DProj_SOURCE_DIR}/Source/Plugins)
include_directories(${DREAM3DProj_SOURCE_DIR}/Source/QtSupport)
include_directories(${DREAM3DProj_SOURCE_DIR}/Source/FilterWidgets)


function(ADD_DREAM3D_TEST)

  set(options )
  set(oneValueArgs TESTNAME )
  set(multiValueArgs LINK_LIBRARIES)
  cmake_parse_arguments(T "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )

  add_executable( ${T_TESTNAME} "${OrientationLibTest_SOURCE_DIR}/${T_TESTNAME}.cpp")
  target_link_libraries( ${T_TESTNAME} ${T_LINK_LIBRARIES})
  set_target_properties( ${T_TESTNAME} PROPERTIES FOLDER Test)
  add_test(${T_TESTNAME} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${T_TESTNAME})
endfunction()



ADD_DREAM3D_TEST(TESTNAME QuaternionMathTest LINK_LIBRARIES DREAM3DLib OrientationLib)
ADD_DREAM3D_TEST(TESTNAME OrientationMathTest LINK_LIBRARIES DREAM3DLib OrientationLib)
ADD_DREAM3D_TEST(TESTNAME TextureTest LINK_LIBRARIES DREAM3DLib OrientationLib)
ADD_DREAM3D_TEST(TESTNAME ODFTest LINK_LIBRARIES DREAM3DLib OrientationLib)
ADD_DREAM3D_TEST(TESTNAME RotArrayTest LINK_LIBRARIES DREAM3DLib OrientationLib )


