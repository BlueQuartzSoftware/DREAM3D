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


#include "TriangleAreaFilter.h"

#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
#include <tbb/parallel_for.h>
#include <tbb/blocked_range.h>
#include <tbb/partitioner.h>
#include <tbb/task_scheduler_init.h>
#endif

#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersWriter.h"
#include "SIMPLib/FilterParameters/DataArrayCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"

#include "SIMPLib/Math/MatrixMath.h"

#include "SurfaceMeshing/SurfaceMeshingConstants.h"

#define SQR(value) (value)*(value)

/**
 * @brief The CalculateAreasImpl class implements a threaded algorithm that computes the area of each
 * triangle for a set of triangles
 */
class CalculateAreasImpl
{
    SharedVertexList::Pointer m_Nodes;
    SharedTriList::Pointer m_Triangles;
    double* m_Areas;

  public:
    CalculateAreasImpl(SharedVertexList::Pointer nodes,
                       SharedTriList::Pointer triangles,
                       double* Areas) :
      m_Nodes(nodes),
      m_Triangles(triangles),
      m_Areas(Areas)
    {}
    virtual ~CalculateAreasImpl() {}

    void generate(size_t start, size_t end) const
    {
      int64_t* triangles = m_Triangles->getPointer(0);
      int64_t nIdx0 = 0, nIdx1 = 0, nIdx2 = 0;
      float vecA[3] = { 0.0f, 0.0f, 0.0f };
      float vecB[3] = { 0.0f, 0.0f, 0.0f };
      float cross[3] = { 0.0f, 0.0f, 0.0f };
      for (size_t i = start; i < end; i++)
      {
        nIdx0 = triangles[i * 3];
        nIdx1 = triangles[i * 3 + 1];
        nIdx2 = triangles[i * 3 + 2];
        float* A = m_Nodes->getPointer(nIdx0 * 3);
        float* B = m_Nodes->getPointer(nIdx1 * 3);
        float* C = m_Nodes->getPointer(nIdx2 * 3);

        MatrixMath::Subtract3x1s(A, B, vecA);
        MatrixMath::Subtract3x1s(A, C, vecB);
        MatrixMath::CrossProduct(vecA, vecB, cross);
        float area = 0.5f * MatrixMath::Magnitude3x1(cross);
        m_Areas[i] = area;
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
#include "moc_TriangleAreaFilter.cpp"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TriangleAreaFilter::TriangleAreaFilter() :
  SurfaceMeshFilter(),
  m_SurfaceMeshTriangleAreasArrayPath(DREAM3D::Defaults::TriangleDataContainerName, DREAM3D::Defaults::FaceAttributeMatrixName, DREAM3D::FaceData::SurfaceMeshFaceAreas),
  m_SurfaceMeshTriangleAreas(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TriangleAreaFilter::~TriangleAreaFilter()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TriangleAreaFilter::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(SeparatorFilterParameter::New("Face Data", FilterParameter::CreatedArray));
  {
    DataArrayCreationFilterParameter::RequirementType req = DataArrayCreationFilterParameter::CreateRequirement(DREAM3D::AttributeMatrixType::Face, DREAM3D::GeometryType::TriangleGeometry);
    parameters.push_back(DataArrayCreationFilterParameter::New("Face Areas", "SurfaceMeshTriangleAreasArrayPath", getSurfaceMeshTriangleAreasArrayPath(), FilterParameter::CreatedArray, req));
  }
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TriangleAreaFilter::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setSurfaceMeshTriangleAreasArrayPath(reader->readDataArrayPath("SurfaceMeshTriangleAreasArrayPath", getSurfaceMeshTriangleAreasArrayPath()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int TriangleAreaFilter::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  SIMPL_FILTER_WRITE_PARAMETER(FilterVersion)
  SIMPL_FILTER_WRITE_PARAMETER(SurfaceMeshTriangleAreasArrayPath)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TriangleAreaFilter::dataCheck()
{
  setErrorCondition(0);

  TriangleGeom::Pointer triangles = getDataContainerArray()->getPrereqGeometryFromDataContainer<TriangleGeom, AbstractFilter>(this, getSurfaceMeshTriangleAreasArrayPath().getDataContainerName());

  QVector<IDataArray::Pointer> dataArrays;

  if(getErrorCondition() >= 0) { dataArrays.push_back(triangles->getTriangles()); }

  QVector<size_t> cDims(1, 1);
  m_SurfaceMeshTriangleAreasPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<double>, AbstractFilter, double>(this, getSurfaceMeshTriangleAreasArrayPath(), 0, cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_SurfaceMeshTriangleAreasPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_SurfaceMeshTriangleAreas = m_SurfaceMeshTriangleAreasPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCondition() >= 0) { dataArrays.push_back(m_SurfaceMeshTriangleAreasPtr.lock()); }

  getDataContainerArray()->validateNumberOfTuples<AbstractFilter>(this, dataArrays);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TriangleAreaFilter::preflight()
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
void TriangleAreaFilter::execute()
{
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  DataContainer::Pointer sm = getDataContainerArray()->getDataContainer(getSurfaceMeshTriangleAreasArrayPath().getDataContainerName());

#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
  tbb::task_scheduler_init init;
  bool doParallel = true;
#endif

  TriangleGeom::Pointer triangleGeom = sm->getGeometryAs<TriangleGeom>();

#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
  if (doParallel == true)
  {
    tbb::parallel_for(tbb::blocked_range<size_t>(0, triangleGeom->getNumberOfTris()),
                      CalculateAreasImpl(triangleGeom->getVertices(), triangleGeom->getTriangles(), m_SurfaceMeshTriangleAreas), tbb::auto_partitioner());

  }
  else
#endif
  {
    CalculateAreasImpl serial(triangleGeom->getVertices(), triangleGeom->getTriangles(), m_SurfaceMeshTriangleAreas);
    serial.generate(0, triangleGeom->getNumberOfTris());
  }

  /* Let the GUI know we are done with this filter */
  notifyStatusMessage(getHumanLabel(), "Complete");
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer TriangleAreaFilter::newFilterInstance(bool copyFilterParameters)
{
  TriangleAreaFilter::Pointer filter = TriangleAreaFilter::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString TriangleAreaFilter::getCompiledLibraryName()
{ return SurfaceMeshingConstants::SurfaceMeshingBaseName; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString TriangleAreaFilter::getGroupName()
{ return DREAM3D::FilterGroups::SurfaceMeshingFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString TriangleAreaFilter::getSubGroupName()
{ return DREAM3D::FilterSubGroups::MiscFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString TriangleAreaFilter::getHumanLabel()
{ return "Generate Triangle Areas"; }
