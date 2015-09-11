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



set(SIMPLib_Math_HDRS
  ${SIMPLib_SOURCE_DIR}/Math/GeometryMath.h
  ${SIMPLib_SOURCE_DIR}/Math/MatrixMath.h
  ${SIMPLib_SOURCE_DIR}/Math/QuaternionMath.hpp
  ${SIMPLib_SOURCE_DIR}/Math/SIMPLibMath.h
  ${SIMPLib_SOURCE_DIR}/Math/RadialDistributionFunction.h
)
set(SIMPLib_Math_SRCS
  ${SIMPLib_SOURCE_DIR}/Math/GeometryMath.cpp
  ${SIMPLib_SOURCE_DIR}/Math/MatrixMath.cpp
  ${SIMPLib_SOURCE_DIR}/Math/SIMPLibMath.cpp
  ${SIMPLib_SOURCE_DIR}/Math/RadialDistributionFunction.cpp
)
cmp_IDE_SOURCE_PROPERTIES( "SIMPLibLib/Math" "${SIMPLib_Math_HDRS}" "${SIMPLib_Math_SRCS}" "0")
if( ${PROJECT_INSTALL_HEADERS} EQUAL 1 )
    INSTALL (FILES ${SIMPLib_Math_HDRS}
            DESTINATION include/SIMPLib/Math
            COMPONENT Headers   )
endif()
