SET (DREAM3D_CalculateStats_HDRS
)

SET (DREAM3D_CalculateStats_SRCS
)
cmp_IDE_SOURCE_PROPERTIES( "DREAM3DLib/CalculateStats" "${DREAM3D_CalculateStats_HDRS}" "${DREAM3D_CalculateStats_SRCS}" "0")
if ( ${DREAM3D_INSTALL_FILES} EQUAL 1 )
    INSTALL (FILES ${DREAM3D_CalculateStats_HDRS}
            DESTINATION include/DREAM3D/CalculateStats
            COMPONENT Headers   )
endif()
