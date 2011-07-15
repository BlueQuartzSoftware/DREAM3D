#message(STATUS "MXA/Utilities/SourceList.cmake")
SET (TSLLib_Utilities_SRCS
  ${TSLLib_SOURCE_DIR}/Utilities/MXADir.cpp
  ${TSLLib_SOURCE_DIR}/Utilities/MXAFileInfo.cpp
 # ${TSLLib_SOURCE_DIR}/Utilities/MXALogger.cpp
)

SET (TSLLib_Utilities_HDRS
  ${TSLLib_SOURCE_DIR}/Utilities/StringUtils.h
  ${TSLLib_SOURCE_DIR}/Utilities/MXADir.h
  ${TSLLib_SOURCE_DIR}/Utilities/MXAFileInfo.h
#  ${TSLLib_SOURCE_DIR}/Utilities/MXALogger.h
)

IF (MSVC)
    SET (TSLLib_Utilities_SRCS ${TSLLib_Utilities_SRCS} ${TSLLib_SOURCE_DIR}/Utilities/MXADirent.c )
    SET (TSLLib_Utilities_HDRS ${TSLLib_Utilities_HDRS} ${TSLLib_SOURCE_DIR}/Utilities/MXADirent.h )
endif()
cmp_IDE_SOURCE_PROPERTIES( "TSLLib/Utilities" "${TSLLib_Utilities_HDRS}" "${TSLLib_Utilities_SRCS}" "0")

if ( ${TSLLib_INSTALL_FILES} EQUAL 1 )
    INSTALL (FILES ${TSLLib_Utilities_HDRS}
            DESTINATION include/TSLLib/Utilities
            COMPONENT Headers   )
endif()

