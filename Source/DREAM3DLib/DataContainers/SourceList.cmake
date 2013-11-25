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

set(DREAM3DLib_DataContainers_HDRS
  ${DREAM3DLib_SOURCE_DIR}/DataContainers/SolidMeshDataContainer.h
  ${DREAM3DLib_SOURCE_DIR}/DataContainers/SurfaceMeshDataContainer.h
  ${DREAM3DLib_SOURCE_DIR}/DataContainers/VoxelDataContainer.h
  ${DREAM3DLib_SOURCE_DIR}/DataContainers/DataContainerMacros.h
  
)

set(DREAM3DLib_DataContainers_SRCS
  ${DREAM3DLib_SOURCE_DIR}/DataContainers/SolidMeshDataContainer.cpp
  ${DREAM3DLib_SOURCE_DIR}/DataContainers/SurfaceMeshDataContainer.cpp
  ${DREAM3DLib_SOURCE_DIR}/DataContainers/VoxelDataContainer.cpp
)
cmp_IDE_SOURCE_PROPERTIES( "DREAM3DLib/DataContainers" "${DREAM3DLib_DataContainers_HDRS}" "${DREAM3DLib_DataContainers_SRCS}" "0")
if( ${PROJECT_INSTALL_HEADERS} EQUAL 1 )
    INSTALL (FILES ${DREAM3DLib_DataContainers_HDRS}
            DESTINATION include/DREAM3D/DataContainers
            COMPONENT Headers   )
endif()
