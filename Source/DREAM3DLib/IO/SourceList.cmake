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

SET (DREAM3DLib_IO_HDRS
  ${DREAM3DLib_SOURCE_DIR}/IO/DxGrainIdWriter.h
  ${DREAM3DLib_SOURCE_DIR}/IO/PhWriter.hpp
)

SET (DREAM3DLib_IO_SRCS
  ${DREAM3DLib_SOURCE_DIR}/IO/DxGrainIdWriter.cpp
)
cmp_IDE_SOURCE_PROPERTIES( "DREAM3DLib/IO" "${DREAM3DLib_IO_HDRS}" "${DREAM3DLib_IO_SRCS}" "0")
if ( ${PROJECT_INSTALL_HEADERS} EQUAL 1 )
    INSTALL (FILES ${DREAM3DLib_IO_HDRS}
            DESTINATION include/DREAM3D/IO
            COMPONENT Headers   )
endif()

