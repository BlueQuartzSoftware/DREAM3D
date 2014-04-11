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
  m_CellEnsembleAttributeMatrixName(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellEnsembleAttributeMatrixName, ""),
  m_SelectedFeatureArrayPath("", "", ""),
  m_CorrelatedFeatureArrayPath("", "", ""),
  m_DistributionType(DREAM3D::DistributionType::UnknownDistributionType),
  m_NumberOfCorrelatedBins(1),
  m_RemoveBiasedFeatures(false),
  m_FeaturePhasesArrayPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellFeatureAttributeMatrixName, DREAM3D::FeatureData::Phases),
  m_BiasedFeaturesArrayPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellFeatureAttributeMatrixName, DREAM3D::FeatureData::BiasedFeatures),
  m_NewEnsembleArrayArrayName(""),
  m_BiasedFeaturesArrayName(DREAM3D::FeatureData::BiasedFeatures),
  m_BiasedFeatures(NULL),
  m_NewEnsembleArray(NULL),
  m_FeaturePhasesArrayName(DREAM3D::FeatureData::Phases),
  m_FeaturePhases(NULL)
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
  parameters.push_back(FilterParameter::New("Feature Array To Fit", "SelectedFeatureArrayPath", FilterParameterWidgetType::DataArraySelectionWidget,"DataArrayPath", false));
  parameters.push_back(FilterParameter::New("Array To Correlate With", "CorrelatedFeatureArrayPath", FilterParameterWidgetType::DataArraySelectionWidget,"DataArrayPath", false));
  {
    ChoiceFilterParameter::Pointer parameter = ChoiceFilterParameter::New();
    parameter->setHumanLabel("Distribution Type");
    parameter->setPropertyName("DistributionType");
    parameter->setWidgetType(FilterParameterWidgetType::ChoiceWidget);
    parameter->setValueType("unsigned int");
    QVector<QString> choices;
    choices.push_back("Beta");
    choices.push_back("LogNormal");
    choices.push_back("Power Law");
    parameter->setChoices(choices);
    parameters.push_back(parameter);
  }
  parameters.push_back(FilterParameter::New("Number Of Bins For Correlated Array", "NumberOfCorrelatedBins", FilterParameterWidgetType::IntWidget,"int", false));
  parameters.push_back(FilterParameter::New("Remove Biased Features", "RemoveBiasedFeatures", FilterParameterWidgetType::BooleanWidget,"bool", false));
  parameters.push_back(FilterParameter::New("Required Information", "", FilterParameterWidgetType::SeparatorWidget, "QString", true));
  parameters.push_back(FilterParameter::New("FeaturePhases", "FeaturePhasesArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, "DataArrayPath", true, ""));
  parameters.push_back(FilterParameter::New("BiasedFeatures", "BiasedFeaturesArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, "DataArrayPath", true, ""));
  parameters.push_back(FilterParameter::New("Cell Ensemble Attribute Matrix Name", "CellEnsembleAttributeMatrixName", FilterParameterWidgetType::AttributeMatrixSelectionWidget, "DataArrayPath", true, ""));
  parameters.push_back(FilterParameter::New("Created Information", "", FilterParameterWidgetType::SeparatorWidget, "QString", true));
  parameters.push_back(FilterParameter::New("NewEnsembleArray", "NewEnsembleArrayArrayName", FilterParameterWidgetType::StringWidget, "QString", true, ""));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FitCorrelatedFeatureData::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setCellEnsembleAttributeMatrixName(reader->readDataArrayPath("CellEnsembleAttributeMatrixName", getCellEnsembleAttributeMatrixName()));
  setNewEnsembleArrayArrayName(reader->readString("NewEnsembleArrayArrayName", getNewEnsembleArrayArrayName() ) );
  setBiasedFeaturesArrayPath(reader->readDataArrayPath("BiasedFeaturesArrayPath", getBiasedFeaturesArrayPath() ) );
  setFeaturePhasesArrayPath(reader->readDataArrayPath("FeaturePhasesArrayPath", getFeaturePhasesArrayPath() ) );
  setSelectedFeatureArrayPath( reader->readDataArrayPath( "SelectedFeatureArrayPath", getSelectedFeatureArrayPath() ) );
  setCorrelatedFeatureArrayPath( reader->readDataArrayPath( "CorrelatedFeatureArrayPath", getCorrelatedFeatureArrayPath() ) );
  setDistributionType( reader->readValue( "DistributionType", getDistributionType() ) );
  setRemoveBiasedFeatures( reader->readValue( "RemoveBiasedFeatures", getRemoveBiasedFeatures() ) );
  setNumberOfCorrelatedBins( reader->readValue( "NumberOfCorrelatedBins", getNumberOfCorrelatedBins() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int FitCorrelatedFeatureData::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("CellEnsembleAttributeMatrixName", getCellEnsembleAttributeMatrixName());
  writer->writeValue("NewEnsembleArrayArrayName", getNewEnsembleArrayArrayName() );
  writer->writeValue("BiasedFeaturesArrayPath", getBiasedFeaturesArrayPath() );
  writer->writeValue("FeaturePhasesArrayPath", getFeaturePhasesArrayPath() );
  writer->writeValue("SelectedFeatureArrayPath", getSelectedFeatureArrayPath() );
  writer->writeValue("CorrelatedFeatureArrayPath", getSelectedFeatureArrayPath() );
  writer->writeValue("DistributionType", getDistributionType() );
  writer->writeValue("RemovedBiasedFeatures", getRemoveBiasedFeatures() );
  writer->writeValue("NumberOfCorrelatedBins", getNumberOfCorrelatedBins() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FitCorrelatedFeatureData::dataCheck()
{
  DataArrayPath tempPath;
  setErrorCondition(0);

  QVector<size_t> dims(1, 1);
  m_FeaturePhasesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getFeaturePhasesArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeaturePhasesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeaturePhases = m_FeaturePhasesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  if(m_SelectedFeatureArrayPath.isEmpty() == true)
  {
    setErrorCondition(-11000);
    notifyErrorMessage(getHumanLabel(), "An array from the Volume DataContainer must be selected.", getErrorCondition());
  }
  if(m_CorrelatedFeatureArrayPath.isEmpty() == true)
  {
    setErrorCondition(-11000);
    notifyErrorMessage(getHumanLabel(), "An array from the Volume DataContainer must be selected.", getErrorCondition());
  }

  int numComp = 0;
  QString distType("UNKNOWN");
// Determining number of components and name given distribution type
  if (m_DistributionType == DREAM3D::DistributionType::Beta) { distType = "Beta", numComp = DREAM3D::DistributionType::BetaColumnCount; }
  else if (m_DistributionType == DREAM3D::DistributionType::LogNormal) { distType = "LogNormal", numComp = DREAM3D::DistributionType::LogNormalColumnCount; }
  else if (m_DistributionType == DREAM3D::DistributionType::Power) { distType = "PowerLaw", numComp = DREAM3D::DistributionType::PowerLawColumnCount; }

  m_NewEnsembleArrayArrayName = m_SelectedFeatureArrayPath.getDataArrayName() + distType + QString("Fit") + QString("CorrelatedTo") + m_CorrelatedFeatureArrayPath.getDataArrayName();
  dims.resize(2);
  dims[0] = m_NumberOfCorrelatedBins;
  dims[1] = numComp;
  tempPath.update(getCellEnsembleAttributeMatrixName().getDataContainerName(), getCellEnsembleAttributeMatrixName().getAttributeMatrixName(), getNewEnsembleArrayArrayName() );
  m_NewEnsembleArrayPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, tempPath, 0, dims); /* Assigns the shared_ptr<>(this, tempPath, 0, dims); Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_NewEnsembleArrayPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_NewEnsembleArray = m_NewEnsembleArrayPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  if(m_RemoveBiasedFeatures == true)
  {
    dims.resize(1);
    dims[0] = 1;
    m_BiasedFeaturesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<bool>, AbstractFilter>(this, getBiasedFeaturesArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if( NULL != m_BiasedFeaturesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    { m_BiasedFeatures = m_BiasedFeaturesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FitCorrelatedFeatureData::preflight()
{
  emit preflightAboutToExecute();
  emit updateFilterParameters(this);
  dataCheck();
  emit preflightExecuted();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T>
void fitData(IDataArray::Pointer inputData, float* ensembleArray, int32_t* eIds, int32_t numEnsembles, unsigned int dType, Int32ArrayType::Pointer binArray, int numBins, bool removeBiasedFeatures, bool* biasedFeatures)
{
  StatsData::Pointer sData = StatsData::New();

  QVector<DistributionAnalysisOps::Pointer> m_DistributionAnalysis;
  m_DistributionAnalysis.push_back(BetaOps::New());
  m_DistributionAnalysis.push_back(LogNormalOps::New());
  m_DistributionAnalysis.push_back(PowerLawOps::New());

  DataArray<T>* featureArray = DataArray<T>::SafePointerDownCast(inputData.get());
  if (NULL == featureArray)
  {
    return;
  }
  QString ss;
  QString distType;
  int numComp = 1;

// Determining number of components and name given distribution type
  if (dType == DREAM3D::DistributionType::Beta) { distType = "Beta", numComp = DREAM3D::DistributionType::BetaColumnCount; }
  else if (dType == DREAM3D::DistributionType::LogNormal) { distType = "LogNormal", numComp = DREAM3D::DistributionType::LogNormalColumnCount; }
  else if (dType == DREAM3D::DistributionType::Power) { distType = "PowerLaw", numComp = DREAM3D::DistributionType::PowerLawColumnCount; }

  T* fPtr = featureArray->getPointer(0);
  int32_t* bPtr = binArray->getPointer(0);

  //float max;
  //float min;
  QVector<VectorOfFloatArray> dist;
  QVector<QVector<QVector<float > > > values;

  size_t numfeatures = featureArray->getNumberOfTuples();

  dist.resize(numEnsembles);
  values.resize(numEnsembles);

  for(int64_t i = 1; i < numEnsembles; i++)
  {
    dist[i] = sData->CreateCorrelatedDistributionArrays(dType, numBins);
    values[i].resize(numBins);
  }

  int32_t ensemble;
  for (size_t i = 1; i < numfeatures; i++)
  {
    if(removeBiasedFeatures == false || biasedFeatures[i] == false)
    {
      ensemble = eIds[i];
      values[ensemble][bPtr[i]].push_back(static_cast<float>(fPtr[i]));
    }
  }
  for (int64_t i = 1; i < numEnsembles; i++)
  {
    m_DistributionAnalysis[dType]->calculateCorrelatedParameters(values[i], dist[i]);
    for (int j = 0; j < numBins; j++)
    {
      for(int k = 0; k < numComp; k++)
      {
        VectorOfFloatArray data = dist[i];
        ensembleArray[(numComp * numBins * i) + (numComp * j) + k] = data[k]->getValue(j);
      }
    }
  }
  return;
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
    if(static_cast<T>(fPtr[i]) < min) { min = static_cast<T>(fPtr[i]); }
    if(static_cast<T>(fPtr[i]) > max) { max = static_cast<T>(fPtr[i]); }
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

  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(m_SelectedFeatureArrayPath.getDataContainerName());
  int64_t numEnsembles = m_NewEnsembleArrayPtr.lock()->getNumberOfTuples();

  QString ss;

  IDataArray::Pointer inputData = m->getAttributeMatrix(m_SelectedFeatureArrayPath.getAttributeMatrixName())->getAttributeArray(m_SelectedFeatureArrayPath.getDataArrayName());
  if (NULL == inputData.get())
  {
    ss = QObject::tr("Selected array '%1' does not exist in the Voxel Data Container. Was it spelled correctly?").arg(m_SelectedFeatureArrayPath.getDataArrayName());
    setErrorCondition(-11001);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }
  IDataArray::Pointer correlatedData = m->getAttributeMatrix(m_CorrelatedFeatureArrayPath.getAttributeMatrixName())->getAttributeArray(m_CorrelatedFeatureArrayPath.getDataArrayName());
  if (NULL == correlatedData.get())
  {
    ss = QObject::tr("Selected array '%1' does not exist in the Voxel Data Container. Was it spelled correctly?").arg(m_CorrelatedFeatureArrayPath.getDataArrayName());
    setErrorCondition(-11001);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
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
  if (dType.compare("int8_t") == 0)
  {
    fitData<int8_t>(inputData, m_NewEnsembleArray, m_FeaturePhases, numEnsembles, m_DistributionType, binArray, m_NumberOfCorrelatedBins, m_RemoveBiasedFeatures, m_BiasedFeatures);
  }
  else if (dType.compare("uint8_t") == 0)
  {
    fitData<uint8_t>(inputData, m_NewEnsembleArray, m_FeaturePhases, numEnsembles, m_DistributionType, binArray, m_NumberOfCorrelatedBins, m_RemoveBiasedFeatures, m_BiasedFeatures);
  }
  else if (dType.compare("int16_t") == 0)
  {
    fitData<int16_t>(inputData, m_NewEnsembleArray, m_FeaturePhases, numEnsembles, m_DistributionType, binArray, m_NumberOfCorrelatedBins, m_RemoveBiasedFeatures, m_BiasedFeatures);
  }
  else if (dType.compare("uint16_t") == 0)
  {
    fitData<uint16_t>(inputData, m_NewEnsembleArray, m_FeaturePhases, numEnsembles, m_DistributionType, binArray, m_NumberOfCorrelatedBins, m_RemoveBiasedFeatures, m_BiasedFeatures);
  }
  else if (dType.compare("int32_t") == 0)
  {
    fitData<int32_t>(inputData, m_NewEnsembleArray, m_FeaturePhases, numEnsembles, m_DistributionType, binArray, m_NumberOfCorrelatedBins, m_RemoveBiasedFeatures, m_BiasedFeatures);
  }
  else if (dType.compare("uint32_t") == 0)
  {
    fitData<uint32_t>(inputData, m_NewEnsembleArray, m_FeaturePhases, numEnsembles, m_DistributionType, binArray, m_NumberOfCorrelatedBins, m_RemoveBiasedFeatures, m_BiasedFeatures);
  }
  else if (dType.compare("int64_t") == 0)
  {
    fitData<int64_t>(inputData, m_NewEnsembleArray, m_FeaturePhases, numEnsembles, m_DistributionType, binArray, m_NumberOfCorrelatedBins, m_RemoveBiasedFeatures, m_BiasedFeatures);
  }
  else if (dType.compare("uint64_t") == 0)
  {
    fitData<uint64_t>(inputData, m_NewEnsembleArray, m_FeaturePhases, numEnsembles, m_DistributionType, binArray, m_NumberOfCorrelatedBins, m_RemoveBiasedFeatures, m_BiasedFeatures);
  }
  else if (dType.compare("float") == 0)
  {
    fitData<float>(inputData, m_NewEnsembleArray, m_FeaturePhases, numEnsembles, m_DistributionType, binArray, m_NumberOfCorrelatedBins, m_RemoveBiasedFeatures, m_BiasedFeatures);
  }
  else if (dType.compare("double") == 0)
  {
    fitData<double>(inputData, m_NewEnsembleArray, m_FeaturePhases, numEnsembles, m_DistributionType, binArray, m_NumberOfCorrelatedBins, m_RemoveBiasedFeatures, m_BiasedFeatures);
  }
  else if (dType.compare("bool") == 0)
  {
    fitData<bool>(inputData, m_NewEnsembleArray, m_FeaturePhases, numEnsembles, m_DistributionType, binArray, m_NumberOfCorrelatedBins, m_RemoveBiasedFeatures, m_BiasedFeatures);
  }

  notifyStatusMessage(getHumanLabel(), "Complete");
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer FitCorrelatedFeatureData::newFilterInstance(bool copyFilterParameters)
{
  FitCorrelatedFeatureData::Pointer filter = FitCorrelatedFeatureData::New();
  if(true == copyFilterParameters)
  {
    filter->setFilterParameters(getFilterParameters() );
    
    //Loop over each Filter Parameter that is registered to the filter either through this class or a parent class
    // and copy the value from the current instance of the object into the "new" instance that was just created
    QVector<FilterParameter::Pointer> options = getFilterParameters(); // Get the current set of filter parameters
    for (QVector<FilterParameter::Pointer>::iterator iter = options.begin(); iter != options.end(); ++iter )
    {
      FilterParameter* parameter = (*iter).get();
      if (parameter->getWidgetType().compare(FilterParameterWidgetType::SeparatorWidget) == 0 )
      {
        continue; // Skip this type of filter parameter as it has nothing to do with anything in the filter.
      }
      // Get the property from the current instance of the filter
      QVariant var = property(parameter->getPropertyName().toLatin1().constData());
      bool ok = filter->setProperty(parameter->getPropertyName().toLatin1().constData(), var);
      if(false == ok)
      {
        QString ss = QString("Error occurred transferring the Filter Parameter '%1' in Filter '%2' to the filter instance. The pipeline may run but the underlying filter will NOT be using the values from the GUI."
                             " Please report this issue to the developers of this filter.").arg(parameter->getPropertyName()).arg(filter->getHumanLabel());
        Q_ASSERT_X(ok, __FILE__, ss.toLatin1().constData());
      }

      if(parameter->isConditional() == true)
      {
        QVariant cond = property(parameter->getConditionalProperty().toLatin1().constData() );
        ok = filter->setProperty(parameter->getConditionalProperty().toLatin1().constData(), cond);
        if(false == ok)
        {
          QString ss = QString("%1::newFilterInstance()\nError occurred transferring the Filter Parameter '%2' in Filter '%3' to the filter instance. "
                              " The filter parameter has a conditional property '%4'. The transfer of this property from the old filter to the new filter failed."
                              " Please report this issue to the developers of this filter.").arg(filter->getNameOfClass())
                              .arg(parameter->getPropertyName())
                              .arg(filter->getHumanLabel())
                              .arg(parameter->getConditionalProperty());
          Q_ASSERT_X(ok, __FILE__, ss.toLatin1().constData());
        }
      }
    }
  }
  return filter;
}
