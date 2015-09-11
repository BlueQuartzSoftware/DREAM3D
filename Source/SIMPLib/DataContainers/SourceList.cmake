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
set(SIMPLib_DataContainers_Moc_HDRS
  ${SIMPLib_SOURCE_DIR}/DataContainers/DataContainerArray.h
)
# --------------------------------------------------------------------
# Run Qts automoc program to generate some source files that get compiled
#QT5_WRAP_CPP( SIMPLib_DataContainers_Generated_MOC_SRCS ${SIMPLib_DataContainers_Moc_HDRS})

set(SIMPLib_DataContainers_HDRS
  ${SIMPLib_DataContainers_Moc_HDRS}  # Add the headers that get Moc'ed here so they show up in solutions/IDEs/Project files
  ${SIMPLib_SOURCE_DIR}/DataContainers/DataContainer.h
  ${SIMPLib_SOURCE_DIR}/DataContainers/AttributeMatrix.h
  ${SIMPLib_SOURCE_DIR}/DataContainers/DataContainerArrayProxy.h
  ${SIMPLib_SOURCE_DIR}/DataContainers/DataContainerProxy.h
  ${SIMPLib_SOURCE_DIR}/DataContainers/AttributeMatrixProxy.h
  ${SIMPLib_SOURCE_DIR}/DataContainers/DataArrayProxy.h
  ${SIMPLib_SOURCE_DIR}/DataContainers/DataArrayPath.h
  ${SIMPLib_SOURCE_DIR}/DataContainers/IDataContainerBundle.h
  ${SIMPLib_SOURCE_DIR}/DataContainers/DataContainerBundle.h
)

set(SIMPLib_DataContainers_SRCS
  ${SIMPLib_DataContainers_Generated_MOC_SRCS}  # Add the generated source files here so they get compiled.
  ${SIMPLib_SOURCE_DIR}/DataContainers/DataContainer.cpp
  ${SIMPLib_SOURCE_DIR}/DataContainers/DataContainerArray.cpp
  ${SIMPLib_SOURCE_DIR}/DataContainers/AttributeMatrix.cpp
  ${SIMPLib_SOURCE_DIR}/DataContainers/DataContainerArrayProxy.cpp
  ${SIMPLib_SOURCE_DIR}/DataContainers/DataArrayPath.cpp
  ${SIMPLib_SOURCE_DIR}/DataContainers/DataContainerBundle.cpp
)

cmp_IDE_SOURCE_PROPERTIES( "DREAM3DLib/DataContainers" "${SIMPLib_DataContainers_HDRS}" "${SIMPLib_DataContainers_SRCS}" "0")
cmp_IDE_SOURCE_PROPERTIES( "Generated/DREAM3DLib/DataContainers" "" "${SIMPLib_DataContainers_Generated_MOC_SRCS}" "0")

set(SIMPLib_DataContainers_HDRS
  ${SIMPLib_DataContainers_HDRS}
  ${SIMPLib_DataContainers_Moc_HDRS}  # Add the headers that get Moc'ed here so they show up in solutions/IDEs/Project files
)

if( ${PROJECT_INSTALL_HEADERS} EQUAL 1 )
    INSTALL (FILES ${SIMPLib_DataContainers_HDRS}
            DESTINATION include/DREAM3D/DataContainers
            COMPONENT Headers   )
endif()
