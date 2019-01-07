set(BRUKER_NANO_SRCS
  )

set(BRUKER_NANO_HDRS
  )


if(EbsdLib_ENABLE_HDF5)
    set(BRUKER_NANO_SRCS ${BRUKER_NANO_SRCS}
      ${EbsdLib_SOURCE_DIR}/BrukerNano/EspritPhase.cpp
      ${EbsdLib_SOURCE_DIR}/BrukerNano/H5EspritReader.cpp
      ${EbsdLib_SOURCE_DIR}/BrukerNano/H5EspritFields.cpp
    )
    set(BRUKER_NANO_HDRS ${BRUKER_NANO_HDRS}
      ${EbsdLib_SOURCE_DIR}/BrukerNano/EspritPhase.h
      ${EbsdLib_SOURCE_DIR}/BrukerNano/H5EspritReader.h
      ${EbsdLib_SOURCE_DIR}/BrukerNano/H5EspritFields.h
    )
endif()


cmp_IDE_SOURCE_PROPERTIES( "BrukerNano" "${BRUKER_NANO_HDRS}" "${BRUKER_NANO_SRCS}" ${PROJECT_INSTALL_HEADERS})

if( ${EbsdLib_INSTALL_FILES} EQUAL 1 )
    INSTALL (FILES ${BRUKER_NANO_HDRS}
            DESTINATION include/EbsdLib/BrukerNano
            COMPONENT Headers   )
endif()


