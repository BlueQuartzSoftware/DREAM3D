#--////////////////////////////////////////////////////////////////////////////
#--
#--  Copyright (c) 2011, Michael A. Jackson. BlueQuartz Software
#--  Copyright (c) 2011, Michael Groeber, US Air Force Research Laboratory
#--  All rights reserved.
#--  BSD License: http://www.opensource.org/licenses/bsd-license.html
#--
#-- This code was partly written under US Air Force Contract FA8650-10-D-5210
#--
#--////////////////////////////////////////////////////////////////////////////


set(OrientationLib_IO_HDRS
  ${OrientationLib_SOURCE_DIR}/IO/AngleFileLoader.h
)

set(OrientationLib_IO_SRCS
  ${OrientationLib_SOURCE_DIR}/IO/AngleFileLoader.cpp
)

cmp_IDE_SOURCE_PROPERTIES( "IO" "${OrientationLib_IO_HDRS}" "${OrientationLib_IO_SRCS}" "0")
if( ${PROJECT_INSTALL_HEADERS} EQUAL 1 )
    INSTALL (FILES ${OrientationLib_IO_HDRS}
            DESTINATION include/OrientationLib/IO
            COMPONENT Headers   )
endif()
