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

set(ShapeOps_HDRS
  ${SyntheticBuilding_SOURCE_DIR}/ShapeOps/ShapeOps.h
  ${SyntheticBuilding_SOURCE_DIR}/ShapeOps/CubeOctohedronOps.h
  ${SyntheticBuilding_SOURCE_DIR}/ShapeOps/CylinderOps.h
  ${SyntheticBuilding_SOURCE_DIR}/ShapeOps/EllipsoidOps.h
  ${SyntheticBuilding_SOURCE_DIR}/ShapeOps/SuperEllipsoidOps.h
)
set(ShapeOps_SRCS
  ${SyntheticBuilding_SOURCE_DIR}/ShapeOps/ShapeOps.cpp
  ${SyntheticBuilding_SOURCE_DIR}/ShapeOps/CubeOctohedronOps.cpp
  ${SyntheticBuilding_SOURCE_DIR}/ShapeOps/CylinderOps.cpp
  ${SyntheticBuilding_SOURCE_DIR}/ShapeOps/EllipsoidOps.cpp
  ${SyntheticBuilding_SOURCE_DIR}/ShapeOps/SuperEllipsoidOps.cpp
)
cmp_IDE_SOURCE_PROPERTIES( "${PLUGIN_NAME}/ShapeOps" "${ShapeOps_HDRS}" "${ShapeOps_SRCS}" "0")
if( ${PROJECT_INSTALL_HEADERS} EQUAL 1 )
    INSTALL (FILES ${ShapeOps_HDRS}
            DESTINATION include/${PLUGIN_NAME}/ShapeOps
            COMPONENT Headers   )
endif()
