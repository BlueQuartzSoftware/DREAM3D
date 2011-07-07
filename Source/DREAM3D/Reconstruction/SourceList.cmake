SET (DREAM3D_Reconstruction_HDRS
  ${DREAM3DLib_SOURCE_DIR}/Reconstruction/Reconstruction.h
  ${DREAM3DLib_SOURCE_DIR}/Reconstruction/ReconstructionFunc.h
#  ${DREAM3DLib_SOURCE_DIR}/Reconstruction/ReconstructionVTKWriter.h
)

SET (DREAM3D_Reconstruction_SRCS
  ${DREAM3DLib_SOURCE_DIR}/Reconstruction/Reconstruction.cpp
  ${DREAM3DLib_SOURCE_DIR}/Reconstruction/ReconstructionFunc.cpp
#  ${DREAM3DLib_SOURCE_DIR}/Reconstruction/ReconstructionVTKWriter.cpp
)
cmp_IDE_SOURCE_PROPERTIES( "DREAM3DLib/Reconstruction" "${DREAM3D_Reconstruction_HDRS}" "${DREAM3D_Reconstruction_SRCS}" "0")
if ( ${DREAM3D_INSTALL_FILES} EQUAL 1 )
    INSTALL (FILES ${DREAM3D_Reconstruction_HDRS}
            DESTINATION include/DREAM3D/Reconstruction
            COMPONENT Headers   )
endif()
