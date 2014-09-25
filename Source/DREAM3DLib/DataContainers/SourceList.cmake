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

# --------------------------------------------------------------------
# Any Class that inherits from QObject, either directly or through the heirarchy needs to have its header listed here
set(DREAM3DLib_DataContainers_Moc_HDRS
  ${DREAM3DLib_SOURCE_DIR}/DataContainers/DataContainerArray.h
)
# --------------------------------------------------------------------
# Run Qts automoc program to generate some source files that get compiled
#QT4_WRAP_CPP( DREAM3DLib_DataContainers_Generated_MOC_SRCS ${DREAM3DLib_DataContainers_Moc_HDRS})

set(DREAM3DLib_DataContainers_HDRS
  ${DREAM3DLib_DataContainers_Moc_HDRS}  # Add the headers that get Moc'ed here so they show up in solutions/IDEs/Project files
  ${DREAM3DLib_SOURCE_DIR}/DataContainers/DataContainer.h
  ${DREAM3DLib_SOURCE_DIR}/DataContainers/EdgeDataContainer.h
  ${DREAM3DLib_SOURCE_DIR}/DataContainers/SurfaceDataContainer.h
  ${DREAM3DLib_SOURCE_DIR}/DataContainers/VertexDataContainer.h
  ${DREAM3DLib_SOURCE_DIR}/DataContainers/VolumeDataContainer.h
  ${DREAM3DLib_SOURCE_DIR}/DataContainers/AttributeMatrix.h
  ${DREAM3DLib_SOURCE_DIR}/DataContainers/CellArray.hpp
  ${DREAM3DLib_SOURCE_DIR}/DataContainers/EdgeArray.hpp
  ${DREAM3DLib_SOURCE_DIR}/DataContainers/FaceArray.hpp
  ${DREAM3DLib_SOURCE_DIR}/DataContainers/DynamicListArray.hpp
  ${DREAM3DLib_SOURCE_DIR}/DataContainers/MeshStructs.h
  ${DREAM3DLib_SOURCE_DIR}/DataContainers/VertexArray.h
  ${DREAM3DLib_SOURCE_DIR}/DataContainers/DataContainerArrayProxy.h
  ${DREAM3DLib_SOURCE_DIR}/DataContainers/DataContainerProxy.h
  ${DREAM3DLib_SOURCE_DIR}/DataContainers/AttributeMatrixProxy.h
  ${DREAM3DLib_SOURCE_DIR}/DataContainers/DataArrayProxy.h
  ${DREAM3DLib_SOURCE_DIR}/DataContainers/DataArrayPath.h
  ${DREAM3DLib_SOURCE_DIR}/DataContainers/IDataContainerBundle.h
  ${DREAM3DLib_SOURCE_DIR}/DataContainers/DataContainerBundle.h
)

set(DREAM3DLib_DataContainers_SRCS
  ${DREAM3DLib_DataContainers_Generated_MOC_SRCS}  # Add the generated source files here so they get compiled.
  ${DREAM3DLib_SOURCE_DIR}/DataContainers/DataContainer.cpp
  ${DREAM3DLib_SOURCE_DIR}/DataContainers/DataContainerArray.cpp
  ${DREAM3DLib_SOURCE_DIR}/DataContainers/EdgeDataContainer.cpp
  ${DREAM3DLib_SOURCE_DIR}/DataContainers/SurfaceDataContainer.cpp
  ${DREAM3DLib_SOURCE_DIR}/DataContainers/VertexDataContainer.cpp
  ${DREAM3DLib_SOURCE_DIR}/DataContainers/VolumeDataContainer.cpp
  ${DREAM3DLib_SOURCE_DIR}/DataContainers/AttributeMatrix.cpp
  ${DREAM3DLib_SOURCE_DIR}/DataContainers/VertexArray.cpp
  ${DREAM3DLib_SOURCE_DIR}/DataContainers/DataContainerArrayProxy.cpp
  ${DREAM3DLib_SOURCE_DIR}/DataContainers/DataArrayPath.cpp
  ${DREAM3DLib_SOURCE_DIR}/DataContainers/DataContainerBundle.cpp
)

cmp_IDE_SOURCE_PROPERTIES( "DREAM3DLib/DataContainers" "${DREAM3DLib_DataContainers_HDRS}" "${DREAM3DLib_DataContainers_SRCS}" "0")
cmp_IDE_SOURCE_PROPERTIES( "Generated/DREAM3DLib/DataContainers" "" "${DREAM3DLib_DataContainers_Generated_MOC_SRCS}" "0")

set(DREAM3DLib_DataContainers_HDRS
  ${DREAM3DLib_DataContainers_HDRS}
  ${DREAM3DLib_DataContainers_Moc_HDRS}  # Add the headers that get Moc'ed here so they show up in solutions/IDEs/Project files
)

if( ${PROJECT_INSTALL_HEADERS} EQUAL 1 )
    INSTALL (FILES ${DREAM3DLib_DataContainers_HDRS}
            DESTINATION include/DREAM3D/DataContainers
            COMPONENT Headers   )
endif()
