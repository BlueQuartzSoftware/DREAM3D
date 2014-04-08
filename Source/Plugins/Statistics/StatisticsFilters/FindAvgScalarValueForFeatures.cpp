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

#include "FindAvgScalarValueForFeatures.h"

#include "DREAM3DLib/Common/Constants.h"

#include "DREAM3DLib/DistributionAnalysisOps/BetaOps.h"
#include "DREAM3DLib/DistributionAnalysisOps/PowerLawOps.h"
#include "DREAM3DLib/DistributionAnalysisOps/LogNormalOps.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindAvgScalarValueForFeatures::FindAvgScalarValueForFeatures() :
  AbstractFilter(),
  m_DataContainerName(DREAM3D::Defaults::VolumeDataContainerName),
  m_CellFeatureAttributeMatrixName(DREAM3D::Defaults::CellFeatureAttributeMatrixName),
  m_SelectedCellArrayPath("", "", ""),
  /*[]*/m_FeatureIdsArrayPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, DREAM3D::CellData::FeatureIds),
  m_NewFeatureArrayArrayName(""),
  m_FeatureIdsArrayName(DREAM3D::CellData::FeatureIds),
  m_FeatureIds(NULL),
  m_NewFeatureArray(NULL)
{

  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindAvgScalarValueForFeatures::~FindAvgScalarValueForFeatures()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindAvgScalarValueForFeatures::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(FilterParameter::New("Cell Array To Average", "SelectedCellArrayPath", FilterParameterWidgetType::DataArraySelectionWidget,"DataArrayPath", false));
  parameters.push_back(FilterParameter::New("Required Information", "", FilterParameterWidgetType::SeparatorWidget, "QString", true));
/*[]*/parameters.push_back(FilterParameter::New("FeatureIds", "FeatureIdsArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, "DataArrayPath", true, ""));
  parameters.push_back(FilterParameter::New("Created Information", "", FilterParameterWidgetType::SeparatorWidget, "QString", true));
/*##*/parameters.push_back(FilterParameter::New("NewFeatureArray", "NewFeatureArrayArrayName", FilterParameterWidgetType::StringWidget, "QString", true, ""));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindAvgScalarValueForFeatures::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
/*[]*/setNewFeatureArrayArrayName(reader->readString("NewFeatureArrayArrayName", getNewFeatureArrayArrayName() ) );
  setFeatureIdsArrayPath(reader->readDataArrayPath("FeatureIdsArrayPath", getFeatureIdsArrayPath() ) );
  setSelectedCellArrayPath( reader->readDataArrayPath( "SelectedCellArrayPath", getSelectedCellArrayPath() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int FindAvgScalarValueForFeatures::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
/*[]*/writer->writeValue("NewFeatureArrayArrayName", getNewFeatureArrayArrayName() );
  writer->writeValue("FeatureIdsArrayPath", getFeatureIdsArrayPath() );
  writer->writeValue("SelectedCellArrayPath", getSelectedCellArrayPath() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindAvgScalarValueForFeatures::dataCheck()
{
  DataArrayPath tempPath;
  setErrorCondition(0);

  QString cellAttrMatName = getSelectedCellArrayPath().getAttributeMatrixName();

  VolumeDataContainer* m = getDataContainerArray()->getPrereqDataContainer<VolumeDataContainer, AbstractFilter>(this, getDataContainerName(), false);
  if(getErrorCondition() < 0 || NULL == m) { return; }
  AttributeMatrix::Pointer cellFeatureAttrMat = m->getPrereqAttributeMatrix<AbstractFilter>(this, getCellFeatureAttributeMatrixName(), -301);
  if(getErrorCondition() < 0) { return; }
  AttributeMatrix::Pointer cellAttrMat = m->getPrereqAttributeMatrix<AbstractFilter>(this, cellAttrMatName, -301);
  if(getErrorCondition() < 0) { return; }

  QVector<size_t> dims(1, 1);
  m_FeatureIdsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getFeatureIdsArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeatureIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  if(m_SelectedCellArrayPath.isEmpty() == true)
  {
    setErrorCondition(-11000);
    notifyErrorMessage(getHumanLabel(), "An array from the Volume DataContainer must be selected.", getErrorCondition());
    return;
  }

  QString m_NewFeatureArrayArrayName = QString("Average") + m_SelectedCellArrayPath.getDataArrayName();
  dims[0] = 1;
  m_NewFeatureArrayPtr = cellFeatureAttrMat->createNonPrereqArray<DataArray<float>, AbstractFilter, float>(this, m_NewFeatureArrayArrayName, 0.0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
////==>MIKE_GROEBER_FIX tempPath.update(DATACONTAINER_NAME, ATTRIBUTEMATRIX_NAME, getNewFeatureArrayArrayName() );
////==>MIKE_GROEBER_FIX m_NewFeatureArrayPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this, tempPath, 0.0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_NewFeatureArrayPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_NewFeatureArray = m_NewFeatureArrayPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindAvgScalarValueForFeatures::preflight()
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
void findAverage(IDataArray::Pointer inputData, FloatArrayType::Pointer averageArray, int32_t* fIds)
{
  DataArray<T>* cellArray = DataArray<T>::SafePointerDownCast(inputData.get());
  if (NULL == cellArray) return;

  T* cPtr = cellArray->getPointer(0);
  float* aPtr = averageArray->getPointer(0);
  size_t numPoints = cellArray->getNumberOfTuples();
  size_t numFeatures = averageArray->getNumberOfTuples();

  QVector<float> counts(numFeatures, 0.0);

  int32_t feature;
  float value;
  for (size_t i = 0; i < numPoints; i++)
  {
    value = float(cPtr[i]);
    feature = fIds[i];
    aPtr[feature] += value;
    counts[feature] += 1.0;
  }
  for (size_t i = 1; i < numFeatures; i++)
  {
    if(counts[i] == 0) aPtr[i] = 0;
    else aPtr[i] /= float(counts[i]);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindAvgScalarValueForFeatures::execute()
{
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

  QString ss;

  IDataArray::Pointer inputData = m->getAttributeMatrix(m_SelectedCellArrayPath.getAttributeMatrixName())->getAttributeArray(m_SelectedCellArrayPath.getDataArrayName());
  if (NULL == inputData.get())
  {
    ss = QObject::tr("Selected array '%1' does not exist. Was it spelled correctly?").arg(m_SelectedCellArrayPath.getDataArrayName());
    setErrorCondition(-11001);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  QString dType = inputData->getTypeAsString();
  IDataArray::Pointer p = IDataArray::NullPointer();
  if (dType.compare("int8_t") == 0)
  {
    findAverage<int8_t>(inputData, m_NewFeatureArrayPtr.lock(), m_FeatureIds);
  }
  else if (dType.compare("uint8_t") == 0)
  {
    findAverage<uint8_t>(inputData, m_NewFeatureArrayPtr.lock(), m_FeatureIds);
  }
  else if (dType.compare("int16_t") == 0)
  {
    findAverage<int16_t>(inputData, m_NewFeatureArrayPtr.lock(), m_FeatureIds);
  }
  else if (dType.compare("uint16_t") == 0)
  {
    findAverage<uint16_t>(inputData, m_NewFeatureArrayPtr.lock(), m_FeatureIds);
  }
  else if (dType.compare("int32_t") == 0)
  {
    findAverage<int32_t>(inputData, m_NewFeatureArrayPtr.lock(), m_FeatureIds);
  }
  else if (dType.compare("uint32_t") == 0)
  {
    findAverage<uint32_t>(inputData, m_NewFeatureArrayPtr.lock(), m_FeatureIds);
  }
  else if (dType.compare("int64_t") == 0)
  {
    findAverage<int64_t>(inputData, m_NewFeatureArrayPtr.lock(), m_FeatureIds);
  }
  else if (dType.compare("uint64_t") == 0)
  {
    findAverage<uint64_t>(inputData, m_NewFeatureArrayPtr.lock(), m_FeatureIds);
  }
  else if (dType.compare("float") == 0)
  {
    findAverage<float>(inputData, m_NewFeatureArrayPtr.lock(), m_FeatureIds);
  }
  else if (dType.compare("double") == 0)
  {
    findAverage<double>(inputData, m_NewFeatureArrayPtr.lock(), m_FeatureIds);
  }
  else if (dType.compare("bool") == 0)
  {
    ss = QObject::tr("Selected array '%1' cannot be of type Bool?").arg(m_SelectedCellArrayPath.getDataArrayName());
    setErrorCondition(-11002);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer FindAvgScalarValueForFeatures::newFilterInstance(bool copyFilterParameters)
{
  FindAvgScalarValueForFeatures::Pointer filter = FindAvgScalarValueForFeatures::New();
  if(true == copyFilterParameters)
  {
    filter->setNewFeatureArrayArrayName(getNewFeatureArrayArrayName());
    filter->setFeatureIdsArrayPath(getFeatureIdsArrayPath());
    filter->setSelectedCellArrayPath( getSelectedCellArrayPath() );
  }
  return filter;
}
