#message(STATUS "MXA/Utilities/SourceList.cmake")
SET (MXA_Utilities_SRCS
  ${MXA_SOURCE_DIR}/Utilities/MXADir.cpp
  ${MXA_SOURCE_DIR}/Utilities/MXAFileInfo.cpp
  ${MXA_SOURCE_DIR}/Utilities/MXALogger.cpp
)

SET (MXA_Utilities_HDRS
  ${MXA_SOURCE_DIR}/Utilities/StringUtils.h
  ${MXA_SOURCE_DIR}/Utilities/MXADir.h
  ${MXA_SOURCE_DIR}/Utilities/MXAFileInfo.h
  ${MXA_SOURCE_DIR}/Utilities/MXALogger.h
)

IF (MSVC)
    SET (MXA_Utilities_SRCS ${MXA_Utilities_SRCS} ${MXA_SOURCE_DIR}/Utilities/MXADirent.c )
    SET (MXA_Utilities_HDRS ${MXA_Utilities_HDRS} ${MXA_SOURCE_DIR}/Utilities/MXADirent.h )
endif()
cmp_IDE_SOURCE_PROPERTIES( "MXA/Utilities" "${MXA_Utilities_HDRS}" "${MXA_Utilities_SRCS}" "0")

if ( ${MXA_INSTALL_FILES} EQUAL 1 )
    INSTALL (FILES ${MXA_Utilities_HDRS}
            DESTINATION include/MXA/Utilities
            COMPONENT Headers   )
endif()

