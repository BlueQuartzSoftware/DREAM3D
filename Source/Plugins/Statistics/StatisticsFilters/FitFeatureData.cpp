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


#include "FitFeatureData.h"

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/Common/TemplateHelpers.hpp"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersWriter.h"

#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArrayCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedBooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/ChoiceFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"

#include "Statistics/DistributionAnalysisOps/BetaOps.h"
#include "Statistics/DistributionAnalysisOps/PowerLawOps.h"
#include "Statistics/DistributionAnalysisOps/LogNormalOps.h"

#include "Statistics/StatisticsConstants.h"

// Include the MOC generated file for this class
#include "moc_FitFeatureData.cpp"



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FitFeatureData::FitFeatureData() :
  AbstractFilter(),
  m_SelectedFeatureArrayPath("", "", ""),
  m_DistributionType(DREAM3D::DistributionType::UnknownDistributionType),
  m_RemoveBiasedFeatures(false),
  m_FeaturePhasesArrayPath("", "", ""),
  m_BiasedFeaturesArrayPath("", "", ""),
  m_NewEnsembleArrayArray("", "", ""),
  m_InDataArray(NULL),
  m_BiasedFeatures(NULL),
  m_FeaturePhases(NULL),
  m_NewEnsembleArray(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FitFeatureData::~FitFeatureData()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FitFeatureData::setupFilterParameters()
{
  FilterParameterVector parameters;
  {
    ChoiceFilterParameter::Pointer parameter = ChoiceFilterParameter::New();
    parameter->setHumanLabel("Distribution Type");
    parameter->setPropertyName("DistributionType");

    QVector<QString> choices;
    choices.push_back("Beta");
    choices.push_back("Log-Normal");
    choices.push_back("Power Law");
    parameter->setChoices(choices);
    parameter->setCategory(FilterParameter::Parameter);
    parameters.push_back(parameter);
  }
  QStringList linkedProps("BiasedFeaturesArrayPath");
  parameters.push_back(LinkedBooleanFilterParameter::New("Remove Biased Features", "RemoveBiasedFeatures", getRemoveBiasedFeatures(), linkedProps, FilterParameter::Parameter));
  parameters.push_back(SeparatorFilterParameter::New("Feature Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateCategoryRequirement(DREAM3D::Defaults::AnyPrimitive, 1, DREAM3D::AttributeMatrixObjectType::Feature);
    parameters.push_back(DataArraySelectionFilterParameter::New("Feature Array to Fit", "SelectedFeatureArrayPath", getSelectedFeatureArrayPath(), FilterParameter::RequiredArray, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateCategoryRequirement(DREAM3D::TypeNames::Int32, 1, DREAM3D::AttributeMatrixObjectType::Feature);
    parameters.push_back(DataArraySelectionFilterParameter::New("Phases", "FeaturePhasesArrayPath", getFeaturePhasesArrayPath(), FilterParameter::RequiredArray, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateCategoryRequirement(DREAM3D::TypeNames::Bool, 1, DREAM3D::AttributeMatrixObjectType::Feature);
    parameters.push_back(DataArraySelectionFilterParameter::New("Biased Features", "BiasedFeaturesArrayPath", getBiasedFeaturesArrayPath(), FilterParameter::RequiredArray, req));
  }
  parameters.push_back(SeparatorFilterParameter::New("Ensemble Data", FilterParameter::CreatedArray));
  {
    DataArrayCreationFilterParameter::RequirementType req = DataArrayCreationFilterParameter::CreateRequirement(DREAM3D::AttributeMatrixObjectType::Ensemble);
    parameters.push_back(DataArrayCreationFilterParameter::New("Fit Parameters", "NewEnsembleArrayArray", getNewEnsembleArrayArray(), FilterParameter::CreatedArray, req));
  }
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FitFeatureData::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setNewEnsembleArrayArray(reader->readDataArrayPath("NewEnsembleArrayArray", getNewEnsembleArrayArray()));
  setBiasedFeaturesArrayPath(reader->readDataArrayPath("BiasedFeaturesArrayPath", getBiasedFeaturesArrayPath() ) );
  setFeaturePhasesArrayPath(reader->readDataArrayPath("FeaturePhasesArrayPath", getFeaturePhasesArrayPath() ) );
  setSelectedFeatureArrayPath( reader->readDataArrayPath( "SelectedFeatureArrayPath", getSelectedFeatureArrayPath() ) );
  setDistributionType( reader->readValue( "DistributionType", getDistributionType() ) );
  setRemoveBiasedFeatures( reader->readValue( "RemoveBiasedFeatures", getRemoveBiasedFeatures() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int FitFeatureData::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  SIMPL_FILTER_WRITE_PARAMETER(FilterVersion)
  SIMPL_FILTER_WRITE_PARAMETER(NewEnsembleArrayArray)
  SIMPL_FILTER_WRITE_PARAMETER(BiasedFeaturesArrayPath)
  SIMPL_FILTER_WRITE_PARAMETER(FeaturePhasesArrayPath)
  SIMPL_FILTER_WRITE_PARAMETER(SelectedFeatureArrayPath)
  SIMPL_FILTER_WRITE_PARAMETER(DistributionType)
  SIMPL_FILTER_WRITE_PARAMETER(RemoveBiasedFeatures)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FitFeatureData::dataCheck()
{
  setErrorCondition(0);

  QVector<size_t> cDims(1, 1);
  m_FeaturePhasesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getFeaturePhasesArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeaturePhasesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeaturePhases = m_FeaturePhasesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  m_InDataArrayPtr = getDataContainerArray()->getPrereqIDataArrayFromPath<IDataArray, AbstractFilter>(this, getSelectedFeatureArrayPath());

  int32_t numComp = 0;
  QString distType("UNKNOWN");
  // Determining number of components and name given distribution type
  if (m_DistributionType == DREAM3D::DistributionType::Beta) { distType = "Beta", numComp = DREAM3D::DistributionType::BetaColumnCount; }
  else if (m_DistributionType == DREAM3D::DistributionType::LogNormal) { distType = "LogNormal", numComp = DREAM3D::DistributionType::LogNormalColumnCount; }
  else if (m_DistributionType == DREAM3D::DistributionType::Power) { distType = "PowerLaw", numComp = DREAM3D::DistributionType::PowerLawColumnCount; }

  getNewEnsembleArrayArray().setDataArrayName(m_SelectedFeatureArrayPath.getDataArrayName() + distType + QString("Fit"));
  cDims[0] = numComp;
  m_NewEnsembleArrayPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getNewEnsembleArrayArray(), 0, cDims); /* Assigns the shared_ptr<>(this, tempPath, 0, dims); Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_NewEnsembleArrayPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_NewEnsembleArray = m_NewEnsembleArrayPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  if(m_RemoveBiasedFeatures == true)
  {
    cDims[0] = 1;
    m_BiasedFeaturesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<bool>, AbstractFilter>(this, getBiasedFeaturesArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if( NULL != m_BiasedFeaturesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    { m_BiasedFeatures = m_BiasedFeaturesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FitFeatureData::preflight()
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
void fitData(IDataArray::Pointer inDataPtr, float* ensembleArray, int32_t* eIds, size_t numEnsembles, uint32_t dType, bool removeBiasedFeatures, bool* biasedFeatures)
{
  typename DataArray<T>::Pointer inputDataPtr = std::dynamic_pointer_cast<DataArray<T> >(inDataPtr);

  StatsData::Pointer sData = StatsData::New();

  std::vector<DistributionAnalysisOps::Pointer> distributionAnalysis;
  distributionAnalysis.push_back(BetaOps::New());
  distributionAnalysis.push_back(LogNormalOps::New());
  distributionAnalysis.push_back(PowerLawOps::New());

  QString distType;
  int32_t numComp = 1;

  // Determining number of components and name given distribution type
  if (dType == DREAM3D::DistributionType::Beta) { distType = "Beta", numComp = DREAM3D::DistributionType::BetaColumnCount; }
  else if (dType == DREAM3D::DistributionType::LogNormal) { distType = "LogNormal", numComp = DREAM3D::DistributionType::LogNormalColumnCount; }
  else if (dType == DREAM3D::DistributionType::Power) { distType = "PowerLaw", numComp = DREAM3D::DistributionType::PowerLawColumnCount; }

  T* fPtr = inputDataPtr->getPointer(0);

  std::vector<FloatArrayType::Pointer> dist;
  std::vector<std::vector<float > > values;

  size_t numfeatures = inputDataPtr->getNumberOfTuples();

  dist.resize(numEnsembles);
  values.resize(numEnsembles);

  for(size_t i = 1; i < numEnsembles; i++)
  {
    dist[i] = sData->CreateDistributionArrays(dType);
  }

  int32_t ensemble = 0;
  for (size_t i = 1; i < numfeatures; i++)
  {
    if (removeBiasedFeatures == false || biasedFeatures[i] == false)
    {
      ensemble = eIds[i];
      values[ensemble].push_back(static_cast<float>(fPtr[i]));
    }
  }
  for (size_t i = 1; i < numEnsembles; i++)
  {
    distributionAnalysis[dType]->calculateParameters(values[i], dist[i]);
    for (int32_t j = 0; j < numComp; j++)
    {
      FloatArrayType::Pointer data = dist[i];
      ensembleArray[numComp * i + j] = data->getValue(j);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FitFeatureData::execute()
{
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  size_t numEnsembles = m_NewEnsembleArrayPtr.lock()->getNumberOfTuples();

  EXECUTE_FUNCTION_TEMPLATE(this, fitData, m_InDataArrayPtr.lock(), m_InDataArrayPtr.lock(), m_NewEnsembleArray, m_FeaturePhases, numEnsembles, m_DistributionType, m_RemoveBiasedFeatures, m_BiasedFeatures)

  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer FitFeatureData::newFilterInstance(bool copyFilterParameters)
{
  FitFeatureData::Pointer filter = FitFeatureData::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FitFeatureData::getCompiledLibraryName()
{ return StatisticsConstants::StatisticsBaseName; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FitFeatureData::getGroupName()
{ return DREAM3D::FilterGroups::StatisticsFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FitFeatureData::getSubGroupName()
{ return DREAM3D::FilterSubGroups::EnsembleStatsFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FitFeatureData::getHumanLabel()
{ return "Fit Distribution to Feature Data"; }
