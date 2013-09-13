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



set(DREAM3DLib_Math_HDRS
  ${DREAM3DLib_SOURCE_DIR}/Math/OrientationMath.h
  ${DREAM3DLib_SOURCE_DIR}/Math/MatrixMath.h
  ${DREAM3DLib_SOURCE_DIR}/Math/QuaternionMath.hpp
  ${DREAM3DLib_SOURCE_DIR}/Math/DREAM3DMath.h
)
set(DREAM3DLib_Math_SRCS
  ${DREAM3DLib_SOURCE_DIR}/Math/OrientationMath.cpp
  ${DREAM3DLib_SOURCE_DIR}/Math/MatrixMath.cpp
  ${DREAM3DLib_SOURCE_DIR}/Math/DREAM3DMath.cpp  
)
cmp_IDE_SOURCE_PROPERTIES( "DREAM3DLib/Math" "${DREAM3DLib_Math_HDRS}" "${DREAM3DLib_Math_SRCS}" "0")
if( ${PROJECT_INSTALL_HEADERS} EQUAL 1 )
    INSTALL (FILES ${DREAM3DLib_Math_HDRS}
            DESTINATION include/DREAM3D/Math
            COMPONENT Headers   )
endif()
