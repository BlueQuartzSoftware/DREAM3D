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

project(PipelineBuilderTest)

include_directories(${PipelineBuilderTest_BINARY_DIR})
include_directories(${DREAM3DProj_SOURCE_DIR}/Source/QtSupport)
include_directories(${PipelineBuilderPlugin_SOURCE_DIR})

# --------------------------------------------------------------------------
#
# --------------------------------------------------------------------------
macro(MXA_ADD_TEST testname Exename)
    if (NOT APPLE)
        ADD_TEST(${testname} ${Exename})
    else()
        foreach(arch ${CMAKE_OSX_ARCHITECTURES})
            ADD_TEST( ${arch}_${testname} ${CMP_OSX_TOOLS_SOURCE_DIR}/UBTest.sh ${arch} ${Exename} )
        endforeach()
    endif()
endmacro()

# --------------------------------------------------------------------------
# FilterManager Test
# --------------------------------------------------------------------------
set(FilterManagerTest_SRCS
    ${PipelineBuilderTest_SOURCE_DIR}/FilterManagerTest.cpp
)
add_executable(FilterManagerTest ${FilterManagerTest_SRCS} )
target_link_libraries(FilterManagerTest FilterWidgets)

