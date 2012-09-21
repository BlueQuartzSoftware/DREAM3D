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
    ${DREAM3DLib_SOURCE_DIR}/ReconstructionFilters/AlignSectionsMisorientation.h
    ${DREAM3DLib_SOURCE_DIR}/ReconstructionFilters/AlignSectionsMutualInformation.h
    ${DREAM3DLib_SOURCE_DIR}/ReconstructionFilters/AlignSectionsFeature.h
    ${DREAM3DLib_SOURCE_DIR}/ReconstructionFilters/AlignSectionsFeatureCentroid.h
    ${DREAM3DLib_SOURCE_DIR}/ReconstructionFilters/AlignSectionsList.h
    ${DREAM3DLib_SOURCE_DIR}/ReconstructionFilters/EBSDSegmentGrains.h
    ${DREAM3DLib_SOURCE_DIR}/ReconstructionFilters/CAxisSegmentGrains.h
    ${DREAM3DLib_SOURCE_DIR}/ReconstructionFilters/MergeTwins.h
    ${DREAM3DLib_SOURCE_DIR}/ReconstructionFilters/MergeColonies.h
    ${DREAM3DLib_SOURCE_DIR}/ReconstructionFilters/GroupMicroTextureRegions.h
)
#-- Add in any additional NON Filter classes or filters that will NOT show up in the GUI filter list
SET (DREAM3DLib_ReconstructionFilters_HDRS
    ${ReconstructionFilters_FILTERS_HDRS}
    ${DREAM3DLib_SOURCE_DIR}/ReconstructionFilters/SegmentGrains.h
    ${DREAM3DLib_SOURCE_DIR}/ReconstructionFilters/AlignSections.h
)
SET (DREAM3DLib_ReconstructionFilters_SRCS
    ${DREAM3DLib_SOURCE_DIR}/ReconstructionFilters/AlignSections.cpp
    ${DREAM3DLib_SOURCE_DIR}/ReconstructionFilters/AlignSectionsMisorientation.cpp
    ${DREAM3DLib_SOURCE_DIR}/ReconstructionFilters/AlignSectionsMutualInformation.cpp
    ${DREAM3DLib_SOURCE_DIR}/ReconstructionFilters/AlignSectionsFeature.cpp    
    ${DREAM3DLib_SOURCE_DIR}/ReconstructionFilters/AlignSectionsFeatureCentroid.cpp    
    ${DREAM3DLib_SOURCE_DIR}/ReconstructionFilters/AlignSectionsList.cpp    
    ${DREAM3DLib_SOURCE_DIR}/ReconstructionFilters/SegmentGrains.cpp    
    ${DREAM3DLib_SOURCE_DIR}/ReconstructionFilters/EBSDSegmentGrains.cpp     
    ${DREAM3DLib_SOURCE_DIR}/ReconstructionFilters/CAxisSegmentGrains.cpp
    ${DREAM3DLib_SOURCE_DIR}/ReconstructionFilters/MergeTwins.cpp 
    ${DREAM3DLib_SOURCE_DIR}/ReconstructionFilters/MergeColonies.cpp  
    ${DREAM3DLib_SOURCE_DIR}/ReconstructionFilters/GroupMicroTextureRegions.cpp
)
if (NOT DEFINED ONLY_FILTERS)
cmp_IDE_SOURCE_PROPERTIES( "DREAM3DLib/ReconstructionFilters" "${DREAM3DLib_ReconstructionFilters_HDRS}" "${DREAM3DLib_ReconstructionFilters_SRCS}" "0")
endif()
if ( ${PROJECT_INSTALL_HEADERS} EQUAL 1 )
    INSTALL (FILES ${DREAM3DLib_ReconstructionFilters_HDRS}
            DESTINATION include/DREAM3D/ReconstructionFilters
            COMPONENT Headers   )
endif()
