/* ============================================================================
* Copyright (c) 2009-2015 BlueQuartz Software, LLC
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

#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/FilterParameters/AbstractFilterParametersReader.h"
#include "DREAM3DLib/FilterParameters/AbstractFilterParametersWriter.h"

#include "DREAM3DLib/FilterParameters/FloatVec3FilterParameter.h"
#include "DREAM3DLib/FilterParameters/AttributeMatrixSelectionFilterParameter.h"
#include "DREAM3DLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "DREAM3DLib/FilterParameters/StringFilterParameter.h"
#include "DREAM3DLib/FilterParameters/LinkedBooleanFilterParameter.h"
#include "DREAM3DLib/FilterParameters/SeparatorFilterParameter.h"

#include "Sampling/SamplingConstants.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ChangeResolution::ChangeResolution() :
  AbstractFilter(),
  m_NewDataContainerName(DREAM3D::Defaults::NewImageDataContainerName),
  m_CellAttributeMatrixPath(DREAM3D::Defaults::ImageDataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, ""),
  m_CellFeatureAttributeMatrixPath(DREAM3D::Defaults::ImageDataContainerName, DREAM3D::Defaults::CellFeatureAttributeMatrixName, ""),
  m_RenumberFeatures(true),
  m_SaveAsNewDataContainer(false),
  m_FeatureIdsArrayPath(DREAM3D::Defaults::ImageDataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, DREAM3D::CellData::FeatureIds),
  m_FeatureIds(NULL)
{
  m_Resolution.x = 1.0f;
  m_Resolution.y = 1.0f;
  m_Resolution.z = 1.0f;

  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ChangeResolution::~ChangeResolution()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ChangeResolution::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(FloatVec3FilterParameter::New("Resolution", "Resolution", getResolution(), FilterParameter::Parameter));
  QStringList linkedProps;
  linkedProps << "CellFeatureAttributeMatrixPath" << "FeatureIdsArrayPath";
  parameters.push_back(LinkedBooleanFilterParameter::New("Renumber Features", "RenumberFeatures", getRenumberFeatures(), linkedProps, FilterParameter::Parameter));
  linkedProps.clear();
  linkedProps << "NewDataContainerName";
  parameters.push_back(LinkedBooleanFilterParameter::New("Save as New Data Container", "SaveAsNewDataContainer", getSaveAsNewDataContainer(), linkedProps, FilterParameter::Parameter));
  parameters.push_back(SeparatorFilterParameter::New("Cell Data", FilterParameter::RequiredArray));
  {
    AttributeMatrixSelectionFilterParameter::DataStructureRequirements req = AttributeMatrixSelectionFilterParameter::CreateRequirement(DREAM3D::AttributeMatrixType::Cell, DREAM3D::GeometryType::ImageGeometry);
    parameters.push_back(AttributeMatrixSelectionFilterParameter::New("Cell Attribute Matrix", "CellAttributeMatrixPath", getCellAttributeMatrixPath(), FilterParameter::RequiredArray, req));
  }
  {
    DataArraySelectionFilterParameter::DataStructureRequirements req = DataArraySelectionFilterParameter::CreateRequirement(DREAM3D::TypeNames::Int32, 1, DREAM3D::AttributeMatrixType::Cell, DREAM3D::GeometryType::ImageGeometry);
    parameters.push_back(DataArraySelectionFilterParameter::New("Feature Ids", "FeatureIdsArrayPath", getFeatureIdsArrayPath(), FilterParameter::RequiredArray, req));
  }
  parameters.push_back(SeparatorFilterParameter::New("Cell Feature Data", FilterParameter::RequiredArray));
  {
    AttributeMatrixSelectionFilterParameter::DataStructureRequirements req = AttributeMatrixSelectionFilterParameter::CreateRequirement(DREAM3D::AttributeMatrixType::CellFeature, DREAM3D::GeometryType::ImageGeometry);
    parameters.push_back(AttributeMatrixSelectionFilterParameter::New("Cell Feature Attribute Matrix", "CellFeatureAttributeMatrixPath", getCellFeatureAttributeMatrixPath(), FilterParameter::RequiredArray, req));
  }
  parameters.push_back(StringFilterParameter::New("Data Container", "NewDataContainerName", getNewDataContainerName(), FilterParameter::CreatedArray));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ChangeResolution::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setNewDataContainerName( reader->readString("NewDataContainerName", getNewDataContainerName() ) );
  setCellAttributeMatrixPath( reader->readDataArrayPath("CellAttributeMatrixPath", getCellAttributeMatrixPath() ) );
  setCellFeatureAttributeMatrixPath( reader->readDataArrayPath("CellFeatureAttributeMatrixPath", getCellFeatureAttributeMatrixPath() ) );
  setFeatureIdsArrayPath(reader->readDataArrayPath("FeatureIdsArrayPath", getFeatureIdsArrayPath() ) );
  setResolution( reader->readFloatVec3("Resolution", getResolution() ) );
  setRenumberFeatures( reader->readValue("RenumberFeatures", getRenumberFeatures()) );
  setSaveAsNewDataContainer( reader->readValue("SaveAsNewDataContainer", getSaveAsNewDataContainer()) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int ChangeResolution::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  DREAM3D_FILTER_WRITE_PARAMETER(FilterVersion)
  DREAM3D_FILTER_WRITE_PARAMETER(NewDataContainerName)
  DREAM3D_FILTER_WRITE_PARAMETER(CellAttributeMatrixPath)
  DREAM3D_FILTER_WRITE_PARAMETER(CellFeatureAttributeMatrixPath)
  DREAM3D_FILTER_WRITE_PARAMETER(FeatureIdsArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(Resolution)
  DREAM3D_FILTER_WRITE_PARAMETER(RenumberFeatures)
  DREAM3D_FILTER_WRITE_PARAMETER(SaveAsNewDataContainer)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ChangeResolution::dataCheck()
{
  setErrorCondition(0);

  if (getResolution().x <= 0)
  {
    QString ss = QObject::tr("The X resolution (%1) must be positive").arg(getResolution().x);
    setErrorCondition(-5555);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

  if (getResolution().y <= 0)
  {
    QString ss = QObject::tr("The Y resolution (%1) must be positive").arg(getResolution().y);
    setErrorCondition(-5556);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

  if (getResolution().z <= 0)
  {
    QString ss = QObject::tr("The  resolution (%1) must be positive").arg(getResolution().z);
    setErrorCondition(-5557);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

  if (getSaveAsNewDataContainer() == false)
  {
    getDataContainerArray()->getPrereqGeometryFromDataContainer<ImageGeom, AbstractFilter>(this, getCellAttributeMatrixPath().getDataContainerName());
  }
  else
  {
    getDataContainerArray()->duplicateDataContainer(getCellAttributeMatrixPath().getDataContainerName(), getNewDataContainerName());
    getDataContainerArray()->getPrereqGeometryFromDataContainer<ImageGeom, AbstractFilter>(this, getCellAttributeMatrixPath().getDataContainerName());
  }

  getDataContainerArray()->getPrereqAttributeMatrixFromPath<AbstractFilter>(this, getCellAttributeMatrixPath(), -301);

  if (getRenumberFeatures() == true)
  {
    QVector<size_t> cDims(1, 1);
    m_FeatureIdsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getFeatureIdsArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if( NULL != m_FeatureIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    { m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
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

  if (getErrorCondition() < 0)
  {
    emit preflightExecuted();
    setInPreflight(false);
    return;
  }

  DataContainer::Pointer m;
  if (m_SaveAsNewDataContainer == false)
  {
    m = getDataContainerArray()->getDataContainer(getCellAttributeMatrixPath().getDataContainerName());
  }
  else
  {
    m = getDataContainerArray()->getDataContainer(getNewDataContainerName());
  }

  size_t dims[3] = { 0, 0, 0 };
  m->getGeometryAs<ImageGeom>()->getDimensions(dims);

  float sizex = (dims[0]) * m->getGeometryAs<ImageGeom>()->getXRes();
  float sizey = (dims[1]) * m->getGeometryAs<ImageGeom>()->getYRes();
  float sizez = (dims[2]) * m->getGeometryAs<ImageGeom>()->getZRes();
  size_t m_XP = size_t(sizex / m_Resolution.x);
  size_t m_YP = size_t(sizey / m_Resolution.y);
  size_t m_ZP = size_t(sizez / m_Resolution.z);
  if (m_XP == 0) { m_XP = 1; }
  if (m_YP == 0) { m_YP = 1; }
  if (m_ZP == 0) { m_ZP = 1; }

  m->getGeometryAs<ImageGeom>()->setDimensions(m_XP, m_YP, m_ZP);
  m->getGeometryAs<ImageGeom>()->setResolution(m_Resolution.x, m_Resolution.y, m_Resolution.z);

  QVector<size_t> tDims(3, 0);
  tDims[0] = m_XP;
  tDims[1] = m_YP;
  tDims[2] = m_ZP;
  m->getAttributeMatrix(getCellAttributeMatrixPath().getAttributeMatrixName())->setTupleDimensions(tDims);

  if (m_RenumberFeatures == true)
  {
    AttributeMatrix::Pointer cellFeatureAttrMat = m->getAttributeMatrix(getCellFeatureAttributeMatrixPath().getAttributeMatrixName());
    if (NULL != cellFeatureAttrMat.get())
    {
      QVector<bool> activeObjects(cellFeatureAttrMat->getNumTuples(), true);
      cellFeatureAttrMat->removeInactiveObjects(activeObjects, m_FeatureIdsPtr.lock());
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
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  DataContainer::Pointer m;
  if (m_SaveAsNewDataContainer == false)
  {
    m = getDataContainerArray()->getDataContainer(getCellAttributeMatrixPath().getDataContainerName());
  }
  else
  {
    m = getDataContainerArray()->getDataContainer(getNewDataContainerName());
  }

  if(m->getGeometryAs<ImageGeom>()->getXRes() == m_Resolution.x
      && m->getGeometryAs<ImageGeom>()->getYRes() == m_Resolution.y
      && m->getGeometryAs<ImageGeom>()->getZRes() == m_Resolution.z)
  {
    return;
  }

  AttributeMatrix::Pointer cellAttrMat = m->getAttributeMatrix(getCellAttributeMatrixPath().getAttributeMatrixName());

  size_t dims[3] = { 0, 0, 0 };
  m->getGeometryAs<ImageGeom>()->getDimensions(dims);

  float sizex = (dims[0]) * m->getGeometryAs<ImageGeom>()->getXRes();
  float sizey = (dims[1]) * m->getGeometryAs<ImageGeom>()->getYRes();
  float sizez = (dims[2]) * m->getGeometryAs<ImageGeom>()->getZRes();
  size_t m_XP = size_t(sizex / m_Resolution.x);
  size_t m_YP = size_t(sizey / m_Resolution.y);
  size_t m_ZP = size_t(sizez / m_Resolution.z);
  if (m_XP == 0) { m_XP = 1; }
  if (m_YP == 0) { m_YP = 1; }
  if (m_ZP == 0) { m_ZP = 1; }
  size_t totalPoints = m_XP * m_YP * m_ZP;

  float x = 0.0f, y = 0.0f, z = 0.0f;
  size_t col = 0, row = 0, plane = 0;
  size_t index;
  size_t index_old;
  std::vector<size_t> newindicies(totalPoints);

  for (size_t i = 0; i < m_ZP; i++)
  {
    QString ss = QObject::tr("Changing Resolution - %1 Percent Complete").arg(((float)i / m->getGeometryAs<ImageGeom>()->getZPoints()) * 100);
    notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);
    for (size_t j = 0; j < m_YP; j++)
    {
      for (size_t k = 0; k < m_XP; k++)
      {
        x = (k * m_Resolution.x);
        y = (j * m_Resolution.y);
        z = (i * m_Resolution.z);
        col = size_t(x / m->getGeometryAs<ImageGeom>()->getXRes());
        row = size_t(y / m->getGeometryAs<ImageGeom>()->getYRes());
        plane = size_t(z / m->getGeometryAs<ImageGeom>()->getZRes());
        index_old = (plane * m->getGeometryAs<ImageGeom>()->getXPoints() * m->getGeometryAs<ImageGeom>()->getYPoints()) + (row * m->getGeometryAs<ImageGeom>()->getXPoints()) + col;
        index = (i * m_XP * m_YP) + (j * m_XP) + k;
        newindicies[index] = index_old;
      }
    }
  }

  QVector<size_t> tDims(3, 0);
  tDims[0] = m_XP;
  tDims[1] = m_YP;
  tDims[2] = m_ZP;
  AttributeMatrix::Pointer newCellAttrMat = AttributeMatrix::New(tDims, cellAttrMat->getName(), cellAttrMat->getType());

  QList<QString> voxelArrayNames = cellAttrMat->getAttributeArrayNames();
  for (QList<QString>::iterator iter = voxelArrayNames.begin(); iter != voxelArrayNames.end(); ++iter)
  {
    IDataArray::Pointer p = cellAttrMat->getAttributeArray(*iter);
    // Make a copy of the 'p' array that has the same name. When placed into
    // the data container this will over write the current array with
    // the same name. At least in theory.
    IDataArray::Pointer data = p->createNewArray(p->getNumberOfTuples(), p->getComponentDimensions(), p->getName());
    data->resize(totalPoints);
    void* source = NULL;
    void* destination = NULL;
    size_t newIndicies_I = 0;
    int32_t nComp = data->getNumberOfComponents();
    for (size_t i = 0; i < static_cast<size_t>(totalPoints); i++)
    {
      newIndicies_I = newindicies[i];
      source = p->getVoidPointer((nComp * newIndicies_I));
      destination = data->getVoidPointer((data->getNumberOfComponents() * i));
      ::memcpy(destination, source, p->getTypeSize() * data->getNumberOfComponents());
    }
    cellAttrMat->removeAttributeArray(*iter);
    newCellAttrMat->addAttributeArray(*iter, data);
  }
  m->getGeometryAs<ImageGeom>()->setResolution(m_Resolution.x, m_Resolution.y, m_Resolution.z);
  m->getGeometryAs<ImageGeom>()->setDimensions(m_XP, m_YP, m_ZP);
  m->removeAttributeMatrix(getCellAttributeMatrixPath().getAttributeMatrixName());
  m->addAttributeMatrix(getCellAttributeMatrixPath().getAttributeMatrixName(), newCellAttrMat);
  
  // Feature Ids MUST already be renumbered.
  if (m_RenumberFeatures == true)
  {
    totalPoints = m->getGeometryAs<ImageGeom>()->getNumberOfElements();
    AttributeMatrix::Pointer cellFeatureAttrMat = m->getAttributeMatrix(getCellFeatureAttributeMatrixPath().getAttributeMatrixName());
    size_t totalFeatures = cellFeatureAttrMat->getNumTuples();
    QVector<bool> activeObjects(totalFeatures, false);
    if (0 == totalFeatures)
    {
      notifyErrorMessage(getHumanLabel(), "The number of Features is 0 and should be greater than 0", -600);
      return;
    }
    
    // We have blown away the old FeatureIds array during the above copy loop, so grab it again here
    IDataArray::Pointer featureIdsPtr = m->getAttributeMatrix(getCellAttributeMatrixPath().getAttributeMatrixName())->getAttributeArray(getFeatureIdsArrayPath().getDataArrayName());
    Int32ArrayType::Pointer featureIds = boost::dynamic_pointer_cast<Int32ArrayType>(featureIdsPtr);
    int32_t* fIds = featureIds->getPointer(0);
    
    // Find the unique set of feature ids
    for (size_t i = 0; i < totalPoints; ++i)
    {
      activeObjects[fIds[i]] = true;
    }
    cellFeatureAttrMat->removeInactiveObjects(activeObjects, featureIds);
  }

  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer ChangeResolution::newFilterInstance(bool copyFilterParameters)
{
  ChangeResolution::Pointer filter = ChangeResolution::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ChangeResolution::getCompiledLibraryName()
{ return SamplingConstants::SamplingBaseName; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ChangeResolution::getGroupName()
{ return DREAM3D::FilterGroups::SamplingFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ChangeResolution::getSubGroupName()
{ return DREAM3D::FilterSubGroups::ResolutionFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ChangeResolution::getHumanLabel()
{ return "Change Resolution"; }

