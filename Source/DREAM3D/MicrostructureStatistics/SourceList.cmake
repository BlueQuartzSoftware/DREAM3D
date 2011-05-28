SET (DREAM3D_MicrostructureStatistics_HDRS
#  ${DREAM3DLib_SOURCE_DIR}/MicrostructureStatistics/MicrostructureStatistics.h
  ${DREAM3DLib_SOURCE_DIR}/MicrostructureStatistics/MicrostructureStatisticsFunc.h

)

SET (DREAM3D_MicrostructureStatistics_SRCS
#  ${DREAM3DLib_SOURCE_DIR}/MicrostructureStatistics/MicrostructureStatistics.cpp
  ${DREAM3DLib_SOURCE_DIR}/MicrostructureStatistics/MicrostructureStatisticsFunc.cpp

)
cmp_IDE_SOURCE_PROPERTIES( "DREAM3DLib/MicrostructureStatistics" "${DREAM3D_MicrostructureStatistics_HDRS}" "${DREAM3D_MicrostructureStatistics_SRCS}" "0")
if ( ${DREAM3D_INSTALL_FILES} EQUAL 1 )
    INSTALL (FILES ${DREAM3D_MicrostructureStatistics_HDRS}
            DESTINATION include/DREAM3D/MicrostructureStatistics
            COMPONENT Headers   )
endif()
