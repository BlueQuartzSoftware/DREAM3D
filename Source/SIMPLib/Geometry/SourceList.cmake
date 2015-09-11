#--////////////////////////////////////////////////////////////////////////////
#--
#--  Copyright (c) 2011, Michael A. Jackson. BlueQuartz Software
#--  Copyright (c) 2011, Michael Groeber, US Air Force Research Laboratory
#--  All rights reserved.
#--  BSD License: http://www.opensource.org/licenses/bsd-license.html
#--
#-- This code was partly written under US Air Force Contract FA8650-07-D-5800
#--
#--////////////////////////////////////////////////////////////////////////////

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

cmp_IDE_SOURCE_PROPERTIES( "DREAM3DLib/Geometry" "${SIMPLib_Geometry_HDRS}" "${SIMPLib_Geometry_SRCS}" "0")

set(SIMPLib_Geometry_HDRS
  ${SIMPLib_Geometry_HDRS}
)

if( ${PROJECT_INSTALL_HEADERS} EQUAL 1 )
    INSTALL (FILES ${SIMPLib_Geometry_HDRS}
            DESTINATION include/DREAM3D/Geometry
            COMPONENT Headers   )
endif()
