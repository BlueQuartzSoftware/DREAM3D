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


set(DREAM3DLib_Utilities_IO_HDRS
  ${DREAM3DLib_SOURCE_DIR}/Utilities/IO/QFilterParametersWriter.h
  ${DREAM3DLib_SOURCE_DIR}/Utilities/IO/QFilterParametersReader.h
  ${DREAM3DLib_SOURCE_DIR}/Utilities/IO/AngleFileLoader.h
)

set(DREAM3DLib_Utilities_IO_SRCS
  ${DREAM3DLib_SOURCE_DIR}/Utilities/IO/QFilterParametersWriter.cpp
  ${DREAM3DLib_SOURCE_DIR}/Utilities/IO/QFilterParametersReader.cpp
  ${DREAM3DLib_SOURCE_DIR}/Utilities/IO/AngleFileLoader.cpp
)

cmp_IDE_SOURCE_PROPERTIES( "DREAM3DLib/Utilities/IO" "${DREAM3DLib_Utilities_IO_HDRS}" "${DREAM3DLib_Utilities_IO_SRCS}" "0")
if( ${PROJECT_INSTALL_HEADERS} EQUAL 1 )
    INSTALL (FILES ${DREAM3DLib_Utilities_IO_HDRS}
            DESTINATION include/DREAM3D/Utilities/IO
            COMPONENT Headers   )
endif()
