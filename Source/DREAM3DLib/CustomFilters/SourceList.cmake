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

SET (CustomFilters_FILTERS_HDRS
        ${DREAM3DLib_SOURCE_DIR}/CustomFilters/CropVolumePipeline.h
)

#-- Add in any additional NON Filter classes or filters that will NOT show up in the GUI filter list
SET (DREAM3DLib_CustomFilters_HDRS
    ${CustomFilters_FILTERS_HDRS}

)
SET (DREAM3DLib_CustomFilters_SRCS
    ${DREAM3DLib_SOURCE_DIR}/CustomFilters/CropVolumePipeline.cpp

)
if (NOT DEFINED ONLY_FILTERS)
cmp_IDE_SOURCE_PROPERTIES( "DREAM3DLib/CustomFilters" "${DREAM3DLib_CustomFilters_HDRS}" "${DREAM3DLib_CustomFilters_SRCS}" "0")
endif()
if ( ${PROJECT_INSTALL_HEADERS} EQUAL 1 )
    INSTALL (FILES ${DREAM3DLib_CustomFilters_HDRS}
            DESTINATION include/DREAM3D/CustomFilters
            COMPONENT Headers   )
endif()
