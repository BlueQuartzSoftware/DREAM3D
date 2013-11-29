#include "FitFeatureData.h"

#include "DREAM3DLib/Common/Constants.h"

#include "DREAM3DLib/DistributionAnalysisOps/BetaOps.h"
#include "DREAM3DLib/DistributionAnalysisOps/PowerLawOps.h"
#include "DREAM3DLib/DistributionAnalysisOps/LogNormalOps.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FitFeatureData::FitFeatureData() :
  AbstractFilter(),
  m_DataContainerName(DREAM3D::Defaults::VolumeDataContainerName),
  m_CellFeatureAttributeMatrixName(DREAM3D::Defaults::CellFeatureAttributeMatrixName),
  m_CellEnsembleAttributeMatrixName(DREAM3D::Defaults::CellEnsembleAttributeMatrixName),
  m_BiasedFeaturesArrayName(DREAM3D::FeatureData::BiasedFeatures),
  m_SelectedFeatureArrayName(""),
  m_DistributionType(DREAM3D::DistributionType::UnknownDistributionType),
  m_RemoveBiasedFeatures(false)
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
void FitFeatureData::readFilterParameters(AbstractFilterParametersReader* reader, int index)
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
int FitFeatureData::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
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
void FitFeatureData::dataCheck(bool preflight, size_t voxels, size_t features, size_t ensembles)
{
  setErrorCondition(0);

  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

  if(m_SelectedFeatureArrayName.isEmpty() == true)
  {
    setErrorCondition(-11000);
    addErrorMessage(getHumanLabel(), "An array from the Volume DataContainer must be selected.", getErrorCondition());
  }
  if(m_RemoveBiasedFeatures == true)
  {
    QVector<int> dims(1, 1);
    m_BiasedFeaturesPtr = m->getPrereqArray<bool, AbstractFilter>(this, m_CellFeatureAttributeMatrixName,  m_BiasedFeaturesArrayName, -302, features, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if( NULL != m_BiasedFeaturesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    { m_BiasedFeatures = m_BiasedFeaturesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FitFeatureData::preflight()
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
IDataArray::Pointer fitData(IDataArray::Pointer inputData, int64_t ensembles, QString selectedFeatureArrayName, unsigned int dType, bool removeBiasedFeatures, bool* biasedFeatures)
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
  QVector<int> dims(1, numComp);
  typename DataArray<float>::Pointer ensembleArray = DataArray<float>::CreateArray(ensembles, dims, ss);

  T* fPtr = featureArray->getPointer(0);
  float* ePtr = ensembleArray->getPointer(0);

  QVector<FloatArrayType::Pointer> dist;
  QVector<QVector<float > > values;

  size_t numfeatures = featureArray->getNumberOfTuples();

  dist.resize(ensembles);
  values.resize(ensembles);

  for(int64_t i = 1; i < ensembles; i++)
  {
    dist[i] = sData->CreateDistributionArrays(dType);
    values[i].resize(1);
  }

  for (size_t i = 1; i < numfeatures; i++)
  {
    if(removeBiasedFeatures == false || biasedFeatures[i] == false)
    {
      values[1].push_back(static_cast<float>(fPtr[i]));
    }
  }
  for (int64_t i = 1; i < ensembles; i++)
  {
    m_DistributionAnalysis[dType]->calculateParameters(values[i], dist[i]);
    for (int j = 0; j < numComp; j++)
    {
      FloatArrayType::Pointer data = dist[i];
      ePtr[numComp * i + j] = data->GetValue(j);
    }
  }
  return ensembleArray;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FitFeatureData::execute()
{
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The DataContainer Object was NULL", -999);
    return;
  }
  setErrorCondition(0);
  int64_t features = m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->getNumTuples();
  int64_t ensembles = m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->getNumTuples();
  dataCheck(false, 0, features, ensembles);
  if (getErrorCondition() < 0)
  {
    return;
  }

  QString ss;

  IDataArray::Pointer inputData = m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->getAttributeArray(m_SelectedFeatureArrayName);

  if (NULL == inputData.get())
  {
    ss = QObject::tr("Selected array '%1' does not exist in the Voxel Data Container. Was it spelled correctly?").arg(m_SelectedFeatureArrayName);
    setErrorCondition(-11001);
    notifyErrorMessage(ss, getErrorCondition());
    return;
  }

  QString dType = inputData->getTypeAsString();
  IDataArray::Pointer p = IDataArray::NullPointer();
  if (dType.compare("int8_t") == 0)
  {
    p = fitData<int8_t>(inputData, ensembles, m_SelectedFeatureArrayName, m_DistributionType, m_RemoveBiasedFeatures, m_BiasedFeatures);
  }
  else if (dType.compare("uint8_t") == 0)
  {
    p = fitData<uint8_t>(inputData, ensembles, m_SelectedFeatureArrayName, m_DistributionType, m_RemoveBiasedFeatures, m_BiasedFeatures);
  }
  else if (dType.compare("int16_t") == 0)
  {
    p = fitData<int16_t>(inputData, ensembles, m_SelectedFeatureArrayName, m_DistributionType, m_RemoveBiasedFeatures, m_BiasedFeatures);
  }
  else if (dType.compare("uint16_t") == 0)
  {
    p = fitData<uint16_t>(inputData, ensembles, m_SelectedFeatureArrayName, m_DistributionType, m_RemoveBiasedFeatures, m_BiasedFeatures);
  }
  else if (dType.compare("int32_t") == 0)
  {
    p = fitData<int32_t>(inputData, ensembles, m_SelectedFeatureArrayName, m_DistributionType, m_RemoveBiasedFeatures, m_BiasedFeatures);
  }
  else if (dType.compare("uint32_t") == 0)
  {
    p = fitData<uint32_t>(inputData, ensembles, m_SelectedFeatureArrayName, m_DistributionType, m_RemoveBiasedFeatures, m_BiasedFeatures);
  }
  else if (dType.compare("int64_t") == 0)
  {
    p = fitData<int64_t>(inputData, ensembles, m_SelectedFeatureArrayName, m_DistributionType, m_RemoveBiasedFeatures, m_BiasedFeatures);
  }
  else if (dType.compare("uint64_t") == 0)
  {
    p = fitData<uint64_t>(inputData, ensembles, m_SelectedFeatureArrayName, m_DistributionType, m_RemoveBiasedFeatures, m_BiasedFeatures);
  }
  else if (dType.compare("float") == 0)
  {
    p = fitData<float>(inputData, ensembles, m_SelectedFeatureArrayName, m_DistributionType, m_RemoveBiasedFeatures, m_BiasedFeatures);
  }
  else if (dType.compare("double") == 0)
  {
    p = fitData<double>(inputData, ensembles, m_SelectedFeatureArrayName, m_DistributionType, m_RemoveBiasedFeatures, m_BiasedFeatures);
  }
  else if (dType.compare("bool") == 0)
  {
    p = fitData<bool>(inputData, ensembles, m_SelectedFeatureArrayName, m_DistributionType, m_RemoveBiasedFeatures, m_BiasedFeatures);
  }

  m->getAttributeMatrix(getCellEnsembleAttributeMatrixName())->addAttributeArray(p->GetName(), p);
  notifyStatusMessage("Complete");
}

