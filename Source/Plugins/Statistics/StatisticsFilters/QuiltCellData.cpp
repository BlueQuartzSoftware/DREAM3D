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
  m_SelectedCellArrayName(""),
  m_NewCellArrayArrayName(""),
  m_NewCellArray(NULL)
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
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("Cell Array To Quilt");
    parameter->setPropertyName("SelectedFeatureArrayName");
    parameter->setWidgetType(FilterParameterWidgetType::VolumeCellArrayNameSelectionWidget);
    parameter->setValueType("QString");
    parameter->setUnits("");
    parameters.push_back(parameter);
  }
  {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("Quilt Step X (Voxels)");
    parameter->setPropertyName("QStepX");
    parameter->setWidgetType(FilterParameterWidgetType::IntWidget);
    parameter->setValueType("int");
    parameters.push_back(parameter);
  }
  {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("Quilt Step Y (Voxels)");
    parameter->setPropertyName("QStepY");
    parameter->setWidgetType(FilterParameterWidgetType::IntWidget);
    parameter->setValueType("int");
    parameters.push_back(parameter);
  }
  {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("Quilt Step Z (Voxels)");
    parameter->setPropertyName("QStepZ");
    parameter->setWidgetType(FilterParameterWidgetType::IntWidget);
    parameter->setValueType("int");
    parameters.push_back(parameter);
  }
  {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("Patch Size X (Voxels)");
    parameter->setPropertyName("PSizeX");
    parameter->setWidgetType(FilterParameterWidgetType::IntWidget);
    parameter->setValueType("int");
    parameters.push_back(parameter);
  }
  {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("Patch Size Y (Voxels)");
    parameter->setPropertyName("PSizeY");
    parameter->setWidgetType(FilterParameterWidgetType::IntWidget);
    parameter->setValueType("int");
    parameters.push_back(parameter);
  }
  {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("Patch Size Z (Voxels)");
    parameter->setPropertyName("PSizeZ");
    parameter->setWidgetType(FilterParameterWidgetType::IntWidget);
    parameter->setValueType("int");
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
  setQStepX( reader->readValue("QStepX", getQStepX()));
  setQStepY( reader->readValue("QStepY", getQStepY()));
  setQStepZ( reader->readValue("QStepZ", getQStepZ()));
  setPSizeX( reader->readValue("PSizeX", getPSizeX()));
  setPSizeY( reader->readValue("PSizeY", getPSizeY()));
  setPSizeZ( reader->readValue("PSizeZ", getPSizeZ()));
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
  writer->writeValue("QStepX", getQStepX());
  writer->writeValue("QStepY", getQStepY());
  writer->writeValue("QStepZ", getQStepZ());
  writer->writeValue("PSizeX", getPSizeX());
  writer->writeValue("PSizeY", getPSizeY());
  writer->writeValue("PSizeZ", getPSizeZ());
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
  if(getErrorCondition() < 0 || NULL == cellAttrMat.get() ) { return; }

  //Establish the dimensions, resolutions and origin of the new data container
  size_t dcDims[3];
  m->getDimensions(dcDims[0], dcDims[1], dcDims[2]);
  m2->setDimensions(int(dcDims[0]/m_QStepX), int(dcDims[1]/m_QStepY), int(dcDims[2]/m_QStepZ));
  float res[3] = {m_QStepX*m->getXRes(), m_QStepY*m->getYRes(), m_QStepZ*m->getZRes()};
  m2->setResolution(res);
  m2->setOrigin(0.0f, 0.0f, 0.0f);

  //Create the cell attrMat in the new data container
  QVector<size_t> tDims(3, 0);
  tDims[0] = m2->getXPoints();
  tDims[1] = m2->getYPoints();
  tDims[2] = m2->getZPoints();
  AttributeMatrix::Pointer newCellAttrMat = m2->createNonPrereqAttributeMatrix<AbstractFilter>(this, getNewCellAttributeMatrixName(), tDims, DREAM3D::AttributeMatrixType::Cell);
  if(getErrorCondition() < 0) { return; }

  if(m_SelectedCellArrayName.isEmpty() == true)
  {
    setErrorCondition(-11000);
    notifyErrorMessage(getHumanLabel(), "An array from the Volume DataContainer must be selected.", getErrorCondition());
  }

  //Get the name and create the array in the new data attrMat
  m_NewCellArrayArrayName = m_SelectedCellArrayName + QString("Quilt");
  QVector<size_t> dims(1, 1);
  m_NewCellArrayPtr = newCellAttrMat->createNonPrereqArray<DataArray<float>, AbstractFilter, float>(this, m_NewCellArrayArrayName, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_NewCellArrayPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_NewCellArray = m_NewCellArrayPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
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
float quiltData(IDataArray::Pointer inputData, int xc, int yc, int zc, int pSizeX, int pSizeY, int pSizeZ, int xDim, int yDim, int zDim)
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
  for(int k = -(pSizeZ/2); k < (pSizeZ/2); k++)
  {
    if((zc+k) >= 0 && (zc+k) < zDim)
    {
      zStride = ((zc+k)*xDim*yDim);
      for(int j = -(pSizeY/2); j < (pSizeY/2); j++)
      {
        if((yc+j) >= 0 && (yc+j) < yDim)
        {
          yStride = ((yc+j)*xDim);
          for(int i = -(pSizeX/2); i < (pSizeX/2); i++)
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
  VolumeDataContainer* m2 = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getNewDataContainerName());

  size_t dcDims[3];
  m->getDimensions(dcDims[0], dcDims[1], dcDims[2]);
  int newXP = int(dcDims[0]/m_QStepX);
  int newYP = int(dcDims[1]/m_QStepY);
  int newZP = int(dcDims[2]/m_QStepZ);

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
        xc = i*m_QStepX + m_QStepX/2;
        yc = j*m_QStepY + m_QStepY/2;
        zc = k*m_QStepZ + m_QStepZ/2;
        if (dType.compare("int8_t") == 0)
        {
          m_NewCellArray[zStride+yStride+i] = quiltData<int8_t>(inputData, xc, yc, zc, m_PSizeX, m_PSizeY, m_PSizeZ, dcDims[0], dcDims[1], dcDims[2]);
        }
        else if (dType.compare("uint8_t") == 0)
        {
          m_NewCellArray[zStride+yStride+i] = quiltData<uint8_t>(inputData, xc, yc, zc, m_PSizeX, m_PSizeY, m_PSizeZ, dcDims[0], dcDims[1], dcDims[2]);
        }
        else if (dType.compare("int16_t") == 0)
        {
          m_NewCellArray[zStride+yStride+i] = quiltData<int16_t>(inputData, xc, yc, zc, m_PSizeX, m_PSizeY, m_PSizeZ, dcDims[0], dcDims[1], dcDims[2]);
        }
        else if (dType.compare("uint16_t") == 0)
        {
          m_NewCellArray[zStride+yStride+i] = quiltData<uint16_t>(inputData, xc, yc, zc, m_PSizeX, m_PSizeY, m_PSizeZ, dcDims[0], dcDims[1], dcDims[2]);
        }
        else if (dType.compare("int32_t") == 0)
        {
          m_NewCellArray[zStride+yStride+i] = quiltData<int32_t>(inputData, xc, yc, zc, m_PSizeX, m_PSizeY, m_PSizeZ, dcDims[0], dcDims[1], dcDims[2]);
        }
        else if (dType.compare("uint32_t") == 0)
        {
          m_NewCellArray[zStride+yStride+i] = quiltData<uint32_t>(inputData, xc, yc, zc, m_PSizeX, m_PSizeY, m_PSizeZ, dcDims[0], dcDims[1], dcDims[2]);
        }
        else if (dType.compare("int64_t") == 0)
        {
          m_NewCellArray[zStride+yStride+i] = quiltData<int64_t>(inputData, xc, yc, zc, m_PSizeX, m_PSizeY, m_PSizeZ, dcDims[0], dcDims[1], dcDims[2]);
        }
        else if (dType.compare("uint64_t") == 0)
        {
          m_NewCellArray[zStride+yStride+i] = quiltData<uint64_t>(inputData, xc, yc, zc, m_PSizeX, m_PSizeY, m_PSizeZ, dcDims[0], dcDims[1], dcDims[2]);
        }
        else if (dType.compare("float") == 0)
        {
          m_NewCellArray[zStride+yStride+i] = quiltData<float>(inputData, xc, yc, zc, m_PSizeX, m_PSizeY, m_PSizeZ, dcDims[0], dcDims[1], dcDims[2]);
        }
        else if (dType.compare("double") == 0)
        {
          m_NewCellArray[zStride+yStride+i] = quiltData<double>(inputData, xc, yc, zc, m_PSizeX, m_PSizeY, m_PSizeZ, dcDims[0], dcDims[1], dcDims[2]);
        }
        else if (dType.compare("bool") == 0)
        {
          m_NewCellArray[zStride+yStride+i] = quiltData<bool>(inputData, xc, yc, zc, m_PSizeX, m_PSizeY, m_PSizeZ, dcDims[0], dcDims[1], dcDims[2]);
        }
      }
    }
  }

  notifyStatusMessage(getHumanLabel(), "Complete");
}


