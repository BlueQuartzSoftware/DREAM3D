#--////////////////////////////////////////////////////////////////////////////
#--
#--  Copyright (c) 2009, 2010, 2011 Michael A. Jackson. BlueQuartz Software
#--  Copyright (c) 2009, 2010, 2011 Michael Groeber, US Air Force Research Laboratory
#--  All rights reserved.
#--  BSD License: http://www.opensource.org/licenses/bsd-license.html
#--
#--////////////////////////////////////////////////////////////////////////////

#///////////////////////////////////////////////////////////////////////////////
#// This code was partly written under US Air Force Contract FA8650-07-D-5800
#///////////////////////////////////////////////////////////////////////////////

#message(STATUS "MXA/Utilities/SourceList.cmake")
set(EbsdLib_Utilities_SRCS
  ${EbsdLib_SOURCE_DIR}/Utilities/MXADir.cpp
  ${EbsdLib_SOURCE_DIR}/Utilities/MXAFileInfo.cpp
 # ${EbsdLib_SOURCE_DIR}/Utilities/MXALogger.cpp
)

set(EbsdLib_Utilities_HDRS
  ${EbsdLib_SOURCE_DIR}/Utilities/StringUtils.h
  ${EbsdLib_SOURCE_DIR}/Utilities/MXADir.h
  ${EbsdLib_SOURCE_DIR}/Utilities/MXAFileInfo.h
#  ${EbsdLib_SOURCE_DIR}/Utilities/MXALogger.h
)

if(MSVC)
    set(EbsdLib_Utilities_SRCS ${EbsdLib_Utilities_SRCS} ${EbsdLib_SOURCE_DIR}/Utilities/MXADirent.c )
    set(EbsdLib_Utilities_HDRS ${EbsdLib_Utilities_HDRS} ${EbsdLib_SOURCE_DIR}/Utilities/MXADirent.h )
endif()
cmp_IDE_SOURCE_PROPERTIES( "EbsdLib/Utilities" "${EbsdLib_Utilities_HDRS}" "${EbsdLib_Utilities_SRCS}" "0")

if( ${EbsdLib_INSTALL_FILES} EQUAL 1 )
    INSTALL (FILES ${EbsdLib_Utilities_HDRS}
            DESTINATION include/EbsdLib/Utilities
            COMPONENT Headers   )
endif()

