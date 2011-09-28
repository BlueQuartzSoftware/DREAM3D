
set (install_files "0")


if (MSVC)
    SET (MXA_WIN_Common_SRCS
      ${MXA_SOURCE_DIR}/Common/IO/Private/MXAPrivateWindowsFileReader.cpp
      ${MXA_SOURCE_DIR}/Common/IO/Private/MXAPrivateWindowsFileWriter.cpp
    )
    SET (MXA_WIN_Common_HDRS
      ${MXA_SOURCE_DIR}/Common/IO/Private/MXAPrivateWindowsFileReader.h
      ${MXA_SOURCE_DIR}/Common/IO/Private/MXAPrivateWindowsFileWriter.h
    )
    cmp_IDE_SOURCE_PROPERTIES( "MXA/Common/IO/Private" "${MXA_WIN_Common_HDRS}" "${MXA_WIN_Common_SRCS}" "${install_files}")
else(MSVC)
    SET (MXA_UNIX_Common_SRCS
      ${MXA_SOURCE_DIR}/Common/IO/Private/MXAPrivateFileReader.cpp
      ${MXA_SOURCE_DIR}/Common/IO/Private/MXAPrivateFileWriter.cpp
    )
    SET (MXA_UNIX_Common_HDRS
      ${MXA_SOURCE_DIR}/Common/IO/Private/MXAPrivateFileReader.h
      ${MXA_SOURCE_DIR}/Common/IO/Private/MXAPrivateFileWriter.h
    )
    cmp_IDE_SOURCE_PROPERTIES( "MXA/Common/IO/Private" "${MXA_UNIX_Common_HDRS}" "${MXA_UNIX_Common_SRCS}" "${install_files}")
endif()





SET (MXA_Common_SOURCES

)

SET (MXA_Common_HEADERS
    ${MXA_SOURCE_DIR}/Common/LogTime.h
    ${MXA_SOURCE_DIR}/Common/DLLExport.h
    ${MXA_SOURCE_DIR}/Common/MXAEndian.h
    ${MXA_SOURCE_DIR}/Common/MXATypeDefs.h
    ${MXA_SOURCE_DIR}/Common/MXAErrorDefinitions.h
    ${MXA_SOURCE_DIR}/Common/MXASetGetMacros.h
    ${MXA_SOURCE_DIR}/Common/IO/MXAFileReader64.h
    ${MXA_SOURCE_DIR}/Common/IO/MXAFileWriter64.h
    ${MXA_SOURCE_DIR}/Common/Cast/Cast.h
    ${MXA_SOURCE_DIR}/Common/Win32Defines.h
)


cmp_IDE_SOURCE_PROPERTIES( "MXA/Common" "${MXA_Common_HEADERS}" "${MXA_Common_SOURCES}" "0")

SET (MXA_Common_SRCS
    ${MXA_WIN_Common_SRCS}
    ${MXA_UNIX_Common_SRCS}
    ${MXA_Common_SOURCES}
)

SET (MXA_Common_HDRS 
    ${MXA_Common_HDRS}
    ${MXA_WIN_Common_HDRS}
    ${MXA_UNIX_Common_HDRS}
)

if ( ${MXA_INSTALL_FILES} EQUAL 1 )

    INSTALL (FILES  ${MXA_SOURCE_DIR}/Common/IO/MXAFileReader64.h
                    ${MXA_SOURCE_DIR}/Common/IO/MXAFileWriter64.h
             DESTINATION include/MXA/Common/IO
             COMPONENT Headers           
    )
    
    INSTALL (FILES  ${MXA_WIN_Common_HDRS}
                    ${MXA_UNIX_Common_HDRS}
             DESTINATION include/MXA/Common/IO/Private
             COMPONENT Headers           
    )
    
    INSTALL (FILES   ${MXA_SOURCE_DIR}/Common/Cast/Cast.h
             DESTINATION include/MXA/Common/Cast
             COMPONENT Headers           
    )
endif()

