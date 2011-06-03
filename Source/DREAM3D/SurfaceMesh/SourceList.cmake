

SET (DREAM3D_SurfaceMesh_HDRS
  ${DREAM3DLib_SOURCE_DIR}/SurfaceMesh/LinearAlgebra.h
  ${DREAM3DLib_SOURCE_DIR}/SurfaceMesh/SMVtkFileIO.h
  ${DREAM3DLib_SOURCE_DIR}/SurfaceMesh/SurfaceMesh.h
  ${DREAM3DLib_SOURCE_DIR}/SurfaceMesh/SurfaceMeshFunc.h
#  ${DREAM3DLib_SOURCE_DIR}/SurfaceMesh/smooth-grain3d.h
  ${DREAM3DLib_SOURCE_DIR}/SurfaceMesh/Face.h
  ${DREAM3DLib_SOURCE_DIR}/SurfaceMesh/ISegment.h
  ${DREAM3DLib_SOURCE_DIR}/SurfaceMesh/Neighbor.h
  ${DREAM3DLib_SOURCE_DIR}/SurfaceMesh/Node.h
  ${DREAM3DLib_SOURCE_DIR}/SurfaceMesh/Patch.h
  ${DREAM3DLib_SOURCE_DIR}/SurfaceMesh/Segment.h
  ${DREAM3DLib_SOURCE_DIR}/SurfaceMesh/STLWriter.h
)

SET (DREAM3D_SurfaceMesh_SRCS
  ${DREAM3DLib_SOURCE_DIR}/SurfaceMesh/SMVtkFileIO.cpp
  ${DREAM3DLib_SOURCE_DIR}/SurfaceMesh/SurfaceMesh.cpp
  ${DREAM3DLib_SOURCE_DIR}/SurfaceMesh/SurfaceMeshFunc.cpp
#  ${DREAM3DLib_SOURCE_DIR}/SurfaceMesh/smooth-grain3d.cpp
  ${DREAM3DLib_SOURCE_DIR}/SurfaceMesh/Face.cpp
  ${DREAM3DLib_SOURCE_DIR}/SurfaceMesh/ISegment.cpp
  ${DREAM3DLib_SOURCE_DIR}/SurfaceMesh/Neighbor.cpp
  ${DREAM3DLib_SOURCE_DIR}/SurfaceMesh/Node.cpp
  ${DREAM3DLib_SOURCE_DIR}/SurfaceMesh/Patch.cpp
  ${DREAM3DLib_SOURCE_DIR}/SurfaceMesh/Segment.cpp
  ${DREAM3DLib_SOURCE_DIR}/SurfaceMesh/STLWriter.cpp
)


cmp_IDE_SOURCE_PROPERTIES( "DREAM3DLib/SurfaceMesh" "${DREAM3D_SurfaceMesh_HDRS}" "${DREAM3D_SurfaceMesh_SRCS}" "0")
if ( ${DREAM3D_INSTALL_FILES} EQUAL 1 )
    INSTALL (FILES ${DREAM3D_SurfaceMesh_HDRS}
            DESTINATION include/DREAM3D/SurfaceMesh
            COMPONENT Headers   )
endif()
