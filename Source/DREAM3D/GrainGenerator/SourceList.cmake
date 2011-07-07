SET (DREAM3D_GrainGenerator_HDRS
  ${DREAM3DLib_SOURCE_DIR}/GrainGenerator/GrainGenerator.h
  ${DREAM3DLib_SOURCE_DIR}/GrainGenerator/GrainGeneratorFunc.h
)

SET (DREAM3D_GrainGenerator_SRCS
  ${DREAM3DLib_SOURCE_DIR}/GrainGenerator/GrainGenerator.cpp
  ${DREAM3DLib_SOURCE_DIR}/GrainGenerator/GrainGeneratorFunc.cpp
)
cmp_IDE_SOURCE_PROPERTIES( "DREAM3DLib/GrainGenerator" "${DREAM3D_GrainGenerator_HDRS}" "${DREAM3D_GrainGenerator_SRCS}" "0")
if ( ${DREAM3D_INSTALL_FILES} EQUAL 1 )
    INSTALL (FILES ${DREAM3D_GrainGenerator_HDRS}
            DESTINATION include/DREAM3D/GrainGenerator
            COMPONENT Headers   )
endif()
