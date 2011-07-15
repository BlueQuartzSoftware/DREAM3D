SET (DREAM3D_ANGSupport_HDRS
  ${DREAM3DLib_SOURCE_DIR}/ANGSupport/AbstractAngDataLoader.h
  ${DREAM3DLib_SOURCE_DIR}/ANGSupport/AngDataLoader.h
  ${DREAM3DLib_SOURCE_DIR}/ANGSupport/H5AngDataLoader.h
)

SET (DREAM3D_ANGSupport_SRCS
  ${DREAM3DLib_SOURCE_DIR}/ANGSupport/AngDataLoader.cpp
  ${DREAM3DLib_SOURCE_DIR}/ANGSupport/H5AngDataLoader.cpp
)
cmp_IDE_SOURCE_PROPERTIES( "DREAM3DLib/ANGSupport" "${DREAM3D_ANGSupport_HDRS}" "${DREAM3D_ANGSupport_SRCS}" "0")
if ( ${DREAM3D_INSTALL_FILES} EQUAL 1 )
    INSTALL (FILES ${DREAM3D_ANGSupport_HDRS}
            DESTINATION include/DREAM3D/ANGSupport
            COMPONENT Headers   )
endif()
