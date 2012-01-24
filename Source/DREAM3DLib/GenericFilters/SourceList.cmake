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
    ${DREAM3DLib_SOURCE_DIR}/GenericFilters/DataContainerReader.h
    ${DREAM3DLib_SOURCE_DIR}/GenericFilters/DataContainerWriter.h
    ${DREAM3DLib_SOURCE_DIR}/GenericFilters/FindNeighbors.h
    ${DREAM3DLib_SOURCE_DIR}/GenericFilters/FieldDataCSVWriter.h
    ${DREAM3DLib_SOURCE_DIR}/GenericFilters/RenumberGrains.h
)

#-- Add in any addition NON Filter classes here
SET (DREAM3DLib_GenericFilters_HDRS
    ${GenericFilters_FILTERS_HDRS}
)

SET (DREAM3DLib_GenericFilters_SRCS
    ${DREAM3DLib_SOURCE_DIR}/GenericFilters/DataContainerReader.cpp
    ${DREAM3DLib_SOURCE_DIR}/GenericFilters/DataContainerWriter.cpp
    ${DREAM3DLib_SOURCE_DIR}/GenericFilters/FindNeighbors.cpp
    ${DREAM3DLib_SOURCE_DIR}/GenericFilters/FieldDataCSVWriter.cpp
    ${DREAM3DLib_SOURCE_DIR}/GenericFilters/RenumberGrains.cpp
)



cmp_IDE_SOURCE_PROPERTIES( "DREAM3DLib/GenericFilters" "${DREAM3DLib_GenericFilters_HDRS}" "${DREAM3DLib_GenericFilters_SRCS}" "0")
if ( ${PROJECT_INSTALL_HEADERS} EQUAL 1 )
    INSTALL (FILES ${DREAM3DLib_GenericFilters_HDRS}
            DESTINATION include/DREAM3D/GenericFilters
            COMPONENT Headers   )
endif()
