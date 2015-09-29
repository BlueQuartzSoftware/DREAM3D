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



set(OrientationLib_SpaceGroupOps_HDRS
  ${OrientationLib_SOURCE_DIR}/SpaceGroupOps/SpaceGroupOps.h
  ${OrientationLib_SOURCE_DIR}/SpaceGroupOps/CubicOps.h
  ${OrientationLib_SOURCE_DIR}/SpaceGroupOps/CubicLowOps.h
  ${OrientationLib_SOURCE_DIR}/SpaceGroupOps/HexagonalOps.h
  ${OrientationLib_SOURCE_DIR}/SpaceGroupOps/HexagonalLowOps.h
  ${OrientationLib_SOURCE_DIR}/SpaceGroupOps/OrthoRhombicOps.h
  ${OrientationLib_SOURCE_DIR}/SpaceGroupOps/TrigonalOps.h
  ${OrientationLib_SOURCE_DIR}/SpaceGroupOps/TrigonalLowOps.h
  ${OrientationLib_SOURCE_DIR}/SpaceGroupOps/TetragonalOps.h
  ${OrientationLib_SOURCE_DIR}/SpaceGroupOps/TetragonalLowOps.h
  ${OrientationLib_SOURCE_DIR}/SpaceGroupOps/TriclinicOps.h
  ${OrientationLib_SOURCE_DIR}/SpaceGroupOps/MonoclinicOps.h
  ${OrientationLib_SOURCE_DIR}/SpaceGroupOps/SO3Sampler.h
)
set(OrientationLib_SpaceGroupOps_SRCS
  ${OrientationLib_SOURCE_DIR}/SpaceGroupOps/SpaceGroupOps.cpp
  ${OrientationLib_SOURCE_DIR}/SpaceGroupOps/CubicOps.cpp
  ${OrientationLib_SOURCE_DIR}/SpaceGroupOps/CubicLowOps.cpp
  ${OrientationLib_SOURCE_DIR}/SpaceGroupOps/HexagonalOps.cpp
  ${OrientationLib_SOURCE_DIR}/SpaceGroupOps/HexagonalLowOps.cpp
  ${OrientationLib_SOURCE_DIR}/SpaceGroupOps/OrthoRhombicOps.cpp
  ${OrientationLib_SOURCE_DIR}/SpaceGroupOps/TrigonalOps.cpp
  ${OrientationLib_SOURCE_DIR}/SpaceGroupOps/TrigonalLowOps.cpp
  ${OrientationLib_SOURCE_DIR}/SpaceGroupOps/TetragonalOps.cpp
  ${OrientationLib_SOURCE_DIR}/SpaceGroupOps/TetragonalLowOps.cpp
  ${OrientationLib_SOURCE_DIR}/SpaceGroupOps/TriclinicOps.cpp
  ${OrientationLib_SOURCE_DIR}/SpaceGroupOps/MonoclinicOps.cpp
  ${OrientationLib_SOURCE_DIR}/SpaceGroupOps/SO3Sampler.cpp
)
cmp_IDE_SOURCE_PROPERTIES( "SpaceGroupOps" "${OrientationLib_SpaceGroupOps_HDRS}" "${OrientationLib_SpaceGroupOps_SRCS}" "0")
if( ${PROJECT_INSTALL_HEADERS} EQUAL 1 )
    INSTALL (FILES ${OrientationLib_SpaceGroupOps_HDRS}
            DESTINATION include/OrientationLib/SpaceGroupOps
            COMPONENT Headers   )
endif()
