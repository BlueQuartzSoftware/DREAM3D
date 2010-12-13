
set (ANG_SRCS
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/ANG/AngDirectoryPatterns.cpp
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/ANG/AngFileHelper.cpp
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/ANG/AngReader.cpp
 )
 
set (ANG_HDRS
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/ANG/AngDirectoryPatterns.h
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/ANG/AngFileHelper.h
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/ANG/AngReader.h
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/ANG/AngConstants.h
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/ANG/AngHeaderEntry.h
)

if (AIM_USE_HDF5)

set (ANG_SRCS ${ANG_SRCS}
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/ANG/H5AngImporter.cpp
    )
set (ANG_HDRS ${ANG_HDRS}
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/ANG/H5AngImporter.h
    )
endif()



cmp_IDE_SOURCE_PROPERTIES( "Source/AIM/ANG" "${ANG_HDRS}" "${ANG_SRCS}" ${CMP_INSTALL_FILES})

