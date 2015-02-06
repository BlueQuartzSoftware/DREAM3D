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

#include "CropSurfaceMesh.h"

#include <QtCore/QMap>


#include "DREAM3DLib/Common/Constants.h"

#include "DREAM3DLib/Utilities/DREAM3DRandom.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CropSurfaceMesh::CropSurfaceMesh() :
  AbstractFilter(),
  m_NewDataContainerName(DREAM3D::Defaults::NewVolumeDataContainerName),
  m_FaceAttributeMatrixPath(DREAM3D::Defaults::SurfaceDataContainerName, DREAM3D::Defaults::FaceAttributeMatrixName, ""),
  m_VertexAttributeMatrixPath(DREAM3D::Defaults::SurfaceDataContainerName, DREAM3D::Defaults::VertexAttributeMatrixName, ""),
  m_FaceFeatureAttributeMatrixPath(DREAM3D::Defaults::SurfaceDataContainerName, DREAM3D::Defaults::FaceFeatureAttributeMatrixName, ""),
  m_HasNodeData(false),
  m_HasFaceData(true),
  m_XMin(0),
  m_YMin(0),
  m_ZMin(0),
  m_XMax(0),
  m_YMax(0),
  m_ZMax(0),
  m_RenumberFeatures(true),
  m_SaveAsNewDataContainer(false),
  m_FeatureIdsArrayPath(DREAM3D::Defaults::SurfaceDataContainerName, DREAM3D::Defaults::FaceAttributeMatrixName, DREAM3D::CellData::FeatureIds),
  m_FeatureIdsArrayName(DREAM3D::CellData::FeatureIds),
  m_FeatureIds(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CropSurfaceMesh::~CropSurfaceMesh()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CropSurfaceMesh::setupFilterParameters()
{
  FilterParameterVector parameters;
  QStringList linkedProps;
  linkedProps << "VertexAttributeMatrixPath";
  parameters.push_back(LinkedBooleanFilterParameter::New("Has Node Data", "HasNodeData", getHasNodeData(), linkedProps, false));
  linkedProps.clear();
  linkedProps << "FaceAttributeMatrixPath";
  parameters.push_back(LinkedBooleanFilterParameter::New("Has Face Data", "HasFaceData", getHasFaceData(), linkedProps, false));
  linkedProps.clear();

  parameters.push_back(FilterParameter::New("Node Attribute Matrix", "VertexAttributeMatrixPath", FilterParameterWidgetType::AttributeMatrixSelectionWidget, getVertexAttributeMatrixPath(), false));
  parameters.push_back(FilterParameter::New("Face Attribute Matrix", "FaceAttributeMatrixPath", FilterParameterWidgetType::AttributeMatrixSelectionWidget, getFaceAttributeMatrixPath(), false));
  parameters.push_back(FilterParameter::New("X Min", "XMin", FilterParameterWidgetType::DoubleWidget, getXMin(), false, ""));
  parameters.push_back(FilterParameter::New("Y Min", "YMin", FilterParameterWidgetType::DoubleWidget, getYMin(), false, ""));
  parameters.push_back(FilterParameter::New("Z Min", "ZMin", FilterParameterWidgetType::DoubleWidget, getZMin(), false, ""));
  parameters.push_back(FilterParameter::New("X Max", "XMax", FilterParameterWidgetType::DoubleWidget, getXMax(), false, ""));
  parameters.push_back(FilterParameter::New("Y Max", "YMax", FilterParameterWidgetType::DoubleWidget, getYMax(), false, ""));
  parameters.push_back(FilterParameter::New("Z Max", "ZMax", FilterParameterWidgetType::DoubleWidget, getZMax(), false, ""));

  linkedProps << "CellFeatureAttributeMatrixPath" << "FeatureIdsArrayPath";
  parameters.push_back(LinkedBooleanFilterParameter::New("Renumber Features", "RenumberFeatures", getRenumberFeatures(), linkedProps, false));
  linkedProps.clear();
  parameters.push_back(FilterParameter::New("Required Information", "", FilterParameterWidgetType::SeparatorWidget, "", true));
  parameters.push_back(FilterParameter::New("Face Feature Attribute Matrix", "FaceFeatureAttributeMatrixPath", FilterParameterWidgetType::AttributeMatrixSelectionWidget, getFaceFeatureAttributeMatrixPath(), true));
  parameters.push_back(FilterParameter::New("FeatureIds", "FeatureIdsArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getFeatureIdsArrayPath(), true, ""));
  parameters.push_back(FilterParameter::New("Created Information", "", FilterParameterWidgetType::SeparatorWidget, "", true));
  linkedProps << "NewDataContainerName";
  parameters.push_back(LinkedBooleanFilterParameter::New("Save As New Data Container", "SaveAsNewDataContainer", getSaveAsNewDataContainer(), linkedProps, true));
  parameters.push_back(FilterParameter::New("New Data Container Name", "NewDataContainerName", FilterParameterWidgetType::StringWidget, getNewDataContainerName(), true));
  setFilterParameters(parameters);
}
// -----------------------------------------------------------------------------
void CropSurfaceMesh::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setNewDataContainerName( reader->readString("NewDataContainerName", getNewDataContainerName() ) );
  setVertexAttributeMatrixPath( reader->readDataArrayPath("VertexAttributeMatrixPath", getVertexAttributeMatrixPath() ) );
  setFaceAttributeMatrixPath( reader->readDataArrayPath("FaceAttributeMatrixPath", getFaceAttributeMatrixPath() ) );
  setFaceFeatureAttributeMatrixPath( reader->readDataArrayPath("FaceFeatureAttributeMatrixPath", getFaceFeatureAttributeMatrixPath() ) );
  setFeatureIdsArrayPath(reader->readDataArrayPath("FeatureIdsArrayPath", getFeatureIdsArrayPath() ) );
  setXMin( reader->readValue("XMin", getXMin()) );
  setYMin( reader->readValue("YMin", getYMin()) );
  setZMin( reader->readValue("ZMin", getZMin()) );
  setXMax( reader->readValue("XMax", getXMax()) );
  setYMax( reader->readValue("YMax", getYMax()) );
  setZMax( reader->readValue("ZMax", getZMax()) );
  setRenumberFeatures( reader->readValue("RenumberFeatures", getRenumberFeatures()) );
  setSaveAsNewDataContainer( reader->readValue("SaveAsNewDataContainer", getSaveAsNewDataContainer()) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int CropSurfaceMesh::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  DREAM3D_FILTER_WRITE_PARAMETER(FilterVersion)
  DREAM3D_FILTER_WRITE_PARAMETER(NewDataContainerName)
  DREAM3D_FILTER_WRITE_PARAMETER(VertexAttributeMatrixPath)
  DREAM3D_FILTER_WRITE_PARAMETER(FaceAttributeMatrixPath)
  DREAM3D_FILTER_WRITE_PARAMETER(FaceFeatureAttributeMatrixPath)
  DREAM3D_FILTER_WRITE_PARAMETER(FeatureIdsArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(XMin)
  DREAM3D_FILTER_WRITE_PARAMETER(YMin)
  DREAM3D_FILTER_WRITE_PARAMETER(ZMin)
  DREAM3D_FILTER_WRITE_PARAMETER(XMax)
  DREAM3D_FILTER_WRITE_PARAMETER(YMax)
  DREAM3D_FILTER_WRITE_PARAMETER(ZMax)
  DREAM3D_FILTER_WRITE_PARAMETER(RenumberFeatures)
  DREAM3D_FILTER_WRITE_PARAMETER(SaveAsNewDataContainer)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CropSurfaceMesh::dataCheck()
{
  if(getErrorCondition() < 0) { return; }

  SurfaceDataContainer* srcSurfDataContainer = getDataContainerArray()->getPrereqDataContainer<SurfaceDataContainer, AbstractFilter>(this, getFaceAttributeMatrixPath().getDataContainerName());
  if (NULL == srcSurfDataContainer)
  {
    return;
  }

  AttributeMatrix::Pointer nodeAttrMat = AttributeMatrix::NullPointer();
  AttributeMatrix::Pointer faceAttrMat = AttributeMatrix::NullPointer();

  if(m_HasNodeData == true)
  {
    nodeAttrMat = srcSurfDataContainer->getAttributeMatrix(getVertexAttributeMatrixPath().getAttributeMatrixName());
    if(NULL == nodeAttrMat) { return; }
  }
  if(m_HasFaceData == true)
  {
    faceAttrMat = srcSurfDataContainer->getAttributeMatrix(getFaceAttributeMatrixPath().getAttributeMatrixName());
    if(NULL == faceAttrMat) { return; }
  }
  SurfaceDataContainer* destSurfDataContainer = srcSurfDataContainer;
  if(NULL == destSurfDataContainer) { return; }

  if (m_SaveAsNewDataContainer == true)
  {
    destSurfDataContainer = getDataContainerArray()->createNonPrereqDataContainer<SurfaceDataContainer, AbstractFilter>(this, getNewDataContainerName());

    if(m_HasNodeData == true)
    {
      AttributeMatrix::Pointer nodeAttrMatCopy = nodeAttrMat->deepCopy();
      destSurfDataContainer->addAttributeMatrix(nodeAttrMatCopy->getName(), nodeAttrMatCopy);
      nodeAttrMat = srcSurfDataContainer->getAttributeMatrix(getVertexAttributeMatrixPath().getAttributeMatrixName());
      if(NULL == nodeAttrMat) { return; }
    }

    if(m_HasFaceData == true)
    {
      AttributeMatrix::Pointer faceAttrMatCopy = faceAttrMat->deepCopy();
      destSurfDataContainer->addAttributeMatrix(faceAttrMatCopy->getName(), faceAttrMatCopy);
      faceAttrMat = srcSurfDataContainer->getAttributeMatrix(getFaceAttributeMatrixPath().getAttributeMatrixName());
      if(NULL == faceAttrMat) { return; }
    }
  }

  if(getErrorCondition() < 0)
  {
    return;
  }

  if(m_RenumberFeatures == true)
  {
    QVector<size_t> dims(1, 1);
    m_FeatureIdsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getFeatureIdsArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if( NULL != m_FeatureIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    { m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

    AttributeMatrix::Pointer faceFeatureAttrMat = srcSurfDataContainer->getAttributeMatrix(getFaceFeatureAttributeMatrixPath().getAttributeMatrixName());
    if(getErrorCondition() < 0) { return; }
    QVector<bool> activeObjects(faceFeatureAttrMat->getNumTuples(), true);
    faceFeatureAttrMat->removeInactiveObjects(activeObjects, m_FeatureIdsPtr.lock());
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CropSurfaceMesh::preflight()
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
void CropSurfaceMesh::execute()
{
  int err = 0;
  setErrorCondition(err);

  /* Normally, filters call dataCheck during the execute to reuse code.  Unfortunately, this cannot happen for this filter, because calling dataCheck
      would destroy an Attribute Matrix that we need during the execute.  Do not uncomment the code, and be careful when reusing code from either of
      these functions.  Make sure you understand how this works before you reuse any code. */
  //dataCheck();
  //if(getErrorCondition() < 0) { return; }

  SurfaceDataContainer* srcSurfDataContainer = getDataContainerArray()->getPrereqDataContainer<SurfaceDataContainer, AbstractFilter>(this, getFaceAttributeMatrixPath().getDataContainerName());
  AttributeMatrix::Pointer nodeAttrMat = AttributeMatrix::NullPointer();
  AttributeMatrix::Pointer faceAttrMat = AttributeMatrix::NullPointer();

  if(m_HasNodeData == true)
  {
    nodeAttrMat = srcSurfDataContainer->getAttributeMatrix(getVertexAttributeMatrixPath().getAttributeMatrixName());
    if(NULL == nodeAttrMat) { return; }
  }
  if(m_HasFaceData == true)
  {
    faceAttrMat = srcSurfDataContainer->getAttributeMatrix(getFaceAttributeMatrixPath().getAttributeMatrixName());
    if(NULL == faceAttrMat) { return; }
  }
  SurfaceDataContainer* destSurfDataContainer = srcSurfDataContainer;
  if(NULL == destSurfDataContainer) { return; }

  if (m_SaveAsNewDataContainer == true)
  {
    destSurfDataContainer = getDataContainerArray()->createNonPrereqDataContainer<SurfaceDataContainer, AbstractFilter>(this, getNewDataContainerName());

    if(m_HasNodeData == true)
    {
      AttributeMatrix::Pointer nodeAttrMatCopy = nodeAttrMat->deepCopy();
      destSurfDataContainer->addAttributeMatrix(nodeAttrMatCopy->getName(), nodeAttrMatCopy);
      nodeAttrMat = srcSurfDataContainer->getAttributeMatrix(getVertexAttributeMatrixPath().getAttributeMatrixName());
      if(NULL == nodeAttrMat) { return; }
    }

    if(m_HasFaceData == true)
    {
      AttributeMatrix::Pointer faceAttrMatCopy = faceAttrMat->deepCopy();
      destSurfDataContainer->addAttributeMatrix(faceAttrMatCopy->getName(), faceAttrMatCopy);
      faceAttrMat = srcSurfDataContainer->getAttributeMatrix(getFaceAttributeMatrixPath().getAttributeMatrixName());
      if(NULL == faceAttrMat) { return; }
    }
  }

  if(getErrorCondition() < 0)
  {
    return;
  }

  // No matter where the AM is (same DC or new DC), we have the correct DC and AM pointers...now it's time to crop
  VertexArray::Pointer vertices = srcSurfDataContainer->getVertices();
  VertexArray::Vert_t* vertex = vertices->getPointer(0);
  size_t numVerts = vertices->getNumberOfTuples();
  FaceArray::Pointer faces = srcSurfDataContainer->getFaces();
  FaceArray::Face_t* face = faces->getPointer(0);
  size_t numFaces = faces->getNumberOfTuples();

  QList<QString> nodeArrayNames;
  if(m_HasNodeData == true) { nodeArrayNames = nodeAttrMat->getAttributeArrayNames(); }
  QList<QString> faceArrayNames;
  if(m_HasFaceData == true) { faceArrayNames = faceAttrMat->getAttributeArrayNames(); }

  //Determine which nodes are within the cropped volume and remove the nodes that do not
  bool badNode = false;
  size_t goodNodeCount = 0;
  Int64ArrayType::Pointer newNodeNumbers = Int64ArrayType::CreateArray(numVerts, "newNumbers");
  newNodeNumbers->initializeWithValue(-1);
  int64_t* newNNumbers = newNodeNumbers->getPointer(0);
  for (size_t i = 0; i < numVerts; i++)
  {
    badNode = false;
    if(vertex[i].pos[0] < m_XMin || vertex[i].pos[0] > m_XMax) { badNode = true; }
    if(vertex[i].pos[1] < m_YMin || vertex[i].pos[1] > m_YMax) { badNode = true; }
    if(vertex[i].pos[2] < m_ZMin || vertex[i].pos[2] > m_ZMax) { badNode = true; }
    if(badNode == false)
    {
      vertex[goodNodeCount].pos[0] = vertex[i].pos[0];
      vertex[goodNodeCount].pos[1] = vertex[i].pos[1];
      vertex[goodNodeCount].pos[2] = vertex[i].pos[2];
      newNNumbers[i] = goodNodeCount;
      for (QList<QString>::iterator iter = nodeArrayNames.begin(); iter != nodeArrayNames.end(); ++iter)
      {
        QString name = *iter;
        IDataArray::Pointer p = nodeAttrMat->getAttributeArray(*iter);
        p->copyTuple(i, goodNodeCount);
      }
      goodNodeCount++;
    }
  }
  vertices->resizeArray(goodNodeCount);
  if(m_HasNodeData == true)
  {
    QVector<size_t> tDims(1, goodNodeCount);
    nodeAttrMat->resizeAttributeArrays(tDims);
  }

  //Determine which faces are within the cropped volume and remove the faces that do not
  bool badFace = false;
  int32_t node1, node2, node3;
  size_t goodFaceCount = 0;
  Int64ArrayType::Pointer newFaceNumbers = Int64ArrayType::CreateArray(numFaces, "newNumbers");
  newFaceNumbers->initializeWithValue(-1);
  int64_t* newFNumbers = newFaceNumbers->getPointer(0);
  for (size_t i = 0; i < numFaces; i++)
  {
    badFace = false;
    node1 = face[i].verts[0];
    node2 = face[i].verts[1];
    node3 = face[i].verts[2];
    if(newNNumbers[node1] == -1 || newNNumbers[node2] == -1 || newNNumbers[node3] == -1) { badFace = true; }
    if(badFace == false)
    {
      face[goodFaceCount].verts[0] = newNNumbers[node1];
      face[goodFaceCount].verts[1] = newNNumbers[node2];
      face[goodFaceCount].verts[2] = newNNumbers[node3];
      newFNumbers[i] = goodFaceCount;
      for (QList<QString>::iterator iter = faceArrayNames.begin(); iter != faceArrayNames.end(); ++iter)
      {
        QString name = *iter;
        IDataArray::Pointer p = faceAttrMat->getAttributeArray(*iter);
        p->copyTuple(i, goodFaceCount);
      }
      goodFaceCount++;
    }
  }
  faces->resizeArray(goodFaceCount);
  if(m_HasFaceData == true)
  {
    QVector<size_t> tDims(1, goodFaceCount);
    faceAttrMat->resizeAttributeArrays(tDims);
  }

  // Feature Ids MUST already be renumbered.
  if (m_RenumberFeatures == true)
  {
    numFaces = faceAttrMat->getNumTuples();
    AttributeMatrix::Pointer faceFeatureAttrMat = srcSurfDataContainer->getAttributeMatrix(getFaceFeatureAttributeMatrixPath().getAttributeMatrixName());
    size_t totalFeatures = faceFeatureAttrMat->getNumTuples();
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
    for (size_t i = 0; i < numFaces; ++i)
    {
      int32_t currentFeatureId = m_FeatureIds[i];
      if (currentFeatureId < totalFeatures)
      {
        activeObjects[currentFeatureId] = true;
      }
      else
      {
        setErrorCondition(-601);
        QString ss = QObject::tr("The total number of features from %1 is %2, but a value of %3 was found in Data Array %4.").arg(faceFeatureAttrMat->getName()).arg(totalFeatures).arg(currentFeatureId).arg(getFeatureIdsArrayPath().serialize("/"));
        notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
        return;
      }
    }
    faceFeatureAttrMat->removeInactiveObjects(activeObjects, m_FeatureIdsPtr.lock());
  }

  notifyStatusMessage(getHumanLabel(), "Completed");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer CropSurfaceMesh::newFilterInstance(bool copyFilterParameters)
{
  CropSurfaceMesh::Pointer filter = CropSurfaceMesh::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CropSurfaceMesh::getCompiledLibraryName()
{ return Sampling::SamplingBaseName; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CropSurfaceMesh::getGroupName()
{ return DREAM3D::FilterGroups::SamplingFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CropSurfaceMesh::getHumanLabel()
{ return "Crop Surface Mesh"; }

