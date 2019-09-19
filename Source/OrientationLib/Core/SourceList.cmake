


set(OrientationLib_Core_HDRS
  ${OrientationLib_SOURCE_DIR}/Core/Orientation.hpp
  ${OrientationLib_SOURCE_DIR}/Core/OrientationTransformation.hpp
  ${OrientationLib_SOURCE_DIR}/Core/Quaternion.hpp
  ${OrientationLib_SOURCE_DIR}/Core/OrientationMath.h

  )

set(OrientationLib_Core_SRCS
${OrientationLib_SOURCE_DIR}/Core/OrientationMath.cpp

)

cmp_IDE_SOURCE_PROPERTIES( "Core" "${OrientationLib_Core_HDRS}" "${OrientationLib_Core_SRCS}" "0")
if( ${PROJECT_INSTALL_HEADERS} EQUAL 1 )
    INSTALL (FILES ${OrientationLib_Core_HDRS}
            DESTINATCoreN include/OrientationLib/Core
            COMPONENT Headers   )
endif()
