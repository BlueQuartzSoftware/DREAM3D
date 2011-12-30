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

project(DREAM3DTest)

configure_file(${PROJECT_RESOURCES_DIR}/EbsdTestFiles/EbsdTestFileLocation.h.in
               ${DREAM3DTest_BINARY_DIR}/EbsdTestFileLocation.h @ONLY IMMEDIATE)
configure_file(${DREAM3DTest_SOURCE_DIR}/TestFileLocations.h.in
               ${DREAM3DTest_BINARY_DIR}/TestFileLocations.h @ONLY IMMEDIATE)

include_directories(${DREAM3DTest_BINARY_DIR})
include_directories(${DREAM3DProj_SOURCE_DIR}/Source/Applications)
include_directories(${DREAM3DProj_SOURCE_DIR}/Source/Plugins)

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


if (0)
set (srcs
    ${DREAM3DTest_SOURCE_DIR}/AngDirectoryPatternTest.cpp
    ${DREAM3DProj_SOURCE_DIR}/Source/DREAM3D/ANG/AngDirectoryPatterns.cpp
    ${DREAM3DProj_SOURCE_DIR}/Source/DREAM3D/Common/Grain.cpp
    )
add_executable(AngDirectoryPattern ${srcs})
target_link_libraries(AngDirectoryPattern MXA)
endif()


# --------
# FilterManager Test
add_executable(FilterManagerTest ${DREAM3DTest_SOURCE_DIR}/FilterManagerTest.cpp)
target_link_libraries(FilterManagerTest ${QT_LIBRARIES} DREAM3DLib FilterWidgets)

# --------------------------------------------------------------------------
# H5Support Library Testing
# --------------------------------------------------------------------------
add_executable(H5LiteTest ${DREAM3DTest_SOURCE_DIR}/H5LiteTest.cpp)
target_link_libraries(H5LiteTest H5Support MXA)
add_test(H5LiteTest ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/H5LiteTest)

add_executable(H5UtilitiesTest ${DREAM3DTest_SOURCE_DIR}/H5UtilitiesTest.cpp)
target_link_libraries(H5UtilitiesTest H5Support MXA)
add_test(H5UtilitiesTest ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/H5UtilitiesTest)

# --------------------------------------------------------------------------
# File Conversion Testing
# --------------------------------------------------------------------------
add_executable(PhIOTest ${DREAM3DTest_SOURCE_DIR}/PhIOTest.cpp)
target_link_libraries(PhIOTest DREAM3DLib)
add_test(PhIOTest ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/PhIOTest)

add_executable(DxIOTest ${DREAM3DTest_SOURCE_DIR}/DxIOTest.cpp)
target_link_libraries(DxIOTest DREAM3DLib)
add_test(DxIOTest ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/DxIOTest)

add_executable(VtkGrainIdIOTest ${DREAM3DTest_SOURCE_DIR}/VtkGrainIdIOTest.cpp)
target_link_libraries(VtkGrainIdIOTest DREAM3DLib)
add_test(VtkGrainIdIOTest ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/VtkGrainIdIOTest)

# --------------------------------------------------------------------------
# Ebsd Data File Testing
# --------------------------------------------------------------------------
add_executable(CtfReaderTest ${DREAM3DTest_SOURCE_DIR}/CtfReaderTest.cpp)
target_link_libraries(CtfReaderTest EbsdLib)
add_test(CtfReaderTest ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/CtfReaderTest)

add_executable(EbsdImportTest ${DREAM3DTest_SOURCE_DIR}/EbsdImportTest.cpp
                              ${DREAM3DProj_SOURCE_DIR}/Source/Plugins/EbsdImport/EbsdImport.cpp)
target_link_libraries(EbsdImportTest DREAM3DLib MXA EbsdLib H5Support)
add_test(EbsdImportTest ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/EbsdImportTest)

# --------------------------------------------------------------------------
# FindNeighbors Testing
# --------------------------------------------------------------------------
add_executable(FindNeighborsTest ${DREAM3DTest_SOURCE_DIR}/FindNeighborsTest.cpp)
target_link_libraries(FindNeighborsTest DREAM3DLib)
add_test(FindNeighborsTest ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/FindNeighborsTest)


# --------
add_executable(GrainDataReaderTest ${DREAM3DTest_SOURCE_DIR}/GrainDataReaderTest.cpp
                                ${DREAM3DTest_SOURCE_DIR}/CSVGrainDataReader.cpp )

# --------
add_executable(ODFTest ${DREAM3DTest_SOURCE_DIR}/ODFTest.cpp  ${DREAM3DProj_SOURCE_DIR}/Source/Applications/StatsGenerator/StatsGen.cpp)
target_link_libraries(ODFTest DREAM3DLib)

# --------
add_executable(VoxelTest ${DREAM3DTest_SOURCE_DIR}/VoxelTest.cpp)
#target_link_libraries(VoxelTest DREAM3DLib)

# --------
add_executable(TextureTest ${DREAM3DTest_SOURCE_DIR}/TextureTest.cpp)
target_link_libraries(TextureTest DREAM3DLib)

# --------
#add_executable(DxTest ${DREAM3DTest_SOURCE_DIR}/DxTest.cpp 
#                        ${DREAM3DProj_SOURCE_DIR}/Source/Plugins/GrainGenerator/GrainGeneratorFunc.cpp
#                        ${DREAM3DProj_SOURCE_DIR}/Source/Plugins/GrainGenerator/Algorithms/PackGrainsGen3.cpp
#                        ${DREAM3DProj_SOURCE_DIR}/Source/Plugins/GrainGenerator/Algorithms/PackGrainsGen2.cpp
#                        ${DREAM3DProj_SOURCE_DIR}/Source/Plugins/GrainGenerator/StructureReaders/DXStructureReader.cpp  )
#target_link_libraries(DxTest DREAM3DLib)

# --------
add_executable(SurfaceMeshTest ${DREAM3DTest_SOURCE_DIR}/SurfaceMeshTest.cpp)
target_link_libraries(SurfaceMeshTest DREAM3DLib)

# --------
add_executable(StatsGenTest ${DREAM3DTest_SOURCE_DIR}/StatsGenTest.cpp)
target_link_libraries(StatsGenTest DREAM3DLib)

# --------
add_executable(RNGTest ${DREAM3DTest_SOURCE_DIR}/RNGTest.cpp)
target_link_libraries(RNGTest DREAM3DLib)

# --------
add_executable(ResFixer ${DREAM3DTest_SOURCE_DIR}/ResFixer.cpp)
target_link_libraries(ResFixer EbsdLib MXA)

