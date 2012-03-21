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
${DREAM3DLib_SOURCE_DIR}/ProcessingFilters/CleanupGrains.h
${DREAM3DLib_SOURCE_DIR}/ProcessingFilters/GrainDilation.h

)
#-- Add in any addition NON Filter classes here
SET (DREAM3DLib_ProcessingFilters_HDRS
    ${ProcessingFilters_FILTERS_HDRS}

)
SET (DREAM3DLib_ProcessingFilters_SRCS
    ${DREAM3DLib_SOURCE_DIR}/ProcessingFilters/CleanupGrains.cpp    
    ${DREAM3DLib_SOURCE_DIR}/ProcessingFilters/GrainDilation.cpp    

)
if (NOT DEFINED ONLY_FILTERS)
cmp_IDE_SOURCE_PROPERTIES( "DREAM3DLib/ProcessingFilters" "${DREAM3DLib_ProcessingFilters_HDRS}" "${DREAM3DLib_ProcessingFilters_SRCS}" "0")
endif()
if ( ${PROJECT_INSTALL_HEADERS} EQUAL 1 )
    INSTALL (FILES ${DREAM3DLib_ProcessingFilters_HDRS}
            DESTINATION include/DREAM3D/ProcessingFilters
            COMPONENT Headers   )
endif()
