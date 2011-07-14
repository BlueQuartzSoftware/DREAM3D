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
    ${DREAM3D_SOURCE_DIR}/Source/DREAM3D/ANG/AngDirectoryPatterns.cpp
    ${DREAM3D_SOURCE_DIR}/Source/DREAM3D/Common/Grain.cpp
    )
add_executable(AngDirectoryPattern ${srcs})
target_link_libraries(AngDirectoryPattern MXA)
endif()

include_directories(${DREAM3D_SOURCE_DIR}/Source/Applications)
include_directories(${DREAM3D_SOURCE_DIR}/Source/Plugins)

add_executable(ODFTest ${AIMTest_SOURCE_DIR}/ODFTest.cpp  ${DREAM3D_SOURCE_DIR}/Source/Applications/StatsGenerator/StatsGen.cpp)
target_link_libraries(ODFTest DREAM3DLib)


add_executable(AngReaderTest ${AIMTest_SOURCE_DIR}/AngReaderTest.cpp 
                            ${DREAM3D_SOURCE_DIR}/Source/Plugins/OIMImport/OIMImport.cpp)
target_link_libraries(AngReaderTest DREAM3DLib)

add_executable(VoxelTest ${AIMTest_SOURCE_DIR}/VoxelTest.cpp)
#target_link_libraries(VoxelTest DREAM3DLib)

add_executable(TextureTest ${AIMTest_SOURCE_DIR}/TextureTest.cpp)
target_link_libraries(TextureTest DREAM3DLib)

add_executable(VTKFileWritersTest ${AIMTest_SOURCE_DIR}/VTKFileWritersTest.cpp
                ${DREAM3D_SOURCE_DIR}/Source/Plugins/GrainGenerator/GrainGeneratorFunc.cpp
                ${DREAM3D_SOURCE_DIR}/Source/Plugins/Reconstruction/ReconstructionFunc.cpp
                ${DREAM3D_SOURCE_DIR}/Source/Plugins/MicrostructureStatistics/MicrostructureStatisticsFunc.cpp
)
target_link_libraries(VTKFileWritersTest DREAM3DLib)

add_executable(PhWriterTest ${AIMTest_SOURCE_DIR}/PhWriterTest.cpp)
target_link_libraries(PhWriterTest DREAM3DLib)

add_executable(DxTest ${AIMTest_SOURCE_DIR}/DxTest.cpp 
                        ${DREAM3D_SOURCE_DIR}/Source/Plugins/GrainGenerator/GrainGeneratorFunc.cpp
                        ${DREAM3D_SOURCE_DIR}/Source/Plugins/GrainGenerator/StructureReaders/DXStructureReader.cpp  )
target_link_libraries(DxTest DREAM3DLib)

add_executable(SurfaceMeshTest ${AIMTest_SOURCE_DIR}/SurfaceMeshTest.cpp)
target_link_libraries(SurfaceMeshTest DREAM3DLib)



add_executable(StatsGenTest ${AIMTest_SOURCE_DIR}/StatsGenTest.cpp)
target_link_libraries(StatsGenTest DREAM3DLib)
MXA_ADD_TEST(StatsGenTest ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/StatsGenTest)
