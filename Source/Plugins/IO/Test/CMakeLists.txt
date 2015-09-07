#--////////////////////////////////////////////////////////////////////////////
#--
#--  Copyright (c) 2015 BlueQuartz Software
#--  All rights reserved.
#--  BSD License: http://www.opensource.org/licenses/bsd-license.html
#--
#-- This code was partly written under US Air Force Contract FA8650-07-D-5800
#--
#--////////////////////////////////////////////////////////////////////////////

project(IOTest)

include(${DREAM3DProj_SOURCE_DIR}/Test/DREAM3DTestFunctions.cmake)


include_directories(${${PROJECT_NAME}_SOURCE_DIR})
include_directories(${${PROJECT_NAME}_BINARY_DIR})


set(TEST_TEMP_DIR ${${PROJECT_NAME}_BINARY_DIR}/Temp)
# Make sure the directory is created during CMake time
file(MAKE_DIRECTORY ${TEST_TEMP_DIR})


configure_file(${${PROJECT_NAME}_SOURCE_DIR}/TestFileLocations.h.in
               ${${PROJECT_NAME}_BINARY_DIR}/${PROJECT_NAME}FileLocations.h @ONLY IMMEDIATE)

set(${PROJECT_NAME}_Link_Libs Qt5::Core H5Support SIMPLib)


QT5_WRAP_CPP( GenerateFeatureIds_MOC  "${${PROJECT_NAME}_SOURCE_DIR}/GenerateFeatureIds.h")
set_source_files_properties(${GenerateFeatureIds_MOC} PROPERTIES HEADER_FILE_ONLY TRUE)
AddDREAM3DUnitTest(TESTNAME PhIOTest
                   SOURCES ${${PROJECT_NAME}_SOURCE_DIR}/PhIOTest.cpp ${${PROJECT_NAME}_SOURCE_DIR}/GenerateFeatureIds.h ${GenerateFeatureIds_MOC}
                   FOLDER "${PLUGIN_NAME}Plugin/Test"
                   LINK_LIBRARIES ${${PROJECT_NAME}_Link_Libs})

AddDREAM3DUnitTest(TESTNAME DxIOTest
                   SOURCES ${${PROJECT_NAME}_SOURCE_DIR}/DxIOTest.cpp ${${PROJECT_NAME}_SOURCE_DIR}/GenerateFeatureIds.h ${GenerateFeatureIds_MOC}
                   FOLDER "${PLUGIN_NAME}Plugin/Test"
                   LINK_LIBRARIES ${${PROJECT_NAME}_Link_Libs})

AddDREAM3DUnitTest(TESTNAME VtkStruturedPointsReaderTest
                  SOURCES ${${PROJECT_NAME}_SOURCE_DIR}/VtkStruturedPointsReaderTest.cpp
                  FOLDER "${PLUGIN_NAME}Plugin/Test"
                   LINK_LIBRARIES ${${PROJECT_NAME}_Link_Libs})

AddDREAM3DUnitTest(TESTNAME EnsembleInfoReaderTest SOURCES ${${PROJECT_NAME}_SOURCE_DIR}/EnsembleInfoReaderTest.cpp
                  FOLDER "${PLUGIN_NAME}Plugin/Test"
                   LINK_LIBRARIES ${${PROJECT_NAME}_Link_Libs})

AddDREAM3DUnitTest(TESTNAME ExportDataTest SOURCES ${${PROJECT_NAME}_SOURCE_DIR}/ExportDataTest.cpp
                  FOLDER "${PLUGIN_NAME}Plugin/Test"
                   LINK_LIBRARIES ${${PROJECT_NAME}_Link_Libs})

