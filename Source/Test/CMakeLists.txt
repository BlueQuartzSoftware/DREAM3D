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


if (0)
set (srcs
    ${AIMTest_SOURCE_DIR}/AngDirectoryPatternTest.cpp
    ${DREAM3D_SOURCE_DIR}/Source/AIM/ANG/AngDirectoryPatterns.cpp
    ${DREAM3D_SOURCE_DIR}/Source/AIM/Common/Grain.cpp
    )
add_executable(AngDirectoryPattern ${srcs})
target_link_libraries(AngDirectoryPattern MXA)
endif()

add_executable(ODFTest ${AIMTest_SOURCE_DIR}/ODFTest.cpp  ${DREAM3D_SOURCE_DIR}/Source/StatsGenerator/StatsGen.cpp)
target_link_libraries(ODFTest AIMCommon)


add_executable(AngReaderTest ${AIMTest_SOURCE_DIR}/AngReaderTest.cpp ${DREAM3D_SOURCE_DIR}/Source/AIM/OIMImport/OIMImport.cpp)
target_link_libraries(AngReaderTest AIMCommon)

add_executable(VoxelTest ${AIMTest_SOURCE_DIR}/VoxelTest.cpp)
target_link_libraries(VoxelTest AIMCommon)


add_executable(TextureTest ${AIMTest_SOURCE_DIR}/TextureTest.cpp)
target_link_libraries(TextureTest AIMCommon)

add_executable(StatsGenTest ${AIMTest_SOURCE_DIR}/StatsGenTest.cpp)
target_link_libraries(StatsGenTest AIMCommon)
MXA_ADD_TEST(StatsGenTest ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/StatsGenTest)
