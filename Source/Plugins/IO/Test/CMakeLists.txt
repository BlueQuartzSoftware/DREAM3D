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
# Include this file from the CMP Project
include(${CMP_SOURCE_DIR}/cmpCMakeMacros.cmake)



set(TEST_TEMP_DIR ${${PLUGIN_NAME}_BINARY_DIR}/Test/Temp)
# Make sure the directory is created during CMake time
file(MAKE_DIRECTORY ${TEST_TEMP_DIR})

configure_file(${${PLUGIN_NAME}Test_SOURCE_DIR}/TestFileLocations.h.in
               ${${PLUGIN_NAME}Test_BINARY_DIR}/${PLUGIN_NAME}TestFileLocations.h @ONLY IMMEDIATE)

configure_file(${SIMPLProj_SOURCE_DIR}/Resources/UnitTestSupport.hpp
               ${${PLUGIN_NAME}Test_BINARY_DIR}/UnitTestSupport.hpp COPYONLY IMMEDIATE)


#------------------------------------------------------------------------------
# List all the source files here. They will NOT be compiled but instead
# be directly included in the main test source file. We list them here so that
# they will show up in IDEs
set(TEST_NAMES
  PhIOTest
  DxIOTest
  VtkStruturedPointsReaderTest
  EnsembleInfoReaderTest
  ExportDataTest
)


set( ${PLUGIN_NAME}_TEST_SRCS )
set(FilterTestIncludes "")
set(TestMainFunctors "")

foreach(name  ${TEST_NAMES})
  set( ${PLUGIN_NAME}_TEST_SRCS
    ${${PLUGIN_NAME}_TEST_SRCS}
    "${${PLUGIN_NAME}_SOURCE_DIR}/Test/${name}.cpp"
    )
  string(CONCAT
    FilterTestIncludes
    ${FilterTestIncludes}
    "#include \"${name}.cpp\"\n"
    )

  string(CONCAT
    TestMainFunctors
   ${TestMainFunctors}
   "  ${name}()()|\n")
endforeach()

if(NOT "${TestMainFunctors}" STREQUAL "")
  STRING(REPLACE "|" ";" TestMainFunctors ${TestMainFunctors}   )
endif()

configure_file(${SIMPLProj_SOURCE_DIR}/Source/SIMPLib/Testing/TestMain.cpp.in
               ${${PLUGIN_NAME}Test_BINARY_DIR}/${PLUGIN_NAME}UnitTest.cpp @ONLY)

QT5_WRAP_CPP( GenerateFeatureIds_MOC  "${${PLUGIN_NAME}Test_SOURCE_DIR}/GenerateFeatureIds.h")


# Set the source files properties on each source file.
foreach(f ${${PLUGIN_NAME}_TEST_SRCS})
  set_source_files_properties( ${f} PROPERTIES HEADER_FILE_ONLY TRUE)
endforeach()

set( ${PLUGIN_NAME}_TEST_SRCS
  ${${PLUGIN_NAME}_TEST_SRCS}
  ${${PLUGIN_NAME}Test_SOURCE_DIR}/GenerateFeatureIds.h
  ${GenerateFeatureIds_MOC}
  )

AddSIMPLUnitTest(TESTNAME ${PLUGIN_NAME}Test
                 SOURCES ${${PLUGIN_NAME}Test_BINARY_DIR}/${PLUGIN_NAME}UnitTest.cpp ${${PLUGIN_NAME}_TEST_SRCS}
                 FOLDER "${PLUGIN_NAME}Plugin/Test"
                 LINK_LIBRARIES Qt5::Core H5Support SIMPLib
                 INCLUDE_DIRS 
                      ${${PLUGIN_NAME}Test_SOURCE_DIR}
                      ${${PLUGIN_NAME}Test_BINARY_DIR}
                    )



if(MSVC)
  set_source_files_properties(${${PLUGIN_NAME}Test_BINARY_DIR}/${PLUGIN_NAME}UnitTest.cpp PROPERTIES COMPILE_FLAGS /bigobj)
endif()

