
#-- Get the HKL Sources
set (HKL_SRCS
 #   ${EbsdLib_SOURCE_DIR}/HKL/CtfDirectoryPatterns.cpp
    ${EbsdLib_SOURCE_DIR}/HKL/CtfReader.cpp
    ${EbsdLib_SOURCE_DIR}/HKL/CtfPhase.cpp
 )
 
set (HKL_HDRS
    ${EbsdLib_SOURCE_DIR}/HKL/CtfConstants.h
 #   ${EbsdLib_SOURCE_DIR}/HKL/CtfDirectoryPatterns.h
    ${EbsdLib_SOURCE_DIR}/HKL/CtfHeaderEntry.h
    ${EbsdLib_SOURCE_DIR}/HKL/CtfReader.h
    ${EbsdLib_SOURCE_DIR}/HKL/CtfPhase.h
)

if (EbsdLib_ENABLE_HDF5)
    add_definitions(-DEbsdLib_HAVE_HDF5)
    set (HKL_SRCS ${HKL_SRCS}
        ${EbsdLib_SOURCE_DIR}/HKL/H5CtfImporter.cpp
        ${EbsdLib_SOURCE_DIR}/HKL/H5CtfReader.cpp
    )
    set (HKL_HDRS ${HKL_HDRS}
        ${EbsdLib_SOURCE_DIR}/HKL/H5CtfImporter.h
        ${EbsdLib_SOURCE_DIR}/HKL/H5CtfReader.h
    )
    set (EbsdLib_HDF5_SUPPORT 1)
endif()
cmp_IDE_SOURCE_PROPERTIES( "EbsdLib/HKL" "${HKL_HDRS}" "${HKL_SRCS}" ${PROJECT_INSTALL_HEADERS})

if ( ${EbsdLib_INSTALL_FILES} EQUAL 1 )
    INSTALL (FILES ${HKL_HDRS}
            DESTINATION include/EbsdLib/HKL
            COMPONENT Headers   )
endif()