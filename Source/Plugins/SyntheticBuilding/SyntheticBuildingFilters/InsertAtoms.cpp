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


#include "InsertAtoms.h"

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
#include <tbb/parallel_for.h>
#include <tbb/blocked_range.h>
#include <tbb/partitioner.h>
#include <tbb/task_scheduler_init.h>
#endif

#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/FilterParameters/AbstractFilterParametersReader.h"
#include "DREAM3DLib/FilterParameters/AbstractFilterParametersWriter.h"

#include "DREAM3DLib/FilterParameters/FloatVec3FilterParameter.h"
#include "DREAM3DLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "DREAM3DLib/FilterParameters/StringFilterParameter.h"
#include "DREAM3DLib/FilterParameters/ChoiceFilterParameter.h"
#include "DREAM3DLib/FilterParameters/SeparatorFilterParameter.h"
#include "DREAM3DLib/Math/GeometryMath.h"
#include "OrientationLib/OrientationMath/OrientationMath.h"
#include "OrientationLib/OrientationMath/OrientationArray.hpp"
#include "OrientationLib/OrientationMath/OrientationTransforms.hpp"
#include "DREAM3DLib/DataArrays/DynamicListArray.hpp"

#include "DREAM3DLib/Utilities/DREAM3DRandom.h"

#include "SyntheticBuilding/SyntheticBuildingConstants.h"

/**
 * @brief The InsertAtomsImpl class implements a threaded algorithm that inserts vertex points ('atoms') onto surface meshed Features
 */
class InsertAtomsImpl
{
    TriangleGeom::Pointer m_Faces;
    Int32Int32DynamicListArray::Pointer m_FaceIds;
    VertexGeom::Pointer m_FaceBBs;
    QuatF* m_AvgQuats;
    FloatVec3_t m_LatticeConstants;
    uint32_t m_Basis;
    QVector<VertexGeom::Pointer> m_Points;
    QVector<BoolArrayType::Pointer> m_InFeature;

  public:
    InsertAtomsImpl(TriangleGeom::Pointer faces, Int32Int32DynamicListArray::Pointer faceIds, VertexGeom::Pointer faceBBs, QuatF* avgQuats, FloatVec3_t latticeConstants, uint32_t basis, QVector<VertexGeom::Pointer> points, QVector<BoolArrayType::Pointer> inFeature) :
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
      float radius = 0.0f;
      FloatArrayType::Pointer llPtr = FloatArrayType::CreateArray(3, "_INTERNAL_USE_ONLY_Lower_Left");
      FloatArrayType::Pointer urPtr = FloatArrayType::CreateArray(3, "_INTERNAL_USE_ONLY_Upper_Right");
      FloatArrayType::Pointer ll_rotPtr = FloatArrayType::CreateArray(3, "_INTERNAL_USE_ONLY_Lower_Left_Rotated");
      FloatArrayType::Pointer ur_rotPtr = FloatArrayType::CreateArray(3, "_INTERNAL_USE_ONLY_Upper_Right_Rotated");
      float* ll = llPtr->getPointer(0);
      float* ur = urPtr->getPointer(0);
      float* ll_rot = ll_rotPtr->getPointer(0);
      float* ur_rot = ur_rotPtr->getPointer(0);
      float* point = NULL;
      char code = ' ';
      float g[3][3] = { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } };

      for (size_t iter = start; iter < end; iter++)
      {
        Int32Int32DynamicListArray::ElementList& faceIds = m_FaceIds->getElementList(iter);

        FOrientArrayType om(9, 0.0);
        FOrientTransformsType::qu2om(FOrientArrayType(m_AvgQuats[iter]), om);
        om.toGMatrix(g);

        // find bounding box for current feature
        GeometryMath::FindBoundingBoxOfFaces(m_Faces, faceIds, ll, ur);
        GeometryMath::FindBoundingBoxOfRotatedFaces(m_Faces, faceIds, g, ll_rot, ur_rot);
        GeometryMath::FindDistanceBetweenPoints(ll, ur, radius);

        generatePoints(iter, m_Points, m_InFeature, m_AvgQuats, m_LatticeConstants, m_Basis, ll_rot, ur_rot);

        // check points in vertex array to see if they are in the bounding box of the feature
        int64_t numPoints = m_Points[iter]->getNumberOfVertices();
        VertexGeom::Pointer vertArray = m_Points[iter];
        BoolArrayType::Pointer boolArray = m_InFeature[iter];
        for (int64_t i = 0; i < numPoints; i++)
        {
          point = vertArray->getVertexPointer(i);
          if (boolArray->getValue(i) == false)
          {
            code = GeometryMath::PointInPolyhedron(m_Faces, faceIds, m_FaceBBs, point, ll, ur, radius);
            if (code == 'i' || code == 'V' || code == 'E' || code == 'F') { m_InFeature[start]->setValue(i, true); }
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

    void generatePoints(size_t iter, QVector<VertexGeom::Pointer> points, QVector<BoolArrayType::Pointer> inFeature, QuatF* m_AvgQuats, FloatVec3_t latticeConstants, uint32_t basis, float* ll, float* ur) const
    {
      float g[3][3] = { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } };
      float gT[3][3] = { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } };

      FOrientArrayType om(9, 0.0);
      FOrientTransformsType::qu2om(FOrientArrayType(m_AvgQuats[iter]), om);
      om.toGMatrix(g);
      MatrixMath::Transpose3x3(g, gT);

      float minx = ll[0];
      float miny = ll[1];
      float minz = ll[2];
      float maxx = ur[0];
      float maxy = ur[1];
      float maxz = ur[2];
      float deltaX = maxx - minx;
      float deltaY = maxy - miny;
      float deltaZ = maxz - minz;
      int64_t atomMult = 1;
      if (basis == 0) { atomMult = 1; }
      if (basis == 1) { atomMult = 2; }
      if (basis == 2) { atomMult = 4; }
      int64_t xPoints = (int64_t(deltaX / latticeConstants.x) + 1);
      int64_t yPoints = (int64_t(deltaY / latticeConstants.y) + 1);
      int64_t zPoints = (int64_t(deltaZ / latticeConstants.z) + 1);
      int64_t nPoints = atomMult * xPoints * yPoints * zPoints;

      points[iter]->resizeVertexList(nPoints);
      inFeature[iter]->resize(nPoints);

      int64_t count = 0;
      float coords[3] = { 0.0f, 0.0f, 0.0f };
      float coordsT[3] = { 0.0f, 0.0f, 0.0f };
      for (int64_t k = 0; k < zPoints; k++)
      {
        for (int64_t j = 0; j < yPoints; j++)
        {
          for (int64_t i = 0; i < xPoints; i++)
          {
            coords[0] = float(i) * latticeConstants.x + minx;
            coords[1] = float(j) * latticeConstants.y + miny;
            coords[2] = float(k) * latticeConstants.z + minz;
            MatrixMath::Multiply3x3with3x1(gT, coords, coordsT);
            points[iter]->setCoords(count, coordsT);
            count++;
            if (basis == 1)
            {
              coords[0] = coords[0] + (0.5 * latticeConstants.x);
              coords[1] = coords[1] + (0.5 * latticeConstants.y);
              coords[2] = coords[2] + (0.5 * latticeConstants.z);
              MatrixMath::Multiply3x3with3x1(gT, coords, coordsT);
              points[iter]->setCoords(count, coordsT);
              count++;
            }
            if (basis == 2)
            {
              // makes the (0.5,0.5,0) atom
              coords[0] = coords[0] + (0.5 * latticeConstants.x);
              coords[1] = coords[1] + (0.5 * latticeConstants.y);
              coords[2] = coords[2];
              MatrixMath::Multiply3x3with3x1(gT, coords, coordsT);
              points[iter]->setCoords(count, coordsT);
              count++;
              // makes the (0.5,0,0.5) atom
              coords[0] = coords[0];
              coords[1] = coords[1] - (0.5 * latticeConstants.y);
              coords[2] = coords[2] + (0.5 * latticeConstants.z);
              MatrixMath::Multiply3x3with3x1(gT, coords, coordsT);
              points[iter]->setCoords(count, coordsT);
              count++;
              // makes the (0,0.5,0.5) atom
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
  m_SurfaceMeshFaceLabelsArrayPath(DREAM3D::Defaults::TriangleDataContainerName, DREAM3D::Defaults::FaceAttributeMatrixName, DREAM3D::FaceData::SurfaceMeshFaceLabels),
  m_AvgQuatsArrayPath(DREAM3D::Defaults::ImageDataContainerName, DREAM3D::Defaults::CellFeatureAttributeMatrixName, DREAM3D::FeatureData::AvgQuats),
  m_AtomFeatureLabelsArrayName(DREAM3D::VertexData::AtomFeatureLabels),
  m_SurfaceMeshFaceLabels(NULL),
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
  parameters.push_back(FloatVec3FilterParameter::New("Lattice Constants (Angstroms)", "LatticeConstants", getLatticeConstants(), FilterParameter::Parameter));
  {
    ChoiceFilterParameter::Pointer parameter = ChoiceFilterParameter::New();
    parameter->setHumanLabel("Crystal Basis");
    parameter->setPropertyName("Basis");

    QVector<QString> choices;
    choices.push_back("Simple Cubic");
    choices.push_back("Body Centered Cubic");
    choices.push_back("Face Centered Cubic");
    parameter->setChoices(choices);
    parameter->setCategory(FilterParameter::Parameter);
    parameters.push_back(parameter);
  }
  parameters.push_back(SeparatorFilterParameter::New("Face Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::DataStructureRequirements req = DataArraySelectionFilterParameter::CreateRequirement(DREAM3D::TypeNames::Int32, 2, DREAM3D::AttributeMatrixType::Face, DREAM3D::GeometryType::TriangleGeometry);
    parameters.push_back(DataArraySelectionFilterParameter::New("Face Labels", "SurfaceMeshFaceLabelsArrayPath", getSurfaceMeshFaceLabelsArrayPath(), FilterParameter::RequiredArray, req));
  }
  parameters.push_back(SeparatorFilterParameter::New("Cell Feature Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::DataStructureRequirements req = DataArraySelectionFilterParameter::CreateRequirement(DREAM3D::TypeNames::Float, 4, DREAM3D::AttributeMatrixType::CellFeature, DREAM3D::GeometryType::ImageGeometry);

    parameters.push_back(DataArraySelectionFilterParameter::New("Average Quaternions", "AvgQuatsArrayPath", getAvgQuatsArrayPath(), FilterParameter::RequiredArray, req));
  }

  parameters.push_back(StringFilterParameter::New("Data Container", "VertexDataContainerName", getVertexDataContainerName(), FilterParameter::CreatedArray));
  parameters.push_back(SeparatorFilterParameter::New("Vertex Data", FilterParameter::CreatedArray));
  parameters.push_back(StringFilterParameter::New("Vertex Attribute Matrix", "VertexAttributeMatrixName", getVertexAttributeMatrixName(), FilterParameter::CreatedArray));
  parameters.push_back(StringFilterParameter::New("Atom Feature Labels", "AtomFeatureLabelsArrayName", getAtomFeatureLabelsArrayName(), FilterParameter::CreatedArray));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
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
  DREAM3D_FILTER_WRITE_PARAMETER(FilterVersion)
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
  setErrorCondition(0);
  DataArrayPath tempPath;

  TriangleGeom::Pointer triangles = getDataContainerArray()->getPrereqGeometryFromDataContainer<TriangleGeom, AbstractFilter>(this, getSurfaceMeshFaceLabelsArrayPath().getDataContainerName());

  QVector<IDataArray::Pointer> dataArrays;

  if(getErrorCondition() >= 0) { dataArrays.push_back(triangles->getTriangles()); }

  DataContainer::Pointer v = getDataContainerArray()->createNonPrereqDataContainer<AbstractFilter>(this, getVertexDataContainerName());
  if(getErrorCondition() < 0) { return; }

  VertexGeom::Pointer vertices = VertexGeom::CreateGeometry(0, DREAM3D::Geometry::VertexGeometry);
  v->setGeometry(vertices);

  QVector<size_t> tDims(1, 0);
  AttributeMatrix::Pointer vertexAttrMat = v->createNonPrereqAttributeMatrix<AbstractFilter>(this, getVertexAttributeMatrixName(), tDims, DREAM3D::AttributeMatrixType::Vertex);
  if(getErrorCondition() < 0 || NULL == vertexAttrMat.get()) { return; }

  QVector<size_t> cDims(1, 2);
  m_SurfaceMeshFaceLabelsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getSurfaceMeshFaceLabelsArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_SurfaceMeshFaceLabelsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_SurfaceMeshFaceLabels = m_SurfaceMeshFaceLabelsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCondition() >= 0) { dataArrays.push_back(m_SurfaceMeshFaceLabelsPtr.lock()); }

  cDims[0] = 4;
  m_AvgQuatsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getAvgQuatsArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_AvgQuatsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_AvgQuats = m_AvgQuatsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  cDims[0] = 1;
  tempPath.update(getVertexDataContainerName(), getVertexAttributeMatrixName(), getAtomFeatureLabelsArrayName() );
  m_AtomFeatureLabelsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter, int32_t>(this, tempPath, -301, cDims); /* Assigns the shared_ptr<>(this, tempPath, -301, dims);  Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_AtomFeatureLabelsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_AtomFeatureLabels = m_AtomFeatureLabelsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  getDataContainerArray()->validateNumberOfTuples<AbstractFilter>(this, dataArrays);
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
void InsertAtoms::assign_points(QVector<VertexGeom::Pointer> points, QVector<BoolArrayType::Pointer> inFeature)
{
  size_t count = 0;
  int32_t numFeatures = points.size();
  for (int32_t i = 0; i < numFeatures; i++)
  {
    int64_t numPoints = points[i]->getNumberOfVertices();
    bool* inside = inFeature[i]->getPointer(0);
    for (int64_t j = 0; j < numPoints; j++)
    {
      if (inside[j] == true) { count++; }
    }
  }

  DataContainer::Pointer v = getDataContainerArray()->getDataContainer(getVertexDataContainerName());
  VertexGeom::Pointer vertices = VertexGeom::CreateGeometry(count, DREAM3D::VertexData::SurfaceMeshNodes);

  AttributeMatrix::Pointer vertexAttrMat = v->getAttributeMatrix(getVertexAttributeMatrixName());
  QVector<size_t> tDims(1, count);
  vertexAttrMat->resizeAttributeArrays(tDims);
  updateVertexInstancePointers();

  count = 0;
  float coords[3] = { 0.0f, 0.0f, 0.0f };
  for (int32_t i = 0; i < numFeatures; i++)
  {
    int64_t numPoints = points[i]->getNumberOfVertices();
    bool* inside = inFeature[i]->getPointer(0);
    for (int64_t j = 0; j < numPoints; j++)
    {
      if (inside[j] == true)
      {
        coords[0] = points[i]->getVertexPointer(j)[0];
        coords[1] = points[i]->getVertexPointer(j)[1];
        coords[2] = points[i]->getVertexPointer(j)[2];
        vertices->setCoords(count, coords);
        m_AtomFeatureLabels[count] = i;
        count++;
      }
    }
  }
  v->setGeometry(vertices);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InsertAtoms::execute()
{
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  // Validate that the selected AvgQuats array has tuples equal to the largest
  // Feature Id; the filter would not crash otherwise, but the user should
  // be notified of unanticipated behavior ; this cannot be done in the dataCheck since
  // we don't have acces to the data yet
  int32_t numFeaturesIn = static_cast<int32_t>(m_AvgQuatsPtr.lock()->getNumberOfTuples());
  bool mismatchedFeatures = true;
  int32_t largestFeature = 0;
  size_t numTuples = m_SurfaceMeshFaceLabelsPtr.lock()->getNumberOfTuples();
  for (size_t i = 0; i < numTuples; i++)
  {
    if (m_SurfaceMeshFaceLabels[2 * i] > largestFeature)
    {
      largestFeature = m_SurfaceMeshFaceLabels[2 * i];
      if (largestFeature >= numFeaturesIn)
      {
        mismatchedFeatures = true;
        break;
      }
    }
    else if (m_SurfaceMeshFaceLabels[2 * i + 1] > largestFeature)
    {
      largestFeature = m_SurfaceMeshFaceLabels[2 * i + 1];
      if (largestFeature >= numFeaturesIn)
      {
        mismatchedFeatures = true;
        break;
      }
    }
  }

  if (mismatchedFeatures == true)
  {
    QString ss = QObject::tr("The number of Features in the AvgQuats array (%1) is larger than the largest Feature Id in the SurfaceMeshFaceLabels array").arg(numFeaturesIn);
    setErrorCondition(-5555);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  if (largestFeature != (numFeaturesIn - 1))
  {
    QString ss = QObject::tr("The number of Features in the AvgQuats array (%1) does not match the largest Feature Id in the SurfaceMeshFaceLabels array").arg(numFeaturesIn);
    setErrorCondition(-5555);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  FloatVec3_t latticeConstants;
  latticeConstants.x = m_LatticeConstants.x / 10000.0;
  latticeConstants.y = m_LatticeConstants.y / 10000.0;
  latticeConstants.z = m_LatticeConstants.z / 10000.0;

  DataContainer::Pointer sm = getDataContainerArray()->getDataContainer(getSurfaceMeshFaceLabelsArrayPath().getDataContainerName());
  DREAM3D_RANDOMNG_NEW()

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
  tbb::task_scheduler_init init;
  bool doParallel = true;
#endif

  // pull down faces
  TriangleGeom::Pointer triangleGeom = sm->getGeometryAs<TriangleGeom>();
  int64_t numFaces = m_SurfaceMeshFaceLabelsPtr.lock()->getNumberOfTuples();

  // create array to hold bounding vertices for each face
  FloatArrayType::Pointer llPtr = FloatArrayType::CreateArray(3, "Lower_Left_Internal_Use_Only");
  FloatArrayType::Pointer urPtr = FloatArrayType::CreateArray(3, "Upper_Right_Internal_Use_Only");
  float* ll = llPtr->getPointer(0);
  float* ur = urPtr->getPointer(0);
  VertexGeom::Pointer faceBBs = VertexGeom::CreateGeometry(2 * numFaces, "faceBBs");

  // walk through faces to see how many features there are
  int32_t g1 = 0, g2 = 0;
  int32_t maxFeatureId = 0;
  for (int64_t i = 0; i < numFaces; i++)
  {
    g1 = m_SurfaceMeshFaceLabels[2 * i];
    g2 = m_SurfaceMeshFaceLabels[2 * i + 1];
    if (g1 > maxFeatureId) { maxFeatureId = g1; }
    if (g2 > maxFeatureId) { maxFeatureId = g2; }
  }

  // add one to account for feature 0
  int32_t numFeatures = maxFeatureId + 1;

  // create a dynamic list array to hold face lists
  Int32Int32DynamicListArray::Pointer faceLists = Int32Int32DynamicListArray::New();
  QVector<int32_t> linkCount(numFeatures, 0);

  // fill out lists with number of references to cells
  typedef boost::shared_array<int32_t> SharedInt32Array_t;
  SharedInt32Array_t linkLocPtr(new int32_t[numFaces]);
  int32_t* linkLoc = linkLocPtr.get();

  ::memset(linkLoc, 0, numFaces * sizeof(int32_t));

  // traverse data to determine number of faces belonging to each feature
  for (int64_t i = 0; i < numFaces; i++)
  {
    g1 = m_SurfaceMeshFaceLabels[2 * i];
    g2 = m_SurfaceMeshFaceLabels[2 * i + 1];
    if (g1 > 0) { linkCount[g1]++; }
    if (g2 > 0) { linkCount[g2]++; }
  }

  // now allocate storage for the faces
  faceLists->allocateLists(linkCount);

  // traverse data again to get the faces belonging to each feature
  for (int64_t i = 0; i < numFaces; i++)
  {
    g1 = m_SurfaceMeshFaceLabels[2 * i];
    g2 = m_SurfaceMeshFaceLabels[2 * i + 1];
    if (g1 > 0) { faceLists->insertCellReference(g1, (linkLoc[g1])++, i); }
    if (g2 > 0) { faceLists->insertCellReference(g2, (linkLoc[g2])++, i); }
    // find bounding box for each face
    GeometryMath::FindBoundingBoxOfFace(triangleGeom, i, ll, ur);
    faceBBs->setCoords(2 * i, ll);
    faceBBs->setCoords(2 * i + 1, ur);
  }

  // generate the list of sampling points fom subclass
  QVector<VertexGeom::Pointer> points(numFeatures);
  QVector<BoolArrayType::Pointer> inFeature(numFeatures);
  for (int32_t i = 0; i < numFeatures; i++)
  {
    points[i] = VertexGeom::CreateGeometry(0, "_INTERNAL_USE_ONLY_points");
    inFeature[i] = BoolArrayType::CreateArray(0, "_INTERNAL_USE_ONLY_inside");
  }

  QuatF* avgQuats = reinterpret_cast<QuatF*>(m_AvgQuats);

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
  if (doParallel == true)
  {
    tbb::parallel_for(tbb::blocked_range<size_t>(0, numFeatures),
                      InsertAtomsImpl(triangleGeom, faceLists, faceBBs, avgQuats, latticeConstants, m_Basis, points, inFeature), tbb::auto_partitioner());

  }
  else
#endif
  {
    InsertAtomsImpl serial(triangleGeom, faceLists, faceBBs, avgQuats, latticeConstants, m_Basis, points, inFeature);
    serial.checkPoints(0, numFeatures);
  }

  assign_points(points, inFeature);

  notifyStatusMessage(getHumanLabel(), "Complete");
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
{ return SyntheticBuildingConstants::SyntheticBuildingBaseName; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString InsertAtoms::getGroupName()
{ return DREAM3D::FilterGroups::SyntheticBuildingFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString InsertAtoms::getSubGroupName()
{ return DREAM3D::FilterSubGroups::PackingFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString InsertAtoms::getHumanLabel()
{ return "Insert Atoms"; }
