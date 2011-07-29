#message(STATUS "MXA/Utilities/SourceList.cmake")
SET (EbsdLib_Utilities_SRCS
  ${EbsdLib_SOURCE_DIR}/Utilities/MXADir.cpp
  ${EbsdLib_SOURCE_DIR}/Utilities/MXAFileInfo.cpp
 # ${EbsdLib_SOURCE_DIR}/Utilities/MXALogger.cpp
)

SET (EbsdLib_Utilities_HDRS
  ${EbsdLib_SOURCE_DIR}/Utilities/StringUtils.h
  ${EbsdLib_SOURCE_DIR}/Utilities/MXADir.h
  ${EbsdLib_SOURCE_DIR}/Utilities/MXAFileInfo.h
#  ${EbsdLib_SOURCE_DIR}/Utilities/MXALogger.h
)

IF (MSVC)
    SET (EbsdLib_Utilities_SRCS ${EbsdLib_Utilities_SRCS} ${EbsdLib_SOURCE_DIR}/Utilities/MXADirent.c )
    SET (EbsdLib_Utilities_HDRS ${EbsdLib_Utilities_HDRS} ${EbsdLib_SOURCE_DIR}/Utilities/MXADirent.h )
endif()
cmp_IDE_SOURCE_PROPERTIES( "EbsdLib/Utilities" "${EbsdLib_Utilities_HDRS}" "${EbsdLib_Utilities_SRCS}" "0")

if ( ${EbsdLib_INSTALL_FILES} EQUAL 1 )
    INSTALL (FILES ${EbsdLib_Utilities_HDRS}
            DESTINATION include/EbsdLib/Utilities
            COMPONENT Headers   )
endif()

