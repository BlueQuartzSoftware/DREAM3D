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

set(SIMPLib_StatsData_HDRS
  ${SIMPLib_SOURCE_DIR}/StatsData/BoundaryStatsData.h
  ${SIMPLib_SOURCE_DIR}/StatsData/MatrixStatsData.h
  ${SIMPLib_SOURCE_DIR}/StatsData/PrecipitateStatsData.h
  ${SIMPLib_SOURCE_DIR}/StatsData/PrimaryStatsData.h
  ${SIMPLib_SOURCE_DIR}/StatsData/StatsData.h
  ${SIMPLib_SOURCE_DIR}/StatsData/TransformationStatsData.h
)

set(SIMPLib_StatsData_SRCS
  ${SIMPLib_SOURCE_DIR}/StatsData/BoundaryStatsData.cpp
  ${SIMPLib_SOURCE_DIR}/StatsData/MatrixStatsData.cpp
  ${SIMPLib_SOURCE_DIR}/StatsData/PrecipitateStatsData.cpp
  ${SIMPLib_SOURCE_DIR}/StatsData/PrimaryStatsData.cpp
  ${SIMPLib_SOURCE_DIR}/StatsData/StatsData.cpp
  ${SIMPLib_SOURCE_DIR}/StatsData/TransformationStatsData.cpp
)
cmp_IDE_SOURCE_PROPERTIES( "SIMPLib/StatsData" "${SIMPLib_StatsData_HDRS}" "${SIMPLib_StatsData_SRCS}" "0")
if( ${PROJECT_INSTALL_HEADERS} EQUAL 1 )
    INSTALL (FILES ${SIMPLib_StatsData_HDRS}
            DESTINATION include/DREAM3D/StatsData
            COMPONENT Headers   )
endif()
