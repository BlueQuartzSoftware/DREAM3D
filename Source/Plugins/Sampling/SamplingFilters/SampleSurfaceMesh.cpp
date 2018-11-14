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

#include "SampleSurfaceMesh.h"

#include <mutex>
#include <thread>

#include <QtCore/QDateTime>

#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
#include <tbb/blocked_range.h>
#include <tbb/parallel_for.h>
#include <tbb/partitioner.h>
#include <tbb/task_scheduler_init.h>
#include <tbb/tbb_machine.h>
#endif

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/DataArrays/DynamicListArray.hpp"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/Geometry/TriangleGeom.h"
#include "SIMPLib/Geometry/VertexGeom.h"
#include "SIMPLib/Math/GeometryMath.h"
#include "SIMPLib/Math/SIMPLibRandom.h"
#include "SIMPLib/Utilities/TimeUtilities.h"

#include "Sampling/SamplingConstants.h"
#include "Sampling/SamplingVersion.h"

class SampleSurfaceMeshImplByPoints
{
  SampleSurfaceMesh* m_Filter = nullptr;
  TriangleGeom::Pointer m_Faces;
  Int32Int32DynamicListArray::Pointer m_FaceIds;
  VertexGeom::Pointer m_FaceBBs;
  VertexGeom::Pointer m_Points;
  size_t m_FeatureId = 0;
  int32_t* m_PolyIds = nullptr;

public:
  SampleSurfaceMeshImplByPoints(SampleSurfaceMesh* filter, TriangleGeom::Pointer faces, Int32Int32DynamicListArray::Pointer faceIds, VertexGeom::Pointer faceBBs, VertexGeom::Pointer points,
                                size_t featureId, int32_t* polyIds)
  : m_Filter(filter)
  , m_Faces(faces)
  , m_FaceIds(faceIds)
  , m_FaceBBs(faceBBs)
  , m_Points(points)
  , m_FeatureId(featureId)
  , m_PolyIds(polyIds)
  {
  }
  virtual ~SampleSurfaceMeshImplByPoints() = default;

  void checkPoints(size_t start, size_t end) const
  {
    float radius = 0.0f;
    float distToBoundary = 0.0f;
    int64_t numPoints = m_Points->getNumberOfVertices();
    FloatArrayType::Pointer llPtr = FloatArrayType::CreateArray(3, "_INTERNAL_USE_ONLY_Lower");
    FloatArrayType::Pointer urPtr = FloatArrayType::CreateArray(3, "_INTERNAL_USE_ONLY_Upper_Right");
    float* ll = llPtr->getPointer(0);
    float* ur = urPtr->getPointer(0);
    float* point = nullptr;
    char code = ' ';

    size_t iter = m_FeatureId;

    // find bounding box for current feature
    GeometryMath::FindBoundingBoxOfFaces(m_Faces.get(), m_FaceIds->getElementList(iter), ll, ur);
    GeometryMath::FindDistanceBetweenPoints(ll, ur, radius);
    int64_t pointsVisited = 0;
    // check points in vertex array to see if they are in the bounding box of the feature
    for(int64_t i = static_cast<int64_t>(start); i < static_cast<int64_t>(end); i++)
    {
      // Check for the filter being cancelled.
      if(m_Filter->getCancel())
      {
        return;
      }

      point = m_Points->getVertexPointer(i);
      if(m_PolyIds[i] == 0 && GeometryMath::PointInBox(point, ll, ur))
      {
        code = GeometryMath::PointInPolyhedron(m_Faces.get(), m_FaceIds->getElementList(iter), m_FaceBBs.get(), point, ll, ur, radius, distToBoundary);
        if(code == 'i' || code == 'V' || code == 'E' || code == 'F')
        {
          m_PolyIds[i] = iter;
        }
      }
      pointsVisited++;

      if(pointsVisited % 1000 == 0)
      {
        m_Filter->sendThreadSafeProgressMessage(m_FeatureId, 1000, numPoints);
      }
    }
  }

#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
  void operator()(const tbb::blocked_range<size_t>& r) const
  {
    checkPoints(r.begin(), r.end());
  }
#endif
private:
};

/**
 * @brief The SampleSurfaceMeshImpl class implements a threaded algorithm that samples a surface mesh based on points passed from subclassed Filters.
 */
class SampleSurfaceMeshImpl
{
  SampleSurfaceMesh* m_Filter = nullptr;
  TriangleGeom::Pointer m_Faces;
  Int32Int32DynamicListArray::Pointer m_FaceIds;
  VertexGeom::Pointer m_FaceBBs;
  VertexGeom::Pointer m_Points;
  int32_t* m_PolyIds = nullptr;

public:
  SampleSurfaceMeshImpl(SampleSurfaceMesh* filter, TriangleGeom::Pointer faces, Int32Int32DynamicListArray::Pointer faceIds, VertexGeom::Pointer faceBBs, VertexGeom::Pointer points, int32_t* polyIds)
  : m_Filter(filter)
  , m_Faces(faces)
  , m_FaceIds(faceIds)
  , m_FaceBBs(faceBBs)
  , m_Points(points)
  , m_PolyIds(polyIds)
  {
  }
  virtual ~SampleSurfaceMeshImpl() = default;

  void checkPoints(size_t start, size_t end) const
  {
    float radius = 0.0f;
    float distToBoundary = 0.0f;
    int64_t numPoints = m_Points->getNumberOfVertices();
    FloatArrayType::Pointer llPtr = FloatArrayType::CreateArray(3, "_INTERNAL_USE_ONLY_Lower");
    FloatArrayType::Pointer urPtr = FloatArrayType::CreateArray(3, "_INTERNAL_USE_ONLY_Upper_Right");
    float* ll = llPtr->getPointer(0);
    float* ur = urPtr->getPointer(0);
    float* point = nullptr;
    char code = ' ';

    for(size_t iter = start; iter < end; iter++)
    {
      // find bounding box for current feature
      GeometryMath::FindBoundingBoxOfFaces(m_Faces.get(), m_FaceIds->getElementList(iter), ll, ur);
      GeometryMath::FindDistanceBetweenPoints(ll, ur, radius);

      // check points in vertex array to see if they are in the bounding box of the feature
      for(int64_t i = 0; i < numPoints; i++)
      {
        // Check for the filter being cancelled.
        if(m_Filter->getCancel())
        {
          return;
        }

        point = m_Points->getVertexPointer(i);
        if(m_PolyIds[i] == 0 && GeometryMath::PointInBox(point, ll, ur))
        {
          code = GeometryMath::PointInPolyhedron(m_Faces.get(), m_FaceIds->getElementList(iter), m_FaceBBs.get(), point, ll, ur, radius, distToBoundary);
          if(code == 'i' || code == 'V' || code == 'E' || code == 'F')
          {
            m_PolyIds[i] = iter;
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
private:
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SampleSurfaceMesh::SampleSurfaceMesh()
: m_SurfaceMeshFaceLabelsArrayPath(SIMPL::Defaults::TriangleDataContainerName, SIMPL::Defaults::FaceAttributeMatrixName, SIMPL::FaceData::SurfaceMeshFaceLabels)
, m_SurfaceMeshFaceLabels(nullptr)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SampleSurfaceMesh::~SampleSurfaceMesh() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SampleSurfaceMesh::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(SeparatorFilterParameter::New("Face Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Int32, 2, AttributeMatrix::Type::Face, IGeometry::Type::Triangle);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Face Labels", SurfaceMeshFaceLabelsArrayPath, FilterParameter::RequiredArray, SampleSurfaceMesh, req));
  }
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SampleSurfaceMesh::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setSurfaceMeshFaceLabelsArrayPath(reader->readDataArrayPath("SurfaceMeshFaceLabelsArrayPath", getSurfaceMeshFaceLabelsArrayPath()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SampleSurfaceMesh::initialize()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SampleSurfaceMesh::dataCheck()
{
  TriangleGeom::Pointer triangles = getDataContainerArray()->getPrereqGeometryFromDataContainer<TriangleGeom, AbstractFilter>(this, getSurfaceMeshFaceLabelsArrayPath().getDataContainerName());
  if(getErrorCondition() < 0)
  {
    return;
  }

  QVector<IDataArray::Pointer> dataArrays;

  if(getErrorCondition() >= 0)
  {
    dataArrays.push_back(triangles->getTriangles());
  }

  QVector<size_t> cDims(1, 2);
  m_SurfaceMeshFaceLabelsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getSurfaceMeshFaceLabelsArrayPath(),
                                                                                                                   cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_SurfaceMeshFaceLabelsPtr.lock()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_SurfaceMeshFaceLabels = m_SurfaceMeshFaceLabelsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCondition() >= 0)
  {
    dataArrays.push_back(m_SurfaceMeshFaceLabelsPtr.lock());
  }

  getDataContainerArray()->validateNumberOfTuples<AbstractFilter>(this, dataArrays);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SampleSurfaceMesh::preflight()
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
VertexGeom::Pointer SampleSurfaceMesh::generate_points()
{
  return VertexGeom::CreateGeometry(0, "ERROR_SAMPLE_SURFACE_MESH");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SampleSurfaceMesh::assign_points(Int32ArrayType::Pointer iArray)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SampleSurfaceMesh::execute()
{
  setErrorCondition(0);
  setWarningCondition(0);
  dataCheck();
  if(getErrorCondition() < 0)
  {
    return;
  }

  DataContainer::Pointer sm = getDataContainerArray()->getDataContainer(m_SurfaceMeshFaceLabelsArrayPath.getDataContainerName());
  SIMPL_RANDOMNG_NEW()

  TriangleGeom::Pointer triangleGeom = sm->getGeometryAs<TriangleGeom>();

  // pull down faces
  int64_t numFaces = m_SurfaceMeshFaceLabelsPtr.lock()->getNumberOfTuples();

  // create array to hold bounding vertices for each face
  FloatArrayType::Pointer llPtr = FloatArrayType::CreateArray(3, "_INTERNAL_USE_ONLY_Lower_Left");
  FloatArrayType::Pointer urPtr = FloatArrayType::CreateArray(3, "_INTERNAL_USE_ONLY_Upper_Right");
  float* ll = llPtr->getPointer(0);
  float* ur = urPtr->getPointer(0);
  VertexGeom::Pointer faceBBs = VertexGeom::CreateGeometry(2 * numFaces, "_INTERNAL_USE_ONLY_faceBBs");

  notifyStatusMessage(getMessagePrefix(), getHumanLabel(), "Counting number of Features...");

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

  // Check for user canceled flag.
  if(getCancel())
  {
    return;
  }

  // add one to account for feature 0
  int32_t numFeatures = maxFeatureId + 1;

  // create a dynamic list array to hold face lists
  Int32Int32DynamicListArray::Pointer faceLists = Int32Int32DynamicListArray::New();
  std::vector<int32_t> linkCount(numFeatures, 0);

  // fill out lists with number of references to cells
  Int32ArrayType::Pointer linkLocPtr = Int32ArrayType::CreateArray(numFaces, "_INTERNAL_USE_ONLY_cell refs");
  linkLocPtr->initializeWithZeros();
  int32_t* linkLoc = linkLocPtr->getPointer(0);

  notifyStatusMessage(getMessagePrefix(), getHumanLabel(), "Counting number of triangle faces per feature ...");

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

  // Check for user canceled flag.
  if(getCancel())
  {
    return;
  }

  // now allocate storage for the faces
  faceLists->allocateLists(linkCount);

  notifyStatusMessage(getMessagePrefix(), getHumanLabel(), "Allocating triangle faces per feature ...");

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

  // Check for user canceled flag.
  if(getCancel())
  {
    return;
  }

  notifyStatusMessage(getMessagePrefix(), getHumanLabel(), "Vertex Geometry generating sampling points");

  // generate the list of sampling points from subclass
  VertexGeom::Pointer points = generate_points();
  if(getErrorCondition() < 0 || nullptr == points.get())
  {
    return;
  }
  int64_t numPoints = points->getNumberOfVertices();

  // create array to hold which polyhedron (feature) each point falls in
  Int32ArrayType::Pointer iArray = Int32ArrayType::NullPointer();
  iArray = Int32ArrayType::CreateArray(numPoints, "_INTERNAL_USE_ONLY_polyhedronIds");
  iArray->initializeWithZeros();
  int32_t* polyIds = iArray->getPointer(0);

  notifyStatusMessage(getMessagePrefix(), getHumanLabel(), "Sampling triangle geometry ...");

#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
  tbb::task_scheduler_init init;
  bool doParallel = true;
#endif

  // C++11 RIGHT HERE....
  int32_t nthreads = static_cast<int32_t>(std::thread::hardware_concurrency()); // Returns ZERO if not defined on this platform
  // If the number of features is larger than the number of cores to do the work then parallelize over the number of features
  // otherwise parallelize over the number of triangle points.
  if(numFeatures > nthreads)
  {
#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
    if(doParallel)
    {
      tbb::parallel_for(tbb::blocked_range<size_t>(0, numFeatures), SampleSurfaceMeshImpl(this, triangleGeom, faceLists, faceBBs, points, polyIds), tbb::auto_partitioner());
    }
    else
#endif
    {
      SampleSurfaceMeshImpl serial(this, triangleGeom, faceLists, faceBBs, points, polyIds);
      serial.checkPoints(0, numFeatures);
    }
  }
  else
  {
    for(int featureId = 0; featureId < numFeatures; featureId++)
    {
      m_NumCompleted = 0;
      m_StartMillis = QDateTime::currentMSecsSinceEpoch();
      m_Millis = m_StartMillis;
      size_t numPoints = points->getNumberOfVertices();
#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
      if(doParallel)
      {
        tbb::parallel_for(tbb::blocked_range<size_t>(0, numPoints), SampleSurfaceMeshImplByPoints(this, triangleGeom, faceLists, faceBBs, points, featureId, polyIds), tbb::auto_partitioner());
      }
      else
#endif
      {
        SampleSurfaceMeshImplByPoints serial(this, triangleGeom, faceLists, faceBBs, points, featureId, polyIds);
        serial.checkPoints(0, numPoints);
      }
    }
  }
  assign_points(iArray);

  notifyStatusMessage(getMessagePrefix(), getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SampleSurfaceMesh::sendThreadSafeProgressMessage(int featureId, size_t numCompleted, size_t totalFeatures)
{
  static std::mutex mutex;
  std::lock_guard<std::mutex> lock(mutex);
  qint64 currentMillis = QDateTime::currentMSecsSinceEpoch();
  m_NumCompleted = m_NumCompleted + numCompleted;
  if(currentMillis - m_Millis > 1000)
  {
    float inverseRate = static_cast<float>(currentMillis - m_Millis) / static_cast<float>(m_NumCompleted - m_LastCompletedPoints);
    qint64 remainMillis = inverseRate * (totalFeatures - m_NumCompleted);
    QString ss = QObject::tr("Feature %3 | Points Completed: %1 of %2").arg(m_NumCompleted).arg(totalFeatures).arg(featureId);
    ss = ss + QObject::tr(" || Est. Time Remain: %1").arg(DREAM3D::convertMillisToHrsMinSecs(remainMillis));
    notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);
    m_Millis = QDateTime::currentMSecsSinceEpoch();
    m_LastCompletedPoints = m_NumCompleted;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer SampleSurfaceMesh::newFilterInstance(bool copyFilterParameters) const
{
  SampleSurfaceMesh::Pointer filter = SampleSurfaceMesh::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString SampleSurfaceMesh::getCompiledLibraryName() const
{
  return SamplingConstants::SamplingBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString SampleSurfaceMesh::getBrandingString() const
{
  return "Sampling";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString SampleSurfaceMesh::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << Sampling::Version::Major() << "." << Sampling::Version::Minor() << "." << Sampling::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString SampleSurfaceMesh::getGroupName() const
{
  return SIMPL::FilterGroups::SamplingFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QUuid SampleSurfaceMesh::getUuid()
{
  return QUuid("{1fc20816-a312-51ae-b89f-fa25e5042a03}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString SampleSurfaceMesh::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::ResolutionFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString SampleSurfaceMesh::getHumanLabel() const
{
  return "Sample Triangle Geometry";
}
