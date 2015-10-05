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

set(SIMPLib_Geometry_HDRS
  ${SIMPLib_SOURCE_DIR}/Geometry/IGeometry.h
  ${SIMPLib_SOURCE_DIR}/Geometry/IGeometry2D.h
  ${SIMPLib_SOURCE_DIR}/Geometry/IGeometry3D.h
  ${SIMPLib_SOURCE_DIR}/Geometry/ImageGeom.h
  ${SIMPLib_SOURCE_DIR}/Geometry/RectGridGeom.h
  ${SIMPLib_SOURCE_DIR}/Geometry/VertexGeom.h
  ${SIMPLib_SOURCE_DIR}/Geometry/EdgeGeom.h
  ${SIMPLib_SOURCE_DIR}/Geometry/TriangleGeom.h
  ${SIMPLib_SOURCE_DIR}/Geometry/QuadGeom.h
  ${SIMPLib_SOURCE_DIR}/Geometry/MeshStructs.h
  ${SIMPLib_SOURCE_DIR}/Geometry/DerivativeHelpers.h
  ${SIMPLib_SOURCE_DIR}/Geometry/GeometryHelpers.hpp
  ${SIMPLib_SOURCE_DIR}/Geometry/ShapeOps/ShapeOps.h
  ${SIMPLib_SOURCE_DIR}/Geometry/ShapeOps/CubeOctohedronOps.h
  ${SIMPLib_SOURCE_DIR}/Geometry/ShapeOps/CylinderAOps.h
  ${SIMPLib_SOURCE_DIR}/Geometry/ShapeOps/CylinderBOps.h
  ${SIMPLib_SOURCE_DIR}/Geometry/ShapeOps/CylinderCOps.h
  ${SIMPLib_SOURCE_DIR}/Geometry/ShapeOps/EllipsoidOps.h
  ${SIMPLib_SOURCE_DIR}/Geometry/ShapeOps/SuperEllipsoidOps.h
)

set(SIMPLib_Geometry_SRCS
  ${SIMPLib_SOURCE_DIR}/Geometry/IGeometry.cpp
  ${SIMPLib_SOURCE_DIR}/Geometry/IGeometry2D.cpp
  ${SIMPLib_SOURCE_DIR}/Geometry/IGeometry3D.cpp
  ${SIMPLib_SOURCE_DIR}/Geometry/ImageGeom.cpp
  ${SIMPLib_SOURCE_DIR}/Geometry/RectGridGeom.cpp
  ${SIMPLib_SOURCE_DIR}/Geometry/VertexGeom.cpp
  ${SIMPLib_SOURCE_DIR}/Geometry/EdgeGeom.cpp
  ${SIMPLib_SOURCE_DIR}/Geometry/TriangleGeom.cpp
  ${SIMPLib_SOURCE_DIR}/Geometry/QuadGeom.cpp
  ${SIMPLib_SOURCE_DIR}/Geometry/DerivativeHelpers.cpp
  ${SIMPLib_SOURCE_DIR}/Geometry/ShapeOps/ShapeOps.cpp
  ${SIMPLib_SOURCE_DIR}/Geometry/ShapeOps/CubeOctohedronOps.cpp
  ${SIMPLib_SOURCE_DIR}/Geometry/ShapeOps/CylinderAOps.cpp
  ${SIMPLib_SOURCE_DIR}/Geometry/ShapeOps/CylinderBOps.cpp
  ${SIMPLib_SOURCE_DIR}/Geometry/ShapeOps/CylinderCOps.cpp
  ${SIMPLib_SOURCE_DIR}/Geometry/ShapeOps/EllipsoidOps.cpp
  ${SIMPLib_SOURCE_DIR}/Geometry/ShapeOps/SuperEllipsoidOps.cpp
)

cmp_IDE_SOURCE_PROPERTIES( "Geometry" "${SIMPLib_Geometry_HDRS}" "${SIMPLib_Geometry_SRCS}" "0")

set(SIMPLib_Geometry_HDRS
  ${SIMPLib_Geometry_HDRS}
)

if( ${PROJECT_INSTALL_HEADERS} EQUAL 1 )
    INSTALL (FILES ${SIMPLib_Geometry_HDRS}
            DESTINATION include/DREAM3D/Geometry
            COMPONENT Headers   )
endif()
