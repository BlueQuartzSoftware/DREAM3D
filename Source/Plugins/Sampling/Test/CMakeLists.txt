# ============================================================================
# Copyright (c) 2009-2016 BlueQuartz Software, LLC
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

set(${PLUGIN_NAME}Test_SOURCE_DIR ${${PLUGIN_NAME}_SOURCE_DIR}/Test)
set(${PLUGIN_NAME}Test_BINARY_DIR ${${PLUGIN_NAME}_BINARY_DIR}/Test)

#------------------------------------------------------------------------------
# List all the source files here. They will NOT be compiled but instead
# be directly included in the main test source file. We list them here so that
# they will show up in IDEs
set(TEST_NAMES
  #ResampleRectGridToImageGeomTest
  CropVolumeTest
  SampleSurfaceMeshSpecifiedPointsTest
)


#------------------------------------------------------------------------------
# Include this file from the CMP Project
include(${CMP_SOURCE_DIR}/cmpCMakeMacros.cmake)
include(${SIMPLProj_SOURCE_DIR}/Source/SIMPLib/SIMPLibMacros.cmake)


get_filename_component(${PLUGIN_NAME}_PARENT_SOURCE_DIR "${${PLUGIN_NAME}_SOURCE_DIR}" DIRECTORY)
get_filename_component(${PLUGIN_NAME}_PARENT_BINARY_DIR "${${PLUGIN_NAME}_BINARY_DIR}" DIRECTORY)

SIMPL_GenerateUnitTestFile(PLUGIN_NAME ${PLUGIN_NAME}
                           TEST_DATA_DIR ${${PLUGIN_NAME}_SOURCE_DIR}/Test/Data
                           SOURCES ${TEST_NAMES}
                           LINK_LIBRARIES Qt5::Core Qt5::Gui SIMPLib
                           INCLUDE_DIRS ${${PLUGIN_NAME}_PARENT_SOURCE_DIR}
                                        ${${PLUGIN_NAME}Test_SOURCE_DIR}
                                        ${${PLUGIN_NAME}Test_BINARY_DIR}
                                        ${${PLUGIN_NAME}_PARENT_BINARY_DIR}
                           )
