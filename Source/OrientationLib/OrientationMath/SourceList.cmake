
set(OrientationLib_OrientationMath_HDRS
  ${OrientationLib_SOURCE_DIR}/OrientationMath/OrientationConverter.hpp
)

set(OrientationLib_OrientationMath_SRCS
)
cmp_IDE_SOURCE_PROPERTIES( "OrientationMath" "${OrientationLib_OrientationMath_HDRS}" "${OrientationLib_OrientationMath_SRCS}" "0")
if( ${PROJECT_INSTALL_HEADERS} EQUAL 1 )
    INSTALL (FILES ${OrientationLib_OrientationMath_HDRS}
            DESTINATION include/OrientationLib/OrientationMath
            COMPONENT Headers   )
endif()
