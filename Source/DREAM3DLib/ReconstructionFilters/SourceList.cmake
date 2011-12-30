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
SET (ReconstructionFilters_FILTERS_HDRS
    ${DREAM3DLib_SOURCE_DIR}/ReconstructionFilters/LoadSlices.h
    ${DREAM3DLib_SOURCE_DIR}/ReconstructionFilters/AlignSections.h
    ${DREAM3DLib_SOURCE_DIR}/ReconstructionFilters/SegmentGrains.h
    ${DREAM3DLib_SOURCE_DIR}/ReconstructionFilters/CleanupGrains.h
    ${DREAM3DLib_SOURCE_DIR}/ReconstructionFilters/MergeTwins.h
    ${DREAM3DLib_SOURCE_DIR}/ReconstructionFilters/MergeColonies.h
)
#-- Add in any addition NON Filter classes here
SET (DREAM3DLib_ReconstructionFilters_HDRS
    ${ReconstructionFilters_FILTERS_HDRS}
)
SET (DREAM3DLib_ReconstructionFilters_SRCS
    ${DREAM3DLib_SOURCE_DIR}/ReconstructionFilters/LoadSlices.cpp
    ${DREAM3DLib_SOURCE_DIR}/ReconstructionFilters/AlignSections.cpp
    ${DREAM3DLib_SOURCE_DIR}/ReconstructionFilters/SegmentGrains.cpp
    ${DREAM3DLib_SOURCE_DIR}/ReconstructionFilters/CleanupGrains.cpp
    ${DREAM3DLib_SOURCE_DIR}/ReconstructionFilters/MergeTwins.cpp
    ${DREAM3DLib_SOURCE_DIR}/ReconstructionFilters/MergeColonies.cpp


)
cmp_IDE_SOURCE_PROPERTIES( "DREAM3DLib/ReconstructionFilters" "${DREAM3DLib_ReconstructionFilters_HDRS}" "${DREAM3DLib_ReconstructionFilters_SRCS}" "0")
if ( ${PROJECT_INSTALL_HEADERS} EQUAL 1 )
    INSTALL (FILES ${DREAM3DLib_ReconstructionFilters_HDRS}
            DESTINATION include/DREAM3D/ReconstructionFilters
            COMPONENT Headers   )
endif()
