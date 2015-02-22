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



set(OrientationLib_Math_HDRS
  ${OrientationLib_SOURCE_DIR}/Math/OrientationMath.h
#  ${OrientationLib_SOURCE_DIR}/Math/RotationTransforms.h
#  ${OrientationLib_SOURCE_DIR}/Math/RotArray.hpp
)

set(OrientationLib_Math_SRCS
  ${OrientationLib_SOURCE_DIR}/Math/OrientationMath.cpp
#  ${OrientationLib_SOURCE_DIR}/Math/RotationTransforms.cpp
)
cmp_IDE_SOURCE_PROPERTIES( "OrientationLib/Math" "${OrientationLib_Math_HDRS}" "${OrientationLib_Math_SRCS}" "0")
if( ${PROJECT_INSTALL_HEADERS} EQUAL 1 )
    INSTALL (FILES ${OrientationLib_Math_HDRS}
            DESTINATION include/OrientationLib/Math
            COMPONENT Headers   )
endif()
