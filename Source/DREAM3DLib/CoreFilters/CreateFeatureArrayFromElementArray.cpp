/* ============================================================================
* Copyright (c) 2009-2015 BlueQuartz Software, LLC
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
*
* Redistributions of source code must retain the above copyright notice, this
* list of conditions and the following disclaimer.
*
* Redistributions in binary form must reproduce the above copyright notice, this
* list of conditions and the following disclaimer in the documentation and/or
* other materials provided with the distribution.
*
* Neither the name of BlueQuartz Software, the US Air Force, nor the names of its
* contributors may be used to endorse or promote products derived from this software
* without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
* USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
* The code contained herein was partially funded by the followig contracts:
*    United States Air Force Prime Contract FA8650-07-D-5800
*    United States Air Force Prime Contract FA8650-10-D-5210
*    United States Prime Contract Navy N00173-07-C-2068
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */


#include "CreateFeatureArrayFromElementArray.h"

#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Common/TemplateHelpers.hpp"
#include "DREAM3DLib/FilterParameters/AbstractFilterParametersReader.h"
#include "DREAM3DLib/FilterParameters/AbstractFilterParametersWriter.h"

#include "DREAM3DLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "DREAM3DLib/FilterParameters/AttributeMatrixSelectionFilterParameter.h"
#include "DREAM3DLib/FilterParameters/StringFilterParameter.h"
#include "DREAM3DLib/FilterParameters/SeparatorFilterParameter.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CreateFeatureArrayFromElementArray::CreateFeatureArrayFromElementArray() :
  AbstractFilter(),
  m_CellFeatureAttributeMatrixName("", "", ""),
  m_SelectedCellArrayPath("", "", ""),
  m_CreatedArrayName(""),
  m_FeatureIdsArrayPath("", "", ""),
  m_FeatureIds(NULL),
  m_InArray(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CreateFeatureArrayFromElementArray::~CreateFeatureArrayFromElementArray()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CreateFeatureArrayFromElementArray::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(SeparatorFilterParameter::New("Element Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::DataStructureRequirements req = DataArraySelectionFilterParameter::CreateCategoryRequirement(QString(""), std::numeric_limits<uint32_t>::max(), DREAM3D::AttributeMatrixObjectType::Element);
    parameters.push_back(DataArraySelectionFilterParameter::New("Element Data to Copy to Feature Data", "SelectedCellArrayPath", getSelectedCellArrayPath(), FilterParameter::RequiredArray, req));
  }
  {
    DataArraySelectionFilterParameter::DataStructureRequirements req = DataArraySelectionFilterParameter::CreateCategoryRequirement(DREAM3D::TypeNames::Int32, 1, DREAM3D::AttributeMatrixObjectType::Element);
    parameters.push_back(DataArraySelectionFilterParameter::New("Feature Ids", "FeatureIdsArrayPath", getFeatureIdsArrayPath(), FilterParameter::RequiredArray, req));
  }
  parameters.push_back(SeparatorFilterParameter::New("Feature Data", FilterParameter::CreatedArray));
  {
    AttributeMatrixSelectionFilterParameter::DataStructureRequirements req = AttributeMatrixSelectionFilterParameter::CreateRequirement(DREAM3D::AttributeMatrixType::CellFeature, DREAM3D::GeometryType::UnknownGeometry);
    parameters.push_back(AttributeMatrixSelectionFilterParameter::New("Feature Attribute Matrix", "CellFeatureAttributeMatrixName", getCellFeatureAttributeMatrixName(), FilterParameter::CreatedArray, req));
  }
  parameters.push_back(StringFilterParameter::New("Copied Attribute Array", "CreatedArrayName", getCreatedArrayName(), FilterParameter::CreatedArray));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CreateFeatureArrayFromElementArray::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setCellFeatureAttributeMatrixName(reader->readDataArrayPath("CellFeatureAttributeMatrixName", getCellFeatureAttributeMatrixName()));
  setFeatureIdsArrayPath(reader->readDataArrayPath("FeatureIdsArrayPath", getFeatureIdsArrayPath() ) );
  setSelectedCellArrayPath(reader->readDataArrayPath( "SelectedCellArrayPath", getSelectedCellArrayPath() ) );
  setCreatedArrayName(reader->readString("CreatedArrayName", getCreatedArrayName() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int CreateFeatureArrayFromElementArray::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  DREAM3D_FILTER_WRITE_PARAMETER(FilterVersion)
  DREAM3D_FILTER_WRITE_PARAMETER(CellFeatureAttributeMatrixName)
  DREAM3D_FILTER_WRITE_PARAMETER(FeatureIdsArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(SelectedCellArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(CreatedArrayName)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CreateFeatureArrayFromElementArray::dataCheck()
{
  setErrorCondition(0);

  if (getCreatedArrayName().isEmpty() == true)
  {
    setErrorCondition(-11002);
    notifyErrorMessage(getHumanLabel(), "The new Feature Array name must be set", getErrorCondition());
    return;
  }

  QVector<size_t> cDims(1, 1);
  m_FeatureIdsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getFeatureIdsArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeatureIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0); }   /* Now assign the raw pointer to data from the DataArray<T> object */

  m_InArrayPtr = getDataContainerArray()->getPrereqIDataArrayFromPath<IDataArray, AbstractFilter>(this, getSelectedCellArrayPath());

  getDataContainerArray()->getPrereqAttributeMatrixFromPath<AbstractFilter>(this, getCellFeatureAttributeMatrixName(), -301);

  if(getErrorCondition() < 0 ) { return; }

  DataArrayPath tempPath(getCellFeatureAttributeMatrixName().getDataContainerName(), getCellFeatureAttributeMatrixName().getAttributeMatrixName(), getCreatedArrayName());
  TemplateHelpers::CreateNonPrereqArrayFromArrayType()(this, tempPath, m_InArrayPtr.lock()->getComponentDimensions(), m_InArrayPtr.lock());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CreateFeatureArrayFromElementArray::preflight()
{
  setInPreflight(true);
  emit preflightAboutToExecute();
  emit updateFilterParameters(this);
  dataCheck();
  emit preflightExecuted();
  setInPreflight(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T>
IDataArray::Pointer copyCellData(IDataArray::Pointer inputData, int32_t features, int32_t* featureIds)
{
  QString featureArrayName = inputData->getName();

  typename DataArray<T>::Pointer cell = boost::dynamic_pointer_cast<DataArray<T> >(inputData);
  if (NULL == cell) { return IDataArray::NullPointer(); }

  QVector<size_t> dims = inputData->getComponentDimensions();
  typename DataArray<T>::Pointer feature = DataArray<T>::CreateArray(features, dims, featureArrayName);

  T* fPtr = feature->getPointer(0);
  T* cPtr = cell->getPointer(0);

  int32_t numComp = cell->getNumberOfComponents();
  int32_t featureIdx = 0;

  size_t cells = inputData->getNumberOfTuples();
  for (size_t i = 0; i < cells; ++i)
  {
    // Get the feature id (or what ever the user has selected as their "Feature" identifier
    featureIdx = featureIds[i];
    // Now get the pointer to the start of the tuple for the Cell Array at the given Feature Id Index value
    T* cSourcePtr = cPtr + (numComp * i);
    // Now get the pointer to the start of the tuple for the Feature Array at the proper index
    T* fDestPtr = fPtr + (numComp * featureIdx);

    // Now just raw copy the bytes from the source to the destination
    ::memcpy(fDestPtr, cSourcePtr, sizeof(T) * numComp);
  }
  return feature;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CreateFeatureArrayFromElementArray::execute()
{
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  // Validate that the selected InArray has tuples equal to the largest
  // Feature Id; the filter would not crash otherwise, but the user should
  // be notified of unanticipated behavior ; this cannot be done in the dataCheck since
  // we don't have acces to the data yet
  int32_t totalFeatures = getDataContainerArray()->getAttributeMatrix(m_CellFeatureAttributeMatrixName)->getNumTuples();
  bool mismatchedFeatures = false;
  int32_t largestFeature = 0;
  size_t totalPoints = m_FeatureIdsPtr.lock()->getNumberOfTuples();
  for (size_t i = 0; i < totalPoints; i ++)
  {
    if (m_FeatureIds[i] > largestFeature)
    {
      largestFeature = m_FeatureIds[i];
      if (largestFeature >= totalFeatures)
      {
        mismatchedFeatures = true;
        break;
      }
    }
  }

  if (mismatchedFeatures == true)
  {
    QString ss = QObject::tr("The number of Features in the InArray array (%1) is larger than the largest Feature Id in the FeatureIds array").arg(totalFeatures);
    setErrorCondition(-5555);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  if (largestFeature != (totalFeatures - 1))
  {
    QString ss = QObject::tr("The number of Features in the InArray array (%1) does not match the largest Feature Id in the FeatureIds array").arg(totalFeatures);
    setErrorCondition(-5555);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  IDataArray::Pointer p = IDataArray::NullPointer();

  if (TemplateHelpers::CanDynamicCast<Int8ArrayType>()(m_InArrayPtr.lock()))
  {
    p = copyCellData<int8_t>(m_InArrayPtr.lock(), totalFeatures, m_FeatureIds);
  }
  else if (TemplateHelpers::CanDynamicCast<UInt8ArrayType>()(m_InArrayPtr.lock()))
  {
    p = copyCellData<uint8_t>(m_InArrayPtr.lock(), totalFeatures, m_FeatureIds);
  }
  else if (TemplateHelpers::CanDynamicCast<Int16ArrayType>()(m_InArrayPtr.lock()))
  {
    p = copyCellData<int16_t>(m_InArrayPtr.lock(), totalFeatures, m_FeatureIds);
  }
  else if (TemplateHelpers::CanDynamicCast<UInt16ArrayType>()(m_InArrayPtr.lock()))
  {
    p = copyCellData<uint16_t>(m_InArrayPtr.lock(), totalFeatures, m_FeatureIds);
  }
  else if (TemplateHelpers::CanDynamicCast<Int32ArrayType>()(m_InArrayPtr.lock()))
  {
    p = copyCellData<int32_t>(m_InArrayPtr.lock(), totalFeatures, m_FeatureIds);
  }
  else if (TemplateHelpers::CanDynamicCast<UInt32ArrayType>()(m_InArrayPtr.lock()))
  {
    p = copyCellData<uint32_t>(m_InArrayPtr.lock(), totalFeatures, m_FeatureIds);
  }
  else if (TemplateHelpers::CanDynamicCast<Int64ArrayType>()(m_InArrayPtr.lock()))
  {
    p = copyCellData<int64_t>(m_InArrayPtr.lock(), totalFeatures, m_FeatureIds);
  }
  else if (TemplateHelpers::CanDynamicCast<UInt64ArrayType>()(m_InArrayPtr.lock()))
  {
    p = copyCellData<uint64_t>(m_InArrayPtr.lock(), totalFeatures, m_FeatureIds);
  }
  else if (TemplateHelpers::CanDynamicCast<FloatArrayType>()(m_InArrayPtr.lock()))
  {
    p = copyCellData<float>(m_InArrayPtr.lock(), totalFeatures, m_FeatureIds);
  }
  else if (TemplateHelpers::CanDynamicCast<DoubleArrayType>()(m_InArrayPtr.lock()))
  {
    p = copyCellData<double>(m_InArrayPtr.lock(), totalFeatures, m_FeatureIds);
  }
  else if (TemplateHelpers::CanDynamicCast<BoolArrayType>()(m_InArrayPtr.lock()))
  {
    p = copyCellData<bool>(m_InArrayPtr.lock(), totalFeatures, m_FeatureIds);
  }
  else
  {
    QString ss = QObject::tr("The selected array was of unsupported type. The path is %1").arg(m_SelectedCellArrayPath.serialize());
    setErrorCondition(-14000);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

  if (p.get() != NULL)
  {
    getDataContainerArray()->getAttributeMatrix(m_CellFeatureAttributeMatrixName)->addAttributeArray(p->getName(), p);
  }

  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer CreateFeatureArrayFromElementArray::newFilterInstance(bool copyFilterParameters)
{
  CreateFeatureArrayFromElementArray::Pointer filter = CreateFeatureArrayFromElementArray::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CreateFeatureArrayFromElementArray::getCompiledLibraryName()
{
  return Core::CoreBaseName;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CreateFeatureArrayFromElementArray::getGroupName()
{
  return DREAM3D::FilterGroups::CoreFilters;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CreateFeatureArrayFromElementArray::getSubGroupName()
{
  return DREAM3D::FilterSubGroups::MemoryManagementFilters;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CreateFeatureArrayFromElementArray::getHumanLabel()
{
  return "Create Feature Array from Element Array";
}

