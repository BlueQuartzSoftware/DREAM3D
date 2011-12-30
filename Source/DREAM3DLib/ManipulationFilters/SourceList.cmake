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

SET (ManipulationFilters_FILTERS_HDRS
    ${DREAM3DLib_SOURCE_DIR}/ManipulationFilters/ChangeResolution.h
    ${DREAM3DLib_SOURCE_DIR}/ManipulationFilters/CropVolume.h
)
#-- Add in any addition NON Filter classes here
SET (DREAM3DLib_ManipulationFilters_HDRS
    ${ManipulationFilters_FILTERS_HDRS}
)

SET (DREAM3DLib_ManipulationFilters_SRCS
    ${DREAM3DLib_SOURCE_DIR}/ManipulationFilters/ChangeResolution.cpp
    ${DREAM3DLib_SOURCE_DIR}/ManipulationFilters/CropVolume.cpp
)
cmp_IDE_SOURCE_PROPERTIES( "DREAM3DLib/ManipulationFilters" "${DREAM3DLib_ManipulationFilters_HDRS}" "${DREAM3DLib_ManipulationFilters_SRCS}" "0")
if ( ${PROJECT_INSTALL_HEADERS} EQUAL 1 )
    INSTALL (FILES ${DREAM3DLib_ManipulationFilters_HDRS}
            DESTINATION include/DREAM3D/ManipulationFilters
            COMPONENT Headers   )
endif()
