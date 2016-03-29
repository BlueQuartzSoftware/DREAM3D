/* ============================================================================
* Copyright (c) 2009-2016 BlueQuartz Software, LLC
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

#include "QuiltCellData.h"

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/SIMPLibVersion.h"
#include "SIMPLib/Math/SIMPLibMath.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersWriter.h"
#include "SIMPLib/FilterParameters/IntVec3FilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedBooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/Geometry/ImageGeom.h"

// Include the MOC generated file for this class
#include "moc_QuiltCellData.cpp"



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QuiltCellData::QuiltCellData() :
  AbstractFilter(),
  m_SelectedCellArrayPath("", "", ""),
  m_OutputDataContainerName(SIMPL::Defaults::NewImageDataContainerName),
  m_OutputAttributeMatrixName(SIMPL::Defaults::CellAttributeMatrixName),
  m_OutputArrayName("Quilt_Data"),
  m_OutputArray(NULL)
{
  m_QuiltStep.x = 2;
  m_QuiltStep.y = 2;
  m_QuiltStep.z = 2;

  m_PatchSize.x = 3;
  m_PatchSize.y = 3;
  m_PatchSize.z = 3;

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

  parameters.push_back(IntVec3FilterParameter::New("Quilt Step (Voxels)", "QuiltStep", getQuiltStep(), FilterParameter::Parameter));
  parameters.push_back(IntVec3FilterParameter::New("Patch Size (Voxels)", "PatchSize", getPatchSize(), FilterParameter::Parameter));

  {
    DataArraySelectionFilterParameter::RequirementType req;
    parameters.push_back(DataArraySelectionFilterParameter::New("Cell Array To Quilt", "SelectedCellArrayPath", getSelectedCellArrayPath(), FilterParameter::RequiredArray, req));
  }

  parameters.push_back(StringFilterParameter::New("Output DataContainer Name", "OutputDataContainerName", getOutputDataContainerName(), FilterParameter::CreatedArray));
  parameters.push_back(StringFilterParameter::New("Output AttributeMatrix Name", "OutputAttributeMatrixName", getOutputAttributeMatrixName(), FilterParameter::CreatedArray));
  parameters.push_back(StringFilterParameter::New("Output Data Array Name", "OutputArrayName", getOutputArrayName(), FilterParameter::CreatedArray));

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QuiltCellData::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setSelectedCellArrayPath( reader->readDataArrayPath( "SelectedCellArrayPath", getSelectedCellArrayPath() ) );
  setOutputDataContainerName( reader->readString( "OutputDataContainerName", getOutputDataContainerName() ) );
  setOutputAttributeMatrixName( reader->readString( "OutputAttributeMatrixName", getOutputAttributeMatrixName() ) );
  setOutputArrayName( reader->readString( "OutputArrayName", getOutputArrayName() ) );
  setQuiltStep( reader->readIntVec3("QuiltStep", getQuiltStep() ) );
  setPatchSize( reader->readIntVec3("PatchSize", getPatchSize() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int QuiltCellData::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  SIMPL_FILTER_WRITE_PARAMETER(FilterVersion)
  SIMPL_FILTER_WRITE_PARAMETER(SelectedCellArrayPath)
  SIMPL_FILTER_WRITE_PARAMETER(OutputDataContainerName)
  SIMPL_FILTER_WRITE_PARAMETER(OutputAttributeMatrixName)
  SIMPL_FILTER_WRITE_PARAMETER(OutputArrayName)
  SIMPL_FILTER_WRITE_PARAMETER(QuiltStep)
  SIMPL_FILTER_WRITE_PARAMETER(PatchSize)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QuiltCellData::dataCheck()
{
  DataArrayPath tempPath;
  setErrorCondition(0);

  // First sanity check the inputs and output names. All must be filled in

  if(m_SelectedCellArrayPath.isEmpty() == true)
  {
    QString ss = QObject::tr("The input array name is empty. Please select a name for the input array");
    setErrorCondition(-11000);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  if(getOutputDataContainerName().isEmpty() == true)
  {
    QString ss = QObject::tr("The output DataContainer name is empty. Please assign a name for the created DataContainer");
    setErrorCondition(-11001);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  if(getOutputAttributeMatrixName().isEmpty() == true)
  {
    QString ss = QObject::tr("The output AttributeMatrix name is empty. Please assign a name for the created AttributeMatrix");
    setErrorCondition(-11002);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  if(getOutputArrayName().isEmpty() == true)
  {
    QString ss = QObject::tr("The output array name is empty. Please assign a name for the created array");
    setErrorCondition(-11003);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  // Check to make sure the QuiltStep and Patch Size are non-zero
  if(m_QuiltStep.x < 1 || m_QuiltStep.y < 1 || m_QuiltStep.z < 1)
  {
    QString ss = QObject::tr("The QuiltStep parameter is invalid because one of the values is Negative or Zero. Value=(%1, %2, %3)").arg(m_QuiltStep.x).arg(m_QuiltStep.y).arg(m_QuiltStep.z);
    setErrorCondition(-11004);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }
  // Check to make sure the QuiltStep and Patch Size are non-zero
  if(m_PatchSize.x < 1 || m_PatchSize.y < 1 || m_PatchSize.z < 1)
  {
    QString ss = QObject::tr("The Patch Size parameter is invalid because one of the values is Negative or Zero. Value=(%1, %2, %3)").arg(m_PatchSize.x).arg(m_PatchSize.y).arg(m_PatchSize.z);
    setErrorCondition(-11005);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  // Next check the existing DataContainer/AttributeMatrix
  DataContainer::Pointer m = getDataContainerArray()->getPrereqDataContainer<AbstractFilter>(this, m_SelectedCellArrayPath.getDataContainerName());
  if(getErrorCondition() < 0) { return; }

  ImageGeom::Pointer image = m->getPrereqGeometry<ImageGeom, AbstractFilter>(this);
  if(getErrorCondition() < 0 || NULL == image.get()) { return; }

  //Establish the dimensions, resolutions and origin of the new data container
  size_t dcDims[3] = { 0, 0, 0};
  m->getGeometryAs<ImageGeom>()->getDimensions(dcDims[0], dcDims[1], dcDims[2]);
  float res[3] = {m_QuiltStep.x* m->getGeometryAs<ImageGeom>()->getXRes(), m_QuiltStep.y* m->getGeometryAs<ImageGeom>()->getYRes(), m_QuiltStep.z* m->getGeometryAs<ImageGeom>()->getZRes()};

  // Create a new DataContainer
  DataContainer::Pointer m2 = getDataContainerArray()->createNonPrereqDataContainer<AbstractFilter>(this, getOutputDataContainerName());
  if(getErrorCondition() < 0) { return; }

  ImageGeom::Pointer newImage = ImageGeom::CreateGeometry(SIMPL::Geometry::ImageGeometry);
  m2->setGeometry(newImage);

  int newDimX = int(dcDims[0] / m_QuiltStep.x);
  int newDimY = int(dcDims[1] / m_QuiltStep.y);
  int newDimZ = int(dcDims[2] / m_QuiltStep.z);
  if(dcDims[0] == 1) { newDimX = 1; }
  if(dcDims[1] == 1) { newDimY = 1; }
  if(dcDims[2] == 1) { newDimZ = 1; }

  m2->getGeometryAs<ImageGeom>()->setDimensions(newDimX, newDimY, newDimZ);
  m2->getGeometryAs<ImageGeom>()->setResolution(res);
  m2->getGeometryAs<ImageGeom>()->setOrigin(0.0f, 0.0f, 0.0f);

  //Create the cell attrMat in the new data container
  QVector<size_t> tDims(3, 0);
  tDims[0] = m2->getGeometryAs<ImageGeom>()->getXPoints();
  tDims[1] = m2->getGeometryAs<ImageGeom>()->getYPoints();
  tDims[2] = m2->getGeometryAs<ImageGeom>()->getZPoints();
  AttributeMatrix::Pointer newCellAttrMat = m2->createNonPrereqAttributeMatrix<AbstractFilter>(this, getOutputAttributeMatrixName(), tDims, SIMPL::AttributeMatrixType::Cell);
  if(getErrorCondition() < 0 || NULL == newCellAttrMat.get()) { return; }

  //Get the name and create the array in the new data attrMat
  QVector<size_t> dims(1, 1);
  tempPath.update(getOutputDataContainerName(), getOutputAttributeMatrixName(), getOutputArrayName() );
  m_OutputArrayPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this, tempPath, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_OutputArrayPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_OutputArray = m_OutputArrayPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QuiltCellData::preflight()
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
float quiltData(IDataArray::Pointer inputData, int64_t xc, int64_t yc, int64_t zc, IntVec3_t pSize, int64_t xDim, int64_t yDim, int64_t zDim)
{
  float value = 0.0;
  DataArray<T>* cellArray = DataArray<T>::SafePointerDownCast(inputData.get());
  if (NULL == cellArray)
  {
    return 0.0;
  }

  T* cPtr = cellArray->getPointer(0);

  int64_t zStride = 0, yStride = 0;
  float count = 0;

  int xRangeMin = -floorf((float)pSize.x / 2.0f);
  int xRangeMax = floorf((float)pSize.x / 2.0f);
  int yRangeMin = -floorf((float)pSize.y / 2.0f);
  int yRangeMax = floorf((float)pSize.y / 2.0f);
  int zRangeMin = -floorf((float)pSize.z / 2.0f);
  int zRangeMax = floorf((float)pSize.z / 2.0f);

  if (pSize.x == 1)
  {
    xRangeMin = 0;
    xRangeMax = 1;
  }

  if (pSize.y == 1)
  {
    yRangeMin = 0;
    yRangeMax = 1;
  }


  if (pSize.z == 1)
  {
    zRangeMin = 0;
    zRangeMax = 1;
  }

  for(int64_t k = zRangeMin; k < zRangeMax; k++)
  {
    if((zc + k) >= 0 && (zc + k) < zDim)
    {
      zStride = ((zc + k) * xDim * yDim);
      for(int64_t j = yRangeMin; j < yRangeMax; j++)
      {
        if((yc + j) >= 0 && (yc + j) < yDim)
        {
          yStride = ((yc + j) * xDim);
          for(int64_t i = xRangeMin; i < xRangeMax; i++)
          {
            if((xc + i) >= 0 && (xc + i) < xDim)
            {
              value += cPtr[zStride + yStride + (xc + i)];
              count++;
            }
          }
        }
      }
    }
  }
  if (count == 0)
  {
    value = 0;
  }
  else
  {
    value /= count;
  }
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

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(m_SelectedCellArrayPath.getDataContainerName());
  DataContainer::Pointer m2 = getDataContainerArray()->getDataContainer(getOutputDataContainerName());

  size_t dcDims[3];
  m->getGeometryAs<ImageGeom>()->getDimensions(dcDims[0], dcDims[1], dcDims[2]);
  size_t dc2Dims[3];
  m2->getGeometryAs<ImageGeom>()->getDimensions(dc2Dims[0], dc2Dims[1], dc2Dims[2]);

  IDataArray::Pointer inputData = m->getAttributeMatrix(m_SelectedCellArrayPath.getAttributeMatrixName())->getAttributeArray(m_SelectedCellArrayPath.getDataArrayName());
  if (NULL == inputData.get())
  {
    ss = QObject::tr("Selected array '%1' does not exist in the Voxel Data Container. Was it spelled correctly?").arg(m_SelectedCellArrayPath.getDataArrayName());
    setErrorCondition(-11001);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  QString dType = inputData->getTypeAsString();

  int64_t dims[3] = { 0, 0, 0 };
  dims[0] = static_cast<int64_t>(dcDims[0]);
  dims[1] = static_cast<int64_t>(dcDims[1]);
  dims[2] = static_cast<int64_t>(dcDims[2]);

  int64_t zStride = 0, yStride = 0;
  int64_t xc = 0, yc = 0, zc = 0;
  for(size_t k = 0; k < dc2Dims[2]; k++)
  {
    zStride = (k * dc2Dims[0] * dc2Dims[1]);
    for(size_t j = 0; j < dc2Dims[1]; j++)
    {
      yStride = (j * dc2Dims[0]);
      for(size_t i = 0; i < dc2Dims[0]; i++)
      {
        xc = i * m_QuiltStep.x + m_QuiltStep.x / 2;
        yc = j * m_QuiltStep.y + m_QuiltStep.y / 2;
        //zc = k * m_QuiltStep.z + m_QuiltStep.z / 2;
        zc = 0;
        if (dType.compare("int8_t") == 0)
        {
          m_OutputArray[zStride + yStride + i] = quiltData<int8_t>(inputData, xc, yc, zc, m_PatchSize, dims[0], dims[1], dims[2]);
        }
        else if (dType.compare("uint8_t") == 0)
        {
          m_OutputArray[zStride + yStride + i] = quiltData<uint8_t>(inputData, xc, yc, zc, m_PatchSize, dims[0], dims[1], dims[2]);
        }
        else if (dType.compare("int16_t") == 0)
        {
          m_OutputArray[zStride + yStride + i] = quiltData<int16_t>(inputData, xc, yc, zc, m_PatchSize, dims[0], dims[1], dims[2]);
        }
        else if (dType.compare("uint16_t") == 0)
        {
          m_OutputArray[zStride + yStride + i] = quiltData<uint16_t>(inputData, xc, yc, zc, m_PatchSize, dims[0], dims[1], dims[2]);
        }
        else if (dType.compare("int32_t") == 0)
        {
          m_OutputArray[zStride + yStride + i] = quiltData<int32_t>(inputData, xc, yc, zc, m_PatchSize, dims[0], dims[1], dims[2]);
        }
        else if (dType.compare("uint32_t") == 0)
        {
          m_OutputArray[zStride + yStride + i] = quiltData<uint32_t>(inputData, xc, yc, zc, m_PatchSize, dims[0], dims[1], dims[2]);
        }
        else if (dType.compare("int64_t") == 0)
        {
          m_OutputArray[zStride + yStride + i] = quiltData<int64_t>(inputData, xc, yc, zc, m_PatchSize, dims[0], dims[1], dims[2]);
        }
        else if (dType.compare("uint64_t") == 0)
        {
          m_OutputArray[zStride + yStride + i] = quiltData<uint64_t>(inputData, xc, yc, zc, m_PatchSize, dims[0], dims[1], dims[2]);
        }
        else if (dType.compare("float") == 0)
        {
          m_OutputArray[zStride + yStride + i] = quiltData<float>(inputData, xc, yc, zc, m_PatchSize, dims[0], dims[1], dims[2]);
        }
        else if (dType.compare("double") == 0)
        {
          m_OutputArray[zStride + yStride + i] = quiltData<double>(inputData, xc, yc, zc, m_PatchSize, dims[0], dims[1], dims[2]);
        }
        else if (dType.compare("bool") == 0)
        {
          m_OutputArray[zStride + yStride + i] = quiltData<bool>(inputData, xc, yc, zc, m_PatchSize, dims[0], dims[1], dims[2]);
        }
      }
    }
  }

  notifyStatusMessage(getHumanLabel(), "Complete");
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer QuiltCellData::newFilterInstance(bool copyFilterParameters)
{
  QuiltCellData::Pointer filter = QuiltCellData::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString QuiltCellData::getCompiledLibraryName()
{
  return StatisticsConstants::StatisticsBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString QuiltCellData::getBrandingString()
{
  return "Statistics";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString QuiltCellData::getFilterVersion()
{
  QString version;
  QTextStream vStream(&version);
  vStream <<  SIMPLib::Version::Major() << "." << SIMPLib::Version::Minor() << "." << SIMPLib::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString QuiltCellData::getGroupName()
{ return SIMPL::FilterGroups::StatisticsFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString QuiltCellData::getSubGroupName()
{ return SIMPL::FilterSubGroups::EnsembleStatsFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString QuiltCellData::getHumanLabel()
{ return "Quilt Cell Data"; }

