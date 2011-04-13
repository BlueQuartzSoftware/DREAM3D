#message(STATUS "MXA/Utilities/SourceList.cmake")
SET (TSLAngLib_Utilities_SRCS
  ${TSLAngLib_SOURCE_DIR}/Utilities/MXADir.cpp
  ${TSLAngLib_SOURCE_DIR}/Utilities/MXAFileInfo.cpp
 # ${TSLAngLib_SOURCE_DIR}/Utilities/MXALogger.cpp
)

SET (TSLAngLib_Utilities_HDRS
  ${TSLAngLib_SOURCE_DIR}/Utilities/StringUtils.h
  ${TSLAngLib_SOURCE_DIR}/Utilities/MXADir.h
  ${TSLAngLib_SOURCE_DIR}/Utilities/MXAFileInfo.h
#  ${TSLAngLib_SOURCE_DIR}/Utilities/MXALogger.h
)

IF (MSVC)
    SET (TSLAngLib_Utilities_SRCS ${TSLAngLib_Utilities_SRCS} ${TSLAngLib_SOURCE_DIR}/Utilities/MXADirent.c )
    SET (TSLAngLib_Utilities_HDRS ${TSLAngLib_Utilities_HDRS} ${TSLAngLib_SOURCE_DIR}/Utilities/MXADirent.h )
endif()
cmp_IDE_SOURCE_PROPERTIES( "ANG/Utilities" "${TSLAngLib_Utilities_HDRS}" "${TSLAngLib_Utilities_SRCS}" "0")

if ( ${TSLAngLib_INSTALL_FILES} EQUAL 1 )
    INSTALL (FILES ${TSLAngLib_Utilities_HDRS}
            DESTINATION include/ANG/Utilities
            COMPONENT Headers   )
endif()

