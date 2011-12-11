#--////////////////////////////////////////////////////////////////////////////
#--
#--  Copyright (c) 2011, Michael A. Jackson. BlueQuartz Software
#--  Copyright (c) 2011, Michael A. Groeber, US Air Force Research Laboratory
#--  All rights reserved.
#--  BSD License: http://www.opensource.org/licenses/bsd-license.html
#--
#-- This code was partly written under US Air Force Contract FA8650-07-D-5800
#--
#--////////////////////////////////////////////////////////////////////////////

SET (MESHING_SRCS
  ${DREAM3DLib_SOURCE_DIR}/SurfaceMeshingFilters/Meshing/Face.h
  ${DREAM3DLib_SOURCE_DIR}/SurfaceMeshingFilters/Meshing/Neighbor.h
  ${DREAM3DLib_SOURCE_DIR}/SurfaceMeshingFilters/Meshing/Node.h
  ${DREAM3DLib_SOURCE_DIR}/SurfaceMeshingFilters/Meshing/Patch.h
  ${DREAM3DLib_SOURCE_DIR}/SurfaceMeshingFilters/Meshing/Segment.h

)
SET (MESHING_HDRS
  ${DREAM3DLib_SOURCE_DIR}/SurfaceMeshingFilters/Meshing/Face.cpp
  ${DREAM3DLib_SOURCE_DIR}/SurfaceMeshingFilters/Meshing/Neighbor.cpp
  ${DREAM3DLib_SOURCE_DIR}/SurfaceMeshingFilters/Meshing/Node.cpp
  ${DREAM3DLib_SOURCE_DIR}/SurfaceMeshingFilters/Meshing/Patch.cpp
  ${DREAM3DLib_SOURCE_DIR}/SurfaceMeshingFilters/Meshing/Segment.cpp

)
cmp_IDE_SOURCE_PROPERTIES( "DREAM3DLib/SurfaceMeshFilters/Meshing" "${MESHING_HDRS}" "${MESHING_SRCS}" "${PROJECT_INSTALL_HEADERS}")

SET (SMOOTHING_SRCS
    ${DREAM3DLib_SOURCE_DIR}/SurfaceMeshingFilters/Smoothing/Smoothing.cpp
)
SET (SMOOTHING_HDRS
    ${DREAM3DLib_SOURCE_DIR}/SurfaceMeshingFilters/Smoothing/Smoothing.h
    ${DREAM3DLib_SOURCE_DIR}/SurfaceMeshingFilters/Smoothing/LinearAlgebra.h
    ${DREAM3DLib_SOURCE_DIR}/SurfaceMeshingFilters/Smoothing/Triangle.hpp
    ${DREAM3DLib_SOURCE_DIR}/SurfaceMeshingFilters/Smoothing/Node.hpp
)
cmp_IDE_SOURCE_PROPERTIES( "DREAM3DLib/SurfaceMeshFilters/Smoothing" "${SMOOTHING_HDRS}" "${SMOOTHING_SRCS}" "${PROJECT_INSTALL_HEADERS}")

set (DREAM3DLib_SurfaceMeshingFilters_SRCS
    ${DREAM3DLib_SOURCE_DIR}/SurfaceMeshingFilters/SurfaceMeshFilter.cpp
    ${DREAM3DLib_SOURCE_DIR}/SurfaceMeshingFilters/SMStlWriter.cpp
    ${DREAM3DLib_SOURCE_DIR}/SurfaceMeshingFilters/SMVtkPolyDataWriter.cpp
)
set (DREAM3DLib_SurfaceMeshingFilters_HDRS
    ${DREAM3DLib_SOURCE_DIR}/SurfaceMeshingFilters/SurfaceMeshFilter.h
    ${DREAM3DLib_SOURCE_DIR}/SurfaceMeshingFilters/SMStlWriter.h
    ${DREAM3DLib_SOURCE_DIR}/SurfaceMeshingFilters/SMVtkPolyDataWriter.h
    ${DREAM3DLib_SOURCE_DIR}/SurfaceMeshingFilters/SMTempFile.hpp
)
cmp_IDE_SOURCE_PROPERTIES( "DREAM3DLib/SurfaceMeshFilters" "${DREAM3DLib_SurfaceMeshingFilters_HDRS}" "${DREAM3DLib_SurfaceMeshingFilters_SRCS}" "${PROJECT_INSTALL_HEADERS}")



#-- Gather all the Surface Meshing Sources together
set (DREAM3DLib_SurfaceMeshingFilters_SRCS
    ${DREAM3DLib_SurfaceMeshingFilters_SRCS}
    ${MESHING_SRCS}
    ${SMOOTHING_SRCS}    
    )

#-- Gather all teh Surface Meshing Headers together    
set (DREAM3DLib_SurfaceMeshingFilters_HDRS
    ${DREAM3DLib_SurfaceMeshingFilters_HDRS}
    ${MESHING_HDRS}
    ${SMOOTHING_HDRS}
)


if ( ${PROJECT_INSTALL_HEADERS} EQUAL 1 )
    INSTALL (FILES ${DREAM3DLib_SurfaceMeshingFilters_HDRS}
            DESTINATION include/DREAM3D/SurfaceMeshingFilters
            COMPONENT Headers   )
endif()


