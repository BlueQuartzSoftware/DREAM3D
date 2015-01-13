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

#include "ConvertArrayTo8BitImage.h"

#include "DREAM3DLib/Common/Constants.h"

#include "Statistics/DistributionAnalysisOps/BetaOps.h"
#include "Statistics/DistributionAnalysisOps/PowerLawOps.h"
#include "Statistics/DistributionAnalysisOps/LogNormalOps.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ConvertArrayTo8BitImage::ConvertArrayTo8BitImage() :
  AbstractFilter(),
  m_SelectedArrayPath("", "", "")
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ConvertArrayTo8BitImage::~ConvertArrayTo8BitImage()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ConvertArrayTo8BitImage::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(FilterParameter::New("Feature Array To Bin", "SelectedFeatureArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getSelectedFeatureArrayPath(), false));
  parameters.push_back(FilterParameter::New("Required Information", "", FilterParameterWidgetType::SeparatorWidget, "", true));
  parameters.push_back(FilterParameter::New("Created Information", "", FilterParameterWidgetType::SeparatorWidget, "", true));
  parameters.push_back(FilterParameter::New("New Array Name", "NewArrayName", FilterParameterWidgetType::StringWidget, getNewArrayName(), true, ""));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ConvertArrayTo8BitImage::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setNewArrayName(reader->readString("NewArrayName", getNewArrayName() ) );
  setSelectedArrayPath( reader->readDataArrayPath( "SelectedArrayPath", getSelectedArrayPath() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int ConvertArrayTo8BitImage::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  DREAM3D_FILTER_WRITE_PARAMETER(NewArrayName)
  DREAM3D_FILTER_WRITE_PARAMETER(SelectedArrayPath)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ConvertArrayTo8BitImage::dataCheck()
{
  DataArrayPath tempPath;
  setErrorCondition(0);

  if(m_SelectedArrayPath.isEmpty() == true)
  {
    setErrorCondition(-11000);
    notifyErrorMessage(getHumanLabel(), "An array from the Volume DataContainer must be selected.", getErrorCondition());
  }
  IDataArray::Pointer inputData = getDataContainerArray()->getDataContainer(m_SelectedArrayPath.getDataContainerName())->getAttributeMatrix(m_SelectedArrayPath.getAttributeMatrixName())->getAttributeArray(m_SelectedArrayPath.getDataArrayName());
  if (NULL == inputData.get())
  {
    QString ss = QObject::tr("Data array '%1' does not exist in the Voxel Data Container. Was it spelled correctly?").arg(m_SelectedArrayPath.getDataArrayName());
    setErrorCondition(-11001);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }
  else
  {
    QVector<size_t> dims(1, 1);
    DataArray<uint8>::Pointer data = DataArray<uint8>::CreateArray(inputData->getNumTuples(), dims);
    getDataContainerArray()->getDataContainer(m_SelectedArrayPath.getDataContainerName())->getAttributeMatrix(m_SelectedArrayPath.getAttributeMatrixName())->addAttributeArray(m_NewArrayName, data);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ConvertArrayTo8BitImage::preflight()
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
void ConvertArrayTo8BitImage::execute()
{
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(m_SelectedFeatureArrayPath.getDataContainerName());

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
AbstractFilter::Pointer ConvertArrayTo8BitImage::newFilterInstance(bool copyFilterParameters)
{
  ConvertArrayTo8BitImage::Pointer filter = ConvertArrayTo8BitImage::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ConvertArrayTo8BitImage::getCompiledLibraryName()
{ return Statistics::StatisticsBaseName; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ConvertArrayTo8BitImage::getGroupName()
{ return DREAM3D::FilterGroups::StatisticsFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ConvertArrayTo8BitImage::getSubGroupName()
{ return DREAM3D::FilterSubGroups::EnsembleStatsFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ConvertArrayTo8BitImage::getHumanLabel()
{ return "Find Feature Histogram"; }

