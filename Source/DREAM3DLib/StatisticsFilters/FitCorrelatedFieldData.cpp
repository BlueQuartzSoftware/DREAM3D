#include "FitCorrelatedFieldData.h"

#include "DREAM3DLib/Common/Constants.h"

#include "DREAM3DLib/DistributionAnalysisOps/BetaOps.h"
#include "DREAM3DLib/DistributionAnalysisOps/PowerLawOps.h"
#include "DREAM3DLib/DistributionAnalysisOps/LogNormalOps.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FitCorrelatedFieldData::FitCorrelatedFieldData() :
  AbstractFilter(),
  m_DataContainerName(DREAM3D::HDF5::VolumeDataContainerName),
  m_BiasedFieldsArrayName(DREAM3D::FieldData::BiasedFields),
  m_SelectedFieldArrayName(""),
  m_CorrelatedFieldArrayName(""),
  m_DistributionType(DREAM3D::DistributionType::UnknownDistributionType),
  m_NumberOfCorrelatedBins(1),
  m_RemoveBiasedFields(false)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FitCorrelatedFieldData::~FitCorrelatedFieldData()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FitCorrelatedFieldData::setupFilterParameters()
{
  FilterParameterVector parameters;
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Field Array To Fit");
    option->setPropertyName("SelectedFieldArrayName");
    option->setWidgetType(FilterParameter::VolumeFieldArrayNameSelectionWidget);
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
    option->setPropertyName("CorrelatedFieldArrayName");
    option->setWidgetType(FilterParameter::VolumeFieldArrayNameSelectionWidget);
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
    option->setHumanLabel("Remove Biased Fields");
    option->setPropertyName("RemoveBiasedFields");
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
void FitCorrelatedFieldData::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  /* Code to read the values goes between these statements */
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE BEGIN*/
  setSelectedFieldArrayName( reader->readString( "SelectedFieldArrayName", getSelectedFieldArrayName() ) );
  setDistributionType( reader->readValue( "DistributionType", getDistributionType() ) );
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE END*/
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int FitCorrelatedFieldData::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("SelectedFieldArrayName", getSelectedFieldArrayName() );
  writer->writeValue("DistributionType", getDistributionType() );
  writer->writeValue("RemovedBiasedFields", getRemoveBiasedFields() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FitCorrelatedFieldData::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);

  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());
  if(NULL == m)
  {
    setErrorCondition(-10000);
    addErrorMessage(getHumanLabel(), "Volume Data Container is NULL", getErrorCondition());
    return;
  }

  if(m_SelectedFieldArrayName.isEmpty() == true)
  {
    setErrorCondition(-11000);
    addErrorMessage(getHumanLabel(), "An array from the Voxel Data Container must be selected.", getErrorCondition());
  }
  if(m_CorrelatedFieldArrayName.isEmpty() == true)
  {
    setErrorCondition(-11000);
    addErrorMessage(getHumanLabel(), "An array from the Voxel Data Container must be selected.", getErrorCondition());
  }
  if(m_RemoveBiasedFields == true)
  {
    QVector<int> dims(1, 1);
    GET_PREREQ_DATA(m, DREAM3D, CellFieldData, BiasedFields, -302, bool, BoolArrayType, fields, dims)
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FitCorrelatedFieldData::preflight()
{
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The DataContainer Object was NULL", -999);
    return;
  }

  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T>
IDataArray::Pointer fitData(IDataArray::Pointer inputData, int64_t ensembles, QString selectedFieldArrayName, unsigned int dType, Int32ArrayType::Pointer binArray, int numBins, bool removeBiasedFields, bool* biasedFields)
{
  StatsData::Pointer sData = StatsData::New();

  QVector<DistributionAnalysisOps::Pointer> m_DistributionAnalysis;
  m_DistributionAnalysis.push_back(BetaOps::New());
  m_DistributionAnalysis.push_back(LogNormalOps::New());
  m_DistributionAnalysis.push_back(PowerLawOps::New());

  DataArray<T>* fieldArray = DataArray<T>::SafePointerDownCast(inputData.get());
  if (NULL == fieldArray)
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

  ss = selectedFieldArrayName + distType + QString("Fit");
  QVector<int> dims(2);
  dims[0] = numBins;
  dims[1] = numComp;
  typename DataArray<float>::Pointer ensembleArray = DataArray<float>::CreateArray(ensembles, dims, ss);

  T* fPtr = fieldArray->getPointer(0);
  float* ePtr = ensembleArray->getPointer(0);
  int32_t* bPtr = binArray->getPointer(0);

  float max;
  float min;
  QVector<VectorOfFloatArray> dist;
  QVector<QVector<QVector<float > > > values;

  size_t numgrains = fieldArray->getNumberOfTuples();

  dist.resize(ensembles);
  values.resize(ensembles);

  for(int64_t i = 1; i < ensembles; i++)
  {
    dist[i] = sData->CreateCorrelatedDistributionArrays(dType, numBins);
    values[i].resize(numBins);
  }

  for (size_t i = 1; i < numgrains; i++)
  {
    if(removeBiasedFields == false || biasedFields[i] == false)
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
  DataArray<T>* fieldArray = DataArray<T>::SafePointerDownCast(correlatedData.get());
  if (NULL == fieldArray)
  {
    return Int32ArrayType::NullPointer();
  }

  T* fPtr = fieldArray->getPointer(0);
  size_t numfields = fieldArray->getNumberOfTuples();

  typename DataArray<int32_t>::Pointer binArray = DataArray<int32_t>::CreateArray(numfields, "binIds");
  int32_t* bPtr = binArray->getPointer(0);

  float max = -100000000.0;
  float min = 100000000.0;

  for (size_t i = 1; i < numfields; i++)
  {
    if(fPtr[i] < min) min = fPtr[i];
    if(fPtr[i] > max) max = fPtr[i];
  }
  //to make sure the max value field doesn't walk off the end of the array, add a small value to the max
  max += 0.000001;

  float step = (max-min)/numBins;
  for (size_t i = 1; i < numfields; i++)
  {
    bPtr[i] = (fPtr[i]-min)/step;
  }
  return binArray;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FitCorrelatedFieldData::execute()
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
  int64_t fields = m->getNumCellFieldTuples();
  int64_t ensembles = m->getNumCellEnsembleTuples();
  dataCheck(false, voxels, fields, ensembles);
  if (getErrorCondition() < 0)
  {
    return;
  }

  QString ss;

  IDataArray::Pointer inputData = m->getCellFieldData(m_SelectedFieldArrayName);
  if (NULL == inputData.get())
  {
    ss = QObject::tr("Selected array '%1' does not exist in the Voxel Data Container. Was it spelled correctly?").arg(m_SelectedFieldArrayName);
    setErrorCondition(-11001);
    notifyErrorMessage(ss, getErrorCondition());
    return;
  }
  IDataArray::Pointer correlatedData = m->getCellFieldData(m_CorrelatedFieldArrayName);
  if (NULL == correlatedData.get())
  {
    ss = QObject::tr("Selected array '%1' does not exist in the Voxel Data Container. Was it spelled correctly?").arg(m_CorrelatedFieldArrayName);
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
    p = fitData<int8_t>(inputData, ensembles, m_SelectedFieldArrayName, m_DistributionType, binArray, m_NumberOfCorrelatedBins, m_RemoveBiasedFields, m_BiasedFields);
  }
  else if (dType.compare("uint8_t") == 0)
  {
    p = fitData<uint8_t>(inputData, ensembles, m_SelectedFieldArrayName, m_DistributionType, binArray, m_NumberOfCorrelatedBins, m_RemoveBiasedFields, m_BiasedFields);
  }
  else if (dType.compare("int16_t") == 0)
  {
    p = fitData<int16_t>(inputData, ensembles, m_SelectedFieldArrayName, m_DistributionType, binArray, m_NumberOfCorrelatedBins, m_RemoveBiasedFields, m_BiasedFields);
  }
  else if (dType.compare("uint16_t") == 0)
  {
    p = fitData<uint16_t>(inputData, ensembles, m_SelectedFieldArrayName, m_DistributionType, binArray, m_NumberOfCorrelatedBins, m_RemoveBiasedFields, m_BiasedFields);
  }
  else if (dType.compare("int32_t") == 0)
  {
    p = fitData<int32_t>(inputData, ensembles, m_SelectedFieldArrayName, m_DistributionType, binArray, m_NumberOfCorrelatedBins, m_RemoveBiasedFields, m_BiasedFields);
  }
  else if (dType.compare("uint32_t") == 0)
  {
    p = fitData<uint32_t>(inputData, ensembles, m_SelectedFieldArrayName, m_DistributionType, binArray, m_NumberOfCorrelatedBins, m_RemoveBiasedFields, m_BiasedFields);
  }
  else if (dType.compare("int64_t") == 0)
  {
    p = fitData<int64_t>(inputData, ensembles, m_SelectedFieldArrayName, m_DistributionType, binArray, m_NumberOfCorrelatedBins, m_RemoveBiasedFields, m_BiasedFields);
  }
  else if (dType.compare("uint64_t") == 0)
  {
    p = fitData<uint64_t>(inputData, ensembles, m_SelectedFieldArrayName, m_DistributionType, binArray, m_NumberOfCorrelatedBins, m_RemoveBiasedFields, m_BiasedFields);
  }
  else if (dType.compare("float") == 0)
  {
    p = fitData<float>(inputData, ensembles, m_SelectedFieldArrayName, m_DistributionType, binArray, m_NumberOfCorrelatedBins, m_RemoveBiasedFields, m_BiasedFields);
  }
  else if (dType.compare("double") == 0)
  {
    p = fitData<double>(inputData, ensembles, m_SelectedFieldArrayName, m_DistributionType, binArray, m_NumberOfCorrelatedBins, m_RemoveBiasedFields, m_BiasedFields);
  }
  else if (dType.compare("bool") == 0)
  {
    p = fitData<bool>(inputData, ensembles, m_SelectedFieldArrayName, m_DistributionType, binArray, m_NumberOfCorrelatedBins, m_RemoveBiasedFields, m_BiasedFields);
  }

  m->addCellEnsembleData(p->GetName(), p);
  notifyStatusMessage("Complete");
}

