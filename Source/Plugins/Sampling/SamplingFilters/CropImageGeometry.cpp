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
#include "SIMPLib/FilterParameters/AttributeMatrixSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/BooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/IntFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedBooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/FilterParameters/PreflightUpdatedValueFilterParameter.h"
#include "SIMPLib/Geometry/ImageGeom.h"
#include "SIMPLib/Math/SIMPLibRandom.h"

#include "Sampling/SamplingConstants.h"
#include "Sampling/SamplingVersion.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CropImageGeometry::CropImageGeometry()
: m_NewDataContainerName(SIMPL::Defaults::NewImageDataContainerName)
, m_CellAttributeMatrixPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellAttributeMatrixName, "")
, m_CellFeatureAttributeMatrixPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellFeatureAttributeMatrixName, "")
, m_XMin(0)
, m_YMin(0)
, m_ZMin(0)
, m_XMax(0)
, m_YMax(0)
, m_ZMax(0)
, m_RenumberFeatures(false)
, m_SaveAsNewDataContainer(false)
, m_UpdateOrigin(true)
, m_FeatureIdsArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellAttributeMatrixName, SIMPL::CellData::FeatureIds)
{
  m_OldDimensions.x = 0, m_OldDimensions.y = 0; m_OldDimensions.z = 0;
  m_OldResolution.x = 0.0f; m_OldResolution.y = 0.0f; m_OldResolution.z = 0.0f;
  m_OldOrigin.x = 0.0f; m_OldOrigin.y = 0.0f; m_OldOrigin.z = 0.0f;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CropImageGeometry::~CropImageGeometry() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CropImageGeometry::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(SIMPL_NEW_INTEGER_FP("X Min Voxel (Column) ", XMin, FilterParameter::Parameter, CropImageGeometry));
  parameters.push_back(SIMPL_NEW_INTEGER_FP("Y Min Voxel (Row)", YMin, FilterParameter::Parameter, CropImageGeometry));
  parameters.push_back(SIMPL_NEW_INTEGER_FP("Z Min Voxel (Plane)", ZMin, FilterParameter::Parameter, CropImageGeometry));
  parameters.push_back(SIMPL_NEW_INTEGER_FP("X Max Voxel (Column) [Inclusive]", XMax, FilterParameter::Parameter, CropImageGeometry));
  parameters.push_back(SIMPL_NEW_INTEGER_FP("Y Max Voxel (Row) [Inclusive]", YMax, FilterParameter::Parameter, CropImageGeometry));
  parameters.push_back(SIMPL_NEW_INTEGER_FP("Z Max Voxel (Plane) [Inclusive]", ZMax, FilterParameter::Parameter, CropImageGeometry));
  
  PreflightUpdatedValueFilterParameter::Pointer param =
      SIMPL_NEW_PREFLIGHTUPDATEDVALUE_FP("Old Box Size in Length Units", OldBoxDimensions, FilterParameter::Parameter, CropImageGeometry);
  param->setReadOnly(true);
  parameters.push_back(param);
  
  param = SIMPL_NEW_PREFLIGHTUPDATEDVALUE_FP("New Box Size in Length Units", NewBoxDimensions, FilterParameter::Parameter, CropImageGeometry);
  param->setReadOnly(true);
  parameters.push_back(param);
  
  QStringList linkedProps;
  linkedProps << "NewDataContainerName";
  parameters.push_back(SIMPL_NEW_BOOL_FP("Update Origin", UpdateOrigin, FilterParameter::Parameter, CropImageGeometry));
  parameters.push_back(SIMPL_NEW_LINKED_BOOL_FP("Save As New Data Container", SaveAsNewDataContainer, FilterParameter::Parameter, CropImageGeometry, linkedProps));
  parameters.push_back(SIMPL_NEW_STRING_FP("Data Container", NewDataContainerName, FilterParameter::CreatedArray, CropImageGeometry));

  parameters.push_back(SeparatorFilterParameter::New("Cell Data", FilterParameter::RequiredArray));
  {
    AttributeMatrixSelectionFilterParameter::RequirementType req = AttributeMatrixSelectionFilterParameter::CreateRequirement(AttributeMatrix::Type::Cell, IGeometry::Type::Image);
    parameters.push_back(SIMPL_NEW_AM_SELECTION_FP("Cell Attribute Matrix", CellAttributeMatrixPath, FilterParameter::RequiredArray, CropImageGeometry, req));
  }

  parameters.push_back(SeparatorFilterParameter::New("Renumber Features Parameters", FilterParameter::RequiredArray));
  linkedProps.clear();
  linkedProps << "CellFeatureAttributeMatrixPath"
              << "FeatureIdsArrayPath";
  parameters.push_back(SIMPL_NEW_LINKED_BOOL_FP("Renumber Features", RenumberFeatures, FilterParameter::Parameter, CropImageGeometry, linkedProps));
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Int32, 1, AttributeMatrix::Type::Cell, IGeometry::Type::Image);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Feature Ids", FeatureIdsArrayPath, FilterParameter::RequiredArray, CropImageGeometry, req));
  }
  // parameters.push_back(SeparatorFilterParameter::New("Cell Feature Data", FilterParameter::RequiredArray));
  {
    AttributeMatrixSelectionFilterParameter::RequirementType req = AttributeMatrixSelectionFilterParameter::CreateRequirement(AttributeMatrix::Type::CellFeature, IGeometry::Type::Image);
    parameters.push_back(SIMPL_NEW_AM_SELECTION_FP("Cell Feature Attribute Matrix", CellFeatureAttributeMatrixPath, FilterParameter::RequiredArray, CropImageGeometry, req));
  }
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CropImageGeometry::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setNewDataContainerName(reader->readString("NewDataContainerName", getNewDataContainerName()));
  setCellAttributeMatrixPath(reader->readDataArrayPath("CellAttributeMatrixPath", getCellAttributeMatrixPath()));
  setCellFeatureAttributeMatrixPath(reader->readDataArrayPath("CellFeatureAttributeMatrixPath", getCellFeatureAttributeMatrixPath()));
  setFeatureIdsArrayPath(reader->readDataArrayPath("FeatureIdsArrayPath", getFeatureIdsArrayPath()));
  setXMin(reader->readValue("XMin", getXMin()));
  setYMin(reader->readValue("YMin", getYMin()));
  setZMin(reader->readValue("ZMin", getZMin()));
  setXMax(reader->readValue("XMax", getXMax()));
  setYMax(reader->readValue("YMax", getYMax()));
  setZMax(reader->readValue("ZMax", getZMax()));
  setRenumberFeatures(reader->readValue("RenumberFeatures", getRenumberFeatures()));
  setSaveAsNewDataContainer(reader->readValue("SaveAsNewDataContainer", getSaveAsNewDataContainer()));
  setUpdateOrigin(reader->readValue("UpdateOrigin", getUpdateOrigin()));
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
  if(getErrorCondition() < 0)
  {
    return;
  }
  setErrorCondition(0);
  setWarningCondition(0);

  // Validate the incoming DataContainer, Geometry, and AttributeMatrix ; bail if any do not exist since we plan on using them later on in the dataCheck
  // Error messages are handled by the getPrereq functions
  DataContainer::Pointer srcCellDataContainer = getDataContainerArray()->getPrereqDataContainer(this, getCellAttributeMatrixPath().getDataContainerName());
  ImageGeom::Pointer image = getDataContainerArray()->getPrereqGeometryFromDataContainer<ImageGeom, AbstractFilter>(this, getCellAttributeMatrixPath().getDataContainerName());
  AttributeMatrix::Pointer srcCellAttrMat = getDataContainerArray()->getPrereqAttributeMatrixFromPath<AbstractFilter>(this, getCellAttributeMatrixPath(), -301);
  if(getErrorCondition() < 0)
  {
    return;
  }

  m_OldDimensions = getCurrentVolumeDataContainerDimensions();
  m_OldResolution = getCurrentVolumeDataContainerResolutions();
  if(image)
  {
    image->getOrigin(m_OldOrigin.x, m_OldOrigin.y, m_OldOrigin.z);
  } 
  
  DataContainer::Pointer destCellDataContainer = srcCellDataContainer;
  AttributeMatrix::Pointer destCellAttrMat;

  if(m_SaveAsNewDataContainer)
  {
    destCellDataContainer = getDataContainerArray()->createNonPrereqDataContainer<AbstractFilter>(this, getNewDataContainerName());
    if(nullptr == destCellDataContainer.get() || getErrorCondition() < 0)
    {
      return;
    }
    IGeometry::Pointer imageCopy = image->deepCopy();
    destCellDataContainer->setGeometry(imageCopy);

    destCellAttrMat = srcCellAttrMat->deepCopy(getInPreflight());
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

  if(getXMax() < getXMin())
  {
    QString ss = QObject::tr("X Max (%1) less than X Min (%2)").arg(getXMax()).arg(getXMin());
    notifyErrorMessage("", ss, -5550);
  }
  if(getYMax() < getYMin())
  {
    QString ss = QObject::tr("Y Max (%1) less than Y Min (%2)").arg(getYMax()).arg(getYMin());
    notifyErrorMessage("", ss, -5550);
  }
  if(getZMax() < getZMin())
  {
    QString ss = QObject::tr("Z Max (%1) less than Z Min (%2)").arg(getZMax()).arg(getZMin());
    notifyErrorMessage("", ss, -5550);
  }
  if(getXMin() < 0)
  {
    QString ss = QObject::tr("X Min (%1) less than 0").arg(getXMin());
    notifyErrorMessage("", ss, -5550);
  }
  if(getYMin() < 0)
  {
    QString ss = QObject::tr("Y Min (%1) less than 0").arg(getYMin());
    notifyErrorMessage("", ss, -5550);
  }
  if(getZMin() < 0)
  {
    QString ss = QObject::tr("Z Min (%1) less than 0").arg(getZMin());
    notifyErrorMessage("", ss, -5550);
  }

  if(getXMax() > (static_cast<int64_t>(destCellDataContainer->getGeometryAs<ImageGeom>()->getXPoints()) - 1))
  {
    QString ss =
        QObject::tr("The X Max (%1) is greater than the Image Geometry X extent (%2)").arg(getXMax()).arg(static_cast<int64_t>(destCellDataContainer->getGeometryAs<ImageGeom>()->getXPoints()) - 1);
    notifyErrorMessage("", ss, -5550);
  }

  if(getYMax() > (static_cast<int64_t>(destCellDataContainer->getGeometryAs<ImageGeom>()->getYPoints()) - 1))
  {
    QString ss =
        QObject::tr("The Y Max (%1) is greater than the Image Geometry Y extent (%2)").arg(getYMax()).arg(static_cast<int64_t>(destCellDataContainer->getGeometryAs<ImageGeom>()->getYPoints()) - 1);
    notifyErrorMessage("", ss, -5550);
  }

  if(getZMax() > (static_cast<int64_t>(destCellDataContainer->getGeometryAs<ImageGeom>()->getZPoints()) - 1))
  {
    QString ss =
        QObject::tr("The Z Max (%1) is greater than the Image Geometry Z extent (%2)").arg(getZMax()).arg(static_cast<int64_t>(destCellDataContainer->getGeometryAs<ImageGeom>()->getZPoints()) - 1);
    notifyErrorMessage("", ss, -5550);
  }

  QVector<size_t> tDims(3, 0);
  if(getXMax() - getXMin() < 0)
  {
    setXMax(getXMin() + 1);
  }
  if(getYMax() - getYMin() < 0)
  {
    setYMax(getYMin() + 1);
  }
  if(getZMax() - getZMin() < 0)
  {
    setZMax(getZMin() + 1);
  }

  
  tDims[0] = (getXMax() - getXMin()) + 1;
  tDims[1] = (getYMax() - getYMin()) + 1;
  tDims[2] = (getZMax() - getZMin()) + 1;
  
  m_NewDimensions.x = tDims[0];
  m_NewDimensions.y = tDims[1];
  m_NewDimensions.z = tDims[2];
  m_NewResolution = m_OldResolution;

  if(m_UpdateOrigin)
  {
    m_NewOrigin.x = getXMin() * m_NewResolution.x + m_OldOrigin.x;
    m_NewOrigin.y = getYMin() * m_NewResolution.y + m_OldOrigin.y;
    m_NewOrigin.z = getZMin() * m_NewResolution.z + m_OldOrigin.z;
  }
  else
  {
    m_NewOrigin = m_OldOrigin;
  }
  destCellDataContainer->getGeometryAs<ImageGeom>()->setDimensions(tDims[0], tDims[1], tDims[2]);

  // If any of the sanity checks fail above then we should NOT attempt to go any further.
  if(getErrorCondition() < 0)
  {
    return;
  }

  size_t totalPoints = 1;
  for(int i = 0; i < 3; i++)
  {
    if(tDims[i] != 0)
    {
      totalPoints *= tDims[i];
    }
  }
  AttributeMatrix::Pointer newCellAttrMat = AttributeMatrix::New(tDims, destCellAttrMat->getName(), destCellAttrMat->getType());

  QList<QString> voxelArrayNames = destCellAttrMat->getAttributeArrayNames();
  for(QList<QString>::iterator iter = voxelArrayNames.begin(); iter != voxelArrayNames.end(); ++iter)
  {
    IDataArray::Pointer p = destCellAttrMat->getAttributeArray(*iter);
    //
    IDataArray::Pointer data = p->createNewArray(totalPoints, p->getComponentDimensions(), p->getName(), false);

    destCellAttrMat->removeAttributeArray(*iter);
    newCellAttrMat->addAttributeArray(*iter, data);
  }
  destCellDataContainer->removeAttributeMatrix(destCellAttrMat->getName());
  destCellDataContainer->addAttributeMatrix(newCellAttrMat->getName(), newCellAttrMat);

  if(m_RenumberFeatures)
  {
    QVector<size_t> cDims(1, 1);
    m_FeatureIdsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(nullptr, getFeatureIdsArrayPath(), cDims);
    if(nullptr != m_FeatureIdsPtr.lock())
    {
      m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0);
    }
    else
    {
      QString ss = QObject::tr("The DataArray '%1' which defines the Feature Ids to renumber is invalid. Does it exist? Is it the correct type?").arg(getFeatureIdsArrayPath().serialize("/"));
      notifyErrorMessage("", ss, -55500);
    }

    AttributeMatrix::Pointer cellFeatureAttrMat = srcCellDataContainer->getPrereqAttributeMatrix(nullptr, getCellFeatureAttributeMatrixPath().getAttributeMatrixName(), -56);
    if(nullptr == cellFeatureAttrMat.get())
    {
      QString ss = QObject::tr("The AttributeMatrix '%1' is invalid. Does it exist? Is it the correct type?. The AttributeMatrix defines where the segmented features are stored.")
                       .arg(getCellFeatureAttributeMatrixPath().serialize("/"));
      notifyErrorMessage("", ss, -55501);
      return;
    }

    QVector<bool> activeObjects(cellFeatureAttrMat->getNumberOfTuples(), true);
    cellFeatureAttrMat->removeInactiveObjects(activeObjects, m_FeatureIdsPtr.lock().get());
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
  setWarningCondition(0);

  /* Normally, filters call dataCheck during the execute to reuse code.  Unfortunately,
   * this cannot happen for this filter, because calling dataCheck would destroy
   * an Attribute Matrix that we need during the execute.  Do not uncomment the code,
   * and be careful when reusing code from either of these functions.  Make sure
   * you understand how this works before you reuse any code. */
  // dataCheck();
  // if(getErrorCondition() < 0) { return; }

  DataContainer::Pointer srcCellDataContainer = getDataContainerArray()->getPrereqDataContainer(this, getCellAttributeMatrixPath().getDataContainerName());
  AttributeMatrix::Pointer cellAttrMat = srcCellDataContainer->getAttributeMatrix(getCellAttributeMatrixPath().getAttributeMatrixName());
  DataContainer::Pointer destCellDataContainer = srcCellDataContainer;

  if(m_SaveAsNewDataContainer)
  {
    float ox = 0.0f, oy = 0.0f, oz = 0.0f, rx = 0.0f, ry = 0.0f, rz = 0.0f;
    srcCellDataContainer->getGeometryAs<ImageGeom>()->getOrigin(ox, oy, oz);
    srcCellDataContainer->getGeometryAs<ImageGeom>()->getResolution(rx, ry, rz);

    destCellDataContainer = getDataContainerArray()->createNonPrereqDataContainer<AbstractFilter>(this, getNewDataContainerName());
    ImageGeom::Pointer image = ImageGeom::CreateGeometry(SIMPL::Geometry::ImageGeometry);
    destCellDataContainer->setGeometry(image);

    destCellDataContainer->getGeometryAs<ImageGeom>()->setOrigin(ox, oy, oz);
    destCellDataContainer->getGeometryAs<ImageGeom>()->setResolution(rx, ry, rz);

    AttributeMatrix::Pointer cellAttrMatCopy = cellAttrMat->deepCopy(false);
    destCellDataContainer->addAttributeMatrix(cellAttrMatCopy->getName(), cellAttrMatCopy);
    cellAttrMat = destCellDataContainer->getAttributeMatrix(getCellAttributeMatrixPath().getAttributeMatrixName());
  }

  if(nullptr == destCellDataContainer.get() || nullptr == cellAttrMat.get() || getErrorCondition() < 0)
  {
    return;
  }

  // No matter where the AM is (same DC or new DC), we have the correct DC and AM pointers...now it's time to crop
  int64_t totalPoints = cellAttrMat->getNumberOfTuples();

  size_t udims[3] = {0, 0, 0};
  std::tie(udims[0], udims[1], udims[2]) = srcCellDataContainer->getGeometryAs<ImageGeom>()->getDimensions();

  int64_t dims[3] = {
      static_cast<int64_t>(udims[0]), static_cast<int64_t>(udims[1]), static_cast<int64_t>(udims[2]),
  };

  // Check to see if the dims have actually changed.
  if(dims[0] == (m_XMax - m_XMin) && dims[1] == (m_YMax - m_YMin) && dims[2] == (m_ZMax - m_ZMin))
  {
    return;
  }

  // Get current origin
  float oldOrigin[3] = {0.0f, 0.0f, 0.0f};
  destCellDataContainer->getGeometryAs<ImageGeom>()->getOrigin(oldOrigin);

  // Check to make sure the new dimensions are not "out of bounds" and warn the user if they are
  if(dims[0] <= m_XMax)
  {
    QString ss = QObject::tr("The Max X value (%1) is greater than the Image Geometry X entent (%2)."
                             " This may lead to junk data being filled into the extra space.")
                     .arg(m_XMax)
                     .arg(dims[0]);
    notifyErrorMessage("", ss, -950);
    return;
  }
  if(dims[1] <= m_YMax)
  {

    QString ss = QObject::tr("The Max Y value (%1) is greater than the Image Geometry Y entent (%2)."
                             " This may lead to junk data being filled into the extra space.")
                     .arg(m_YMax)
                     .arg(dims[1]);
    notifyErrorMessage("", ss, -951);
    return;
  }
  if(dims[2] <= m_ZMax)
  {

    QString ss = QObject::tr("The Max Z value (%1) is greater than the Image Geometry Z entent (%2)."
                             " This may lead to junk data being filled into the extra space.")
                     .arg(m_ZMax)
                     .arg(dims[2]);
    notifyErrorMessage("", ss, -952);
    return;
  }

  int64_t XP = ((m_XMax - m_XMin) + 1);
  int64_t YP = ((m_YMax - m_YMin) + 1);
  int64_t ZP = ((m_ZMax - m_ZMin) + 1);

  int64_t col = 0, row = 0, plane = 0;
  int64_t colold = 0, rowold = 0, planeold = 0;
  int64_t index = 0;
  int64_t index_old = 0;
  QList<QString> voxelArrayNames = cellAttrMat->getAttributeArrayNames();
  for(int64_t i = 0; i < ZP; i++)
  {
    if(getCancel())
    {
      break;
    }
    QString ss = QObject::tr("Cropping Volume || Slice %1 of %2 Complete").arg(i).arg(ZP);
    notifyStatusMessage(getMessagePrefix(), ss);
    planeold = (i + m_ZMin) * (srcCellDataContainer->getGeometryAs<ImageGeom>()->getXPoints() * srcCellDataContainer->getGeometryAs<ImageGeom>()->getYPoints());
    plane = (i * XP * YP);
    for(int64_t j = 0; j < YP; j++)
    {
      rowold = (j + m_YMin) * srcCellDataContainer->getGeometryAs<ImageGeom>()->getXPoints();
      row = (j * XP);
      for(int64_t k = 0; k < XP; k++)
      {
        colold = (k + m_XMin);
        col = k;
        index_old = planeold + rowold + colold;
        index = plane + row + col;
        for(const auto& arrayName : voxelArrayNames)
        {
          IDataArray::Pointer p = cellAttrMat->getAttributeArray(arrayName);
          p->copyTuple(index_old, index);
        }
      }
    }
  }
  if(getCancel())
  {
    return;
  }
  destCellDataContainer->getGeometryAs<ImageGeom>()->setDimensions(static_cast<size_t>(XP), static_cast<size_t>(YP), static_cast<size_t>(ZP));
  totalPoints = destCellDataContainer->getGeometryAs<ImageGeom>()->getNumberOfElements();
  QVector<size_t> tDims(3, 0);
  tDims[0] = XP;
  tDims[1] = YP;
  tDims[2] = ZP;
  cellAttrMat->setTupleDimensions(tDims); // THIS WILL CAUSE A RESIZE of all the underlying data arrays.

  if(m_RenumberFeatures)
  {
    totalPoints = destCellDataContainer->getGeometryAs<ImageGeom>()->getNumberOfElements();

    // This just sanity checks to make sure there were existing features before the cropping
    AttributeMatrix::Pointer cellFeatureAttrMat = srcCellDataContainer->getAttributeMatrix(getCellFeatureAttributeMatrixPath().getAttributeMatrixName());
    size_t totalFeatures = cellFeatureAttrMat->getNumberOfTuples();
    QVector<bool> activeObjects(totalFeatures, false);
    if(0 == totalFeatures)
    {
      notifyErrorMessage("", "The number of Features is 0 and should be greater than 0", -600);
      return;
    }

    // QVector<size_t> cDims(1, 1);
    DataArrayPath dap = getFeatureIdsArrayPath();
    if(getSaveAsNewDataContainer())
    {
      dap.setDataContainerName(getNewDataContainerName());
    }
    m_FeatureIdsPtr = cellAttrMat->getAttributeArrayAs<Int32ArrayType>(dap.getDataArrayName()); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if(nullptr != m_FeatureIdsPtr.lock())                                                       /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
    {
      m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0);
    } /* Now assign the raw pointer to data from the DataArray<T> object */
    else
    {
      QString ss = QObject::tr("The FeatureIds array with name '%1' was not found in the destination DataContainer. The expected path was '%2'").arg(dap.getDataArrayName()).arg(dap.serialize("/"));
      notifyErrorMessage("", ss, -601);
      return;
    }

    // Find the unique set of feature ids
    for(int64_t i = 0; i < totalPoints; ++i)
    {
      if(getCancel())
      {
        break;
      }

      int32_t currentFeatureId = m_FeatureIds[i];
      if(currentFeatureId < totalFeatures)
      {
        activeObjects[currentFeatureId] = true;
      }
      else
      {
        QString ss = QObject::tr("The total number of Features from %1 is %2, but a value of %3 was found in DataArray %4.")
                         .arg(cellFeatureAttrMat->getName())
                         .arg(totalFeatures)
                         .arg(currentFeatureId)
                         .arg(getFeatureIdsArrayPath().serialize("/"));
        qDebug() << ss;
        notifyErrorMessage("", ss, -601);
        return;
      }
    }
    cellFeatureAttrMat->removeInactiveObjects(activeObjects, m_FeatureIdsPtr.lock().get());
  }

  if(m_UpdateOrigin)
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

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IntVec3_t CropImageGeometry::getCurrentVolumeDataContainerDimensions()
{
  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getCellAttributeMatrixPath().getDataContainerName());

  IntVec3_t data;
  if(nullptr != m.get())
  {
    ImageGeom::Pointer image = m->getGeometryAs<ImageGeom>();
    if(image.get() != nullptr)
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
  if(nullptr != m)
  {
    ImageGeom::Pointer image = m->getGeometryAs<ImageGeom>();
    if(image.get() != nullptr)
    {
      std::tie(data.x, data.y, data.z) = image->getResolution();
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
QString CropImageGeometry::getOldBoxDimensions()
{
  QString desc = QString("Please select a Data Container that has an Image Geometry");
  DataContainerArray::Pointer dca = getDataContainerArray();
  if(nullptr != dca)
  {
    DataContainer::Pointer m = dca->getDataContainer(getCellAttributeMatrixPath().getDataContainerName());
    if(nullptr != m)
    {
      ImageGeom::Pointer image = m->getGeometryAs<ImageGeom>();
      if(nullptr != image)
      {
        desc.clear();
        QTextStream ss(&desc);
        ss << "X Range: " << m_OldOrigin.x << " to " << (m_OldOrigin.x + (m_OldDimensions.x * m_OldResolution.x)) << " (Delta: " << (m_OldDimensions.x * m_OldResolution.x) << ") " 
           << 0 << "-" <<  m_OldDimensions.x-1 << " Voxels\n";
        ss << "Y Range: " << m_OldOrigin.y << " to " << (m_OldOrigin.y + (m_OldDimensions.y * m_OldResolution.y)) << " (Delta: " << (m_OldDimensions.y * m_OldResolution.y) << ") "
           << 0 << "-" <<  m_OldDimensions.y-1 << " Voxels\n";
        ss << "Z Range: " << m_OldOrigin.z << " to " << (m_OldOrigin.z + (m_OldDimensions.z * m_OldResolution.z)) << " (Delta: " << (m_OldDimensions.z * m_OldResolution.z) << ") "
           << 0 << "-" <<  m_OldDimensions.z-1 << " Voxels\n";
      }
    }
  }
  return desc;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString CropImageGeometry::getNewBoxDimensions()
{
  QString desc = QString("Please select a Data Container that has an Image Geometry");
  DataContainerArray::Pointer dca = getDataContainerArray();
  if(nullptr != dca)
  {
    DataContainer::Pointer m = dca->getDataContainer(getCellAttributeMatrixPath().getDataContainerName());
    if(nullptr != m)
    {
      ImageGeom::Pointer image = m->getGeometryAs<ImageGeom>();
      if(nullptr != image)
      {
        desc.clear();
        QTextStream ss(&desc);
        ss << "X Range: " << m_NewOrigin.x << " to " << (m_NewOrigin.x + (m_NewDimensions.x * m_NewResolution.x)) << " (Delta: " << (m_NewDimensions.x * m_NewResolution.x) << ") "
           << 0 << "-" <<  m_NewDimensions.x-1 << " Voxels\n";
        ss << "Y Range: " << m_NewOrigin.y << " to " << (m_NewOrigin.y + (m_NewDimensions.y * m_NewResolution.y)) << " (Delta: " << (m_NewDimensions.y * m_NewResolution.y) << ") "
           << 0 << "-" <<  m_NewDimensions.y-1 << " Voxels\n";
        ss << "Z Range: " << m_NewOrigin.z << " to " << (m_NewOrigin.z + (m_NewDimensions.z * m_NewResolution.z)) << " (Delta: " << (m_NewDimensions.z * m_NewResolution.z) << ") "
           << 0 << "-" <<  m_NewDimensions.z-1 << " Voxels\n";
      }
    }
  }
  return desc;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer CropImageGeometry::newFilterInstance(bool copyFilterParameters) const
{
  CropImageGeometry::Pointer filter = CropImageGeometry::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CropImageGeometry::getCompiledLibraryName() const
{
  return SamplingConstants::SamplingBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CropImageGeometry::getBrandingString() const
{
  return "Sampling";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CropImageGeometry::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << Sampling::Version::Major() << "." << Sampling::Version::Minor() << "." << Sampling::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CropImageGeometry::getGroupName() const
{
  return SIMPL::FilterGroups::SamplingFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CropImageGeometry::getHumanLabel() const
{
  return "Crop Geometry (Image)";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QUuid CropImageGeometry::getUuid()
{
  return QUuid("{baa4b7fe-31e5-5e63-a2cb-0bb9d844cfaf}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CropImageGeometry::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::CropCutFilters;
}
