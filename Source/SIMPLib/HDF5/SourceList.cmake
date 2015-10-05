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

set(SIMPLib_HDF5_HDRS
  ${SIMPLib_SOURCE_DIR}/HDF5/H5BoundaryStatsDataDelegate.h
  ${SIMPLib_SOURCE_DIR}/HDF5/H5DataArrayReader.h
  ${SIMPLib_SOURCE_DIR}/HDF5/H5DataArrayWriter.hpp
  ${SIMPLib_SOURCE_DIR}/HDF5/H5Macros.h
  ${SIMPLib_SOURCE_DIR}/HDF5/H5MatrixStatsDataDelegate.h
  ${SIMPLib_SOURCE_DIR}/HDF5/H5PrecipitateStatsDataDelegate.h
  ${SIMPLib_SOURCE_DIR}/HDF5/H5PrimaryStatsDataDelegate.h
  ${SIMPLib_SOURCE_DIR}/HDF5/H5StatsDataDelegate.h
  ${SIMPLib_SOURCE_DIR}/HDF5/H5TransformationStatsDataDelegate.h
  ${SIMPLib_SOURCE_DIR}/HDF5/VTKH5Constants.h

)

set(SIMPLib_HDF5_SRCS
  ${SIMPLib_SOURCE_DIR}/HDF5/H5BoundaryStatsDataDelegate.cpp
  ${SIMPLib_SOURCE_DIR}/HDF5/H5DataArrayReader.cpp
  ${SIMPLib_SOURCE_DIR}/HDF5/H5MatrixStatsDataDelegate.cpp
  ${SIMPLib_SOURCE_DIR}/HDF5/H5PrecipitateStatsDataDelegate.cpp
  ${SIMPLib_SOURCE_DIR}/HDF5/H5PrimaryStatsDataDelegate.cpp
  ${SIMPLib_SOURCE_DIR}/HDF5/H5StatsDataDelegate.cpp
  ${SIMPLib_SOURCE_DIR}/HDF5/H5TransformationStatsDataDelegate.cpp

)

cmp_IDE_SOURCE_PROPERTIES( "HDF5" "${SIMPLib_HDF5_HDRS}" "${SIMPLib_HDF5_SRCS}" "0")
if( ${PROJECT_INSTALL_HEADERS} EQUAL 1 )
    INSTALL (FILES ${SIMPLib_HDF5_HDRS}
            DESTINATION include/DREAM3D/HDF5
            COMPONENT Headers   )
endif()
