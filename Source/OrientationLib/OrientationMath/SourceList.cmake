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



set(OrientationLib_OrientationMath_HDRS
  ${OrientationLib_SOURCE_DIR}/OrientationMath/OrientationMath.h
  ${OrientationLib_SOURCE_DIR}/OrientationMath/OrientationTransforms.hpp
  ${OrientationLib_SOURCE_DIR}/OrientationMath/OrientationArray.hpp
  ${OrientationLib_SOURCE_DIR}/OrientationMath/OrientationConverter.hpp
)

set(OrientationLib_OrientationMath_SRCS
  ${OrientationLib_SOURCE_DIR}/OrientationMath/OrientationMath.cpp
)
cmp_IDE_SOURCE_PROPERTIES( "OrientationMath" "${OrientationLib_OrientationMath_HDRS}" "${OrientationLib_OrientationMath_SRCS}" "0")
if( ${PROJECT_INSTALL_HEADERS} EQUAL 1 )
    INSTALL (FILES ${OrientationLib_OrientationMath_HDRS}
            DESTINATION include/OrientationLib/OrientationMath
            COMPONENT Headers   )
endif()
