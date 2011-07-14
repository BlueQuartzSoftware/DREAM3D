

SET (DREAM3D_SurfaceMesh_HDRS
  ${DREAM3DLib_SOURCE_DIR}/SurfaceMesh/SurfaceMesh.h
  ${DREAM3DLib_SOURCE_DIR}/SurfaceMesh/SurfaceMeshFunc.h

  )

SET (DREAM3D_SurfaceMesh_SRCS
  ${DREAM3DLib_SOURCE_DIR}/SurfaceMesh/SurfaceMesh.cpp
  ${DREAM3DLib_SOURCE_DIR}/SurfaceMesh/SurfaceMeshFunc.cpp

)
cmp_IDE_SOURCE_PROPERTIES( "DREAM3DLib/SurfaceMesh" "${DREAM3D_SurfaceMesh_HDRS}" "${DREAM3D_SurfaceMesh_SRCS}" "0")


SET (MESHING_SRCS
  ${DREAM3DLib_SOURCE_DIR}/SurfaceMesh/Meshing/Face.h
  ${DREAM3DLib_SOURCE_DIR}/SurfaceMesh/Meshing/Neighbor.h
  ${DREAM3DLib_SOURCE_DIR}/SurfaceMesh/Meshing/Node.h
  ${DREAM3DLib_SOURCE_DIR}/SurfaceMesh/Meshing/Patch.h
  ${DREAM3DLib_SOURCE_DIR}/SurfaceMesh/Meshing/Segment.h
  ${DREAM3DLib_SOURCE_DIR}/SurfaceMesh/Meshing/STLWriter.h
  ${DREAM3DLib_SOURCE_DIR}/SurfaceMesh/Meshing/SMVtkFileIO.h
)
SET (MESHING_HDRS
  ${DREAM3DLib_SOURCE_DIR}/SurfaceMesh/Meshing/Face.cpp
  ${DREAM3DLib_SOURCE_DIR}/SurfaceMesh/Meshing/Neighbor.cpp
  ${DREAM3DLib_SOURCE_DIR}/SurfaceMesh/Meshing/Node.cpp
  ${DREAM3DLib_SOURCE_DIR}/SurfaceMesh/Meshing/Patch.cpp
  ${DREAM3DLib_SOURCE_DIR}/SurfaceMesh/Meshing/Segment.cpp
  ${DREAM3DLib_SOURCE_DIR}/SurfaceMesh/Meshing/STLWriter.cpp
  ${DREAM3DLib_SOURCE_DIR}/SurfaceMesh/Meshing/SMVtkFileIO.cpp
)
cmp_IDE_SOURCE_PROPERTIES( "DREAM3DLib/SurfaceMesh/Meshing" "${MESHING_HDRS}" "${MESHING_SRCS}" "0")

SET (SMOOTHING_SRCS
    ${DREAM3DLib_SOURCE_DIR}/SurfaceMesh/Smoothing/Smoothing.cpp
)
SET (SMOOTHING_HDRS
    ${DREAM3DLib_SOURCE_DIR}/SurfaceMesh/Smoothing/Smoothing.h
    ${DREAM3DLib_SOURCE_DIR}/SurfaceMesh/Smoothing/LinearAlgebra.h
    ${DREAM3DLib_SOURCE_DIR}/SurfaceMesh/Smoothing/Triangle.hpp
    ${DREAM3DLib_SOURCE_DIR}/SurfaceMesh/Smoothing/Node.hpp
)
cmp_IDE_SOURCE_PROPERTIES( "DREAM3DLib/SurfaceMesh/Meshing" "${SMOOTHING_HDRS}" "${SMOOTHING_SRCS}" "0")

set (DREAM3D_SurfaceMesh_HDRS ${DREAM3D_SurfaceMesh_HDRS} ${SMOOTHING_HDRS} ${MESHING_HDRS})
set (DREAM3D_SurfaceMesh_SRCS ${DREAM3D_SurfaceMesh_SRCS} ${SMOOTHING_SRCS} ${MESHING_SRCS})


if ( ${DREAM3D_INSTALL_FILES} EQUAL 1 )
    INSTALL (FILES ${DREAM3D_SurfaceMesh_HDRS}
            DESTINATION include/DREAM3D/SurfaceMesh
            COMPONENT Headers   )
endif()
