SET (EbsdSupport_HDRS
  ${ReconstructionPlugin_SOURCE_DIR}/EbsdSupport/H5EbsdVolumeReader.h
  ${ReconstructionPlugin_SOURCE_DIR}/EbsdSupport/H5AngVolumeReader.h
  ${ReconstructionPlugin_SOURCE_DIR}/EbsdSupport/H5CtfVolumeReader.h
)

SET (EbsdSupport_SRCS
  ${ReconstructionPlugin_SOURCE_DIR}/EbsdSupport/H5EbsdVolumeReader.cpp
  ${ReconstructionPlugin_SOURCE_DIR}/EbsdSupport/H5AngVolumeReader.cpp
  ${ReconstructionPlugin_SOURCE_DIR}/EbsdSupport/H5CtfVolumeReader.cpp
)
cmp_IDE_SOURCE_PROPERTIES( "Reconstruction/EbsdSupport" "${EbsdSupport_HDRS}" "${EbsdSupport_SRCS}" "0")
#if ( ${DREAM3D_INSTALL_FILES} EQUAL 1 )
#    INSTALL (FILES ${DREAM3D_EbsdSupport_HDRS}
#            DESTINATION include/DREAM3D/EbsdSupport
#            COMPONENT Headers   )
#endif()
