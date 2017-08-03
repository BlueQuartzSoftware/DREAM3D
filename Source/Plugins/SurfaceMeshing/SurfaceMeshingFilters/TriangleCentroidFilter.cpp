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

#include "TriangleCentroidFilter.h"

#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
#include <tbb/blocked_range.h>
#include <tbb/parallel_for.h>
#include <tbb/partitioner.h>
#include <tbb/task_scheduler_init.h>
#endif

#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/DataArrayCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/Geometry/TriangleGeom.h"

#include "SurfaceMeshing/SurfaceMeshingConstants.h"
#include "SurfaceMeshing/SurfaceMeshingVersion.h"

/**
 * @brief The CalculateCentroidsImpl class implements a threaded algorithm that computes the centroids of
 * each triangle in a set of triangles
 */
class CalculateCentroidsImpl
{
  SharedVertexList::Pointer m_Nodes;
  SharedTriList::Pointer m_Triangles;
  double* m_Centroids;

public:
  CalculateCentroidsImpl(SharedVertexList::Pointer nodes, SharedTriList::Pointer triangles, double* centroids)
  : m_Nodes(nodes)
  , m_Triangles(triangles)
  , m_Centroids(centroids)
  {
  }
  virtual ~CalculateCentroidsImpl()
  {
  }

  void generate(size_t start, size_t end) const
  {
    float* nodes = m_Nodes->getPointer(0);
    int64_t* triangles = m_Triangles->getPointer(0);

    for(size_t i = start; i < end; i++)
    {
      m_Centroids[i * 3] = (nodes[triangles[i * 3] * 3 + 0] + nodes[triangles[i * 3 + 1] * 3 + 0] + nodes[triangles[i * 3 + 2] * 3 + 0]) / 3.0;
      m_Centroids[i * 3 + 1] = (nodes[triangles[i * 3] * 3 + 1] + nodes[triangles[i * 3 + 1] * 3 + 1] + nodes[triangles[i * 3 + 2] * 3 + 1]) / 3.0;
      m_Centroids[i * 3 + 2] = (nodes[triangles[i * 3] * 3 + 2] + nodes[triangles[i * 3 + 1] * 3 + 2] + nodes[triangles[i * 3 + 2] * 3 + 2]) / 3.0;
    }
  }

#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
  void operator()(const tbb::blocked_range<size_t>& r) const
  {
    generate(r.begin(), r.end());
  }
#endif
};

// Include the MOC generated file for this class
#include "moc_TriangleCentroidFilter.cpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TriangleCentroidFilter::TriangleCentroidFilter()
: SurfaceMeshFilter()
, m_SurfaceMeshTriangleCentroidsArrayPath(SIMPL::Defaults::TriangleDataContainerName, SIMPL::Defaults::FaceAttributeMatrixName, SIMPL::FaceData::SurfaceMeshFaceCentroids)
, m_SurfaceMeshTriangleCentroids(nullptr)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TriangleCentroidFilter::~TriangleCentroidFilter()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TriangleCentroidFilter::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(SeparatorFilterParameter::New("Face Data", FilterParameter::CreatedArray));
  {
    DataArrayCreationFilterParameter::RequirementType req = DataArrayCreationFilterParameter::CreateRequirement(AttributeMatrix::Type::Face, IGeometry::Type::Triangle);
    parameters.push_back(SIMPL_NEW_DA_CREATION_FP("Face Centroids", SurfaceMeshTriangleCentroidsArrayPath, FilterParameter::CreatedArray, TriangleCentroidFilter, req));
  }
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TriangleCentroidFilter::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setSurfaceMeshTriangleCentroidsArrayPath(reader->readDataArrayPath("SurfaceMeshTriangleCentroidsArrayPath", getSurfaceMeshTriangleCentroidsArrayPath()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TriangleCentroidFilter::initialize()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TriangleCentroidFilter::dataCheck()
{
  setErrorCondition(0);
  setWarningCondition(0);

  TriangleGeom::Pointer triangles = getDataContainerArray()->getPrereqGeometryFromDataContainer<TriangleGeom, AbstractFilter>(this, getSurfaceMeshTriangleCentroidsArrayPath().getDataContainerName());

  QVector<IDataArray::Pointer> dataArrays;

  if(getErrorCondition() >= 0)
  {
    dataArrays.push_back(triangles->getTriangles());
  }

  QVector<size_t> cDims(1, 3);
  m_SurfaceMeshTriangleCentroidsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<double>, AbstractFilter, double>(
      this, getSurfaceMeshTriangleCentroidsArrayPath(), 0, cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_SurfaceMeshTriangleCentroidsPtr.lock().get())    /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_SurfaceMeshTriangleCentroids = m_SurfaceMeshTriangleCentroidsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCondition() >= 0)
  {
    dataArrays.push_back(m_SurfaceMeshTriangleCentroidsPtr.lock());
  }

  getDataContainerArray()->validateNumberOfTuples<AbstractFilter>(this, dataArrays);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TriangleCentroidFilter::preflight()
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
void TriangleCentroidFilter::execute()
{
  setErrorCondition(0);
  setWarningCondition(0);
  dataCheck();
  if(getErrorCondition() < 0)
  {
    return;
  }

  DataContainer::Pointer sm = getDataContainerArray()->getDataContainer(getSurfaceMeshTriangleCentroidsArrayPath().getDataContainerName());

#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
  tbb::task_scheduler_init init;
  bool doParallel = true;
#endif

  // No check because datacheck() made sure we can do the next line.
  TriangleGeom::Pointer triangleGeom = sm->getGeometryAs<TriangleGeom>();

#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
  if(doParallel == true)
  {
    tbb::parallel_for(tbb::blocked_range<size_t>(0, triangleGeom->getNumberOfTris()), CalculateCentroidsImpl(triangleGeom->getVertices(), triangleGeom->getTriangles(), m_SurfaceMeshTriangleCentroids),
                      tbb::auto_partitioner());
  }
  else
#endif
  {
    CalculateCentroidsImpl serial(triangleGeom->getVertices(), triangleGeom->getTriangles(), m_SurfaceMeshTriangleCentroids);
    serial.generate(0, triangleGeom->getNumberOfTris());
  }

  /* Let the GUI know we are done with this filter */
  notifyStatusMessage(getHumanLabel(), "Complete");
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer TriangleCentroidFilter::newFilterInstance(bool copyFilterParameters)
{
  TriangleCentroidFilter::Pointer filter = TriangleCentroidFilter::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString TriangleCentroidFilter::getCompiledLibraryName()
{
  return SurfaceMeshingConstants::SurfaceMeshingBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString TriangleCentroidFilter::getBrandingString()
{
  return "SurfaceMeshing";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString TriangleCentroidFilter::getFilterVersion()
{
  QString version;
  QTextStream vStream(&version);
  vStream << SurfaceMeshing::Version::Major() << "." << SurfaceMeshing::Version::Minor() << "." << SurfaceMeshing::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString TriangleCentroidFilter::getGroupName()
{
  return SIMPL::FilterGroups::SurfaceMeshingFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString TriangleCentroidFilter::getSubGroupName()
{
  return SIMPL::FilterSubGroups::MiscFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString TriangleCentroidFilter::getHumanLabel()
{
  return "Generate Triangle Centroids";
}
