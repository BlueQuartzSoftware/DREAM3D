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

set(SIMPLib_FilterParameters_HDRS
  ${SIMPLib_SOURCE_DIR}/FilterParameters/AbstractFilterParametersReader.h
  ${SIMPLib_SOURCE_DIR}/FilterParameters/AbstractFilterParametersWriter.h
  ${SIMPLib_SOURCE_DIR}/FilterParameters/AttributeMatrixCreationFilterParameter.h
  ${SIMPLib_SOURCE_DIR}/FilterParameters/AttributeMatrixSelectionFilterParameter.h
  ${SIMPLib_SOURCE_DIR}/FilterParameters/AxisAngleFilterParameter.h
  ${SIMPLib_SOURCE_DIR}/FilterParameters/BooleanFilterParameter.h
  ${SIMPLib_SOURCE_DIR}/FilterParameters/ChoiceFilterParameter.h
  ${SIMPLib_SOURCE_DIR}/FilterParameters/ComparisonSelectionFilterParameter.h
  ${SIMPLib_SOURCE_DIR}/FilterParameters/DataArrayCreationFilterParameter.h
  ${SIMPLib_SOURCE_DIR}/FilterParameters/DataArraySelectionFilterParameter.h
  ${SIMPLib_SOURCE_DIR}/FilterParameters/DataBundleSelectionFilterParameter.h
  ${SIMPLib_SOURCE_DIR}/FilterParameters/DataContainerArrayProxyFilterParameter.h
  ${SIMPLib_SOURCE_DIR}/FilterParameters/DataContainerReaderFilterParameter.h
  ${SIMPLib_SOURCE_DIR}/FilterParameters/DataContainerSelectionFilterParameter.h
  ${SIMPLib_SOURCE_DIR}/FilterParameters/DoubleFilterParameter.h
  ${SIMPLib_SOURCE_DIR}/FilterParameters/DynamicChoiceFilterParameter.h
  ${SIMPLib_SOURCE_DIR}/FilterParameters/DynamicTableData.h
  ${SIMPLib_SOURCE_DIR}/FilterParameters/DynamicTableFilterParameter.h
  ${SIMPLib_SOURCE_DIR}/FilterParameters/FileListInfoFilterParameter.h
  ${SIMPLib_SOURCE_DIR}/FilterParameters/FilterParameter.h
  ${SIMPLib_SOURCE_DIR}/FilterParameters/FloatVec3FilterParameter.h
  ${SIMPLib_SOURCE_DIR}/FilterParameters/FloatVec4FilterParameter.h
  ${SIMPLib_SOURCE_DIR}/FilterParameters/FourthOrderPolynomialFilterParameter.h
  ${SIMPLib_SOURCE_DIR}/FilterParameters/H5FilterParametersConstants.h
  ${SIMPLib_SOURCE_DIR}/FilterParameters/H5FilterParametersReader.h
  ${SIMPLib_SOURCE_DIR}/FilterParameters/H5FilterParametersWriter.h
  ${SIMPLib_SOURCE_DIR}/FilterParameters/InputFileFilterParameter.h
  ${SIMPLib_SOURCE_DIR}/FilterParameters/InputPathFilterParameter.h
  ${SIMPLib_SOURCE_DIR}/FilterParameters/IntFilterParameter.h
  ${SIMPLib_SOURCE_DIR}/FilterParameters/IntVec3FilterParameter.h
  ${SIMPLib_SOURCE_DIR}/FilterParameters/JsonFilterParametersReader.h
  ${SIMPLib_SOURCE_DIR}/FilterParameters/JsonFilterParametersWriter.h
  ${SIMPLib_SOURCE_DIR}/FilterParameters/LinkedBooleanFilterParameter.h
  ${SIMPLib_SOURCE_DIR}/FilterParameters/LinkedChoicesFilterParameter.h
  ${SIMPLib_SOURCE_DIR}/FilterParameters/MultiDataArraySelectionFilterParameter.h
  ${SIMPLib_SOURCE_DIR}/FilterParameters/OutputFileFilterParameter.h
  ${SIMPLib_SOURCE_DIR}/FilterParameters/OutputPathFilterParameter.h
  ${SIMPLib_SOURCE_DIR}/FilterParameters/PhaseTypeSelectionFilterParameter.h
  ${SIMPLib_SOURCE_DIR}/FilterParameters/PreflightUpdatedValueFilterParameter.h
  ${SIMPLib_SOURCE_DIR}/FilterParameters/QFilterParametersReader.h
  ${SIMPLib_SOURCE_DIR}/FilterParameters/QFilterParametersWriter.h
  ${SIMPLib_SOURCE_DIR}/FilterParameters/SecondOrderPolynomialFilterParameter.h
  ${SIMPLib_SOURCE_DIR}/FilterParameters/SeparatorFilterParameter.h
  ${SIMPLib_SOURCE_DIR}/FilterParameters/ShapeTypeSelectionFilterParameter.h
  ${SIMPLib_SOURCE_DIR}/FilterParameters/StringFilterParameter.h
  ${SIMPLib_SOURCE_DIR}/FilterParameters/Symmetric6x6FilterParameter.h
  ${SIMPLib_SOURCE_DIR}/FilterParameters/ThirdOrderPolynomialFilterParameter.h
  ${SIMPLib_SOURCE_DIR}/FilterParameters/UnknownFilterParameter.h
  ${SIMPLib_SOURCE_DIR}/FilterParameters/VolumeDataContainerInfoFilterParameter.h
)

set(SIMPLib_FilterParameters_SRCS
  ${SIMPLib_SOURCE_DIR}/FilterParameters/AbstractFilterParametersReader.cpp
  ${SIMPLib_SOURCE_DIR}/FilterParameters/AbstractFilterParametersWriter.cpp
  ${SIMPLib_SOURCE_DIR}/FilterParameters/AttributeMatrixCreationFilterParameter.cpp
  ${SIMPLib_SOURCE_DIR}/FilterParameters/AttributeMatrixSelectionFilterParameter.cpp
  ${SIMPLib_SOURCE_DIR}/FilterParameters/AxisAngleFilterParameter.cpp
  ${SIMPLib_SOURCE_DIR}/FilterParameters/BooleanFilterParameter.cpp
  ${SIMPLib_SOURCE_DIR}/FilterParameters/ChoiceFilterParameter.cpp
  ${SIMPLib_SOURCE_DIR}/FilterParameters/ComparisonSelectionFilterParameter.cpp
  ${SIMPLib_SOURCE_DIR}/FilterParameters/DataArrayCreationFilterParameter.cpp
  ${SIMPLib_SOURCE_DIR}/FilterParameters/DataArraySelectionFilterParameter.cpp
  ${SIMPLib_SOURCE_DIR}/FilterParameters/DataBundleSelectionFilterParameter.cpp
  ${SIMPLib_SOURCE_DIR}/FilterParameters/DataContainerArrayProxyFilterParameter.cpp
  ${SIMPLib_SOURCE_DIR}/FilterParameters/DataContainerReaderFilterParameter.cpp
  ${SIMPLib_SOURCE_DIR}/FilterParameters/DataContainerSelectionFilterParameter.cpp
  ${SIMPLib_SOURCE_DIR}/FilterParameters/DoubleFilterParameter.cpp
  ${SIMPLib_SOURCE_DIR}/FilterParameters/DynamicChoiceFilterParameter.cpp
  ${SIMPLib_SOURCE_DIR}/FilterParameters/DynamicTableData.cpp
  ${SIMPLib_SOURCE_DIR}/FilterParameters/DynamicTableFilterParameter.cpp
  ${SIMPLib_SOURCE_DIR}/FilterParameters/FileListInfoFilterParameter.cpp
  ${SIMPLib_SOURCE_DIR}/FilterParameters/FilterParameter.cpp
  ${SIMPLib_SOURCE_DIR}/FilterParameters/FloatVec3FilterParameter.cpp
  ${SIMPLib_SOURCE_DIR}/FilterParameters/FloatVec4FilterParameter.cpp
  ${SIMPLib_SOURCE_DIR}/FilterParameters/FourthOrderPolynomialFilterParameter.cpp
  ${SIMPLib_SOURCE_DIR}/FilterParameters/H5FilterParametersReader.cpp
  ${SIMPLib_SOURCE_DIR}/FilterParameters/H5FilterParametersWriter.cpp
  ${SIMPLib_SOURCE_DIR}/FilterParameters/InputFileFilterParameter.cpp
  ${SIMPLib_SOURCE_DIR}/FilterParameters/InputPathFilterParameter.cpp
  ${SIMPLib_SOURCE_DIR}/FilterParameters/IntFilterParameter.cpp
  ${SIMPLib_SOURCE_DIR}/FilterParameters/IntVec3FilterParameter.cpp
  ${SIMPLib_SOURCE_DIR}/FilterParameters/JsonFilterParametersReader.cpp
  ${SIMPLib_SOURCE_DIR}/FilterParameters/JsonFilterParametersWriter.cpp
  ${SIMPLib_SOURCE_DIR}/FilterParameters/LinkedBooleanFilterParameter.cpp
  ${SIMPLib_SOURCE_DIR}/FilterParameters/LinkedChoicesFilterParameter.cpp
  ${SIMPLib_SOURCE_DIR}/FilterParameters/MultiDataArraySelectionFilterParameter.cpp
  ${SIMPLib_SOURCE_DIR}/FilterParameters/OutputFileFilterParameter.cpp
  ${SIMPLib_SOURCE_DIR}/FilterParameters/OutputPathFilterParameter.cpp
  ${SIMPLib_SOURCE_DIR}/FilterParameters/PhaseTypeSelectionFilterParameter.cpp
  ${SIMPLib_SOURCE_DIR}/FilterParameters/PreflightUpdatedValueFilterParameter.cpp
  ${SIMPLib_SOURCE_DIR}/FilterParameters/QFilterParametersReader.cpp
  ${SIMPLib_SOURCE_DIR}/FilterParameters/QFilterParametersWriter.cpp
  ${SIMPLib_SOURCE_DIR}/FilterParameters/SecondOrderPolynomialFilterParameter.cpp
  ${SIMPLib_SOURCE_DIR}/FilterParameters/SeparatorFilterParameter.cpp
  ${SIMPLib_SOURCE_DIR}/FilterParameters/ShapeTypeSelectionFilterParameter.cpp
  ${SIMPLib_SOURCE_DIR}/FilterParameters/StringFilterParameter.cpp
  ${SIMPLib_SOURCE_DIR}/FilterParameters/Symmetric6x6FilterParameter.cpp
  ${SIMPLib_SOURCE_DIR}/FilterParameters/ThirdOrderPolynomialFilterParameter.cpp
  ${SIMPLib_SOURCE_DIR}/FilterParameters/UnknownFilterParameter.cpp
  ${SIMPLib_SOURCE_DIR}/FilterParameters/VolumeDataContainerInfoFilterParameter.cpp
)

cmp_IDE_SOURCE_PROPERTIES( "SIMPLibLib/FilterParameters" "${SIMPLib_FilterParameters_HDRS}" "${SIMPLib_FilterParameters_SRCS}" "0")
if( ${PROJECT_INSTALL_HEADERS} EQUAL 1 )
    INSTALL (FILES ${SIMPLib_FilterParameters_HDRS}
            DESTINATION include/SIMPLib/FilterParameters
            COMPONENT Headers   )
endif()
