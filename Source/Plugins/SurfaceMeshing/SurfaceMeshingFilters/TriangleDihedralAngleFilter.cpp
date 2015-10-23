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


#include "TriangleDihedralAngleFilter.h"

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
#include "SIMPLib/Math/SIMPLibMath.h"

#include "SurfaceMeshing/SurfaceMeshingConstants.h"

/**
 * @brief The CalculateDihedralAnglesImpl class implements a threaded algorithm that computes the minimum dihedral angle
 * for each triangle in a set of triangles
 */
class CalculateDihedralAnglesImpl
{
    SharedVertexList::Pointer m_Nodes;
    SharedTriList::Pointer m_Triangles;
    double* m_DihedralAngles;

  public:
    CalculateDihedralAnglesImpl(SharedVertexList::Pointer nodes,
                                SharedTriList::Pointer triangles,
                                double* DihedralAngles) :
      m_Nodes(nodes),
      m_Triangles(triangles),
      m_DihedralAngles(DihedralAngles)
    {}
    virtual ~CalculateDihedralAnglesImpl() {}

    void generate(size_t start, size_t end) const
    {
      float* nodes = m_Nodes->getPointer(0);
      int64_t* triangles = m_Triangles->getPointer(0);

      float radToDeg = 180.0f / SIMPLib::Constants::k_Pi;

      float ABx = 0.0f, ABy = 0.0f, ABz = 0.0f, ACx = 0.0f, ACy = 0.0f, ACz = 0.0f, BCx = 0.0f, BCy = 0.0f, BCz = 0.0f;
      float magAB = 0.0f, magAC = 0.0f, magBC = 0.0f;
      float dihedralAngle1 = 0.0f, dihedralAngle2 = 0.0f, dihedralAngle3 = 0.0f, minDihedralAngle = 0.0f;
      for (size_t i = start; i < end; i++)
      {
        minDihedralAngle = 180.0f;

        ABx = nodes[triangles[i * 3] * 3 + 0] - nodes[triangles[i * 3 + 1] * 3 + 0];
        ABy = nodes[triangles[i * 3] * 3 + 1] - nodes[triangles[i * 3 + 1] * 3 + 1];
        ABz = nodes[triangles[i * 3] * 3 + 2] - nodes[triangles[i * 3 + 1] * 3 + 2];
        magAB = sqrtf(ABx * ABx + ABy * ABy + ABz * ABz);

        ACx = nodes[triangles[i * 3] * 3 + 0] - nodes[triangles[i * 3 + 2] * 3 + 0];
        ACy = nodes[triangles[i * 3] * 3 + 1] - nodes[triangles[i * 3 + 2] * 3 + 1];
        ACz = nodes[triangles[i * 3] * 3 + 2] - nodes[triangles[i * 3 + 2] * 3 + 2];
        magAC = sqrtf(ACx * ACx + ACy * ACy + ACz * ACz);

        BCx = nodes[triangles[i * 3 + 1] * 3 + 0] - nodes[triangles[i * 3 + 2] * 3 + 0];
        BCy = nodes[triangles[i * 3 + 1] * 3 + 1] - nodes[triangles[i * 3 + 2] * 3 + 1];
        BCz = nodes[triangles[i * 3 + 1] * 3 + 2] - nodes[triangles[i * 3 + 2] * 3 + 2];
        magBC = sqrtf(BCx * BCx + BCy * BCy + BCz * BCz);

        dihedralAngle1 = radToDeg * acos(((ABx * ACx) + (ABy * ACy) + (ABz * ACz)) / (magAB * magAC));
        // 180 - angle because AB points out of vertex and BC points into vertex, so angle is actually angle outside of triangle
        dihedralAngle2 = 180.0f - (radToDeg * acos(((ABx * BCx) + (ABy * BCy) + (ABz * BCz)) / (magAB * magBC)));
        dihedralAngle3 = radToDeg * acos(((BCx * ACx) + (BCy * ACy) + (BCz * ACz)) / (magBC * magAC));
        minDihedralAngle = dihedralAngle1;
        if (dihedralAngle2 < minDihedralAngle) { minDihedralAngle = dihedralAngle2; }
        if (dihedralAngle3 < minDihedralAngle) { minDihedralAngle = dihedralAngle3; }
        m_DihedralAngles[i]  = minDihedralAngle;
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
#include "moc_TriangleDihedralAngleFilter.cpp"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TriangleDihedralAngleFilter::TriangleDihedralAngleFilter() :
  SurfaceMeshFilter(),
  m_SurfaceMeshTriangleDihedralAnglesArrayPath(DREAM3D::Defaults::TriangleDataContainerName, DREAM3D::Defaults::FaceAttributeMatrixName, DREAM3D::FaceData::SurfaceMeshFaceDihedralAngles),
  m_SurfaceMeshTriangleDihedralAngles(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TriangleDihedralAngleFilter::~TriangleDihedralAngleFilter()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TriangleDihedralAngleFilter::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(SeparatorFilterParameter::New("Face Data", FilterParameter::CreatedArray));
  {
    DataArrayCreationFilterParameter::RequirementType req = DataArrayCreationFilterParameter::CreateRequirement(DREAM3D::AttributeMatrixType::Face, DREAM3D::GeometryType::TriangleGeometry);
    parameters.push_back(DataArrayCreationFilterParameter::New("Face Dihedral Angles", "SurfaceMeshTriangleDihedralAnglesArrayPath", getSurfaceMeshTriangleDihedralAnglesArrayPath(), FilterParameter::CreatedArray, req));
  }
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TriangleDihedralAngleFilter::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setSurfaceMeshTriangleDihedralAnglesArrayPath(reader->readDataArrayPath("SurfaceMeshTriangleDihedralAnglesArrayPath", getSurfaceMeshTriangleDihedralAnglesArrayPath()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int TriangleDihedralAngleFilter::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  SIMPL_FILTER_WRITE_PARAMETER(FilterVersion)
  SIMPL_FILTER_WRITE_PARAMETER(SurfaceMeshTriangleDihedralAnglesArrayPath)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TriangleDihedralAngleFilter::dataCheck()
{
  setErrorCondition(0);

  TriangleGeom::Pointer triangles = getDataContainerArray()->getPrereqGeometryFromDataContainer<TriangleGeom, AbstractFilter>(this, getSurfaceMeshTriangleDihedralAnglesArrayPath().getDataContainerName());

  QVector<IDataArray::Pointer> dataArrays;

  if(getErrorCondition() >= 0) { dataArrays.push_back(triangles->getTriangles()); }

  QVector<size_t> cDims(1, 1);
  m_SurfaceMeshTriangleDihedralAnglesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<double>, AbstractFilter, double>(this, getSurfaceMeshTriangleDihedralAnglesArrayPath(), 0, cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_SurfaceMeshTriangleDihedralAnglesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_SurfaceMeshTriangleDihedralAngles = m_SurfaceMeshTriangleDihedralAnglesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCondition() >= 0) { dataArrays.push_back(m_SurfaceMeshTriangleDihedralAnglesPtr.lock()); }

  getDataContainerArray()->validateNumberOfTuples<AbstractFilter>(this, dataArrays);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TriangleDihedralAngleFilter::preflight()
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
void TriangleDihedralAngleFilter::execute()
{
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  DataContainer::Pointer sm = getDataContainerArray()->getDataContainer(getSurfaceMeshTriangleDihedralAnglesArrayPath().getDataContainerName());

#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
  tbb::task_scheduler_init init;
  bool doParallel = true;
#endif

  TriangleGeom::Pointer triangleGeom = sm->getGeometryAs<TriangleGeom>();

#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
  if (doParallel == true)
  {
    tbb::parallel_for(tbb::blocked_range<size_t>(0, triangleGeom->getNumberOfTris()),
                      CalculateDihedralAnglesImpl(triangleGeom->getVertices(), triangleGeom->getTriangles(), m_SurfaceMeshTriangleDihedralAngles), tbb::auto_partitioner());
  }
  else
#endif
  {
    CalculateDihedralAnglesImpl serial(triangleGeom->getVertices(), triangleGeom->getTriangles(), m_SurfaceMeshTriangleDihedralAngles);
    serial.generate(0, triangleGeom->getNumberOfTris());
  }

  /* Let the GUI know we are done with this filter */
  notifyStatusMessage(getHumanLabel(), "Complete");
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer TriangleDihedralAngleFilter::newFilterInstance(bool copyFilterParameters)
{
  TriangleDihedralAngleFilter::Pointer filter = TriangleDihedralAngleFilter::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString TriangleDihedralAngleFilter::getCompiledLibraryName()
{ return SurfaceMeshingConstants::SurfaceMeshingBaseName; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString TriangleDihedralAngleFilter::getGroupName()
{ return DREAM3D::FilterGroups::SurfaceMeshingFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString TriangleDihedralAngleFilter::getSubGroupName()
{ return DREAM3D::FilterSubGroups::MiscFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString TriangleDihedralAngleFilter::getHumanLabel()
{ return "Find Minimum Triangle Dihedral Angle"; }
