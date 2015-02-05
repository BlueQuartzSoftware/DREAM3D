/* ============================================================================
 * Copyright (c) 2014 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2014 Dr. Michael A. Groeber (US Air Force Research Laboratories)
 * Copyright (c) 2014 Dr. Joseph C. Tucker (UES, Inc.)
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

#include "OrientationBinModeFilter.h"

#include <QtCore/QMap>


#include "DREAM3DLib/Common/Constants.h"

#include "DREAM3DLib/Utilities/DREAM3DRandom.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
OrientationBinModeFilter::OrientationBinModeFilter() :
  AbstractFilter(),
  m_NewDataContainerName(DREAM3D::Defaults::NewDataContainerName),
  m_CellAttributeMatrixPath(DREAM3D::Defaults::DataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, ""),
  m_CellFeatureAttributeMatrixPath(DREAM3D::Defaults::DataContainerName, DREAM3D::Defaults::CellFeatureAttributeMatrixName, ""),
  m_RenumberFeatures(true),
  m_SaveAsNewDataContainer(false),
  m_FeatureIdsArrayPath(DREAM3D::Defaults::DataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, DREAM3D::CellData::FeatureIds),
  m_FeatureIdsArrayName(DREAM3D::CellData::FeatureIds),
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
OrientationBinModeFilter::~OrientationBinModeFilter()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationBinModeFilter::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(FilterParameter::New("Cell Attribute Matrix", "CellAttributeMatrixPath", FilterParameterWidgetType::AttributeMatrixSelectionWidget, getCellAttributeMatrixPath(), false));
  parameters.push_back(FilterParameter::New("Resolution", "Resolution", FilterParameterWidgetType::FloatVec3Widget, getResolution(), false, "Microns"));
  QStringList linkedProps;
  linkedProps << "CellFeatureAttributeMatrixPath" << "FeatureIdsArrayPath";
  parameters.push_back(LinkedBooleanFilterParameter::New("Renumber Features", "RenumberFeatures", getRenumberFeatures(), linkedProps, false));
  linkedProps.clear();
  linkedProps << "NewDataContainerName";
  parameters.push_back(LinkedBooleanFilterParameter::New("Save As New Data Container", "SaveAsNewDataContainer", getSaveAsNewDataContainer(), linkedProps, false));
  parameters.push_back(FilterParameter::New("Required Information", "", FilterParameterWidgetType::SeparatorWidget, "", true));
  parameters.push_back(FilterParameter::New("Cell Feature Attribute Matrix", "CellFeatureAttributeMatrixPath", FilterParameterWidgetType::AttributeMatrixSelectionWidget, getCellFeatureAttributeMatrixPath(), true));
  parameters.push_back(FilterParameter::New("FeatureIds", "FeatureIdsArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getFeatureIdsArrayPath(), true, ""));
  parameters.push_back(FilterParameter::New("Created Information", "", FilterParameterWidgetType::SeparatorWidget, "", true));
  parameters.push_back(FilterParameter::New("New Data Container Name", "NewDataContainerName", FilterParameterWidgetType::StringWidget, getNewDataContainerName(), true));
  setFilterParameters(parameters);
}


// -----------------------------------------------------------------------------
void OrientationBinModeFilter::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setNewDataContainerName( reader->readString("NewDataContainerName", getNewDataContainerName() ) );
  setCellAttributeMatrixPath( reader->readDataArrayPath("CellAttributeMatrixPath", getCellAttributeMatrixPath() ) );
  setCellFeatureAttributeMatrixPath( reader->readDataArrayPath("CellFeatureAttributeMatrixPath", getCellFeatureAttributeMatrixPath() ) );
  setFeatureIdsArrayPath(reader->readDataArrayPath("FeatureIdsArrayPath", getFeatureIdsArrayPath() ) );
  setResolution( reader->readFloatVec3("Resolution", getResolution() ) );
  setRenumberFeatures( reader->readValue("RenumberFeatures", false) );
  setSaveAsNewDataContainer( reader->readValue("SaveAsNewDataContainer", false) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int OrientationBinModeFilter::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  DREAM3D_FILTER_WRITE_PARAMETER(NewDataContainerName)
  DREAM3D_FILTER_WRITE_PARAMETER(CellAttributeMatrixPath)
  DREAM3D_FILTER_WRITE_PARAMETER(CellFeatureAttributeMatrixPath)
  DREAM3D_FILTER_WRITE_PARAMETER(FeatureIdsArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(Resolution)
  DREAM3D_FILTER_WRITE_PARAMETER(RenumberFeatures)
  DREAM3D_FILTER_WRITE_PARAMETER(SaveAsNewDataContiner)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationBinModeFilter::updateCellInstancePointers()
{
  setErrorCondition(0);

  if( NULL != m_FeatureIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationBinModeFilter::dataCheck()
{
  DataContainer::Pointer m;
  if(m_SaveAsNewDataContainer == false) { m = getDataContainerArray()->getPrereqDataContainer<AbstractFilter>(this, getCellAttributeMatrixPath().getDataContainerName()); }
  else
  {
    getDataContainerArray()->duplicateDataContainer(getCellAttributeMatrixPath().getDataContainerName(), getNewDataContainerName());
    m = getDataContainerArray()->getPrereqDataContainer<AbstractFilter>(this, getNewDataContainerName());
  }
  if(getErrorCondition() < 0 || NULL == m) { return; }

  if (m_RenumberFeatures == true)
  {
    QVector<size_t> dims(1, 1);
    m_FeatureIdsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getFeatureIdsArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if( NULL != m_FeatureIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    { m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationBinModeFilter::preflight()
{
  setInPreflight(true);
  emit preflightAboutToExecute();
  emit updateFilterParameters(this);
  dataCheck();
  emit preflightExecuted();

  if(getErrorCondition() < 0) { return; }

  DataContainer::Pointer m;
  if(m_SaveAsNewDataContainer == false) { m = getDataContainerArray()->getDataContainer(getCellAttributeMatrixPath().getDataContainerName()); }
  else { m = getDataContainerArray()->getDataContainer(getNewDataContainerName()); }

  size_t dims[3];
  /* FIXME: ImageGeom */ m->getGeometryAs<ImageGeom>()->getDimensions(dims);

  float sizex = (dims[0]) * /* FIXME: ImageGeom */ m->getGeometryAs<ImageGeom>()->getXRes();
  float sizey = (dims[1]) * /* FIXME: ImageGeom */ m->getGeometryAs<ImageGeom>()->getYRes();
  float sizez = (dims[2]) * /* FIXME: ImageGeom */ m->getGeometryAs<ImageGeom>()->getZRes();
  int m_XP = int(sizex / m_Resolution.x);
  int m_YP = int(sizey / m_Resolution.y);
  int m_ZP = int(sizez / m_Resolution.z);

  /* FIXME: ImageGeom */ m->getGeometryAs<ImageGeom>()->setDimensions(m_XP, m_YP, m_ZP);
  /* FIXME: ImageGeom */ m->getGeometryAs<ImageGeom>()->setResolution(m_Resolution.x, m_Resolution.y, m_Resolution.z);

  QVector<size_t> tDims(3, 0);
  tDims[0] = m_XP;
  tDims[1] = m_YP;
  tDims[2] = m_ZP;
  m->getAttributeMatrix(getCellAttributeMatrixPath().getAttributeMatrixName())->setTupleDimensions(tDims);

  if(m_RenumberFeatures == true)
  {
    AttributeMatrix::Pointer cellFeatureAttrMat = m->getAttributeMatrix(getCellFeatureAttributeMatrixPath().getAttributeMatrixName());
    QVector<bool> activeObjects(cellFeatureAttrMat->getNumTuples(), true);
    cellFeatureAttrMat->removeInactiveObjects(activeObjects, m_FeatureIdsPtr.lock());
  }
  setInPreflight(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationBinModeFilter::execute()
{
  int err = 0;
  setErrorCondition(err);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  DREAM3D_RANDOMNG_NEW()

  DataContainer::Pointer m;
  if(m_SaveAsNewDataContainer == false) { m = getDataContainerArray()->getDataContainer(getCellAttributeMatrixPath().getDataContainerName()); }
  else { m = getDataContainerArray()->getDataContainer(getNewDataContainerName()); }

  if(/* FIXME: ImageGeom */ m->getGeometryAs<ImageGeom>()->getXRes() == m_Resolution.x
      && /* FIXME: ImageGeom */ m->getGeometryAs<ImageGeom>()->getYRes() == m_Resolution.y
      && /* FIXME: ImageGeom */ m->getGeometryAs<ImageGeom>()->getZRes() == m_Resolution.z)
  {
    return;
  }

  AttributeMatrix::Pointer cellAttrMat = m->getAttributeMatrix(getCellAttributeMatrixPath().getAttributeMatrixName());

  size_t dims[3];
  /* FIXME: ImageGeom */ m->getGeometryAs<ImageGeom>()->getDimensions(dims);

  float sizex = (dims[0]) * /* FIXME: ImageGeom */ m->getGeometryAs<ImageGeom>()->getXRes();
  float sizey = (dims[1]) * /* FIXME: ImageGeom */ m->getGeometryAs<ImageGeom>()->getYRes();
  float sizez = (dims[2]) * /* FIXME: ImageGeom */ m->getGeometryAs<ImageGeom>()->getZRes();
  int m_XP = int(sizex / m_Resolution.x);
  int m_YP = int(sizey / m_Resolution.y);
  int m_ZP = int(sizez / m_Resolution.z);
  int64_t totalPoints = m_XP * m_YP * m_ZP;

  float x, y, z;
  int col, row, plane;
  int index;
  int index_old;
  QVector<size_t> newindicies;
  newindicies.resize(totalPoints);
  for (int i = 0; i < m_ZP; i++)
  {
    QString ss = QObject::tr("Changing Resolution - %1 Percent Complete").arg(((float)i / /* FIXME: ImageGeom */ m->getGeometryAs<ImageGeom>()->getZPoints()) * 100);
    notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);
    for (int j = 0; j < m_YP; j++)
    {
      for (int k = 0; k < m_XP; k++)
      {
        x = (k * m_Resolution.x);
        y = (j * m_Resolution.y);
        z = (i * m_Resolution.z);
        col = int(x / /* FIXME: ImageGeom */ m->getGeometryAs<ImageGeom>()->getXRes());
        row = int(y / /* FIXME: ImageGeom */ m->getGeometryAs<ImageGeom>()->getYRes());
        plane = int(z / /* FIXME: ImageGeom */ m->getGeometryAs<ImageGeom>()->getZRes());
        index_old = (plane * /* FIXME: ImageGeom */ m->getGeometryAs<ImageGeom>()->getXPoints() * /* FIXME: ImageGeom */ m->getGeometryAs<ImageGeom>()->getYPoints()) + (row * /* FIXME: ImageGeom */ m->getGeometryAs<ImageGeom>()->getXPoints()) + col;
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

  QList<QString> voxelArrayNames = cellAttrMat->getAttributeArrayNameList();
  for (QList<QString>::iterator iter = voxelArrayNames.begin(); iter != voxelArrayNames.end(); ++iter)
  {
    QString name = *iter;
    IDataArray::Pointer p = cellAttrMat->getAttributeArray(*iter);
    // Make a copy of the 'p' array that has the same name. When placed into
    // the data container this will over write the current array with
    // the same name. At least in theory
    IDataArray::Pointer data = p->createNewArray(p->getNumberOfTuples(), p->getComponentDimensions(), p->getName());
    data->resize(totalPoints);
    void* source = NULL;
    void* destination = NULL;
    size_t newIndicies_I = 0;
    int nComp = data->getNumberOfComponents();
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
  /* FIXME: ImageGeom */ m->getGeometryAs<ImageGeom>()->setResolution(m_Resolution.x, m_Resolution.y, m_Resolution.z);
  /* FIXME: ImageGeom */ m->getGeometryAs<ImageGeom>()->setDimensions(m_XP, m_YP, m_ZP);
  m->removeAttributeMatrix(getCellAttributeMatrixPath().getAttributeMatrixName());
  m->addAttributeMatrix(getCellAttributeMatrixPath().getAttributeMatrixName(), newCellAttrMat);

  // Feature Ids MUST already be renumbered.
  if (m_RenumberFeatures == true)
  {
    totalPoints = /* FIXME: ImageGeom */ m->getGeometryAs<ImageGeom>()->getNumberOfTuples();
    AttributeMatrix::Pointer cellFeatureAttrMat = m->getAttributeMatrix(getCellFeatureAttributeMatrixPath().getAttributeMatrixName());
    size_t totalFeatures = cellFeatureAttrMat->getNumTuples();
    QVector<bool> activeObjects(totalFeatures, false);
    if (0 == totalFeatures)
    {
      notifyErrorMessage(getHumanLabel(), "The number of features is Zero and should be greater than Zero", -600);
      notifyStatusMessage(getHumanLabel(), "Completed");
      return;
    }

    updateCellInstancePointers();

    // Find the unique set of feature ids
    for (int64_t i = 0; i < totalPoints; ++i)
    {
      activeObjects[m_FeatureIds[i]] = true;
    }
    cellFeatureAttrMat->removeInactiveObjects(activeObjects, m_FeatureIdsPtr.lock());
  }

  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer OrientationBinModeFilter::newFilterInstance(bool copyFilterParameters)
{
  OrientationBinModeFilter::Pointer filter = OrientationBinModeFilter::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}
