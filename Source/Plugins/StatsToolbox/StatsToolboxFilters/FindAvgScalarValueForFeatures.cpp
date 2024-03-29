/* ============================================================================
 * Copyright (c) 2009-2016 BlueQuartz Software, LLC
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
 * The code contained herein was partially funded by the following contracts:
 *    United States Air Force Prime Contract FA8650-07-D-5800
 *    United States Air Force Prime Contract FA8650-10-D-5210
 *    United States Prime Contract Navy N00173-07-C-2068
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include "FindAvgScalarValueForFeatures.h"

#include <QtCore/QTextStream>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/Common/TemplateHelpers.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/DataArrayCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"

#include "StatsToolbox/StatsToolboxConstants.h"
#include "StatsToolbox/StatsToolboxVersion.h"

/* Create Enumerations to allow the created Attribute Arrays to take part in renaming */
enum createdPathID : RenameDataPath::DataID_t
{
  DataArrayID30 = 30,
  DataArrayID31 = 31,
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindAvgScalarValueForFeatures::FindAvgScalarValueForFeatures() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindAvgScalarValueForFeatures::~FindAvgScalarValueForFeatures() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindAvgScalarValueForFeatures::setupFilterParameters()
{
  FilterParameterVectorType parameters;
  parameters.push_back(SeparatorFilterParameter::Create("Element Data", FilterParameter::Category::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateCategoryRequirement(SIMPL::Defaults::AnyPrimitive, 1, AttributeMatrix::Category::Element);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Attribute Array to Average", SelectedCellArrayPath, FilterParameter::Category::RequiredArray, FindAvgScalarValueForFeatures, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateCategoryRequirement(SIMPL::TypeNames::Int32, 1, AttributeMatrix::Category::Element);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Feature Ids", FeatureIdsArrayPath, FilterParameter::Category::RequiredArray, FindAvgScalarValueForFeatures, req));
  }
  parameters.push_back(SeparatorFilterParameter::Create("Feature Data", FilterParameter::Category::CreatedArray));
  {
    DataArrayCreationFilterParameter::RequirementType req = DataArrayCreationFilterParameter::CreateRequirement(AttributeMatrix::Category::Feature);
    parameters.push_back(SIMPL_NEW_DA_CREATION_FP("Scalar Feature Averages", NewFeatureArrayArrayPath, FilterParameter::Category::CreatedArray, FindAvgScalarValueForFeatures, req));
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
  setFeatureIdsArrayPath(reader->readDataArrayPath("FeatureIdsArrayPath", getFeatureIdsArrayPath()));
  setSelectedCellArrayPath(reader->readDataArrayPath("SelectedCellArrayPath", getSelectedCellArrayPath()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindAvgScalarValueForFeatures::initialize()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindAvgScalarValueForFeatures::dataCheck()
{
  clearErrorCode();
  clearWarningCode();

  std::vector<size_t> cDims(1, 1);
  m_FeatureIdsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>>(this, getFeatureIdsArrayPath(), cDims);
  if(nullptr != m_FeatureIdsPtr.lock())
  {
    m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  cDims[0] = 1;
  m_NewFeatureArrayPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>>(this, getNewFeatureArrayArrayPath(), 0.0, cDims, "", DataArrayID31);
  if(nullptr != m_NewFeatureArrayPtr.lock())
  {
    m_NewFeatureArray = m_NewFeatureArrayPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  m_InDataArrayPtr = getDataContainerArray()->getPrereqIDataArrayFromPath(this, getSelectedCellArrayPath());
  if(getErrorCode() < 0)
  {
    return;
  }
  if(nullptr != m_InDataArrayPtr.lock())
  {
    int32_t cDims = m_InDataArrayPtr.lock()->getNumberOfComponents();
    if(cDims != 1)
    {
      QString ss = QObject::tr("Selected array has number of components %1 and is not a scalar array. The path is %2").arg(cDims).arg(getSelectedCellArrayPath().serialize());
      setErrorCondition(-11003, ss);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template <typename T>
void findAverage(IDataArray::Pointer inDataPtr, FloatArrayType::Pointer averageArray, int32_t* fIds)
{
  typename DataArray<T>::Pointer inputDataPtr = std::dynamic_pointer_cast<DataArray<T>>(inDataPtr);

  T* cPtr = inputDataPtr->getPointer(0);
  float* aPtr = averageArray->getPointer(0);
  size_t numPoints = inputDataPtr->getNumberOfTuples();
  size_t numFeatures = averageArray->getNumberOfTuples();

  std::vector<float> counts(numFeatures, 0.0f);

  int32_t feature = 0;
  float value = 0.0f;
  for(size_t i = 0; i < numPoints; i++)
  {
    value = float(cPtr[i]);
    feature = fIds[i];
    aPtr[feature] += value;
    counts[feature] += 1.0f;
  }
  for(size_t i = 1; i < numFeatures; i++)
  {
    if(counts[i] == 0)
    {
      aPtr[i] = 0;
    }
    else
    {
      aPtr[i] /= counts[i];
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindAvgScalarValueForFeatures::execute()
{
  dataCheck();
  if(getErrorCode() < 0)
  {
    return;
  }

  EXECUTE_FUNCTION_TEMPLATE(this, findAverage, m_InDataArrayPtr.lock(), m_InDataArrayPtr.lock(), m_NewFeatureArrayPtr.lock(), m_FeatureIds)
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer FindAvgScalarValueForFeatures::newFilterInstance(bool copyFilterParameters) const
{
  FindAvgScalarValueForFeatures::Pointer filter = FindAvgScalarValueForFeatures::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FindAvgScalarValueForFeatures::getCompiledLibraryName() const
{
  return StatsToolboxConstants::StatsToolboxBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FindAvgScalarValueForFeatures::getBrandingString() const
{
  return "Statistics";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FindAvgScalarValueForFeatures::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << StatsToolbox::Version::Major() << "." << StatsToolbox::Version::Minor() << "." << StatsToolbox::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FindAvgScalarValueForFeatures::getGroupName() const
{
  return SIMPL::FilterGroups::StatisticsFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid FindAvgScalarValueForFeatures::getUuid() const
{
  return QUuid("{cd91b8fd-0383-5803-ad26-9a47d6c309b0}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FindAvgScalarValueForFeatures::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::MiscFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FindAvgScalarValueForFeatures::getHumanLabel() const
{
  return "Find Average Value of Scalars For Feature";
}

// -----------------------------------------------------------------------------
FindAvgScalarValueForFeatures::Pointer FindAvgScalarValueForFeatures::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<FindAvgScalarValueForFeatures> FindAvgScalarValueForFeatures::New()
{
  struct make_shared_enabler : public FindAvgScalarValueForFeatures
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString FindAvgScalarValueForFeatures::getNameOfClass() const
{
  return QString("FindAvgScalarValueForFeatures");
}

// -----------------------------------------------------------------------------
QString FindAvgScalarValueForFeatures::ClassName()
{
  return QString("FindAvgScalarValueForFeatures");
}

// -----------------------------------------------------------------------------
void FindAvgScalarValueForFeatures::setSelectedCellArrayPath(const DataArrayPath& value)
{
  m_SelectedCellArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath FindAvgScalarValueForFeatures::getSelectedCellArrayPath() const
{
  return m_SelectedCellArrayPath;
}

// -----------------------------------------------------------------------------
void FindAvgScalarValueForFeatures::setFeatureIdsArrayPath(const DataArrayPath& value)
{
  m_FeatureIdsArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath FindAvgScalarValueForFeatures::getFeatureIdsArrayPath() const
{
  return m_FeatureIdsArrayPath;
}

// -----------------------------------------------------------------------------
void FindAvgScalarValueForFeatures::setNewFeatureArrayArrayPath(const DataArrayPath& value)
{
  m_NewFeatureArrayArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath FindAvgScalarValueForFeatures::getNewFeatureArrayArrayPath() const
{
  return m_NewFeatureArrayArrayPath;
}
