# ============================================================================
# Copyright (c) 2009-2015 BlueQuartz Software, LLC
#
# Redistribution and use in source and binary forms, with or without modification,
# are permitted provided that the following conditions are met:
#
# Redistributions of source code must retain the above copyright notice, this
# list of conditions and the following disclaimer.
#
# Redistributions in binary form must reproduce the above copyright notice, this
# list of conditions and the following disclaimer in the documentation and/or
# other materials provided with the distribution.
#
# Neither the name of BlueQuartz Software, the US Air Force, nor the names of its
# contributors may be used to endorse or promote products derived from this software
# without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
# USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
# The code contained herein was partially funded by the followig contracts:
#    United States Air Force Prime Contract FA8650-07-D-5800
#    United States Air Force Prime Contract FA8650-10-D-5210
#    United States Prime Contract Navy N00173-07-C-2068
#
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

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

