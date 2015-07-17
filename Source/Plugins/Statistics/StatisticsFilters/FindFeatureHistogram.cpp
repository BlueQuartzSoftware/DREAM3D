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


#include "FindFeatureHistogram.h"

#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/FilterParameters/AbstractFilterParametersReader.h"
#include "DREAM3DLib/FilterParameters/AbstractFilterParametersWriter.h"

#include "DREAM3DLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "DREAM3DLib/FilterParameters/DataArrayCreationFilterParameter.h"
#include "DREAM3DLib/FilterParameters/LinkedBooleanFilterParameter.h"
#include "DREAM3DLib/FilterParameters/ChoiceFilterParameter.h"
#include "DREAM3DLib/FilterParameters/SeparatorFilterParameter.h"

#include "Statistics/DistributionAnalysisOps/BetaOps.h"
#include "Statistics/DistributionAnalysisOps/PowerLawOps.h"
#include "Statistics/DistributionAnalysisOps/LogNormalOps.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindFeatureHistogram::FindFeatureHistogram() :
  AbstractFilter(),
  m_SelectedFeatureArrayPath("", "", ""),
  m_NumberOfBins(1),
  m_RemoveBiasedFeatures(false),
  m_FeaturePhasesArrayPath("", "", ""),
  m_BiasedFeaturesArrayPath("", "", ""),
  m_NewEnsembleArrayArrayPath("", "", ""),
  m_BiasedFeatures(NULL),
  m_NewEnsembleArray(NULL),
  m_FeaturePhases(NULL)
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
    ChoiceFilterParameter::Pointer parameter = ChoiceFilterParameter::New();
    parameter->setHumanLabel("Number of Bins");
    parameter->setPropertyName("NumberOfBins");

    parameters.push_back(parameter);
  }
  QStringList linkedProps("BiasedFeaturesArrayPath");
  parameters.push_back(LinkedBooleanFilterParameter::New("Remove Biased Features", "RemoveBiasedFeatures", getRemoveBiasedFeatures(), linkedProps, FilterParameter::Parameter));
  {
    DataArraySelectionFilterParameter::DataStructureRequirements req;
    parameters.push_back(DataArraySelectionFilterParameter::New("Feature Array To Bin", "SelectedFeatureArrayPath", getSelectedFeatureArrayPath(), FilterParameter::RequiredArray, req));
  }
  {
    DataArraySelectionFilterParameter::DataStructureRequirements req;
    parameters.push_back(DataArraySelectionFilterParameter::New("FeaturePhases", "FeaturePhasesArrayPath", getFeaturePhasesArrayPath(), FilterParameter::RequiredArray, req));
  }
  {
    DataArraySelectionFilterParameter::DataStructureRequirements req;
    parameters.push_back(DataArraySelectionFilterParameter::New("BiasedFeatures", "BiasedFeaturesArrayPath", getBiasedFeaturesArrayPath(), FilterParameter::RequiredArray, req));
  }

  {
    DataArrayCreationFilterParameter::DataStructureRequirements req;
    parameters.push_back(DataArrayCreationFilterParameter::New("New Ensemble Array", "NewEnsembleArrayArrayPath", getNewEnsembleArrayArrayPath(), FilterParameter::CreatedArray, req));
  }

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindFeatureHistogram::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setNewEnsembleArrayArrayPath(reader->readDataArrayPath("NewEnsembleArrayArrayPath", getNewEnsembleArrayArrayPath()));
  setBiasedFeaturesArrayPath(reader->readDataArrayPath("BiasedFeaturesArrayPath", getBiasedFeaturesArrayPath() ) );
  setFeaturePhasesArrayPath(reader->readDataArrayPath("FeaturePhasesArrayPath", getFeaturePhasesArrayPath() ) );
  setSelectedFeatureArrayPath( reader->readDataArrayPath( "SelectedFeatureArrayPath", getSelectedFeatureArrayPath() ) );
  setNumberOfBins( reader->readValue( "NumberOfBins", getNumberOfBins() ) );
  setRemoveBiasedFeatures( reader->readValue( "RemoveBiasedFeatures", getRemoveBiasedFeatures() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int FindFeatureHistogram::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  DREAM3D_FILTER_WRITE_PARAMETER(FilterVersion)
  DREAM3D_FILTER_WRITE_PARAMETER(NewEnsembleArrayArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(BiasedFeaturesArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(FeaturePhasesArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(SelectedFeatureArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(NumberOfBins)
  DREAM3D_FILTER_WRITE_PARAMETER(RemoveBiasedFeatures)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindFeatureHistogram::dataCheck()
{
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

  int numComp = m_NumberOfBins;
  getNewEnsembleArrayArrayPath().setDataArrayName(m_SelectedFeatureArrayPath.getDataArrayName() + QString("Histogram"));
  dims[0] = numComp;
  m_NewEnsembleArrayPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int>, AbstractFilter>(this, getNewEnsembleArrayArrayPath(), 0, dims); /* Assigns the shared_ptr<>(this, tempPath, 0, dims); Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_NewEnsembleArrayPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_NewEnsembleArray = m_NewEnsembleArrayPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  if(m_RemoveBiasedFeatures == true)
  {
    dims[0] = 1;
    m_BiasedFeaturesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<bool>, AbstractFilter>(this, getBiasedFeaturesArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if( NULL != m_BiasedFeaturesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    { m_BiasedFeatures = m_BiasedFeaturesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindFeatureHistogram::preflight()
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
void findHistogram(IDataArray::Pointer inputData, int32_t* ensembleArray, int32_t* eIds, int NumberOfBins, bool removeBiasedFeatures, bool* biasedFeatures)
{
  DataArray<T>* featureArray = DataArray<T>::SafePointerDownCast(inputData.get());
  if (NULL == featureArray)
  {
    return;
  }

  T* fPtr = featureArray->getPointer(0);
  size_t numfeatures = featureArray->getNumberOfTuples();

  int32_t bin;
  int32_t ensemble;
  float min = 1000000.0f;
  float max = 0.0f;
  float value;
  for (size_t i = 1; i < numfeatures; i++)
  {
    value = fPtr[i];
    if(value > max) { max = value; }
    if(value < min) { min = value; }
  }
  float stepsize = (max - min) / NumberOfBins;

  for (size_t i = 1; i < numfeatures; i++)
  {
    if(removeBiasedFeatures == false || biasedFeatures[i] == false)
    {
      ensemble = eIds[i];
      bin = (fPtr[i] - min) / stepsize;
      if(bin >= NumberOfBins) { bin = NumberOfBins - 1; }
      ensembleArray[(NumberOfBins * ensemble) + bin]++;
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindFeatureHistogram::execute()
{
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(m_SelectedFeatureArrayPath.getDataContainerName());

  QString ss;

  IDataArray::Pointer inputData = m->getAttributeMatrix(m_SelectedFeatureArrayPath.getAttributeMatrixName())->getAttributeArray(m_SelectedFeatureArrayPath.getDataArrayName());
  if (NULL == inputData.get())
  {
    ss = QObject::tr("Selected array '%1' does not exist in the Voxel Data Container. Was it spelled correctly?").arg(m_SelectedFeatureArrayPath.getDataArrayName());
    setErrorCondition(-11001);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  QString dType = inputData->getTypeAsString();
  IDataArray::Pointer p = IDataArray::NullPointer();
  if (dType.compare("int8_t") == 0)
  {
    findHistogram<int8_t>(inputData, m_NewEnsembleArray, m_FeaturePhases, m_NumberOfBins, m_RemoveBiasedFeatures, m_BiasedFeatures);
  }
  else if (dType.compare("uint8_t") == 0)
  {
    findHistogram<uint8_t>(inputData, m_NewEnsembleArray, m_FeaturePhases, m_NumberOfBins, m_RemoveBiasedFeatures, m_BiasedFeatures);
  }
  else if (dType.compare("int16_t") == 0)
  {
    findHistogram<int16_t>(inputData, m_NewEnsembleArray, m_FeaturePhases, m_NumberOfBins, m_RemoveBiasedFeatures, m_BiasedFeatures);
  }
  else if (dType.compare("uint16_t") == 0)
  {
    findHistogram<uint16_t>(inputData, m_NewEnsembleArray, m_FeaturePhases, m_NumberOfBins, m_RemoveBiasedFeatures, m_BiasedFeatures);
  }
  else if (dType.compare("int32_t") == 0)
  {
    findHistogram<int32_t>(inputData, m_NewEnsembleArray, m_FeaturePhases, m_NumberOfBins, m_RemoveBiasedFeatures, m_BiasedFeatures);
  }
  else if (dType.compare("uint32_t") == 0)
  {
    findHistogram<uint32_t>(inputData, m_NewEnsembleArray, m_FeaturePhases, m_NumberOfBins, m_RemoveBiasedFeatures, m_BiasedFeatures);
  }
  else if (dType.compare("int64_t") == 0)
  {
    findHistogram<int64_t>(inputData, m_NewEnsembleArray, m_FeaturePhases, m_NumberOfBins, m_RemoveBiasedFeatures, m_BiasedFeatures);
  }
  else if (dType.compare("uint64_t") == 0)
  {
    findHistogram<uint64_t>(inputData, m_NewEnsembleArray, m_FeaturePhases, m_NumberOfBins, m_RemoveBiasedFeatures, m_BiasedFeatures);
  }
  else if (dType.compare("float") == 0)
  {
    findHistogram<float>(inputData, m_NewEnsembleArray, m_FeaturePhases, m_NumberOfBins, m_RemoveBiasedFeatures, m_BiasedFeatures);
  }
  else if (dType.compare("double") == 0)
  {
    findHistogram<double>(inputData, m_NewEnsembleArray, m_FeaturePhases, m_NumberOfBins, m_RemoveBiasedFeatures, m_BiasedFeatures);
  }
  else if (dType.compare("bool") == 0)
  {
    findHistogram<bool>(inputData, m_NewEnsembleArray, m_FeaturePhases, m_NumberOfBins, m_RemoveBiasedFeatures, m_BiasedFeatures);
  }

  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer FindFeatureHistogram::newFilterInstance(bool copyFilterParameters)
{
  FindFeatureHistogram::Pointer filter = FindFeatureHistogram::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindFeatureHistogram::getCompiledLibraryName()
{ return StatisticsConstants::StatisticsBaseName; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindFeatureHistogram::getGroupName()
{ return DREAM3D::FilterGroups::StatisticsFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindFeatureHistogram::getSubGroupName()
{ return DREAM3D::FilterSubGroups::EnsembleStatsFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindFeatureHistogram::getHumanLabel()
{ return "Find Feature Histogram"; }

