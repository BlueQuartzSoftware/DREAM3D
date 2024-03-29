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
#include "FitCorrelatedFeatureData.h"

#include <QtCore/QTextStream>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/DataContainers/DataContainer.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/ChoiceFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArrayCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/IntFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedBooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"

#include "StatsToolbox/DistributionAnalysisOps/BetaOps.h"
#include "StatsToolbox/DistributionAnalysisOps/LogNormalOps.h"
#include "StatsToolbox/DistributionAnalysisOps/PowerLawOps.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FitCorrelatedFeatureData::FitCorrelatedFeatureData() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FitCorrelatedFeatureData::~FitCorrelatedFeatureData() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FitCorrelatedFeatureData::setupFilterParameters()
{
  FilterParameterVectorType parameters;

  {
    ChoiceFilterParameter::Pointer parameter = ChoiceFilterParameter::New();
    parameter->setHumanLabel("Distribution Type");
    parameter->setPropertyName("DistributionType");
    parameter->setSetterCallback(SIMPL_BIND_SETTER(FitCorrelatedFeatureData, this, DistributionType));
    parameter->setGetterCallback(SIMPL_BIND_GETTER(FitCorrelatedFeatureData, this, DistributionType));

    // parameter->setValueType("unsigned int");
    std::vector<QString> choices;
    choices.push_back("Beta");
    choices.push_back("LogNormal");
    choices.push_back("Power Law");
    parameter->setChoices(choices);
    parameter->setCategory(FilterParameter::Category::Parameter);
    parameters.push_back(parameter);
  }
  parameters.push_back(SIMPL_NEW_INTEGER_FP("Number Of Bins For Correlated Array", NumberOfCorrelatedBins, FilterParameter::Category::Parameter, FitCorrelatedFeatureData));
  std::vector<QString> linkedProps = {"BiasedFeaturesArrayPath"};
  parameters.push_back(SIMPL_NEW_LINKED_BOOL_FP("Remove Biased Features", RemoveBiasedFeatures, FilterParameter::Category::Parameter, FitCorrelatedFeatureData, linkedProps));

  {
    DataArraySelectionFilterParameter::RequirementType req;
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Feature Array To Fit", SelectedFeatureArrayPath, FilterParameter::Category::RequiredArray, FitCorrelatedFeatureData, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req;
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Array To Correlate With", CorrelatedFeatureArrayPath, FilterParameter::Category::RequiredArray, FitCorrelatedFeatureData, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req;
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("FeaturePhases", FeaturePhasesArrayPath, FilterParameter::Category::RequiredArray, FitCorrelatedFeatureData, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req;
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("BiasedFeatures", BiasedFeaturesArrayPath, FilterParameter::Category::RequiredArray, FitCorrelatedFeatureData, req));
  }
  {
    DataArrayCreationFilterParameter::RequirementType req = DataArrayCreationFilterParameter::CreateRequirement(AttributeMatrix::Category::Ensemble);
    parameters.push_back(SIMPL_NEW_DA_CREATION_FP("New Ensemble Array", NewEnsembleArrayArrayPath, FilterParameter::Category::CreatedArray, FitCorrelatedFeatureData, req));
  }

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FitCorrelatedFeatureData::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setNewEnsembleArrayArrayPath(reader->readDataArrayPath("NewEnsembleArrayArrayPath", getNewEnsembleArrayArrayPath()));
  setBiasedFeaturesArrayPath(reader->readDataArrayPath("BiasedFeaturesArrayPath", getBiasedFeaturesArrayPath()));
  setFeaturePhasesArrayPath(reader->readDataArrayPath("FeaturePhasesArrayPath", getFeaturePhasesArrayPath()));
  setSelectedFeatureArrayPath(reader->readDataArrayPath("SelectedFeatureArrayPath", getSelectedFeatureArrayPath()));
  setCorrelatedFeatureArrayPath(reader->readDataArrayPath("CorrelatedFeatureArrayPath", getCorrelatedFeatureArrayPath()));
  setDistributionType(reader->readValue("DistributionType", getDistributionType()));
  setRemoveBiasedFeatures(reader->readValue("RemoveBiasedFeatures", getRemoveBiasedFeatures()));
  setNumberOfCorrelatedBins(reader->readValue("NumberOfCorrelatedBins", getNumberOfCorrelatedBins()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FitCorrelatedFeatureData::initialize()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FitCorrelatedFeatureData::dataCheck()
{
  clearErrorCode();
  clearWarningCode();

  std::vector<size_t> dims(1, 1);
  m_FeaturePhasesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>>(this, getFeaturePhasesArrayPath(), dims);
  if(nullptr != m_FeaturePhasesPtr.lock())
  {
    m_FeaturePhases = m_FeaturePhasesPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  if(m_SelectedFeatureArrayPath.isEmpty())
  {
    setErrorCondition(-11000, "An array from the Volume DataContainer must be selected.");
  }
  if(m_CorrelatedFeatureArrayPath.isEmpty())
  {
    setErrorCondition(-11000, "An array from the Volume DataContainer must be selected.");
  }

  int numComp = 0;
  QString distType("UNKNOWN");
  // Determining number of components and name given distribution type
  if(m_DistributionType == SIMPL::DistributionType::Beta)
  {
    distType = "Beta", numComp = SIMPL::DistributionType::BetaColumnCount;
  }
  else if(m_DistributionType == SIMPL::DistributionType::LogNormal)
  {
    distType = "LogNormal", numComp = SIMPL::DistributionType::LogNormalColumnCount;
  }
  else if(m_DistributionType == SIMPL::DistributionType::Power)
  {
    distType = "PowerLaw", numComp = SIMPL::DistributionType::PowerLawColumnCount;
  }

  getNewEnsembleArrayArrayPath().setDataArrayName(m_SelectedFeatureArrayPath.getDataArrayName() + distType + QString("Fit") + QString("CorrelatedTo") +
                                                  m_CorrelatedFeatureArrayPath.getDataArrayName());
  dims.resize(2);
  dims[0] = m_NumberOfCorrelatedBins;
  dims[1] = numComp;
  m_NewEnsembleArrayPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>>(
      this, getNewEnsembleArrayArrayPath(), 0, dims); /* Assigns the shared_ptr<>(this, tempPath, 0, dims); Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_NewEnsembleArrayPtr.lock())
  {
    m_NewEnsembleArray = m_NewEnsembleArrayPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  if(m_RemoveBiasedFeatures)
  {
    dims.resize(1);
    dims[0] = 1;
    m_BiasedFeaturesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<bool>>(this, getBiasedFeaturesArrayPath(), dims);
    if(nullptr != m_BiasedFeaturesPtr.lock())
    {
      m_BiasedFeatures = m_BiasedFeaturesPtr.lock()->getPointer(0);
    } /* Now assign the raw pointer to data from the DataArray<T> object */
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template <typename T>
void fitData(IDataArray::Pointer inputData, float* ensembleArray, int32_t* eIds, int32_t numEnsembles, unsigned int dType, Int32ArrayType::Pointer binArray, int numBins, bool removeBiasedFeatures,
             bool* biasedFeatures)
{
  StatsData::Pointer sData = StatsData::New();

  std::vector<DistributionAnalysisOps::Pointer> distributionAnalysis;
  distributionAnalysis.push_back(BetaOps::New());
  distributionAnalysis.push_back(LogNormalOps::New());
  distributionAnalysis.push_back(PowerLawOps::New());

  typename DataArray<T>::Pointer featureArray = std::dynamic_pointer_cast<DataArray<T>>(inputData);
  if(nullptr == featureArray.get())
  {
    return;
  }
  QString ss;
  QString distType;
  int numComp = 1;

  // Determining number of components and name given distribution type
  if(dType == SIMPL::DistributionType::Beta)
  {
    distType = "Beta", numComp = SIMPL::DistributionType::BetaColumnCount;
  }
  else if(dType == SIMPL::DistributionType::LogNormal)
  {
    distType = "LogNormal", numComp = SIMPL::DistributionType::LogNormalColumnCount;
  }
  else if(dType == SIMPL::DistributionType::Power)
  {
    distType = "PowerLaw", numComp = SIMPL::DistributionType::PowerLawColumnCount;
  }

  T* fPtr = featureArray->getPointer(0);
  int32_t* bPtr = binArray->getPointer(0);

  // float max;
  // float min;
  std::vector<VectorOfFloatArray> dist;
  std::vector<std::vector<std::vector<float>>> values;

  size_t numfeatures = featureArray->getNumberOfTuples();

  dist.resize(numEnsembles);
  values.resize(numEnsembles);

  for(int64_t i = 1; i < numEnsembles; i++)
  {
    dist[i] = sData->CreateCorrelatedDistributionArrays(dType, numBins);
    values[i].resize(numBins);
  }

  int32_t ensemble;
  for(size_t i = 1; i < numfeatures; i++)
  {
    if(!removeBiasedFeatures || !biasedFeatures[i])
    {
      ensemble = eIds[i];
      values[ensemble][bPtr[i]].push_back(static_cast<float>(fPtr[i]));
    }
  }
  for(int64_t i = 1; i < numEnsembles; i++)
  {
    distributionAnalysis[dType]->calculateCorrelatedParameters(values[i], dist[i]);
    for(int j = 0; j < numBins; j++)
    {
      for(int k = 0; k < numComp; k++)
      {
        VectorOfFloatArray data = dist[i];
        ensembleArray[(numComp * numBins * i) + (numComp * j) + k] = data[k]->getValue(j);
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template <typename T>
Int32ArrayType::Pointer binData(typename DataArray<T>::Pointer featureArray, int64_t numBins)
{
  if(nullptr == featureArray.get())
  {
    return Int32ArrayType::NullPointer();
  }

  T* fPtr = featureArray->getPointer(0);
  size_t numfeatures = featureArray->getNumberOfTuples();

  typename DataArray<int32_t>::Pointer binArray = DataArray<int32_t>::CreateArray(numfeatures, std::string("binIds"), true);
  int32_t* bPtr = binArray->getPointer(0);

  float max = -100000000.0;
  float min = 100000000.0;

  for(size_t i = 1; i < numfeatures; i++)
  {
    if(static_cast<T>(fPtr[i]) < min)
    {
      min = static_cast<T>(fPtr[i]);
    }
    if(static_cast<T>(fPtr[i]) > max)
    {
      max = static_cast<T>(fPtr[i]);
    }
  }
  // to make sure the max value feature doesn't walk off the end of the array, add a small value to the max
  max += 0.000001f;

  float step = (max - min) / numBins;
  for(size_t i = 1; i < numfeatures; i++)
  {
    bPtr[i] = (fPtr[i] - min) / step;
  }
  return binArray;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FitCorrelatedFeatureData::execute()
{
  dataCheck();
  if(getErrorCode() < 0)
  {
    return;
  }

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(m_SelectedFeatureArrayPath.getDataContainerName());
  int64_t numEnsembles = m_NewEnsembleArrayPtr.lock()->getNumberOfTuples();

  QString ss;

  IDataArray::Pointer inputData = m->getAttributeMatrix(m_SelectedFeatureArrayPath.getAttributeMatrixName())->getAttributeArray(m_SelectedFeatureArrayPath.getDataArrayName());
  if(nullptr == inputData.get())
  {
    ss = QObject::tr("Selected array '%1' does not exist in the Voxel Data Container. Was it spelled correctly?").arg(m_SelectedFeatureArrayPath.getDataArrayName());
    setErrorCondition(-11001, ss);
    return;
  }
  IDataArray::Pointer correlatedData = m->getAttributeMatrix(m_CorrelatedFeatureArrayPath.getAttributeMatrixName())->getAttributeArray(m_CorrelatedFeatureArrayPath.getDataArrayName());
  if(nullptr == correlatedData.get())
  {
    ss = QObject::tr("Selected array '%1' does not exist in the Voxel Data Container. Was it spelled correctly?").arg(m_CorrelatedFeatureArrayPath.getDataArrayName());
    setErrorCondition(-11001, ss);
    return;
  }

  // determine the bin of the correlated array each value of the array to fit falls in
  QString dType = correlatedData->getTypeAsString();
  Int32ArrayType::Pointer binArray = Int32ArrayType::NullPointer();
  if(dType.compare("int8_t") == 0)
  {
    binArray = binData<int8_t>(std::dynamic_pointer_cast<Int8ArrayType>(correlatedData), m_NumberOfCorrelatedBins);
  }
  else if(dType.compare("uint8_t") == 0)
  {
    binArray = binData<uint8_t>(std::dynamic_pointer_cast<UInt8ArrayType>(correlatedData), m_NumberOfCorrelatedBins);
  }
  else if(dType.compare("int16_t") == 0)
  {
    binArray = binData<int16_t>(std::dynamic_pointer_cast<Int16ArrayType>(correlatedData), m_NumberOfCorrelatedBins);
  }
  else if(dType.compare("uint16_t") == 0)
  {
    binArray = binData<uint16_t>(std::dynamic_pointer_cast<UInt16ArrayType>(correlatedData), m_NumberOfCorrelatedBins);
  }
  else if(dType.compare("int32_t") == 0)
  {
    binArray = binData<int32_t>(std::dynamic_pointer_cast<Int32ArrayType>(correlatedData), m_NumberOfCorrelatedBins);
  }
  else if(dType.compare("uint32_t") == 0)
  {
    binArray = binData<uint32_t>(std::dynamic_pointer_cast<UInt32ArrayType>(correlatedData), m_NumberOfCorrelatedBins);
  }
  else if(dType.compare("int64_t") == 0)
  {
    binArray = binData<int64_t>(std::dynamic_pointer_cast<Int64ArrayType>(correlatedData), m_NumberOfCorrelatedBins);
  }
  else if(dType.compare("uint64_t") == 0)
  {
    binArray = binData<uint64_t>(std::dynamic_pointer_cast<UInt64ArrayType>(correlatedData), m_NumberOfCorrelatedBins);
  }
  else if(dType.compare("float") == 0)
  {
    binArray = binData<float>(std::dynamic_pointer_cast<FloatArrayType>(correlatedData), m_NumberOfCorrelatedBins);
  }
  else if(dType.compare("double") == 0)
  {
    binArray = binData<double>(std::dynamic_pointer_cast<DoubleArrayType>(correlatedData), m_NumberOfCorrelatedBins);
  }

  // fit the data
  dType = inputData->getTypeAsString();
  if(dType.compare("int8_t") == 0)
  {
    fitData<int8_t>(inputData, m_NewEnsembleArray, m_FeaturePhases, numEnsembles, m_DistributionType, binArray, m_NumberOfCorrelatedBins, m_RemoveBiasedFeatures, m_BiasedFeatures);
  }
  else if(dType.compare("uint8_t") == 0)
  {
    fitData<uint8_t>(inputData, m_NewEnsembleArray, m_FeaturePhases, numEnsembles, m_DistributionType, binArray, m_NumberOfCorrelatedBins, m_RemoveBiasedFeatures, m_BiasedFeatures);
  }
  else if(dType.compare("int16_t") == 0)
  {
    fitData<int16_t>(inputData, m_NewEnsembleArray, m_FeaturePhases, numEnsembles, m_DistributionType, binArray, m_NumberOfCorrelatedBins, m_RemoveBiasedFeatures, m_BiasedFeatures);
  }
  else if(dType.compare("uint16_t") == 0)
  {
    fitData<uint16_t>(inputData, m_NewEnsembleArray, m_FeaturePhases, numEnsembles, m_DistributionType, binArray, m_NumberOfCorrelatedBins, m_RemoveBiasedFeatures, m_BiasedFeatures);
  }
  else if(dType.compare("int32_t") == 0)
  {
    fitData<int32_t>(inputData, m_NewEnsembleArray, m_FeaturePhases, numEnsembles, m_DistributionType, binArray, m_NumberOfCorrelatedBins, m_RemoveBiasedFeatures, m_BiasedFeatures);
  }
  else if(dType.compare("uint32_t") == 0)
  {
    fitData<uint32_t>(inputData, m_NewEnsembleArray, m_FeaturePhases, numEnsembles, m_DistributionType, binArray, m_NumberOfCorrelatedBins, m_RemoveBiasedFeatures, m_BiasedFeatures);
  }
  else if(dType.compare("int64_t") == 0)
  {
    fitData<int64_t>(inputData, m_NewEnsembleArray, m_FeaturePhases, numEnsembles, m_DistributionType, binArray, m_NumberOfCorrelatedBins, m_RemoveBiasedFeatures, m_BiasedFeatures);
  }
  else if(dType.compare("uint64_t") == 0)
  {
    fitData<uint64_t>(inputData, m_NewEnsembleArray, m_FeaturePhases, numEnsembles, m_DistributionType, binArray, m_NumberOfCorrelatedBins, m_RemoveBiasedFeatures, m_BiasedFeatures);
  }
  else if(dType.compare("float") == 0)
  {
    fitData<float>(inputData, m_NewEnsembleArray, m_FeaturePhases, numEnsembles, m_DistributionType, binArray, m_NumberOfCorrelatedBins, m_RemoveBiasedFeatures, m_BiasedFeatures);
  }
  else if(dType.compare("double") == 0)
  {
    fitData<double>(inputData, m_NewEnsembleArray, m_FeaturePhases, numEnsembles, m_DistributionType, binArray, m_NumberOfCorrelatedBins, m_RemoveBiasedFeatures, m_BiasedFeatures);
  }
  else if(dType.compare("bool") == 0)
  {
    fitData<bool>(inputData, m_NewEnsembleArray, m_FeaturePhases, numEnsembles, m_DistributionType, binArray, m_NumberOfCorrelatedBins, m_RemoveBiasedFeatures, m_BiasedFeatures);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer FitCorrelatedFeatureData::newFilterInstance(bool copyFilterParameters) const
{
  FitCorrelatedFeatureData::Pointer filter = FitCorrelatedFeatureData::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FitCorrelatedFeatureData::getCompiledLibraryName() const
{
  return StatsToolboxConstants::StatsToolboxBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FitCorrelatedFeatureData::getBrandingString() const
{
  return "Statistics";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FitCorrelatedFeatureData::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << StatsToolbox::Version::Major() << "." << StatsToolbox::Version::Minor() << "." << StatsToolbox::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FitCorrelatedFeatureData::getGroupName() const
{
  return SIMPL::FilterGroups::StatisticsFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid FitCorrelatedFeatureData::getUuid() const
{
  return QUuid("{d9fb65ac-e75e-581b-8ebc-0129d797b546}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FitCorrelatedFeatureData::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::EnsembleStatsFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FitCorrelatedFeatureData::getHumanLabel() const
{
  return "Fit Correlated Distributions To Feature Data";
}

// -----------------------------------------------------------------------------
FitCorrelatedFeatureData::Pointer FitCorrelatedFeatureData::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<FitCorrelatedFeatureData> FitCorrelatedFeatureData::New()
{
  struct make_shared_enabler : public FitCorrelatedFeatureData
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString FitCorrelatedFeatureData::getNameOfClass() const
{
  return QString("FitCorrelatedFeatureData");
}

// -----------------------------------------------------------------------------
QString FitCorrelatedFeatureData::ClassName()
{
  return QString("FitCorrelatedFeatureData");
}

// -----------------------------------------------------------------------------
void FitCorrelatedFeatureData::setSelectedFeatureArrayPath(const DataArrayPath& value)
{
  m_SelectedFeatureArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath FitCorrelatedFeatureData::getSelectedFeatureArrayPath() const
{
  return m_SelectedFeatureArrayPath;
}

// -----------------------------------------------------------------------------
void FitCorrelatedFeatureData::setCorrelatedFeatureArrayPath(const DataArrayPath& value)
{
  m_CorrelatedFeatureArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath FitCorrelatedFeatureData::getCorrelatedFeatureArrayPath() const
{
  return m_CorrelatedFeatureArrayPath;
}

// -----------------------------------------------------------------------------
void FitCorrelatedFeatureData::setDistributionType(unsigned int value)
{
  m_DistributionType = value;
}

// -----------------------------------------------------------------------------
unsigned int FitCorrelatedFeatureData::getDistributionType() const
{
  return m_DistributionType;
}

// -----------------------------------------------------------------------------
void FitCorrelatedFeatureData::setNumberOfCorrelatedBins(int value)
{
  m_NumberOfCorrelatedBins = value;
}

// -----------------------------------------------------------------------------
int FitCorrelatedFeatureData::getNumberOfCorrelatedBins() const
{
  return m_NumberOfCorrelatedBins;
}

// -----------------------------------------------------------------------------
void FitCorrelatedFeatureData::setRemoveBiasedFeatures(bool value)
{
  m_RemoveBiasedFeatures = value;
}

// -----------------------------------------------------------------------------
bool FitCorrelatedFeatureData::getRemoveBiasedFeatures() const
{
  return m_RemoveBiasedFeatures;
}

// -----------------------------------------------------------------------------
void FitCorrelatedFeatureData::setFeaturePhasesArrayPath(const DataArrayPath& value)
{
  m_FeaturePhasesArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath FitCorrelatedFeatureData::getFeaturePhasesArrayPath() const
{
  return m_FeaturePhasesArrayPath;
}

// -----------------------------------------------------------------------------
void FitCorrelatedFeatureData::setBiasedFeaturesArrayPath(const DataArrayPath& value)
{
  m_BiasedFeaturesArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath FitCorrelatedFeatureData::getBiasedFeaturesArrayPath() const
{
  return m_BiasedFeaturesArrayPath;
}

// -----------------------------------------------------------------------------
void FitCorrelatedFeatureData::setNewEnsembleArrayArrayPath(const DataArrayPath& value)
{
  m_NewEnsembleArrayArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath FitCorrelatedFeatureData::getNewEnsembleArrayArrayPath() const
{
  return m_NewEnsembleArrayArrayPath;
}
