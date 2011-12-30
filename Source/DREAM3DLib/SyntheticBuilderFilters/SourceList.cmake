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

SET (SyntheticBuilderFilters_FILTERS_HDRS
    ${DREAM3DLib_SOURCE_DIR}/SyntheticBuilderFilters/AdjustVolume.h    
    ${DREAM3DLib_SOURCE_DIR}/SyntheticBuilderFilters/PackGrainsGen2.h
    ${DREAM3DLib_SOURCE_DIR}/SyntheticBuilderFilters/MatchCrystallography.h
    ${DREAM3DLib_SOURCE_DIR}/SyntheticBuilderFilters/PlacePrecipitates.h
)

SET (DREAM3DLib_SyntheticBuilderFilters_HDRS
    ${SyntheticBuilderFilters_FILTERS_HDRS}
)
SET (DREAM3DLib_SyntheticBuilderFilters_SRCS
    ${DREAM3DLib_SOURCE_DIR}/SyntheticBuilderFilters/AdjustVolume.cpp
    ${DREAM3DLib_SOURCE_DIR}/SyntheticBuilderFilters/PackGrainsGen2.cpp
    ${DREAM3DLib_SOURCE_DIR}/SyntheticBuilderFilters/MatchCrystallography.cpp
    ${DREAM3DLib_SOURCE_DIR}/SyntheticBuilderFilters/PlacePrecipitates.cpp

)
cmp_IDE_SOURCE_PROPERTIES( "DREAM3DLib/SyntheticBuilderFilters" "${DREAM3DLib_SyntheticBuilderFilters_HDRS}" "${DREAM3DLib_SyntheticBuilderFilters_SRCS}" "0")
if ( ${PROJECT_INSTALL_HEADERS} EQUAL 1 )
    INSTALL (FILES ${DREAM3DLib_SyntheticBuilderFilters_HDRS}
            DESTINATION include/DREAM3D/SyntheticBuilderFilters
            COMPONENT Headers   )
endif()
