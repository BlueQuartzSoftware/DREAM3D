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

SET (SurfaceMeshingPlugin_Smoothing_HDRS
    ${SurfaceMeshingPlugin_SOURCE_DIR}/Smoothing/Smoothing.h
    ${SurfaceMeshingPlugin_SOURCE_DIR}/Smoothing/LinearAlgebra.h
    ${SurfaceMeshingPlugin_SOURCE_DIR}/Smoothing/Triangle.hpp
    ${SurfaceMeshingPlugin_SOURCE_DIR}/Smoothing/Node.hpp
)
SET (SurfaceMeshingPlugin_Smoothing_SRCS
    ${SurfaceMeshingPlugin_SOURCE_DIR}/Smoothing/Smoothing.cpp
)

#if (NOT DEFINED ONLY_FILTERS)
cmp_IDE_SOURCE_PROPERTIES( "SurfaceMeshing/Smoothing" "${SurfaceMeshingPlugin_Smoothing_HDRS}" "${SurfaceMeshingPlugin_Smoothing_SRCS}" "${PROJECT_INSTALL_HEADERS}")
#endif()
