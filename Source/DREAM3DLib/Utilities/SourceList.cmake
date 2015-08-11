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

set(DREAM3DLib_Utilities_HDRS
  ${DREAM3DLib_SOURCE_DIR}/Utilities/ColorTable.h
  ${DREAM3DLib_SOURCE_DIR}/Utilities/ColorUtilities.h
  ${DREAM3DLib_SOURCE_DIR}/Utilities/TimeUtilities.h
  ${DREAM3DLib_SOURCE_DIR}/Utilities/DREAM3DEndian.h
  ${DREAM3DLib_SOURCE_DIR}/Utilities/DREAM3DRandom.h
  ${DREAM3DLib_SOURCE_DIR}/Utilities/FilePathGenerator.h
  ${DREAM3DLib_SOURCE_DIR}/Utilities/QMetaObjectUtilities.h
  ${DREAM3DLib_SOURCE_DIR}/Utilities/UnitTestSupport.hpp
  ${DREAM3DLib_SOURCE_DIR}/Utilities/TestObserver.h
)

set(DREAM3DLib_Utilities_SRCS
  ${DREAM3DLib_SOURCE_DIR}/Utilities/ColorTable.cpp
  ${DREAM3DLib_SOURCE_DIR}/Utilities/ColorUtilities.cpp
  ${DREAM3DLib_SOURCE_DIR}/Utilities/DREAM3DRandom.cpp
  ${DREAM3DLib_SOURCE_DIR}/Utilities/FilePathGenerator.cpp
  ${DREAM3DLib_SOURCE_DIR}/Utilities/QMetaObjectUtilities.cpp
  ${DREAM3DLib_SOURCE_DIR}/Utilities/TestObserver.cpp
)

cmp_IDE_SOURCE_PROPERTIES( "DREAM3DLib/Utilities" "${DREAM3DLib_Utilities_HDRS}" "${DREAM3DLib_Utilities_SRCS}" "0")
if( ${PROJECT_INSTALL_HEADERS} EQUAL 1 )
    INSTALL (FILES ${DREAM3DLib_Utilities_HDRS}
            DESTINATION include/DREAM3D/Utilities
            COMPONENT Headers   )
endif()

