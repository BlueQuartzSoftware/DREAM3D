/* ============================================================================
 * Copyright (c) 2011 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2011 Dr. Michael A. Groeber (US Air Force Research Laboratories)
 * Copyright (c) 2013 Dr. Joseph C. Tucker (UES, Inc.)
 * All rights reserved.
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
 * Neither the name of Joseph C. Tucker, Michael A. Groeber, Michael A. Jackson,
 * UES, Inc., the US Air Force, BlueQuartz Software nor the names of its contributors  
 * may be used to endorse or promote products derived from this software without  
 * specific prior written permission.
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
 *  This code was written under United States Air Force Contract number
 *                   FA8650-07-D-5800 and FA8650-10-D-5226
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "FitCorrelatedFeatureData.h"

#include "DREAM3DLib/Common/Constants.h"

#include "DREAM3DLib/DistributionAnalysisOps/BetaOps.h"
#include "DREAM3DLib/DistributionAnalysisOps/PowerLawOps.h"
#include "DREAM3DLib/DistributionAnalysisOps/LogNormalOps.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FitCorrelatedFeatureData::FitCorrelatedFeatureData() :
  AbstractFilter(),
  m_DataContainerName(DREAM3D::Defaults::VolumeDataContainerName),
  m_CellFeatureAttributeMatrixName(DREAM3D::Defaults::CellFeatureAttributeMatrixName),
  m_CellEnsembleAttributeMatrixName(DREAM3D::Defaults::CellEnsembleAttributeMatrixName),
  m_SelectedFeatureArrayName(""),
  m_CorrelatedFeatureArrayName(""),
  m_DistributionType(DREAM3D::DistributionType::UnknownDistributionType),
  m_NumberOfCorrelatedBins(1),
  m_RemoveBiasedFeatures(false),
  m_BiasedFeaturesArrayName(DREAM3D::FeatureData::BiasedFeatures),
  m_BiasedFeatures(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FitCorrelatedFeatureData::~FitCorrelatedFeatureData()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FitCorrelatedFeatureData::setupFilterParameters()
{
  FilterParameterVector parameters;
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Feature Array To Fit");
    option->setPropertyName("SelectedFeatureArrayName");
    option->setWidgetType(FilterParameter::VolumeFeatureArrayNameSelectionWidget);
    option->setValueType("string");
    option->setUnits("");
    parameters.push_back(option);
  }
  {
    ChoiceFilterParameter::Pointer option = ChoiceFilterParameter::New();
    option->setHumanLabel("Distribution Type");
    option->setPropertyName("DistributionType");
    option->setWidgetType(FilterParameter::ChoiceWidget);
    option->setValueType("unsigned int");
    QVector<QString> choices;
    choices.push_back("Beta");
    choices.push_back("LogNormal");
    choices.push_back("Power Law");
    option->setChoices(choices);
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Array To Correlate With");
    option->setPropertyName("CorrelatedFeatureArrayName");
    option->setWidgetType(FilterParameter::VolumeFeatureArrayNameSelectionWidget);
    option->setValueType("string");
    option->setUnits("");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Number Of Bins For Correlated Array");
    option->setPropertyName("NumberOfCorrelatedBins");
    option->setWidgetType(FilterParameter::IntWidget);
    option->setValueType("int");
    option->setUnits("");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Remove Biased Features");
    option->setPropertyName("RemoveBiasedFeatures");
    option->setWidgetType(FilterParameter::BooleanWidget);
    option->setValueType("bool");
    option->setUnits("");
    parameters.push_back(option);
  }
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FitCorrelatedFeatureData::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  /* Code to read the values goes between these statements */
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE BEGIN*/
  setSelectedFeatureArrayName( reader->readString( "SelectedFeatureArrayName", getSelectedFeatureArrayName() ) );
  setDistributionType( reader->readValue( "DistributionType", getDistributionType() ) );
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE END*/
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int FitCorrelatedFeatureData::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("SelectedFeatureArrayName", getSelectedFeatureArrayName() );
  writer->writeValue("DistributionType", getDistributionType() );
  writer->writeValue("RemovedBiasedFeatures", getRemoveBiasedFeatures() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FitCorrelatedFeatureData::dataCheck()
{
  setErrorCondition(0);

  VolumeDataContainer* m = getDataContainerArray()->getPrereqDataContainer<VolumeDataContainer, FitCorrelatedFeatureData>(this, getDataContainerName(), false);
  if(getErrorCondition() < 0) { return; }
  AttributeMatrix* cellFeatureAttrMat = m->getPrereqAttributeMatrix<AbstractFilter>(this, getCellFeatureAttributeMatrixName(), -301);
  if(getErrorCondition() < 0) { return; }

  if(m_SelectedFeatureArrayName.isEmpty() == true)
  {
    setErrorCondition(-11000);
    PipelineMessage em (getHumanLabel(), "An array from the Volume DataContainer must be selected.", getErrorCondition(), PipelineMessage::Error);
    emit filterGeneratedMessage(em);
  }
  if(m_CorrelatedFeatureArrayName.isEmpty() == true)
  {
    setErrorCondition(-11000);
    PipelineMessage em (getHumanLabel(), "An array from the Volume DataContainer must be selected.", getErrorCondition(), PipelineMessage::Error);
    emit filterGeneratedMessage(em);
  }
  if(m_RemoveBiasedFeatures == true)
  {
    QVector<int> dims(1, 1);
    m_BiasedFeaturesPtr = cellFeatureAttrMat->getPrereqArray<DataArray<bool>, AbstractFilter>(this, m_BiasedFeaturesArrayName, -302, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if( NULL != m_BiasedFeaturesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    { m_BiasedFeatures = m_BiasedFeaturesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FitCorrelatedFeatureData::preflight()
{
  dataCheck();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T>
IDataArray::Pointer fitData(IDataArray::Pointer inputData, int64_t ensembles, QString selectedFeatureArrayName, unsigned int dType, Int32ArrayType::Pointer binArray, int numBins, bool removeBiasedFeatures, bool* biasedFeatures)
{
  StatsData::Pointer sData = StatsData::New();

  QVector<DistributionAnalysisOps::Pointer> m_DistributionAnalysis;
  m_DistributionAnalysis.push_back(BetaOps::New());
  m_DistributionAnalysis.push_back(LogNormalOps::New());
  m_DistributionAnalysis.push_back(PowerLawOps::New());

  DataArray<T>* featureArray = DataArray<T>::SafePointerDownCast(inputData.get());
  if (NULL == featureArray)
  {
    return IDataArray::NullPointer();
  }
  QString ss;
  QString distType;
  int numComp = 1;

// Determining number of components and name given distribution type
  if (dType == DREAM3D::DistributionType::Beta) { distType = "Beta", numComp = DREAM3D::DistributionType::BetaColumnCount; }
  else if (dType == DREAM3D::DistributionType::LogNormal) { distType = "LogNormal", numComp = DREAM3D::DistributionType::LogNormalColumnCount; }
  else if (dType == DREAM3D::DistributionType::Power) { distType = "PowerLaw", numComp = DREAM3D::DistributionType::PowerLawColumnCount; }

  ss = selectedFeatureArrayName + distType + QString("Fit");
  QVector<int> dims(2);
  dims[0] = numBins;
  dims[1] = numComp;
  typename DataArray<float>::Pointer ensembleArray = DataArray<float>::CreateArray(ensembles, dims, ss);

  T* fPtr = featureArray->getPointer(0);
  float* ePtr = ensembleArray->getPointer(0);
  int32_t* bPtr = binArray->getPointer(0);

  //float max;
  //float min;
  QVector<VectorOfFloatArray> dist;
  QVector<QVector<QVector<float > > > values;

  size_t numfeatures = featureArray->getNumberOfTuples();

  dist.resize(ensembles);
  values.resize(ensembles);

  for(int64_t i = 1; i < ensembles; i++)
  {
    dist[i] = sData->CreateCorrelatedDistributionArrays(dType, numBins);
    values[i].resize(numBins);
  }

  for (size_t i = 1; i < numfeatures; i++)
  {
    if(removeBiasedFeatures == false || biasedFeatures[i] == false)
    {
      values[1][bPtr[i]].push_back(static_cast<float>(fPtr[i]));
    }
  }
  for (int64_t i = 1; i < ensembles; i++)
  {
    m_DistributionAnalysis[dType]->calculateCorrelatedParameters(values[i], dist[i]);
    for (int j = 0; j < numBins; j++)
    {
      for(int k = 0; k < numComp; k++)
      {
        VectorOfFloatArray data = dist[i];
        ePtr[(numComp * numBins * i) + (numComp * j) + k] = data[k]->GetValue(j);
      }
    }
  }
  return ensembleArray;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T>
Int32ArrayType::Pointer binData(IDataArray::Pointer correlatedData, int64_t numBins)
{
  DataArray<T>* featureArray = DataArray<T>::SafePointerDownCast(correlatedData.get());
  if (NULL == featureArray)
  {
    return Int32ArrayType::NullPointer();
  }

  T* fPtr = featureArray->getPointer(0);
  size_t numfeatures = featureArray->getNumberOfTuples();

  typename DataArray<int32_t>::Pointer binArray = DataArray<int32_t>::CreateArray(numfeatures, "binIds");
  int32_t* bPtr = binArray->getPointer(0);

  float max = -100000000.0;
  float min = 100000000.0;

  for (size_t i = 1; i < numfeatures; i++)
  {
    if(fPtr[i] < min) { min = fPtr[i]; }
    if(fPtr[i] > max) { max = fPtr[i]; }
  }
  //to make sure the max value feature doesn't walk off the end of the array, add a small value to the max
  max += 0.000001;

  float step = (max - min) / numBins;
  for (size_t i = 1; i < numfeatures; i++)
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
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());
  int64_t ensembles = m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->getNumTuples();

  QString ss;

  IDataArray::Pointer inputData = m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->getAttributeArray(m_SelectedFeatureArrayName);

  if (NULL == inputData.get())
  {
    ss = QObject::tr("Selected array '%1' does not exist in the Voxel Data Container. Was it spelled correctly?").arg(m_SelectedFeatureArrayName);
    setErrorCondition(-11001);
    PipelineMessage em(getHumanLabel(), ss, getErrorCondition(), PipelineMessage::Error);
emit filterGeneratedMessage(em);
    return;
  }
  IDataArray::Pointer correlatedData = m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->getAttributeArray(m_CorrelatedFeatureArrayName);
  if (NULL == correlatedData.get())
  {
    ss = QObject::tr("Selected array '%1' does not exist in the Voxel Data Container. Was it spelled correctly?").arg(m_CorrelatedFeatureArrayName);
    setErrorCondition(-11001);
    PipelineMessage em(getHumanLabel(), ss, getErrorCondition(), PipelineMessage::Error);
emit filterGeneratedMessage(em);
    return;
  }

  //determine the bin of the correlated array each value of the array to fit falls in
  QString dType = correlatedData->getTypeAsString();
  Int32ArrayType::Pointer binArray = Int32ArrayType::NullPointer();
  if (dType.compare("int8_t") == 0)
  {
    binArray = binData<int8_t>(correlatedData, m_NumberOfCorrelatedBins);
  }
  else if (dType.compare("uint8_t") == 0)
  {
    binArray = binData<uint8_t>(correlatedData, m_NumberOfCorrelatedBins);
  }
  else if (dType.compare("int16_t") == 0)
  {
    binArray = binData<int16_t>(correlatedData, m_NumberOfCorrelatedBins);
  }
  else if (dType.compare("uint16_t") == 0)
  {
    binArray = binData<uint16_t>(correlatedData, m_NumberOfCorrelatedBins);
  }
  else if (dType.compare("int32_t") == 0)
  {
    binArray = binData<int32_t>(correlatedData, m_NumberOfCorrelatedBins);
  }
  else if (dType.compare("uint32_t") == 0)
  {
    binArray = binData<uint32_t>(correlatedData, m_NumberOfCorrelatedBins);
  }
  else if (dType.compare("int64_t") == 0)
  {
    binArray = binData<int64_t>(correlatedData, m_NumberOfCorrelatedBins);
  }
  else if (dType.compare("uint64_t") == 0)
  {
    binArray = binData<uint64_t>(correlatedData, m_NumberOfCorrelatedBins);
  }
  else if (dType.compare("float") == 0)
  {
    binArray = binData<float>(correlatedData, m_NumberOfCorrelatedBins);
  }
  else if (dType.compare("double") == 0)
  {
    binArray = binData<double>(correlatedData, m_NumberOfCorrelatedBins);
  }
  else if (dType.compare("bool") == 0)
  {
    binArray = binData<bool>(correlatedData, m_NumberOfCorrelatedBins);
  }

  // fit the data
  dType = inputData->getTypeAsString();
  IDataArray::Pointer p = IDataArray::NullPointer();
  if (dType.compare("int8_t") == 0)
  {
    p = fitData<int8_t>(inputData, ensembles, m_SelectedFeatureArrayName, m_DistributionType, binArray, m_NumberOfCorrelatedBins, m_RemoveBiasedFeatures, m_BiasedFeatures);
  }
  else if (dType.compare("uint8_t") == 0)
  {
    p = fitData<uint8_t>(inputData, ensembles, m_SelectedFeatureArrayName, m_DistributionType, binArray, m_NumberOfCorrelatedBins, m_RemoveBiasedFeatures, m_BiasedFeatures);
  }
  else if (dType.compare("int16_t") == 0)
  {
    p = fitData<int16_t>(inputData, ensembles, m_SelectedFeatureArrayName, m_DistributionType, binArray, m_NumberOfCorrelatedBins, m_RemoveBiasedFeatures, m_BiasedFeatures);
  }
  else if (dType.compare("uint16_t") == 0)
  {
    p = fitData<uint16_t>(inputData, ensembles, m_SelectedFeatureArrayName, m_DistributionType, binArray, m_NumberOfCorrelatedBins, m_RemoveBiasedFeatures, m_BiasedFeatures);
  }
  else if (dType.compare("int32_t") == 0)
  {
    p = fitData<int32_t>(inputData, ensembles, m_SelectedFeatureArrayName, m_DistributionType, binArray, m_NumberOfCorrelatedBins, m_RemoveBiasedFeatures, m_BiasedFeatures);
  }
  else if (dType.compare("uint32_t") == 0)
  {
    p = fitData<uint32_t>(inputData, ensembles, m_SelectedFeatureArrayName, m_DistributionType, binArray, m_NumberOfCorrelatedBins, m_RemoveBiasedFeatures, m_BiasedFeatures);
  }
  else if (dType.compare("int64_t") == 0)
  {
    p = fitData<int64_t>(inputData, ensembles, m_SelectedFeatureArrayName, m_DistributionType, binArray, m_NumberOfCorrelatedBins, m_RemoveBiasedFeatures, m_BiasedFeatures);
  }
  else if (dType.compare("uint64_t") == 0)
  {
    p = fitData<uint64_t>(inputData, ensembles, m_SelectedFeatureArrayName, m_DistributionType, binArray, m_NumberOfCorrelatedBins, m_RemoveBiasedFeatures, m_BiasedFeatures);
  }
  else if (dType.compare("float") == 0)
  {
    p = fitData<float>(inputData, ensembles, m_SelectedFeatureArrayName, m_DistributionType, binArray, m_NumberOfCorrelatedBins, m_RemoveBiasedFeatures, m_BiasedFeatures);
  }
  else if (dType.compare("double") == 0)
  {
    p = fitData<double>(inputData, ensembles, m_SelectedFeatureArrayName, m_DistributionType, binArray, m_NumberOfCorrelatedBins, m_RemoveBiasedFeatures, m_BiasedFeatures);
  }
  else if (dType.compare("bool") == 0)
  {
    p = fitData<bool>(inputData, ensembles, m_SelectedFeatureArrayName, m_DistributionType, binArray, m_NumberOfCorrelatedBins, m_RemoveBiasedFeatures, m_BiasedFeatures);
  }

  m->getAttributeMatrix(getCellEnsembleAttributeMatrixName())->addAttributeArray(p->GetName(), p);
  emit filterGeneratedMessage(PipelineMessage::CreateStatusMessage(getHumanLabel(), "Complete") );
}


