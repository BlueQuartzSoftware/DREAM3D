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
  m_DataContainerName(DREAM3D::HDF5::VolumeDataContainerName),
  m_BiasedFeaturesArrayName(DREAM3D::FeatureData::BiasedFeatures),
  m_SelectedFeatureArrayName(""),
  m_CorrelatedFeatureArrayName(""),
  m_DistributionType(DREAM3D::DistributionType::UnknownDistributionType),
  m_NumberOfCorrelatedBins(1),
  m_RemoveBiasedFeatures(false)
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
    option->setHumanLabel("Number of Bins for Correlated Array");
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
void FitCorrelatedFeatureData::dataCheck(bool preflight, size_t voxels, size_t features, size_t ensembles)
{
  setErrorCondition(0);

  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

  if(m_SelectedFeatureArrayName.isEmpty() == true)
  {
    setErrorCondition(-11000);
    addErrorMessage(getHumanLabel(), "An array from the Volume DataContainer must be selected.", getErrorCondition());
  }
  if(m_CorrelatedFeatureArrayName.isEmpty() == true)
  {
    setErrorCondition(-11000);
    addErrorMessage(getHumanLabel(), "An array from the Volume DataContainer must be selected.", getErrorCondition());
  }
  if(m_RemoveBiasedFeatures == true)
  {
    QVector<int> dims(1, 1);
    GET_PREREQ_DATA(m, DREAM3D, CellFeatureData, BiasedFeatures, -302, bool, BoolArrayType, features, dims)
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FitCorrelatedFeatureData::preflight()
{
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());
  if(NULL == m)
  {
    setErrorCondition(-999);
    addErrorMessage(getHumanLabel(), "The VolumeDataContainer Object with the specific name " + getDataContainerName() + " was not available.", getErrorCondition());
    return;
  }

  dataCheck(true, 1, 1, 1);
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
  if (dType == DREAM3D::DistributionType::Beta) distType = "Beta", numComp = DREAM3D::HDF5::BetaColumnCount;
  else if (dType == DREAM3D::DistributionType::LogNormal) distType = "LogNormal", numComp = DREAM3D::HDF5::LogNormalColumnCount;
  else if (dType == DREAM3D::DistributionType::Power) distType = "PowerLaw", numComp = DREAM3D::HDF5::PowerLawColumnCount;

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
      for(int k =0; k < numComp; k++)
      {
        VectorOfFloatArray data = dist[i];
        ePtr[(numComp*numBins*i)+(numComp*j)+k] = data[k]->GetValue(j);
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
    if(fPtr[i] < min) min = fPtr[i];
    if(fPtr[i] > max) max = fPtr[i];
  }
  //to make sure the max value feature doesn't walk off the end of the array, add a small value to the max
  max += 0.000001;

  float step = (max-min)/numBins;
  for (size_t i = 1; i < numfeatures; i++)
  {
    bPtr[i] = (fPtr[i]-min)/step;
  }
  return binArray;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FitCorrelatedFeatureData::execute()
{
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The DataContainer Object was NULL", -999);
    return;
  }
  setErrorCondition(0);
  int64_t voxels = m->getTotalPoints();
  int64_t features = m->getNumCellFeatureTuples();
  int64_t ensembles = m->getNumCellEnsembleTuples();
  dataCheck(false, voxels, features, ensembles);
  if (getErrorCondition() < 0)
  {
    return;
  }

  QString ss;

  IDataArray::Pointer inputData = m->getCellFeatureData(m_SelectedFeatureArrayName);
  if (NULL == inputData.get())
  {
    ss = QObject::tr("Selected array '%1' does not exist in the Voxel Data Container. Was it spelled correctly?").arg(m_SelectedFeatureArrayName);
    setErrorCondition(-11001);
    notifyErrorMessage(ss, getErrorCondition());
    return;
  }
  IDataArray::Pointer correlatedData = m->getCellFeatureData(m_CorrelatedFeatureArrayName);
  if (NULL == correlatedData.get())
  {
    ss = QObject::tr("Selected array '%1' does not exist in the Voxel Data Container. Was it spelled correctly?").arg(m_CorrelatedFeatureArrayName);
    setErrorCondition(-11001);
    notifyErrorMessage(ss, getErrorCondition());
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

  m->addCellEnsembleData(p->GetName(), p);
  notifyStatusMessage("Complete");
}

