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
#include "ChangeResolution.h"

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
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/Geometry/ImageGeom.h"

#include "Sampling/SamplingConstants.h"
#include "Sampling/SamplingVersion.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ChangeResolution::ChangeResolution() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ChangeResolution::~ChangeResolution() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ChangeResolution::setupFilterParameters()
{
  FilterParameterVectorType parameters;
  parameters.push_back(SIMPL_NEW_FLOAT_VEC3_FP("Spacing", Spacing, FilterParameter::Parameter, ChangeResolution));
  parameters.back()->setLegacyPropertyName("Resolution");

  QStringList linkedProps;
  linkedProps << "CellFeatureAttributeMatrixPath"
              << "FeatureIdsArrayPath";
  parameters.push_back(SIMPL_NEW_LINKED_BOOL_FP("Renumber Features", RenumberFeatures, FilterParameter::Parameter, ChangeResolution, linkedProps));
  linkedProps.clear();
  linkedProps << "NewDataContainerPath";
  parameters.push_back(SIMPL_NEW_LINKED_BOOL_FP("Save as New Data Container", SaveAsNewDataContainer, FilterParameter::Parameter, ChangeResolution, linkedProps));
  parameters.push_back(SeparatorFilterParameter::New("Cell Data", FilterParameter::RequiredArray));
  {
    AttributeMatrixSelectionFilterParameter::RequirementType req = AttributeMatrixSelectionFilterParameter::CreateRequirement(AttributeMatrix::Type::Cell, IGeometry::Type::Image);
    parameters.push_back(SIMPL_NEW_AM_SELECTION_FP("Cell Attribute Matrix", CellAttributeMatrixPath, FilterParameter::RequiredArray, ChangeResolution, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Int32, 1, AttributeMatrix::Type::Cell, IGeometry::Type::Image);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Feature Ids", FeatureIdsArrayPath, FilterParameter::RequiredArray, ChangeResolution, req));
  }
  parameters.push_back(SeparatorFilterParameter::New("Cell Feature Data", FilterParameter::RequiredArray));
  {
    AttributeMatrixSelectionFilterParameter::RequirementType req = AttributeMatrixSelectionFilterParameter::CreateRequirement(AttributeMatrix::Type::CellFeature, IGeometry::Type::Image);
    parameters.push_back(SIMPL_NEW_AM_SELECTION_FP("Cell Feature Attribute Matrix", CellFeatureAttributeMatrixPath, FilterParameter::RequiredArray, ChangeResolution, req));
  }
  parameters.push_back(SIMPL_NEW_DC_CREATION_FP("Data Container", NewDataContainerPath, FilterParameter::CreatedArray, ChangeResolution));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ChangeResolution::readFilterParameters(AbstractFilterParametersReader* reader, int index)
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
void ChangeResolution::initialize()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ChangeResolution::dataCheck()
{
  clearErrorCode();
  clearWarningCode();

  DataContainerArray::Pointer dca = getDataContainerArray();

  // MUST have the Required Cell AttributeMatrix that is going to get resampled
  AttributeMatrix::Pointer origAM = dca->getPrereqAttributeMatrixFromPath(this, getCellAttributeMatrixPath(), -46500);
  if(getErrorCode() < 0)
  {
    return;
  }

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

  DataContainer::Pointer origDc = dca->getDataContainer(getCellAttributeMatrixPath().getDataContainerName());
  ImageGeom::Pointer origImageGeom = origDc->getPrereqGeometry<ImageGeom>(this);
  if(getErrorCode() < 0)
  {
    return;
  }
  FloatVec3Type origSpacing = origImageGeom->getSpacing();
  // If the spacing is the same between the origin and the new, then just bail out now because there is nothing to do.
  if(origSpacing[0] == m_Spacing[0] && origSpacing[1] == m_Spacing[1] && origSpacing[2] == m_Spacing[2])
  {
    return;
  }

  // Now make the copy of the DataContainer and adjust the Image Geometry and adjust the Cell Attribute Matrix tuple count
  DataContainer::Pointer copyDc = origDc->deepCopy(getInPreflight());

  ImageGeom::Pointer copyImageGeom = copyDc->getGeometryAs<ImageGeom>();
  // Adjust the Geometry values
  FloatVec3Type copySpacing = copyImageGeom->getSpacing();
  SizeVec3Type dims = copyImageGeom->getDimensions();

  size_t m_XP = static_cast<size_t>(((copySpacing[0] * static_cast<float>(dims[0])) / m_Spacing[0]));
  size_t m_YP = static_cast<size_t>(((copySpacing[1] * static_cast<float>(dims[1])) / m_Spacing[1]));
  size_t m_ZP = static_cast<size_t>(((copySpacing[2] * static_cast<float>(dims[2])) / m_Spacing[2]));
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
  copyImageGeom->setDimensions(m_XP, m_YP, m_ZP);
  copyImageGeom->setSpacing(m_Spacing);

  // Resize the Cell Attribute Matrix Tuple dimensions (which will resize the contained Attribute Arrays)
  size_t totalPoints = m_XP * m_YP * m_ZP;
  AttributeMatrix::Pointer copyCellAM = AttributeMatrix::New({m_XP, m_YP, m_ZP}, origAM->getName(), origAM->getType());
  copyDc->addOrReplaceAttributeMatrix(copyCellAM);
  QList<QString> voxelArrayNames = origAM->getAttributeArrayNames();
  for(QList<QString>::iterator iter = voxelArrayNames.begin(); iter != voxelArrayNames.end(); ++iter)
  {
    IDataArray::Pointer p = origAM->getAttributeArray(*iter);
    IDataArray::Pointer data = p->createNewArray(totalPoints, p->getComponentDimensions(), p->getName(), !getInPreflight());
    copyCellAM->insertOrAssign(data);
  }

  // If we are going an "In Place" resample then remove the original Data Container and just rename the new one with the updated
  // ImageGeometry and Cell Attribute Matrix
  m_PreviousDataContainer = origDc; // Save for later
  if(getSaveAsNewDataContainer())
  {
    copyDc->setName(getNewDataContainerPath().getDataContainerName());
    dca->addOrReplaceDataContainer(copyDc);
  }
  else
  {
    dca->addOrReplaceDataContainer(copyDc);
  }

#if 0
  if(getRenumberFeatures())
  {
    std::vector<size_t> cDims(1, 1);
    m_FeatureIdsPtr = getDataContainerArray()->getPrereqArrayFromPath<Int32ArrayType>(this, getFeatureIdsArrayPath(), cDims);
    if(nullptr != m_FeatureIdsPtr.lock())
    {
      m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0);
    }
  }
#endif
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ChangeResolution::execute()
{
  dataCheck();
  if(getErrorCode() < 0)
  {
    return;
  }

  DataContainerArray::Pointer dca = getDataContainerArray();

  DataContainer::Pointer origDc = m_PreviousDataContainer;
  ImageGeom::Pointer origGeom = origDc->getGeometryAs<ImageGeom>();
  AttributeMatrix::Pointer origCellAm = origDc->getAttributeMatrix(getCellAttributeMatrixPath());
  SizeVec3Type origDims = origGeom->getDimensions();
  FloatVec3Type origSpacing = origGeom->getSpacing();

  // Get the resampled data container, by default it would have been inserted into the Data Container Array
  DataContainer::Pointer copyDc = dca->getDataContainer(getCellAttributeMatrixPath());
  // Unless the user selected to create a new one
  if(getSaveAsNewDataContainer())
  {
    copyDc = dca->getDataContainer(getNewDataContainerPath());
  }
  AttributeMatrix::Pointer copyAM = copyDc->getAttributeMatrix(getCellAttributeMatrixPath().getAttributeMatrixName());

  size_t origTotalPoints = origCellAm->getNumberOfTuples();

  float x = 0.0F, y = 0.0F, z = 0.0F;
  size_t col = 0, row = 0, plane = 0;
  size_t index = 0;
  size_t index_old = 0;
  size_t progressInt = 0;
  std::vector<size_t> newindicies(origTotalPoints);

  for(size_t i = 0; i < origDims[2]; i++)
  {
    if(getCancel())
    {
      break;
    }
    progressInt = static_cast<size_t>((static_cast<float>(i) / origDims[2]) * 100.0f);
    QString ss = QObject::tr("Changing Resolution || %1% Complete").arg(progressInt);
    notifyStatusMessage(ss);
    for(size_t j = 0; j < origDims[1]; j++)
    {
      for(size_t k = 0; k < origDims[0]; k++)
      {
        x = (k * m_Spacing[0]);
        y = (j * m_Spacing[1]);
        z = (i * m_Spacing[2]);
        col = static_cast<size_t>(x / origSpacing[0]);
        row = static_cast<size_t>(y / origSpacing[1]);
        plane = static_cast<size_t>(z / origSpacing[2]);
        index_old = (plane * origDims[1] * origDims[0]) + (row * origDims[0]) + col;
        index = (i * origDims[0] * origDims[1]) + (j * origDims[0]) + k;
        newindicies[index] = index_old;
      }
    }
  }

  QString ss = QObject::tr("Copying Data...");
  notifyStatusMessage(ss);

  QList<QString> voxelArrayNames = copyAM->getAttributeArrayNames();
  for(QList<QString>::iterator iter = voxelArrayNames.begin(); iter != voxelArrayNames.end(); ++iter)
  {
    IDataArray::Pointer data = origCellAm->getAttributeArray(*iter);

    IDataArray::Pointer p = copyAM->getAttributeArray(*iter);
    // Make a copy of the 'p' array that has the same name. When placed into
    // the data container this will over write the current array with
    // the same name. At least in theory.

    void* source = nullptr;
    void* destination = nullptr;
    size_t newIndicies_I = 0;
    int32_t nComp = p->getNumberOfComponents();
    for(size_t i = 0; i < static_cast<size_t>(origTotalPoints); i++)
    {
      newIndicies_I = newindicies[i];
      source = p->getVoidPointer((nComp * newIndicies_I));
      destination = data->getVoidPointer((data->getNumberOfComponents() * i));
      ::memcpy(destination, source, p->getTypeSize() * data->getNumberOfComponents());
    }
  }

#if 0
  DataContainer::Pointer m;
  if(m_SaveAsNewDataContainer)
  {
    m = getDataContainerArray()->getDataContainer(getNewDataContainerPath());
  }
  else
  {
    m = getDataContainerArray()->getDataContainer(getCellAttributeMatrixPath().getDataContainerName());
  }

  ImageGeom::Pointer imageGeomOrig = m->getGeometryAs<ImageGeom>();

  FloatVec3Type spacingOrig = imageGeomOrig->getSpacing();

  if(spacingOrig[0] == m_Spacing[0] && spacingOrig[1] == m_Spacing[1] && spacingOrig[2] == m_Spacing[2])
  {
    return;
  }

  AttributeMatrix::Pointer cellAttrMat = m->getAttributeMatrix(getCellAttributeMatrixPath().getAttributeMatrixName());

  SizeVec3Type dims = imageGeomOrig->getDimensions();

  size_t m_XP = static_cast<size_t>(((spacingOrig[0] * static_cast<float>(dims[0])) / m_Spacing[0]));
  size_t m_YP = static_cast<size_t>(((spacingOrig[1] * static_cast<float>(dims[1])) / m_Spacing[1]));
  size_t m_ZP = static_cast<size_t>(((spacingOrig[2] * static_cast<float>(dims[2])) / m_Spacing[2]));
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
  size_t totalPoints = m_XP * m_YP * m_ZP;

  float x = 0.0f, y = 0.0f, z = 0.0f;
  size_t col = 0, row = 0, plane = 0;
  size_t index = 0;
  size_t index_old = 0;
  size_t progressInt = 0;
  std::vector<size_t> newindicies(totalPoints);

  for(size_t i = 0; i < m_ZP; i++)
  {
    if(getCancel())
    {
      break;
    }
    progressInt = static_cast<size_t>((static_cast<float>(i) / m_ZP) * 100.0f);
    QString ss = QObject::tr("Changing Resolution || %1% Complete").arg(progressInt);
    notifyStatusMessage(ss);
    for(size_t j = 0; j < m_YP; j++)
    {
      for(size_t k = 0; k < m_XP; k++)
      {
        x = (k * m_Spacing[0]);
        y = (j * m_Spacing[1]);
        z = (i * m_Spacing[2]);
        col = static_cast<size_t>(x / spacingOrig[0]);
        row = static_cast<size_t>(y / spacingOrig[1]);
        plane = static_cast<size_t>(z / spacingOrig[2]);
        index_old = (plane * dims[1] * dims[0]) + (row * dims[0]) + col;
        index = (i * m_XP * m_YP) + (j * m_XP) + k;
        newindicies[index] = index_old;
      }
    }
  }

  QString ss = QObject::tr("Copying Data...");
  notifyStatusMessage(ss);

  std::vector<size_t> tDims(3, 0);
  tDims[0] = m_XP;
  tDims[1] = m_YP;
  tDims[2] = m_ZP;
  AttributeMatrix::Pointer newCellAttrMat = AttributeMatrix::New(tDims, cellAttrMat->getName(), cellAttrMat->getType());

  QList<QString> voxelArrayNames = cellAttrMat->getAttributeArrayNames();
  for(QList<QString>::iterator iter = voxelArrayNames.begin(); iter != voxelArrayNames.end(); ++iter)
  {
    IDataArray::Pointer p = cellAttrMat->getAttributeArray(*iter);
    // Make a copy of the 'p' array that has the same name. When placed into
    // the data container this will over write the current array with
    // the same name. At least in theory.
    IDataArray::Pointer data = p->createNewArray(p->getNumberOfTuples(), p->getComponentDimensions(), p->getName());
    data->resizeTuples(totalPoints);
    void* source = nullptr;
    void* destination = nullptr;
    size_t newIndicies_I = 0;
    int32_t nComp = data->getNumberOfComponents();
    for(size_t i = 0; i < static_cast<size_t>(totalPoints); i++)
    {
      newIndicies_I = newindicies[i];
      source = p->getVoidPointer((nComp * newIndicies_I));
      destination = data->getVoidPointer((data->getNumberOfComponents() * i));
      ::memcpy(destination, source, p->getTypeSize() * data->getNumberOfComponents());
    }
    cellAttrMat->removeAttributeArray(*iter);
    newCellAttrMat->addOrReplaceAttributeArray(data);
  }

  m->getGeometryAs<ImageGeom>()->setResolution(std::make_tuple(m_Resolution.x, m_Resolution.y, m_Resolution.z));
  m->getGeometryAs<ImageGeom>()->setDimensions(std::make_tuple(m_XP, m_YP, m_ZP));
  m->removeAttributeMatrix(getCellAttributeMatrixPath().getAttributeMatrixName());
  m->addAttributeMatrix(getCellAttributeMatrixPath().getAttributeMatrixName(), newCellAttrMat);

  // Feature Ids MUST already be renumbered.
  if(m_RenumberFeatures)
  {
    totalPoints = m->getGeometryAs<ImageGeom>()->getNumberOfElements();
    AttributeMatrix::Pointer cellFeatureAttrMat = m->getAttributeMatrix(getCellFeatureAttributeMatrixPath().getAttributeMatrixName());
    size_t totalFeatures = cellFeatureAttrMat->getNumberOfTuples();
    QVector<bool> activeObjects(totalFeatures, false);
    if(0 == totalFeatures)
    {
      notifyErrorMessage(getHumanLabel(), "The number of Features is 0 and should be greater than 0", -600);
      return;
    }

    // We have blown away the old FeatureIds array during the above copy loop, so grab it again here
    IDataArray::Pointer featureIdsPtr = m->getAttributeMatrix(getCellAttributeMatrixPath().getAttributeMatrixName())->getAttributeArray(getFeatureIdsArrayPath().getDataArrayName());
    Int32ArrayType::Pointer featureIds = std::dynamic_pointer_cast<Int32ArrayType>(featureIdsPtr);
    int32_t* fIds = featureIds->getPointer(0);

    // Find the unique set of feature ids
    for(size_t i = 0; i < totalPoints; ++i)
    {
      activeObjects[fIds[i]] = true;
    }
    cellFeatureAttrMat->removeInactiveObjects(activeObjects, featureIds.get());
  
  }
#endif
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer ChangeResolution::newFilterInstance(bool copyFilterParameters) const
{
  ChangeResolution::Pointer filter = ChangeResolution::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ChangeResolution::getCompiledLibraryName() const
{
  return SamplingConstants::SamplingBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ChangeResolution::getBrandingString() const
{
  return "Sampling";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ChangeResolution::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << Sampling::Version::Major() << "." << Sampling::Version::Minor() << "." << Sampling::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ChangeResolution::getGroupName() const
{
  return SIMPL::FilterGroups::SamplingFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid ChangeResolution::getUuid() const
{
  return QUuid("{1966e540-759c-5798-ae26-0c6a3abc65c0}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ChangeResolution::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::ResolutionFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ChangeResolution::getHumanLabel() const
{
  return "Change Spacing";
}

// -----------------------------------------------------------------------------
ChangeResolution::Pointer ChangeResolution::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<ChangeResolution> ChangeResolution::New()
{
  struct make_shared_enabler : public ChangeResolution
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString ChangeResolution::getNameOfClass() const
{
  return QString("ChangeResolution");
}

// -----------------------------------------------------------------------------
QString ChangeResolution::ClassName()
{
  return QString("ChangeResolution");
}

// -----------------------------------------------------------------------------
void ChangeResolution::setNewDataContainerPath(const DataArrayPath& value)
{
  m_NewDataContainerPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath ChangeResolution::getNewDataContainerPath() const
{
  return m_NewDataContainerPath;
}

// -----------------------------------------------------------------------------
void ChangeResolution::setCellAttributeMatrixPath(const DataArrayPath& value)
{
  m_CellAttributeMatrixPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath ChangeResolution::getCellAttributeMatrixPath() const
{
  return m_CellAttributeMatrixPath;
}

// -----------------------------------------------------------------------------
void ChangeResolution::setCellFeatureAttributeMatrixPath(const DataArrayPath& value)
{
  m_CellFeatureAttributeMatrixPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath ChangeResolution::getCellFeatureAttributeMatrixPath() const
{
  return m_CellFeatureAttributeMatrixPath;
}

// -----------------------------------------------------------------------------
void ChangeResolution::setSpacing(const FloatVec3Type& value)
{
  m_Spacing = value;
}

// -----------------------------------------------------------------------------
FloatVec3Type ChangeResolution::getSpacing() const
{
  return m_Spacing;
}

// -----------------------------------------------------------------------------
void ChangeResolution::setRenumberFeatures(bool value)
{
  m_RenumberFeatures = value;
}

// -----------------------------------------------------------------------------
bool ChangeResolution::getRenumberFeatures() const
{
  return m_RenumberFeatures;
}

// -----------------------------------------------------------------------------
void ChangeResolution::setSaveAsNewDataContainer(bool value)
{
  m_SaveAsNewDataContainer = value;
}

// -----------------------------------------------------------------------------
bool ChangeResolution::getSaveAsNewDataContainer() const
{
  return m_SaveAsNewDataContainer;
}

// -----------------------------------------------------------------------------
void ChangeResolution::setFeatureIdsArrayPath(const DataArrayPath& value)
{
  m_FeatureIdsArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath ChangeResolution::getFeatureIdsArrayPath() const
{
  return m_FeatureIdsArrayPath;
}
