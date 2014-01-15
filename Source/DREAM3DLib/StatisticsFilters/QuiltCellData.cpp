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

#include "QuiltCellData.h"

#include "DREAM3DLib/Common/Constants.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QuiltCellData::QuiltCellData() :
  AbstractFilter(),
  m_DataContainerName(DREAM3D::Defaults::VolumeDataContainerName),
  m_NewDataContainerName(DREAM3D::Defaults::NewVolumeDataContainerName),
  m_CellAttributeMatrixName(DREAM3D::Defaults::CellAttributeMatrixName),
  m_NewCellAttributeMatrixName(DREAM3D::Defaults::CellAttributeMatrixName),
  m_SelectedCellArrayName("")
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QuiltCellData::~QuiltCellData()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QuiltCellData::setupFilterParameters()
{
  FilterParameterVector parameters;
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Cell Array To Quilt");
    option->setPropertyName("SelectedFeatureArrayName");
    option->setWidgetType(FilterParameterWidgetType::VolumeCellArrayNameSelectionWidget);
    option->setValueType("string");
    option->setUnits("");
    parameters.push_back(option);
  }
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QuiltCellData::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  /* Code to read the values goes between these statements */
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE BEGIN*/
  setSelectedFeatureArrayName( reader->readString( "SelectedCellArrayName", getSelectedCellArrayName() ) );
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE END*/
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int QuiltCellData::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("SelectedCellArrayName", getSelectedCellArrayName() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QuiltCellData::dataCheck()
{
  setErrorCondition(0);

  VolumeDataContainer* m = getDataContainerArray()->getPrereqDataContainer<VolumeDataContainer, QuiltCellData>(this, getDataContainerName());
  if(getErrorCondition() < 0) { return; }
  VolumeDataContainer* m2 = getDataContainerArray()->createNonPrereqDataContainer<VolumeDataContainer, QuiltCellData>(this, getNewDataContainerName());
  if(getErrorCondition() < 0) { return; }
  AttributeMatrix::Pointer cellAttrMat = m->getPrereqAttributeMatrix<AbstractFilter>(this, getCellAttributeMatrixName(), -301);
  if(getErrorCondition() < 0) { return; }
  AttributeMatrix::Pointer newCellAttrMat = m2->getPrereqAttributeMatrix<AbstractFilter>(this, getNewCellAttributeMatrixName(), -302);
  if(getErrorCondition() < 0) { return; }

  if(m_SelectedCellArrayName.isEmpty() == true)
  {
    setErrorCondition(-11000);
    notifyErrorMessage(getHumanLabel(), "An array from the Volume DataContainer must be selected.", getErrorCondition());
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QuiltCellData::preflight()
{
  dataCheck();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T>
float quiltData(IDataArray::Pointer inputData)
{
  DataArray<T>* cellArray = DataArray<T>::SafePointerDownCast(inputData.get());
  if (NULL == cellArray)
  {
    return 0.0;
  }

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
      ePtr[numComp * i + j] = data->getValue(j);
    }
  }
  return ensembleArray;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QuiltCellData::execute()
{
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

  QString ss;

  ss = selectedCellArrayName + QString("Quilt");
  QVector<size_t> dims(1, 1);
  typename DataArray<float>::Pointer newCellArray = DataArray<float>::CreateArray(ensembles, dims, ss);

  IDataArray::Pointer inputData = m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->getAttributeArray(m_SelectedCellArrayName);

  if (NULL == inputData.get())
  {
    ss = QObject::tr("Selected array '%1' does not exist in the Voxel Data Container. Was it spelled correctly?").arg(m_SelectedCellArrayName);
    setErrorCondition(-11001);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  QString dType = inputData->getTypeAsString();
  IDataArray::Pointer p = IDataArray::NullPointer();
  if (dType.compare("int8_t") == 0)
  {
    p = quiltData<int8_t>(inputData);
  }
  else if (dType.compare("uint8_t") == 0)
  {
    p = quiltData<uint8_t>(inputData);
  }
  else if (dType.compare("int16_t") == 0)
  {
    p = quiltData<int16_t>(inputData);
  }
  else if (dType.compare("uint16_t") == 0)
  {
    p = quiltData<uint16_t>(inputData);
  }
  else if (dType.compare("int32_t") == 0)
  {
    p = quiltData<int32_t>(inputData);
  }
  else if (dType.compare("uint32_t") == 0)
  {
    p = quiltData<uint32_t>(inputData);
  }
  else if (dType.compare("int64_t") == 0)
  {
    p = quiltData<int64_t>(inputData);
  }
  else if (dType.compare("uint64_t") == 0)
  {
    p = quiltData<uint64_t>(inputData);
  }
  else if (dType.compare("float") == 0)
  {
    p = quiltData<float>(inputData);
  }
  else if (dType.compare("double") == 0)
  {
    p = quiltData<double>(inputData);
  }
  else if (dType.compare("bool") == 0)
  {
    p = quiltData<bool>(inputData);
  }

  m->getAttributeMatrix(getCellEnsembleAttributeMatrixName())->addAttributeArray(p->getName(), p);
  notifyStatusMessage(getHumanLabel(), "Complete");
}


