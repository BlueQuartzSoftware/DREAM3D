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

#include "ChangeResolution.h"

#include <QtCore/QTextStream>

#include "SIMPLib/Common/Constants.h"

#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/AttributeMatrixSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/DataContainerCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/FloatVec3FilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedBooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/Geometry/ImageGeom.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/DataContainers/DataContainer.h"

#include "Sampling/SamplingConstants.h"
#include "Sampling/SamplingVersion.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ChangeResolution::ChangeResolution()
: m_NewDataContainerName(SIMPL::Defaults::NewImageDataContainerName)
, m_CellAttributeMatrixPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellAttributeMatrixName, "")
, m_CellFeatureAttributeMatrixPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellFeatureAttributeMatrixName, "")
, m_RenumberFeatures(true)
, m_SaveAsNewDataContainer(false)
, m_FeatureIdsArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellAttributeMatrixName, SIMPL::CellData::FeatureIds)
{
  m_Spacing[0] = 1.0f;
  m_Spacing[1] = 1.0f;
  m_Spacing[2] = 1.0f;
}

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

  QStringList linkedProps;
  linkedProps << "CellFeatureAttributeMatrixPath"
              << "FeatureIdsArrayPath";
  parameters.push_back(SIMPL_NEW_LINKED_BOOL_FP("Renumber Features", RenumberFeatures, FilterParameter::Parameter, ChangeResolution, linkedProps));
  linkedProps.clear();
  linkedProps << "NewDataContainerName";
  parameters.push_back(SIMPL_NEW_LINKED_BOOL_FP("Save as New Data Container", SaveAsNewDataContainer, FilterParameter::Parameter, ChangeResolution, linkedProps));
  parameters.push_back(SeparatorFilterParameter::New("Cell Data", FilterParameter::RequiredArray));
  {
    AttributeMatrixSelectionFilterParameter::RequirementType req = AttributeMatrixSelectionFilterParameter::CreateRequirement(AttributeMatrix::Type::Cell, IGeometry::Type::Image);
    parameters.push_back(SIMPL_NEW_AM_SELECTION_FP("Cell Attribute Matrix", CellAttributeMatrixPath, FilterParameter::RequiredArray, ChangeResolution, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req =
        DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Int32, 1, AttributeMatrix::Type::Cell, IGeometry::Type::Image);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Feature Ids", FeatureIdsArrayPath, FilterParameter::RequiredArray, ChangeResolution, req));
  }
  parameters.push_back(SeparatorFilterParameter::New("Cell Feature Data", FilterParameter::RequiredArray));
  {
    AttributeMatrixSelectionFilterParameter::RequirementType req =
        AttributeMatrixSelectionFilterParameter::CreateRequirement(AttributeMatrix::Type::CellFeature, IGeometry::Type::Image);
    parameters.push_back(SIMPL_NEW_AM_SELECTION_FP("Cell Feature Attribute Matrix", CellFeatureAttributeMatrixPath, FilterParameter::RequiredArray, ChangeResolution, req));
  }
  parameters.push_back(SIMPL_NEW_DC_CREATION_FP("Data Container", NewDataContainerName, FilterParameter::CreatedArray, ChangeResolution));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ChangeResolution::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setNewDataContainerName(reader->readDataArrayPath("NewDataContainerName", getNewDataContainerName()));
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

  if(getSpacing()[0] <= 0)
  {
    QString ss = QObject::tr("The X spacing (%1) must be positive").arg(getSpacing()[0]);
    setErrorCondition(-5555, ss);
  }

  if(getSpacing()[1] <= 0)
  {
    QString ss = QObject::tr("The Y spacing (%1) must be positive").arg(getSpacing()[1]);
    setErrorCondition(-5556, ss);
  }

  if(getSpacing()[2] <= 0)
  {
    QString ss = QObject::tr("The  spacing (%1) must be positive").arg(getSpacing()[2]);
    setErrorCondition(-5557, ss);
  }

  if(!getSaveAsNewDataContainer())
  {
    getDataContainerArray()->getPrereqGeometryFromDataContainer<ImageGeom, AbstractFilter>(this, getCellAttributeMatrixPath().getDataContainerName());
  }
  else
  {
    getDataContainerArray()->duplicateDataContainer(getCellAttributeMatrixPath().getDataContainerName(), getNewDataContainerName().getDataContainerName());
    getDataContainerArray()->getPrereqGeometryFromDataContainer<ImageGeom, AbstractFilter>(this, getCellAttributeMatrixPath().getDataContainerName());
  }

  getDataContainerArray()->getPrereqAttributeMatrixFromPath<AbstractFilter>(this, getCellAttributeMatrixPath(), -301);

  if(getRenumberFeatures())
  {
    std::vector<size_t> cDims(1, 1);
    m_FeatureIdsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getFeatureIdsArrayPath(),
                                                                                                          cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if(nullptr != m_FeatureIdsPtr.lock())                                                                         /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
    {
      m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0);
    } /* Now assign the raw pointer to data from the DataArray<T> object */
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ChangeResolution::preflight()
{
  setInPreflight(true);
  emit preflightAboutToExecute();
  emit updateFilterParameters(this);
  dataCheck();

  if(getErrorCode() < 0)
  {
    emit preflightExecuted();
    setInPreflight(false);
    return;
  }

  DataContainer::Pointer m;
  if(!m_SaveAsNewDataContainer)
  {
    m = getDataContainerArray()->getDataContainer(getCellAttributeMatrixPath().getDataContainerName());
  }
  else
  {
    m = getDataContainerArray()->getDataContainer(getNewDataContainerName());
  }

  ImageGeom::Pointer image = m->getGeometryAs<ImageGeom>();
  SizeVec3Type dims = image->getDimensions();

  FloatVec3Type spacing = m->getGeometryAs<ImageGeom>()->getSpacing();
  spacing[0] *= static_cast<float>(dims[0]);
  spacing[1] *= static_cast<float>(dims[1]);
  spacing[2] *= static_cast<float>(dims[2]);

  size_t m_XP = size_t(spacing[0] / m_Spacing[0]);
  size_t m_YP = size_t(spacing[1] / m_Spacing[1]);
  size_t m_ZP = size_t(spacing[2] / m_Spacing[2]);
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

  image->setDimensions(std::make_tuple(m_XP, m_YP, m_ZP));
  image->setSpacing(std::make_tuple(m_Spacing[0], m_Spacing[1], m_Spacing[2]));

  std::vector<size_t> tDims(3, 0);
  tDims[0] = m_XP;
  tDims[1] = m_YP;
  tDims[2] = m_ZP;
  // m->getAttributeMatrix(getCellAttributeMatrixPath().getAttributeMatrixName())->setTupleDimensions(tDims);

  AttributeMatrix::Pointer cellAttrMat = m->getAttributeMatrix(getCellAttributeMatrixPath().getAttributeMatrixName());

  size_t totalPoints = 1;
  for(int i = 0; i < 3; i++)
  {
    if(tDims[i] != 0)
    {
      totalPoints *= tDims[i];
    }
  }
  AttributeMatrix::Pointer newCellAttrMat = AttributeMatrix::New(tDims, cellAttrMat->getName(), cellAttrMat->getType());

  QList<QString> voxelArrayNames = cellAttrMat->getAttributeArrayNames();
  for(QList<QString>::iterator iter = voxelArrayNames.begin(); iter != voxelArrayNames.end(); ++iter)
  {
    IDataArray::Pointer p = cellAttrMat->getAttributeArray(*iter);
    //
    IDataArray::Pointer data = p->createNewArray(totalPoints, p->getComponentDimensions(), p->getName(), false);

    cellAttrMat->removeAttributeArray(*iter);
    newCellAttrMat->insertOrAssign(data);
  }
  m->removeAttributeMatrix(getCellAttributeMatrixPath().getAttributeMatrixName());
  m->addOrReplaceAttributeMatrix(newCellAttrMat);

  if(m_RenumberFeatures)
  {
    AttributeMatrix::Pointer cellFeatureAttrMat = m->getAttributeMatrix(getCellFeatureAttributeMatrixPath().getAttributeMatrixName());
    if(nullptr != cellFeatureAttrMat.get())
    {
      QVector<bool> activeObjects(cellFeatureAttrMat->getNumberOfTuples(), true);
      cellFeatureAttrMat->removeInactiveObjects(activeObjects, m_FeatureIdsPtr.lock().get());
    }
  }
  emit preflightExecuted();
  setInPreflight(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ChangeResolution::execute()
{
  clearErrorCode();
  clearWarningCode();
  dataCheck();
  if(getErrorCode() < 0)
  {
    return;
  }

  DataContainer::Pointer m;
  if(!m_SaveAsNewDataContainer)
  {
    m = getDataContainerArray()->getDataContainer(getCellAttributeMatrixPath().getDataContainerName());
  }
  else
  {
    m = getDataContainerArray()->getDataContainer(getNewDataContainerName());
  }
  FloatVec3Type spacing = m->getGeometryAs<ImageGeom>()->getSpacing();

  if(spacing[0] == m_Spacing[0] && spacing[1] == m_Spacing[1] && spacing[2] == m_Spacing[2])
  {
    return;
  }

  AttributeMatrix::Pointer cellAttrMat = m->getAttributeMatrix(getCellAttributeMatrixPath().getAttributeMatrixName());

  SizeVec3Type dims = m->getGeometryAs<ImageGeom>()->getDimensions();

  spacing[0] *= static_cast<float>(dims[0]);
  spacing[1] *= static_cast<float>(dims[1]);
  spacing[2] *= static_cast<float>(dims[2]);

  size_t m_XP = size_t(spacing[0] / m_Spacing[0]);
  size_t m_YP = size_t(spacing[1] / m_Spacing[1]);
  size_t m_ZP = size_t(spacing[2] / m_Spacing[2]);
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
  FloatVec3Type res = m->getGeometryAs<ImageGeom>()->getSpacing();

  for(size_t i = 0; i < m_ZP; i++)
  {
    if(getCancel())
    {
      break;
    }
    progressInt = static_cast<size_t>((static_cast<float>(i) / m_ZP) * 100.0f);
    QString ss = QObject::tr("Changing Spacing || %1% Complete").arg(progressInt);
    notifyStatusMessage(ss);
    for(size_t j = 0; j < m_YP; j++)
    {
      for(size_t k = 0; k < m_XP; k++)
      {
        x = (k * m_Spacing[0]);
        y = (j * m_Spacing[1]);
        z = (i * m_Spacing[2]);
        col = size_t(x / res[0]);
        row = size_t(y / res[1]);
        plane = size_t(z / res[2]);
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
    newCellAttrMat->insertOrAssign(data);
  }
  m->getGeometryAs<ImageGeom>()->setSpacing(std::make_tuple(m_Spacing[0], m_Spacing[1], m_Spacing[2]));
  m->getGeometryAs<ImageGeom>()->setDimensions(std::make_tuple(m_XP, m_YP, m_ZP));
  m->removeAttributeMatrix(getCellAttributeMatrixPath().getAttributeMatrixName());
  m->addOrReplaceAttributeMatrix(newCellAttrMat);

  // Feature Ids MUST already be renumbered.
  if(m_RenumberFeatures)
  {
    totalPoints = m->getGeometryAs<ImageGeom>()->getNumberOfElements();
    AttributeMatrix::Pointer cellFeatureAttrMat = m->getAttributeMatrix(getCellFeatureAttributeMatrixPath().getAttributeMatrixName());
    size_t totalFeatures = cellFeatureAttrMat->getNumberOfTuples();
    QVector<bool> activeObjects(totalFeatures, false);
    if(0 == totalFeatures)
    {
      setErrorCondition(-600, "The number of Features is 0 and should be greater than 0");
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
void ChangeResolution::setNewDataContainerName(const DataArrayPath& value)
{
  m_NewDataContainerName = value;
}

// -----------------------------------------------------------------------------
DataArrayPath ChangeResolution::getNewDataContainerName() const
{
  return m_NewDataContainerName;
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
