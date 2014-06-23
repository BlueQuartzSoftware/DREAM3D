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

#include "InsertAtoms.h"

#include <QtCore/QMap>

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
#include <tbb/parallel_for.h>
#include <tbb/blocked_range.h>
#include <tbb/partitioner.h>
#include <tbb/task_scheduler_init.h>
#endif

#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Math/GeometryMath.h"
#include "DREAM3DLib/Math/OrientationMath.h"
#include "DREAM3DLib/DataContainers/DynamicListArray.hpp"

#include "DREAM3DLib/Utilities/DREAM3DRandom.h"

class InsertAtomsImpl
{
    FaceArray::Pointer m_Faces;
    Int32DynamicListArray::Pointer m_FaceIds;
    VertexArray::Pointer m_FaceBBs;
    QuatF* m_AvgQuats;
    FloatVec3_t m_LatticeConstants;
    uint32_t m_Basis;
    QVector<VertexArray::Pointer> m_Points;
    QVector<BoolArrayType::Pointer> m_InFeature;

  public:
    InsertAtomsImpl(FaceArray::Pointer faces, Int32DynamicListArray::Pointer faceIds, VertexArray::Pointer faceBBs, QuatF* avgQuats, FloatVec3_t latticeConstants, uint32_t basis, QVector<VertexArray::Pointer> points, QVector<BoolArrayType::Pointer> inFeature) :
      m_Faces(faces),
      m_FaceIds(faceIds),
      m_FaceBBs(faceBBs),
      m_AvgQuats(avgQuats),
      m_LatticeConstants(latticeConstants),
      m_Basis(basis),
      m_Points(points),
      m_InFeature(inFeature)
    {}
    virtual ~InsertAtomsImpl() {}

    void checkPoints(size_t start, size_t end) const
    {
      float radius;
      VertexArray::Vert_t ll, ur;
      VertexArray::Vert_t ll_rot, ur_rot;
      VertexArray::Vert_t point;
      char code;
      float g[3][3];

      for(int iter = start; iter < end; iter++)
      {
        Int32DynamicListArray::ElementList& faceIds = m_FaceIds->getElementList(iter);

        OrientationMath::QuattoMat(m_AvgQuats[iter], g);
        //find bounding box for current feature
        GeometryMath::FindBoundingBoxOfFaces(m_Faces, faceIds, ll, ur);
        GeometryMath::FindBoundingBoxOfRotatedFaces(m_Faces, faceIds, g, ll_rot, ur_rot);
        GeometryMath::FindDistanceBetweenPoints(ll, ur, radius);

        generatePoints(iter, m_Points, m_InFeature, m_AvgQuats, m_LatticeConstants, m_Basis, ll_rot, ur_rot);

        //check points in vertex array to see if they are in the bounding box of the feature
        int numPoints = m_Points[iter]->getNumberOfTuples();
        VertexArray::Pointer vertArray = m_Points[iter];
        BoolArrayType::Pointer boolArray = m_InFeature[iter];
        for(int i = 0; i < numPoints; i++)
        {
          point = vertArray->getVert(i);
          if(boolArray->getValue(i) == false)
          {
            code = GeometryMath::PointInPolyhedron(m_Faces, faceIds, m_FaceBBs, point, ll, ur, radius);
            if(code == 'i' || code == 'V' || code == 'E' || code == 'F') { m_InFeature[start]->setValue(i, true); }
          }
        }
      }
    }

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
    void operator()(const tbb::blocked_range<size_t>& r) const
    {
      checkPoints(r.begin(), r.end());
    }
#endif

    void generatePoints(int iter, QVector<VertexArray::Pointer> points, QVector<BoolArrayType::Pointer> inFeature, QuatF* m_AvgQuats, FloatVec3_t latticeConstants, uint32_t basis, VertexArray::Vert_t ll, VertexArray::Vert_t ur) const
    {
      float g[3][3];
      float gT[3][3];

      OrientationMath::QuattoMat(m_AvgQuats[iter], g);
      MatrixMath::Transpose3x3(g, gT);

      float minx = ll.pos[0];
      float miny = ll.pos[1];
      float minz = ll.pos[2];
      float maxx = ur.pos[0];
      float maxy = ur.pos[1];
      float maxz = ur.pos[2];
      float deltaX = maxx - minx;
      float deltaY = maxy - miny;
      float deltaZ = maxz - minz;
      size_t atomMult = 1;
      if(basis == 0) { atomMult = 1; }
      if(basis == 1) { atomMult = 2; }
      if(basis == 2) { atomMult = 4; }
      size_t xPoints = atomMult * (size_t(deltaX / latticeConstants.x) + 1);
      size_t yPoints = atomMult * (size_t(deltaY / latticeConstants.y) + 1);
      size_t zPoints = atomMult * (size_t(deltaZ / latticeConstants.z) + 1);
      size_t nPoints = xPoints * yPoints * zPoints;

      points[iter]->resizeArray(nPoints);
      inFeature[iter]->resize(nPoints);

      int count = 0;
      float coords[3];
      float coordsT[3];
      for(int k = 0; k < zPoints; k++)
      {
        for(int j = 0; j < yPoints; j++)
        {
          for(int i = 0; i < xPoints; i++)
          {
            coords[0] = float(i) * latticeConstants.x + minx;
            coords[1] = float(j) * latticeConstants.y + miny;
            coords[2] = float(k) * latticeConstants.z + minz;
            MatrixMath::Multiply3x3with3x1(gT, coords, coordsT);
            points[iter]->setCoords(count, coordsT);
            count++;
            if(basis == 1)
            {
              coords[0] = coords[0] + (0.5 * latticeConstants.x);
              coords[1] = coords[1] + (0.5 * latticeConstants.y);
              coords[2] = coords[2] + (0.5 * latticeConstants.z);
              MatrixMath::Multiply3x3with3x1(gT, coords, coordsT);
              points[iter]->setCoords(count, coordsT);
              count++;
            }
            if(basis == 2)
            {
              //makes the (0.5,0.5,0) atom
              coords[0] = coords[0] + (0.5 * latticeConstants.x);
              coords[1] = coords[1] + (0.5 * latticeConstants.y);
              coords[2] = coords[2];
              MatrixMath::Multiply3x3with3x1(gT, coords, coordsT);
              points[iter]->setCoords(count, coordsT);
              count++;
              //makes the (0.5,0,0.5) atom
              coords[0] = coords[0];
              coords[1] = coords[1] - (0.5 * latticeConstants.y);
              coords[2] = coords[2] + (0.5 * latticeConstants.z);
              MatrixMath::Multiply3x3with3x1(gT, coords, coordsT);
              points[iter]->setCoords(count, coordsT);
              count++;
              //makes the (0,0.5,0.5) atom
              coords[0] = coords[0] - (0.5 * latticeConstants.x);
              coords[1] = coords[1] + (0.5 * latticeConstants.y);
              coords[2] = coords[2];
              MatrixMath::Multiply3x3with3x1(gT, coords, coordsT);
              points[iter]->setCoords(count, coordsT);
              count++;
            }
          }
        }
      }
    }
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
InsertAtoms::InsertAtoms() :
  AbstractFilter(),
  m_VertexDataContainerName(DREAM3D::Defaults::VertexDataContainerName),
  m_VertexAttributeMatrixName(DREAM3D::Defaults::VertexAttributeMatrixName),
  m_Basis(0),
  m_SurfaceMeshFaceLabelsArrayPath(DREAM3D::Defaults::SurfaceDataContainerName, DREAM3D::Defaults::FaceAttributeMatrixName, DREAM3D::FaceData::SurfaceMeshFaceLabels),
  m_AvgQuatsArrayPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellFeatureAttributeMatrixName, DREAM3D::FeatureData::AvgQuats),
  m_AtomFeatureLabelsArrayName(DREAM3D::VertexData::AtomFeatureLabels),
  m_SurfaceMeshFaceLabelsArrayName(DREAM3D::FaceData::SurfaceMeshFaceLabels),
  m_SurfaceMeshFaceLabels(NULL),
  m_AvgQuatsArrayName(DREAM3D::FeatureData::AvgQuats),
  m_AvgQuats(NULL),
  m_AtomFeatureLabels(NULL)
{
  m_LatticeConstants.x = 1.0f;
  m_LatticeConstants.y = 1.0f;
  m_LatticeConstants.z = 1.0f;

  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
InsertAtoms::~InsertAtoms()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InsertAtoms::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(FilterParameter::New("LatticeConstants", "LatticeConstants", FilterParameterWidgetType::FloatVec3Widget, getLatticeConstants(), false, "Angstroms"));
  {
    ChoiceFilterParameter::Pointer parameter = ChoiceFilterParameter::New();
    parameter->setHumanLabel("Crystal Basis");
    parameter->setPropertyName("Basis");
    parameter->setWidgetType(FilterParameterWidgetType::ChoiceWidget);
    //parameter->setValueType("int");
    QVector<QString> choices;
    choices.push_back("Simple Cubic");
    choices.push_back("Body Centered Cubic");
    choices.push_back("Face Centered Cubic");
    parameter->setChoices(choices);
    parameters.push_back(parameter);
  }
  parameters.push_back(FilterParameter::New("Required Information", "", FilterParameterWidgetType::SeparatorWidget, "", true));
  parameters.push_back(FilterParameter::New("SurfaceMeshFaceLabels", "SurfaceMeshFaceLabelsArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getSurfaceMeshFaceLabelsArrayPath(), true, ""));
  parameters.push_back(FilterParameter::New("AvgQuats", "AvgQuatsArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getAvgQuatsArrayPath(), true, ""));
  parameters.push_back(FilterParameter::New("Created Information", "", FilterParameterWidgetType::SeparatorWidget, "", true));
  parameters.push_back(FilterParameter::New("Vertex Data Container Name", "VertexDataContainerName", FilterParameterWidgetType::StringWidget, getVertexDataContainerName(), true, ""));
  parameters.push_back(FilterParameter::New("Vertex Attribute Matrix Name", "VertexAttributeMatrixName", FilterParameterWidgetType::StringWidget, getVertexAttributeMatrixName(), true, ""));
  parameters.push_back(FilterParameter::New("AtomFeatureLabels", "AtomFeatureLabelsArrayName", FilterParameterWidgetType::StringWidget, getAtomFeatureLabelsArrayName(), true, ""));
  setFilterParameters(parameters);
}


// -----------------------------------------------------------------------------
void InsertAtoms::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setVertexDataContainerName(reader->readString("VertexDataContainerName", getVertexDataContainerName() ) );
  setVertexAttributeMatrixName(reader->readString("VertexAttributeMatrixName", getVertexAttributeMatrixName() ) );
  setAtomFeatureLabelsArrayName(reader->readString("AtomFeatureLabelsArrayName", getAtomFeatureLabelsArrayName() ) );
  setAvgQuatsArrayPath(reader->readDataArrayPath("AvgQuatsArrayPath", getAvgQuatsArrayPath() ) );
  setSurfaceMeshFaceLabelsArrayPath(reader->readDataArrayPath("SurfaceMeshFaceLabelsArrayPath", getSurfaceMeshFaceLabelsArrayPath() ) );
  setLatticeConstants( reader->readFloatVec3("LatticeConstants", getLatticeConstants() ) );
  setBasis( reader->readValue("Basis", getBasis() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int InsertAtoms::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  DREAM3D_FILTER_WRITE_PARAMETER(VertexDataContainerName)
  DREAM3D_FILTER_WRITE_PARAMETER(VertexAttributeMatrixName)
  DREAM3D_FILTER_WRITE_PARAMETER(AtomFeatureLabelsArrayName)
  DREAM3D_FILTER_WRITE_PARAMETER(AvgQuatsArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(SurfaceMeshFaceLabelsArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(LatticeConstants)
  DREAM3D_FILTER_WRITE_PARAMETER(Basis)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InsertAtoms::updateVertexInstancePointers()
{
  setErrorCondition(0);

  if( NULL != m_AtomFeatureLabelsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_AtomFeatureLabels = m_AtomFeatureLabelsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InsertAtoms::dataCheck()
{
  DataArrayPath tempPath;

  SurfaceDataContainer* sm = getDataContainerArray()->getPrereqDataContainer<SurfaceDataContainer, AbstractFilter>(this, m_SurfaceMeshFaceLabelsArrayPath.getDataContainerName());
  if(getErrorCondition() < 0) { return; }
  VertexDataContainer* v = getDataContainerArray()->createNonPrereqDataContainer<VertexDataContainer, AbstractFilter>(this, getVertexDataContainerName());
  if(getErrorCondition() < 0) { return; }

  QVector<size_t> tDims(1, 0);
  AttributeMatrix::Pointer vertexAttrMat = v->createNonPrereqAttributeMatrix<AbstractFilter>(this, getVertexAttributeMatrixName(), tDims, DREAM3D::AttributeMatrixType::Vertex);
  if(getErrorCondition() < 0) { return; }

  // We MUST have Nodes
  if(sm->getVertices().get() == NULL)
  {
    setErrorCondition(-384);
    notifyErrorMessage(getHumanLabel(), "SurfaceMesh DataContainer missing Nodes", getErrorCondition());
  }

  // We MUST have Triangles defined also.
  if(sm->getFaces().get() == NULL)
  {
    setErrorCondition(-385);
    notifyErrorMessage(getHumanLabel(), "SurfaceMesh DataContainer missing Triangles", getErrorCondition());
  }
  else
  {
    QVector<size_t> dims(1, 2);
    m_SurfaceMeshFaceLabelsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getSurfaceMeshFaceLabelsArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if( NULL != m_SurfaceMeshFaceLabelsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    { m_SurfaceMeshFaceLabels = m_SurfaceMeshFaceLabelsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  }
  QVector<size_t> dims(1, 4);
  // Feature Data
  m_AvgQuatsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getAvgQuatsArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_AvgQuatsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_AvgQuats = m_AvgQuatsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  dims[0] = 1;
  tempPath.update(getVertexDataContainerName(), getVertexAttributeMatrixName(), getAtomFeatureLabelsArrayName() );
  m_AtomFeatureLabelsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter, int32_t>(this, tempPath, -301, dims); /* Assigns the shared_ptr<>(this, tempPath, -301, dims);  Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_AtomFeatureLabelsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_AtomFeatureLabels = m_AtomFeatureLabelsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InsertAtoms::preflight()
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
void InsertAtoms::execute()
{
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  m_LatticeConstants.x = m_LatticeConstants.x / 10000.0;
  m_LatticeConstants.y = m_LatticeConstants.y / 10000.0;
  m_LatticeConstants.z = m_LatticeConstants.z / 10000.0;

  SurfaceDataContainer* sm = getDataContainerArray()->getDataContainerAs<SurfaceDataContainer>(getSurfaceMeshFaceLabelsArrayPath().getDataContainerName());
  DREAM3D_RANDOMNG_NEW()

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
  tbb::task_scheduler_init init;
  bool doParallel = true;
#endif

  setErrorCondition(0);

  //pull down faces
  FaceArray::Pointer faces = sm->getFaces();
  int numFaces = m_SurfaceMeshFaceLabelsPtr.lock()->getNumberOfTuples();

  //create array to hold bounding vertices for each face
  VertexArray::Vert_t ll, ur;
  //VertexArray::Vert_t point;
  VertexArray::Pointer faceBBs = VertexArray::CreateArray(2 * numFaces, "faceBBs");

  //walk through faces to see how many features there are
  int g1, g2;
  int maxFeatureId = 0;
  for(int i = 0; i < numFaces; i++)
  {
    g1 = m_SurfaceMeshFaceLabels[2 * i];
    g2 = m_SurfaceMeshFaceLabels[2 * i + 1];
    if(g1 > maxFeatureId) { maxFeatureId = g1; }
    if(g2 > maxFeatureId) { maxFeatureId = g2; }
  }
  //add one to account for feature 0
  int numFeatures = maxFeatureId + 1;

  //create a dynamic list array to hold face lists
  Int32DynamicListArray::Pointer faceLists = Int32DynamicListArray::New();
  QVector<int32_t> linkCount(numFeatures, 0);
  int32_t* linkLoc;

  // fill out lists with number of references to cells
  typedef boost::shared_array<int32_t> SharedInt32Array_t;
  SharedInt32Array_t linkLocPtr(new int32_t[numFaces]);
  linkLoc = linkLocPtr.get();

  ::memset(linkLoc, 0, numFaces * sizeof(int32_t));

  // traverse data to determine number of faces belonging to each feature
  for (int i = 0; i < numFaces; i++)
  {
    g1 = m_SurfaceMeshFaceLabels[2 * i];
    g2 = m_SurfaceMeshFaceLabels[2 * i + 1];
    if(g1 > 0) { linkCount[g1]++; }
    if(g2 > 0) { linkCount[g2]++; }
  }

  // now allocate storage for the faces
  faceLists->allocateLists(linkCount);

  // traverse data again to get the faces belonging to each feature
  for (int i = 0; i < numFaces; i++)
  {
    g1 = m_SurfaceMeshFaceLabels[2 * i];
    g2 = m_SurfaceMeshFaceLabels[2 * i + 1];
    if(g1 > 0) { faceLists->insertCellReference(g1, (linkLoc[g1])++, i); }
    if(g2 > 0) { faceLists->insertCellReference(g2, (linkLoc[g2])++, i); }
    //find bounding box for each face
    GeometryMath::FindBoundingBoxOfFace(faces, i, ll, ur);
    faceBBs->setCoords(2 * i, ll.pos);
    faceBBs->setCoords(2 * i + 1, ur.pos);
  }

  //generate the list of sampling points fom subclass
  QVector<VertexArray::Pointer> points(numFeatures);
  QVector<BoolArrayType::Pointer> inFeature(numFeatures);
  for(int i = 0; i < numFeatures; i++)
  {
    points[i] = VertexArray::CreateArray(0, "points");
    inFeature[i] = BoolArrayType::CreateArray(0, "inside");
  }

  QuatF* avgQuats = reinterpret_cast<QuatF*>(m_AvgQuats);

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
  if (doParallel == true)
  {
    tbb::parallel_for(tbb::blocked_range<size_t>(0, numFeatures),
                      InsertAtomsImpl(faces, faceLists, faceBBs, avgQuats, m_LatticeConstants, m_Basis, points, inFeature), tbb::auto_partitioner());

  }
  else
#endif
  {
    InsertAtomsImpl serial(faces, faceLists, faceBBs, avgQuats, m_LatticeConstants, m_Basis, points, inFeature);
    serial.checkPoints(0, numFeatures);
  }


  notifyStatusMessage(getHumanLabel(), "Assigning Points");
  assign_points(points, inFeature);

  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InsertAtoms::assign_points(QVector<VertexArray::Pointer> points, QVector<BoolArrayType::Pointer> inFeature)
{
  size_t count = 0;
  size_t numFeatures = points.size();
  for(int i = 0; i < numFeatures; i++)
  {
    size_t numPoints = points[i]->getNumberOfTuples();
    bool* inside = inFeature[i]->getPointer(0);
    for(int j = 0; j < numPoints; j++)
    {
      if(inside[j] == true) { count++; }
    }
  }

  notifyStatusMessage(getHumanLabel(), "Getting VDC");
  VertexDataContainer* v = getDataContainerArray()->getDataContainerAs<VertexDataContainer>(getVertexDataContainerName());

  notifyStatusMessage(getHumanLabel(), "Creating Verts");
  VertexArray::Pointer vertices = VertexArray::CreateArray(count, DREAM3D::VertexData::SurfaceMeshNodes);

  notifyStatusMessage(getHumanLabel(), "Getting VAM");
  AttributeMatrix::Pointer vertexAttrMat = v->getAttributeMatrix(getVertexAttributeMatrixName());
  QVector<size_t> tDims(1, count);
  notifyStatusMessage(getHumanLabel(), "Resizing VAM");
  vertexAttrMat->resizeAttributeArrays(tDims);
  notifyStatusMessage(getHumanLabel(), "updating Pointers");
  updateVertexInstancePointers();

  notifyStatusMessage(getHumanLabel(), "Making Verts");
  count = 0;
  float coords[3];
  for(int i = 0; i < numFeatures; i++)
  {
    size_t numPoints = points[i]->getNumberOfTuples();
    bool* inside = inFeature[i]->getPointer(0);
    for(int j = 0; j < numPoints; j++)
    {
      if(inside[j] == true)
      {
        coords[0] = points[i]->getVert(j).pos[0];
        coords[1] = points[i]->getVert(j).pos[1];
        coords[2] = points[i]->getVert(j).pos[2];
        vertices->setCoords(count, coords);
        m_AtomFeatureLabels[count] = i;
        count++;
      }
    }
  }
  v->setVertices(vertices);

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer InsertAtoms::newFilterInstance(bool copyFilterParameters)
{
  InsertAtoms::Pointer filter = InsertAtoms::New();
  if(true == copyFilterParameters)
  {
    filter->setFilterParameters(getFilterParameters());
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString InsertAtoms::getCompiledLibraryName()
{ return SyntheticBuilding::SyntheticBuildingBaseName; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString InsertAtoms::getGroupName()
{ return DREAM3D::FilterGroups::SyntheticBuildingFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString InsertAtoms::getHumanLabel()
{ return "Insert Atoms"; }

