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

if("${DREAM3D_DATA_DIR}" STREQUAL "")
 message(FATAL_ERROR "The DREAM3D_DATA_DIR variable is not set and it needs to be set to a location where the test data is located")
endif()
# --------------------------------------------------------------------------
macro(ConvertPathToHeaderCompatible INPUT)
    if(WIN32)
      STRING(REPLACE "\\" "\\\\" ${INPUT} ${${INPUT}} )
      STRING(REPLACE "/" "\\\\" ${INPUT} ${${INPUT}}  )
    endif()
endmacro()
include_directories(${DREAM3DTest_BINARY_DIR})
set(TEST_TEMP_DIR ${DREAM3DTest_BINARY_DIR}/Temp)
file(MAKE_DIRECTORY ${TEST_TEMP_DIR})

#-- Convert all '\' to '\\' so that they are properly escaped in the header file
set(DATAIMPORT_TEST_XMLH5TIFF_OUTPUT_FILE "${TEST_TEMP_DIR}DataImportTest_XMLH5Tiff.h5")


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



macro(ADD_DREAM3D_TEST testname LIBRARIES)
  add_executable( ${testname} "${DREAM3DTest_SOURCE_DIR}/${testname}.cpp")
message(STATUS "LIBRARIES: ${LIBRARIES}")
  target_link_libraries( ${testname} "${LIBRARIES}")
  set_target_properties( ${testname} PROPERTIES FOLDER Test)
  add_test(${testname} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${testname})
endmacro()



ADD_DREAM3D_TEST(H5LiteTest "H5Support")
ADD_DREAM3D_TEST(H5UtilitiesTest "H5Support")


ADD_DREAM3D_TEST(QuaternionMathTest  "DREAM3DLib")
ADD_DREAM3D_TEST(OrientationMathTest  "DREAM3DLib")
ADD_DREAM3D_TEST(StructArrayTest  "DREAM3DLib")
ADD_DREAM3D_TEST(DxIOTest  "DREAM3DLib")
ADD_DREAM3D_TEST(FilterPipelineTest  "DREAM3DLib")
ADD_DREAM3D_TEST(DataArrayTest  "DREAM3DLib")
ADD_DREAM3D_TEST(ErrorMessageTest  "DREAM3DLib")
ADD_DREAM3D_TEST(GenericFilterTest  "DREAM3DLib")
ADD_DREAM3D_TEST(PreFlightTest  "DREAM3DLib")
ADD_DREAM3D_TEST(DataContainerTest  "DREAM3DLib")

if(0)
ADD_DREAM3D_TEST(RawBinaryReaderTest  "DREAM3DLib")
ADD_DREAM3D_TEST(StatsGenTest  "DREAM3DLib")
ADD_DREAM3D_TEST(PhIOTest  "DREAM3DLib")

ADD_DREAM3D_TEST(QFilterParameterIOTest  "DREAM3DLib")
ADD_DREAM3D_TEST(AngleFileLoaderTest  "DREAM3DLib")


ADD_DREAM3D_TEST(ManagedPointerArrayTest  "DREAM3DLib")


ADD_DREAM3D_TEST(SyntheticTest  "DREAM3DLib")
ADD_DREAM3D_TEST(FilterParametersRWTest  "DREAM3DLib")

ADD_DREAM3D_TEST(LambertArrayRWTest  "DREAM3DLib")
ADD_DREAM3D_TEST(NewDataContainerStructureTest  "DREAM3DLib")
endif()


# --------------------------------------------------------------------------
# ReplaceText Program
# --------------------------------------------------------------------------
add_executable(ReplaceText ${DREAM3DTest_SOURCE_DIR}/ReplaceText.cpp)
target_link_libraries(ReplaceText ${QT_QTCORE_LIBRARY})
set_target_properties(ReplaceText PROPERTIES FOLDER Test)

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
add_executable(SurfaceMeshTest ${DREAM3DTest_SOURCE_DIR}/SurfaceMeshTest.cpp)
target_link_libraries(SurfaceMeshTest DREAM3DLib)
set_target_properties(SurfaceMeshTest PROPERTIES FOLDER Test)


# --------
add_executable(RNGTest ${DREAM3DTest_SOURCE_DIR}/RNGTest.cpp)
target_link_libraries(RNGTest DREAM3DLib)
set_target_properties(RNGTest PROPERTIES FOLDER Test)

# --------
add_executable(ResFixer ${DREAM3DTest_SOURCE_DIR}/ResFixer.cpp)
target_link_libraries(ResFixer ${QT_QTCORE_LIBRARY} EbsdLib DREAM3DLib)
set_target_properties(ResFixer PROPERTIES FOLDER Test)



if(0)
# THis is just a quick test to make sure that the latest HDF5 can actually write data
# sets that are larger than 4GB in size
add_executable(BigHDF5DatasetTest ${DREAM3DTest_SOURCE_DIR}/BigHDF5DatasetTest.cpp)
target_link_libraries(BigHDF5DatasetTest H5Support ${QT_QTCORE_LIBRARY})
set_target_properties(BigHDF5DatasetTest PROPERTIES FOLDER Test)
add_test(BigHDF5DatasetTest ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/BigHDF5DatasetTest)


endif()



