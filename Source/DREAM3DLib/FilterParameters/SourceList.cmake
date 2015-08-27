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
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/AttributeMatrixCreationFilterParameter.h
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/AttributeMatrixSelectionFilterParameter.h
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/AxisAngleFilterParameter.h
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/BooleanFilterParameter.h
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/ChoiceFilterParameter.h
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/ComparisonSelectionFilterParameter.h
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/DataArrayCreationFilterParameter.h
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/DataArraySelectionFilterParameter.h
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/DataBundleSelectionFilterParameter.h
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/DataContainerArrayProxyFilterParameter.h
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/DataContainerReaderFilterParameter.h
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/DataContainerSelectionFilterParameter.h
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/DoubleFilterParameter.h
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/DynamicChoiceFilterParameter.h
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/DynamicTableData.h
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/DynamicTableFilterParameter.h
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/FileListInfoFilterParameter.h
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/FilterParameter.h
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/FilterParameterHelpers.hpp
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/FloatVec3FilterParameter.h
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/FloatVec4FilterParameter.h
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/FourthOrderPolynomialFilterParameter.h
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/H5FilterParametersConstants.h
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/H5FilterParametersReader.h
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/H5FilterParametersWriter.h
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/InputFileFilterParameter.h
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/InputPathFilterParameter.h
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/IntFilterParameter.h
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/IntVec3FilterParameter.h
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/JsonFilterParametersReader.h
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/JsonFilterParametersWriter.h
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/LinkedBooleanFilterParameter.h
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/LinkedChoicesFilterParameter.h
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/MultiDataArraySelectionFilterParameter.h
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/OutputFileFilterParameter.h
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/OutputPathFilterParameter.h
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/PhaseTypeSelectionFilterParameter.h
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/PreflightUpdatedValueFilterParameter.h
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/QFilterParametersReader.h
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/QFilterParametersWriter.h
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/SecondOrderPolynomialFilterParameter.h
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/SeparatorFilterParameter.h
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/ShapeTypeSelectionFilterParameter.h
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/StringFilterParameter.h
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/Symmetric6x6FilterParameter.h
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/ThirdOrderPolynomialFilterParameter.h
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/UnknownFilterParameter.h
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/VolumeDataContainerInfoFilterParameter.h
)

set(DREAM3DLib_FilterParameters_SRCS
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/AbstractFilterParametersReader.cpp
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/AbstractFilterParametersWriter.cpp
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/AttributeMatrixCreationFilterParameter.cpp
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/AttributeMatrixSelectionFilterParameter.cpp
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/AxisAngleFilterParameter.cpp
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/BooleanFilterParameter.cpp
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/ChoiceFilterParameter.cpp
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/ComparisonSelectionFilterParameter.cpp
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/DataArrayCreationFilterParameter.cpp
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/DataArraySelectionFilterParameter.cpp
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/DataBundleSelectionFilterParameter.cpp
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/DataContainerArrayProxyFilterParameter.cpp
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/DataContainerReaderFilterParameter.cpp
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/DataContainerSelectionFilterParameter.cpp
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/DoubleFilterParameter.cpp
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/DynamicChoiceFilterParameter.cpp
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/DynamicTableData.cpp
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/DynamicTableFilterParameter.cpp
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/FileListInfoFilterParameter.cpp
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/FilterParameter.cpp
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/FloatVec3FilterParameter.cpp
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/FloatVec4FilterParameter.cpp
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/FourthOrderPolynomialFilterParameter.cpp
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/H5FilterParametersReader.cpp
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/H5FilterParametersWriter.cpp
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/InputFileFilterParameter.cpp
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/InputPathFilterParameter.cpp
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/IntFilterParameter.cpp
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/IntVec3FilterParameter.cpp
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/JsonFilterParametersReader.cpp
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/JsonFilterParametersWriter.cpp
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/LinkedBooleanFilterParameter.cpp
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/LinkedChoicesFilterParameter.cpp
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/MultiDataArraySelectionFilterParameter.cpp
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/OutputFileFilterParameter.cpp
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/OutputPathFilterParameter.cpp
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/PhaseTypeSelectionFilterParameter.cpp
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/PreflightUpdatedValueFilterParameter.cpp
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/QFilterParametersReader.cpp
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/QFilterParametersWriter.cpp
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/SecondOrderPolynomialFilterParameter.cpp
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/SeparatorFilterParameter.cpp
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/ShapeTypeSelectionFilterParameter.cpp
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/StringFilterParameter.cpp
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/Symmetric6x6FilterParameter.cpp
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/ThirdOrderPolynomialFilterParameter.cpp
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/UnknownFilterParameter.cpp
  ${DREAM3DLib_SOURCE_DIR}/FilterParameters/VolumeDataContainerInfoFilterParameter.cpp
)

cmp_IDE_SOURCE_PROPERTIES( "DREAM3DLib/FilterParameters" "${DREAM3DLib_FilterParameters_HDRS}" "${DREAM3DLib_FilterParameters_SRCS}" "0")
if( ${PROJECT_INSTALL_HEADERS} EQUAL 1 )
    INSTALL (FILES ${DREAM3DLib_FilterParameters_HDRS}
            DESTINATION include/DREAM3D/FilterParameters
            COMPONENT Headers   )
endif()
