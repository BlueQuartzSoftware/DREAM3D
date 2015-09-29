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

# project(OrientationLibTest)
set(PLUGIN_NAME "OrientationLib")
set(${PLUGIN_NAME}Test_SOURCE_DIR ${${PLUGIN_NAME}_SOURCE_DIR}/Test/Test)
set(${PLUGIN_NAME}Test_BINARY_DIR ${${PLUGIN_NAME}_BINARY_DIR}/Test)

include(${DREAM3DProj_SOURCE_DIR}/Test/DREAM3DTestFunctions.cmake)

include_directories(${${PLUGIN_NAME}_SOURCE_DIR}/Test)
include_directories(${${PLUGIN_NAME}_BINARY_DIR})


set(TEST_TEMP_DIR ${${PLUGIN_NAME}Test_BINARY_DIR}/Test/Temp)
# Make sure the directory is created during CMake time
file(MAKE_DIRECTORY ${TEST_TEMP_DIR})


configure_file(${${PLUGIN_NAME}_SOURCE_DIR}/Test/TestFileLocations.h.in
               ${${PLUGIN_NAME}_BINARY_DIR}/Test/${PLUGIN_NAME}TestFileLocations.h @ONLY IMMEDIATE)


set(OrientationLib_Link_Libs Qt5::Core H5Support SIMPLib OrientationLib)

AddDREAM3DUnitTest(TESTNAME QuaternionMathTest
                    SOURCES ${${PLUGIN_NAME}_SOURCE_DIR}/Test/QuaternionMathTest.cpp
                    FOLDER "OrientationLibProj/Test"
                    LINK_LIBRARIES ${OrientationLib_Link_Libs})

AddDREAM3DUnitTest(TESTNAME OrientationMathTest
                    SOURCES ${${PLUGIN_NAME}_SOURCE_DIR}/Test/OrientationMathTest.cpp
                    FOLDER "OrientationLibProj/Test"
                    LINK_LIBRARIES ${OrientationLib_Link_Libs})

AddDREAM3DUnitTest(TESTNAME TextureTest
                    SOURCES ${${PLUGIN_NAME}_SOURCE_DIR}/Test/TextureTest.cpp
                    FOLDER "OrientationLibProj/Test"
                    LINK_LIBRARIES ${OrientationLib_Link_Libs})

AddDREAM3DUnitTest(TESTNAME ODFTest
                    SOURCES ${${PLUGIN_NAME}_SOURCE_DIR}/Test/ODFTest.cpp
                    FOLDER "OrientationLibProj/Test"
                    LINK_LIBRARIES ${OrientationLib_Link_Libs})

AddDREAM3DUnitTest(TESTNAME OrientationArrayTest
                    SOURCES ${${PLUGIN_NAME}_SOURCE_DIR}/Test/OrientationArrayTest.cpp ${${PLUGIN_NAME}_SOURCE_DIR}/Test/TestPrintFunctions.h
                    FOLDER "OrientationLibProj/Test"
                    LINK_LIBRARIES ${OrientationLib_Link_Libs})

AddDREAM3DUnitTest(TESTNAME OrientationConverterTest
                    SOURCES ${${PLUGIN_NAME}_SOURCE_DIR}/Test/OrientationConverterTest.cpp ${${PLUGIN_NAME}_SOURCE_DIR}/Test/TestPrintFunctions.h
                    FOLDER "OrientationLibProj/Test"
                    LINK_LIBRARIES ${OrientationLib_Link_Libs})

AddDREAM3DUnitTest(TESTNAME IPFLegendTest
                    SOURCES ${${PLUGIN_NAME}_SOURCE_DIR}/Test/IPFLegendTest.cpp
                    FOLDER "OrientationLibProj/Test"
                    LINK_LIBRARIES ${OrientationLib_Link_Libs} Qt5::Gui )

AddDREAM3DUnitTest(TESTNAME SO3SamplerTest
                    SOURCES ${${PLUGIN_NAME}_SOURCE_DIR}/Test/SO3SamplerTest.cpp
                    FOLDER "OrientationLibProj/Test"
                    LINK_LIBRARIES ${OrientationLib_Link_Libs})

AddDREAM3DUnitTest(TESTNAME OrientationTransformsTest
                    SOURCES ${${PLUGIN_NAME}_SOURCE_DIR}/Test/OrientationTransformsTest.cpp ${${PLUGIN_NAME}_SOURCE_DIR}/Test/TestPrintFunctions.h
                            ${${PLUGIN_NAME}_SOURCE_DIR}/Test/GenerateFunctionList.h
                    FOLDER "OrientationLibProj/Test"
                    LINK_LIBRARIES ${OrientationLib_Link_Libs})

