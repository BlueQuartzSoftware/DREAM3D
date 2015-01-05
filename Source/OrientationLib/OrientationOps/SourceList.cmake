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



set(OrientationLib_OrientationOps_HDRS
  ${OrientationLib_SOURCE_DIR}/OrientationOps/OrientationOps.h
  ${OrientationLib_SOURCE_DIR}/OrientationOps/CubicOps.h
  ${OrientationLib_SOURCE_DIR}/OrientationOps/CubicLowOps.h
  ${OrientationLib_SOURCE_DIR}/OrientationOps/HexagonalOps.h
  ${OrientationLib_SOURCE_DIR}/OrientationOps/HexagonalLowOps.h
  ${OrientationLib_SOURCE_DIR}/OrientationOps/OrthoRhombicOps.h
  ${OrientationLib_SOURCE_DIR}/OrientationOps/TrigonalOps.h
  ${OrientationLib_SOURCE_DIR}/OrientationOps/TrigonalLowOps.h
  ${OrientationLib_SOURCE_DIR}/OrientationOps/TetragonalOps.h
  ${OrientationLib_SOURCE_DIR}/OrientationOps/TetragonalLowOps.h
  ${OrientationLib_SOURCE_DIR}/OrientationOps/TriclinicOps.h
  ${OrientationLib_SOURCE_DIR}/OrientationOps/MonoclinicOps.h
)
set(OrientationLib_OrientationOps_SRCS
  ${OrientationLib_SOURCE_DIR}/OrientationOps/OrientationOps.cpp
  ${OrientationLib_SOURCE_DIR}/OrientationOps/CubicOps.cpp
  ${OrientationLib_SOURCE_DIR}/OrientationOps/CubicLowOps.cpp
  ${OrientationLib_SOURCE_DIR}/OrientationOps/HexagonalOps.cpp
  ${OrientationLib_SOURCE_DIR}/OrientationOps/HexagonalLowOps.cpp
  ${OrientationLib_SOURCE_DIR}/OrientationOps/OrthoRhombicOps.cpp
  ${OrientationLib_SOURCE_DIR}/OrientationOps/TrigonalOps.cpp
  ${OrientationLib_SOURCE_DIR}/OrientationOps/TrigonalLowOps.cpp
  ${OrientationLib_SOURCE_DIR}/OrientationOps/TetragonalOps.cpp
  ${OrientationLib_SOURCE_DIR}/OrientationOps/TetragonalLowOps.cpp
  ${OrientationLib_SOURCE_DIR}/OrientationOps/TriclinicOps.cpp
  ${OrientationLib_SOURCE_DIR}/OrientationOps/MonoclinicOps.cpp
)
cmp_IDE_SOURCE_PROPERTIES( "OrientationLib/OrientationOps" "${OrientationLib_OrientationOps_HDRS}" "${OrientationLib_OrientationOps_SRCS}" "0")
if( ${PROJECT_INSTALL_HEADERS} EQUAL 1 )
    INSTALL (FILES ${OrientationLib_OrientationOps_HDRS}
            DESTINATION include/OrientationLib/OrientationOps
            COMPONENT Headers   )
endif()
