/* ============================================================================
 * Copyright (c) 2012 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2012 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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

#include "LinkFeatureMapToCellArray.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
LinkFeatureMapToCellArray::LinkFeatureMapToCellArray() :
  AbstractFilter(),
  m_DataContainerName(DREAM3D::Defaults::VolumeDataContainerName),
  m_CellAttributeMatrixName(DREAM3D::Defaults::CellAttributeMatrixName),
  m_CellFeatureAttributeMatrixName(DREAM3D::Defaults::CellFeatureAttributeMatrixName),
  m_SelectedCellDataArrayName(""),
  m_SelectedCellData(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
LinkFeatureMapToCellArray::~LinkFeatureMapToCellArray()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void LinkFeatureMapToCellArray::setupFilterParameters()
{
  FilterParameterVector parameters;
  {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("Cell Array Name");
    parameter->setPropertyName("SelectedCellDataArrayName");
    parameter->setWidgetType(FilterParameterWidgetType::VolumeCellArrayNameSelectionWidget);
    parameter->setValueType("QString");
    parameter->setUnits("");
    parameters.push_back(parameter);
  }

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void LinkFeatureMapToCellArray::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  /* Code to read the values goes between these statements */
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE BEGIN*/
  setSelectedCellDataArrayName( reader->readString( "SelectedCellDataArrayName", getSelectedCellDataArrayName() ) );
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE END*/
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int LinkFeatureMapToCellArray::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("SelectedCellDataArrayName", getSelectedCellDataArrayName() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void LinkFeatureMapToCellArray::dataCheck()
{
  setErrorCondition(0);

  VolumeDataContainer* m = getDataContainerArray()->getPrereqDataContainer<VolumeDataContainer, AbstractFilter>(this, getDataContainerName(), false);
  if(getErrorCondition() < 0 || NULL == m) { return; }
  AttributeMatrix::Pointer cellAttrMat = m->getPrereqAttributeMatrix<AbstractFilter>(this, getCellAttributeMatrixName(), -301);
  if(getErrorCondition() < 0) { return; }
  AttributeMatrix::Pointer cellFeatureAttrMat = m->getPrereqAttributeMatrix<AbstractFilter>(this, getCellFeatureAttributeMatrixName(), -301);
  if(getErrorCondition() < 0) { return; }

  IDataArray::Pointer data = cellAttrMat->getAttributeArray(m_SelectedCellDataArrayName);
  if (NULL == data.get())
  {
    QString ss = QObject::tr("Selected array '%1' does not exist in the Voxel Data Container. Was it spelled correctly?").arg(m_SelectedCellDataArrayName);
    setErrorCondition(-11001);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  QString dType = data->getTypeAsString();
  IDataArray::Pointer p = IDataArray::NullPointer();
  if (dType.compare("int32_t") == 0)
  {
    DataArray<int32_t>* feature = DataArray<int32_t>::SafePointerDownCast(data.get());
    m_SelectedCellData = feature->getPointer(0);
  }
  else
  {
    QString ss = QObject::tr("Selected array '%1' is not an Integer array. Is this the array you want to use?").arg(m_SelectedCellDataArrayName);
    setErrorCondition(-11001);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  cellFeatureAttrMat->clearAttributeArrays();
  QVector<size_t> dims(1, 1);
  cellFeatureAttrMat->createAndAddAttributeArray<DataArray<bool>, bool>(DREAM3D::FeatureData::Active, false, dims);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void LinkFeatureMapToCellArray::preflight()
{
  dataCheck();
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void LinkFeatureMapToCellArray::execute()
{
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());
  int64_t totalPoints = m->getAttributeMatrix(getCellAttributeMatrixName())->getNumTuples();

  // We get the attribute maxtrix here but no need to check it as that should have been done in the "dataCheck()" function
  AttributeMatrix::Pointer attrMatrix = m->getAttributeMatrix(getCellFeatureAttributeMatrixName());
  attrMatrix->clearAttributeArrays();

  int maxIndex = 0;
  std::vector<bool> active;
  for(int64_t i = 0; i < totalPoints; i++)
  {
    int index = m_SelectedCellData[i];
    if((index + 1) > maxIndex)
    {
      active.resize(index + 1);
      active[index] = true;
      maxIndex = index + 1;
    }
  }

  BoolArrayType::Pointer m_Active = BoolArrayType::CreateArray(maxIndex, DREAM3D::FeatureData::Active);
  bool* mActive = m_Active->getPointer(0);
  for(int i = 0; i < maxIndex; i++)
  {
    mActive[i] = active[i];
  }
  attrMatrix->addAttributeArray(DREAM3D::FeatureData::Active, m_Active);

  notifyStatusMessage(getHumanLabel(), "Complete");
}


