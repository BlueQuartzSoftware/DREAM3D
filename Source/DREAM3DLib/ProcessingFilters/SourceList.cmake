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
SET (ProcessingFilters_FILTERS_HDRS
${DREAM3DLib_SOURCE_DIR}/ProcessingFilters/MinNeighbors.h
${DREAM3DLib_SOURCE_DIR}/ProcessingFilters/MinSize.h
${DREAM3DLib_SOURCE_DIR}/ProcessingFilters/FillBadData.h
${DREAM3DLib_SOURCE_DIR}/ProcessingFilters/OpenCloseBadData.h
${DREAM3DLib_SOURCE_DIR}/ProcessingFilters/IdentifySample.h
${DREAM3DLib_SOURCE_DIR}/GenericFilters/ConvertEulerAngles.h
)
#-- Add in any addition NON Filter classes here
SET (DREAM3DLib_ProcessingFilters_HDRS
    ${ProcessingFilters_FILTERS_HDRS}

)
SET (DREAM3DLib_ProcessingFilters_SRCS
    ${DREAM3DLib_SOURCE_DIR}/ProcessingFilters/MinNeighbors.cpp    
    ${DREAM3DLib_SOURCE_DIR}/ProcessingFilters/MinSize.cpp    
    ${DREAM3DLib_SOURCE_DIR}/ProcessingFilters/FillBadData.cpp    
    ${DREAM3DLib_SOURCE_DIR}/ProcessingFilters/OpenCloseBadData.cpp    
    ${DREAM3DLib_SOURCE_DIR}/ProcessingFilters/IdentifySample.cpp    
    ${DREAM3DLib_SOURCE_DIR}/GenericFilters/ConvertEulerAngles.cpp
)
if (NOT DEFINED ONLY_FILTERS)
cmp_IDE_SOURCE_PROPERTIES( "DREAM3DLib/ProcessingFilters" "${DREAM3DLib_ProcessingFilters_HDRS}" "${DREAM3DLib_ProcessingFilters_SRCS}" "0")
endif()
if ( ${PROJECT_INSTALL_HEADERS} EQUAL 1 )
    INSTALL (FILES ${DREAM3DLib_ProcessingFilters_HDRS}
            DESTINATION include/DREAM3D/ProcessingFilters
            COMPONENT Headers   )
endif()
