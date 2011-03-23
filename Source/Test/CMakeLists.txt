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

project(AIMTest)

if (0)
set (srcs
    ${AIMTest_SOURCE_DIR}/AngDirectoryPatternTest.cpp
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/ANG/AngDirectoryPatterns.cpp
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/Common/Grain.cpp
    )
add_executable(AngDirectoryPattern ${srcs})
target_link_libraries(AngDirectoryPattern MXA)
endif()

add_executable(ODFTest ${AIMTest_SOURCE_DIR}/ODFTest.cpp  ${AIMRepresentation_SOURCE_DIR}/Source/AIM/StatsGenerator/StatsGen.cpp)
target_link_libraries(ODFTest AIMCommon)


add_executable(AngReaderTest ${AIMTest_SOURCE_DIR}/AngReaderTest.cpp)
target_link_libraries(AngReaderTest AIMCommon)

