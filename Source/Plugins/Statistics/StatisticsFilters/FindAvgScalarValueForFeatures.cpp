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


#include "FindAvgScalarValueForFeatures.h"

#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Common/TemplateHelpers.hpp"
#include "DREAM3DLib/FilterParameters/AbstractFilterParametersReader.h"
#include "DREAM3DLib/FilterParameters/AbstractFilterParametersWriter.h"

#include "DREAM3DLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "DREAM3DLib/FilterParameters/DataArrayCreationFilterParameter.h"
#include "DREAM3DLib/FilterParameters/SeparatorFilterParameter.h"

#include "Statistics/StatisticsConstants.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindAvgScalarValueForFeatures::FindAvgScalarValueForFeatures() :
  AbstractFilter(),
  m_SelectedCellArrayPath("", "", ""),
  m_FeatureIdsArrayPath("", "", ""),
  m_NewFeatureArrayArrayPath("", "", ""),
  m_InDataArray(NULL),
  m_FeatureIds(NULL),
  m_NewFeatureArray(NULL)
{

  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindAvgScalarValueForFeatures::~FindAvgScalarValueForFeatures()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindAvgScalarValueForFeatures::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(SeparatorFilterParameter::New("Element Data", FilterParameter::RequiredArray));
  {
 	DataArraySelectionFilterParameter::DataStructureRequirements req = DataArraySelectionFilterParameter::CreateCategoryRequirement(QString(""), 1, DREAM3D::AttributeMatrixObjectType::Element);
    parameters.push_back(DataArraySelectionFilterParameter::New("Attribute Array to Average", "SelectedCellArrayPath", getSelectedCellArrayPath(), FilterParameter::RequiredArray, req));
  }
  {
    DataArraySelectionFilterParameter::DataStructureRequirements req = DataArraySelectionFilterParameter::CreateCategoryRequirement(DREAM3D::TypeNames::Int32, 1, DREAM3D::AttributeMatrixObjectType::Element);
    parameters.push_back(DataArraySelectionFilterParameter::New("Feature Ids", "FeatureIdsArrayPath", getFeatureIdsArrayPath(), FilterParameter::RequiredArray, req));
  }
  parameters.push_back(SeparatorFilterParameter::New("Feature Data", FilterParameter::CreatedArray));
  {
    DataArrayCreationFilterParameter::DataStructureRequirements req = DataArrayCreationFilterParameter::CreateRequirement(DREAM3D::AttributeMatrixObjectType::Feature);
    parameters.push_back(DataArrayCreationFilterParameter::New("Scalar Feature Averages", "NewFeatureArrayArrayPath", getNewFeatureArrayArrayPath(), FilterParameter::CreatedArray, req));
  }
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindAvgScalarValueForFeatures::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setNewFeatureArrayArrayPath(reader->readDataArrayPath("NewFeatureArrayArrayPath", getNewFeatureArrayArrayPath()));
  setFeatureIdsArrayPath(reader->readDataArrayPath("FeatureIdsArrayPath", getFeatureIdsArrayPath() ) );
  setSelectedCellArrayPath( reader->readDataArrayPath( "SelectedCellArrayPath", getSelectedCellArrayPath() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int FindAvgScalarValueForFeatures::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  DREAM3D_FILTER_WRITE_PARAMETER(FilterVersion)
  DREAM3D_FILTER_WRITE_PARAMETER(NewFeatureArrayArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(FeatureIdsArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(SelectedCellArrayPath)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindAvgScalarValueForFeatures::dataCheck()
{
  setErrorCondition(0);

  QVector<size_t> cDims(1, 1);
  m_FeatureIdsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getFeatureIdsArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeatureIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  cDims[0] = 1;
  m_NewFeatureArrayPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this, getNewFeatureArrayArrayPath(), 0.0, cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_NewFeatureArrayPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_NewFeatureArray = m_NewFeatureArrayPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  m_InDataArrayPtr = getDataContainerArray()->getPrereqIDataArrayFromPath<IDataArray, AbstractFilter>(this, getSelectedCellArrayPath());
  if (getErrorCondition() < 0) { return; }
  if (NULL != m_InDataArrayPtr.lock().get())
  {
    int32_t cDims = m_InDataArrayPtr.lock()->getNumberOfComponents();
    QString ss = QObject::tr("Selected array has number of components %1 and is not a scalar array. The path is %2").arg(cDims).arg(getSelectedCellArrayPath().serialize());
    setErrorCondition(-11003);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindAvgScalarValueForFeatures::preflight()
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
void findAverage(IDataArray::Pointer inDataPtr, FloatArrayType::Pointer averageArray, int32_t* fIds)
{
  typename DataArray<T>::Pointer inputDataPtr = boost::dynamic_pointer_cast<DataArray<T> >(inDataPtr);

  T* cPtr = inputDataPtr->getPointer(0);
  float* aPtr = averageArray->getPointer(0);
  size_t numPoints = inputDataPtr->getNumberOfTuples();
  size_t numFeatures = averageArray->getNumberOfTuples();

  std::vector<float> counts(numFeatures, 0.0f);

  int32_t feature = 0;
  float value = 0.0f;
  for (size_t i = 0; i < numPoints; i++)
  {
    value = float(cPtr[i]);
    feature = fIds[i];
    aPtr[feature] += value;
    counts[feature] += 1.0f;
  }
  for (size_t i = 1; i < numFeatures; i++)
  {
    if (counts[i] == 0) { aPtr[i] = 0; }
    else { aPtr[i] /= counts[i]; }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindAvgScalarValueForFeatures::execute()
{
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  EXECUTE_FUNCTION_TEMPLATE(this, findAverage, m_InDataArrayPtr.lock(), m_InDataArrayPtr.lock(), m_NewFeatureArrayPtr.lock(), m_FeatureIds)

  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer FindAvgScalarValueForFeatures::newFilterInstance(bool copyFilterParameters)
{
  FindAvgScalarValueForFeatures::Pointer filter = FindAvgScalarValueForFeatures::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindAvgScalarValueForFeatures::getCompiledLibraryName()
{ return StatisticsConstants::StatisticsBaseName; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindAvgScalarValueForFeatures::getGroupName()
{ return DREAM3D::FilterGroups::StatisticsFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindAvgScalarValueForFeatures::getSubGroupName()
{ return DREAM3D::FilterSubGroups::MiscFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindAvgScalarValueForFeatures::getHumanLabel()
{ return "Find Average Value of Scalars For Feature"; }
