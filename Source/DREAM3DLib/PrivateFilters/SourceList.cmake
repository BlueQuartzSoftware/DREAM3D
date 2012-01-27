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

SET (PrivateFilters_FILTERS_HDRS
    ${DREAM3DLib_SOURCE_DIR}/PrivateFilters/FindNeighbors.h
    ${DREAM3DLib_SOURCE_DIR}/PrivateFilters/FindGrainPhases.h
    ${DREAM3DLib_SOURCE_DIR}/PrivateFilters/RenumberGrains.h
)

#-- Add in any addition NON Filter classes here
SET (DREAM3DLib_PrivateFilters_HDRS
    ${PrivateFilters_FILTERS_HDRS}
)

SET (DREAM3DLib_PrivateFilters_SRCS
        ${DREAM3DLib_SOURCE_DIR}/PrivateFilters/FindNeighbors.cpp    
${DREAM3DLib_SOURCE_DIR}/PrivateFilters/FindGrainPhases.cpp
 ${DREAM3DLib_SOURCE_DIR}/PrivateFilters/RenumberGrains.cpp
)



cmp_IDE_SOURCE_PROPERTIES( "DREAM3DLib/PrivateFilters" "${DREAM3DLib_PrivateFilters_HDRS}" "${DREAM3DLib_PrivateFilters_SRCS}" "0")
if ( ${PROJECT_INSTALL_HEADERS} EQUAL 1 )
    INSTALL (FILES ${DREAM3DLib_PrivateFilters_HDRS}
            DESTINATION include/DREAM3D/PrivateFilters
            COMPONENT Headers   )
endif()
