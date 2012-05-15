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

SET (GenericFilters_FILTERS_HDRS
    ${DREAM3DLib_SOURCE_DIR}/GenericFilters/DetermineGoodVoxels.h
    ${DREAM3DLib_SOURCE_DIR}/GenericFilters/FindNeighbors.h
    ${DREAM3DLib_SOURCE_DIR}/GenericFilters/FindGrainPhases.h
    ${DREAM3DLib_SOURCE_DIR}/GenericFilters/FindSurfaceCells.h
    ${DREAM3DLib_SOURCE_DIR}/GenericFilters/FindCellQuats.h
    ${DREAM3DLib_SOURCE_DIR}/GenericFilters/FindGrainCentroids.h
    ${DREAM3DLib_SOURCE_DIR}/GenericFilters/FindSurfaceGrains.h
    ${DREAM3DLib_SOURCE_DIR}/GenericFilters/FindBoundingBoxGrains.h
)

#-- Add in any additional NON Filter classes or filters that will NOT show up in the GUI filter list
SET (DREAM3DLib_GenericFilters_HDRS
    ${GenericFilters_FILTERS_HDRS}
    ${DREAM3DLib_SOURCE_DIR}/GenericFilters/RenumberGrains.h
)

SET (DREAM3DLib_GenericFilters_SRCS
    ${DREAM3DLib_SOURCE_DIR}/GenericFilters/DetermineGoodVoxels.cpp
    ${DREAM3DLib_SOURCE_DIR}/GenericFilters/FindNeighbors.cpp    
    ${DREAM3DLib_SOURCE_DIR}/GenericFilters/FindGrainPhases.cpp
    ${DREAM3DLib_SOURCE_DIR}/GenericFilters/FindCellQuats.cpp
    ${DREAM3DLib_SOURCE_DIR}/GenericFilters/FindSurfaceCells.cpp
    ${DREAM3DLib_SOURCE_DIR}/GenericFilters/FindGrainCentroids.cpp
    ${DREAM3DLib_SOURCE_DIR}/GenericFilters/RenumberGrains.cpp
    ${DREAM3DLib_SOURCE_DIR}/GenericFilters/FindSurfaceGrains.cpp
    ${DREAM3DLib_SOURCE_DIR}/GenericFilters/FindBoundingBoxGrains.cpp
)


if (NOT DEFINED ONLY_FILTERS)
cmp_IDE_SOURCE_PROPERTIES( "DREAM3DLib/GenericFilters" "${DREAM3DLib_GenericFilters_HDRS}" "${DREAM3DLib_GenericFilters_SRCS}" "0")
endif()

if ( ${PROJECT_INSTALL_HEADERS} EQUAL 1 )
    INSTALL (FILES ${DREAM3DLib_GenericFilters_HDRS}
            DESTINATION include/DREAM3D/GenericFilters
            COMPONENT Headers   )
endif()
