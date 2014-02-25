/* ============================================================================
 * Copyright (c) 2011 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2011 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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
 * Neither the name of Michael A. Groeber, Michael A. Jackson, the US Air Force,
 * BlueQuartz Software nor the names of its contributors may be used to endorse
 * or promote products derived from this software without specific prior written
 * permission.
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
 *                           FA8650-07-D-5800
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "FindProjectedImageStatistics.h"

#include "DREAM3DLib/Math/DREAM3DMath.h"
#include "DREAM3DLib/Common/Constants.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindProjectedImageStatistics::FindProjectedImageStatistics() :
  AbstractFilter(),
  m_DataContainerName(DREAM3D::Defaults::VolumeDataContainerName),
  m_CellAttributeMatrixName(DREAM3D::Defaults::CellAttributeMatrixName),
  m_Plane(0),
  m_ProjectedImageMinArrayName(DREAM3D::CellData::ProjectedImageMin),
  m_ProjectedImageMin(NULL),
  m_ProjectedImageMaxArrayName(DREAM3D::CellData::ProjectedImageMax),
  m_ProjectedImageMax(NULL),
  m_ProjectedImageAvgArrayName(DREAM3D::CellData::ProjectedImageAvg),
  m_ProjectedImageAvg(NULL),
  m_ProjectedImageStdArrayName(DREAM3D::CellData::ProjectedImageStd),
  m_ProjectedImageStd(NULL),
  m_ProjectedImageVarArrayName(DREAM3D::CellData::ProjectedImageVar),
  m_ProjectedImageVar(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindProjectedImageStatistics::~FindProjectedImageStatistics()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindProjectedImageStatistics::setupFilterParameters()
{
  FilterParameterVector parameters;
  {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("Cell Array To Bin");
    parameter->setPropertyName("SelectedCellArrayName");
    parameter->setWidgetType(FilterParameterWidgetType::SingleArraySelectionWidget);
    parameter->setValueType("QString");
    parameter->setUnits("");
    parameters.push_back(parameter);
  }
  {
    ChoiceFilterParameter::Pointer parameter = ChoiceFilterParameter::New();
    parameter->setHumanLabel("Plane of Interest");
    parameter->setPropertyName("Plane");
    parameter->setWidgetType(FilterParameterWidgetType::ChoiceWidget);
    parameter->setValueType("unsigned int");
    QVector<QString> choices;
    choices.push_back("XY");
    choices.push_back("XZ");
    choices.push_back("YZ");
    parameter->setChoices(choices);
    parameters.push_back(parameter);
  }
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
void FindProjectedImageStatistics::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE BEGIN*/
  setSelectedCellArrayName( reader->readString( "SelectedcellArrayName", getSelectedCellArrayName() ) );
  setPlane( reader->readValue("Plane", getPlane()));
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE END*/
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int FindProjectedImageStatistics::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("SelectedCellArrayName", getSelectedCellArrayName() );
  writer->writeValue("Plane", getPlane());
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindProjectedImageStatistics::dataCheck()
{
  setErrorCondition(0);

  VolumeDataContainer* m = getDataContainerArray()->getPrereqDataContainer<VolumeDataContainer, AbstractFilter>(this, getDataContainerName(), false);
  if(getErrorCondition() < 0 || NULL == m) { return; }
  AttributeMatrix::Pointer cellFeatureAttrMat = m->getPrereqAttributeMatrix<AbstractFilter>(this, getCellFeatureAttributeMatrixName(), -304);
  if(getErrorCondition() < 0) { return; }
  AttributeMatrix::Pointer cellAttrMat = m->getPrereqAttributeMatrix<AbstractFilter>(this, getCellAttributeMatrixName(), -301);
  if(getErrorCondition() < 0 || NULL == cellAttrMat.get() ) { return; }

  if(m_SelectedCellArrayName.isEmpty() == true)
  {
    setErrorCondition(-11000);
    notifyErrorMessage(getHumanLabel(), "An array from the Volume DataContainer must be selected.", getErrorCondition());
  }

  QVector<size_t> dims(1, 1);
  m_ProjectedImageMinPtr = cellFeatureAttrMat->createNonPrereqArray<DataArray<float>, AbstractFilter, float>(this, m_ProjectedImageMinArrayName, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_ProjectedImageMinPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_ProjectedImageMin = m_ProjectedImageMinPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  m_ProjectedImageMaxPtr = cellFeatureAttrMat->createNonPrereqArray<DataArray<float>, AbstractFilter, float>(this, m_ProjectedImageMaxArrayName, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_ProjectedImageMaxPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_ProjectedImageMax = m_ProjectedImageMaxPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  m_ProjectedImageAvgPtr = cellFeatureAttrMat->createNonPrereqArray<DataArray<float>, AbstractFilter, float>(this, m_ProjectedImageAvgArrayName, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_ProjectedImageAvgPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_ProjectedImageAvg = m_ProjectedImageAvgPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  m_ProjectedImageStdPtr = cellFeatureAttrMat->createNonPrereqArray<DataArray<float>, AbstractFilter, float>(this, m_ProjectedImageStdArrayName, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_ProjectedImageStdPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_ProjectedImageStd = m_ProjectedImageStdPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  m_ProjectedImageVarPtr = cellFeatureAttrMat->createNonPrereqArray<DataArray<float>, AbstractFilter, float>(this, m_ProjectedImageVarArrayName, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_ProjectedImageVarPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_ProjectedImageVar = m_ProjectedImageVarPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindProjectedImageStatistics::preflight()
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
void findProjectedStats(IDataArray::Pointer inputData, float* m_ProjectedImageMin, float* m_ProjectedImageMax, float* m_ProjectedImageAvg, float* m_ProjectedImageStd, float* m_ProjectedImageVar)
{
  DataArray<T>* cellArray = DataArray<T>::SafePointerDownCast(inputData.get());
  if (NULL == cellArray)
  {
    return;
  }

  T* fPtr = cellArray->getPointer(0);
  size_t numfeatures = cellArray->getNumberOfTuples();


}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindProjectedImageStatistics::execute()
{
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

  if(m->getXPoints() <= 1 && m->getYPoints() <= 1 && m->getZPoints() <= 1)
  {
    setErrorCondition(-999);
    notifyErrorMessage(getHumanLabel(), "The volume is not 3D and cannot be run through this filter", -999);
    return;
  }

  QString ss;

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
    findProjectedStats<int8_t>(inputData, m_ProjectedImageMin, m_ProjectedImageMax, m_ProjectedImageAvg, m_ProjectedImageStd, m_ProjectedImageVar);
  }
  else if (dType.compare("uint8_t") == 0)
  {
    findProjectedStats<uint8_t>(inputData, m_ProjectedImageMin, m_ProjectedImageMax, m_ProjectedImageAvg, m_ProjectedImageStd, m_ProjectedImageVar);
  }
  else if (dType.compare("int16_t") == 0)
  {
    findProjectedStats<int16_t>(inputData, m_ProjectedImageMin, m_ProjectedImageMax, m_ProjectedImageAvg, m_ProjectedImageStd, m_ProjectedImageVar);
  }
  else if (dType.compare("uint16_t") == 0)
  {
    findProjectedStats<uint16_t>(inputData, m_ProjectedImageMin, m_ProjectedImageMax, m_ProjectedImageAvg, m_ProjectedImageStd, m_ProjectedImageVar);
  }
  else if (dType.compare("int32_t") == 0)
  {
    findProjectedStats<int32_t>(inputData, m_ProjectedImageMin, m_ProjectedImageMax, m_ProjectedImageAvg, m_ProjectedImageStd, m_ProjectedImageVar);
  }
  else if (dType.compare("uint32_t") == 0)
  {
    findProjectedStats<uint32_t>(inputData, m_ProjectedImageMin, m_ProjectedImageMax, m_ProjectedImageAvg, m_ProjectedImageStd, m_ProjectedImageVar);
  }
  else if (dType.compare("int64_t") == 0)
  {
    findProjectedStats<int64_t>(inputData, m_ProjectedImageMin, m_ProjectedImageMax, m_ProjectedImageAvg, m_ProjectedImageStd, m_ProjectedImageVar);
  }
  else if (dType.compare("uint64_t") == 0)
  {
    findProjectedStats<uint64_t>(inputData, m_ProjectedImageMin, m_ProjectedImageMax, m_ProjectedImageAvg, m_ProjectedImageStd, m_ProjectedImageVar);
  }
  else if (dType.compare("float") == 0)
  {
    findProjectedStats<float>(inputData, m_ProjectedImageMin, m_ProjectedImageMax, m_ProjectedImageAvg, m_ProjectedImageStd, m_ProjectedImageVar);
  }
  else if (dType.compare("double") == 0)
  {
    findProjectedStats<double>(inputData, m_ProjectedImageMin, m_ProjectedImageMax, m_ProjectedImageAvg, m_ProjectedImageStd, m_ProjectedImageVar);
  }
  else if (dType.compare("bool") == 0)
  {
    ss = QObject::tr("Selected array '%1' cannot be of type bool").arg(m_SelectedCellArrayName);
    setErrorCondition(-11001);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  notifyStatusMessage(getHumanLabel(), "FindProjectedImageStatistics Completed");
}

