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

#include "FindFeatureHistogram.h"

#include "DREAM3DLib/Common/Constants.h"

#include "DREAM3DLib/DistributionAnalysisOps/BetaOps.h"
#include "DREAM3DLib/DistributionAnalysisOps/PowerLawOps.h"
#include "DREAM3DLib/DistributionAnalysisOps/LogNormalOps.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindFeatureHistogram::FindFeatureHistogram() :
  AbstractFilter(),
  m_DataContainerName(DREAM3D::Defaults::VolumeDataContainerName),
  m_CellFeatureAttributeMatrixName(DREAM3D::Defaults::CellFeatureAttributeMatrixName),
  m_CellEnsembleAttributeMatrixName(DREAM3D::Defaults::CellEnsembleAttributeMatrixName),
  m_SelectedFeatureArrayName(""),
  m_NumBins(1),
  m_RemoveBiasedFeatures(false),
  m_BiasedFeaturesArrayName(DREAM3D::FeatureData::BiasedFeatures),
  m_BiasedFeatures(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindFeatureHistogram::~FindFeatureHistogram()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindFeatureHistogram::setupFilterParameters()
{
  FilterParameterVector parameters;
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Feature Array To Bin");
    option->setPropertyName("SelectedFeatureArrayName");
    option->setWidgetType(FilterParameter::VolumeFeatureArrayNameSelectionWidget);
    option->setValueType("string");
    option->setUnits("");
    parameters.push_back(option);
  }
  {
    ChoiceFilterParameter::Pointer option = ChoiceFilterParameter::New();
    option->setHumanLabel("Number of Bins");
    option->setPropertyName("NumBins");
    option->setWidgetType(FilterParameter::IntWidget);
    option->setValueType("int");
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
void FindFeatureHistogram::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  /* Code to read the values goes between these statements */
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE BEGIN*/
  setSelectedFeatureArrayName( reader->readString( "SelectedFeatureArrayName", getSelectedFeatureArrayName() ) );
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE END*/
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int FindFeatureHistogram::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("SelectedFeatureArrayName", getSelectedFeatureArrayName() );
  writer->writeValue("NumberOfBins", getNumBins() );
  writer->writeValue("RemovedBiasedFeatures", getRemoveBiasedFeatures() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindFeatureHistogram::dataCheck()
{
  setErrorCondition(0);

  VolumeDataContainer* m = getDataContainerArray()->getPrereqDataContainer<VolumeDataContainer, AbstractFilter>(this, getDataContainerName(), false);
  if(getErrorCondition() < 0) { return; }
  AttributeMatrix* cellFeatureAttrMat = m->getPrereqAttributeMatrix<AbstractFilter>(this, getCellFeatureAttributeMatrixName(), -301);
  if(getErrorCondition() < 0) { return; }

  if(m_SelectedFeatureArrayName.isEmpty() == true)
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
void FindFeatureHistogram::preflight()
{
  dataCheck();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T>
IDataArray::Pointer findHistogram(IDataArray::Pointer inputData, int64_t ensembles, QString selectedFeatureArrayName, int NumBins, bool removeBiasedFeatures, bool* biasedFeatures)
{
  StatsData::Pointer sData = StatsData::New();

  DataArray<T>* featureArray = DataArray<T>::SafePointerDownCast(inputData.get());
  if (NULL == featureArray)
  {
    return IDataArray::NullPointer();
  }
  QString ss;
  int numComp = NumBins;

  ss = selectedFeatureArrayName + QString("Histogram");
  QVector<int> dims(1, numComp);
  typename DataArray<float>::Pointer ensembleArray = DataArray<float>::CreateArray(ensembles, dims, ss);

  T* fPtr = featureArray->getPointer(0);
  float* ePtr = ensembleArray->getPointer(0);

  QVector<QVector<int > > counts;

  size_t numfeatures = featureArray->getNumberOfTuples();

  counts.resize(ensembles);

  for(int64_t i = 1; i < ensembles; i++)
  {
    counts[i].resize(NumBins);
    for(int j = 0; j < NumBins; j++)
    {
      counts[i][j] = 0;
    }
  }

  int ensemble, bin;
  float min = 1000000.0f;
  float max = 0.0f;
  float value;
  for (size_t i = 1; i < numfeatures; i++)
  {
    value = fPtr[i];
    if(value > max) { max = value; }
    if(value < min) { min = value; }
  }
  float stepsize = (max - min) / NumBins;

  for (size_t i = 1; i < numfeatures; i++)
  {
    if(removeBiasedFeatures == false || biasedFeatures[i] == false)
    {
      bin = (fPtr[i] - min) / stepsize;
      if(bin >= NumBins) { bin = NumBins - 1; }
      counts[1][bin]++;
    }
  }
  for (int64_t i = 1; i < ensembles; i++)
  {
    for (int j = 0; j < numComp; j++)
    {
      ePtr[numComp * i + j] = counts[i][j];
    }
  }
  return ensembleArray;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindFeatureHistogram::execute()
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

  QString dType = inputData->getTypeAsString();
  IDataArray::Pointer p = IDataArray::NullPointer();
  if (dType.compare("int8_t") == 0)
  {
    p = findHistogram<int8_t>(inputData, ensembles, m_SelectedFeatureArrayName, m_NumBins, m_RemoveBiasedFeatures, m_BiasedFeatures);
  }
  else if (dType.compare("uint8_t") == 0)
  {
    p = findHistogram<uint8_t>(inputData, ensembles, m_SelectedFeatureArrayName, m_NumBins, m_RemoveBiasedFeatures, m_BiasedFeatures);
  }
  else if (dType.compare("int16_t") == 0)
  {
    p = findHistogram<int16_t>(inputData, ensembles, m_SelectedFeatureArrayName, m_NumBins, m_RemoveBiasedFeatures, m_BiasedFeatures);
  }
  else if (dType.compare("uint16_t") == 0)
  {
    p = findHistogram<uint16_t>(inputData, ensembles, m_SelectedFeatureArrayName, m_NumBins, m_RemoveBiasedFeatures, m_BiasedFeatures);
  }
  else if (dType.compare("int32_t") == 0)
  {
    p = findHistogram<int32_t>(inputData, ensembles, m_SelectedFeatureArrayName, m_NumBins, m_RemoveBiasedFeatures, m_BiasedFeatures);
  }
  else if (dType.compare("uint32_t") == 0)
  {
    p = findHistogram<uint32_t>(inputData, ensembles, m_SelectedFeatureArrayName, m_NumBins, m_RemoveBiasedFeatures, m_BiasedFeatures);
  }
  else if (dType.compare("int64_t") == 0)
  {
    p = findHistogram<int64_t>(inputData, ensembles, m_SelectedFeatureArrayName, m_NumBins, m_RemoveBiasedFeatures, m_BiasedFeatures);
  }
  else if (dType.compare("uint64_t") == 0)
  {
    p = findHistogram<uint64_t>(inputData, ensembles, m_SelectedFeatureArrayName, m_NumBins, m_RemoveBiasedFeatures, m_BiasedFeatures);
  }
  else if (dType.compare("float") == 0)
  {
    p = findHistogram<float>(inputData, ensembles, m_SelectedFeatureArrayName, m_NumBins, m_RemoveBiasedFeatures, m_BiasedFeatures);
  }
  else if (dType.compare("double") == 0)
  {
    p = findHistogram<double>(inputData, ensembles, m_SelectedFeatureArrayName, m_NumBins, m_RemoveBiasedFeatures, m_BiasedFeatures);
  }
  else if (dType.compare("bool") == 0)
  {
    p = findHistogram<bool>(inputData, ensembles, m_SelectedFeatureArrayName, m_NumBins, m_RemoveBiasedFeatures, m_BiasedFeatures);
  }

  m->getAttributeMatrix(getCellEnsembleAttributeMatrixName())->addAttributeArray(p->GetName(), p);
  emit filterGeneratedMessage(PipelineMessage::CreateStatusMessage(getHumanLabel(), "Complete") );
}

