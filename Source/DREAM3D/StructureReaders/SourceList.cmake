SET (DREAM3D_StructureReaders_HDRS
  ${DREAM3DLib_SOURCE_DIR}/StructureReaders/AbstractStructureReader.h
  ${DREAM3DLib_SOURCE_DIR}/StructureReaders/DXStructureReader.h
  ${DREAM3DLib_SOURCE_DIR}/StructureReaders/VTKStructureReader.h
)

SET (DREAM3D_StructureReaders_SRCS
  ${DREAM3DLib_SOURCE_DIR}/StructureReaders/DXStructureReader.cpp
  ${DREAM3DLib_SOURCE_DIR}/StructureReaders/VTKStructureReader.cpp
)
cmp_IDE_SOURCE_PROPERTIES( "DREAM3DLib/StructureReaders" "${DREAM3D_StructureReaders_HDRS}" "${DREAM3D_StructureReaders_SRCS}" "0")
if ( ${DREAM3D_INSTALL_FILES} EQUAL 1 )
    INSTALL (FILES ${DREAM3D_StructureReaders_HDRS}
            DESTINATION include/DREAM3D/StructureReaders
            COMPONENT Headers   )
endif()
