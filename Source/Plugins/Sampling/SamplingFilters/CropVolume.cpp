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

  parameters.push_back(VolumeInfoFilterParameter::New("Volume DataContainer Info", "CurrentVolumeDataContainerDimensions", FilterParameterWidgetType::VolumeDataContainerInfoWidget, getCurrentVolumeDataContainerDimensions(), false, "", "CurrentVolumeDataContainerResolutions"));
  parameters.push_back(FilterParameter::New("X Min (Voxels)", "XMin", FilterParameterWidgetType::IntWidget, getXMin(), false, "Column"));
  parameters.push_back(FilterParameter::New("Y Min (Voxels)", "YMin", FilterParameterWidgetType::IntWidget, getYMin(), false, "Row"));
  parameters.push_back(FilterParameter::New("Z Min (Voxels)", "ZMin", FilterParameterWidgetType::IntWidget, getZMin(), false, "Plane"));
  parameters.push_back(FilterParameter::New("X Max (Voxels)", "XMax", FilterParameterWidgetType::IntWidget, getXMax(), false, "Column"));
  parameters.push_back(FilterParameter::New("Y Max (Voxels)", "YMax", FilterParameterWidgetType::IntWidget, getYMax(), false, "Row"));
  parameters.push_back(FilterParameter::New("Z Max (Voxels)", "ZMax", FilterParameterWidgetType::IntWidget, getZMax(), false, "Plane"));
  QStringList linkedProps;
  linkedProps << "CellFeatureAttributeMatrixPath" << "FeatureIdsArrayPath";
  parameters.push_back(LinkedBooleanFilterParameter::New("Renumber Features", "RenumberFeatures", getRenumberFeatures(), linkedProps, false));
  linkedProps.clear();
  linkedProps << "NewDataContainerName";
  parameters.push_back(FilterParameter::New("Update Origin", "UpdateOrigin", FilterParameterWidgetType::BooleanWidget, getUpdateOrigin(), false));
  parameters.push_back(FilterParameter::New("Required Information", "", FilterParameterWidgetType::SeparatorWidget, "", true));
  parameters.push_back(FilterParameter::New("Cell Feature Attribute Matrix", "CellFeatureAttributeMatrixPath", FilterParameterWidgetType::AttributeMatrixSelectionWidget, getCellFeatureAttributeMatrixPath(), true));
  parameters.push_back(FilterParameter::New("FeatureIds", "FeatureIdsArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getFeatureIdsArrayPath(), true, ""));
  parameters.push_back(FilterParameter::New("Created Information", "", FilterParameterWidgetType::SeparatorWidget, "", true));
  parameters.push_back(LinkedBooleanFilterParameter::New("Save As New Data Container", "SaveAsNewDataContainer", getSaveAsNewDataContainer(), linkedProps, true));
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
  setRenumberFeatures( reader->readValue("RenumberFeatures", getRenumberFeatures()) );
  setSaveAsNewDataContainer( reader->readValue("SaveAsNewDataContainer", getSaveAsNewDataContainer()) );
  setUpdateOrigin( reader->readValue("UpdateOrigin", getUpdateOrigin()) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int CropVolume::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  DREAM3D_FILTER_WRITE_PARAMETER(NewDataContainerName)
  DREAM3D_FILTER_WRITE_PARAMETER(CellAttributeMatrixPath)
  DREAM3D_FILTER_WRITE_PARAMETER(CellFeatureAttributeMatrixPath)
  DREAM3D_FILTER_WRITE_PARAMETER(FeatureIdsArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(XMin)
  DREAM3D_FILTER_WRITE_PARAMETER(YMin)
  DREAM3D_FILTER_WRITE_PARAMETER(ZMin)
  DREAM3D_FILTER_WRITE_PARAMETER(XMax)
  DREAM3D_FILTER_WRITE_PARAMETER(YMax)
  DREAM3D_FILTER_WRITE_PARAMETER(ZMax)
  DREAM3D_FILTER_WRITE_PARAMETER(RenumberFeatures)
  DREAM3D_FILTER_WRITE_PARAMETER(SaveAsNewDataContainer)
  DREAM3D_FILTER_WRITE_PARAMETER(UpdateOrigin)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CropVolume::dataCheck()
{
  if(getErrorCondition() < 0) { return; }

  VolumeDataContainer* srcCellDataContainer = getDataContainerArray()->getPrereqDataContainer<VolumeDataContainer, AbstractFilter>(this, getCellAttributeMatrixPath().getDataContainerName());

  // Bail if the source data container cannot be found
  if (NULL == srcCellDataContainer)
  {
      return;
  }

  AttributeMatrix::Pointer cellAttrMat = srcCellDataContainer->getAttributeMatrix(getCellAttributeMatrixPath().getAttributeMatrixName());
  VolumeDataContainer* destCellDataContainer = srcCellDataContainer;

  IDataArray::Pointer da1 = cellAttrMat->getAttributeArray(cellAttrMat->getAttributeArrayNameList()[0]);

  if (m_SaveAsNewDataContainer == true)
  {
      float ox = 0.0f, oy = 0.0f, oz = 0.0f, rx = 0.0f, ry = 0.0f, rz = 0.0f;
      size_t dx = 0, dy = 0, dz = 0;
      srcCellDataContainer->getOrigin(ox, oy, oz);
      srcCellDataContainer->getResolution(rx, ry, rz);
      srcCellDataContainer->getDimensions(dx, dy, dz);

      destCellDataContainer = getDataContainerArray()->createNonPrereqDataContainer<VolumeDataContainer, AbstractFilter>(this, getNewDataContainerName());

      destCellDataContainer->setOrigin(ox, oy, oz);
      destCellDataContainer->setResolution(rx, ry, rz);
      destCellDataContainer->setDimensions(dx, dy, dz);

      AttributeMatrix::Pointer cellAttrMatCopy = cellAttrMat->deepCopy();
      destCellDataContainer->addAttributeMatrix(cellAttrMatCopy->getName(), cellAttrMatCopy);
      cellAttrMat = destCellDataContainer->getAttributeMatrix(getCellAttributeMatrixPath().getAttributeMatrixName());
  }

  if(NULL == destCellDataContainer || NULL == cellAttrMat || getErrorCondition() < 0)
  {
    return;
  }

  if (getXMax() < getXMin())
  {
    QString ss = QObject::tr("X Max (%1) less than X Min (%2)").arg(getXMax()).arg(getXMin());
    notifyErrorMessage(getHumanLabel(), ss, -5550);
    setErrorCondition(-5550);
  }
  if (getYMax() < getYMin())
  {
    QString ss = QObject::tr("Y Max (%1) less than Y Min (%2)").arg(getYMax()).arg(getYMin());
    notifyErrorMessage(getHumanLabel(), ss, -5551);
    setErrorCondition(-5551);
  }
  if (getZMax() < getZMin())
  {
    QString ss = QObject::tr("Z Max (%1) less than Z Min (%2)").arg(getZMax()).arg(getZMin());
    notifyErrorMessage(getHumanLabel(), ss, -5552);
    setErrorCondition(-5552);
  }
  if (getXMin() < 0)
  {
    QString ss = QObject::tr("X Min (%1) less than 0").arg(getXMin());
    notifyErrorMessage(getHumanLabel(), ss, -5553);
    setErrorCondition(-5553);
  }
  if (getYMin() < 0)
  {
    QString ss = QObject::tr("Y Min (%1) less than 0").arg(getYMin());
    notifyErrorMessage(getHumanLabel(), ss, -5554);
    setErrorCondition(-5554);
  }
  if (getZMin() < 0)
  {
    QString ss = QObject::tr("Z Min (%1) less than 0").arg(getZMin());
    notifyErrorMessage(getHumanLabel(), ss, -5555);
    setErrorCondition(-5555);
  }

  if (getXMax() > (static_cast<int64_t>(destCellDataContainer->getXPoints()) - 1))
  {
      QString ss = QObject::tr("The X Max you entered of %1 is greater than your Max X Point of %2").arg(getXMax()).arg(static_cast<int64_t>(destCellDataContainer->getXPoints()) - 1);
      notifyErrorMessage(getHumanLabel(), ss, -5556);
      setErrorCondition(-5556);
  }
  if (getYMax() > (static_cast<int64_t>(destCellDataContainer->getYPoints()) - 1))
  {
      QString ss = QObject::tr("The Y Max you entered of %1 is greater than your Max Y Point of %2").arg(getYMax()).arg(static_cast<int64_t>(destCellDataContainer->getYPoints()) - 1);
      notifyErrorMessage(getHumanLabel(), ss, -5557);
      setErrorCondition(-5557);
  }
  if (getZMax() > (static_cast<int64_t>(destCellDataContainer->getZPoints()) - 1))
  {
      QString ss = QObject::tr("The Z Max you entered of %1 is greater than your Max Z Point of %2").arg(getZMax()).arg(static_cast<int64_t>(destCellDataContainer->getZPoints()) - 1);
      notifyErrorMessage(getHumanLabel(), ss, -5558);
      setErrorCondition(-5558);
  }

  QVector<size_t> tDims(3, 0);
  if (getXMax() - getXMin() < 0) { setXMax(getXMin() + 1); }
  if (getYMax() - getYMin() < 0) { setYMax(getYMin() + 1); }
  if (getZMax() - getZMin() < 0) { setZMax(getZMin() + 1); }
  tDims[0] = (getXMax() - getXMin()) + 1;
  tDims[1] = (getYMax() - getYMin()) + 1;
  tDims[2] = (getZMax() - getZMin()) + 1;

  destCellDataContainer->setDimensions(tDims[0], tDims[1], tDims[2]);


  // If any of the sanity checks fail above then we should NOT attempt to go any further.
  if (getErrorCondition() < 0)
  {
    return;
  }

  cellAttrMat->setTupleDimensions(tDims);

  if(getErrorCondition() < 0 || NULL == cellAttrMat) { return; }



  if(m_RenumberFeatures == true)
  {
      QVector<size_t> dims(1, 1);
      m_FeatureIdsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getFeatureIdsArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
      if( NULL != m_FeatureIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
      { m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

      AttributeMatrix::Pointer cellFeatureAttrMat = srcCellDataContainer->getAttributeMatrix(getCellFeatureAttributeMatrixPath().getAttributeMatrixName());
      if(getErrorCondition() < 0) { return; }
      QVector<bool> activeObjects(cellFeatureAttrMat->getNumTuples(), true);
      cellFeatureAttrMat->removeInactiveObjects(activeObjects, m_FeatureIdsPtr.lock());
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CropVolume::preflight()
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
void CropVolume::execute()
{
    int err = 0;
    setErrorCondition(err);

    /* Normally, filters call dataCheck during the execute to reuse code.  Unfortunately, this cannot happen for this filter, because calling dataCheck
       would destroy an Attribute Matrix that we need during the execute.  Do not uncomment the code, and be careful when reusing code from either of
        these functions.  Make sure you understand how this works before you reuse any code. */
    //dataCheck();
    //if(getErrorCondition() < 0) { return; }

    VolumeDataContainer* srcCellDataContainer = getDataContainerArray()->getPrereqDataContainer<VolumeDataContainer, AbstractFilter>(this, getCellAttributeMatrixPath().getDataContainerName());
    AttributeMatrix::Pointer cellAttrMat = srcCellDataContainer->getAttributeMatrix(getCellAttributeMatrixPath().getAttributeMatrixName());
    VolumeDataContainer* destCellDataContainer = srcCellDataContainer;

    if (m_SaveAsNewDataContainer == true)
    {
        float ox = 0.0f, oy = 0.0f, oz = 0.0f, rx = 0.0f, ry = 0.0f, rz = 0.0f;
        srcCellDataContainer->getOrigin(ox, oy, oz);
        srcCellDataContainer->getResolution(rx, ry, rz);

        destCellDataContainer = getDataContainerArray()->createNonPrereqDataContainer<VolumeDataContainer, AbstractFilter>(this, getNewDataContainerName());

        destCellDataContainer->setOrigin(ox, oy, oz);
        destCellDataContainer->setResolution(rx, ry, rz);

        AttributeMatrix::Pointer cellAttrMatCopy = cellAttrMat->deepCopy();
        destCellDataContainer->addAttributeMatrix(cellAttrMatCopy->getName(), cellAttrMatCopy);
        cellAttrMat = destCellDataContainer->getAttributeMatrix(getCellAttributeMatrixPath().getAttributeMatrixName());
    }

    if(NULL == destCellDataContainer || NULL == cellAttrMat || getErrorCondition() < 0)
    {
      return;
    }


  // No matter where the AM is (same DC or new DC), we have the correct DC and AM pointers...now it's time to crop
  int64_t totalPoints = cellAttrMat->getNumTuples();

  size_t udims[3] =
  { 0, 0, 0 };
  srcCellDataContainer->getDimensions(udims);
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
  destCellDataContainer->getOrigin(oldOrigin);

  // Check to make sure the new dimensions are not "out of bounds" and warn the user if they are
  if (dims[0] <= m_XMax)
  {
    QString ss = QObject::tr("A Maximum value of %1 has been entered for the Max X which is larger than the input volume X Dimension of %2"
                             " This may lead to junk data being filled into the extra space.").arg(m_XMax).arg(dims[0]);
    notifyWarningMessage(getHumanLabel(), ss, -950);
  }
  if (dims[1] <= m_YMax)
  {

    QString ss = QObject::tr("A Maximum value of %1 has been entered for the Max Y which is larger than the input volume Y Dimension of %2"
                             " This may lead to junk data being filled into the extra space.").arg(m_YMax).arg(dims[1]);
    notifyWarningMessage(getHumanLabel(), ss, -950);
  }
  if (dims[2] <= m_ZMax)
  {

    QString ss = QObject::tr("A Maximum value of has been entered for the Max Z which is larger than the input volume Z Dimension of "
                             " This may lead to junk data being filled into the extra space.").arg(m_ZMax).arg(dims[2]);
    notifyWarningMessage(getHumanLabel(), ss, -950);
  }

  int64_t XP = ( (m_XMax - m_XMin) + 1 );
  int64_t YP = ( (m_YMax - m_YMin) + 1 );
  int64_t ZP = ( (m_ZMax - m_ZMin) + 1 );

  int64_t col, row, plane;
  int64_t colold, rowold, planeold;
  int64_t index;
  int64_t index_old;
  QList<QString> voxelArrayNames = cellAttrMat->getAttributeArrayNameList();
  for (int64_t i = 0; i < ZP; i++)
  {
    QString ss = QObject::tr("Cropping Volume - Slice %1 of %2 Complete").arg(i).arg(ZP);
    notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);
    planeold = (i + m_ZMin) * (destCellDataContainer->getXPoints() * destCellDataContainer->getYPoints());
    plane = (i * XP * YP);
    for (int64_t j = 0; j < YP; j++)
    {
      rowold = (j + m_YMin) * destCellDataContainer->getXPoints();
      row = (j * XP);
      for (int64_t k = 0; k < XP; k++)
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
  destCellDataContainer->setDimensions(static_cast<size_t>(XP), static_cast<size_t>(YP), static_cast<size_t>(ZP));
  totalPoints = destCellDataContainer->getTotalPoints();
  QVector<size_t> tDims(3, 0);
  tDims[0] = XP;
  tDims[1] = YP;
  tDims[2] = ZP;
  cellAttrMat->setTupleDimensions(tDims); // THIS WILL CAUSE A RESIZE of all the underlying data arrays.

  // Feature Ids MUST already be renumbered.
  if (m_RenumberFeatures == true)
  {
    totalPoints = destCellDataContainer->getTotalPoints();
    AttributeMatrix::Pointer cellFeatureAttrMat = srcCellDataContainer->getAttributeMatrix(getCellFeatureAttributeMatrixPath().getAttributeMatrixName());
    size_t totalFeatures = cellFeatureAttrMat->getNumTuples();
    QVector<bool> activeObjects(totalFeatures, false);
    if (0 == totalFeatures)
    {
      setErrorCondition(-600);
      notifyErrorMessage(getHumanLabel(), "The number of features is Zero and should be greater than Zero", getErrorCondition());
      notifyStatusMessage(getHumanLabel(), "Completed");
      return;
    }

    QVector<size_t> cDims(1, 1);
    m_FeatureIdsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getFeatureIdsArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if( NULL != m_FeatureIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    { m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
    if(getErrorCondition() < 0)
    {
      return;
    }

    // Find the unique set of feature ids
    for (int64_t i = 0; i < totalPoints; ++i)
    {
        int32_t currentFeatureId = m_FeatureIds[i];
        if (currentFeatureId < totalFeatures)
        {
            activeObjects[currentFeatureId] = true;
        }
        else
        {
            setErrorCondition(-601);
            QString ss = QObject::tr("The total number of features from %1 is %2, but a value of %3 was found in Data Array %4.").arg(cellFeatureAttrMat->getName()).arg(totalFeatures).arg(currentFeatureId).arg(getFeatureIdsArrayPath().serialize("/"));
            notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
            return;
        }
    }
    cellFeatureAttrMat->removeInactiveObjects(activeObjects, m_FeatureIdsPtr.lock());
  }

  if(m_UpdateOrigin == true)
  {
    float resolution[3] = {0.0f, 0.0f, 0.0f};
    destCellDataContainer->getResolution(resolution);

    float origin[3] = {0.0f, 0.0f, 0.0f};
    destCellDataContainer->getOrigin(origin);

    origin[0] = m_XMin * resolution[0] + oldOrigin[0];
    origin[1] = m_YMin * resolution[1] + oldOrigin[1];
    origin[2] = m_ZMin * resolution[2] + oldOrigin[2];

    destCellDataContainer->setOrigin(origin);
  }

  notifyStatusMessage(getHumanLabel(), "Completed");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IntVec3_t CropVolume::getCurrentVolumeDataContainerDimensions()
{
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getCellAttributeMatrixPath().getDataContainerName());
  IntVec3_t data;
  if (NULL != m)
  {
    data.x = m->getXPoints();
    data.y = m->getYPoints();
    data.z = m->getZPoints();
  }
  else
  {
    data.x = 0;
    data.y = 0;
    data.z = 0;
  }
  return data;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FloatVec3_t CropVolume::getCurrentVolumeDataContainerResolutions()
{
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getCellAttributeMatrixPath().getDataContainerName());
  FloatVec3_t data;
  if (NULL != m)
  {
    data.x = m->getXRes();
    data.y = m->getYRes();
    data.z = m->getZRes();
  }
  else
  {
    data.x = 0;
    data.y = 0;
    data.z = 0;
  }
  return data;
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

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CropVolume::getCompiledLibraryName()
{ return Sampling::SamplingBaseName; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CropVolume::getGroupName()
{ return DREAM3D::FilterGroups::SamplingFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CropVolume::getHumanLabel()
{ return "Crop Volume"; }

