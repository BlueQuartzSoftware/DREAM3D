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

set(SIMPLib_Utilities_HDRS
  ${SIMPLib_SOURCE_DIR}/Utilities/ColorTable.h
  ${SIMPLib_SOURCE_DIR}/Utilities/ColorUtilities.h
  ${SIMPLib_SOURCE_DIR}/Utilities/TimeUtilities.h
  ${SIMPLib_SOURCE_DIR}/Utilities/SIMPLibEndian.h
  ${SIMPLib_SOURCE_DIR}/Utilities/SIMPLibRandom.h
  ${SIMPLib_SOURCE_DIR}/Utilities/FilePathGenerator.h
  ${SIMPLib_SOURCE_DIR}/Utilities/QMetaObjectUtilities.h
  ${SIMPLib_SOURCE_DIR}/Utilities/UnitTestSupport.hpp
  ${SIMPLib_SOURCE_DIR}/Utilities/TestObserver.h
)

set(SIMPLib_Utilities_SRCS
  ${SIMPLib_SOURCE_DIR}/Utilities/ColorTable.cpp
  ${SIMPLib_SOURCE_DIR}/Utilities/ColorUtilities.cpp
  ${SIMPLib_SOURCE_DIR}/Utilities/SIMPLibRandom.cpp
  ${SIMPLib_SOURCE_DIR}/Utilities/FilePathGenerator.cpp
  ${SIMPLib_SOURCE_DIR}/Utilities/QMetaObjectUtilities.cpp
  ${SIMPLib_SOURCE_DIR}/Utilities/TestObserver.cpp
)

cmp_IDE_SOURCE_PROPERTIES( "SIMPLibLib/Utilities" "${SIMPLib_Utilities_HDRS}" "${SIMPLib_Utilities_SRCS}" "0")
if( ${PROJECT_INSTALL_HEADERS} EQUAL 1 )
    INSTALL (FILES ${SIMPLib_Utilities_HDRS}
            DESTINATION include/SIMPLib/Utilities
            COMPONENT Headers   )
endif()

