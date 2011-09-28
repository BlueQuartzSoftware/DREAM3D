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

SET (DREAM3DLib_ShapeOps_HDRS
  ${DREAM3DLib_SOURCE_DIR}/ShapeOps/ShapeOps.h
  ${DREAM3DLib_SOURCE_DIR}/ShapeOps/CubeOctohedronOps.h
  ${DREAM3DLib_SOURCE_DIR}/ShapeOps/CylinderOps.h
  ${DREAM3DLib_SOURCE_DIR}/ShapeOps/EllipsoidOps.h
  ${DREAM3DLib_SOURCE_DIR}/ShapeOps/SuperEllipsoidOps.h
)
SET (DREAM3DLib_ShapeOps_SRCS
  ${DREAM3DLib_SOURCE_DIR}/ShapeOps/ShapeOps.cpp
  ${DREAM3DLib_SOURCE_DIR}/ShapeOps/CubeOctohedronOps.cpp
  ${DREAM3DLib_SOURCE_DIR}/ShapeOps/CylinderOps.cpp
  ${DREAM3DLib_SOURCE_DIR}/ShapeOps/EllipsoidOps.cpp
  ${DREAM3DLib_SOURCE_DIR}/ShapeOps/SuperEllipsoidOps.cpp
)
cmp_IDE_SOURCE_PROPERTIES( "DREAM3DLib/ShapeOps" "${DREAM3DLib_ShapeOps_HDRS}" "${DREAM3DLib_ShapeOps_SRCS}" "0")
if ( ${DREAM3D_INSTALL_FILES} EQUAL 1 )
    INSTALL (FILES ${DREAM3DLib_ShapeOps_HDRS}
            DESTINATION include/DREAM3D/ShapeOps
            COMPONENT Headers   )
endif()
