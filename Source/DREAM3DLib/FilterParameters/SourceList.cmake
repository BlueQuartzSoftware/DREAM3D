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

set(DREAM3DLib_FilterParameters_HDRS
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/AbstractFilterParametersReader.h
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/AbstractFilterParametersWriter.h
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/FilterParameter.h
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/H5FilterParametersConstants.h
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/H5FilterParametersReader.h
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/H5FilterParametersWriter.h
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/QFilterParametersReader.h
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/QFilterParametersWriter.h
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/TextFilterParametersWriter.h
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/DynamicTableData.h
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/FileListInfoFilterParameter.h
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/LinkedBooleanFilterParameter.h
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/ChoiceFilterParameter.h
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/LinkedChoicesFilterParameter.h
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/ConstrainedFilterParameter.h
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/FileSystemFilterParameter.h
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/VolumeInfoFilterParameter.h
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/DynamicChoiceFilterParameter.h
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/ComparisonFilterParameter.h
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/ShapeTypesFilterParameter.h
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/PhaseTypesFilterParameter.h
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/PreflightUpdatedValue.h
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/DataContainerArrayProxyFilterParameter.h
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/DataContainerReaderFilterParameter.h
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/MultiDataArraySelectionFilterParameter.h
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/DynamicTableFilterParameter.h
)

set(DREAM3DLib_FilterParameters_SRCS
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/AbstractFilterParametersReader.cpp
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/AbstractFilterParametersWriter.cpp
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/FilterParameter.cpp
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/H5FilterParametersReader.cpp
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/H5FilterParametersWriter.cpp
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/QFilterParametersReader.cpp
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/QFilterParametersWriter.cpp
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/TextFilterParametersWriter.cpp
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/DynamicTableData.cpp
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/FileListInfoFilterParameter.cpp
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/ChoiceFilterParameter.cpp
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/LinkedBooleanFilterParameter.cpp
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/LinkedChoicesFilterParameter.cpp
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/ConstrainedFilterParameter.cpp
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/FileSystemFilterParameter.cpp
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/VolumeInfoFilterParameter.cpp
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/DynamicChoiceFilterParameter.cpp
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/ComparisonFilterParameter.cpp
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/ShapeTypesFilterParameter.cpp
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/PhaseTypesFilterParameter.cpp
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/PreflightUpdatedValue.cpp
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/DataContainerArrayProxyFilterParameter.cpp
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/DataContainerReaderFilterParameter.cpp
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/MultiDataArraySelectionFilterParameter.cpp
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/DynamicTableFilterParameter.cpp
)
cmp_IDE_SOURCE_PROPERTIES( "DREAM3DLib/FilterParameters" "${DREAM3DLib_FilterParameters_HDRS}" "${DREAM3DLib_FilterParameters_SRCS}" "0")
if( ${PROJECT_INSTALL_HEADERS} EQUAL 1 )
    INSTALL (FILES ${DREAM3DLib_FilterParameters_HDRS}
            DESTINATION include/DREAM3D/FilterParameters
            COMPONENT Headers   )
endif()
