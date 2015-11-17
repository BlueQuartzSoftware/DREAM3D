#--////////////////////////////////////////////////////////////////////////////
#--
#--  Copyright (c) 2015 BlueQuartz Software
#--  All rights reserved.
#--  BSD License: http://www.opensource.org/licenses/bsd-license.html
#--
#-- This code was partly written under US Air Force Contract FA8650-07-D-5800
#--
#--////////////////////////////////////////////////////////////////////////////

set(${PLUGIN_NAME}Test_SOURCE_DIR ${${PLUGIN_NAME}_SOURCE_DIR}/Test)
set(${PLUGIN_NAME}Test_BINARY_DIR ${${PLUGIN_NAME}_BINARY_DIR}/Test)
include(${DREAM3DProj_SOURCE_DIR}/Test/DREAM3DTestFunctions.cmake)


include_directories(${${PLUGIN_NAME}Test_SOURCE_DIR})
include_directories(${${PLUGIN_NAME}Test_BINARY_DIR})


set(TEST_TEMP_DIR ${${PLUGIN_NAME}Test_BINARY_DIR}/Temp)
# Make sure the directory is created during CMake time
file(MAKE_DIRECTORY ${TEST_TEMP_DIR})


configure_file(${${PLUGIN_NAME}_SOURCE_DIR}/Test/TestFileLocations.h.in
               ${${PLUGIN_NAME}_BINARY_DIR}/Test/${PLUGIN_NAME}TestFileLocations.h @ONLY IMMEDIATE)

set(${PLUGIN_NAME}_Link_Libs Qt5::Core H5Support SIMPLib)


QT5_WRAP_CPP( GenerateFeatureIds_MOC  "${${PLUGIN_NAME}Test_SOURCE_DIR}/GenerateFeatureIds.h")
set_source_files_properties(${${PLUGIN_NAME}Test_SOURCE_DIR}/GenerateFeatureIds.h PROPERTIES HEADER_FILE_ONLY TRUE)
AddDREAM3DUnitTest(TESTNAME PhIOTest
                  SOURCES ${${PLUGIN_NAME}Test_SOURCE_DIR}/PhIOTest.cpp ${${PLUGIN_NAME}Test_SOURCE_DIR}/GenerateFeatureIds.h ${GenerateFeatureIds_MOC}
                  FOLDER "${PLUGIN_NAME}Plugin/Test"
                  LINK_LIBRARIES ${${PLUGIN_NAME}_Link_Libs})

AddDREAM3DUnitTest(TESTNAME DxIOTest
                  SOURCES ${${PLUGIN_NAME}Test_SOURCE_DIR}/DxIOTest.cpp ${${PLUGIN_NAME}Test_SOURCE_DIR}/GenerateFeatureIds.h ${GenerateFeatureIds_MOC}
                  FOLDER "${PLUGIN_NAME}Plugin/Test"
                  LINK_LIBRARIES ${${PLUGIN_NAME}_Link_Libs})

AddDREAM3DUnitTest(TESTNAME VtkStruturedPointsReaderTest
                  SOURCES ${${PLUGIN_NAME}Test_SOURCE_DIR}/VtkStruturedPointsReaderTest.cpp
                  FOLDER "${PLUGIN_NAME}Plugin/Test"
                  LINK_LIBRARIES ${${PLUGIN_NAME}_Link_Libs})

AddDREAM3DUnitTest(TESTNAME EnsembleInfoReaderTest 
                  SOURCES ${${PLUGIN_NAME}Test_SOURCE_DIR}/EnsembleInfoReaderTest.cpp
                  FOLDER "${PLUGIN_NAME}Plugin/Test"
                  LINK_LIBRARIES ${${PLUGIN_NAME}_Link_Libs})

AddDREAM3DUnitTest(TESTNAME ExportDataTest 
                  SOURCES ${${PLUGIN_NAME}Test_SOURCE_DIR}/ExportDataTest.cpp
                  FOLDER "${PLUGIN_NAME}Plugin/Test"
                  LINK_LIBRARIES ${${PLUGIN_NAME}_Link_Libs})


AddDREAM3DUnitTest(TESTNAME ImportASCIIDataTest 
				  SOURCES ${${PLUGIN_NAME}Test_SOURCE_DIR}/ImportASCIIDataTest.cpp
				  FOLDER "${PLUGIN_NAME}Plugin/Test" 
				  LINK_LIBRARIES ${${PLUGIN_NAME}_Link_Libs})

