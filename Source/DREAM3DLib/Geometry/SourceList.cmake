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

set(DREAM3DLib_Geometry_HDRS
  ${DREAM3DLib_SOURCE_DIR}/Geometry/IGeometry.h
  ${DREAM3DLib_SOURCE_DIR}/Geometry/IGeometry2D.h
  ${DREAM3DLib_SOURCE_DIR}/Geometry/IGeometry3D.h
  ${DREAM3DLib_SOURCE_DIR}/Geometry/ImageGeom.h
  ${DREAM3DLib_SOURCE_DIR}/Geometry/RectGridGeom.h
  ${DREAM3DLib_SOURCE_DIR}/Geometry/VertexGeom.h
  ${DREAM3DLib_SOURCE_DIR}/Geometry/EdgeGeom.h
  ${DREAM3DLib_SOURCE_DIR}/Geometry/TriangleGeom.h
  ${DREAM3DLib_SOURCE_DIR}/Geometry/QuadGeom.h
  ${DREAM3DLib_SOURCE_DIR}/Geometry/MeshStructs.h
  ${DREAM3DLib_SOURCE_DIR}/Geometry/DerivativeHelpers.h
  ${DREAM3DLib_SOURCE_DIR}/Geometry/GeometryHelpers.hpp
  ${DREAM3DLib_SOURCE_DIR}/Geometry/ShapeOps/ShapeOps.h
  ${DREAM3DLib_SOURCE_DIR}/Geometry/ShapeOps/CubeOctohedronOps.h
  ${DREAM3DLib_SOURCE_DIR}/Geometry/ShapeOps/CylinderAOps.h
  ${DREAM3DLib_SOURCE_DIR}/Geometry/ShapeOps/CylinderBOps.h
  ${DREAM3DLib_SOURCE_DIR}/Geometry/ShapeOps/CylinderCOps.h
  ${DREAM3DLib_SOURCE_DIR}/Geometry/ShapeOps/EllipsoidOps.h
  ${DREAM3DLib_SOURCE_DIR}/Geometry/ShapeOps/SuperEllipsoidOps.h
)

set(DREAM3DLib_Geometry_SRCS
  ${DREAM3DLib_SOURCE_DIR}/Geometry/IGeometry.cpp
  ${DREAM3DLib_SOURCE_DIR}/Geometry/IGeometry2D.cpp
  ${DREAM3DLib_SOURCE_DIR}/Geometry/IGeometry3D.cpp
  ${DREAM3DLib_SOURCE_DIR}/Geometry/ImageGeom.cpp
  ${DREAM3DLib_SOURCE_DIR}/Geometry/RectGridGeom.cpp
  ${DREAM3DLib_SOURCE_DIR}/Geometry/VertexGeom.cpp
  ${DREAM3DLib_SOURCE_DIR}/Geometry/EdgeGeom.cpp
  ${DREAM3DLib_SOURCE_DIR}/Geometry/TriangleGeom.cpp
  ${DREAM3DLib_SOURCE_DIR}/Geometry/QuadGeom.cpp
  ${DREAM3DLib_SOURCE_DIR}/Geometry/DerivativeHelpers.cpp
  ${DREAM3DLib_SOURCE_DIR}/Geometry/ShapeOps/ShapeOps.cpp
  ${DREAM3DLib_SOURCE_DIR}/Geometry/ShapeOps/CubeOctohedronOps.cpp
  ${DREAM3DLib_SOURCE_DIR}/Geometry/ShapeOps/CylinderAOps.cpp
  ${DREAM3DLib_SOURCE_DIR}/Geometry/ShapeOps/CylinderBOps.cpp
  ${DREAM3DLib_SOURCE_DIR}/Geometry/ShapeOps/CylinderCOps.cpp
  ${DREAM3DLib_SOURCE_DIR}/Geometry/ShapeOps/EllipsoidOps.cpp
  ${DREAM3DLib_SOURCE_DIR}/Geometry/ShapeOps/SuperEllipsoidOps.cpp
)

cmp_IDE_SOURCE_PROPERTIES( "DREAM3DLib/Geometry" "${DREAM3DLib_Geometry_HDRS}" "${DREAM3DLib_Geometry_SRCS}" "0")

set(DREAM3DLib_Geometry_HDRS
  ${DREAM3DLib_Geometry_HDRS}
)

if( ${PROJECT_INSTALL_HEADERS} EQUAL 1 )
    INSTALL (FILES ${DREAM3DLib_Geometry_HDRS}
            DESTINATION include/DREAM3D/Geometry
            COMPONENT Headers   )
endif()
