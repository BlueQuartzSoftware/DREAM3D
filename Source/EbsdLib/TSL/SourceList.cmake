
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
    set(TSL_SRCS ${TSL_SRCS}
        ${EbsdLib_SOURCE_DIR}/TSL/H5AngImporter.cpp
        ${EbsdLib_SOURCE_DIR}/TSL/H5AngReader.cpp
        ${EbsdLib_SOURCE_DIR}/TSL/H5AngVolumeReader.cpp
        ${EbsdLib_SOURCE_DIR}/TSL/H5OIMReader.cpp
    )
    set(TSL_HDRS ${TSL_HDRS}
        ${EbsdLib_SOURCE_DIR}/TSL/H5AngImporter.h
        ${EbsdLib_SOURCE_DIR}/TSL/H5AngReader.h
        ${EbsdLib_SOURCE_DIR}/TSL/H5AngVolumeReader.h
        ${EbsdLib_SOURCE_DIR}/TSL/H5OIMReader.h
    )
endif()

cmp_IDE_SOURCE_PROPERTIES( "TSL" "${TSL_HDRS}" "${TSL_SRCS}" ${PROJECT_INSTALL_HEADERS})

if( ${EbsdLib_INSTALL_FILES} EQUAL 1 )
    INSTALL (FILES ${TSL_HDRS}
            DESTINATION include/EbsdLib/TSL
            COMPONENT Headers   )
endif()

