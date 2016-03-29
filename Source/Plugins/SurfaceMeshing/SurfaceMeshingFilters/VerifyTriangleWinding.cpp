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

#include "VerifyTriangleWinding.h"

#include <QtCore/QString>
#include <iostream>
#include <queue>
#include <deque>
#include <limits>

#ifdef _MSC_VER
#include <hash_set>
// using namespace stdext;
#define STDEXT stdext
#else
#include <ext/hash_set>
// using namespace __gnu_cxx;
#define STDEXT __gnu_cxx
#endif


#include "SIMPLib/Math/MatrixMath.h"
#include "SIMPLib/Geometry/TriangleGeom.h"
#include "SurfaceMeshing/SurfaceMeshingFilters/util/Vector3.h"
#include "SurfaceMeshing/SurfaceMeshingFilters/util/Plane.h"
#include "SurfaceMeshing/SurfaceMeshingFilters/util/TriangleOps.h"
#include "SurfaceMeshing/SurfaceMeshingFilters/ReverseTriangleWinding.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
class Mesh
{
  public:

    SIMPL_SHARED_POINTERS(Mesh)
    SIMPL_STATIC_NEW_MACRO(Mesh)
    virtual ~Mesh() {}
    int getMaxLabel() {return -1;}
    int getMinLabel() {return -1;}
    void setMaxLabel(int32_t l) { m_MaxLabel = l; }
    void setMinLabel(int32_t l) { m_MinLabel = l; }
    void incrementMaxLabel() { m_MaxLabel++; }

  protected:
    Mesh() {}
  private:
    int32_t  m_MinLabel;
    int32_t  m_MaxLabel;

    Mesh(const Mesh&); // Copy Constructor Not Implemented
    void operator=(const Mesh&); // Operator '=' Not Implemented
};


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
class LabelVisitorInfo
{
  public:
    //   typedef Mesh<int32_t>::Pointer                MeshPtrType;
    typedef FaceArray::Face_t                   FaceType;
    typedef QVector<int32_t>      FaceListType;

    SIMPL_SHARED_POINTERS(LabelVisitorInfo)
    static Pointer New(int32_t label, size_t tIndex)
    {
      Pointer sharedPtr (new LabelVisitorInfo(label, tIndex));
      return sharedPtr;
    }

    virtual ~LabelVisitorInfo() {}

  private:

    int32_t m_Label;

  public:

    int32_t getLabel()
    {
      if (m_Relabeled) { return m_NewLabel; }
      return m_Label;
    }

    SIMPL_INSTANCE_PROPERTY(size_t, StartIndex)
    SIMPL_INSTANCE_PROPERTY(bool, Primed)
    SIMPL_INSTANCE_PROPERTY(int32_t, NewLabel)
    SIMPL_INSTANCE_PROPERTY(bool, Relabeled)

    QSet<int32_t>  m_Faces;
    QSet<int32_t>  m_OriginalFaceList;

    /**
    *
    * @param mesh
    * @param masterVisited
    * @return
    */
    Pointer relabelFaces(Mesh::Pointer mesh,
                         DataArray<int32_t>::Pointer masterFaceListPtr,
                         const QVector<bool>& masterVisited)
    {
      size_t triangleIndex = *(m_Faces.begin());
      int32_t newLabel = mesh->getMaxLabel() + 1;
      mesh->incrementMaxLabel();
      int32_t* masterFaceList = masterFaceListPtr->getPointer(0);
      LabelVisitorInfo::Pointer p = LabelVisitorInfo::New(m_Label, triangleIndex);

      p->setPrimed(false);
      p->setNewLabel(newLabel);
      p->setRelabeled(true);
      p->m_Faces = m_Faces; // This will make a copy of the current set of triangles
      p->m_OriginalFaceList = m_Faces; // Make a copy that does NOT get updated
      bool seedIsSet = false;
      for (QSet<int32_t>::iterator triIter = p->m_Faces.begin(); triIter != p->m_Faces.end(); ++triIter )
      {
        if (masterFaceList[ (*triIter) * 2] == m_Label) { masterFaceList[ (*triIter) * 2] = newLabel; }
        if (masterFaceList[ (*triIter) * 2 + 1] == m_Label) { masterFaceList[ (*triIter) * 2 + 1] = newLabel; }
        if (masterVisited[*triIter]) { p->setStartIndex(*triIter); seedIsSet = true; }
      }
      if (seedIsSet)
      {
        p->setPrimed(true);
      }

      return p;
    }

    /**
     *
     * @param mesh
     */
    void revertFaceLabels(DataArray<int32_t>::Pointer masterFaceListPtr)
    {
      int32_t* masterFaceList = masterFaceListPtr->getPointer(0);

      if (m_Relabeled == true)
      {
        //  qDebug() << "    Reverting Label " << m_NewLabel << " To " << m_Label << "\n";
        for (QSet<int32_t>::iterator triIter = m_OriginalFaceList.begin(); triIter != m_OriginalFaceList.end(); ++triIter )
        {
          if (masterFaceList[(*triIter) * 2] == m_NewLabel) { masterFaceList[(*triIter) * 2] = m_Label; }
          if (masterFaceList[(*triIter) * 2 + 1] == m_NewLabel) { masterFaceList[(*triIter) * 2 + 1] = m_Label; }
        }
        m_Relabeled = false;
        m_NewLabel = m_Label;
        m_Primed = false;
      }
    }



  protected:
    LabelVisitorInfo(int32_t label, size_t tIndex) :
      m_Label(label),
      m_StartIndex(tIndex),
      m_Primed(false),
      m_NewLabel(label),
      m_Relabeled(false)
    {
    }

  private:
    LabelVisitorInfo(const LabelVisitorInfo&); // Copy Constructor Not Implemented
    void operator=(const LabelVisitorInfo&);   // Operator '=' Not Implemented
};




// Include the MOC generated file for this class
#include "moc_VerifyTriangleWinding.cpp"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VerifyTriangleWinding::VerifyTriangleWinding() :
  SurfaceMeshFilter(),
  m_SurfaceDataContainerName(SIMPL::Defaults::DataContainerName),
  m_SurfaceMeshNodeFacesArrayName(SIMPL::VertexData::SurfaceMeshNodeFaces),
  m_SurfaceMeshFaceLabelsArrayPath(SIMPL::Defaults::DataContainerName, SIMPL::Defaults::FaceAttributeMatrixName, SIMPL::FaceData::SurfaceMeshFaceLabels),
  m_SurfaceMeshFaceLabelsArrayName(SIMPL::FaceData::SurfaceMeshFaceLabels),
  m_SurfaceMeshFaceLabels(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VerifyTriangleWinding::~VerifyTriangleWinding()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VerifyTriangleWinding::setupFilterParameters()
{
  FilterParameterVector parameters;

  parameters.push_back(SeparatorFilterParameter::New("Required Information", FilterParameter::Uncategorized));
  parameters.push_back(DataArraySelectionFilterParameter::New("SurfaceMeshFaceLabels", "SurfaceMeshFaceLabelsArrayPath", getSurfaceMeshFaceLabelsArrayPath(), FilterParameter::Uncategorized));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VerifyTriangleWinding::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setSurfaceMeshFaceLabelsArrayPath(reader->readDataArrayPath("SurfaceMeshFaceLabelsArrayPath", getSurfaceMeshFaceLabelsArrayPath() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VerifyTriangleWinding::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  SIMPL_FILTER_WRITE_PARAMETER(FilterVersion)
  SIMPL_FILTER_WRITE_PARAMETER(SurfaceMeshFaceLabelsArrayPath)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VerifyTriangleWinding::dataCheck()
{
  DataContainer::Pointer sm = getDataContainerArray()->getPrereqDataContainer<AbstractFilter>(this, getSurfaceMeshFaceLabelsArrayPath().getDataContainerName(), false);
  if(getErrorCondition() < 0) { return; }

  IGeometry::Pointer geom = sm->getGeometry();
  if(NULL == geom.get())
  {
    setErrorCondition(-385);
    QString ss = QObject::tr("DataContainer Geometry is missing.");
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  TriangleGeom::Pointer triangles = sm->getGeometryAs<TriangleGeom>();
  if(NULL == triangles.get())
  {
    setErrorCondition(-384);
    QString ss = QObject::tr("DataContainer Geometry is not compatible. The Geometry type is %1").arg(geom->getGeometryTypeAsString());
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  // We MUST have Nodes
  if (NULL == triangles->getVertices().get())
  {
    setErrorCondition(-386);
    notifyErrorMessage(getHumanLabel(), "DataContainer Geometry missing Vertices", getErrorCondition());
  }
  // We MUST have Triangles defined also.
  if (NULL == triangles->getTriangles().get())
  {
    setErrorCondition(-387);
    notifyErrorMessage(getHumanLabel(), "DataContainer Geometry missing Triangles", getErrorCondition());
  }
  else
  {
    QVector<size_t> dims(1, 2);
    m_SurfaceMeshFaceLabelsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getSurfaceMeshFaceLabelsArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if( NULL != m_SurfaceMeshFaceLabelsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    {m_SurfaceMeshFaceLabels = m_SurfaceMeshFaceLabelsPtr.lock()->getPointer(0);} /* Now assign the raw pointer to data from the DataArray<T> object */
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VerifyTriangleWinding::preflight()
{
  setInPreflight(true);
  emit preflightAboutToExecute();
  emit updateFilterParameters(this);
  dataCheck();
  emit preflightExecuted();
  setInPreflight(false);

  /* *** THIS FILTER NEEDS TO BE CHECKED *** */
  setErrorCondition(0xABABABAB);
  QString ss = QObject::tr("Filter is NOT updated for IGeometry Redesign. A Programmer needs to check this filter. Please report this to the DREAM3D developers.");
  notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  /* *** THIS FILTER NEEDS TO BE CHECKED *** */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VerifyTriangleWinding::execute()
{
  int err = 0;
  setErrorCondition(err);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  DataContainer::Pointer sm = getDataContainerArray()->getDataContainer(getSurfaceMeshFaceLabelsArrayPath().getDataContainerName());

  FaceArray::Pointer facesPtr = sm->getFaces();

  notifyStatusMessage(getHumanLabel(), "Generating Face List for each Node");
  // Make sure the Face Connectivity is created because the FindNRing algorithm needs this and will
  // assert if the data is NOT in the SurfaceMesh Data Container
  if (NULL == facesPtr->getFacesContainingVert()) { facesPtr->findFacesContainingVert(); }
  if (getCancel() == true) { return; }
  if (NULL == facesPtr->getFaceNeighbors()) { facesPtr->findFaceNeighbors(); }
  if (getCancel() == true) { return; }
  if(facesPtr->getUniqueEdges() == NULL) { facesPtr->generateUniqueEdgeIds(); }

  // Execute the actual verification step.
  notifyStatusMessage(getHumanLabel(), "Generating Connectivity Complete. Starting Analysis");
  verifyTriangleWinding();

  /* Let the GUI know we are done with this filter */
  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
// Groups the triangles according to which Feature they are a part of
// -----------------------------------------------------------------------------
void VerifyTriangleWinding::getLabelTriangelMap(LabelFaceMap_t& trianglesToLabelMap)
{
  DataContainer::Pointer sm = getDataContainerArray()->getDataContainer(getSurfaceMeshFaceLabelsArrayPath().getDataContainerName());
  FaceArray::Pointer masterFaceList = sm->getFaces();
  if(NULL == masterFaceList.get())
  {
    setErrorCondition(-556);
    notifyErrorMessage(getHumanLabel(), "The SurfaceMesh DataContainer Does NOT contain Faces", -556);
    return;
  }

  int ntri = masterFaceList->getNumberOfTuples();
  //FaceArray::Face_t* triangles = masterFaceList->getPointer(0);

  // Loop over all the triangles and group them according to which feature/region they are a part of
  for(int t = 0; t < ntri; ++t)
  {
    int32_t* label = m_SurfaceMeshFaceLabels + t * 2;
    trianglesToLabelMap[label[0]].insert(t);
    trianglesToLabelMap[label[1]].insert(t);
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int32_t VerifyTriangleWinding::getSeedTriangle(int32_t label, QSet<int32_t>& triangleIndices)
{
  DataContainer::Pointer sm = getDataContainerArray()->getDataContainer(getSurfaceMeshFaceLabelsArrayPath().getDataContainerName());

  VertexArray::Vert_t* verts = sm->getVertices()->getPointer(0);
  FaceArray::Face_t* triangles = sm->getFaces()->getPointer(0);

  float xMax = std::numeric_limits<float>::min();
  float avgX = 0.0f;
  int32_t seedFaceIdx = 0;

  for (QSet<int32_t>::iterator iter = triangleIndices.begin(); iter != triangleIndices.end(); ++iter)
  {
    int32_t i = *iter;
    avgX  = (verts[triangles[i].verts[0]].pos[0] + verts[triangles[i].verts[1]].pos[0] + verts[triangles[i].verts[2]].pos[0]) / 3.0;
    if (avgX > xMax)
    {
      xMax = avgX;
      seedFaceIdx = i;
    }
  }
  // Now we have the "right most" triangle based on x component of the centroid of the triangles for this label.
  // Lets now figure out if the normal points generally in the positive or negative X direction.

  int32_t* faceLabel = m_SurfaceMeshFaceLabels + seedFaceIdx * 2;
  VectorType normal;
  if (faceLabel[0] == label)
  {
    normal = TriangleOps::computeNormal(verts[triangles[seedFaceIdx].verts[0]], verts[triangles[seedFaceIdx].verts[1]], verts[triangles[seedFaceIdx].verts[2]]);
  }
  else
  {
    normal = TriangleOps::computeNormal(verts[triangles[seedFaceIdx].verts[2]], verts[triangles[seedFaceIdx].verts[1]], verts[triangles[seedFaceIdx].verts[0]]);
  }

  if (normal.x < 0.0f)
  {
    ReverseTriangleWinding::Pointer reverse = ReverseTriangleWinding::New();
    reverse->setDataContainerArray(getDataContainerArray());
    connect(reverse.get(), SIGNAL(filterGeneratedMessage(const PipelineMessage&)),
            this, SLOT(broadcastPipelineMessage(const PipelineMessage&)));
    reverse->setMessagePrefix(getMessagePrefix());
    reverse->execute();
    if (reverse->getErrorCondition() < 0)
    {
      setErrorCondition(reverse->getErrorCondition());
      notifyErrorMessage(getHumanLabel(), "Error Reversing the Face Winding", -801);
      return -1;
    }
    if (faceLabel[0] == label)
    {
      normal = TriangleOps::computeNormal(verts[triangles[seedFaceIdx].verts[0]], verts[triangles[seedFaceIdx].verts[1]], verts[triangles[seedFaceIdx].verts[2]]);
    }
    else
    {
      normal = TriangleOps::computeNormal(verts[triangles[seedFaceIdx].verts[2]], verts[triangles[seedFaceIdx].verts[1]], verts[triangles[seedFaceIdx].verts[0]]);
    }
    if (normal.x < 0.0f)
    {
      notifyErrorMessage(getHumanLabel(), "Error After attempted triangle winding reversal. Face normal is still oriented in wrong direction.", -802);
      seedFaceIdx = -1;
    }
  }

  return seedFaceIdx;
}

#define PRINT_FACE(index, triangles)\
  qDebug() << " " << triangles[index].verts[0] << " " << triangles[index].verts[1] << " " << triangles[index].verts[2] << "\n";\
  qDebug() << "Face ID: " << index << "\n";\
  qDebug() << "Face.labels[0] " << triangles[index].labels[0] << "\n";\
  qDebug() << "Face.labels[1] " << triangles[index].labels[1] << "\n";\

#define PRINT_VERT(index)\
  qDebug() << index << " " << verts[index].pos[0] << " " << verts[index].pos[1] << " " << verts[index].pos[2] << "\n";

#define PRINT_NORMAL(i, nodes, triangles)\
  { VectorType normal = TriangleOps::computeNormal(nodes[triangles[i].verts[0]], nodes[triangles[i].verts[1]], nodes[triangles[i].verts[2]]);\
    qDebug() << normal.x << " " << normal.y << " " << normal.z << "\n";}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
  int VerifyTriangleWinding::verifyTriangleWinding()
  {
    int err = 0;
    //  qDebug() << "--------------------------------------------------------------" << "\n";
    //  FaceListType& masterFaceList = *(mesh->triangles());  // Create a reference variable for better syntax
    //  NodeVector&       masterNodeList = *(mesh->nodes());

    DataContainer::Pointer sm = getDataContainerArray()->getDataContainer(getSurfaceMeshFaceLabelsArrayPath().getDataContainerName());
    // get the triangle definitions - use the pointer to the start of the Struct Array
    FaceArray::Pointer masterFaceList = sm->getFaces();
    if(NULL == masterFaceList.get())
    {
      setErrorCondition(-556);
      notifyErrorMessage(getHumanLabel(), "The SurfaceMesh DataContainer Does NOT contain Faces", -556);
      return getErrorCondition();
    }
    FaceArray::Face_t* triangles = masterFaceList->getPointer(0);

    int numFaces = masterFaceList->getNumberOfTuples();

    VertexArray::Pointer masterNodeListPtr = sm->getVertices();
    if(NULL == masterNodeListPtr.get())
    {
      setErrorCondition(-555);
      notifyErrorMessage(getHumanLabel(), "The SurfaceMesh DataContainer Does NOT contain Nodes", -555);
      return getErrorCondition();
    }

    Mesh::Pointer mesh = Mesh::New();
    int min = std::numeric_limits<int32_t>::max();
    int max = std::numeric_limits<int32_t>::min();
    // Set the min and Max labels in the Mesh class;
    for(int n = 0; n < numFaces; ++n)
    {
      int l = m_SurfaceMeshFaceLabels[n * 2];
      if (l < min) { min = l;}
      if (l > max) { max = l;}
      l = m_SurfaceMeshFaceLabels[n * 2 + 1];
      if (l < min) { min = l;}
      if (l > max) { max = l;}
    }
    mesh->setMaxLabel(max);
    mesh->setMinLabel(min);

    // Get a grouping of triangles by feature ID
    LabelFaceMap_t trianglesToLabelMap;
    getLabelTriangelMap(trianglesToLabelMap);
    //  for(LabelFaceMap_t::iterator iter = trianglesToLabelMap.begin(); iter != trianglesToLabelMap.end(); ++iter)
    //  {
    //    qDebug() << "Feature: " << iter.key() << "   Face Count: " << iter.value().size() << "\n";
    //  }


    int32_t currentLabel = 0;

    // Find the first Non Zero Feature Id (Label). This is going to be our starting feature.
    for(LabelFaceMap_t::iterator iter = trianglesToLabelMap.begin(); iter != trianglesToLabelMap.end(); ++iter)
    {
      if ( iter.key() > 0)
      {
        currentLabel = iter.key();
        break;
      }
    }

    std::deque<LabelVisitorInfo::Pointer> labelObjectsToVisit;

    // Keeps a list of all the triangles that have been visited.
    QVector<bool> masterVisited(masterFaceList->getNumberOfTuples(), false);

    STDEXT::hash_set<int32_t> labelsVisitedSet;
    STDEXT::hash_set<int32_t> labelsToVisitSet;
    int32_t triIndex = 0;

    bool firstLabel = false;

    // Get the first triangle in the list
    // FaceArray::Face_t& triangle = triangles[triIndex];


    // Now that we have the starting Feature, lets try and get a seed triangle that is oriented in the proper direction.
    /* Make sure the winding is correct on the first triangle of the first label that will be checked. */
    triIndex = getSeedTriangle(currentLabel, trianglesToLabelMap[currentLabel]);
    if (getErrorCondition() < 0 || triIndex < 0)
    {
      return -1;
    }

    LabelVisitorInfo::Pointer ldo = LabelVisitorInfo::New(currentLabel, triIndex);
    labelObjectsToVisit.push_back(ldo);
    LabelVisitorInfo::Pointer curLdo = LabelVisitorInfo::NullPointer();
    labelsToVisitSet.insert(currentLabel);

    float total = (float)(trianglesToLabelMap.size());
    float curPercent = 0.0;
    int progressIndex = 0;


    // Start looping on all the Face Labels (Feature Ids) values
    while (labelObjectsToVisit.empty() == false)
    {
      if (getCancel() == true) { return -1; }
      if ( (progressIndex / total * 100.0f) > (curPercent) )
      {
        QString ss = QObject::tr("%1% Complete").arg(static_cast<int>(progressIndex / total * 100.0f));
        notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);
        curPercent += 5.0f;
      }
      ++progressIndex;

      QMap<int32_t, int32_t> neighborlabels;

      curLdo = labelObjectsToVisit.front();
      labelObjectsToVisit.pop_front();
      currentLabel = curLdo->getLabel();
      labelsToVisitSet.erase(currentLabel);
      labelsVisitedSet.insert(currentLabel);
      if (curLdo->getPrimed() == false && curLdo->getRelabeled() == false)
      {
        curLdo->m_Faces = trianglesToLabelMap[currentLabel];
        curLdo->setPrimed(true);
      }
      else if (curLdo->getPrimed() == false && curLdo->getRelabeled() == true)
      {
        //    qDebug() << "    Reint32_ting could not find a starting index. Using first index in triangle set" << "\n";
        curLdo->setStartIndex( *(curLdo->m_Faces.begin()) );
        curLdo->setPrimed(true);
      }

      if (firstLabel == true)
      {
        triIndex = 0;
        firstLabel = false;
      }
      else
      {
        triIndex = curLdo->getStartIndex();
      }
      // Get the number of triangles remaining to be visited for the current label
      QSet<int32_t>::size_type nLabelTriStart = curLdo->m_Faces.size();


      STDEXT::hash_set<int> localVisited;
      std::deque<int> triangleDeque;
      //FaceArray::Face_t& triangle = triangles[triIndex];
      triangleDeque.push_back(triIndex);

      while (triangleDeque.empty() == false)
      {
        int32_t triangleIndex = triangleDeque.front();
        FaceArray::Face_t& triangle = triangles[triangleIndex];
        int32_t* faceLabel = m_SurfaceMeshFaceLabels + (triangleIndex * 2); // Here we are getting a new pointer offset from the start of the labels array

        //   qDebug() << " $ tIndex: " << triangleIndex << "\n";
        FaceArray::Pointer facesPtr = sm->getFaces();
        if(facesPtr == NULL)
        {
          break;
        }
        //FaceArray::Face_t* faces = facesPtr->getPointer(0);

        QVector<int32_t> adjTris = TriangleOps::findAdjacentTriangles(facesPtr, triangleIndex, m_SurfaceMeshFaceLabelsPtr.lock(), currentLabel);

        for (FaceList_t::iterator adjTri = adjTris.begin(); adjTri != adjTris.end(); ++adjTri)
        {
          // qDebug() << "  ^ AdjTri index: " << *adjTri << "\n";
          if (masterVisited[*adjTri] == false)
          {
            //    qDebug() << "   * Checking Winding: " << *adjTri << "\n";
            if (TriangleOps::verifyWinding(triangle, triangles[*adjTri], faceLabel, m_SurfaceMeshFaceLabels + (*adjTri * 2), currentLabel) == true)
            {
              //   qDebug() << "Face winding flipped for triangle id = " << *adjTri << " Feature Id: " << currentLabel << "\n";
            }
          }

          if (localVisited.find(*adjTri) == localVisited.end()
              && find(triangleDeque.begin(), triangleDeque.end(), *adjTri) == triangleDeque.end())
          {
            //qDebug() << "   # Adding to Deque: " << *adjTri << "\n";
            triangleDeque.push_back(*adjTri);
            localVisited.insert(*adjTri);
            masterVisited[*adjTri] = true;
          }
        }

        // Just add the neighbor label to a set so we end up with a list of unique
        // labels that are neighbors to the current label
        if (currentLabel != faceLabel[0])
        {
          neighborlabels.insert(faceLabel[0], triangleIndex);
        }
        if (currentLabel != faceLabel[1])
        {
          neighborlabels.insert(faceLabel[1], triangleIndex);
        }


        localVisited.insert(triangleIndex);
        masterVisited[triangleIndex] = true;
        curLdo->m_Faces.remove(triangleIndex);
        triangleDeque.pop_front();
      }  // End of loop to visit all triangles in the 'currentLabel'

      // Find the Next label to push onto the end of the labels List, but ONLY if it is NOT
      // currently on the list and NOT currently on the label visited list.
      for (QMap<int32_t, int32_t>::iterator neigh = neighborlabels.begin(); neigh != neighborlabels.end(); ++neigh )
      {
        int32_t triangleIndex = neigh.value();
        int32_t* triangleLabel = m_SurfaceMeshFaceLabels + triangleIndex * 2;

        if ( labelsToVisitSet.find(triangleLabel[0]) == labelsToVisitSet.end()
             && (labelsVisitedSet.find(triangleLabel[0]) == labelsVisitedSet.end() ) )
        {
          // Push the int32_t value into the "set"
          labelsToVisitSet.insert(triangleLabel[0]);
          LabelVisitorInfo::Pointer l = LabelVisitorInfo::New(triangleLabel[0], triangleIndex);
          labelObjectsToVisit.push_back(l);
          // qDebug() << "    Adding Label " << triangle.labels[0] << " to visit starting at tIndex " << triangleIndex << "\n";
        }
        if ( labelsToVisitSet.find(triangleLabel[1]) == labelsToVisitSet.end()
             && (labelsVisitedSet.find(triangleLabel[1]) == labelsVisitedSet.end() ) )
        {
          // Push the int32_t value into the "set"
          labelsToVisitSet.insert(triangleLabel[1]);
          LabelVisitorInfo::Pointer l = LabelVisitorInfo::New(triangleLabel[1], triangleIndex);
          labelObjectsToVisit.push_back(l);
          //  qDebug() << "    Adding Label " << triangle.labels[1] << " to visit starting at tIndex " << triangleIndex << "\n";
        }

      }

      QSet<int32_t>::size_type nLabelTriEnd = curLdo->m_Faces.size();
      if (nLabelTriStart == nLabelTriEnd )
      {
        // qDebug() << "    !--!++! No new triangles visited for int32_t " << currentLabel << "\n";
      }
      else if (nLabelTriEnd == 0)
      {
        //  qDebug() << "    No Faces remain to be visited for label " << "\n";
        // If this currentLabel was the result of a "relabeling" then revert back to the original
        // label for those triangles.
        curLdo->revertFaceLabels(m_SurfaceMeshFaceLabelsPtr.lock());
      }
      else
      {
        //qDebug() << "    Not all triangles visited for label"  << "\n";
        //qDebug() << "    " << nLabelTriEnd << " Faces Remaining to be visited" << "\n";
        // At this point relabel the remaining triangles
        //      if (currentLabel == 28) {
        //        qDebug() << "break" << "\n";
        //      }
        LabelVisitorInfo::Pointer p = curLdo->relabelFaces(mesh, m_SurfaceMeshFaceLabelsPtr.lock(), masterVisited);
        labelObjectsToVisit.push_back(p);
        labelsToVisitSet.insert(p->getLabel());
        // Revert the current labelVisitorObject to its original label
        curLdo->revertFaceLabels(m_SurfaceMeshFaceLabelsPtr.lock());
        //qDebug() << "    New label " << mesh->getMaxLabel() << " added to visit" << "\n";
      }

    } // End of Loop over each int32_t

    //qDebug() << "Total: "<< total << "\n";
    //qDebug() << "labelsToVisitSet.size() :" << labelsToVisitSet.size() << "\n";
    if (labelsToVisitSet.size() != 0)
    {
      //qDebug() << " labelsToVisitSet:" << "\n";
      for (STDEXT::hash_set<int32_t>::iterator iter = labelsToVisitSet.begin(); iter != labelsToVisitSet.end(); ++iter )
      {
        //qDebug() << "  int32_t: " << *iter << "\n";
      }
    }
    //qDebug() << "labelsVisited.size() :" << labelsVisitedSet.size() << "\n";


    QSet<int32_t> thelabels = TriangleOps::generateUniqueLabels(m_SurfaceMeshFaceLabelsPtr.lock());
    for (STDEXT::hash_set<int32_t>::iterator iter = labelsVisitedSet.begin(); iter != labelsVisitedSet.end(); ++iter )
    {
      thelabels.remove(*iter);
    }

    //qDebug() << "labels NOT visited:" << "\n";
    for (QSet<int32_t>::iterator iter = thelabels.begin(); iter != thelabels.end(); ++iter )
    {
      //qDebug() << "  int32_t: " << *iter << "\n";
    }

    // qDebug() << "--------------------------------------------------------------" << "\n";
    return err;
  }



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
  AbstractFilter::Pointer VerifyTriangleWinding::newFilterInstance(bool copyFilterParameters)
  {
    VerifyTriangleWinding::Pointer filter = VerifyTriangleWinding::New();
    if(true == copyFilterParameters)
    {
      copyFilterParameterInstanceVariables(filter.get());
    }
    return filter;
  }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
  const QString VerifyTriangleWinding::getCompiledLibraryName()
  { return SurfaceMeshingConstants::SurfaceMeshingBaseName; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
  const QString VerifyTriangleWinding::getGroupName()
  { return SIMPL::FilterGroups::SurfaceMeshingFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
  const QString VerifyTriangleWinding::getSubGroupName()
  { return SIMPL::FilterSubGroups::ConnectivityArrangementFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
  const QString VerifyTriangleWinding::getHumanLabel()
  { return "Verify Triangle Winding"; }

