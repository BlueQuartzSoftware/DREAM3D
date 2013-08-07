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



set(DREAM3DLib_OrientationOps_HDRS
  ${DREAM3DLib_SOURCE_DIR}/OrientationOps/OrientationOps.h
  ${DREAM3DLib_SOURCE_DIR}/OrientationOps/CubicOps.h
  ${DREAM3DLib_SOURCE_DIR}/OrientationOps/CubicLowOps.h
  ${DREAM3DLib_SOURCE_DIR}/OrientationOps/HexagonalOps.h
  ${DREAM3DLib_SOURCE_DIR}/OrientationOps/HexagonalLowOps.h
  ${DREAM3DLib_SOURCE_DIR}/OrientationOps/OrthoRhombicOps.h
  ${DREAM3DLib_SOURCE_DIR}/OrientationOps/TrigonalOps.h
  ${DREAM3DLib_SOURCE_DIR}/OrientationOps/TrigonalLowOps.h
  ${DREAM3DLib_SOURCE_DIR}/OrientationOps/TetragonalOps.h
  ${DREAM3DLib_SOURCE_DIR}/OrientationOps/TetragonalLowOps.h
  ${DREAM3DLib_SOURCE_DIR}/OrientationOps/TriclinicOps.h
  ${DREAM3DLib_SOURCE_DIR}/OrientationOps/MonoclinicOps.h
)
set(DREAM3DLib_OrientationOps_SRCS
  ${DREAM3DLib_SOURCE_DIR}/OrientationOps/OrientationOps.cpp
  ${DREAM3DLib_SOURCE_DIR}/OrientationOps/CubicOps.cpp
  ${DREAM3DLib_SOURCE_DIR}/OrientationOps/CubicLowOps.cpp
  ${DREAM3DLib_SOURCE_DIR}/OrientationOps/HexagonalOps.cpp
  ${DREAM3DLib_SOURCE_DIR}/OrientationOps/HexagonalLowOps.cpp
  ${DREAM3DLib_SOURCE_DIR}/OrientationOps/OrthoRhombicOps.cpp
  ${DREAM3DLib_SOURCE_DIR}/OrientationOps/TrigonalOps.cpp
  ${DREAM3DLib_SOURCE_DIR}/OrientationOps/TrigonalLowOps.cpp
  ${DREAM3DLib_SOURCE_DIR}/OrientationOps/TetragonalOps.cpp
  ${DREAM3DLib_SOURCE_DIR}/OrientationOps/TetragonalLowOps.cpp
  ${DREAM3DLib_SOURCE_DIR}/OrientationOps/TriclinicOps.cpp
  ${DREAM3DLib_SOURCE_DIR}/OrientationOps/MonoclinicOps.cpp
)
cmp_IDE_SOURCE_PROPERTIES( "DREAM3DLib/OrientationOps" "${DREAM3DLib_OrientationOps_HDRS}" "${DREAM3DLib_OrientationOps_SRCS}" "0")
if( ${PROJECT_INSTALL_HEADERS} EQUAL 1 )
    INSTALL (FILES ${DREAM3DLib_OrientationOps_HDRS}
            DESTINATION include/DREAM3D/OrientationOps
            COMPONENT Headers   )
endif()
