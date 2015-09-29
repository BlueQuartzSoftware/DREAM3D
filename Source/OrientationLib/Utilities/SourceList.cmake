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

set(OrientationLib_Utilities_HDRS
  ${OrientationLib_SOURCE_DIR}/Utilities/PoleFigureUtilities.h
  ${OrientationLib_SOURCE_DIR}/Utilities/ModifiedLambertProjection.h
  ${OrientationLib_SOURCE_DIR}/Utilities/ModifiedLambertProjectionArray.h
  ${OrientationLib_SOURCE_DIR}/Utilities/ModifiedLambertProjection3D.hpp
  ${OrientationLib_SOURCE_DIR}/Utilities/OrientationMathHelpers.hpp
  )

set(OrientationLib_Utilities_SRCS
  ${OrientationLib_SOURCE_DIR}/Utilities/PoleFigureUtilities.cpp
  ${OrientationLib_SOURCE_DIR}/Utilities/ModifiedLambertProjection.cpp
  ${OrientationLib_SOURCE_DIR}/Utilities/ModifiedLambertProjectionArray.cpp
)

cmp_IDE_SOURCE_PROPERTIES( "Utilities" "${OrientationLib_Utilities_HDRS}" "${OrientationLib_Utilities_SRCS}" "0")
if( ${PROJECT_INSTALL_HEADERS} EQUAL 1 )
    INSTALL (FILES ${OrientationLib_Utilities_HDRS}
            DESTINATION include/OrientationLib/Utilities
            COMPONENT Headers   )
endif()

set(OrientationLib_Utilities_HDRS ${OrientationLib_Utilities_HDRS} ${OrientationLib_Utilities_IO_HDRS})
set(OrientationLib_Utilities_SRCS ${OrientationLib_Utilities_SRCS} ${OrientationLib_Utilities_IO_SRCS})
