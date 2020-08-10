/* ============================================================================
 * Copyright (c) 2020 BlueQuartz Software, LLC
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
 *    United States Air Force Prime Contract FA8650-10-D-5210
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "ResampleRectGridToImageGeom.h"

#include <QtCore/QTextStream>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/DataContainers/DataContainer.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/FilterParameters/AttributeMatrixSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/ChoiceFilterParameter.h"
#include "SIMPLib/FilterParameters/DataContainerCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/DataContainerSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/FloatVec3FilterParameter.h"
#include "SIMPLib/FilterParameters/IntVec3FilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedPathCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/MultiDataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/PreflightUpdatedValueFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"

#include "Sampling/SamplingConstants.h"
#include "Sampling/SamplingVersion.h"

namespace
{

constexpr int32_t k_BadGeometry = -10101;
constexpr int32_t k_NotEnoughAttributeArrays = -11001;
constexpr int32_t k_AttributeMatrixOwnership = -11004;

constexpr int32_t k_Dim0Error = -5000;
constexpr int32_t k_Dim1Error = -5001;
constexpr int32_t k_Dim2Error = -5002;

constexpr float k_Half = 0.5F;

template <class Filter, typename T>
void checkPositiveDimension(Filter* filter, T var, int32_t err)
{
  if(var < 0)
  {
    QString ss = QObject::tr("%1 must be positive").arg(var);
    filter->setErrorCondition(err, ss);
  }
}
} // namespace

enum createdPathID : RenameDataPath::DataID_t
{
  AttributeMatrixID21 = 21,

  DataContainerID = 1
};
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ResampleRectGridToImageGeom::ResampleRectGridToImageGeom()
{
  initialize();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ResampleRectGridToImageGeom::~ResampleRectGridToImageGeom() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ResampleRectGridToImageGeom::initialize()
{
  clearErrorCode();
  clearWarningCode();
  setCancel(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ResampleRectGridToImageGeom::setupFilterParameters()
{
  FilterParameterVectorType parameters = getFilterParameters();

  {
    DataContainerSelectionFilterParameter::RequirementType req;
    parameters.push_back(SIMPL_NEW_DC_SELECTION_FP("Input Rectilinear Grid", RectilinearGridPath, FilterParameter::Parameter, ResampleRectGridToImageGeom, req));
  }
  {
    MultiDataArraySelectionFilterParameter::RequirementType req;
    parameters.push_back(SIMPL_NEW_MDA_SELECTION_FP("Attribute Arrays to Copy", SelectedDataArrayPaths, FilterParameter::RequiredArray, ResampleRectGridToImageGeom, req));
  }

  PreflightUpdatedValueFilterParameter::Pointer param = SIMPL_NEW_PREFLIGHTUPDATEDVALUE_FP("Rect Grid Geom Info.", RectGridGeometryDesc, FilterParameter::Parameter, ResampleRectGridToImageGeom);
  param->setReadOnly(true);
  parameters.push_back(param);

  parameters.push_back(SeparatorFilterParameter::New("Output Image Geometry Parameters", FilterParameter::CreatedArray));
  parameters.push_back(SIMPL_NEW_INT_VEC3_FP("Dimensions (Voxels)", Dimensions, FilterParameter::Parameter, ResampleRectGridToImageGeom));

  param = SIMPL_NEW_PREFLIGHTUPDATEDVALUE_FP("Created Volume Info.", CreatedGeometryDescription, FilterParameter::Parameter, ResampleRectGridToImageGeom);
  param->setReadOnly(true);
  parameters.push_back(param);

  parameters.push_back(SIMPL_NEW_DC_CREATION_FP("Data Container", ImageGeometryPath, FilterParameter::CreatedArray, ResampleRectGridToImageGeom));
  parameters.push_back(SeparatorFilterParameter::New("Cell Data", FilterParameter::CreatedArray));
  parameters.push_back(SIMPL_NEW_AM_WITH_LINKED_DC_FP("Cell Attribute Matrix", ImageGeomCellAttributeMatrix, ImageGeometryPath, FilterParameter::CreatedArray, ResampleRectGridToImageGeom));

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ResampleRectGridToImageGeom::dataCheck()
{
  clearErrorCode();
  clearWarningCode();
  DataArrayPath tempPath;

  DataContainer::Pointer inputDC = getDataContainerArray()->getPrereqDataContainer(this, getRectilinearGridPath(), false);
  if(getErrorCode() < 0)
  {
    return;
  }

  RectGridGeom::Pointer rectGrid = inputDC->getGeometryAs<RectGridGeom>();
  if(rectGrid == nullptr)
  {
    QString ss = QObject::tr("Input Geometry is NOT Rectilinear Grid or does not exist");
    setErrorCondition(::k_BadGeometry, ss);
    return;
  }
  m_InputRectGridGeometry = rectGrid;
  FloatArrayType::Pointer rgXCoords = m_InputRectGridGeometry->getXBounds();
  FloatArrayType::Pointer rgYCoords = m_InputRectGridGeometry->getYBounds();
  FloatArrayType::Pointer rgZCoords = m_InputRectGridGeometry->getZBounds();

  if(m_SelectedDataArrayPaths.isEmpty())
  {
    QString ss = QObject::tr("At least one Attribute Array must be selected");
    setErrorCondition(k_NotEnoughAttributeArrays, ss);
    return;
  }

  QVector<DataArrayPath> paths = getSelectedDataArrayPaths();
  if(!DataArrayPath::ValidateVector(paths))
  {
    QString ss = QObject::tr("There are Attribute Arrays selected that are not contained in the same Attribute Matrix. All selected Attribute Arrays must belong to the same Attribute Matrix");
    setErrorCondition(k_AttributeMatrixOwnership, ss);
    return;
  }

  for(const DataArrayPath& path : paths)
  {
    IDataArray::WeakPointer ptr = getDataContainerArray()->getPrereqIDataArrayFromPath(this, path);
    m_SelectedWeakPtrVector.push_back(ptr);
  }

  DataContainer::Pointer outputDC = getDataContainerArray()->createNonPrereqDataContainer(this, getImageGeometryPath(), DataContainerID);
  if(getErrorCode() < 0)
  {
    return;
  }

  ImageGeom::Pointer image = ImageGeom::CreateGeometry(SIMPL::Geometry::ImageGeometry);
  m_GeneratedVolume = image;
  outputDC->setGeometry(image);

  // Sanity Check the Dimensions and Spacing
  checkPositiveDimension<ResampleRectGridToImageGeom, int32_t>(this, m_Dimensions[0], k_Dim0Error);
  if(getErrorCode() < 0)
  {
    return;
  }
  checkPositiveDimension<ResampleRectGridToImageGeom, int32_t>(this, m_Dimensions[1], k_Dim1Error);
  if(getErrorCode() < 0)
  {
    return;
  }
  checkPositiveDimension<ResampleRectGridToImageGeom, int32_t>(this, m_Dimensions[2], k_Dim2Error);
  if(getErrorCode() < 0)
  {
    return;
  }

  // Set the Dimensions, Spacing and Origin of the output data container
  image->setDimensions(static_cast<size_t>(m_Dimensions[0]), static_cast<size_t>(m_Dimensions[1]), static_cast<size_t>(m_Dimensions[2]));
  image->setUnits(rectGrid->getUnits());

  if(rgXCoords != nullptr && rgXCoords->isAllocated() && rgYCoords != nullptr && rgYCoords->isAllocated() && rgZCoords != nullptr && rgZCoords->isAllocated())
  {
    FloatVec3Type origin = {(*rgXCoords).front(), (*rgYCoords).front(), (*rgZCoords).front()};
    image->setOrigin(origin);
    FloatVec3Type max = {(*rgXCoords).back(), (*rgYCoords).back(), (*rgZCoords).back()};

    FloatVec3Type spacing = {(max[0] - origin[0]) / m_Dimensions[0], (max[1] - origin[1]) / m_Dimensions[1], (max[2] - origin[2]) / m_Dimensions[2]};
    image->setSpacing(spacing);
  }
  std::vector<size_t> tDims = {static_cast<size_t>(m_Dimensions[0]), static_cast<size_t>(m_Dimensions[1]), static_cast<size_t>(m_Dimensions[2])};
  AttributeMatrix::Pointer imageGeomCellAM = outputDC->createNonPrereqAttributeMatrix(this, getImageGeomCellAttributeMatrix(), tDims, AttributeMatrix::Type::Cell, AttributeMatrixID21);
  if(getErrorCode() < 0 || nullptr == imageGeomCellAM)
  {
    return;
  }

  size_t totalPoints = m_Dimensions[0] * m_Dimensions[1] * m_Dimensions[2];
  for(const auto& path : paths)
  {
    AttributeMatrix::Pointer rectGridCellAM = inputDC->getAttributeMatrix(path.getAttributeMatrixName());
    IDataArray::Pointer inputDataArray = rectGridCellAM->getAttributeArray(path.getDataArrayName());
    IDataArray::Pointer data = inputDataArray->createNewArray(totalPoints, inputDataArray->getComponentDimensions(), inputDataArray->getName(), !getInPreflight());
    imageGeomCellAM->insertOrAssign(data);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ResampleRectGridToImageGeom::execute()
{
  initialize();
  dataCheck();
  if(getErrorCode() < 0)
  {
    return;
  }

  if(getCancel())
  {
    return;
  }

  DataContainer::Pointer inputDC = getDataContainerArray()->getDataContainer(getRectilinearGridPath());
  RectGridGeom::Pointer rectGrid = inputDC->getGeometryAs<RectGridGeom>();
  FloatArrayType::Pointer xGridValues = rectGrid->getXBounds();
  FloatArrayType::Pointer yGridValues = rectGrid->getYBounds();
  FloatArrayType::Pointer zGridValues = rectGrid->getZBounds();
  SizeVec3Type rectGridDims = rectGrid->getDimensions();

  DataContainer::Pointer outputDC = getDataContainerArray()->getDataContainer(getImageGeometryPath());
  ImageGeom::Pointer imageGeom = outputDC->getGeometryAs<ImageGeom>();

  SizeVec3Type imageGeomDims = imageGeom->getDimensions();
  FloatVec3Type imageGeomSpacing = {(xGridValues->back() - xGridValues->front()) / imageGeomDims[0], (yGridValues->back() - yGridValues->front()) / imageGeomDims[1],
                                    (zGridValues->back() - zGridValues->front()) / imageGeomDims[2]};
  imageGeom->setSpacing(imageGeomSpacing);
  FloatVec3Type halfSpacing = {imageGeomSpacing[0] * ::k_Half, imageGeomSpacing[1] * ::k_Half, imageGeomSpacing[2] * ::k_Half};

  FloatVec3Type imageGeomOrigin = {xGridValues->getValue(0), yGridValues->getValue(0), zGridValues->getValue(0)};
  imageGeom->setOrigin(imageGeomOrigin);

  size_t rgIdxStart = 1;
  std::vector<size_t> xIdx(imageGeomDims[0], 0);
  for(size_t x = 0; x < imageGeomDims[0]; x++)
  {
    float coord = imageGeomOrigin[0] + (x * imageGeomSpacing[0]) + halfSpacing[0];
    for(size_t rgIdx = rgIdxStart; rgIdx < xGridValues->size(); rgIdx++)
    {
      if(coord > xGridValues->getValue(rgIdx - 1) && coord <= xGridValues->getValue(rgIdx))
      {
        xIdx[x] = rgIdx - 1;
        rgIdxStart = rgIdx;
        break;
      }
    }
  }

  rgIdxStart = 1;
  std::vector<size_t> yIdx(imageGeomDims[1], 0);
  for(size_t y = 0; y < imageGeomDims[1]; y++)
  {
    float coord = imageGeomOrigin[1] + (y * imageGeomSpacing[1]) + halfSpacing[1];
    for(size_t rgIdx = rgIdxStart; rgIdx < yGridValues->size(); rgIdx++)
    {
      if(coord > yGridValues->getValue(rgIdx - 1) && coord <= yGridValues->getValue(rgIdx))
      {
        yIdx[y] = rgIdx - 1;
        rgIdxStart = rgIdx;
        break;
      }
    }
  }

  rgIdxStart = 1;
  std::vector<size_t> zIdx(imageGeomDims[2], 0);
  for(size_t z = 0; z < imageGeomDims[2]; z++)
  {
    float coord = imageGeomOrigin[2] + (z * imageGeomSpacing[2]) + halfSpacing[2];
    for(size_t rgIdx = rgIdxStart; rgIdx < zGridValues->size(); rgIdx++)
    {
      if(coord > zGridValues->getValue(rgIdx - 1) && coord <= zGridValues->getValue(rgIdx))
      {
        zIdx[z] = rgIdx - 1;
        rgIdxStart = rgIdx;
        break;
      }
    }
  }

  // Store the mapped XYZ index into the RectGrid data ararys
  std::vector<size_t> newIdxs(imageGeom->getNumberOfElements());
  size_t currIdx = 0;
  for(size_t z : zIdx)
  {
    for(size_t y : yIdx)
    {
      for(size_t x : xIdx)
      {
        // Compute the index into the RectGrid Data Array
        size_t idx = (rectGridDims[0] * rectGridDims[1] * z) + (rectGridDims[0] * y) + x;
        // Compute the index into the new Idx Array
        newIdxs[currIdx++] = idx;
      }
    }
  }
  if(getCancel())
  {
    return;
  }

  if(!m_SelectedDataArrayPaths.isEmpty())
  {

    AttributeMatrix::Pointer rectGridCellAM = inputDC->getAttributeMatrix(m_SelectedDataArrayPaths[0].getAttributeMatrixName());

    AttributeMatrix::Pointer imageGeomCellAM = outputDC->getAttributeMatrix(getImageGeomCellAttributeMatrix());
    size_t totalPoints = imageGeom->getNumberOfElements();

    QList<QString> voxelArrayNames = rectGridCellAM->getAttributeArrayNames();
    for(const QString& voxelArrayName : voxelArrayNames)
    {
      IDataArray::Pointer inputDataArray = rectGridCellAM->getAttributeArray(voxelArrayName);
      // Make a copy of the 'p' array that has the same name. When placed into
      // the data container this will over write the current array with
      // the same name. At least in theory
      IDataArray::Pointer data = inputDataArray->createNewArray(totalPoints, inputDataArray->getComponentDimensions(), inputDataArray->getName());
      void* source = nullptr;
      void* destination = nullptr;
      size_t newIndicies_I = 0;
      int nComp = data->getNumberOfComponents();
      for(size_t i = 0; i < static_cast<size_t>(totalPoints); i++)
      {
        newIndicies_I = newIdxs[i];

        source = inputDataArray->getVoidPointer(nComp * newIndicies_I);
        destination = data->getVoidPointer(nComp * i);
        ::memcpy(destination, source, inputDataArray->getTypeSize() * data->getNumberOfComponents());
      }
      //  rectGridCellAM->removeAttributeArray(*iter);
      imageGeomCellAM->insertOrAssign(data);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer ResampleRectGridToImageGeom::newFilterInstance(bool copyFilterParameters) const
{
  ResampleRectGridToImageGeom::Pointer filter = ResampleRectGridToImageGeom::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ResampleRectGridToImageGeom::getCompiledLibraryName() const
{
  return SamplingConstants::SamplingBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ResampleRectGridToImageGeom::getBrandingString() const
{
  return "Sampling";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ResampleRectGridToImageGeom::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << Sampling::Version::Major() << "." << Sampling::Version::Minor() << "." << Sampling::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ResampleRectGridToImageGeom::getGroupName() const
{
  return SIMPL::FilterGroups::Unsupported;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ResampleRectGridToImageGeom::getSubGroupName() const
{
  return "Sampling";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ResampleRectGridToImageGeom::getHumanLabel() const
{
  return "Resample Rectilinear Grid to Image Geom";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid ResampleRectGridToImageGeom::getUuid() const
{
  return {"{77befd69-4536-5856-9f81-02996d038f73}"};
}

// -----------------------------------------------------------------------------
ResampleRectGridToImageGeom::Pointer ResampleRectGridToImageGeom::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<ResampleRectGridToImageGeom> ResampleRectGridToImageGeom::New()
{
  struct make_shared_enabler : public ResampleRectGridToImageGeom
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString ResampleRectGridToImageGeom::getNameOfClass() const
{
  return QString("ResampleRectGridToImageGeom");
}

// -----------------------------------------------------------------------------
QString ResampleRectGridToImageGeom::ClassName()
{
  return QString("ResampleRectGridToImageGeom");
}

// -----------------------------------------------------------------------------
void ResampleRectGridToImageGeom::setImageGeometryPath(const DataArrayPath& value)
{
  m_ImageGeometryPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath ResampleRectGridToImageGeom::getImageGeometryPath() const
{
  return m_ImageGeometryPath;
}

// -----------------------------------------------------------------------------
void ResampleRectGridToImageGeom::setImageGeomCellAttributeMatrix(const QString& value)
{
  m_ImageGeomCellAttributeMatrix = value;
}

// -----------------------------------------------------------------------------
QString ResampleRectGridToImageGeom::getImageGeomCellAttributeMatrix() const
{
  return m_ImageGeomCellAttributeMatrix;
}

// -----------------------------------------------------------------------------
void ResampleRectGridToImageGeom::setLengthUnit(int32_t value)
{
  m_LengthUnit = value;
}

// -----------------------------------------------------------------------------
int32_t ResampleRectGridToImageGeom::getLengthUnit() const
{
  return m_LengthUnit;
}

// -----------------------------------------------------------------------------
void ResampleRectGridToImageGeom::setDimensions(const IntVec3Type& value)
{
  m_Dimensions = value;
}

// -----------------------------------------------------------------------------
IntVec3Type ResampleRectGridToImageGeom::getDimensions() const
{
  return m_Dimensions;
}

// -----------------------------------------------------------------------------
void ResampleRectGridToImageGeom::setRectilinearGridPath(const DataArrayPath& value)
{
  m_RectilinearGridPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath ResampleRectGridToImageGeom::getRectilinearGridPath() const
{
  return m_RectilinearGridPath;
}

// -----------------------------------------------------------------------------
QString GenerateSourceVolumeInformation(RectGridGeom* geom)
{
  QString desc;
  QTextStream ss(&desc);
  SizeVec3Type dims = geom->getDimensions();

  QString lengthUnit = IGeometry::LengthUnitToString(static_cast<IGeometry::LengthUnit>(geom->getUnits()));

  ss // << "X Range: " << origin[0] << " to " << (origin[0] + (dims[0] * spacing[0])) << " (Delta: " << (dims[0] * spacing[0]) << " " << lengthUnit << ") "
     // << 0 << "-"

      << dims[0] << " Voxels\n";
  ss //<< "Y Range: " << origin[1] << " to " << (origin[1] + (dims[1] * spacing[1])) << " (Delta: " << (dims[1] * spacing[1]) << " " << lengthUnit << ") "
     //<< 0 << "-"
      << dims[1] << " Voxels\n";
  ss // << "Z Range: " << origin[2] << " to " << (origin[2] + (dims[2] * spacing[2])) << " (Delta: " << (dims[2] * spacing[2]) << " " << lengthUnit << ") "
     // << 0 << "-"
      << dims[2] << " Voxels\n";

  return desc;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ResampleRectGridToImageGeom::getRectGridGeometryDesc()
{
  QString desc = QString("Geometry Not initialized.");
  if(m_InputRectGridGeometry.get() != nullptr)
  {
    desc = GenerateSourceVolumeInformation(m_InputRectGridGeometry.get());
  }
  return desc;
}

// -----------------------------------------------------------------------------
QString GenerateImageInfoString(ImageGeom* geom)
{
  QString desc;
  QTextStream ss(&desc);
  SizeVec3Type dims = geom->getDimensions();
  FloatVec3Type origin = geom->getOrigin();
  FloatVec3Type spacing = geom->getSpacing();
  QString lengthUnit = IGeometry::LengthUnitToString(static_cast<IGeometry::LengthUnit>(geom->getUnits()));

  ss << "X Range: " << origin[0] << " to " << (origin[0] + (dims[0] * spacing[0])) << " (Delta: " << (dims[0] * spacing[0]) << " " << lengthUnit << ") " << 0 << "-" << dims[0] - 1 << " Voxels\n";
  ss << "Y Range: " << origin[1] << " to " << (origin[1] + (dims[1] * spacing[1])) << " (Delta: " << (dims[1] * spacing[1]) << " " << lengthUnit << ") " << 0 << "-" << dims[1] - 1 << " Voxels\n";
  ss << "Z Range: " << origin[2] << " to " << (origin[2] + (dims[2] * spacing[2])) << " (Delta: " << (dims[2] * spacing[2]) << " " << lengthUnit << ") " << 0 << "-" << dims[2] - 1 << " Voxels\n";
  return desc;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ResampleRectGridToImageGeom::getCreatedGeometryDescription()
{
  QString desc = QString("Geometry Not initialized.");
  if(m_GeneratedVolume != nullptr)
  {
    desc = GenerateImageInfoString(m_GeneratedVolume.get());
  }
  return desc;
}

// -----------------------------------------------------------------------------
void ResampleRectGridToImageGeom::setSelectedDataArrayPaths(const QVector<DataArrayPath>& value)
{
  m_SelectedDataArrayPaths = value;
}

// -----------------------------------------------------------------------------
QVector<DataArrayPath> ResampleRectGridToImageGeom::getSelectedDataArrayPaths() const
{
  return m_SelectedDataArrayPaths;
}
