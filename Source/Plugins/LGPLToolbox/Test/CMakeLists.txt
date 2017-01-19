# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
#                                                                                               #
# Copyright (c) 2015 The Regents of the University of California, Author: William Lenthe        #
#                                                                                               #
# This program is free software: you can redistribute it and/or modify it under the terms of    #
# the GNU Lesser General Public License as published by the Free Software Foundation, either    #
# version 2 of the License, or (at your option) any later version.                              #
#                                                                                               #
# This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;     #
# without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.     #
# See the GNU Lesser General Public License for more details.                                   #
#                                                                                               #
# You should have received a copy of the GNU Lesser General Public License                      #
# along with this program.  If not, see <http://www.gnu.org/licenses/>.                         #
#                                                                                               #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

project(LGPLToolboxTest)

include(${DREAM3DProj_SOURCE_DIR}/Test/DREAM3DTestFunctions.cmake)


include_directories(${${PROJECT_NAME}_SOURCE_DIR})
include_directories(${${PROJECT_NAME}_BINARY_DIR})


set(TEST_TEMP_DIR ${${PROJECT_NAME}_BINARY_DIR}/Temp)
# Make sure the directory is created during CMake time
file(MAKE_DIRECTORY ${TEST_TEMP_DIR})


configure_file(${${PROJECT_NAME}_SOURCE_DIR}/TestFileLocations.h.in
               ${${PROJECT_NAME}_BINARY_DIR}/Test/${PROJECT_NAME}FileLocations.h @ONLY IMMEDIATE)

set(${PROJECT_NAME}_Link_Libs Qt5::Core H5Support SIMPLib)

# AddDREAM3DUnitTest(TESTNAME ArrayCalculatorTest SOURCES ${${PROJECT_NAME}Test_SOURCE_DIR}/ArrayCalculatorTest.cpp FOLDER "${PLUGIN_NAME}Plugin/Test" LINK_LIBRARIES ${${PROJECT_NAME}_Link_Libs})

