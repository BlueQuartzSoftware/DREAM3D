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

set(DREAM3DLib_DataArrays_HDRS
  ${DREAM3DLib_SOURCE_DIR}/DataArrays/DataArray.hpp
  ${DREAM3DLib_SOURCE_DIR}/DataArrays/IDataArray.h
  ${DREAM3DLib_SOURCE_DIR}/DataArrays/ManagedArrayOfArrays.hpp
  ${DREAM3DLib_SOURCE_DIR}/DataArrays/StatsDataArray.h
  ${DREAM3DLib_SOURCE_DIR}/DataArrays/StringDataArray.hpp
  ${DREAM3DLib_SOURCE_DIR}/DataArrays/StructArray.hpp
)

set(DREAM3DLib_DataArrays_SRCS
  ${DREAM3DLib_SOURCE_DIR}/DataArrays/IDataArray.cpp
  ${DREAM3DLib_SOURCE_DIR}/DataArrays/StatsDataArray.cpp
)
cmp_IDE_SOURCE_PROPERTIES( "DREAM3DLib/DataArrays" "${DREAM3DLib_DataArrays_HDRS}" "${DREAM3DLib_DataArrays_SRCS}" "0")
if( ${PROJECT_INSTALL_HEADERS} EQUAL 1 )
    INSTALL (FILES ${DREAM3DLib_DataArrays_HDRS}
            DESTINATION include/DREAM3D/DataArrays
            COMPONENT Headers   )
endif()
