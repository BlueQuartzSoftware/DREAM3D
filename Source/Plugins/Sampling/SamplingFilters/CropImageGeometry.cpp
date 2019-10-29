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

#include "CropImageGeometry.h"

#include <QtCore/QTextStream>

#include <QtCore/QDebug>

#include "SIMPLib/Common/Constants.h"

#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/AttributeMatrixSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/BooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/DataContainerCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/IntFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedBooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/PreflightUpdatedValueFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/Geometry/ImageGeom.h"
#include "SIMPLib/Math/SIMPLibRandom.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/DataContainers/DataContainer.h"

#include "Sampling/SamplingConstants.h"
#include "Sampling/SamplingVersion.h"

enum createdPathID : RenameDataPath::DataID_t
{
  DataContainerID = 1
};

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
  m_OldDimensions[0] = 0, m_OldDimensions[1] = 0;
  m_OldDimensions[2] = 0;
  m_OldResolution[0] = 0.0f;
  m_OldResolution[1] = 0.0f;
  m_OldResolution[2] = 0.0f;
  m_OldOrigin[0] = 0.0f;
  m_OldOrigin[1] = 0.0f;
  m_OldOrigin[2] = 0.0f;
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
  FilterParameterVectorType parameters;
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
  parameters.push_back(SIMPL_NEW_DC_CREATION_FP("Data Container", NewDataContainerName, FilterParameter::CreatedArray, CropImageGeometry));

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
  setNewDataContainerName(reader->readDataArrayPath("NewDataContainerName", getNewDataContainerName()));
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
  if(getErrorCode() < 0)
  {
    return;
  }
  clearErrorCode();
  clearWarningCode();

  // Validate the incoming DataContainer, Geometry, and AttributeMatrix ; bail if any do not exist since we plan on using them later on in the dataCheck
  // Error messages are handled by the getPrereq functions
  DataContainer::Pointer srcCellDataContainer = getDataContainerArray()->getPrereqDataContainer(this, getCellAttributeMatrixPath().getDataContainerName());
  ImageGeom::Pointer image = getDataContainerArray()->getPrereqGeometryFromDataContainer<ImageGeom, AbstractFilter>(this, getCellAttributeMatrixPath().getDataContainerName());
  AttributeMatrix::Pointer srcCellAttrMat = getDataContainerArray()->getPrereqAttributeMatrixFromPath<AbstractFilter>(this, getCellAttributeMatrixPath(), -301);
  if(getErrorCode() < 0)
  {
    return;
  }

  m_OldDimensions = getCurrentVolumeDataContainerDimensions();
  m_OldResolution = getCurrentVolumeDataContainerResolutions();
  if(image)
  {
    m_OldOrigin = image->getOrigin();
  }

  DataContainer::Pointer destCellDataContainer = srcCellDataContainer;
  AttributeMatrix::Pointer destCellAttrMat;

  if(m_SaveAsNewDataContainer)
  {
    destCellDataContainer = getDataContainerArray()->createNonPrereqDataContainer<AbstractFilter>(this, getNewDataContainerName(), DataContainerID);
    if(nullptr == destCellDataContainer.get() || getErrorCode() < 0)
    {
      return;
    }
    IGeometry::Pointer imageCopy = image->deepCopy();
    destCellDataContainer->setGeometry(imageCopy);

    destCellAttrMat = srcCellAttrMat->deepCopy(getInPreflight());
    destCellDataContainer->addOrReplaceAttributeMatrix(destCellAttrMat);
  }
  else
  {
    destCellAttrMat = srcCellAttrMat;
  }

  if(nullptr == destCellDataContainer.get() || nullptr == destCellAttrMat.get() || getErrorCode() < 0)
  {
    return;
  }

  if(getXMax() < getXMin())
  {
    QString ss = QObject::tr("X Max (%1) less than X Min (%2)").arg(getXMax()).arg(getXMin());
    setErrorCondition(-5550, ss);
  }
  if(getYMax() < getYMin())
  {
    QString ss = QObject::tr("Y Max (%1) less than Y Min (%2)").arg(getYMax()).arg(getYMin());
    setErrorCondition(-5550, ss);
  }
  if(getZMax() < getZMin())
  {
    QString ss = QObject::tr("Z Max (%1) less than Z Min (%2)").arg(getZMax()).arg(getZMin());
    setErrorCondition(-5550, ss);
  }
  if(getXMin() < 0)
  {
    QString ss = QObject::tr("X Min (%1) less than 0").arg(getXMin());
    setErrorCondition(-5550, ss);
  }
  if(getYMin() < 0)
  {
    QString ss = QObject::tr("Y Min (%1) less than 0").arg(getYMin());
    setErrorCondition(-5550, ss);
  }
  if(getZMin() < 0)
  {
    QString ss = QObject::tr("Z Min (%1) less than 0").arg(getZMin());
    setErrorCondition(-5550, ss);
  }

  if(getXMax() > (static_cast<int64_t>(destCellDataContainer->getGeometryAs<ImageGeom>()->getXPoints()) - 1))
  {
    QString ss =
        QObject::tr("The X Max (%1) is greater than the Image Geometry X extent (%2)").arg(getXMax()).arg(static_cast<int64_t>(destCellDataContainer->getGeometryAs<ImageGeom>()->getXPoints()) - 1);
    setErrorCondition(-5550, ss);
  }

  if(getYMax() > (static_cast<int64_t>(destCellDataContainer->getGeometryAs<ImageGeom>()->getYPoints()) - 1))
  {
    QString ss =
        QObject::tr("The Y Max (%1) is greater than the Image Geometry Y extent (%2)").arg(getYMax()).arg(static_cast<int64_t>(destCellDataContainer->getGeometryAs<ImageGeom>()->getYPoints()) - 1);
    setErrorCondition(-5550, ss);
  }

  if(getZMax() > (static_cast<int64_t>(destCellDataContainer->getGeometryAs<ImageGeom>()->getZPoints()) - 1))
  {
    QString ss =
        QObject::tr("The Z Max (%1) is greater than the Image Geometry Z extent (%2)").arg(getZMax()).arg(static_cast<int64_t>(destCellDataContainer->getGeometryAs<ImageGeom>()->getZPoints()) - 1);
    setErrorCondition(-5550, ss);
  }

  std::vector<size_t> tDims(3, 0);
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

  m_NewDimensions[0] = tDims[0];
  m_NewDimensions[1] = tDims[1];
  m_NewDimensions[2] = tDims[2];
  m_NewResolution = m_OldResolution;

  if(m_UpdateOrigin)
  {
    m_NewOrigin[0] = getXMin() * m_NewResolution[0] + m_OldOrigin[0];
    m_NewOrigin[1] = getYMin() * m_NewResolution[1] + m_OldOrigin[1];
    m_NewOrigin[2] = getZMin() * m_NewResolution[2] + m_OldOrigin[2];
  }
  else
  {
    m_NewOrigin = m_OldOrigin;
  }
  destCellDataContainer->getGeometryAs<ImageGeom>()->setDimensions(tDims[0], tDims[1], tDims[2]);

  // If any of the sanity checks fail above then we should NOT attempt to go any further.
  if(getErrorCode() < 0)
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
    newCellAttrMat->insertOrAssign(data);
  }
  destCellDataContainer->removeAttributeMatrix(destCellAttrMat->getName());
  destCellDataContainer->addOrReplaceAttributeMatrix(newCellAttrMat);

  if(m_RenumberFeatures)
  {
    std::vector<size_t> cDims(1, 1);
    m_FeatureIdsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(nullptr, getFeatureIdsArrayPath(), cDims);
    if(nullptr != m_FeatureIdsPtr.lock())
    {
      m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0);
    }
    else
    {
      QString ss = QObject::tr("The DataArray '%1' which defines the Feature Ids to renumber is invalid. Does it exist? Is it the correct type?").arg(getFeatureIdsArrayPath().serialize("/"));
      setErrorCondition(-55500, ss);
    }

    AttributeMatrix::Pointer cellFeatureAttrMat = srcCellDataContainer->getPrereqAttributeMatrix(nullptr, getCellFeatureAttributeMatrixPath().getAttributeMatrixName(), -56);
    if(nullptr == cellFeatureAttrMat.get())
    {
      QString ss = QObject::tr("The AttributeMatrix '%1' is invalid. Does it exist? Is it the correct type?. The AttributeMatrix defines where the segmented features are stored.")
                       .arg(getCellFeatureAttributeMatrixPath().serialize("/"));
      setErrorCondition(-55501, ss);
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
  clearErrorCode();
  clearWarningCode();

  /* Normally, filters call dataCheck during the execute to reuse code.  Unfortunately,
   * this cannot happen for this filter, because calling dataCheck would destroy
   * an Attribute Matrix that we need during the execute.  Do not uncomment the code,
   * and be careful when reusing code from either of these functions.  Make sure
   * you understand how this works before you reuse any code. */
  // dataCheck();
  // if(getErrorCode() < 0) { return; }

  DataContainer::Pointer srcCellDataContainer = getDataContainerArray()->getPrereqDataContainer(this, getCellAttributeMatrixPath().getDataContainerName());
  AttributeMatrix::Pointer cellAttrMat = srcCellDataContainer->getAttributeMatrix(getCellAttributeMatrixPath().getAttributeMatrixName());
  DataContainer::Pointer destCellDataContainer = srcCellDataContainer;

  if(m_SaveAsNewDataContainer)
  {
    FloatVec3Type o = srcCellDataContainer->getGeometryAs<ImageGeom>()->getOrigin();
    FloatVec3Type r = srcCellDataContainer->getGeometryAs<ImageGeom>()->getSpacing();

    destCellDataContainer = getDataContainerArray()->createNonPrereqDataContainer<AbstractFilter>(this, getNewDataContainerName(), DataContainerID);
    ImageGeom::Pointer image = ImageGeom::CreateGeometry(SIMPL::Geometry::ImageGeometry);
    destCellDataContainer->setGeometry(image);

    destCellDataContainer->getGeometryAs<ImageGeom>()->setOrigin(o);
    destCellDataContainer->getGeometryAs<ImageGeom>()->setSpacing(r);

    AttributeMatrix::Pointer cellAttrMatCopy = cellAttrMat->deepCopy(false);
    destCellDataContainer->addOrReplaceAttributeMatrix(cellAttrMatCopy);
    cellAttrMat = destCellDataContainer->getAttributeMatrix(getCellAttributeMatrixPath().getAttributeMatrixName());
  }

  if(nullptr == destCellDataContainer.get() || nullptr == cellAttrMat.get() || getErrorCode() < 0)
  {
    return;
  }

  // No matter where the AM is (same DC or new DC), we have the correct DC and AM pointers...now it's time to crop
  int64_t totalPoints = cellAttrMat->getNumberOfTuples();

  SizeVec3Type udims = srcCellDataContainer->getGeometryAs<ImageGeom>()->getDimensions();

  int64_t dims[3] = {
      static_cast<int64_t>(udims[0]), static_cast<int64_t>(udims[1]), static_cast<int64_t>(udims[2]),
  };

  // Check to see if the dims have actually changed.
  if(dims[0] == (m_XMax - m_XMin) && dims[1] == (m_YMax - m_YMin) && dims[2] == (m_ZMax - m_ZMin))
  {
    return;
  }

  // Get current origin
  FloatVec3Type oldOrigin = destCellDataContainer->getGeometryAs<ImageGeom>()->getOrigin();

  // Check to make sure the new dimensions are not "out of bounds" and warn the user if they are
  if(dims[0] <= m_XMax)
  {
    QString ss = QObject::tr("The Max X value (%1) is greater than the Image Geometry X entent (%2)."
                             " This may lead to junk data being filled into the extra space.")
                     .arg(m_XMax)
                     .arg(dims[0]);
    setErrorCondition(-950, ss);
    return;
  }
  if(dims[1] <= m_YMax)
  {

    QString ss = QObject::tr("The Max Y value (%1) is greater than the Image Geometry Y entent (%2)."
                             " This may lead to junk data being filled into the extra space.")
                     .arg(m_YMax)
                     .arg(dims[1]);
    setErrorCondition(-951, ss);
    return;
  }
  if(dims[2] <= m_ZMax)
  {

    QString ss = QObject::tr("The Max Z value (%1) is greater than the Image Geometry Z entent (%2)."
                             " This may lead to junk data being filled into the extra space.")
                     .arg(m_ZMax)
                     .arg(dims[2]);
    setErrorCondition(-952, ss);
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
    notifyStatusMessage(ss);
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
  std::vector<size_t> tDims(3, 0);
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
      setErrorCondition(-600, "The number of Features is 0 and should be greater than 0");
      return;
    }

    // std::vector<size_t> cDims(1, 1);
    DataArrayPath dap = getFeatureIdsArrayPath();
    if(getSaveAsNewDataContainer())
    {
      dap.setDataContainerName(getNewDataContainerName().getDataContainerName());
    }
    m_FeatureIdsPtr = cellAttrMat->getAttributeArrayAs<Int32ArrayType>(dap.getDataArrayName()); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if(nullptr != m_FeatureIdsPtr.lock())                                                       /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
    {
      m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0);
    } /* Now assign the raw pointer to data from the DataArray<T> object */
    else
    {
      QString ss = QObject::tr("The FeatureIds array with name '%1' was not found in the destination DataContainer. The expected path was '%2'").arg(dap.getDataArrayName()).arg(dap.serialize("/"));
      setErrorCondition(-601, ss);
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
        setErrorCondition(-601, ss);
        return;
      }
    }
    cellFeatureAttrMat->removeInactiveObjects(activeObjects, m_FeatureIdsPtr.lock().get());
  }

  if(m_UpdateOrigin)
  {
    FloatVec3Type resolution = destCellDataContainer->getGeometryAs<ImageGeom>()->getSpacing();
    FloatVec3Type origin = destCellDataContainer->getGeometryAs<ImageGeom>()->getOrigin();

    origin[0] = m_XMin * resolution[0] + oldOrigin[0];
    origin[1] = m_YMin * resolution[1] + oldOrigin[1];
    origin[2] = m_ZMin * resolution[2] + oldOrigin[2];

    destCellDataContainer->getGeometryAs<ImageGeom>()->setOrigin(origin);
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IntVec3Type CropImageGeometry::getCurrentVolumeDataContainerDimensions()
{
  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getCellAttributeMatrixPath().getDataContainerName());

  IntVec3Type data;
  if(nullptr != m.get())
  {
    ImageGeom::Pointer image = m->getGeometryAs<ImageGeom>();
    if(image.get() != nullptr)
    {
      data[0] = image->getXPoints();
      data[1] = image->getYPoints();
      data[2] = image->getZPoints();
    }
    else
    {
      data[0] = 0;
      data[1] = 0;
      data[2] = 0;
    }
  }
  else
  {
    data[0] = 0;
    data[1] = 0;
    data[2] = 0;
  }
  return data;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FloatVec3Type CropImageGeometry::getCurrentVolumeDataContainerResolutions()
{
  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getCellAttributeMatrixPath().getDataContainerName());
  FloatVec3Type data;
  if(nullptr != m)
  {
    ImageGeom::Pointer image = m->getGeometryAs<ImageGeom>();
    if(image.get() != nullptr)
    {
      data = image->getSpacing();
    }
    else
    {
      data[0] = 0;
      data[1] = 0;
      data[2] = 0;
    }
  }
  else
  {
    data[0] = 0;
    data[1] = 0;
    data[2] = 0;
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
        ss << "X Range: " << m_OldOrigin[0] << " to " << (m_OldOrigin[0] + (m_OldDimensions[0] * m_OldResolution[0])) << " (Delta: " << (m_OldDimensions[0] * m_OldResolution[0]) << ") " << 0 << "-"
           << m_OldDimensions[0] - 1 << " Voxels\n";
        ss << "Y Range: " << m_OldOrigin[1] << " to " << (m_OldOrigin[1] + (m_OldDimensions[1] * m_OldResolution[1])) << " (Delta: " << (m_OldDimensions[1] * m_OldResolution[1]) << ") " << 0 << "-"
           << m_OldDimensions[1] - 1 << " Voxels\n";
        ss << "Z Range: " << m_OldOrigin[2] << " to " << (m_OldOrigin[2] + (m_OldDimensions[2] * m_OldResolution[2])) << " (Delta: " << (m_OldDimensions[2] * m_OldResolution[2]) << ") " << 0 << "-"
           << m_OldDimensions[2] - 1 << " Voxels\n";
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
        ss << "X Range: " << m_NewOrigin[0] << " to " << (m_NewOrigin[0] + (m_NewDimensions[0] * m_NewResolution[0])) << " (Delta: " << (m_NewDimensions[0] * m_NewResolution[0]) << ") " << 0 << "-"
           << m_NewDimensions[0] - 1 << " Voxels\n";
        ss << "Y Range: " << m_NewOrigin[1] << " to " << (m_NewOrigin[1] + (m_NewDimensions[1] * m_NewResolution[1])) << " (Delta: " << (m_NewDimensions[1] * m_NewResolution[1]) << ") " << 0 << "-"
           << m_NewDimensions[1] - 1 << " Voxels\n";
        ss << "Z Range: " << m_NewOrigin[2] << " to " << (m_NewOrigin[2] + (m_NewDimensions[2] * m_NewResolution[2])) << " (Delta: " << (m_NewDimensions[2] * m_NewResolution[2]) << ") " << 0 << "-"
           << m_NewDimensions[2] - 1 << " Voxels\n";
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
QString CropImageGeometry::getCompiledLibraryName() const
{
  return SamplingConstants::SamplingBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString CropImageGeometry::getBrandingString() const
{
  return "Sampling";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString CropImageGeometry::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << Sampling::Version::Major() << "." << Sampling::Version::Minor() << "." << Sampling::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString CropImageGeometry::getGroupName() const
{
  return SIMPL::FilterGroups::SamplingFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString CropImageGeometry::getHumanLabel() const
{
  return "Crop Geometry (Image)";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid CropImageGeometry::getUuid() const
{
  return QUuid("{baa4b7fe-31e5-5e63-a2cb-0bb9d844cfaf}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString CropImageGeometry::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::CropCutFilters;
}

// -----------------------------------------------------------------------------
CropImageGeometry::Pointer CropImageGeometry::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<CropImageGeometry> CropImageGeometry::New()
{
  struct make_shared_enabler : public CropImageGeometry
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString CropImageGeometry::getNameOfClass() const
{
  return QString("CropImageGeometry");
}

// -----------------------------------------------------------------------------
QString CropImageGeometry::ClassName()
{
  return QString("CropImageGeometry");
}

// -----------------------------------------------------------------------------
void CropImageGeometry::setNewDataContainerName(const DataArrayPath& value)
{
  m_NewDataContainerName = value;
}

// -----------------------------------------------------------------------------
DataArrayPath CropImageGeometry::getNewDataContainerName() const
{
  return m_NewDataContainerName;
}

// -----------------------------------------------------------------------------
void CropImageGeometry::setCellAttributeMatrixPath(const DataArrayPath& value)
{
  m_CellAttributeMatrixPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath CropImageGeometry::getCellAttributeMatrixPath() const
{
  return m_CellAttributeMatrixPath;
}

// -----------------------------------------------------------------------------
void CropImageGeometry::setCellFeatureAttributeMatrixPath(const DataArrayPath& value)
{
  m_CellFeatureAttributeMatrixPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath CropImageGeometry::getCellFeatureAttributeMatrixPath() const
{
  return m_CellFeatureAttributeMatrixPath;
}

// -----------------------------------------------------------------------------
void CropImageGeometry::setXMin(int value)
{
  m_XMin = value;
}

// -----------------------------------------------------------------------------
int CropImageGeometry::getXMin() const
{
  return m_XMin;
}

// -----------------------------------------------------------------------------
void CropImageGeometry::setYMin(int value)
{
  m_YMin = value;
}

// -----------------------------------------------------------------------------
int CropImageGeometry::getYMin() const
{
  return m_YMin;
}

// -----------------------------------------------------------------------------
void CropImageGeometry::setZMin(int value)
{
  m_ZMin = value;
}

// -----------------------------------------------------------------------------
int CropImageGeometry::getZMin() const
{
  return m_ZMin;
}

// -----------------------------------------------------------------------------
void CropImageGeometry::setXMax(int value)
{
  m_XMax = value;
}

// -----------------------------------------------------------------------------
int CropImageGeometry::getXMax() const
{
  return m_XMax;
}

// -----------------------------------------------------------------------------
void CropImageGeometry::setYMax(int value)
{
  m_YMax = value;
}

// -----------------------------------------------------------------------------
int CropImageGeometry::getYMax() const
{
  return m_YMax;
}

// -----------------------------------------------------------------------------
void CropImageGeometry::setZMax(int value)
{
  m_ZMax = value;
}

// -----------------------------------------------------------------------------
int CropImageGeometry::getZMax() const
{
  return m_ZMax;
}

// -----------------------------------------------------------------------------
void CropImageGeometry::setRenumberFeatures(bool value)
{
  m_RenumberFeatures = value;
}

// -----------------------------------------------------------------------------
bool CropImageGeometry::getRenumberFeatures() const
{
  return m_RenumberFeatures;
}

// -----------------------------------------------------------------------------
void CropImageGeometry::setSaveAsNewDataContainer(bool value)
{
  m_SaveAsNewDataContainer = value;
}

// -----------------------------------------------------------------------------
bool CropImageGeometry::getSaveAsNewDataContainer() const
{
  return m_SaveAsNewDataContainer;
}

// -----------------------------------------------------------------------------
void CropImageGeometry::setUpdateOrigin(bool value)
{
  m_UpdateOrigin = value;
}

// -----------------------------------------------------------------------------
bool CropImageGeometry::getUpdateOrigin() const
{
  return m_UpdateOrigin;
}

// -----------------------------------------------------------------------------
void CropImageGeometry::setFeatureIdsArrayPath(const DataArrayPath& value)
{
  m_FeatureIdsArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath CropImageGeometry::getFeatureIdsArrayPath() const
{
  return m_FeatureIdsArrayPath;
}
