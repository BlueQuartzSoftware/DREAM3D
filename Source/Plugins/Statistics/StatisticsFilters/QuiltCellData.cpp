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

#include <memory>

#include "QuiltCellData.h"

#include <QtCore/QTextStream>

#include "SIMPLib/Common/Constants.h"

#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/DataContainerCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/IntVec3FilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedBooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/Geometry/ImageGeom.h"
#include "SIMPLib/Math/SIMPLibMath.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/DataContainers/DataContainer.h"

enum createdPathID : RenameDataPath::DataID_t
{
  AttributeMatrixID21 = 21,

  DataArrayID31 = 31,

  DataContainerID = 1
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QuiltCellData::QuiltCellData()
: m_SelectedCellArrayPath("", "", "")
, m_OutputDataContainerName(SIMPL::Defaults::NewImageDataContainerName)
, m_OutputAttributeMatrixName(SIMPL::Defaults::CellAttributeMatrixName)
, m_OutputArrayName("Quilt_Data")
{
  m_QuiltStep[0] = 2;
  m_QuiltStep[1] = 2;
  m_QuiltStep[2] = 2;

  m_PatchSize[0] = 3;
  m_PatchSize[1] = 3;
  m_PatchSize[2] = 3;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QuiltCellData::~QuiltCellData() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QuiltCellData::setupFilterParameters()
{
  FilterParameterVectorType parameters;

  parameters.push_back(SIMPL_NEW_INT_VEC3_FP("Quilt Step (Voxels)", QuiltStep, FilterParameter::Parameter, QuiltCellData));
  parameters.push_back(SIMPL_NEW_INT_VEC3_FP("Patch Size (Voxels)", PatchSize, FilterParameter::Parameter, QuiltCellData));

  {
    DataArraySelectionFilterParameter::RequirementType req;
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Cell Array To Quilt", SelectedCellArrayPath, FilterParameter::RequiredArray, QuiltCellData, req));
  }

  parameters.push_back(SIMPL_NEW_DC_CREATION_FP("Output DataContainer Name", OutputDataContainerName, FilterParameter::CreatedArray, QuiltCellData));
  parameters.push_back(SIMPL_NEW_STRING_FP("Output AttributeMatrix Name", OutputAttributeMatrixName, FilterParameter::CreatedArray, QuiltCellData));
  parameters.push_back(SIMPL_NEW_STRING_FP("Output Data Array Name", OutputArrayName, FilterParameter::CreatedArray, QuiltCellData));

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QuiltCellData::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setSelectedCellArrayPath(reader->readDataArrayPath("SelectedCellArrayPath", getSelectedCellArrayPath()));
  setOutputDataContainerName(reader->readDataArrayPath("OutputDataContainerName", getOutputDataContainerName()));
  setOutputAttributeMatrixName(reader->readString("OutputAttributeMatrixName", getOutputAttributeMatrixName()));
  setOutputArrayName(reader->readString("OutputArrayName", getOutputArrayName()));
  setQuiltStep(reader->readIntVec3("QuiltStep", getQuiltStep()));
  setPatchSize(reader->readIntVec3("PatchSize", getPatchSize()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QuiltCellData::initialize()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QuiltCellData::dataCheck()
{
  DataArrayPath tempPath;
  clearErrorCode();
  clearWarningCode();

  // First sanity check the inputs and output names. All must be filled in

  if(m_SelectedCellArrayPath.isEmpty())
  {
    QString ss = QObject::tr("The input array name is empty. Please select a name for the input array");
    setErrorCondition(-11000, ss);
    return;
  }

  if(getOutputDataContainerName().isEmpty())
  {
    QString ss = QObject::tr("The output DataContainer name is empty. Please assign a name for the created DataContainer");
    setErrorCondition(-11001, ss);
    return;
  }

  if(getOutputAttributeMatrixName().isEmpty())
  {
    QString ss = QObject::tr("The output AttributeMatrix name is empty. Please assign a name for the created AttributeMatrix");
    setErrorCondition(-11002, ss);
    return;
  }

  if(getOutputArrayName().isEmpty())
  {
    QString ss = QObject::tr("The output array name is empty. Please assign a name for the created array");
    setErrorCondition(-11003, ss);
    return;
  }

  // Check to make sure the QuiltStep and Patch Size are non-zero
  if(m_QuiltStep[0] < 1 || m_QuiltStep[1] < 1 || m_QuiltStep[2] < 1)
  {
    QString ss = QObject::tr("The QuiltStep parameter is invalid because one of the values is Negative or Zero. Value=(%1, %2, %3)").arg(m_QuiltStep[0]).arg(m_QuiltStep[1]).arg(m_QuiltStep[2]);
    setErrorCondition(-11004, ss);
    return;
  }
  // Check to make sure the QuiltStep and Patch Size are non-zero
  if(m_PatchSize[0] < 1 || m_PatchSize[1] < 1 || m_PatchSize[2] < 1)
  {
    QString ss = QObject::tr("The Patch Size parameter is invalid because one of the values is Negative or Zero. Value=(%1, %2, %3)").arg(m_PatchSize[0]).arg(m_PatchSize[1]).arg(m_PatchSize[2]);
    setErrorCondition(-11005, ss);
    return;
  }

  // Next check the existing DataContainer/AttributeMatrix
  DataContainer::Pointer m = getDataContainerArray()->getPrereqDataContainer(this, m_SelectedCellArrayPath.getDataContainerName());
  if(getErrorCode() < 0)
  {
    return;
  }

  ImageGeom::Pointer image = m->getPrereqGeometry<ImageGeom, AbstractFilter>(this);
  if(getErrorCode() < 0 || nullptr == image.get())
  {
    return;
  }

  // Establish the dimensions, resolutions and origin of the new data container
  SizeVec3Type dcDims = m->getGeometryAs<ImageGeom>()->getDimensions();
  FloatVec3Type res = m->getGeometryAs<ImageGeom>()->getSpacing();
  // Create a new DataContainer
  DataContainer::Pointer m2 = getDataContainerArray()->createNonPrereqDataContainer<AbstractFilter>(this, getOutputDataContainerName(), DataContainerID);
  if(getErrorCode() < 0)
  {
    return;
  }

  ImageGeom::Pointer newImage = ImageGeom::CreateGeometry(SIMPL::Geometry::ImageGeometry);
  m2->setGeometry(newImage);

  int newDimX = int(dcDims[0] / m_QuiltStep[0]);
  int newDimY = int(dcDims[1] / m_QuiltStep[1]);
  int newDimZ = int(dcDims[2] / m_QuiltStep[2]);
  if(dcDims[0] == 1)
  {
    newDimX = 1;
  }
  if(dcDims[1] == 1)
  {
    newDimY = 1;
  }
  if(dcDims[2] == 1)
  {
    newDimZ = 1;
  }

  m2->getGeometryAs<ImageGeom>()->setDimensions(newDimX, newDimY, newDimZ);
  m2->getGeometryAs<ImageGeom>()->setSpacing(res);
  m2->getGeometryAs<ImageGeom>()->setOrigin(FloatVec3Type(0.0f, 0.0f, 0.0f));

  // Create the cell attrMat in the new data container
  std::vector<size_t> tDims(3, 0);
  tDims[0] = m2->getGeometryAs<ImageGeom>()->getXPoints();
  tDims[1] = m2->getGeometryAs<ImageGeom>()->getYPoints();
  tDims[2] = m2->getGeometryAs<ImageGeom>()->getZPoints();

  AttributeMatrix::Pointer newCellAttrMat = m2->createNonPrereqAttributeMatrix(this, getOutputAttributeMatrixName(), tDims, AttributeMatrix::Type::Cell, AttributeMatrixID21);
  if(getErrorCode() < 0 || nullptr == newCellAttrMat.get())
  {
    return;
  }

  // Get the name and create the array in the new data attrMat
  std::vector<size_t> dims(1, 1);
  tempPath.update(getOutputDataContainerName().getDataContainerName(), getOutputAttributeMatrixName(), getOutputArrayName());
  m_OutputArrayPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this, tempPath, 0, dims, "", DataArrayID31);
  if(nullptr != m_OutputArrayPtr.lock()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_OutputArray = m_OutputArrayPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
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
template <typename T> float quiltData(IDataArray::Pointer inputData, int64_t xc, int64_t yc, int64_t zc, IntVec3Type pSize, int64_t xDim, int64_t yDim, int64_t zDim)
{
  float value = 0.0;
  typename DataArray<T>::Pointer cellArray = std::dynamic_pointer_cast<DataArray<T>>(inputData);
  if(nullptr == cellArray)
  {
    return 0.0;
  }

  T* cPtr = cellArray->getPointer(0);

  int64_t zStride = 0, yStride = 0;
  float count = 0;

  int xRangeMin = -floorf((float)pSize[0] / 2.0f);
  int xRangeMax = floorf((float)pSize[0] / 2.0f);
  int yRangeMin = -floorf((float)pSize[1] / 2.0f);
  int yRangeMax = floorf((float)pSize[1] / 2.0f);
  int zRangeMin = -floorf((float)pSize[2] / 2.0f);
  int zRangeMax = floorf((float)pSize[2] / 2.0f);

  if(pSize[0] == 1)
  {
    xRangeMin = 0;
    xRangeMax = 1;
  }

  if(pSize[1] == 1)
  {
    yRangeMin = 0;
    yRangeMax = 1;
  }

  if(pSize[2] == 1)
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
  if(count == 0)
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
  clearErrorCode();
  clearWarningCode();
  dataCheck();
  if(getErrorCode() < 0)
  {
    return;
  }

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(m_SelectedCellArrayPath.getDataContainerName());
  DataContainer::Pointer m2 = getDataContainerArray()->getDataContainer(getOutputDataContainerName());

  SizeVec3Type dcDims = m->getGeometryAs<ImageGeom>()->getDimensions();
  SizeVec3Type dc2Dims = m2->getGeometryAs<ImageGeom>()->getDimensions();

  IDataArray::Pointer inputData = m->getAttributeMatrix(m_SelectedCellArrayPath.getAttributeMatrixName())->getAttributeArray(m_SelectedCellArrayPath.getDataArrayName());
  if(nullptr == inputData.get())
  {
    ss = QObject::tr("Selected array '%1' does not exist in the Voxel Data Container. Was it spelled correctly?").arg(m_SelectedCellArrayPath.getDataArrayName());
    setErrorCondition(-11001, ss);
    return;
  }

  QString dType = inputData->getTypeAsString();

  int64_t dims[3] = {0, 0, 0};
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
        xc = i * m_QuiltStep[0] + m_QuiltStep[0] / 2;
        yc = j * m_QuiltStep[1] + m_QuiltStep[1] / 2;
        // zc = k * m_QuiltStep[2] + m_QuiltStep[2] / 2;
        zc = 0;
        if(dType.compare("int8_t") == 0)
        {
          m_OutputArray[zStride + yStride + i] = quiltData<int8_t>(inputData, xc, yc, zc, m_PatchSize, dims[0], dims[1], dims[2]);
        }
        else if(dType.compare("uint8_t") == 0)
        {
          m_OutputArray[zStride + yStride + i] = quiltData<uint8_t>(inputData, xc, yc, zc, m_PatchSize, dims[0], dims[1], dims[2]);
        }
        else if(dType.compare("int16_t") == 0)
        {
          m_OutputArray[zStride + yStride + i] = quiltData<int16_t>(inputData, xc, yc, zc, m_PatchSize, dims[0], dims[1], dims[2]);
        }
        else if(dType.compare("uint16_t") == 0)
        {
          m_OutputArray[zStride + yStride + i] = quiltData<uint16_t>(inputData, xc, yc, zc, m_PatchSize, dims[0], dims[1], dims[2]);
        }
        else if(dType.compare("int32_t") == 0)
        {
          m_OutputArray[zStride + yStride + i] = quiltData<int32_t>(inputData, xc, yc, zc, m_PatchSize, dims[0], dims[1], dims[2]);
        }
        else if(dType.compare("uint32_t") == 0)
        {
          m_OutputArray[zStride + yStride + i] = quiltData<uint32_t>(inputData, xc, yc, zc, m_PatchSize, dims[0], dims[1], dims[2]);
        }
        else if(dType.compare("int64_t") == 0)
        {
          m_OutputArray[zStride + yStride + i] = quiltData<int64_t>(inputData, xc, yc, zc, m_PatchSize, dims[0], dims[1], dims[2]);
        }
        else if(dType.compare("uint64_t") == 0)
        {
          m_OutputArray[zStride + yStride + i] = quiltData<uint64_t>(inputData, xc, yc, zc, m_PatchSize, dims[0], dims[1], dims[2]);
        }
        else if(dType.compare("float") == 0)
        {
          m_OutputArray[zStride + yStride + i] = quiltData<float>(inputData, xc, yc, zc, m_PatchSize, dims[0], dims[1], dims[2]);
        }
        else if(dType.compare("double") == 0)
        {
          m_OutputArray[zStride + yStride + i] = quiltData<double>(inputData, xc, yc, zc, m_PatchSize, dims[0], dims[1], dims[2]);
        }
        else if(dType.compare("bool") == 0)
        {
          m_OutputArray[zStride + yStride + i] = quiltData<bool>(inputData, xc, yc, zc, m_PatchSize, dims[0], dims[1], dims[2]);
        }
      }
    }
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer QuiltCellData::newFilterInstance(bool copyFilterParameters) const
{
  QuiltCellData::Pointer filter = QuiltCellData::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString QuiltCellData::getCompiledLibraryName() const
{
  return StatisticsConstants::StatisticsBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString QuiltCellData::getBrandingString() const
{
  return "Statistics";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString QuiltCellData::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << Statistics::Version::Major() << "." << Statistics::Version::Minor() << "." << Statistics::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString QuiltCellData::getGroupName() const
{
  return SIMPL::FilterGroups::StatisticsFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid QuiltCellData::getUuid() const
{
  return QUuid("{bddc673c-d3fa-5e7d-8fb6-5e234dfa9bc5}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString QuiltCellData::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::EnsembleStatsFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString QuiltCellData::getHumanLabel() const
{
  return "Quilt Cell Data";
}

// -----------------------------------------------------------------------------
QuiltCellData::Pointer QuiltCellData::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<QuiltCellData> QuiltCellData::New()
{
  struct make_shared_enabler : public QuiltCellData
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString QuiltCellData::getNameOfClass() const
{
  return QString("QuiltCellData");
}

// -----------------------------------------------------------------------------
QString QuiltCellData::ClassName()
{
  return QString("QuiltCellData");
}

// -----------------------------------------------------------------------------
void QuiltCellData::setSelectedCellArrayPath(const DataArrayPath& value)
{
  m_SelectedCellArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath QuiltCellData::getSelectedCellArrayPath() const
{
  return m_SelectedCellArrayPath;
}

// -----------------------------------------------------------------------------
void QuiltCellData::setQuiltStep(const IntVec3Type& value)
{
  m_QuiltStep = value;
}

// -----------------------------------------------------------------------------
IntVec3Type QuiltCellData::getQuiltStep() const
{
  return m_QuiltStep;
}

// -----------------------------------------------------------------------------
void QuiltCellData::setPatchSize(const IntVec3Type& value)
{
  m_PatchSize = value;
}

// -----------------------------------------------------------------------------
IntVec3Type QuiltCellData::getPatchSize() const
{
  return m_PatchSize;
}

// -----------------------------------------------------------------------------
void QuiltCellData::setOutputDataContainerName(const DataArrayPath& value)
{
  m_OutputDataContainerName = value;
}

// -----------------------------------------------------------------------------
DataArrayPath QuiltCellData::getOutputDataContainerName() const
{
  return m_OutputDataContainerName;
}

// -----------------------------------------------------------------------------
void QuiltCellData::setOutputAttributeMatrixName(const QString& value)
{
  m_OutputAttributeMatrixName = value;
}

// -----------------------------------------------------------------------------
QString QuiltCellData::getOutputAttributeMatrixName() const
{
  return m_OutputAttributeMatrixName;
}

// -----------------------------------------------------------------------------
void QuiltCellData::setOutputArrayName(const QString& value)
{
  m_OutputArrayName = value;
}

// -----------------------------------------------------------------------------
QString QuiltCellData::getOutputArrayName() const
{
  return m_OutputArrayName;
}
