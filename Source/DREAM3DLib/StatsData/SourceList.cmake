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

set(DREAM3DLib_StatsData_HDRS
  ${DREAM3DLib_SOURCE_DIR}/StatsData/BoundaryStatsData.h
  ${DREAM3DLib_SOURCE_DIR}/StatsData/MatrixStatsData.h
  ${DREAM3DLib_SOURCE_DIR}/StatsData/PrecipitateStatsData.h
  ${DREAM3DLib_SOURCE_DIR}/StatsData/PrimaryStatsData.h
  ${DREAM3DLib_SOURCE_DIR}/StatsData/StatsData.h
  ${DREAM3DLib_SOURCE_DIR}/StatsData/TransformationStatsData.h
)

set(DREAM3DLib_StatsData_SRCS
  ${DREAM3DLib_SOURCE_DIR}/StatsData/BoundaryStatsData.cpp
  ${DREAM3DLib_SOURCE_DIR}/StatsData/MatrixStatsData.cpp
  ${DREAM3DLib_SOURCE_DIR}/StatsData/PrecipitateStatsData.cpp
  ${DREAM3DLib_SOURCE_DIR}/StatsData/PrimaryStatsData.cpp
  ${DREAM3DLib_SOURCE_DIR}/StatsData/StatsData.cpp
  ${DREAM3DLib_SOURCE_DIR}/StatsData/TransformationStatsData.cpp
)
cmp_IDE_SOURCE_PROPERTIES( "DREAM3DLib/StatsData" "${DREAM3DLib_StatsData_HDRS}" "${DREAM3DLib_StatsData_SRCS}" "0")
if( ${PROJECT_INSTALL_HEADERS} EQUAL 1 )
    INSTALL (FILES ${DREAM3DLib_StatsData_HDRS}
            DESTINATION include/DREAM3D/StatsData
            COMPONENT Headers   )
endif()
