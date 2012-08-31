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
SET (StatisticsFilters_FILTERS_HDRS
    ${DREAM3DLib_SOURCE_DIR}/StatisticsFilters/FindAvgOrientations.h
    ${DREAM3DLib_SOURCE_DIR}/StatisticsFilters/FindAxisODF.h
    ${DREAM3DLib_SOURCE_DIR}/StatisticsFilters/FindEuclideanDistMap.h
    ${DREAM3DLib_SOURCE_DIR}/StatisticsFilters/FindMDF.h
    ${DREAM3DLib_SOURCE_DIR}/StatisticsFilters/FindNeighborhoods.h
    ${DREAM3DLib_SOURCE_DIR}/StatisticsFilters/FindODF.h
    ${DREAM3DLib_SOURCE_DIR}/StatisticsFilters/FindNumFields.h
    ${DREAM3DLib_SOURCE_DIR}/StatisticsFilters/FindDeformationStatistics.h
    ${DREAM3DLib_SOURCE_DIR}/StatisticsFilters/FindSchmids.h
    ${DREAM3DLib_SOURCE_DIR}/StatisticsFilters/FindShapes.h
    ${DREAM3DLib_SOURCE_DIR}/StatisticsFilters/FindSizes.h
    ${DREAM3DLib_SOURCE_DIR}/StatisticsFilters/FindLocalMisorientationGradients.h
    ${DREAM3DLib_SOURCE_DIR}/StatisticsFilters/FindSlicetoSliceRotations.h
)

#-- Add in any additional NON Filter classes or filters that will NOT show up in the GUI filter list
SET (DREAM3DLib_StatisticsFilters_HDRS 
    ${StatisticsFilters_FILTERS_HDRS}
)
SET (DREAM3DLib_StatisticsFilters_SRCS
    ${DREAM3DLib_SOURCE_DIR}/StatisticsFilters/FindAvgOrientations.cpp
    ${DREAM3DLib_SOURCE_DIR}/StatisticsFilters/FindAxisODF.cpp
    ${DREAM3DLib_SOURCE_DIR}/StatisticsFilters/FindEuclideanDistMap.cpp
    ${DREAM3DLib_SOURCE_DIR}/StatisticsFilters/FindMDF.cpp
    ${DREAM3DLib_SOURCE_DIR}/StatisticsFilters/FindNeighborhoods.cpp
    ${DREAM3DLib_SOURCE_DIR}/StatisticsFilters/FindODF.cpp
    ${DREAM3DLib_SOURCE_DIR}/StatisticsFilters/FindNumFields.cpp
    ${DREAM3DLib_SOURCE_DIR}/StatisticsFilters/FindDeformationStatistics.cpp
    ${DREAM3DLib_SOURCE_DIR}/StatisticsFilters/FindSchmids.cpp
    ${DREAM3DLib_SOURCE_DIR}/StatisticsFilters/FindShapes.cpp
    ${DREAM3DLib_SOURCE_DIR}/StatisticsFilters/FindSizes.cpp
    ${DREAM3DLib_SOURCE_DIR}/StatisticsFilters/FindLocalMisorientationGradients.cpp
    ${DREAM3DLib_SOURCE_DIR}/StatisticsFilters/FindSlicetoSliceRotations.cpp
)
if (NOT DEFINED ONLY_FILTERS)
cmp_IDE_SOURCE_PROPERTIES( "DREAM3DLib/StatisticsFilters" "${DREAM3DLib_StatisticsFilters_HDRS}" "${DREAM3DLib_StatisticsFilters_SRCS}" "0")
endif()

if ( ${PROJECT_INSTALL_HEADERS} EQUAL 1 )
    INSTALL (FILES ${DREAM3DLib_StatisticsFilters_HDRS}
            DESTINATION include/DREAM3D/StatisticsFilters
            COMPONENT Headers   )
endif()


