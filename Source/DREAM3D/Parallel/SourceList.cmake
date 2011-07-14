SET (DREAM3D_Parallel_HDRS
)

SET (DREAM3D_Parallel_SRCS
)
cmp_IDE_SOURCE_PROPERTIES( "DREAM3DLib/Parallel" "${DREAM3D_Parallel_HDRS}" "${DREAM3D_Parallel_SRCS}" "0")
if ( ${DREAM3D_INSTALL_FILES} EQUAL 1 )
    INSTALL (FILES ${DREAM3D_Parallel_HDRS}
            DESTINATION include/DREAM3D/Parallel
            COMPONENT Headers   )
endif()
