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
#include "ResampleImageGeom.h"

#include <thread>

#include <QtCore/QTextStream>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/DataContainers/DataContainer.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/AttributeMatrixSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/DataContainerCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/FloatVec3FilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedBooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/PreflightUpdatedValueFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/Geometry/ImageGeom.h"
#include "SIMPLib/Utilities/ParallelData3DAlgorithm.h"
#include "SIMPLib/Utilities/ParallelDataAlgorithm.h"

#include "Sampling/SamplingConstants.h"
#include "Sampling/SamplingFilters/Utils/SamplingUtils.hpp"
#include "Sampling/SamplingVersion.h"

enum createdPathID : RenameDataPath::DataID_t
{
  DataContainerID = 1
};

// -----------------------------------------------------------------------------
class ChangeResolutionImpl
{
public:
  ChangeResolutionImpl(ResampleImageGeom* filter, std::vector<size_t>& newindices, FloatVec3Type spacing, FloatVec3Type sourceSpacing, SizeVec3Type sourceDims, SizeVec3Type destDims)
  : m_Filter(filter)
  , m_NewIndices(newindices)
  , m_Spacing(spacing)
  , m_OrigSpacing(sourceSpacing)
  , m_OrigDims(sourceDims)
  , m_CopyDims(destDims)
  {
  }
  ~ChangeResolutionImpl() = default;

  // -----------------------------------------------------------------------------
  void compute(size_t zStart, size_t zEnd, size_t yStart, size_t yEnd, size_t xStart, size_t xEnd) const
  {
    for(size_t i = zStart; i < zEnd; i++)
    {
      for(size_t j = yStart; j < yEnd; j++)
      {
        if(m_Filter->getCancel())
        {
          return;
        }

        for(size_t k = xStart; k < xEnd; k++)
        {
          float x = (k * m_Spacing[0]);
          float y = (j * m_Spacing[1]);
          float z = (i * m_Spacing[2]);
          size_t col = static_cast<size_t>(x / m_OrigSpacing[0]);
          size_t row = static_cast<size_t>(y / m_OrigSpacing[1]);
          size_t plane = static_cast<size_t>(z / m_OrigSpacing[2]);
          size_t index_old = (plane * m_OrigDims[1] * m_OrigDims[0]) + (row * m_OrigDims[0]) + col;
          size_t index = (i * m_CopyDims[0] * m_CopyDims[1]) + (j * m_CopyDims[0]) + k;
          m_NewIndices[index] = index_old;
        }
      }
    }
  }

  // -----------------------------------------------------------------------------
  void operator()(const SIMPLRange3D& r) const
  {
    compute(r[0], r[1], r[2], r[3], r[4], r[5]);
  }

private:
  ResampleImageGeom* m_Filter = nullptr;
  std::vector<size_t>& m_NewIndices;
  FloatVec3Type m_Spacing;
  FloatVec3Type m_OrigSpacing;
  SizeVec3Type m_OrigDims;
  SizeVec3Type m_CopyDims;
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
class ChangeResolutionPlaceDataImpl
{
public:
  ChangeResolutionPlaceDataImpl(ResampleImageGeom* filter, std::vector<size_t>& newindices, IDataArray::Pointer sourceData, IDataArray::Pointer destinationData)
  : m_Filter(filter)
  , m_NewIndices(newindices)
  , m_SourceData(sourceData)
  , m_DestinationData(destinationData)
  {
  }
  ~ChangeResolutionPlaceDataImpl() = default;

  // -----------------------------------------------------------------------------
  void compute(size_t start, size_t end) const
  {
    if(m_Filter->getCancel())
    {
      return;
    }

    int32_t nComp = m_SourceData->getNumberOfComponents();

    for(size_t i = start; i < end; i++)
    {
      size_t newIndicies_I = m_NewIndices[i];
      const void* source = m_SourceData->getVoidPointer((nComp * newIndicies_I));
      void* destination = m_DestinationData->getVoidPointer((m_DestinationData->getNumberOfComponents() * i));
      std::memcpy(destination, source, m_SourceData->getTypeSize() * m_DestinationData->getNumberOfComponents());
    }
    }
  }

  // -----------------------------------------------------------------------------
  void operator()(const SIMPLRange& r) const
  {
    compute(r[0], r[1]);
  }

private:
  ResampleImageGeom* m_Filter = nullptr;
  std::vector<size_t>& m_NewIndices;
  IDataArray::Pointer m_SourceData;
  IDataArray::Pointer m_DestinationData;
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ResampleImageGeom::ResampleImageGeom() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ResampleImageGeom::~ResampleImageGeom() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ResampleImageGeom::setupFilterParameters()
{
  FilterParameterVectorType parameters;

  PreflightUpdatedValueFilterParameter::Pointer param = SIMPL_NEW_PREFLIGHTUPDATEDVALUE_FP("Current Image Geometry", CurrentGeomtryInfo, FilterParameter::Parameter, ResampleImageGeom);
  param->setReadOnly(true);
  parameters.push_back(param);

  parameters.push_back(SIMPL_NEW_FLOAT_VEC3_FP("Spacing", Spacing, FilterParameter::Parameter, ResampleImageGeom));
  parameters.back()->setLegacyPropertyName("Resolution");

  param = SIMPL_NEW_PREFLIGHTUPDATEDVALUE_FP("Created/Updated Image Geometry", NewGeomtryInfo, FilterParameter::Parameter, ResampleImageGeom);
  param->setReadOnly(true);
  parameters.push_back(param);

  QStringList linkedProps;
  linkedProps << "CellFeatureAttributeMatrixPath"
              << "FeatureIdsArrayPath";
  parameters.push_back(SIMPL_NEW_LINKED_BOOL_FP("Renumber Features", RenumberFeatures, FilterParameter::Parameter, ResampleImageGeom, linkedProps));
  linkedProps.clear();
  linkedProps << "NewDataContainerPath";
  parameters.push_back(SIMPL_NEW_LINKED_BOOL_FP("Save as New Data Container", SaveAsNewDataContainer, FilterParameter::Parameter, ResampleImageGeom, linkedProps));
  parameters.push_back(SeparatorFilterParameter::New("Cell Data", FilterParameter::RequiredArray));
  {
    AttributeMatrixSelectionFilterParameter::RequirementType req = AttributeMatrixSelectionFilterParameter::CreateRequirement(AttributeMatrix::Type::Cell, IGeometry::Type::Image);
    parameters.push_back(SIMPL_NEW_AM_SELECTION_FP("Cell Attribute Matrix", CellAttributeMatrixPath, FilterParameter::RequiredArray, ResampleImageGeom, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Int32, 1, AttributeMatrix::Type::Cell, IGeometry::Type::Image);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Feature Ids", FeatureIdsArrayPath, FilterParameter::RequiredArray, ResampleImageGeom, req));
  }
  parameters.push_back(SeparatorFilterParameter::New("Cell Feature Data", FilterParameter::RequiredArray));
  {
    AttributeMatrixSelectionFilterParameter::RequirementType req = AttributeMatrixSelectionFilterParameter::CreateRequirement(AttributeMatrix::Type::CellFeature, IGeometry::Type::Image);
    parameters.push_back(SIMPL_NEW_AM_SELECTION_FP("Cell Feature Attribute Matrix", CellFeatureAttributeMatrixPath, FilterParameter::RequiredArray, ResampleImageGeom, req));
  }
  parameters.push_back(SIMPL_NEW_DC_CREATION_FP("Data Container", NewDataContainerPath, FilterParameter::CreatedArray, ResampleImageGeom));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ResampleImageGeom::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setNewDataContainerPath(reader->readDataArrayPath("NewDataContainerPath", getNewDataContainerPath()));
  setCellAttributeMatrixPath(reader->readDataArrayPath("CellAttributeMatrixPath", getCellAttributeMatrixPath()));
  setCellFeatureAttributeMatrixPath(reader->readDataArrayPath("CellFeatureAttributeMatrixPath", getCellFeatureAttributeMatrixPath()));
  setFeatureIdsArrayPath(reader->readDataArrayPath("FeatureIdsArrayPath", getFeatureIdsArrayPath()));
  setSpacing(reader->readFloatVec3("Spacing", getSpacing()));
  setRenumberFeatures(reader->readValue("RenumberFeatures", getRenumberFeatures()));
  setSaveAsNewDataContainer(reader->readValue("SaveAsNewDataContainer", getSaveAsNewDataContainer()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ResampleImageGeom::dataCheck()
{
  clearErrorCode();
  clearWarningCode();

  // Check the validity of the user supplied spacing values
  FloatVec3Type spacing = getSpacing();
  if(spacing[0] <= 0.0F)
  {
    QString ss = QObject::tr("The X spacing (%1) must be positive").arg(spacing[0]);
    setErrorCondition(-5555, ss);
  }

  if(spacing[1] <= 0.0F)
  {
    QString ss = QObject::tr("The Y spacing (%1) must be positive").arg(spacing[1]);
    setErrorCondition(-5556, ss);
  }

  if(spacing[2] <= 0.0F)
  {
    QString ss = QObject::tr("The Z spacing (%1) must be positive").arg(spacing[2]);
    setErrorCondition(-5557, ss);
  }

  if(getErrorCode() < 0)
  {
    return;
  }

  DataContainerArray::Pointer dca = getDataContainerArray();

  // Validate the incoming DataContainer, Geometry, and AttributeMatrix ; bail if any do not exist since we plan on using them later on in the dataCheck
  // Error messages are handled by the getPrereq functions
  DataContainer::Pointer sourceDc = dca->getPrereqDataContainer(this, getCellAttributeMatrixPath().getDataContainerName());
  ImageGeom::Pointer sourceImageGeom = dca->getPrereqGeometryFromDataContainer<ImageGeom>(this, getCellAttributeMatrixPath().getDataContainerName());
  AttributeMatrix::Pointer sourceAM = dca->getPrereqAttributeMatrixFromPath(this, getCellAttributeMatrixPath(), -46500);
  if(getErrorCode() < 0)
  {
    return;
  }

  FloatVec3Type sourceSpacing = sourceImageGeom->getSpacing();
  // If the spacing is the same between the origin and the new, then just bail out now because there is nothing to do.
  if(sourceSpacing[0] == m_Spacing[0] && sourceSpacing[1] == m_Spacing[1] && sourceSpacing[2] == m_Spacing[2])
  {
    return;
  }

  // Now make the copy of the DataContainer and adjust the Image Geometry and adjust the Cell Attribute Matrix tuple count
  DataContainer::Pointer destDc = dca->createNonPrereqDataContainer(this, getNewDataContainerPath(), DataContainerID);
  if(getErrorCode() < 0)
  {
    return;
  }
  IGeometry::Pointer destImageGeom = sourceImageGeom->deepCopy();
  destDc->setGeometry(destImageGeom);

  AttributeMatrix::Pointer destCellAttrMat = sourceAM->deepCopy(getInPreflight());
  destDc->addOrReplaceAttributeMatrix(destCellAttrMat);

  // Adjust the Geometry values

  FloatVec3Type destSpacing = destDc->getGeometryAs<ImageGeom>()->getSpacing();
  SizeVec3Type dims = destDc->getGeometryAs<ImageGeom>()->getDimensions();

  size_t m_XP = static_cast<size_t>(((destSpacing[0] * static_cast<float>(dims[0])) / m_Spacing[0]));
  size_t m_YP = static_cast<size_t>(((destSpacing[1] * static_cast<float>(dims[1])) / m_Spacing[1]));
  size_t m_ZP = static_cast<size_t>(((destSpacing[2] * static_cast<float>(dims[2])) / m_Spacing[2]));
  if(m_XP == 0)
  {
    m_XP = 1;
  }
  if(m_YP == 0)
  {
    m_YP = 1;
  }
  if(m_ZP == 0)
  {
    m_ZP = 1;
  }
  // Update the ImageGeometry object
  destDc->getGeometryAs<ImageGeom>()->setDimensions(m_XP, m_YP, m_ZP);
  destDc->getGeometryAs<ImageGeom>()->setSpacing(m_Spacing);

  // Resize the Cell Attribute Matrix Tuple dimensions (which will resize the contained Attribute Arrays)
  size_t totalPoints = m_XP * m_YP * m_ZP;
  AttributeMatrix::Pointer destCellAM = AttributeMatrix::New({m_XP, m_YP, m_ZP}, sourceAM->getName(), sourceAM->getType());

  destDc->addOrReplaceAttributeMatrix(destCellAM);
  QList<QString> voxelArrayNames = sourceAM->getAttributeArrayNames();
  for(QList<QString>::iterator iter = voxelArrayNames.begin(); iter != voxelArrayNames.end(); ++iter)
  {
    IDataArray::Pointer p = sourceAM->getAttributeArray(*iter);
    IDataArray::Pointer data = p->createNewArray(totalPoints, p->getComponentDimensions(), p->getName(), !getInPreflight());
    destCellAM->insertOrAssign(data);
  }

  if(getRenumberFeatures())
  {
    std::vector<size_t> cDims = {1};
    m_FeatureIdsPtr = dca->getPrereqArrayFromPath<Int32ArrayType>(this, getFeatureIdsArrayPath(), cDims);
    if(getErrorCode() < 0)
    {
      return;
    }

    dca->getPrereqAttributeMatrixFromPath(this, getCellFeatureAttributeMatrixPath(), -46501);
    if(getErrorCode() < 0)
    {
      return;
    }
  }

  // If we are doing an "In Place" resample then remove the original Data Container and just rename the new one with the updated
  // ImageGeometry and Cell Attribute Matrix
  m_PreviousDataContainer = sourceDc; // Save for later
  if(getSaveAsNewDataContainer())
  {
    destDc->setName(getNewDataContainerPath().getDataContainerName());
    if(getRenumberFeatures())
    {
      AttributeMatrix::Pointer sourceFeatureAM = sourceDc->getAttributeMatrix(getCellFeatureAttributeMatrixPath().getAttributeMatrixName());
      destDc->addOrReplaceAttributeMatrix(sourceFeatureAM->deepCopy(getInPreflight()));
    }
  }
  else
  {
    dca->removeDataContainer(sourceDc->getName()); // Remove the original source DC from the DCA

    // Copy over all of the other AttributeMatrices except the Cell AttributeMatrix since it is being recreated
    DataContainer::Container_t sourceAms = sourceDc->getAttributeMatrices();
    for(const auto& am : sourceAms)
    {
      if(am->getName() != getCellAttributeMatrixPath().getAttributeMatrixName())
      {
        destDc->addOrReplaceAttributeMatrix(am);
      }
    }
    destDc->setName(getCellAttributeMatrixPath().getDataContainerName()); // Update the name to the origin DC name
  }
  dca->addOrReplaceDataContainer(destDc);

  m_CurrentGeomInfo = generateCurrentGeomInfo();
  m_NewGeomInfo = generateNewGeomInfo();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ResampleImageGeom::execute()
{
  QString ss = QObject::tr("Allocating memory .....");
  notifyStatusMessage(ss);

  dataCheck();
  if(getErrorCode() < 0)
  {
    return;
  }

  DataContainerArray::Pointer dca = getDataContainerArray();

  DataContainer::Pointer sourceDc = m_PreviousDataContainer;
  ImageGeom::Pointer sourceGeom = sourceDc->getGeometryAs<ImageGeom>();
  AttributeMatrix::Pointer sourceCellAM = sourceDc->getAttributeMatrix(getCellAttributeMatrixPath());
  SizeVec3Type sourceDims = sourceGeom->getDimensions();
  FloatVec3Type sourceSpacing = sourceGeom->getSpacing();

  // Get the resampled data container, by default it would have been inserted into the Data Container Array
  DataContainer::Pointer destDc = dca->getDataContainer(getCellAttributeMatrixPath());
  // Unless the user selected to create a new one
  if(getSaveAsNewDataContainer())
  {
    destDc = dca->getDataContainer(getNewDataContainerPath());
  }
  AttributeMatrix::Pointer destAM = destDc->getAttributeMatrix(getCellAttributeMatrixPath().getAttributeMatrixName());
  size_t destTotalPoints = destAM->getNumberOfTuples();
  ImageGeom::Pointer destGeom = destDc->getGeometryAs<ImageGeom>();
  SizeVec3Type destDims = destGeom->getDimensions();

  std::vector<size_t> newindicies(destTotalPoints);

  ss = QObject::tr("Computing new indices... ");
  notifyStatusMessage(ss);
  // Allow data-based parallelization
  size_t grain = destDims[2] == 1 ? 1 : destDims[2] / std::thread::hardware_concurrency();
  if(grain == 0) // This can happen if dims[2] > number of processors
  {
    grain = 1;
  }
  ParallelData3DAlgorithm dataAlg;
  dataAlg.setRange(destDims[2], destDims[1], destDims[0]);
  dataAlg.setGrain(grain);
  dataAlg.setParallelizationEnabled(true);
  dataAlg.execute(ChangeResolutionImpl(this, newindicies, m_Spacing, sourceSpacing, sourceDims, destDims));

  QList<QString> voxelArrayNames = destAM->getAttributeArrayNames();
  for(QList<QString>::iterator iter = voxelArrayNames.begin(); iter != voxelArrayNames.end(); ++iter)
  {
    ss = QObject::tr("Placing Resampled Data Array '%1'").arg(*iter);
    notifyStatusMessage(ss);

    IDataArray::Pointer sourceData = sourceCellAM->getAttributeArray(*iter);
    IDataArray::Pointer destinationData = destAM->getAttributeArray(*iter);

    ParallelDataAlgorithm placeAlg;
    placeAlg.setRange(0, destTotalPoints);
    placeAlg.setParallelizationEnabled(true);
    placeAlg.execute(ChangeResolutionPlaceDataImpl(this, newindicies, sourceData, destinationData));
  }

  if(m_RenumberFeatures)
  {
    ss = QObject::tr("Renumber Features .....");
    notifyStatusMessage(ss);

    Sampling::RenumberFeatures(this, getNewDataContainerPath(), getCellAttributeMatrixPath(), getCellFeatureAttributeMatrixPath(), getFeatureIdsArrayPath(), getSaveAsNewDataContainer());
  }

  ss = QObject::tr("Cleanup .....");
  notifyStatusMessage(ss);

  if(!getSaveAsNewDataContainer())
  {
    m_PreviousDataContainer = DataContainer::NullPointer();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer ResampleImageGeom::newFilterInstance(bool copyFilterParameters) const
{
  ResampleImageGeom::Pointer filter = ResampleImageGeom::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ResampleImageGeom::getCompiledLibraryName() const
{
  return SamplingConstants::SamplingBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ResampleImageGeom::getBrandingString() const
{
  return "Sampling";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ResampleImageGeom::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << Sampling::Version::Major() << "." << Sampling::Version::Minor() << "." << Sampling::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ResampleImageGeom::getGroupName() const
{
  return SIMPL::FilterGroups::SamplingFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid ResampleImageGeom::getUuid() const
{
  return QUuid("{1966e540-759c-5798-ae26-0c6a3abc65c0}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ResampleImageGeom::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::ResolutionFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ResampleImageGeom::getHumanLabel() const
{
  return "Resample Data (Image Geometry)";
}

// -----------------------------------------------------------------------------
ResampleImageGeom::Pointer ResampleImageGeom::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<ResampleImageGeom> ResampleImageGeom::New()
{
  struct make_shared_enabler : public ResampleImageGeom
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString ResampleImageGeom::getNameOfClass() const
{
  return QString("ResampleImageGeom");
}

// -----------------------------------------------------------------------------
QString ResampleImageGeom::ClassName()
{
  return QString("ResampleImageGeom");
}

// -----------------------------------------------------------------------------
void ResampleImageGeom::setNewDataContainerPath(const DataArrayPath& value)
{
  m_NewDataContainerPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath ResampleImageGeom::getNewDataContainerPath() const
{
  return m_NewDataContainerPath;
}

// -----------------------------------------------------------------------------
void ResampleImageGeom::setCellAttributeMatrixPath(const DataArrayPath& value)
{
  m_CellAttributeMatrixPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath ResampleImageGeom::getCellAttributeMatrixPath() const
{
  return m_CellAttributeMatrixPath;
}

// -----------------------------------------------------------------------------
void ResampleImageGeom::setCellFeatureAttributeMatrixPath(const DataArrayPath& value)
{
  m_CellFeatureAttributeMatrixPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath ResampleImageGeom::getCellFeatureAttributeMatrixPath() const
{
  return m_CellFeatureAttributeMatrixPath;
}

// -----------------------------------------------------------------------------
void ResampleImageGeom::setSpacing(const FloatVec3Type& value)
{
  m_Spacing = value;
}

// -----------------------------------------------------------------------------
FloatVec3Type ResampleImageGeom::getSpacing() const
{
  return m_Spacing;
}

// -----------------------------------------------------------------------------
void ResampleImageGeom::setRenumberFeatures(bool value)
{
  m_RenumberFeatures = value;
}

// -----------------------------------------------------------------------------
bool ResampleImageGeom::getRenumberFeatures() const
{
  return m_RenumberFeatures;
}

// -----------------------------------------------------------------------------
void ResampleImageGeom::setSaveAsNewDataContainer(bool value)
{
  m_SaveAsNewDataContainer = value;
}

// -----------------------------------------------------------------------------
bool ResampleImageGeom::getSaveAsNewDataContainer() const
{
  return m_SaveAsNewDataContainer;
}

// -----------------------------------------------------------------------------
void ResampleImageGeom::setFeatureIdsArrayPath(const DataArrayPath& value)
{
  m_FeatureIdsArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath ResampleImageGeom::getFeatureIdsArrayPath() const
{
  return m_FeatureIdsArrayPath;
}

// -----------------------------------------------------------------------------
QString ResampleImageGeom::getCurrentGeomtryInfo() const
{
  return m_CurrentGeomInfo;
}

// -----------------------------------------------------------------------------
QString ResampleImageGeom::getNewGeomtryInfo() const
{
  return m_NewGeomInfo;
}

// -----------------------------------------------------------------------------
QString ResampleImageGeom::generateCurrentGeomInfo() const
{
  QString desc = QString("Please select a Data Container that has an Image Geometry");
  DataContainerArray::Pointer dca = getDataContainerArray();
  if(nullptr != dca)
  {
    DataContainer::Pointer m = m_PreviousDataContainer;
    if(nullptr != m)
    {
      ImageGeom::Pointer image = m->getGeometryAs<ImageGeom>();
      if(nullptr != image)
      {
        FloatVec3Type spacing = image->getSpacing();
        FloatVec3Type origin = image->getOrigin();
        SizeVec3Type dims = image->getDimensions();

        desc.clear();
        QTextStream ss(&desc);
        ss << "Current Image Geometry:\n";
        ss << "X Range: " << origin[0] << " to " << (origin[0] + (dims[0] * spacing[0])) << " (Delta: " << (dims[0] * spacing[0]) << ") " << 0 << "-" << dims[0] - 1 << " Voxels\n";
        ss << "Y Range: " << origin[1] << " to " << (origin[1] + (dims[1] * spacing[1])) << " (Delta: " << (dims[1] * spacing[1]) << ") " << 0 << "-" << dims[1] - 1 << " Voxels\n";
        ss << "Z Range: " << origin[2] << " to " << (origin[2] + (dims[2] * spacing[2])) << " (Delta: " << (dims[2] * spacing[2]) << ") " << 0 << "-" << dims[2] - 1 << " Voxels\n";
      }
    }
  }
  return desc;
}

// -----------------------------------------------------------------------------
QString ResampleImageGeom::generateNewGeomInfo() const
{
  QString desc = QString("Please select a Data Container that has an Image Geometry");
  DataContainerArray::Pointer dca = getDataContainerArray();
  if(nullptr != dca)
  {
    QString dcName = getNewDataContainerPath().getDataContainerName();
    if(!getSaveAsNewDataContainer())
    {
      dcName = getCellAttributeMatrixPath().getDataContainerName();
    }
    DataContainer::Pointer m = dca->getDataContainer(dcName);
    if(nullptr != m)
    {
      ImageGeom::Pointer image = m->getGeometryAs<ImageGeom>();
      if(nullptr != image)
      {
        FloatVec3Type spacing = image->getSpacing();
        FloatVec3Type origin = image->getOrigin();
        SizeVec3Type dims = image->getDimensions();

        desc.clear();
        QTextStream ss(&desc);
        ss << "New Image Geometry:\n";
        ss << "X Range: " << origin[0] << " to " << (origin[0] + (dims[0] * spacing[0])) << " (Delta: " << (dims[0] * spacing[0]) << ") " << 0 << "-" << dims[0] - 1 << " Voxels\n";
        ss << "Y Range: " << origin[1] << " to " << (origin[1] + (dims[1] * spacing[1])) << " (Delta: " << (dims[1] * spacing[1]) << ") " << 0 << "-" << dims[1] - 1 << " Voxels\n";
        ss << "Z Range: " << origin[2] << " to " << (origin[2] + (dims[2] * spacing[2])) << " (Delta: " << (dims[2] * spacing[2]) << ") " << 0 << "-" << dims[2] - 1 << " Voxels\n";
      }
    }
  }
  return desc;
}
