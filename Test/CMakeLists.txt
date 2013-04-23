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

set(DREAM3D_DATA_DIR ""  CACHE PATH "The directory where to find test data files")

# --------
add_executable(UMich ${DREAM3DTest_SOURCE_DIR}/UMich.cpp)


# --------------------------------------------------------------------------
macro (ConvertPathToHeaderCompatible INPUT)
    if(WIN32)
      STRING(REPLACE "\\" "\\\\" ${INPUT} ${${INPUT}} )
      STRING(REPLACE "/" "\\\\" ${INPUT} ${${INPUT}}  )
    endif()
endmacro()

include_directories(${DREAM3DTest_BINARY_DIR})
set(MXA_TEMP_DIR ${DREAM3DProj_BINARY_DIR}/Testing/)

GET_FILENAME_COMPONENT(MXA_TEST_SRC_DIR ${CMAKE_CURRENT_LIST_FILE} PATH)
FILE(TO_NATIVE_PATH "${CMAKE_BINARY_DIR}/DataImportTest.xml" DATAIMPORT_TEST_IMPORT_XML_FILE)
file(TO_NATIVE_PATH "${DREAM3DTest_BINARY_DIR}" DREAM3DTest_BINARY_DIR_NATIVE)
file(TO_NATIVE_PATH "${DREAM3DProj_BINARY_DIR}" MXA_BINARY_DIR_NATIVE)
#-- Convert all '\' to '\\' so that they are properly escaped in the header file
set(DATAIMPORT_TEST_XMLH5TIFF_OUTPUT_FILE "${MXA_TEMP_DIR}DataImportTest_XMLH5Tiff.h5")
ConvertPathToHeaderCompatible(MXA_TEMP_DIR)
ConvertPathToHeaderCompatible(DATAIMPORT_TEST_XMLH5TIFF_OUTPUT_FILE)
ConvertPathToHeaderCompatible(DATAIMPORT_TEST_IMPORT_XML_FILE)
ConvertPathToHeaderCompatible(DREAM3DTest_BINARY_DIR_NATIVE)
ConvertPathToHeaderCompatible(MXA_BINARY_DIR_NATIVE)

configure_file(${PROJECT_RESOURCES_DIR}/EbsdTestFiles/EbsdTestFileLocation.h.in
               ${DREAM3DTest_BINARY_DIR}/EbsdTestFileLocation.h @ONLY IMMEDIATE)
configure_file(${DREAM3DTest_SOURCE_DIR}/TestFileLocations.h.in
               ${DREAM3DTest_BINARY_DIR}/TestFileLocations.h @ONLY IMMEDIATE)

include_directories(${DREAM3DTest_BINARY_DIR})
include_directories(${DREAM3DProj_SOURCE_DIR}/Source/Applications)
include_directories(${DREAM3DProj_SOURCE_DIR}/Source/Plugins)
include_directories(${DREAM3DProj_SOURCE_DIR}/Source/QtSupport)
include_directories(${DREAM3DProj_SOURCE_DIR}/Source/FilterWidgets)
# --------------------------------------------------------------------------
#
# --------------------------------------------------------------------------
macro(MXA_ADD_TEST testname Exename)
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

# --------------------------------------------------------------------------
# FilterPipelineTest
# --------------------------------------------------------------------------
add_executable(FilterPipelineTest FilterPipelineTest.cpp )
target_link_libraries(FilterPipelineTest DREAM3DLib)
add_test(FilterPipelineTest ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/FilterPipelineTest)

# --------------------------------------------------------------------------
# ManagedPointerArrayTest
# --------------------------------------------------------------------------
add_executable(ManagedPointerArrayTest ManagedPointerArrayTest.cpp )
target_link_libraries(ManagedPointerArrayTest DREAM3DLib)
add_test(ManagedPointerArrayTest ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/ManagedPointerArrayTest)



# --------------------------------------------------------------------------
# MXAFileSystemPathTest
# --------------------------------------------------------------------------
add_executable(MXAFileSystemPathTest MXAFileSystemPathTest.cpp )
target_link_libraries(MXAFileSystemPathTest MXA)
add_test(MXAFileSystemPathTest ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/MXAFileSystemPathTest)


# --------------------------------------------------------------------------
# StatsData Test
# --------------------------------------------------------------------------
#add_executable(StatsDataTest StatsDataTest.cpp )
#target_link_libraries(StatsDataTest DREAM3DLib)
#add_test(StatsDataTest ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/StatsDataTest)

# --------------------------------------------------------------------------
# ErrorMessageTest
# --------------------------------------------------------------------------
add_executable(ErrorMessageTest ErrorMessageTest.cpp )
target_link_libraries(ErrorMessageTest DREAM3DLib)
add_test(ErrorMessageTest ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/ErrorMessageTest)

# --------------------------------------------------------------------------
# DataContainer Test
# --------------------------------------------------------------------------
add_executable(DataContainerTest DataContainerTest.cpp )
target_link_libraries(DataContainerTest DREAM3DLib)
add_test(DataContainerTest ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/DataContainerTest)

# --------------------------------------------------------------------------
# H5Support Library Testing
# --------------------------------------------------------------------------
add_executable(H5LiteTest ${DREAM3DTest_SOURCE_DIR}/H5LiteTest.cpp)
target_link_libraries(H5LiteTest H5Support MXA)
add_test(H5LiteTest ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/H5LiteTest)

add_executable(H5UtilitiesTest ${DREAM3DTest_SOURCE_DIR}/H5UtilitiesTest.cpp)
target_link_libraries(H5UtilitiesTest H5Support MXA)
add_test(H5UtilitiesTest ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/H5UtilitiesTest)

if(0)
# THis is just a quick test to make sure that the latest HDF5 can actually write data
# sets that are larger than 4GB in size
add_executable(BigHDF5DatasetTest ${DREAM3DTest_SOURCE_DIR}/BigHDF5DatasetTest.cpp)
target_link_libraries(BigHDF5DatasetTest H5Support MXA)
add_test(BigHDF5DatasetTest ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/BigHDF5DatasetTest)
endif()

# --------------------------------------------------------------------------
# DataArray Testing
# --------------------------------------------------------------------------
add_executable(DataArrayTest ${DREAM3DTest_SOURCE_DIR}/DataArrayTest.cpp)
target_link_libraries(DataArrayTest DREAM3DLib)
add_test(DataArrayTest ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/DataArrayTest)

# --------------------------------------------------------------------------
# File Conversion Testing
# --------------------------------------------------------------------------
add_executable(PhIOTest ${DREAM3DTest_SOURCE_DIR}/PhIOTest.cpp)
target_link_libraries(PhIOTest DREAM3DLib)
add_test(PhIOTest ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/PhIOTest)

add_executable(DxIOTest ${DREAM3DTest_SOURCE_DIR}/DxIOTest.cpp)
target_link_libraries(DxIOTest DREAM3DLib)
add_test(DxIOTest ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/DxIOTest)

# --------------------------------------------------------------------------
# Ebsd Data File Testing
# --------------------------------------------------------------------------
add_executable(CtfReaderTest ${DREAM3DTest_SOURCE_DIR}/CtfReaderTest.cpp)
target_link_libraries(CtfReaderTest EbsdLib)
add_test(CtfReaderTest ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/CtfReaderTest)

#add_executable(EbsdImportTest ${DREAM3DTest_SOURCE_DIR}/EbsdImportTest.cpp
#                              ${DREAM3DProj_SOURCE_DIR}/Source/Plugins/EbsdImport/EbsdImport.cpp)
#target_link_libraries(EbsdImportTest DREAM3DLib MXA EbsdLib H5Support)
#add_test(EbsdImportTest ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/EbsdImportTest)




if(NOT ${DREAM3D_DATA_DIR} STREQUAL "")
    message(STATUS "Synthetic Builder Test using ${DREAM3D_DATA_DIR} as data directory.")
    message(STATUS "  Please make sure the proper files are available in that directory")

    # --------------------------------------------------------------------------
    # FindNeighbors Testing
    # --------------------------------------------------------------------------
    add_executable(FindNeighborsTest ${DREAM3DTest_SOURCE_DIR}/FindNeighborsTest.cpp)
    target_link_libraries(FindNeighborsTest DREAM3DLib)
    add_test(FindNeighborsTest ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/FindNeighborsTest)

    # --------------------------------------------------------------------------
    # SyntheticBuilder Test
    # --------------------------------------------------------------------------
    add_executable(SyntheticBuilderTest ${DREAM3DTest_SOURCE_DIR}/SyntheticBuilderTest.cpp)
    target_link_libraries(SyntheticBuilderTest DREAM3DLib)
    add_test(SyntheticBuilderTest ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/SyntheticBuilderTest)
endif()

# --------------------------------------------------------------------------
# Preflight Test
# --------------------------------------------------------------------------
#add_executable(PreFlightTest ${DREAM3DTest_SOURCE_DIR}/PreFlightTest.cpp)
#target_link_libraries(PreFlightTest DREAM3DLib)
#add_test(PreFlightTest ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/PreFlightTest)


# --------
add_executable(GrainDataReaderTest ${DREAM3DTest_SOURCE_DIR}/GrainDataReaderTest.cpp
                                ${DREAM3DTest_SOURCE_DIR}/CSVGrainDataReader.cpp )

# --------
add_executable(ODFTest ${DREAM3DTest_SOURCE_DIR}/ODFTest.cpp)
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
#                        ${DREAM3DProj_SOURCE_DIR}/Source/Plugins/GrainGenerator/Algorithms/PackPrimaryPhases.cpp
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
target_link_libraries(ResFixer EbsdLib MXA DREAM3DLib)


# --------
add_executable(StructArrayTest ${DREAM3DTest_SOURCE_DIR}/StructArrayTest.cpp)
target_link_libraries(StructArrayTest EbsdLib MXA DREAM3DLib)
add_test(StructArrayTest ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/StructArrayTest)

# --------
add_executable(RawBinaryReaderTest ${DREAM3DTest_SOURCE_DIR}/RawBinaryReaderTest.cpp)
target_link_libraries(RawBinaryReaderTest EbsdLib MXA DREAM3DLib)
add_test(RawBinaryReaderTest ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/RawBinaryReaderTest)

# --------------------------------------------------------------------------
# Synthetic Generation Test
# --------------------------------------------------------------------------
add_executable(SyntheticTest SyntheticTest.cpp )
target_link_libraries(SyntheticTest DREAM3DLib)
add_test(SyntheticTest ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/SyntheticTest)

# --------
if(0)
include_directories(${VXL_CORE_INCLUDE_DIR} ${VXL_VCL_INCLUDE_DIR} )
add_executable(VXLTest ${DREAM3DTest_SOURCE_DIR}/VXLTest.cpp)
target_link_libraries(VXLTest ${VXL_LIBRARIES})
endif()

