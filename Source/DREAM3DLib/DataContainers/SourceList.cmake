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
  ${DREAM3DLib_SOURCE_DIR}/DataContainers/DataContainer.h
  ${DREAM3DLib_SOURCE_DIR}/DataContainers/DataContainerArray.h
  ${DREAM3DLib_SOURCE_DIR}/DataContainers/DataContainerMacros.h
  ${DREAM3DLib_SOURCE_DIR}/DataContainers/EdgeDataContainer.h
  ${DREAM3DLib_SOURCE_DIR}/DataContainers/SurfaceDataContainer.h
  ${DREAM3DLib_SOURCE_DIR}/DataContainers/VertexDataContainer.h
  ${DREAM3DLib_SOURCE_DIR}/DataContainers/VolumeDataContainer.h
  ${DREAM3DLib_SOURCE_DIR}/DataContainers/CellArray.hpp
  ${DREAM3DLib_SOURCE_DIR}/DataContainers/EdgeArray.hpp
  ${DREAM3DLib_SOURCE_DIR}/DataContainers/FaceArray.hpp
  ${DREAM3DLib_SOURCE_DIR}/DataContainers/DynamicListArray.hpp
  ${DREAM3DLib_SOURCE_DIR}/DataContainers/MeshStructs.h
  ${DREAM3DLib_SOURCE_DIR}/DataContainers/VertexArray.h
)

set(DREAM3DLib_DataContainers_SRCS
  ${DREAM3DLib_SOURCE_DIR}/DataContainers/DataContainer.cpp
  ${DREAM3DLib_SOURCE_DIR}/DataContainers/DataContainerArray.cpp
  ${DREAM3DLib_SOURCE_DIR}/DataContainers/EdgeDataContainer.cpp
  ${DREAM3DLib_SOURCE_DIR}/DataContainers/SurfaceDataContainer.cpp
  ${DREAM3DLib_SOURCE_DIR}/DataContainers/VertexDataContainer.cpp
  ${DREAM3DLib_SOURCE_DIR}/DataContainers/VolumeDataContainer.cpp
  ${DREAM3DLib_SOURCE_DIR}/DataContainers/VertexArray.cpp
)
cmp_IDE_SOURCE_PROPERTIES( "DREAM3DLib/DataContainers" "${DREAM3DLib_DataContainers_HDRS}" "${DREAM3DLib_DataContainers_SRCS}" "0")
if( ${PROJECT_INSTALL_HEADERS} EQUAL 1 )
    INSTALL (FILES ${DREAM3DLib_DataContainers_HDRS}
            DESTINATION include/DREAM3D/DataContainers
            COMPONENT Headers   )
endif()
