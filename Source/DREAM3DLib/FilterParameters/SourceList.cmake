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

set(DREAM3DLib_FilterParameters_HDRS
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/AbstractFilterParametersReader.h
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/AbstractFilterParametersWriter.h
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/FilterParameter.h
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/H5FilterParametersConstants.h
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/H5FilterParametersReader.h
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/H5FilterParametersWriter.h
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/TextFilterParametersWriter.h
)

set(DREAM3DLib_FilterParameters_SRCS
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/AbstractFilterParametersReader.cpp
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/AbstractFilterParametersWriter.cpp
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/H5FilterParametersReader.cpp
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/H5FilterParametersWriter.cpp
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/TextFilterParametersWriter.cpp
)
cmp_IDE_SOURCE_PROPERTIES( "DREAM3DLib/FilterParameters" "${DREAM3DLib_FilterParameters_HDRS}" "${DREAM3DLib_FilterParameters_SRCS}" "0")
if( ${PROJECT_INSTALL_HEADERS} EQUAL 1 )
    INSTALL (FILES ${DREAM3DLib_FilterParameters_HDRS}
            DESTINATION include/DREAM3D/FilterParameters
            COMPONENT Headers   )
endif()
