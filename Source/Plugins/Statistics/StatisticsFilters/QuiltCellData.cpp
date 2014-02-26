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
  m_CellAttributeMatrixName(DREAM3D::Defaults::CellAttributeMatrixName),
  m_SelectedCellArrayName(""),
  m_OutputDataContainerName(DREAM3D::Defaults::NewVolumeDataContainerName),
  m_OutputAttributeMatrixName(DREAM3D::Defaults::CellAttributeMatrixName),
  m_OutputArrayName("Quilt_Data"),
  m_OutputArray(NULL)
{
  m_QuiltStep.x = 0;
  m_QuiltStep.y = 0;
  m_QuiltStep.z = 0;

  m_PatchSize.x = 0;
  m_PatchSize.y = 0;
  m_PatchSize.z = 0;

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
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("Cell Array To Quilt");
    parameter->setPropertyName("SelectedCellArrayName");
    parameter->setWidgetType(FilterParameterWidgetType::SingleArraySelectionWidget);
    parameter->setValueType("QString");
    parameter->setUnits("");
    parameters.push_back(parameter);
  }
  {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("Quilt Step");
    parameter->setPropertyName("QuiltStep");
    parameter->setWidgetType(FilterParameterWidgetType::IntVec3Widget);
    parameter->setValueType("IntVec3_t");
    parameter->setUnits("Voxels");
    parameters.push_back(parameter);
  }
  {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("Patch Size");
    parameter->setPropertyName("Dimensions");
    parameter->setWidgetType(FilterParameterWidgetType::IntVec3Widget);
    parameter->setValueType("IntVec3_t");
    parameter->setUnits("Voxels");
    parameters.push_back(parameter);
  }
  {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("Output DataContainer Name");
    parameter->setPropertyName("OutputDataContainerName");
    parameter->setWidgetType(FilterParameterWidgetType::StringWidget);
    parameter->setValueType("QString");
    parameters.push_back(parameter);
  }
  {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("Output AttributeMatrix Name");
    parameter->setPropertyName("OutputAttributeMatrixName");
    parameter->setWidgetType(FilterParameterWidgetType::StringWidget);
    parameter->setValueType("QString");
    parameters.push_back(parameter);
  }
  {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("Output Data Array Name");
    parameter->setPropertyName("OutputArrayName");
    parameter->setWidgetType(FilterParameterWidgetType::StringWidget);
    parameter->setValueType("QString");
    parameters.push_back(parameter);
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
  setSelectedCellArrayName( reader->readString( "SelectedCellArrayName", getSelectedCellArrayName() ) );
  setOutputDataContainerName( reader->readString( "OutputDataContainerName", getOutputDataContainerName() ) );
  setOutputAttributeMatrixName( reader->readString( "OutputAttributeMatrixName", getOutputAttributeMatrixName() ) );
  setOutputArrayName( reader->readString( "OutputArrayName", getOutputArrayName() ) );
  setQuiltStep( reader->readIntVec3("QuiltSte", getQuiltStep() ) );
  setPatchSize( reader->readIntVec3("PatchSize", getPatchSize() ) );
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
  writer->writeValue("OutputDataContainerName", getOutputDataContainerName() );
  writer->writeValue("OutputAttributeMatrixName", getOutputAttributeMatrixName() );
  writer->writeValue("OutputArrayName", getOutputArrayName() );
  writer->writeValue("QuiltSte", getQuiltStep() );
  writer->writeValue("PatchSize", getPatchSize() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QuiltCellData::dataCheck()
{
  setErrorCondition(0);

  // First sanity check the inputs and output names. All must be filled in

  if(m_SelectedCellArrayName.isEmpty() == true)
  {
    QString ss = QObject::tr("The input array name is empty. Please select a name for the input array");
    setErrorCondition(-11000);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

  if(getOutputDataContainerName().isEmpty() == true)
  {
    QString ss = QObject::tr("The output DataContainer name is empty. Please assign a name for the created DataContainer");
    setErrorCondition(-11000);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }


  if(getOutputAttributeMatrixName().isEmpty() == true)
  {
    QString ss = QObject::tr("The output AttributeMatrix name is empty. Please assign a name for the created AttributeMatrix");
    setErrorCondition(-11000);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

  if(getOutputArrayName().isEmpty() == true)
  {
    QString ss = QObject::tr("The output array name is empty. Please assign a name for the created array");
    setErrorCondition(-11000);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

  // we can not go any further until all of the names are set.
  if(getErrorCondition() < 0)
  {
    return;
  }

  // Next check the existing DataContainer/AttributeMatrix
  VolumeDataContainer* m = getDataContainerArray()->getPrereqDataContainer<VolumeDataContainer, AbstractFilter>(this, getDataContainerName());
  if(getErrorCondition() < 0) { return; }
  AttributeMatrix::Pointer cellAttrMat = m->getPrereqAttributeMatrix<AbstractFilter>(this, getCellAttributeMatrixName(), -301);
  if(getErrorCondition() < 0 || NULL == cellAttrMat.get() ) { return; }

  //Establish the dimensions, resolutions and origin of the new data container
  size_t dcDims[3] = { 0,0,0};
  m->getDimensions(dcDims[0], dcDims[1], dcDims[2]);
  float res[3] = {m_QuiltStep.x*m->getXRes(), m_QuiltStep.y*m->getYRes(), m_QuiltStep.z*m->getZRes()};

  // Create a new DataContainer
  VolumeDataContainer* m2 = getDataContainerArray()->createNonPrereqDataContainer<VolumeDataContainer, AbstractFilter>(this, getOutputDataContainerName());
  if(getErrorCondition() < 0) { return; }

  m2->setDimensions(int(dcDims[0]/m_QuiltStep.x), int(dcDims[1]/m_QuiltStep.y), int(dcDims[2]/m_QuiltStep.z));
  m2->setResolution(res);
  m2->setOrigin(0.0f, 0.0f, 0.0f);

  //Create the cell attrMat in the new data container
  QVector<size_t> tDims(3, 0);
  tDims[0] = m2->getXPoints();
  tDims[1] = m2->getYPoints();
  tDims[2] = m2->getZPoints();
  AttributeMatrix::Pointer newCellAttrMat = m2->createNonPrereqAttributeMatrix<AbstractFilter>(this, getOutputAttributeMatrixName(), tDims, DREAM3D::AttributeMatrixType::Cell);
  if(getErrorCondition() < 0) { return; }


  //Get the name and create the array in the new data attrMat
  QVector<size_t> dims(1, 1);
  m_OutputArrayPtr = newCellAttrMat->createNonPrereqArray<DataArray<float>, AbstractFilter, float>(this, getOutputArrayName(), 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_OutputArrayPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_OutputArray = m_OutputArrayPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QuiltCellData::preflight()
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
float quiltData(IDataArray::Pointer inputData, int xc, int yc, int zc, IntVec3_t pSize, int xDim, int yDim, int zDim)
{
  float value = 0.0;
  DataArray<T>* cellArray = DataArray<T>::SafePointerDownCast(inputData.get());
  if (NULL == cellArray)
  {
    return 0.0;
  }

  T* cPtr = cellArray->getPointer(0);

  int zStride, yStride;
  float count = 0;
  for(int k = -(pSize.z/2); k < (pSize.z/2); k++)
  {
    if((zc+k) >= 0 && (zc+k) < zDim)
    {
      zStride = ((zc+k)*xDim*yDim);
      for(int j = -(pSize.z/2); j < (pSize.y/2); j++)
      {
        if((yc+j) >= 0 && (yc+j) < yDim)
        {
          yStride = ((yc+j)*xDim);
          for(int i = -(pSize.x/2); i < (pSize.x/2); i++)
          {
            if((xc+i) >= 0 && (xc+i) < xDim)
            {
              value += cPtr[zStride+yStride+(xc+i)];
              count++;
            }
          }
        }
      }
    }
  }
  value /= count;
  return value;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QuiltCellData::execute()
{
  QString ss;
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

  size_t dcDims[3];
  m->getDimensions(dcDims[0], dcDims[1], dcDims[2]);
  int newXP = int(dcDims[0]/m_QuiltStep.x);
  int newYP = int(dcDims[1]/m_QuiltStep.y);
  int newZP = int(dcDims[2]/m_QuiltStep.z);

  IDataArray::Pointer inputData = m->getAttributeMatrix(getCellAttributeMatrixName())->getAttributeArray(m_SelectedCellArrayName);
  if (NULL == inputData.get())
  {
    ss = QObject::tr("Selected array '%1' does not exist in the Voxel Data Container. Was it spelled correctly?").arg(m_SelectedCellArrayName);
    setErrorCondition(-11001);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  QString dType = inputData->getTypeAsString();

  int zStride, yStride;
  int xc, yc, zc;
  for(int k = 0; k < newZP; k++)
  {
    zStride = (k*newXP*newYP);
    for(int j = 0; j < newYP; j++)
    {
      yStride = (j*newXP);
      for(int i = 0; i < newXP; i++)
      {
        xc = i*m_QuiltStep.x + m_QuiltStep.x/2;
        yc = j*m_QuiltStep.y + m_QuiltStep.y/2;
        zc = k*m_QuiltStep.z + m_QuiltStep.z/2;
        if (dType.compare("int8_t") == 0)
        {
          m_OutputArray[zStride+yStride+i] = quiltData<int8_t>(inputData, xc, yc, zc, m_PatchSize, dcDims[0], dcDims[1], dcDims[2]);
        }
        else if (dType.compare("uint8_t") == 0)
        {
          m_OutputArray[zStride+yStride+i] = quiltData<uint8_t>(inputData, xc, yc, zc, m_PatchSize, dcDims[0], dcDims[1], dcDims[2]);
        }
        else if (dType.compare("int16_t") == 0)
        {
          m_OutputArray[zStride+yStride+i] = quiltData<int16_t>(inputData, xc, yc, zc, m_PatchSize, dcDims[0], dcDims[1], dcDims[2]);
        }
        else if (dType.compare("uint16_t") == 0)
        {
          m_OutputArray[zStride+yStride+i] = quiltData<uint16_t>(inputData, xc, yc, zc, m_PatchSize, dcDims[0], dcDims[1], dcDims[2]);
        }
        else if (dType.compare("int32_t") == 0)
        {
          m_OutputArray[zStride+yStride+i] = quiltData<int32_t>(inputData, xc, yc, zc, m_PatchSize, dcDims[0], dcDims[1], dcDims[2]);
        }
        else if (dType.compare("uint32_t") == 0)
        {
          m_OutputArray[zStride+yStride+i] = quiltData<uint32_t>(inputData, xc, yc, zc, m_PatchSize, dcDims[0], dcDims[1], dcDims[2]);
        }
        else if (dType.compare("int64_t") == 0)
        {
          m_OutputArray[zStride+yStride+i] = quiltData<int64_t>(inputData, xc, yc, zc, m_PatchSize, dcDims[0], dcDims[1], dcDims[2]);
        }
        else if (dType.compare("uint64_t") == 0)
        {
          m_OutputArray[zStride+yStride+i] = quiltData<uint64_t>(inputData, xc, yc, zc, m_PatchSize, dcDims[0], dcDims[1], dcDims[2]);
        }
        else if (dType.compare("float") == 0)
        {
          m_OutputArray[zStride+yStride+i] = quiltData<float>(inputData, xc, yc, zc, m_PatchSize, dcDims[0], dcDims[1], dcDims[2]);
        }
        else if (dType.compare("double") == 0)
        {
          m_OutputArray[zStride+yStride+i] = quiltData<double>(inputData, xc, yc, zc, m_PatchSize, dcDims[0], dcDims[1], dcDims[2]);
        }
        else if (dType.compare("bool") == 0)
        {
          m_OutputArray[zStride+yStride+i] = quiltData<bool>(inputData, xc, yc, zc, m_PatchSize, dcDims[0], dcDims[1], dcDims[2]);
        }
      }
    }
  }

  notifyStatusMessage(getHumanLabel(), "Complete");
}


