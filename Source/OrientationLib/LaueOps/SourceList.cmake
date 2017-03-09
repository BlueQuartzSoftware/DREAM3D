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



set(OrientationLib_LaueOps_HDRS
  ${OrientationLib_SOURCE_DIR}/LaueOps/LaueOps.h
  ${OrientationLib_SOURCE_DIR}/LaueOps/CubicOps.h
  ${OrientationLib_SOURCE_DIR}/LaueOps/CubicLowOps.h
  ${OrientationLib_SOURCE_DIR}/LaueOps/HexagonalOps.h
  ${OrientationLib_SOURCE_DIR}/LaueOps/HexagonalLowOps.h
  ${OrientationLib_SOURCE_DIR}/LaueOps/OrthoRhombicOps.h
  ${OrientationLib_SOURCE_DIR}/LaueOps/TrigonalOps.h
  ${OrientationLib_SOURCE_DIR}/LaueOps/TrigonalLowOps.h
  ${OrientationLib_SOURCE_DIR}/LaueOps/TetragonalOps.h
  ${OrientationLib_SOURCE_DIR}/LaueOps/TetragonalLowOps.h
  ${OrientationLib_SOURCE_DIR}/LaueOps/TriclinicOps.h
  ${OrientationLib_SOURCE_DIR}/LaueOps/MonoclinicOps.h
  ${OrientationLib_SOURCE_DIR}/LaueOps/SO3Sampler.h
)
set(OrientationLib_LaueOps_SRCS
  ${OrientationLib_SOURCE_DIR}/LaueOps/LaueOps.cpp
  ${OrientationLib_SOURCE_DIR}/LaueOps/CubicOps.cpp
  ${OrientationLib_SOURCE_DIR}/LaueOps/CubicLowOps.cpp
  ${OrientationLib_SOURCE_DIR}/LaueOps/HexagonalOps.cpp
  ${OrientationLib_SOURCE_DIR}/LaueOps/HexagonalLowOps.cpp
  ${OrientationLib_SOURCE_DIR}/LaueOps/OrthoRhombicOps.cpp
  ${OrientationLib_SOURCE_DIR}/LaueOps/TrigonalOps.cpp
  ${OrientationLib_SOURCE_DIR}/LaueOps/TrigonalLowOps.cpp
  ${OrientationLib_SOURCE_DIR}/LaueOps/TetragonalOps.cpp
  ${OrientationLib_SOURCE_DIR}/LaueOps/TetragonalLowOps.cpp
  ${OrientationLib_SOURCE_DIR}/LaueOps/TriclinicOps.cpp
  ${OrientationLib_SOURCE_DIR}/LaueOps/MonoclinicOps.cpp
  ${OrientationLib_SOURCE_DIR}/LaueOps/SO3Sampler.cpp
)
cmp_IDE_SOURCE_PROPERTIES( "LaueOps" "${OrientationLib_LaueOps_HDRS}" "${OrientationLib_LaueOps_SRCS}" "0")
if( ${PROJECT_INSTALL_HEADERS} EQUAL 1 )
    INSTALL (FILES ${OrientationLib_LaueOps_HDRS}
            DESTINATION include/OrientationLib/LaueOps
            COMPONENT Headers   )
endif()
