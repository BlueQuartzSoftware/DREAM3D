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

set(OrientationLib_Utilities_MOC_HDRS
  ${OrientationLib_SOURCE_DIR}/Utilities/PoleFigureData.h
)


set(OrientationLib_Utilities_HDRS
  ${OrientationLib_SOURCE_DIR}/Utilities/PoleFigureUtilities.h
  ${OrientationLib_SOURCE_DIR}/Utilities/ModifiedLambertProjection.h
  ${OrientationLib_SOURCE_DIR}/Utilities/ModifiedLambertProjectionArray.h
  ${OrientationLib_SOURCE_DIR}/Utilities/ModifiedLambertProjection3D.hpp
  ${OrientationLib_SOURCE_DIR}/Utilities/PoleFigureImageUtilities.h
  ${OrientationLib_SOURCE_DIR}/Utilities/ComputeStereographicProjection.h
  ${OrientationLib_SOURCE_DIR}/Utilities/LambertUtilities.h
)

set(OrientationLib_Utilities_SRCS
  ${OrientationLib_SOURCE_DIR}/Utilities/PoleFigureUtilities.cpp
  ${OrientationLib_SOURCE_DIR}/Utilities/ModifiedLambertProjection.cpp
  ${OrientationLib_SOURCE_DIR}/Utilities/ModifiedLambertProjectionArray.cpp
  ${OrientationLib_SOURCE_DIR}/Utilities/PoleFigureImageUtilities.cpp
  ${OrientationLib_SOURCE_DIR}/Utilities/PoleFigureData.cpp
  ${OrientationLib_SOURCE_DIR}/Utilities/ComputeStereographicProjection.cpp
  ${OrientationLib_SOURCE_DIR}/Utilities/LambertUtilities.cpp
)
QT5_WRAP_CPP( OrientationLib_Generated_MOC_SRCS ${OrientationLib_Utilities_MOC_HDRS} )
set_source_files_properties( ${OrientationLib_Generated_MOC_SRCS} PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties( ${OrientationLib_Generated_MOC_SRCS} PROPERTIES GENERATED TRUE)


cmp_IDE_SOURCE_PROPERTIES( "Utilities" "${OrientationLib_Utilities_HDRS}" "${OrientationLib_Utilities_SRCS}" "0")
if( ${PROJECT_INSTALL_HEADERS} EQUAL 1 )
    INSTALL (FILES ${OrientationLib_Utilities_HDRS} ${OrientationLib_Utilities_MOC_HDRS}
            DESTINATION include/OrientationLib/Utilities
            COMPONENT Headers   )
endif()

set(OrientationLib_Utilities_HDRS ${OrientationLib_Utilities_HDRS} ${OrientationLib_Utilities_MOC_HDRS})
set(OrientationLib_Utilities_SRCS ${OrientationLib_Utilities_SRCS} ${OrientationLib_Generated_MOC_SRCS})
