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

#include "InsertAtoms.h"

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/DataArrays/DynamicListArray.hpp"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/ChoiceFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/DataContainerCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/FloatVec3FilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedPathCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/Math/GeometryMath.h"
#include "SIMPLib/Geometry/TriangleGeom.h"
#include "SIMPLib/Geometry/VertexGeom.h"
#include "SIMPLib/Math/SIMPLibRandom.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/DataContainers/DataContainer.h"

#include "OrientationLib/Core/Orientation.hpp"
#include "OrientationLib/Core/OrientationTransformation.hpp"
#include "OrientationLib/Core/Quaternion.hpp"

#include "SyntheticBuilding/SyntheticBuildingConstants.h"
#include "SyntheticBuilding/SyntheticBuildingVersion.h"

#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
#include <tbb/blocked_range.h>
#include <tbb/parallel_for.h>
#include <tbb/partitioner.h>
#include <tbb/task_scheduler_init.h>
#endif

using QuatF = Quaternion<float>;

enum createdPathID : RenameDataPath::DataID_t
{
  AttributeMatrixID21 = 21,

  DataContainerID = 1
};

/**
 * @brief The InsertAtomsImpl class implements a threaded algorithm that inserts vertex points ('atoms') onto surface meshed Features
 */
class InsertAtomsImpl
{
  TriangleGeom::Pointer m_Faces;
  Int32Int32DynamicListArray::Pointer m_FaceIds;
  VertexGeom::Pointer m_FaceBBs;
  float* m_AvgQuats;
  FloatVec3Type m_LatticeConstants;
  uint32_t m_Basis;
  QVector<VertexGeom::Pointer> m_Points;
  QVector<BoolArrayType::Pointer> m_InFeature;

public:
  InsertAtomsImpl(const TriangleGeom::Pointer& faces, const Int32Int32DynamicListArray::Pointer& faceIds, const VertexGeom::Pointer& faceBBs, float* avgQuats, FloatVec3Type latticeConstants,
                  uint32_t basis, const QVector<VertexGeom::Pointer>& points, const QVector<BoolArrayType::Pointer>& inFeature)
  : m_Faces(faces)
  , m_FaceIds(faceIds)
  , m_FaceBBs(faceBBs)
  , m_AvgQuats(avgQuats)
  , m_LatticeConstants(latticeConstants)
  , m_Basis(basis)
  , m_Points(points)
  , m_InFeature(inFeature)
  {
  }
  virtual ~InsertAtomsImpl() = default;

  void checkPoints(size_t start, size_t end) const
  {
    float radius = 0.0f;
    FloatArrayType::Pointer llPtr = FloatArrayType::CreateArray(3, "_INTERNAL_USE_ONLY_Lower_Left", true);
    FloatArrayType::Pointer urPtr = FloatArrayType::CreateArray(3, "_INTERNAL_USE_ONLY_Upper_Right", true);
    FloatArrayType::Pointer ll_rotPtr = FloatArrayType::CreateArray(3, "_INTERNAL_USE_ONLY_Lower_Left_Rotated", true);
    FloatArrayType::Pointer ur_rotPtr = FloatArrayType::CreateArray(3, "_INTERNAL_USE_ONLY_Upper_Right_Rotated", true);
    float* ll = llPtr->getPointer(0);
    float* ur = urPtr->getPointer(0);
    float* ll_rot = ll_rotPtr->getPointer(0);
    float* ur_rot = ur_rotPtr->getPointer(0);
    float* point = nullptr;
    char code = ' ';
    float g[3][3] = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};

    for(size_t iter = start; iter < end; iter++)
    {
      Int32Int32DynamicListArray::ElementList& faceIds = m_FaceIds->getElementList(iter);

      QuatF q1(m_AvgQuats + iter * 4);
      OrientationTransformation::qu2om<QuatF, Orientation<float>>(q1).toGMatrix(g);

      // find bounding box for current feature
      GeometryMath::FindBoundingBoxOfFaces(m_Faces.get(), faceIds, ll, ur);
      GeometryMath::FindBoundingBoxOfRotatedFaces(m_Faces.get(), faceIds, g, ll_rot, ur_rot);
      GeometryMath::FindDistanceBetweenPoints(ll, ur, radius);

      generatePoints(iter, m_Points, m_InFeature, m_AvgQuats, m_LatticeConstants, m_Basis, ll_rot, ur_rot);

      // check points in vertex array to see if they are in the bounding box of the feature
      int64_t numPoints = m_Points[iter]->getNumberOfVertices();
      VertexGeom::Pointer vertArray = m_Points[iter];
      BoolArrayType::Pointer boolArray = m_InFeature[iter];
      for(int64_t i = 0; i < numPoints; i++)
      {
        point = vertArray->getVertexPointer(i);
        if(!boolArray->getValue(i))
        {
          code = GeometryMath::PointInPolyhedron(m_Faces.get(), faceIds, m_FaceBBs.get(), point, ll, ur, radius);
          if(code == 'i' || code == 'V' || code == 'E' || code == 'F')
          {
            m_InFeature[iter]->setValue(i, true);
          }
        }
      }
    }
  }

#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
  void operator()(const tbb::blocked_range<size_t>& r) const
  {
    checkPoints(r.begin(), r.end());
  }
#endif

  void generatePoints(size_t iter, QVector<VertexGeom::Pointer> points, QVector<BoolArrayType::Pointer> inFeature, float* m_AvgQuats, FloatVec3Type latticeConstants, uint32_t basis, const float* ll,
                      const float* ur) const
  {
    float g[3][3] = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};
    float gT[3][3] = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};

    QuatF q1(m_AvgQuats + iter * 4);
    OrientationTransformation::qu2om<QuatF, Orientation<float>>(q1).toGMatrix(g);
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
    if(basis == 0)
    {
      atomMult = 1;
    }
    if(basis == 1)
    {
      atomMult = 2;
    }
    if(basis == 2)
    {
      atomMult = 4;
    }
    if(basis == 3)
    {
      atomMult = 8;
    }
    int64_t xPoints = (int64_t(deltaX / latticeConstants[0]) + 1);
    int64_t yPoints = (int64_t(deltaY / latticeConstants[1]) + 1);
    int64_t zPoints = (int64_t(deltaZ / latticeConstants[2]) + 1);
    int64_t nPoints = atomMult * xPoints * yPoints * zPoints;

    points[iter]->resizeVertexList(nPoints);
    inFeature[iter]->resizeTuples(nPoints);

    int64_t count = 0;
    float coords[3] = {0.0f, 0.0f, 0.0f};
    float coordsT[3] = {0.0f, 0.0f, 0.0f};
    for(int64_t k = 0; k < zPoints; k++)
    {
      for(int64_t j = 0; j < yPoints; j++)
      {
        for(int64_t i = 0; i < xPoints; i++)
        {
          coords[0] = float(i) * latticeConstants[0] + minx;
          coords[1] = float(j) * latticeConstants[1] + miny;
          coords[2] = float(k) * latticeConstants[2] + minz;
          MatrixMath::Multiply3x3with3x1(gT, coords, coordsT);
          points[iter]->setCoords(count, coordsT);
          count++;
          if(basis == 1)
          {
            coords[0] = coords[0] + (0.5f * latticeConstants[0]);
            coords[1] = coords[1] + (0.5f * latticeConstants[1]);
            coords[2] = coords[2] + (0.5f * latticeConstants[2]);
            MatrixMath::Multiply3x3with3x1(gT, coords, coordsT);
            points[iter]->setCoords(count, coordsT);
            count++;
          }
          if(basis == 2)
          {
            // makes the (0.5,0.5,0) atom
            coords[0] = coords[0] + (0.5f * latticeConstants[0]);
            coords[1] = coords[1] + (0.5f * latticeConstants[1]);
            coords[2] = coords[2];
            MatrixMath::Multiply3x3with3x1(gT, coords, coordsT);
            points[iter]->setCoords(count, coordsT);
            count++;
            // makes the (0.5,0,0.5) atom
            coords[0] = coords[0];
            coords[1] = coords[1] - (0.5f * latticeConstants[1]);
            coords[2] = coords[2] + (0.5f * latticeConstants[2]);
            MatrixMath::Multiply3x3with3x1(gT, coords, coordsT);
            points[iter]->setCoords(count, coordsT);
            count++;
            // makes the (0,0.5,0.5) atom
            coords[0] = coords[0] - (0.5f * latticeConstants[0]);
            coords[1] = coords[1] + (0.5f * latticeConstants[1]);
            coords[2] = coords[2];
            MatrixMath::Multiply3x3with3x1(gT, coords, coordsT);
            points[iter]->setCoords(count, coordsT);
            count++;
          }
          if(basis == 3)
          {
            // (+0.25,+0.25,+0.25) for (0,0,0)
            coords[0] = coords[0] + (0.25f * latticeConstants[0]);
            coords[1] = coords[1] - (0.25f * latticeConstants[1]);
            coords[2] = coords[2] - (0.25f * latticeConstants[2]);
            MatrixMath::Multiply3x3with3x1(gT, coords, coordsT);
            points[iter]->setCoords(count, coordsT);
            count++;
            // (+0.25,+0.25,+0.25) for (0.5,0.5,0)
            coords[0] = coords[0] + (0.5f * latticeConstants[0]);
            coords[1] = coords[1] + (0.5f * latticeConstants[1]);
            coords[2] = coords[2];
            MatrixMath::Multiply3x3with3x1(gT, coords, coordsT);
            points[iter]->setCoords(count, coordsT);
            count++;
            // (+0.25,+0.25,+0.25) for (0.5,0,0.5)
            coords[0] = coords[0];
            coords[1] = coords[1] - (0.5f * latticeConstants[1]);
            coords[2] = coords[2] + (0.5f * latticeConstants[2]);
            MatrixMath::Multiply3x3with3x1(gT, coords, coordsT);
            points[iter]->setCoords(count, coordsT);
            count++;
            // (+0.25,+0.25,+0.25) for (0,0.5,0.5)
            coords[0] = coords[0] - (0.5f * latticeConstants[0]);
            coords[1] = coords[1] + (0.5f * latticeConstants[1]);
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
InsertAtoms::InsertAtoms()
: m_VertexDataContainerName(SIMPL::Defaults::VertexDataContainerName)
, m_VertexAttributeMatrixName(SIMPL::Defaults::VertexAttributeMatrixName)
, m_Basis(0)
, m_SurfaceMeshFaceLabelsArrayPath(SIMPL::Defaults::TriangleDataContainerName, SIMPL::Defaults::FaceAttributeMatrixName, SIMPL::FaceData::SurfaceMeshFaceLabels)
, m_AvgQuatsArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellFeatureAttributeMatrixName, SIMPL::FeatureData::AvgQuats)
, m_AtomFeatureLabelsArrayName(SIMPL::VertexData::AtomFeatureLabels)
{
  m_LatticeConstants[0] = 1.0f;
  m_LatticeConstants[1] = 1.0f;
  m_LatticeConstants[2] = 1.0f;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
InsertAtoms::~InsertAtoms() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InsertAtoms::setupFilterParameters()
{
  FilterParameterVectorType parameters;
  parameters.push_back(SIMPL_NEW_FLOAT_VEC3_FP("Lattice Constants (Angstroms)", LatticeConstants, FilterParameter::Parameter, InsertAtoms));
  {
    ChoiceFilterParameter::Pointer parameter = ChoiceFilterParameter::New();
    parameter->setHumanLabel("Crystal Basis");
    parameter->setPropertyName("Basis");
    parameter->setSetterCallback(SIMPL_BIND_SETTER(InsertAtoms, this, Basis));
    parameter->setGetterCallback(SIMPL_BIND_GETTER(InsertAtoms, this, Basis));

    QVector<QString> choices;
    choices.push_back("Simple Cubic");
    choices.push_back("Body Centered Cubic");
    choices.push_back("Face Centered Cubic");
    choices.push_back("Cubic Diamond");
    parameter->setChoices(choices);
    parameter->setCategory(FilterParameter::Parameter);
    parameters.push_back(parameter);
  }
  parameters.push_back(SeparatorFilterParameter::New("Face Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Int32, 2, AttributeMatrix::Type::Face, IGeometry::Type::Triangle);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Face Labels", SurfaceMeshFaceLabelsArrayPath, FilterParameter::RequiredArray, InsertAtoms, req));
  }
  parameters.push_back(SeparatorFilterParameter::New("Cell Feature Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req =
        DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Float, 4, AttributeMatrix::Type::CellFeature, IGeometry::Type::Image);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Average Quaternions", AvgQuatsArrayPath, FilterParameter::RequiredArray, InsertAtoms, req));
  }
  parameters.push_back(SIMPL_NEW_DC_CREATION_FP("Data Container", VertexDataContainerName, FilterParameter::CreatedArray, InsertAtoms));
  parameters.push_back(SeparatorFilterParameter::New("Vertex Data", FilterParameter::CreatedArray));
  parameters.push_back(SIMPL_NEW_AM_WITH_LINKED_DC_FP("Vertex Attribute Matrix", VertexAttributeMatrixName, VertexDataContainerName, FilterParameter::CreatedArray, InsertAtoms));
  parameters.push_back(SIMPL_NEW_DA_WITH_LINKED_AM_FP("Atom Feature Labels", AtomFeatureLabelsArrayName, VertexDataContainerName, VertexAttributeMatrixName, FilterParameter::CreatedArray, InsertAtoms));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InsertAtoms::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setVertexDataContainerName(reader->readDataArrayPath("VertexDataContainerName", getVertexDataContainerName()));
  setVertexAttributeMatrixName(reader->readString("VertexAttributeMatrixName", getVertexAttributeMatrixName()));
  setAtomFeatureLabelsArrayName(reader->readString("AtomFeatureLabelsArrayName", getAtomFeatureLabelsArrayName()));
  setAvgQuatsArrayPath(reader->readDataArrayPath("AvgQuatsArrayPath", getAvgQuatsArrayPath()));
  setSurfaceMeshFaceLabelsArrayPath(reader->readDataArrayPath("SurfaceMeshFaceLabelsArrayPath", getSurfaceMeshFaceLabelsArrayPath()));
  setLatticeConstants(reader->readFloatVec3("LatticeConstants", getLatticeConstants()));
  setBasis(reader->readValue("Basis", getBasis()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InsertAtoms::updateVertexInstancePointers()
{
  clearErrorCode();
  clearWarningCode();
  if(nullptr != m_AtomFeatureLabelsPtr.lock()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_AtomFeatureLabels = m_AtomFeatureLabelsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InsertAtoms::initialize()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InsertAtoms::dataCheck()
{
  clearErrorCode();
  clearWarningCode();
  DataArrayPath tempPath;

  TriangleGeom::Pointer triangles = getDataContainerArray()->getPrereqGeometryFromDataContainer<TriangleGeom, AbstractFilter>(this, getSurfaceMeshFaceLabelsArrayPath().getDataContainerName());

  QVector<IDataArray::Pointer> dataArrays;

  if(getErrorCode() >= 0)
  {
    dataArrays.push_back(triangles->getTriangles());
  }

  DataContainer::Pointer v = getDataContainerArray()->createNonPrereqDataContainer<AbstractFilter>(this, getVertexDataContainerName(), DataContainerID);
  if(getErrorCode() < 0)
  {
    return;
  }

  VertexGeom::Pointer vertices = VertexGeom::CreateGeometry(0, SIMPL::Geometry::VertexGeometry, !getInPreflight());
  v->setGeometry(vertices);

  std::vector<size_t> tDims(1, 0);
  AttributeMatrix::Pointer vertexAttrMat = v->createNonPrereqAttributeMatrix(this, getVertexAttributeMatrixName(), tDims, AttributeMatrix::Type::Vertex, AttributeMatrixID21);
  if(getErrorCode() < 0 || nullptr == vertexAttrMat.get())
  {
    return;
  }

  std::vector<size_t> cDims(1, 2);
  m_SurfaceMeshFaceLabelsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getSurfaceMeshFaceLabelsArrayPath(),
                                                                                                                   cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_SurfaceMeshFaceLabelsPtr.lock()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_SurfaceMeshFaceLabels = m_SurfaceMeshFaceLabelsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCode() >= 0)
  {
    dataArrays.push_back(m_SurfaceMeshFaceLabelsPtr.lock());
  }

  cDims[0] = 4;
  m_AvgQuatsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getAvgQuatsArrayPath(),
                                                                                                    cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_AvgQuatsPtr.lock())                                                                       /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_AvgQuats = m_AvgQuatsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  cDims[0] = 1;
  tempPath.update(getVertexDataContainerName().getDataContainerName(), getVertexAttributeMatrixName(), getAtomFeatureLabelsArrayName());
  m_AtomFeatureLabelsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter, int32_t>(
      this, tempPath, -301, cDims);                  /* Assigns the shared_ptr<>(this, tempPath, -301, dims);  Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_AtomFeatureLabelsPtr.lock())       /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_AtomFeatureLabels = m_AtomFeatureLabelsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  getDataContainerArray()->validateNumberOfTuples<AbstractFilter>(this, dataArrays);

  if(m_Basis < 0 || m_Basis > 3)
  {
    setErrorCondition(-302, "Basis value out of range. Valid values are: 0=Simpl Cubic, 1=Body Centered Cubic, 2=Face Centered Cubic, 3=Cubic Diamond");
  }
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
  for(int32_t i = 0; i < numFeatures; i++)
  {
    int64_t numPoints = points[i]->getNumberOfVertices();
    bool* inside = inFeature[i]->getPointer(0);
    for(int64_t j = 0; j < numPoints; j++)
    {
      if(inside[j])
      {
        count++;
      }
    }
  }

  DataContainer::Pointer v = getDataContainerArray()->getDataContainer(getVertexDataContainerName());

  VertexGeom::Pointer vertices = VertexGeom::CreateGeometry(count, SIMPL::VertexData::SurfaceMeshNodes);

  AttributeMatrix::Pointer vertexAttrMat = v->getAttributeMatrix(getVertexAttributeMatrixName());
  std::vector<size_t> tDims(1, count);
  vertexAttrMat->resizeAttributeArrays(tDims);
  updateVertexInstancePointers();

  count = 0;
  float coords[3] = {0.0f, 0.0f, 0.0f};
  for(int32_t i = 0; i < numFeatures; i++)
  {
    int64_t numPoints = points[i]->getNumberOfVertices();
    bool* inside = inFeature[i]->getPointer(0);
    for(int64_t j = 0; j < numPoints; j++)
    {
      if(inside[j])
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
  clearErrorCode();
  clearWarningCode();
  dataCheck();
  if(getErrorCode() < 0)
  {
    return;
  }

  // Validate that the selected AvgQuats array has tuples equal to the largest
  // Feature Id; the filter would not crash otherwise, but the user should
  // be notified of unanticipated behavior ; this cannot be done in the dataCheck since
  // we don't have acces to the data yet
  int32_t numFeaturesIn = static_cast<int32_t>(m_AvgQuatsPtr.lock()->getNumberOfTuples());
  bool mismatchedFeatures = false;
  int32_t largestFeature = 0;
  size_t numTuples = m_SurfaceMeshFaceLabelsPtr.lock()->getNumberOfTuples();
  for(size_t i = 0; i < numTuples; i++)
  {
    if(m_SurfaceMeshFaceLabels[2 * i] > largestFeature)
    {
      largestFeature = m_SurfaceMeshFaceLabels[2 * i];
      if(largestFeature >= numFeaturesIn)
      {
        mismatchedFeatures = true;
        break;
      }
    }
    else if(m_SurfaceMeshFaceLabels[2 * i + 1] > largestFeature)
    {
      largestFeature = m_SurfaceMeshFaceLabels[2 * i + 1];
      if(largestFeature >= numFeaturesIn)
      {
        mismatchedFeatures = true;
        break;
      }
    }
  }

  if(mismatchedFeatures)
  {
    QString ss = QObject::tr("The number of Features in the AvgQuats array (%1) is larger than the largest Feature Id in the SurfaceMeshFaceLabels array").arg(numFeaturesIn);
    setErrorCondition(-5555, ss);
    return;
  }

  if(largestFeature != (numFeaturesIn - 1))
  {
    QString ss = QObject::tr("The number of Features in the AvgQuats array (%1) does not match the largest Feature Id in the SurfaceMeshFaceLabels array").arg(numFeaturesIn);
    setErrorCondition(-5555, ss);
    return;
  }

  FloatVec3Type latticeConstants;
  latticeConstants[0] = m_LatticeConstants[0] / 10000.0f;
  latticeConstants[1] = m_LatticeConstants[1] / 10000.0f;
  latticeConstants[2] = m_LatticeConstants[2] / 10000.0f;

  DataContainer::Pointer sm = getDataContainerArray()->getDataContainer(getSurfaceMeshFaceLabelsArrayPath().getDataContainerName());
  SIMPL_RANDOMNG_NEW()

#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
  tbb::task_scheduler_init init;
  bool doParallel = true;
#endif

  // pull down faces
  TriangleGeom::Pointer triangleGeom = sm->getGeometryAs<TriangleGeom>();
  int64_t numFaces = m_SurfaceMeshFaceLabelsPtr.lock()->getNumberOfTuples();

  // create array to hold bounding vertices for each face
  FloatArrayType::Pointer llPtr = FloatArrayType::CreateArray(3, "Lower_Left_Internal_Use_Only", true);
  FloatArrayType::Pointer urPtr = FloatArrayType::CreateArray(3, "Upper_Right_Internal_Use_Only", true);
  float* ll = llPtr->getPointer(0);
  float* ur = urPtr->getPointer(0);
  VertexGeom::Pointer faceBBs = VertexGeom::CreateGeometry(2 * numFaces, "faceBBs");

  // walk through faces to see how many features there are
  int32_t g1 = 0, g2 = 0;
  int32_t maxFeatureId = 0;
  for(int64_t i = 0; i < numFaces; i++)
  {
    g1 = m_SurfaceMeshFaceLabels[2 * i];
    g2 = m_SurfaceMeshFaceLabels[2 * i + 1];
    if(g1 > maxFeatureId)
    {
      maxFeatureId = g1;
    }
    if(g2 > maxFeatureId)
    {
      maxFeatureId = g2;
    }
  }

  // add one to account for feature 0
  int32_t numFeatures = maxFeatureId + 1;

  // create a dynamic list array to hold face lists
  Int32Int32DynamicListArray::Pointer faceLists = Int32Int32DynamicListArray::New();
  QVector<int32_t> linkCount(numFeatures, 0);

  // fill out lists with number of references to cells
  Int32ArrayType::Pointer linkLocPtr = Int32ArrayType::CreateArray(numFaces, "_INTERNAL_USE_ONLY_cell refs", true);
  linkLocPtr->initializeWithZeros();
  int32_t* linkLoc = linkLocPtr->getPointer(0);

  // traverse data to determine number of faces belonging to each feature
  for(int64_t i = 0; i < numFaces; i++)
  {
    g1 = m_SurfaceMeshFaceLabels[2 * i];
    g2 = m_SurfaceMeshFaceLabels[2 * i + 1];
    if(g1 > 0)
    {
      linkCount[g1]++;
    }
    if(g2 > 0)
    {
      linkCount[g2]++;
    }
  }

  // now allocate storage for the faces
  faceLists->allocateLists(linkCount);

  // traverse data again to get the faces belonging to each feature
  for(int64_t i = 0; i < numFaces; i++)
  {
    g1 = m_SurfaceMeshFaceLabels[2 * i];
    g2 = m_SurfaceMeshFaceLabels[2 * i + 1];
    if(g1 > 0)
    {
      faceLists->insertCellReference(g1, (linkLoc[g1])++, i);
    }
    if(g2 > 0)
    {
      faceLists->insertCellReference(g2, (linkLoc[g2])++, i);
    }
    // find bounding box for each face
    GeometryMath::FindBoundingBoxOfFace(triangleGeom.get(), i, ll, ur);
    faceBBs->setCoords(2 * i, ll);
    faceBBs->setCoords(2 * i + 1, ur);
  }

  // generate the list of sampling points fom subclass
  QVector<VertexGeom::Pointer> points(numFeatures);
  QVector<BoolArrayType::Pointer> inFeature(numFeatures);
  for(int32_t i = 0; i < numFeatures; i++)
  {
    points[i] = VertexGeom::CreateGeometry(0, "_INTERNAL_USE_ONLY_points");
    inFeature[i] = BoolArrayType::CreateArray(0, "_INTERNAL_USE_ONLY_inside", true);
  }


#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
  if(doParallel)
  {
    tbb::parallel_for(tbb::blocked_range<size_t>(0, numFeatures), InsertAtomsImpl(triangleGeom, faceLists, faceBBs, m_AvgQuats, latticeConstants, m_Basis, points, inFeature), tbb::auto_partitioner());
  }
  else
#endif
  {
    InsertAtomsImpl serial(triangleGeom, faceLists, faceBBs, m_AvgQuats, latticeConstants, m_Basis, points, inFeature);
    serial.checkPoints(0, numFeatures);
  }

  assign_points(points, inFeature);

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer InsertAtoms::newFilterInstance(bool copyFilterParameters) const
{
  InsertAtoms::Pointer filter = InsertAtoms::New();
  if(copyFilterParameters)
  {
    filter->setFilterParameters(getFilterParameters());
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString InsertAtoms::getCompiledLibraryName() const
{
  return SyntheticBuildingConstants::SyntheticBuildingBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString InsertAtoms::getBrandingString() const
{
  return "SyntheticBuilding";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString InsertAtoms::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << SyntheticBuilding::Version::Major() << "." << SyntheticBuilding::Version::Minor() << "." << SyntheticBuilding::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString InsertAtoms::getGroupName() const
{
  return SIMPL::FilterGroups::SyntheticBuildingFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid InsertAtoms::getUuid() const
{
  return QUuid("{4ee65edd-8d7f-5b0b-a7dd-c4b96e272a87}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString InsertAtoms::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::PackingFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString InsertAtoms::getHumanLabel() const
{
  return "Insert Atoms";
}

// -----------------------------------------------------------------------------
InsertAtoms::Pointer InsertAtoms::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<InsertAtoms> InsertAtoms::New()
{
  struct make_shared_enabler : public InsertAtoms
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString InsertAtoms::getNameOfClass() const
{
  return QString("InsertAtoms");
}

// -----------------------------------------------------------------------------
QString InsertAtoms::ClassName()
{
  return QString("InsertAtoms");
}

// -----------------------------------------------------------------------------
void InsertAtoms::setVertexDataContainerName(const DataArrayPath& value)
{
  m_VertexDataContainerName = value;
}

// -----------------------------------------------------------------------------
DataArrayPath InsertAtoms::getVertexDataContainerName() const
{
  return m_VertexDataContainerName;
}

// -----------------------------------------------------------------------------
void InsertAtoms::setVertexAttributeMatrixName(const QString& value)
{
  m_VertexAttributeMatrixName = value;
}

// -----------------------------------------------------------------------------
QString InsertAtoms::getVertexAttributeMatrixName() const
{
  return m_VertexAttributeMatrixName;
}

// -----------------------------------------------------------------------------
void InsertAtoms::setLatticeConstants(const FloatVec3Type& value)
{
  m_LatticeConstants = value;
}

// -----------------------------------------------------------------------------
FloatVec3Type InsertAtoms::getLatticeConstants() const
{
  return m_LatticeConstants;
}

// -----------------------------------------------------------------------------
void InsertAtoms::setBasis(int value)
{
  m_Basis = value;
}

// -----------------------------------------------------------------------------
int InsertAtoms::getBasis() const
{
  return m_Basis;
}

// -----------------------------------------------------------------------------
void InsertAtoms::setSurfaceMeshFaceLabelsArrayPath(const DataArrayPath& value)
{
  m_SurfaceMeshFaceLabelsArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath InsertAtoms::getSurfaceMeshFaceLabelsArrayPath() const
{
  return m_SurfaceMeshFaceLabelsArrayPath;
}

// -----------------------------------------------------------------------------
void InsertAtoms::setAvgQuatsArrayPath(const DataArrayPath& value)
{
  m_AvgQuatsArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath InsertAtoms::getAvgQuatsArrayPath() const
{
  return m_AvgQuatsArrayPath;
}

// -----------------------------------------------------------------------------
void InsertAtoms::setAtomFeatureLabelsArrayName(const QString& value)
{
  m_AtomFeatureLabelsArrayName = value;
}

// -----------------------------------------------------------------------------
QString InsertAtoms::getAtomFeatureLabelsArrayName() const
{
  return m_AtomFeatureLabelsArrayName;
}
