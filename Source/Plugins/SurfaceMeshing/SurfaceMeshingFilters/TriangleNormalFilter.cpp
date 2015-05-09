/* ============================================================================
 * Copyright (c) 2012 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2012 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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
#include "TriangleNormalFilter.h"

#include "DREAM3DLib/FilterParameters/AbstractFilterParametersReader.h"
#include "DREAM3DLib/FilterParameters/AbstractFilterParametersWriter.h"
#include "DREAM3DLib/Math/DREAM3DMath.h"
#include "DREAM3DLib/Math/MatrixMath.h"
#include "SurfaceMeshing/SurfaceMeshingFilters/util/Vector3.h"
#include "SurfaceMeshing/SurfaceMeshingFilters/util/TriangleOps.h"

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
#include <tbb/parallel_for.h>
#include <tbb/blocked_range.h>
#include <tbb/partitioner.h>
#endif


/**
 * @brief The CalculateNormalsImpl class is the actual code that does the computation and can be called either
 * from serial code or from Parallelized code (using TBB).
 */
class CalculateNormalsImpl
{
    SharedVertexList::Pointer m_Nodes;
    SharedTriList::Pointer m_Triangles;
    double* m_Normals;

  public:
    CalculateNormalsImpl(SharedVertexList::Pointer nodes,
                         SharedTriList::Pointer triangles,
                         double* normals) :
      m_Nodes(nodes),
      m_Triangles(triangles),
      m_Normals(normals)
    {}
    virtual ~CalculateNormalsImpl() {}

    /**
     * @brief generate Generates the Normals for the triangles
     * @param start The starting Index
     * @param end The ending Index
     */
    void generate(size_t start, size_t end) const
    {
      float* nodes = m_Nodes->getPointer(0);
      int64_t* triangles = m_Triangles->getPointer(0);
      int64_t nIdx0 = 0, nIdx1 = 0, nIdx2 = 0;
      for (size_t i = start; i < end; i++)
      {
        nIdx0 = triangles[i*3] * 3;
        nIdx1 = triangles[i*3+1] * 3;
        nIdx2 = triangles[i*3+2] * 3;
        float* n0 = &(nodes[nIdx0]);
        float* n1 = &(nodes[nIdx1]);
        float* n2 = &(nodes[nIdx2]);

        VectorType normal = TriangleOps::computeNormal(n0, n1, n2);
        m_Normals[i * 3 + 0] = normal.x;
        m_Normals[i * 3 + 1] = normal.y;
        m_Normals[i * 3 + 2] = normal.z;
      }
    }

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
    /**
     * @brief operator () This is called from the TBB stye of code
     * @param r The range to compute the values
     */
    void operator()(const tbb::blocked_range<size_t>& r) const
    {
      generate(r.begin(), r.end());
    }
#endif


};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TriangleNormalFilter::TriangleNormalFilter() :
  SurfaceMeshFilter(),
  m_FaceAttributeMatrixName(DREAM3D::Defaults::DataContainerName, DREAM3D::Defaults::FaceAttributeMatrixName, ""),
  m_SurfaceMeshTriangleNormalsArrayName(DREAM3D::FaceData::SurfaceMeshFaceNormals),
  m_SurfaceMeshTriangleNormals(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TriangleNormalFilter::~TriangleNormalFilter()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TriangleNormalFilter::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(FilterParameter::New("Required Information", "", FilterParameterWidgetType::SeparatorWidget, "", true));
  parameters.push_back(FilterParameter::New("Face Attribute Matrix Name", "FaceAttributeMatrixName", FilterParameterWidgetType::AttributeMatrixSelectionWidget, getFaceAttributeMatrixName(), true, ""));
  parameters.push_back(FilterParameter::New("Created Information", "", FilterParameterWidgetType::SeparatorWidget, "", true));
  parameters.push_back(FilterParameter::New("SurfaceMeshTriangleNormals", "SurfaceMeshTriangleNormalsArrayName", FilterParameterWidgetType::StringWidget, getSurfaceMeshTriangleNormalsArrayName(), true, ""));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TriangleNormalFilter::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setFaceAttributeMatrixName(reader->readDataArrayPath("FaceAttributeMatrixName", getFaceAttributeMatrixName() ) );
  setSurfaceMeshTriangleNormalsArrayName(reader->readString("SurfaceMeshTriangleNormalsArrayName", getSurfaceMeshTriangleNormalsArrayName() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int TriangleNormalFilter::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  DREAM3D_FILTER_WRITE_PARAMETER(FilterVersion)
  DREAM3D_FILTER_WRITE_PARAMETER(FaceAttributeMatrixName)
  DREAM3D_FILTER_WRITE_PARAMETER(SurfaceMeshTriangleNormalsArrayName)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TriangleNormalFilter::dataCheck()
{
  setErrorCondition(0);

  DataArrayPath tempPath;
  DataContainer::Pointer sm = getDataContainerArray()->getPrereqDataContainer<AbstractFilter>(this, getFaceAttributeMatrixName().getDataContainerName(), false);
  if(getErrorCondition() < 0) { return; }

  TriangleGeom::Pointer triangles = sm->getPrereqGeometry<TriangleGeom, AbstractFilter>(this);
  if(getErrorCondition() < 0) { return; }

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
    QVector<size_t> dims(1, 3);
    tempPath.update(getFaceAttributeMatrixName().getDataContainerName(), getFaceAttributeMatrixName().getAttributeMatrixName(), getSurfaceMeshTriangleNormalsArrayName() );
    m_SurfaceMeshTriangleNormalsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<double>, AbstractFilter, double>(this, tempPath, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if( NULL != m_SurfaceMeshTriangleNormalsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    { m_SurfaceMeshTriangleNormals = m_SurfaceMeshTriangleNormalsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TriangleNormalFilter::preflight()
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
void TriangleNormalFilter::execute()
{
  int err = 0;
  setErrorCondition(err);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  DataContainer::Pointer sm = getDataContainerArray()->getDataContainer(getFaceAttributeMatrixName().getDataContainerName());
  notifyStatusMessage(getMessagePrefix(), getHumanLabel(), "Starting");

  // No check because datacheck() made sure we can do the next line.
  TriangleGeom::Pointer triangleGeom = sm->getGeometryAs<TriangleGeom>();

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
  bool doParallel = true;
#endif


#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
  if (doParallel == true)
  {
    tbb::parallel_for(tbb::blocked_range<size_t>(0, triangleGeom->getNumberOfTris()),
                      CalculateNormalsImpl(triangleGeom->getVertices(), triangleGeom->getTriangles(), m_SurfaceMeshTriangleNormals), tbb::auto_partitioner());

  }
  else
#endif
  {
    CalculateNormalsImpl serial(triangleGeom->getVertices(), triangleGeom->getTriangles(), m_SurfaceMeshTriangleNormals);
    serial.generate(0, triangleGeom->getNumberOfTris());
  }

  /* Let the GUI know we are done with this filter */
  notifyStatusMessage(getHumanLabel(), "Complete");
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer TriangleNormalFilter::newFilterInstance(bool copyFilterParameters)
{
  TriangleNormalFilter::Pointer filter = TriangleNormalFilter::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString TriangleNormalFilter::getCompiledLibraryName()
{ return SurfaceMeshingConstants::SurfaceMeshingBaseName; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString TriangleNormalFilter::getGroupName()
{ return DREAM3D::FilterGroups::SurfaceMeshingFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString TriangleNormalFilter::getSubGroupName()
{ return DREAM3D::FilterSubGroups::MiscFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString TriangleNormalFilter::getHumanLabel()
{ return "Generate Triangle Normals Filter"; }

