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

set(SurfaceMeshingPlugin_Meshing_HDRS
  ${SurfaceMeshingPlugin_SOURCE_DIR}/Code/SurfaceMeshing/Meshing/Face.h
  ${SurfaceMeshingPlugin_SOURCE_DIR}/Code/SurfaceMeshing/Meshing/Neighbor.h
  ${SurfaceMeshingPlugin_SOURCE_DIR}/Code/SurfaceMeshing/Meshing/Node.h
  ${SurfaceMeshingPlugin_SOURCE_DIR}/Code/SurfaceMeshing/Meshing/Patch.h
  ${SurfaceMeshingPlugin_SOURCE_DIR}/Code/SurfaceMeshing/Meshing/Segment.h

)
set(SurfaceMeshingPlugin_Meshing_SRCS
  ${SurfaceMeshingPlugin_SOURCE_DIR}/Code/SurfaceMeshing/Meshing/Face.cpp
  ${SurfaceMeshingPlugin_SOURCE_DIR}/Code/SurfaceMeshing/Meshing/Neighbor.cpp
  ${SurfaceMeshingPlugin_SOURCE_DIR}/Code/SurfaceMeshing/Meshing/Node.cpp
  ${SurfaceMeshingPlugin_SOURCE_DIR}/Code/SurfaceMeshing/Meshing/Patch.cpp
  ${SurfaceMeshingPlugin_SOURCE_DIR}/Code/SurfaceMeshing/Meshing/Segment.cpp
)
#if (NOT DEFINED ONLY_FILTERS)
cmp_IDE_SOURCE_PROPERTIES( "SurfaceMeshing/Meshing" "${SurfaceMeshingPlugin_Meshing_HDRS}" "${SurfaceMeshingPlugin_Meshing_SRCS}" "${PROJECT_INSTALL_HEADERS}")
#endif()
