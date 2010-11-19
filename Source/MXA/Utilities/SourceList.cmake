#message(STATUS "MXA/Utilities/SourceList.cmake")
SET (MXA_Utilities_SRCS
  ${MXA_SOURCE_DIR}/MXA/Utilities/MXADir.cpp
  ${MXA_SOURCE_DIR}/MXA/Utilities/MXAFileInfo.cpp
  ${MXA_SOURCE_DIR}/MXA/Utilities/MXALogger.cpp
)

SET (MXA_Utilities_HDRS
  ${MXA_SOURCE_DIR}/MXA/Utilities/StringUtils.h
  ${MXA_SOURCE_DIR}/MXA/Utilities/MXADir.h
  ${MXA_SOURCE_DIR}/MXA/Utilities/MXAFileInfo.h
  ${MXA_SOURCE_DIR}/MXA/Utilities/MXALogger.h
)

IF (MSVC)
    SET (MXA_Utilities_SRCS ${MXA_Utilities_SRCS} ${MXA_SOURCE_DIR}/MXA/Utilities/MXADirent.c )
    SET (MXA_Utilities_HDRS ${MXA_Utilities_HDRS} ${MXA_SOURCE_DIR}/MXA/Utilities/MXADirent.h )
endif()
cmp_IDE_SOURCE_PROPERTIES( "MXA/Utilities" "${MXA_Utilities_HDRS}" "${MXA_Utilities_SRCS}" "0")

if ( ${MXA_INSTALL_FILES} EQUAL 1 )
    INSTALL (FILES ${MXA_Utilities_HDRS}
            DESTINATION include/MXA/Utilities
            COMPONENT Headers   )
endif()

