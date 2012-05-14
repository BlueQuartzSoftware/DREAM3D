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


#-- Add in any additional NON Filter classes or filters that will NOT show up in the GUI filter list
set (SurfaceMeshingPlugin_IO_HDRS   
    ${SurfaceMeshingPlugin_SOURCE_DIR}/Code/SurfaceMeshing/IO/SMStlWriter.h
    ${SurfaceMeshingPlugin_SOURCE_DIR}/Code/SurfaceMeshing/IO/SMTempFile.hpp
)
set (SurfaceMeshingPlugin_IO_SRCS
    ${SurfaceMeshingPlugin_SOURCE_DIR}/Code/SurfaceMeshing/IO/SMStlWriter.cpp
)

cmp_IDE_SOURCE_PROPERTIES( "SurfaceMeshing/IO" "${SurfaceMeshingPlugin_IO_HDRS}" "${SurfaceMeshingPlugin_IO_SRCS}" "${PROJECT_INSTALL_HEADERS}")

