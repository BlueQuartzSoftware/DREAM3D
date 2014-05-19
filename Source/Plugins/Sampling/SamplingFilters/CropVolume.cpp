/* ============================================================================
 * Copyright (c) 2011 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2011 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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
 * Neither the name of Michael A. Groeber, Michael A. Jackson, the US Air Force,
 * BlueQuartz Software nor the names of its contributors may be used to endorse
 * or promote products derived from this software without specific prior written
 * permission.
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
 *                           FA8650-07-D-5800
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "CropVolume.h"

#include <QtCore/QMap>


#include "DREAM3DLib/Common/Constants.h"

#include "DREAM3DLib/Utilities/DREAM3DRandom.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CropVolume::CropVolume() :
  AbstractFilter(),
  m_NewDataContainerName(DREAM3D::Defaults::NewVolumeDataContainerName),
  m_CellAttributeMatrixPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, ""),
  m_CellFeatureAttributeMatrixPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellFeatureAttributeMatrixName, ""),
  m_XMin(0),
  m_YMin(0),
  m_ZMin(0),
  m_XMax(0),
  m_YMax(0),
  m_ZMax(0),
  m_RenumberFeatures(true),
  m_SaveAsNewDataContainer(false),
  m_UpdateOrigin(true),
  m_FeatureIdsArrayPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, DREAM3D::CellData::FeatureIds),
  m_FeatureIdsArrayName(DREAM3D::CellData::FeatureIds),
  m_FeatureIds(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CropVolume::~CropVolume()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CropVolume::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(FilterParameter::New("Cell Attribute Matrix", "CellAttributeMatrixPath", FilterParameterWidgetType::AttributeMatrixSelectionWidget, getCellAttributeMatrixPath(), false));
  parameters.push_back(FilterParameter::New("X Min (Voxels)", "XMin", FilterParameterWidgetType::IntWidget, getXMin(), false, "Column"));
  parameters.push_back(FilterParameter::New("Y Min (Voxels)", "YMin", FilterParameterWidgetType::IntWidget, getYMin(), false, "Row"));
  parameters.push_back(FilterParameter::New("Z Min (Voxels)", "ZMin", FilterParameterWidgetType::IntWidget, getZMin(), false, "Plane"));
  parameters.push_back(FilterParameter::New("X Max (Voxels)", "XMax", FilterParameterWidgetType::IntWidget, getXMax(), false, "Column"));
  parameters.push_back(FilterParameter::New("Y Max (Voxels)", "YMax", FilterParameterWidgetType::IntWidget, getYMax(), false, "Row"));
  parameters.push_back(FilterParameter::New("Z Max (Voxels)", "ZMax", FilterParameterWidgetType::IntWidget, getZMax(), false, "Plane"));
  parameters.push_back(FilterParameter::New("Renumber Features", "RenumberFeatures", FilterParameterWidgetType::BooleanWidget, getRenumberFeatures(), false));
  parameters.push_back(FilterParameter::New("Save As New Data Container", "SaveAsNewDataContainer", FilterParameterWidgetType::BooleanWidget, getSaveAsNewDataContainer(), false));
  parameters.push_back(FilterParameter::New("Update Origin", "UpdateOrigin", FilterParameterWidgetType::BooleanWidget, getUpdateOrigin(), false));
  parameters.push_back(FilterParameter::New("Required Information", "", FilterParameterWidgetType::SeparatorWidget, "", true));
  parameters.push_back(FilterParameter::New("Cell Feature Attribute Matrix", "CellFeatureAttributeMatrixPath", FilterParameterWidgetType::AttributeMatrixSelectionWidget, getCellFeatureAttributeMatrixPath(), true));
  parameters.push_back(FilterParameter::New("FeatureIds", "FeatureIdsArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getFeatureIdsArrayPath(), true, ""));
  parameters.push_back(FilterParameter::New("Created Information", "", FilterParameterWidgetType::SeparatorWidget, "", true));
  parameters.push_back(FilterParameter::New("New Data Container Name", "NewDataContainerName", FilterParameterWidgetType::StringWidget, getNewDataContainerName(), true));
  setFilterParameters(parameters);
}
// -----------------------------------------------------------------------------
void CropVolume::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setNewDataContainerName( reader->readString("NewDataContainerName", getNewDataContainerName() ) );
  setCellAttributeMatrixPath( reader->readDataArrayPath("CellAttributeMatrixPath", getCellAttributeMatrixPath() ) );
  setCellFeatureAttributeMatrixPath( reader->readDataArrayPath("CellFeatureAttributeMatrixPath", getCellFeatureAttributeMatrixPath() ) );
  setFeatureIdsArrayPath(reader->readDataArrayPath("FeatureIdsArrayPath", getFeatureIdsArrayPath() ) );
  setXMin( reader->readValue("XMin", getXMin()) );
  setYMin( reader->readValue("YMin", getYMin()) );
  setZMin( reader->readValue("ZMin", getZMin()) );
  setXMax( reader->readValue("XMax", getXMax()) );
  setYMax( reader->readValue("YMax", getYMax()) );
  setZMax( reader->readValue("ZMax", getZMax()) );
  setRenumberFeatures( reader->readValue("RenumberFeatures", false) );
  setSaveAsNewDataContainer( reader->readValue("SaveAsNewDataContainer", false) );
  setUpdateOrigin( reader->readValue("UpdateOrigin", false) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int CropVolume::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("NewDataContainerName", getNewDataContainerName() );
  writer->writeValue("CellAttributeMatrixPath", getCellAttributeMatrixPath() );
  writer->writeValue("CellFeatureAttributeMatrixPath", getCellFeatureAttributeMatrixPath() );
  writer->writeValue("FeatureIdsArrayPath", getFeatureIdsArrayPath() );
  writer->writeValue("XMin", getXMin() );
  writer->writeValue("YMin", getYMin() );
  writer->writeValue("ZMin", getZMin() );
  writer->writeValue("XMax", getXMax() );
  writer->writeValue("YMax", getYMax() );
  writer->writeValue("ZMax", getZMax() );
  writer->writeValue("RenumberFeatures", getRenumberFeatures() );
  writer->writeValue("SaveAsNewDataContiner", getSaveAsNewDataContainer() );
  writer->writeValue("UpdateOrigin", getUpdateOrigin() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CropVolume::updateCellInstancePointers()
{
  setErrorCondition(0);

  if( NULL != m_FeatureIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CropVolume::dataCheck()
{
  setErrorCondition(0);

  VolumeDataContainer* m;
  if(m_SaveAsNewDataContainer == false) { m = getDataContainerArray()->getPrereqDataContainer<VolumeDataContainer, AbstractFilter>(this, getCellAttributeMatrixPath().getDataContainerName()); }
  else
  {
    getDataContainerArray()->duplicateDataContainer(getCellAttributeMatrixPath().getDataContainerName(), getNewDataContainerName());
    m = getDataContainerArray()->getPrereqDataContainer<VolumeDataContainer, AbstractFilter>(this, getNewDataContainerName());
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
void CropVolume::preflight()
{
  emit preflightAboutToExecute();
  emit updateFilterParameters(this);
  dataCheck();
  emit preflightExecuted();
  if(getErrorCondition() < 0) { return; }

  VolumeDataContainer* m;
  if(m_SaveAsNewDataContainer == false) { m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getCellAttributeMatrixPath().getDataContainerName()); }
  else { m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getNewDataContainerName()); }

  if (getXMax() < getXMin())
  {
    QString ss = QObject::tr("X Max (%1) less than X Min (%2)").arg(getXMax()).arg(getXMin());
    notifyErrorMessage(getHumanLabel(), ss, -5555);
    setErrorCondition(-5555);
  }
  if (getYMax() < getYMin())
  {
    QString ss = QObject::tr("Y Max (%1) less than Y Min (%2)").arg(getYMax()).arg(getYMin());
    notifyErrorMessage(getHumanLabel(), ss, -5555);
    setErrorCondition(-5555);
  }
  if (getZMax() < getZMin())
  {
    QString ss = QObject::tr("Z Max (%1) less than Z Min (%2)").arg(getZMax()).arg(getZMin());
    notifyErrorMessage(getHumanLabel(), ss, -5555);
    setErrorCondition(-5555);
  }
  if (getXMin() < 0)
  {
    QString ss = QObject::tr("X Min (%1) less than 0").arg(getXMin());
    notifyErrorMessage(getHumanLabel(), ss, -5555);
    setErrorCondition(-5555);
  }
  if (getYMin() < 0)
  {
    QString ss = QObject::tr("Y Min (%1) less than 0").arg(getYMin());
    notifyErrorMessage(getHumanLabel(), ss, -5555);
    setErrorCondition(-5555);
  }
  if (getZMin() < 0)
  {
    QString ss = QObject::tr("Z Min (%1) less than 0").arg(getZMin());
    notifyErrorMessage(getHumanLabel(), ss, -5555);
    setErrorCondition(-5555);
  }
  if (getXMax() > (static_cast<int64_t>(m->getXPoints()) - 1))
  {
    QString ss = QObject::tr("The X Max you entered of %1 is greater than your Max X Point of %2").arg(getXMax()).arg(static_cast<int64_t>(m->getXPoints()) - 1);
    notifyErrorMessage(getHumanLabel(), ss, -5555);
    setErrorCondition(-5555);
  }
  if (getYMax() > (static_cast<int64_t>(m->getYPoints()) - 1))
  {
    QString ss = QObject::tr("The Y Max you entered of %1 is greater than your Max Y Point of %2").arg(getYMax()).arg(static_cast<int64_t>(m->getYPoints()) - 1);
    notifyErrorMessage(getHumanLabel(), ss, -5555);
    setErrorCondition(-5556);
  }
  if (getZMax() > (static_cast<int64_t>(m->getZPoints()) - 1))
  {
    QString ss = QObject::tr("The Z Max you entered of %1) greater than your Max Z Point of %2").arg(getZMax()).arg(static_cast<int64_t>(m->getZPoints()) - 1);
    notifyErrorMessage(getHumanLabel(), ss, -5555);
    setErrorCondition(-5557);
  }

  QVector<size_t> tDims(3, 0);
  tDims[0] = (getXMax() - getXMin()) + 1;
  tDims[1] = (getYMax() - getYMin()) + 1;
  tDims[2] = (getZMax() - getZMin()) + 1;
  m->getAttributeMatrix(getCellAttributeMatrixPath().getAttributeMatrixName())->setTupleDimensions(tDims);
  m->setDimensions(tDims[0], tDims[1], tDims[2]);

  if(m_RenumberFeatures == true)
  {
    AttributeMatrix::Pointer cellFeatureAttrMat = m->getAttributeMatrix(getCellFeatureAttributeMatrixPath().getAttributeMatrixName());
    QVector<bool> activeObjects(cellFeatureAttrMat->getNumTuples(), true);
    cellFeatureAttrMat->removeInactiveObjects(activeObjects, m_FeatureIdsPtr.lock());
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CropVolume::execute()
{
  int err = 0;
  setErrorCondition(err);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  VolumeDataContainer* m;
  if(m_SaveAsNewDataContainer == false) { m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getCellAttributeMatrixPath().getDataContainerName()); }
  else { m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getNewDataContainerName()); }

  AttributeMatrix::Pointer cellAttrMat = m->getAttributeMatrix(getCellAttributeMatrixPath().getAttributeMatrixName());
  int64_t totalPoints = cellAttrMat->getNumTuples();

  size_t udims[3] =
  { 0, 0, 0 };
  m->getDimensions(udims);
#if (CMP_SIZEOF_SIZE_T == 4)
  typedef int32_t DimType;
#else
  typedef int64_t DimType;
#endif
  DimType dims[3] =
  { static_cast<DimType>(udims[0]), static_cast<DimType>(udims[1]), static_cast<DimType>(udims[2]), };

  // Check to see if the dims have actually changed.
  if(dims[0] == (m_XMax - m_XMin) && dims[1] == (m_YMax - m_YMin) && dims[2] == (m_ZMax - m_ZMin))
  {
    return;
  }

  //get current origin
  float oldOrigin[3] = {0.0f, 0.0f, 0.0f};
  m->getOrigin(oldOrigin);

  // Check to make sure the new dimensions are not "out of bounds" and warn the user if they are
  if (dims[0] <= m_XMax)
  {
    QString ss = QObject::tr("A Maximum value of %1 has been entered for the Max X which is larger than the input volume X Dimension of %2"
                             " This may lead to junk data being filled into the extra space.").arg(m_XMax).arg(dims[0]);
    notifyErrorMessage(getHumanLabel(), ss, -950);
  }
  if (dims[1] <= m_YMax)
  {

    QString ss = QObject::tr("A Maximum value of %1 has been entered for the Max Y which is larger than the input volume Y Dimension of %2"
                             " This may lead to junk data being filled into the extra space.").arg(m_YMax).arg(dims[1]);
    notifyErrorMessage(getHumanLabel(), ss, -950);
  }
  if (dims[2] <= m_ZMax)
  {

    QString ss = QObject::tr("A Maximum value of has been entered for the Max Z which is larger than the input volume Z Dimension of "
                             " This may lead to junk data being filled into the extra space.").arg(m_ZMax).arg(dims[2]);
    notifyErrorMessage(getHumanLabel(), ss, -950);
  }

  int64_t m_XP = ( (m_XMax - m_XMin) + 1 );
  int64_t m_YP = ( (m_YMax - m_YMin) + 1 );
  int64_t m_ZP = ( (m_ZMax - m_ZMin) + 1 );

  int64_t col, row, plane;
  int64_t colold, rowold, planeold;
  int64_t index;
  int64_t index_old;
  QList<QString> voxelArrayNames = cellAttrMat->getAttributeArrayNameList();
  for (int64_t i = 0; i < m_ZP; i++)
  {
    QString ss = QObject::tr("Cropping Volume - Slice %1 of %2 Complete").arg(i).arg(m_ZP);
    notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);
    planeold = (i + m_ZMin) * (m->getXPoints() * m->getYPoints());
    plane = (i * m_XP * m_YP);
    for (int64_t j = 0; j < m_YP; j++)
    {
      rowold = (j + m_YMin) * m->getXPoints();
      row = (j * m_XP);
      for (int64_t k = 0; k < m_XP; k++)
      {
        colold = (k + m_XMin);
        col = k;
        index_old = planeold + rowold + colold;
        index = plane + row + col;
        for (QList<QString>::iterator iter = voxelArrayNames.begin(); iter != voxelArrayNames.end(); ++iter)
        {
          QString name = *iter;
          IDataArray::Pointer p = cellAttrMat->getAttributeArray(*iter);
          p->copyTuple(index_old, index);
        }
      }
    }
  }
  m->setDimensions(static_cast<size_t>(m_XP), static_cast<size_t>(m_YP), static_cast<size_t>(m_ZP));
  totalPoints = m->getTotalPoints();
  QVector<size_t> tDims(3, 0);
  tDims[0] = m_XP;
  tDims[1] = m_YP;
  tDims[2] = m_ZP;
  cellAttrMat->setTupleDimensions(tDims);

  // Feature Ids MUST already be renumbered.
  if (m_RenumberFeatures == true)
  {
    totalPoints = m->getTotalPoints();
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

  if(m_UpdateOrigin == true)
  {
    float resolution[3] = {0.0f, 0.0f, 0.0f};
    m->getResolution(resolution);

    float origin[3] = {0.0f, 0.0f, 0.0f};
    m->getOrigin(origin);

    origin[0] = m_XMin * resolution[0] + oldOrigin[0];
    origin[1] = m_YMin * resolution[1] + oldOrigin[1];
    origin[2] = m_ZMin * resolution[2] + oldOrigin[2];

    m->setOrigin(origin);
  }

  notifyStatusMessage(getHumanLabel(), "Completed");
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer CropVolume::newFilterInstance(bool copyFilterParameters)
{
  CropVolume::Pointer filter = CropVolume::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}
