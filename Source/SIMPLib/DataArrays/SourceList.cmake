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

set(SIMPLib_DataArrays_HDRS
  ${SIMPLib_SOURCE_DIR}/DataArrays/DataArray.hpp
  ${SIMPLib_SOURCE_DIR}/DataArrays/IDataArray.h
  ${SIMPLib_SOURCE_DIR}/DataArrays/NeighborList.hpp
  ${SIMPLib_SOURCE_DIR}/DataArrays/StatsDataArray.h
  ${SIMPLib_SOURCE_DIR}/DataArrays/StringDataArray.hpp
  ${SIMPLib_SOURCE_DIR}/DataArrays/StructArray.hpp
  ${SIMPLib_SOURCE_DIR}/DataArrays/DynamicListArray.hpp
)

set(SIMPLib_DataArrays_SRCS
  ${SIMPLib_SOURCE_DIR}/DataArrays/IDataArray.cpp
  ${SIMPLib_SOURCE_DIR}/DataArrays/StatsDataArray.cpp
)
cmp_IDE_SOURCE_PROPERTIES( "DREAM3DLib/DataArrays" "${SIMPLib_DataArrays_HDRS}" "${SIMPLib_DataArrays_SRCS}" "0")
if( ${PROJECT_INSTALL_HEADERS} EQUAL 1 )
    INSTALL (FILES ${SIMPLib_DataArrays_HDRS}
            DESTINATION include/DREAM3D/DataArrays
            COMPONENT Headers   )
endif()
