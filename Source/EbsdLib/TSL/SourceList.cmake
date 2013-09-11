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

#-- Get the TSL Sources
set(TSL_SRCS
    ${EbsdLib_SOURCE_DIR}/TSL/AngReader.cpp
    ${EbsdLib_SOURCE_DIR}/TSL/AngPhase.cpp
    ${EbsdLib_SOURCE_DIR}/TSL/AngFields.cpp
    )

set(TSL_HDRS
    ${EbsdLib_SOURCE_DIR}/TSL/AngConstants.h
    ${EbsdLib_SOURCE_DIR}/TSL/AngHeaderEntry.h
    ${EbsdLib_SOURCE_DIR}/TSL/AngReader.h
    ${EbsdLib_SOURCE_DIR}/TSL/AngPhase.h
    ${EbsdLib_SOURCE_DIR}/TSL/AngFields.h
)

if(EbsdLib_ENABLE_HDF5)
    add_definitions(-DEbsdLib_HAVE_HDF5)
    set(TSL_SRCS ${TSL_SRCS}
        ${EbsdLib_SOURCE_DIR}/TSL/H5AngImporter.cpp
        ${EbsdLib_SOURCE_DIR}/TSL/H5AngReader.cpp
        ${EbsdLib_SOURCE_DIR}/TSL/H5AngVolumeReader.cpp
    )
    set(TSL_HDRS ${TSL_HDRS}
        ${EbsdLib_SOURCE_DIR}/TSL/H5AngImporter.h
        ${EbsdLib_SOURCE_DIR}/TSL/H5AngReader.h
        ${EbsdLib_SOURCE_DIR}/TSL/H5AngVolumeReader.h
    )
    set(EbsdLib_HDF5_SUPPORT 1)
endif()
cmp_IDE_SOURCE_PROPERTIES( "EbsdLib/TSL" "${TSL_HDRS}" "${TSL_SRCS}" ${PROJECT_INSTALL_HEADERS})

if( ${EbsdLib_INSTALL_FILES} EQUAL 1 )
    INSTALL (FILES ${TSL_HDRS}
            DESTINATION include/EbsdLib/TSL
            COMPONENT Headers   )
endif()

