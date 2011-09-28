message(FATAL_ERROR "Volume Mesh is NOT working. All the sources need to be updated")
SET (DREAM3D_VolumeMesh_HDRS
  ${DREAM3DLib_SOURCE_DIR}/VolumeMesh/MeshGenerator.h
#  ${DREAM3DLib_SOURCE_DIR}/VolumeMesh/VolumeMesh.h
)

SET (DREAM3D_VolumeMesh_SRCS
  ${DREAM3DLib_SOURCE_DIR}/VolumeMesh/MeshGenerator.cpp
#  ${DREAM3DLib_SOURCE_DIR}/VolumeMesh/VolumeMesh.cpp
)
cmp_IDE_SOURCE_PROPERTIES( "DREAM3DLib/VolumeMesh" "${DREAM3D_VolumeMesh_HDRS}" "${DREAM3D_VolumeMesh_SRCS}" "0")
if ( ${PROJECT_INSTALL_HEADERS} EQUAL 1 )
    INSTALL (FILES ${DREAM3D_VolumeMesh_HDRS}
            DESTINATION include/DREAM3D/VolumeMesh
            COMPONENT Headers   )
endif()
