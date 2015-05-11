#--////////////////////////////////////////////////////////////////////////////
#--
#--  Copyright (c) 2009, 2010, 2011 Michael A. Jackson. BlueQuartz Software
#--  Copyright (c) 2009, 2010, 2011 Michael Groeber, US Air Force Research Laboratory
#--  All rights reserved.
#--  BSD License: http://www.opensource.org/licenses/bsd-license.html
#--
#--////////////////////////////////////////////////////////////////////////////

#///////////////////////////////////////////////////////////////////////////////
#// This code was partly written under US Air Force Contract FA8650-07-D-5800
#///////////////////////////////////////////////////////////////////////////////

#-- Get the HEDM Sources
set(HEDM_SRCS
    ${EbsdLib_SOURCE_DIR}/HEDM/MicReader.cpp
    ${EbsdLib_SOURCE_DIR}/HEDM/MicPhase.cpp
    ${EbsdLib_SOURCE_DIR}/HEDM/MicFields.cpp
    )

set(HEDM_HDRS
    ${EbsdLib_SOURCE_DIR}/HEDM/MicConstants.h
    ${EbsdLib_SOURCE_DIR}/HEDM/MicHeaderEntry.h
    ${EbsdLib_SOURCE_DIR}/HEDM/MicReader.h
    ${EbsdLib_SOURCE_DIR}/HEDM/MicPhase.h
    ${EbsdLib_SOURCE_DIR}/HEDM/MicFields.h
)


if(EbsdLib_ENABLE_HDF5)
    add_definitions(-DEbsdLib_HAVE_HDF5)
    set(HEDM_SRCS ${HEDM_SRCS}
        ${EbsdLib_SOURCE_DIR}/HEDM/H5MicImporter.cpp
        ${EbsdLib_SOURCE_DIR}/HEDM/H5MicReader.cpp
        ${EbsdLib_SOURCE_DIR}/HEDM/H5MicVolumeReader.cpp
    )
    set(HEDM_HDRS ${HEDM_HDRS}
        ${EbsdLib_SOURCE_DIR}/HEDM/H5MicImporter.h
        ${EbsdLib_SOURCE_DIR}/HEDM/H5MicReader.h
        ${EbsdLib_SOURCE_DIR}/HEDM/H5MicVolumeReader.h
    )
    set(EbsdLib_HDF5_SUPPORT 1)
endif()


cmp_IDE_SOURCE_PROPERTIES( "EbsdLib/HEDM" "${HEDM_HDRS}" "${HEDM_SRCS}" ${PROJECT_INSTALL_HEADERS})

if( ${EbsdLib_INSTALL_FILES} EQUAL 1 )
    INSTALL (FILES ${HEDM_HDRS}
            DESTINATION include/EbsdLib/HEDM
            COMPONENT Headers   )
endif()

