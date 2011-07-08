

SET (DREAM3D_SurfaceMesh_HDRS
  ${DREAM3DLib_SOURCE_DIR}/SurfaceMesh/SMVtkFileIO.h
  ${DREAM3DLib_SOURCE_DIR}/SurfaceMesh/SurfaceMesh.h
  ${DREAM3DLib_SOURCE_DIR}/SurfaceMesh/SurfaceMeshFunc.h
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
  ${DREAM3DLib_SOURCE_DIR}/SurfaceMesh/Face.cpp
  ${DREAM3DLib_SOURCE_DIR}/SurfaceMesh/ISegment.cpp
  ${DREAM3DLib_SOURCE_DIR}/SurfaceMesh/Neighbor.cpp
  ${DREAM3DLib_SOURCE_DIR}/SurfaceMesh/Node.cpp
  ${DREAM3DLib_SOURCE_DIR}/SurfaceMesh/Patch.cpp
  ${DREAM3DLib_SOURCE_DIR}/SurfaceMesh/Segment.cpp
  ${DREAM3DLib_SOURCE_DIR}/SurfaceMesh/STLWriter.cpp
)
cmp_IDE_SOURCE_PROPERTIES( "DREAM3DLib/SurfaceMesh" "${DREAM3D_SurfaceMesh_HDRS}" "${DREAM3D_SurfaceMesh_SRCS}" "0")


SET (WINDING_SRCS
  ${DREAM3DLib_SOURCE_DIR}/SurfaceMesh/Winding/SurfaceWinding.cpp
  ${DREAM3DLib_SOURCE_DIR}/SurfaceMesh/Winding/Edge.cpp
)
SET (WINDING_HDRS
  ${DREAM3DLib_SOURCE_DIR}/SurfaceMesh/Winding/SurfaceWinding.h
  ${DREAM3DLib_SOURCE_DIR}/SurfaceMesh/Winding/Edge.h
  ${DREAM3DLib_SOURCE_DIR}/SurfaceMesh/Winding/Facet.hpp
)
cmp_IDE_SOURCE_PROPERTIES( "DREAM3DLib/SurfaceMesh/Winding" "${WINDING_HDRS}" "${WINDING_SRCS}" "0")

SET (SMOOTHING_SRCS
    ${DREAM3DLib_SOURCE_DIR}/SurfaceMesh/Smoothing/Smoothing.cpp
)
SET (SMOOTHING_HDRS
    ${DREAM3DLib_SOURCE_DIR}/SurfaceMesh/Smoothing/Smoothing.h
    ${DREAM3DLib_SOURCE_DIR}/SurfaceMesh/Smoothing/LinearAlgebra.h
)
cmp_IDE_SOURCE_PROPERTIES( "DREAM3DLib/SurfaceMesh/Smoothing" "${SMOOTHING_HDRS}" "${SMOOTHING_SRCS}" "0")


set (DREAM3D_SurfaceMesh_HDRS ${DREAM3D_SurfaceMesh_HDRS} ${WINDING_HDRS} ${SMOOTHING_HDRS} )
set (DREAM3D_SurfaceMesh_SRCS ${DREAM3D_SurfaceMesh_SRCS} ${WINDING_SRCS} ${SMOOTHING_HDRS} )

if ( ${DREAM3D_INSTALL_FILES} EQUAL 1 )
    INSTALL (FILES ${DREAM3D_SurfaceMesh_HDRS}
            DESTINATION include/DREAM3D/SurfaceMesh
            COMPONENT Headers   )
endif()
