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


set (srcs
    ${AIMTest_SOURCE_DIR}/AngDirectoryPatternTest.cpp
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/ANG/AngDirectoryPatterns.cpp
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/Common/Grain.cpp
    )
add_executable(AngDirectoryPattern ${srcs})
target_link_libraries(AngDirectoryPattern MXA)

