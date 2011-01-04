
set (ANG_SRCS
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/ANG/AngDirectoryPatterns.cpp
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/ANG/AngDataLoader.cpp
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/ANG/AngReader.cpp
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/ANG/H5AngDataLoader.cpp
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/ANG/H5AngReader.cpp
 )
 
set (ANG_HDRS
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/ANG/AngDirectoryPatterns.h
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/ANG/AngDataLoader.h
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/ANG/AngReader.h
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/ANG/AngConstants.h
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/ANG/AngHeaderEntry.h
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/ANG/H5AngDataLoader.h   
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/ANG/H5AngReader.h
)
cmp_IDE_SOURCE_PROPERTIES( "Source/AIM/ANG" "${ANG_HDRS}" "${ANG_SRCS}" ${PROJECT_INSTALL_HEADERS})

