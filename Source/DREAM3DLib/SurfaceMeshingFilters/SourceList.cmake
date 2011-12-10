#--////////////////////////////////////////////////////////////////////////////
#--
#--  Copyright (c) 2011, Michael A. Jackson. BlueQuartz Software
#--  Copyright (c) 2011, Michael Groeber, US Air Force Research Laboratory
#--  All rights reserved.
#--  BSD License: http://www.opensource.org/licenses/bsd-license.html
#--
#-- This code was partly written under US Air Force Contract FA8650-07-D-5800
#--
#--////////////////////////////////////////////////////////////////////////////

SET (DREAM3DLib_SurfaceMeshingFilters_HDRS
  ${DREAM3DLib_SOURCE_DIR}/SurfaceMeshingFilters/SurfaceMeshFunc.h
)
SET (DREAM3DLib_SurfaceMeshingFilters_SRCS
    ${DREAM3DLib_SOURCE_DIR}/SurfaceMeshingFilters/SurfaceMeshFunc.cpp
)
cmp_IDE_SOURCE_PROPERTIES( "DREAM3DLib/SurfaceMeshingFilters" "${DREAM3DLib_SurfaceMeshingFilters_HDRS}" "${DREAM3DLib_SurfaceMeshingFilters_SRCS}" "0")
if ( ${PROJECT_INSTALL_HEADERS} EQUAL 1 )
    INSTALL (FILES ${DREAM3DLib_SurfaceMeshingFilters_HDRS}
            DESTINATION include/DREAM3D/SurfaceMeshingFilters
            COMPONENT Headers   )
endif()


SET (MESHING_SRCS
  ${DREAM3DLib_SOURCE_DIR}/SurfaceMeshingFilters/Meshing/Face.h
  ${DREAM3DLib_SOURCE_DIR}/SurfaceMeshingFilters/Meshing/Neighbor.h
  ${DREAM3DLib_SOURCE_DIR}/SurfaceMeshingFilters/Meshing/Node.h
  ${DREAM3DLib_SOURCE_DIR}/SurfaceMeshingFilters/Meshing/Patch.h
  ${DREAM3DLib_SOURCE_DIR}/SurfaceMeshingFilters/Meshing/Segment.h
  ${DREAM3DLib_SOURCE_DIR}/SurfaceMeshingFilters/Meshing/STLWriter.h
  ${DREAM3DLib_SOURCE_DIR}/SurfaceMeshingFilters/Meshing/SMVtkFileIO.h
)
SET (MESHING_HDRS
  ${DREAM3DLib_SOURCE_DIR}/SurfaceMeshingFilters/Meshing/Face.cpp
  ${DREAM3DLib_SOURCE_DIR}/SurfaceMeshingFilters/Meshing/Neighbor.cpp
  ${DREAM3DLib_SOURCE_DIR}/SurfaceMeshingFilters/Meshing/Node.cpp
  ${DREAM3DLib_SOURCE_DIR}/SurfaceMeshingFilters/Meshing/Patch.cpp
  ${DREAM3DLib_SOURCE_DIR}/SurfaceMeshingFilters/Meshing/Segment.cpp
  ${DREAM3DLib_SOURCE_DIR}/SurfaceMeshingFilters/Meshing/STLWriter.cpp
  ${DREAM3DLib_SOURCE_DIR}/SurfaceMeshingFilters/Meshing/SMVtkFileIO.cpp
)
cmp_IDE_SOURCE_PROPERTIES( "DREAM3DLib/SurfaceMeshingFilters/Meshing" "${MESHING_HDRS}" "${MESHING_SRCS}" "0")

SET (DREAM3DLib_SurfaceMeshingFilters_HDRS ${DREAM3DLib_SurfaceMeshingFilters_HDRS} ${MESHING_HDRS})
SET (DREAM3DLib_SurfaceMeshingFilters_SRCS ${DREAM3DLib_SurfaceMeshingFilters_SRCS} ${MESHING_SRCS})
