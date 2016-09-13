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

#include "CropImageGeometry.h"

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/IntFilterParameter.h"
#include "SIMPLib/FilterParameters/BooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/AttributeMatrixSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedBooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/Utilities/SIMPLibRandom.h"
#include "SIMPLib/Geometry/ImageGeom.h"

#include "Sampling/SamplingConstants.h"
#include "Sampling/SamplingVersion.h"

// Include the MOC generated file for this class
#include "moc_CropImageGeometry.cpp"



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CropImageGeometry::CropImageGeometry() :
  AbstractFilter(),
  m_NewDataContainerName(SIMPL::Defaults::NewImageDataContainerName),
  m_CellAttributeMatrixPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellAttributeMatrixName, ""),
  m_CellFeatureAttributeMatrixPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellFeatureAttributeMatrixName, ""),
  m_XMin(0),
  m_YMin(0),
  m_ZMin(0),
  m_XMax(0),
  m_YMax(0),
  m_ZMax(0),
  m_RenumberFeatures(true),
  m_SaveAsNewDataContainer(false),
  m_UpdateOrigin(true),
  m_FeatureIdsArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellAttributeMatrixName, SIMPL::CellData::FeatureIds),
  m_FeatureIds(nullptr)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CropImageGeometry::~CropImageGeometry()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CropImageGeometry::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(SIMPL_NEW_INTEGER_FP("X Min (Column)", XMin, FilterParameter::Parameter, CropImageGeometry));
  parameters.push_back(SIMPL_NEW_INTEGER_FP("Y Min (Row)", YMin, FilterParameter::Parameter, CropImageGeometry));
  parameters.push_back(SIMPL_NEW_INTEGER_FP("Z Min (Plane)", ZMin, FilterParameter::Parameter, CropImageGeometry));
  parameters.push_back(SIMPL_NEW_INTEGER_FP("X Max (Column) [Inclusive]", XMax, FilterParameter::Parameter, CropImageGeometry));
  parameters.push_back(SIMPL_NEW_INTEGER_FP("Y Max (Row) [Inclusive]", YMax, FilterParameter::Parameter, CropImageGeometry));
  parameters.push_back(SIMPL_NEW_INTEGER_FP("Z Max (Plane) [Inclusive]", ZMax, FilterParameter::Parameter, CropImageGeometry));
  QStringList linkedProps;
  linkedProps << "CellFeatureAttributeMatrixPath" << "FeatureIdsArrayPath";
  parameters.push_back(SIMPL_NEW_LINKED_BOOL_FP("Renumber Features", RenumberFeatures, FilterParameter::Parameter, CropImageGeometry, linkedProps));
  linkedProps.clear();
  linkedProps << "NewDataContainerName";
  parameters.push_back(SIMPL_NEW_BOOL_FP("Update Origin", UpdateOrigin, FilterParameter::Parameter, CropImageGeometry));
  parameters.push_back(SIMPL_NEW_LINKED_BOOL_FP("Save As New Data Container", SaveAsNewDataContainer, FilterParameter::Parameter, CropImageGeometry, linkedProps));
  parameters.push_back(SeparatorFilterParameter::New("Cell Data", FilterParameter::RequiredArray));
  {
    AttributeMatrixSelectionFilterParameter::RequirementType req = AttributeMatrixSelectionFilterParameter::CreateRequirement(SIMPL::AttributeMatrixType::Cell, SIMPL::GeometryType::ImageGeometry);
    parameters.push_back(SIMPL_NEW_AM_SELECTION_FP("Cell Attribute Matrix", CellAttributeMatrixPath, FilterParameter::RequiredArray, CropImageGeometry, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Int32, 1, SIMPL::AttributeMatrixType::Cell, SIMPL::GeometryType::ImageGeometry);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Feature Ids", FeatureIdsArrayPath, FilterParameter::RequiredArray, CropImageGeometry, req));
  }
  parameters.push_back(SeparatorFilterParameter::New("Cell Feature Data", FilterParameter::RequiredArray));
  {
    AttributeMatrixSelectionFilterParameter::RequirementType req = AttributeMatrixSelectionFilterParameter::CreateRequirement(SIMPL::AttributeMatrixType::CellFeature, SIMPL::GeometryType::ImageGeometry);
    parameters.push_back(SIMPL_NEW_AM_SELECTION_FP("Cell Feature Attribute Matrix", CellFeatureAttributeMatrixPath, FilterParameter::RequiredArray, CropImageGeometry, req));
  }
  parameters.push_back(SIMPL_NEW_STRING_FP("Data Container", NewDataContainerName, FilterParameter::CreatedArray, CropImageGeometry));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CropImageGeometry::readFilterParameters(AbstractFilterParametersReader* reader, int index)
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
void CropImageGeometry::initialize()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CropImageGeometry::dataCheck()
{
  if(getErrorCondition() < 0) { return; }
  setErrorCondition(0);

  // Validate the incoming DataContainer, Geometry, and AttributeMatrix ; bail if any do not exist since we plan on using them later on in the dataCheck
  // Error messages are handled by the getPrereq functions
  DataContainer::Pointer srcCellDataContainer = getDataContainerArray()->getPrereqDataContainer<AbstractFilter>(this, getCellAttributeMatrixPath().getDataContainerName());
  ImageGeom::Pointer image = getDataContainerArray()->getPrereqGeometryFromDataContainer<ImageGeom, AbstractFilter>(this, getCellAttributeMatrixPath().getDataContainerName());
  AttributeMatrix::Pointer srcCellAttrMat = getDataContainerArray()->getPrereqAttributeMatrixFromPath<AbstractFilter>(this, getCellAttributeMatrixPath(), -301);
  if(getErrorCondition() < 0) { return; }

  DataContainer::Pointer destCellDataContainer = srcCellDataContainer;
  AttributeMatrix::Pointer destCellAttrMat;

  if (m_SaveAsNewDataContainer == true)
  {
    float ox = 0.0f, oy = 0.0f, oz = 0.0f, rx = 0.0f, ry = 0.0f, rz = 0.0f;
    size_t dx = 0, dy = 0, dz = 0;
    image->getOrigin(ox, oy, oz);
    image->getResolution(rx, ry, rz);
    image->getDimensions(dx, dy, dz);

    destCellDataContainer = getDataContainerArray()->createNonPrereqDataContainer<AbstractFilter>(this, getNewDataContainerName());
    if(nullptr == destCellDataContainer.get() || getErrorCondition() < 0)
    {
      return;
    }
    IGeometry::Pointer imageCopy = image->deepCopy();
    destCellDataContainer->setGeometry(imageCopy);

    destCellAttrMat = srcCellAttrMat->deepCopy();
    destCellDataContainer->addAttributeMatrix(destCellAttrMat->getName(), destCellAttrMat);
  }
  else
  {
    destCellAttrMat = srcCellAttrMat;
  }

  if(nullptr == destCellDataContainer.get() || nullptr == destCellAttrMat.get() || getErrorCondition() < 0)
  {
    return;
  }

  if (getXMax() < getXMin())
  {
    QString ss = QObject::tr("X Max (%1) less than X Min (%2)").arg(getXMax()).arg(getXMin());
    setErrorCondition(-5550);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }
  if (getYMax() < getYMin())
  {
    QString ss = QObject::tr("Y Max (%1) less than Y Min (%2)").arg(getYMax()).arg(getYMin());
    setErrorCondition(-5550);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }
  if (getZMax() < getZMin())
  {
    QString ss = QObject::tr("Z Max (%1) less than Z Min (%2)").arg(getZMax()).arg(getZMin());
    setErrorCondition(-5550);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }
  if (getXMin() < 0)
  {
    QString ss = QObject::tr("X Min (%1) less than 0").arg(getXMin());
    setErrorCondition(-5550);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }
  if (getYMin() < 0)
  {
    QString ss = QObject::tr("Y Min (%1) less than 0").arg(getYMin());
    setErrorCondition(-5550);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }
  if (getZMin() < 0)
  {
    QString ss = QObject::tr("Z Min (%1) less than 0").arg(getZMin());
    setErrorCondition(-5550);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

  if (getXMax() > (static_cast<int64_t>(destCellDataContainer->getGeometryAs<ImageGeom>()->getXPoints()) - 1))
  {
    QString ss = QObject::tr("The X Max (%1) is greater than the Image Geometry X extent (%2)").arg(getXMax()).arg(static_cast<int64_t>(destCellDataContainer->getGeometryAs<ImageGeom>()->getXPoints()) - 1);
    setErrorCondition(-5550);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

  if (getYMax() > (static_cast<int64_t>(destCellDataContainer->getGeometryAs<ImageGeom>()->getYPoints()) - 1))
  {
    QString ss = QObject::tr("The Y Max (%1) is greater than the Image Geometry Y extent (%2)").arg(getYMax()).arg(static_cast<int64_t>(destCellDataContainer->getGeometryAs<ImageGeom>()->getYPoints()) - 1);
    setErrorCondition(-5550);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

  if (getZMax() > (static_cast<int64_t>(destCellDataContainer->getGeometryAs<ImageGeom>()->getZPoints()) - 1))
  {
    QString ss = QObject::tr("The Z Max (%1) is greater than the Image Geometry Z extent (%2)").arg(getZMax()).arg(static_cast<int64_t>(destCellDataContainer->getGeometryAs<ImageGeom>()->getZPoints()) - 1);
    setErrorCondition(-5550);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

  QVector<size_t> tDims(3, 0);
  if (getXMax() - getXMin() < 0) { setXMax(getXMin() + 1); }
  if (getYMax() - getYMin() < 0) { setYMax(getYMin() + 1); }
  if (getZMax() - getZMin() < 0) { setZMax(getZMin() + 1); }
  tDims[0] = (getXMax() - getXMin()) + 1;
  tDims[1] = (getYMax() - getYMin()) + 1;
  tDims[2] = (getZMax() - getZMin()) + 1;

  destCellDataContainer->getGeometryAs<ImageGeom>()->setDimensions(tDims[0], tDims[1], tDims[2]);

  // If any of the sanity checks fail above then we should NOT attempt to go any further.
  if (getErrorCondition() < 0) { return; }

  size_t totalPoints = 1;
  for(int i = 0; i < 3; i++) {
    if(tDims[i] != 0) { totalPoints *= tDims[i]; }
  }
  AttributeMatrix::Pointer newCellAttrMat = AttributeMatrix::New(tDims, destCellAttrMat->getName(), destCellAttrMat->getType());

  QList<QString> voxelArrayNames = destCellAttrMat->getAttributeArrayNames();
  for (QList<QString>::iterator iter = voxelArrayNames.begin(); iter != voxelArrayNames.end(); ++iter)
  {
    IDataArray::Pointer p = destCellAttrMat->getAttributeArray(*iter);
    //
    IDataArray::Pointer data = p->createNewArray(totalPoints, p->getComponentDimensions(), p->getName(), false);

    destCellAttrMat->removeAttributeArray(*iter);
    newCellAttrMat->addAttributeArray(*iter, data);
  }
  destCellDataContainer->removeAttributeMatrix(destCellAttrMat->getName());
  destCellDataContainer->addAttributeMatrix(newCellAttrMat->getName(), newCellAttrMat);


  if(m_RenumberFeatures == true)
  {
    QVector<size_t> cDims(1, 1);
    m_FeatureIdsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getFeatureIdsArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if( nullptr != m_FeatureIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
    {
      m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0);
    } /* Now assign the raw pointer to data from the DataArray<T> object */

    AttributeMatrix::Pointer cellFeatureAttrMat = srcCellDataContainer->getAttributeMatrix(getCellFeatureAttributeMatrixPath().getAttributeMatrixName());
    if(nullptr == cellFeatureAttrMat.get()) { return; }
    QVector<bool> activeObjects(cellFeatureAttrMat->getNumberOfTuples(), true);
    cellFeatureAttrMat->removeInactiveObjects(activeObjects, m_FeatureIdsPtr.lock());
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CropImageGeometry::preflight()
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
void CropImageGeometry::execute()
{
  setErrorCondition(0);

  /* Normally, filters call dataCheck during the execute to reuse code.  Unfortunately,
   * this cannot happen for this filter, because calling dataCheck would destroy
   * an Attribute Matrix that we need during the execute.  Do not uncomment the code,
   * and be careful when reusing code from either of these functions.  Make sure
   * you understand how this works before you reuse any code. */
  //dataCheck();
  //if(getErrorCondition() < 0) { return; }

  DataContainer::Pointer srcCellDataContainer = getDataContainerArray()->getPrereqDataContainer<AbstractFilter>(this, getCellAttributeMatrixPath().getDataContainerName());
  AttributeMatrix::Pointer cellAttrMat = srcCellDataContainer->getAttributeMatrix(getCellAttributeMatrixPath().getAttributeMatrixName());
  DataContainer::Pointer destCellDataContainer = srcCellDataContainer;

  if (m_SaveAsNewDataContainer == true)
  {
    float ox = 0.0f, oy = 0.0f, oz = 0.0f, rx = 0.0f, ry = 0.0f, rz = 0.0f;
    srcCellDataContainer->getGeometryAs<ImageGeom>()->getOrigin(ox, oy, oz);
    srcCellDataContainer->getGeometryAs<ImageGeom>()->getResolution(rx, ry, rz);

    destCellDataContainer = getDataContainerArray()->createNonPrereqDataContainer<AbstractFilter>(this, getNewDataContainerName());
    ImageGeom::Pointer image = ImageGeom::CreateGeometry(SIMPL::Geometry::ImageGeometry);
    destCellDataContainer->setGeometry(image);

    destCellDataContainer->getGeometryAs<ImageGeom>()->setOrigin(ox, oy, oz);
    destCellDataContainer->getGeometryAs<ImageGeom>()->setResolution(rx, ry, rz);

    AttributeMatrix::Pointer cellAttrMatCopy = cellAttrMat->deepCopy();
    destCellDataContainer->addAttributeMatrix(cellAttrMatCopy->getName(), cellAttrMatCopy);
    cellAttrMat = destCellDataContainer->getAttributeMatrix(getCellAttributeMatrixPath().getAttributeMatrixName());
  }

  if(nullptr == destCellDataContainer.get() || nullptr == cellAttrMat.get() || getErrorCondition() < 0)
  {
    return;
  }


  // No matter where the AM is (same DC or new DC), we have the correct DC and AM pointers...now it's time to crop
  int64_t totalPoints = cellAttrMat->getNumberOfTuples();

  size_t udims[3] =
  { 0, 0, 0 };
  srcCellDataContainer->getGeometryAs<ImageGeom>()->getDimensions(udims);

  int64_t dims[3] =
  { static_cast<int64_t>(udims[0]), static_cast<int64_t>(udims[1]), static_cast<int64_t>(udims[2]), };

  // Check to see if the dims have actually changed.
  if(dims[0] == (m_XMax - m_XMin) && dims[1] == (m_YMax - m_YMin) && dims[2] == (m_ZMax - m_ZMin))
  {
    return;
  }

  // Get current origin
  float oldOrigin[3] = {0.0f, 0.0f, 0.0f};
  destCellDataContainer->getGeometryAs<ImageGeom>()->getOrigin(oldOrigin);

  // Check to make sure the new dimensions are not "out of bounds" and warn the user if they are
  if (dims[0] <= m_XMax)
  {
    QString ss = QObject::tr("The Max X value (%1) is greater than the Image Geometry X entent (%2)."
                             " This may lead to junk data being filled into the extra space.").arg(m_XMax).arg(dims[0]);
    setErrorCondition(-950);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }
  if (dims[1] <= m_YMax)
  {

    QString ss = QObject::tr("The Max Y value (%1) is greater than the Image Geometry Y entent (%2)."
                             " This may lead to junk data being filled into the extra space.").arg(m_YMax).arg(dims[1]);
    setErrorCondition(-951);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }
  if (dims[2] <= m_ZMax)
  {

    QString ss = QObject::tr("The Max Z value (%1) is greater than the Image Geometry Z entent (%2)."
                             " This may lead to junk data being filled into the extra space.").arg(m_ZMax).arg(dims[2]);
    setErrorCondition(-952);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  int64_t XP = ( (m_XMax - m_XMin) + 1 );
  int64_t YP = ( (m_YMax - m_YMin) + 1 );
  int64_t ZP = ( (m_ZMax - m_ZMin) + 1 );

  int64_t col = 0, row = 0, plane = 0;
  int64_t colold = 0, rowold = 0, planeold = 0;
  int64_t index = 0;
  int64_t index_old = 0;
  QList<QString> voxelArrayNames = cellAttrMat->getAttributeArrayNames();
  for (int64_t i = 0; i < ZP; i++)
  {
    QString ss = QObject::tr("Cropping Volume || Slice %1 of %2 Complete").arg(i).arg(ZP);
    notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);
    planeold = (i + m_ZMin) * (srcCellDataContainer->getGeometryAs<ImageGeom>()->getXPoints() * srcCellDataContainer->getGeometryAs<ImageGeom>()->getYPoints());
    plane = (i * XP * YP);
    for (int64_t j = 0; j < YP; j++)
    {
      rowold = (j + m_YMin) * srcCellDataContainer->getGeometryAs<ImageGeom>()->getXPoints();
      row = (j * XP);
      for (int64_t k = 0; k < XP; k++)
      {
        colold = (k + m_XMin);
        col = k;
        index_old = planeold + rowold + colold;
        index = plane + row + col;
        for (QList<QString>::iterator iter = voxelArrayNames.begin(); iter != voxelArrayNames.end(); ++iter)
        {
          IDataArray::Pointer p = cellAttrMat->getAttributeArray(*iter);
          p->copyTuple(index_old, index);
        }
      }
    }
  }
  destCellDataContainer->getGeometryAs<ImageGeom>()->setDimensions(static_cast<size_t>(XP), static_cast<size_t>(YP), static_cast<size_t>(ZP));
  totalPoints = destCellDataContainer->getGeometryAs<ImageGeom>()->getNumberOfElements();
  QVector<size_t> tDims(3, 0);
  tDims[0] = XP;
  tDims[1] = YP;
  tDims[2] = ZP;
  cellAttrMat->setTupleDimensions(tDims); // THIS WILL CAUSE A RESIZE of all the underlying data arrays.

  if (m_RenumberFeatures == true)
  {
    totalPoints = destCellDataContainer->getGeometryAs<ImageGeom>()->getNumberOfElements();

    // This just sanity checks to make sure there were existing features before the cropping
    AttributeMatrix::Pointer cellFeatureAttrMat = srcCellDataContainer->getAttributeMatrix(getCellFeatureAttributeMatrixPath().getAttributeMatrixName());
    size_t totalFeatures = cellFeatureAttrMat->getNumberOfTuples();
    QVector<bool> activeObjects(totalFeatures, false);
    if (0 == totalFeatures)
    {
      setErrorCondition(-600);
      notifyErrorMessage(getHumanLabel(), "The number of Features is 0 and should be greater than 0", getErrorCondition());
      return;
    }

    //QVector<size_t> cDims(1, 1);
    DataArrayPath dap = getFeatureIdsArrayPath();
    if(getSaveAsNewDataContainer())
    {
      dap.setDataContainerName(getNewDataContainerName());
    }
    m_FeatureIdsPtr = cellAttrMat->getAttributeArrayAs<Int32ArrayType>(dap.getDataArrayName()); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if( nullptr != m_FeatureIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
    { m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
    else
    {
      setErrorCondition(-601);
      QString ss = QObject::tr("The FeatureIds array with name '%1' was not found in the destination DataContainer. The expected path was '%2'")
                   .arg(dap.getDataArrayName()).arg(dap.serialize("/"));
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
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
        QString ss = QObject::tr("The total number of Features from %1 is %2, but a value of %3 was found in DataArray %4.").arg(cellFeatureAttrMat->getName()).arg(totalFeatures).arg(currentFeatureId).arg(getFeatureIdsArrayPath().serialize("/"));
        qDebug() << ss;
        notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
        return;
      }
    }
    cellFeatureAttrMat->removeInactiveObjects(activeObjects, m_FeatureIdsPtr.lock());
  }

  if(m_UpdateOrigin == true)
  {
    float resolution[3] = {0.0f, 0.0f, 0.0f};
    destCellDataContainer->getGeometryAs<ImageGeom>()->getResolution(resolution);

    float origin[3] = {0.0f, 0.0f, 0.0f};
    destCellDataContainer->getGeometryAs<ImageGeom>()->getOrigin(origin);

    origin[0] = m_XMin * resolution[0] + oldOrigin[0];
    origin[1] = m_YMin * resolution[1] + oldOrigin[1];
    origin[2] = m_ZMin * resolution[2] + oldOrigin[2];

    destCellDataContainer->getGeometryAs<ImageGeom>()->setOrigin(origin);
  }

  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IntVec3_t CropImageGeometry::getCurrentVolumeDataContainerDimensions()
{
  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getCellAttributeMatrixPath().getDataContainerName());

  IntVec3_t data;
  if (nullptr != m.get() )
  {
    ImageGeom::Pointer image = m->getGeometryAs<ImageGeom>();
    if (image.get() != nullptr)
    {
      data.x = image->getXPoints();
      data.y = image->getYPoints();
      data.z = image->getZPoints();
    }
    else
    {
      data.x = 0;
      data.y = 0;
      data.z = 0;
    }
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
FloatVec3_t CropImageGeometry::getCurrentVolumeDataContainerResolutions()
{
  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getCellAttributeMatrixPath().getDataContainerName());
  FloatVec3_t data;
  if (nullptr != m)
  {
    ImageGeom::Pointer image = m->getGeometryAs<ImageGeom>();
    if (image.get() != nullptr)
    {
      data.x = image->getXRes();
      data.y = image->getYRes();
      data.z = image->getZRes();
    }
    else
    {
      data.x = 0;
      data.y = 0;
      data.z = 0;
    }

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
AbstractFilter::Pointer CropImageGeometry::newFilterInstance(bool copyFilterParameters)
{
  CropImageGeometry::Pointer filter = CropImageGeometry::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CropImageGeometry::getCompiledLibraryName()
{
  return SamplingConstants::SamplingBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CropImageGeometry::getBrandingString()
{
  return "Sampling";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CropImageGeometry::getFilterVersion()
{
  QString version;
  QTextStream vStream(&version);
  vStream <<  Sampling::Version::Major() << "." << Sampling::Version::Minor() << "." << Sampling::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CropImageGeometry::getGroupName()
{ return SIMPL::FilterGroups::SamplingFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CropImageGeometry::getHumanLabel()
{ return "Crop Geometry (Image)"; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CropImageGeometry::getSubGroupName()
{ return SIMPL::FilterSubGroups::CropCutFilters; }
