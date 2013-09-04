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

# --------------------------------------------------------------------------
macro(ConvertPathToHeaderCompatible INPUT)
  #message(STATUS "INPUT: ${INPUT}  ${${INPUT}}")
    if(WIN32)
      STRING(REPLACE "\\" "\\\\" ${INPUT} ${${INPUT}} )
      STRING(REPLACE "/" "\\\\" ${INPUT} ${${INPUT}}  )
    endif()
endmacro()
#message(STATUS "DREAM3DTest_BINARY_DIR: ${DREAM3DTest_BINARY_DIR}")
include_directories(${DREAM3DTest_BINARY_DIR})
set(TEST_TEMP_DIR ${DREAM3DTest_BINARY_DIR}/Temp)
file(MAKE_DIRECTORY ${TEST_TEMP_DIR})

# GET_FILENAME_COMPONENT(MXA_TEST_SRC_DIR ${CMAKE_CURRENT_LIST_FILE} PATH)
file(TO_NATIVE_PATH "${CMAKE_BINARY_DIR}/DataImportTest.xml" DATAIMPORT_TEST_IMPORT_XML_FILE)
file(TO_NATIVE_PATH "${DREAM3DTest_BINARY_DIR}" DREAM3DTest_BUILD_DIR_NATIVE)
file(TO_NATIVE_PATH "${DREAM3DProj_BINARY_DIR}" DREAM3DProj_BUILD_DIR_NATIVE)
file(TO_NATIVE_PATH "${DREAM3DProj_BINARY_DIR}/Bin" DREAM3DProj_BIN_DIR_NATIVE)
file(TO_NATIVE_PATH "${PROJECT_RESOURCES_DIR}/EbsdTestFiles/" EBSD_REFERENCE_DATA_DIR_NATIVE)
file(TO_NATIVE_PATH "${PROJECT_RESOURCES_DIR}/HedmTestFiles/" HEDM_REFERENCE_DATA_DIR_NATIVE)

#-- Convert all '\' to '\\' so that they are properly escaped in the header file
set(DATAIMPORT_TEST_XMLH5TIFF_OUTPUT_FILE "${TEST_TEMP_DIR}DataImportTest_XMLH5Tiff.h5")
ConvertPathToHeaderCompatible(DATAIMPORT_TEST_IMPORT_XML_FILE)
ConvertPathToHeaderCompatible(DREAM3DTest_BUILD_DIR_NATIVE)
ConvertPathToHeaderCompatible(DREAM3DProj_BUILD_DIR_NATIVE)
ConvertPathToHeaderCompatible(DREAM3DProj_BIN_DIR_NATIVE)
ConvertPathToHeaderCompatible(EBSD_REFERENCE_DATA_DIR_NATIVE)
ConvertPathToHeaderCompatible(HEDM_REFERENCE_DATA_DIR_NATIVE)

configure_file(${PROJECT_RESOURCES_DIR}/EbsdTestFiles/EbsdTestFileLocation.h.in
               ${DREAM3DTest_BINARY_DIR}/EbsdTestFileLocation.h @ONLY IMMEDIATE)
configure_file(${PROJECT_RESOURCES_DIR}/HedmTestFiles/HedmTestFileLocation.h.in
               ${DREAM3DTest_BINARY_DIR}/HedmTestFileLocation.h @ONLY IMMEDIATE)
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
macro(DREAM3D_ADD_UNIT_TEST testname Exename)
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
# H5Support Library Testing
# --------------------------------------------------------------------------
add_executable(H5LiteTest ${DREAM3DTest_SOURCE_DIR}/H5LiteTest.cpp)
target_link_libraries(H5LiteTest H5Support ${QT_QTCORE_LIBRARY})
set_target_properties(H5LiteTest PROPERTIES FOLDER Test)
add_test(H5LiteTest ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/H5LiteTest)

add_executable(H5UtilitiesTest ${DREAM3DTest_SOURCE_DIR}/H5UtilitiesTest.cpp)
target_link_libraries(H5UtilitiesTest H5Support ${QT_QTCORE_LIBRARY})
set_target_properties(H5UtilitiesTest PROPERTIES FOLDER Test)
add_test(H5UtilitiesTest ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/H5UtilitiesTest)

# --------------------------------------------------------------------------
# ANG Data File Testing
# --------------------------------------------------------------------------
add_executable(AngImportTest ${DREAM3DTest_SOURCE_DIR}/AngImportTest.cpp)
target_link_libraries(AngImportTest ${QT_QTCORE_LIBRARY} EbsdLib)
set_target_properties(AngImportTest PROPERTIES FOLDER Test)
add_test(AngImportTest ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/AngImportTest)

# --------------------------------------------------------------------------
# CTF Data File Testing
# --------------------------------------------------------------------------
add_executable(CtfReaderTest ${DREAM3DTest_SOURCE_DIR}/CtfReaderTest.cpp)
target_link_libraries(CtfReaderTest EbsdLib)
set_target_properties(CtfReaderTest PROPERTIES FOLDER Test)
add_test(CtfReaderTest ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/CtfReaderTest)


# --------------------------------------------------------------------------
# HEDM Tests
# --------------------------------------------------------------------------
add_executable(HedmReaderTest HedmReaderTest.cpp )
target_link_libraries(HedmReaderTest EbsdLib)
set_target_properties(HedmReaderTest PROPERTIES FOLDER Test)
add_test(HedmReaderTest ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/HedmReaderTest)


if(0)


# --------------------------------------------------------------------------
# EBSD Tests
# --------------------------------------------------------------------------
add_executable(EbsdTests EbsdTests.cpp )
target_link_libraries(EbsdTests DREAM3DLib)
set_target_properties(EbsdTests PROPERTIES FOLDER Test)
add_test(EbsdTests ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/EbsdTests)


# --------------------------------------------------------------------------
# Euler Angle File Loading test
# --------------------------------------------------------------------------
add_executable(AngleFileLoaderTest ${DREAM3DTest_SOURCE_DIR}/AngleFileLoaderTest.cpp)
target_link_libraries(AngleFileLoaderTest DREAM3DLib)
set_target_properties(AngleFileLoaderTest PROPERTIES FOLDER Test)


# --------------------------------------------------------------------------
# Pole Figure Test
# --------------------------------------------------------------------------
set(PFTest_SRCS
    PFTest.cpp
)
# and finally this will run moc:
add_executable(PFTest ${PFTest_SRCS} ${PFTest_Generated_MOC_SRCS})
target_link_libraries(PFTest EbsdLib DREAM3DLib ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} QtSupport)
set_target_properties(PFTest PROPERTIES FOLDER Test)
add_test(PFTest ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/PFTest)


# --------------------------------------------------------------------------
# FilterPipelineTest
# --------------------------------------------------------------------------
add_executable(FilterPipelineTest FilterPipelineTest.cpp )
target_link_libraries(FilterPipelineTest DREAM3DLib)
set_target_properties(FilterPipelineTest PROPERTIES FOLDER Test)
add_test(FilterPipelineTest ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/FilterPipelineTest)

# --------------------------------------------------------------------------
# ManagedPointerArrayTest
# --------------------------------------------------------------------------
add_executable(ManagedPointerArrayTest ManagedPointerArrayTest.cpp )
target_link_libraries(ManagedPointerArrayTest DREAM3DLib)
set_target_properties(ManagedPointerArrayTest PROPERTIES FOLDER Test)
add_test(ManagedPointerArrayTest ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/ManagedPointerArrayTest)

# --------------------------------------------------------------------------
# QuaternionMathTest.cpp Test
# --------------------------------------------------------------------------
add_executable(QuaternionMathTest QuaternionMathTest.cpp )
target_link_libraries(QuaternionMathTest DREAM3DLib)
set_target_properties(QuaternionMathTest PROPERTIES FOLDER Test)
add_test(QuaternionMathTest ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/QuaternionMathTest)


# --------------------------------------------------------------------------
# LambertArray HDF5 Write Test
# --------------------------------------------------------------------------
add_executable(LambertArrayRWTest LambertArrayRWTest.cpp )
target_link_libraries(LambertArrayRWTest DREAM3DLib)
set_target_properties(LambertArrayRWTest PROPERTIES FOLDER Test)
add_test(LambertArrayRWTest ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/LambertArrayRWTest)


# --------------------------------------------------------------------------
# ErrorMessageTest
# --------------------------------------------------------------------------
add_executable(ErrorMessageTest ErrorMessageTest.cpp )
target_link_libraries(ErrorMessageTest DREAM3DLib)
set_target_properties(ErrorMessageTest PROPERTIES FOLDER Test)
add_test(ErrorMessageTest ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/ErrorMessageTest)

# --------------------------------------------------------------------------
# DataContainer Test
# --------------------------------------------------------------------------
add_executable(DataContainerTest DataContainerTest.cpp )
target_link_libraries(DataContainerTest DREAM3DLib)
set_target_properties(DataContainerTest PROPERTIES FOLDER Test)
add_test(DataContainerTest ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/DataContainerTest)


if(0)
# THis is just a quick test to make sure that the latest HDF5 can actually write data
# sets that are larger than 4GB in size
add_executable(BigHDF5DatasetTest ${DREAM3DTest_SOURCE_DIR}/BigHDF5DatasetTest.cpp)
target_link_libraries(BigHDF5DatasetTest H5Support ${QT_QTCORE_LIBRARY})
set_target_properties(BigHDF5DatasetTest PROPERTIES FOLDER Test)
add_test(BigHDF5DatasetTest ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/BigHDF5DatasetTest)
endif()

# --------------------------------------------------------------------------
# DataArray Testing
# --------------------------------------------------------------------------
add_executable(DataArrayTest ${DREAM3DTest_SOURCE_DIR}/DataArrayTest.cpp)
target_link_libraries(DataArrayTest DREAM3DLib)
set_target_properties(DataArrayTest PROPERTIES FOLDER Test)
add_test(DataArrayTest ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/DataArrayTest)

# --------------------------------------------------------------------------
# File Conversion Testing
# --------------------------------------------------------------------------
add_executable(PhIOTest ${DREAM3DTest_SOURCE_DIR}/PhIOTest.cpp)
target_link_libraries(PhIOTest DREAM3DLib)
set_target_properties(PhIOTest PROPERTIES FOLDER Test)
add_test(PhIOTest ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/PhIOTest)

add_executable(DxIOTest ${DREAM3DTest_SOURCE_DIR}/DxIOTest.cpp)
target_link_libraries(DxIOTest DREAM3DLib)
set_target_properties(DxIOTest PROPERTIES FOLDER Test)
add_test(DxIOTest ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/DxIOTest)

if(NOT ${DREAM3D_DATA_DIR} STREQUAL "")
    message(STATUS "Synthetic Builder Test using ${DREAM3D_DATA_DIR} as data directory.")
    message(STATUS "  Please make sure the proper files are available in that directory")

    # --------------------------------------------------------------------------
    # FindNeighbors Testing
    # --------------------------------------------------------------------------
    add_executable(FindNeighborsTest ${DREAM3DTest_SOURCE_DIR}/FindNeighborsTest.cpp)
    target_link_libraries(FindNeighborsTest DREAM3DLib)
  set_target_properties(FindNeighborsTest PROPERTIES FOLDER Test)
    add_test(FindNeighborsTest ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/FindNeighborsTest)

    # --------------------------------------------------------------------------
    # SyntheticBuilder Test
    # --------------------------------------------------------------------------
    add_executable(SyntheticBuilderTest ${DREAM3DTest_SOURCE_DIR}/SyntheticBuilderTest.cpp)
    target_link_libraries(SyntheticBuilderTest DREAM3DLib)
    set_target_properties(SyntheticBuilderTest PROPERTIES FOLDER Test)
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
set_target_properties(GrainDataReaderTest PROPERTIES FOLDER Test)

# --------
add_executable(ODFTest ${DREAM3DTest_SOURCE_DIR}/ODFTest.cpp)
target_link_libraries(ODFTest DREAM3DLib)
set_target_properties(ODFTest PROPERTIES FOLDER Test)

# --------
add_executable(VoxelTest ${DREAM3DTest_SOURCE_DIR}/VoxelTest.cpp)
set_target_properties(VoxelTest PROPERTIES FOLDER Test)

# --------
add_executable(TextureTest ${DREAM3DTest_SOURCE_DIR}/TextureTest.cpp)
target_link_libraries(TextureTest DREAM3DLib)
set_target_properties(TextureTest PROPERTIES FOLDER Test)

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
set_target_properties(SurfaceMeshTest PROPERTIES FOLDER Test)

# --------
add_executable(StatsGenTest ${DREAM3DTest_SOURCE_DIR}/StatsGenTest.cpp)
target_link_libraries(StatsGenTest DREAM3DLib)
set_target_properties(StatsGenTest PROPERTIES FOLDER Test)

# --------
add_executable(RNGTest ${DREAM3DTest_SOURCE_DIR}/RNGTest.cpp)
target_link_libraries(RNGTest DREAM3DLib)
set_target_properties(RNGTest PROPERTIES FOLDER Test)

# --------
add_executable(ResFixer ${DREAM3DTest_SOURCE_DIR}/ResFixer.cpp)
target_link_libraries(ResFixer ${QT_QTCORE_LIBRARY} EbsdLib DREAM3DLib)
set_target_properties(ResFixer PROPERTIES FOLDER Test)

# --------
add_executable(OrientationMathTest ${DREAM3DTest_SOURCE_DIR}/OrientationMathTest.cpp)
target_link_libraries(OrientationMathTest ${QT_QTCORE_LIBRARY} EbsdLib DREAM3DLib)
set_target_properties(OrientationMathTest PROPERTIES FOLDER Test)
add_test(OrientationMathTest ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/OrientationMathTest)

# --------
add_executable(StructArrayTest ${DREAM3DTest_SOURCE_DIR}/StructArrayTest.cpp)
target_link_libraries(StructArrayTest ${QT_QTCORE_LIBRARY} EbsdLib DREAM3DLib)
set_target_properties(StructArrayTest PROPERTIES FOLDER Test)
add_test(StructArrayTest ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/StructArrayTest)

# --------
add_executable(RawBinaryReaderTest ${DREAM3DTest_SOURCE_DIR}/RawBinaryReaderTest.cpp)
target_link_libraries(RawBinaryReaderTest ${QT_QTCORE_LIBRARY} EbsdLib DREAM3DLib)
set_target_properties(RawBinaryReaderTest PROPERTIES FOLDER Test)
add_test(RawBinaryReaderTest ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/RawBinaryReaderTest)

# --------------------------------------------------------------------------
# Synthetic Generation Test
# --------------------------------------------------------------------------
add_executable(SyntheticTest SyntheticTest.cpp )
target_link_libraries(SyntheticTest DREAM3DLib)
set_target_properties(SyntheticTest PROPERTIES FOLDER Test)
add_test(SyntheticTest ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/SyntheticTest)

# --------------------------------------------------------------------------
# Data Fusion Test
# --------------------------------------------------------------------------
add_executable(UMich ${DREAM3DTest_SOURCE_DIR}/UMich.cpp)
set_target_properties(UMich PROPERTIES FOLDER Test)


endif()

