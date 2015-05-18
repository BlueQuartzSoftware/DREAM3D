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


#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
#include <tbb/parallel_for.h>
#include <tbb/blocked_range.h>
#include <tbb/partitioner.h>
#include <tbb/task_scheduler_init.h>
#endif

#include "DREAM3DLib/FilterParameters/AbstractFilterParametersReader.h"
#include "DREAM3DLib/FilterParameters/AbstractFilterParametersWriter.h"

#include "DREAM3DLib/Math/DREAM3DMath.h"
#include "DREAM3DLib/Math/MatrixMath.h"

#define SQR(value) (value)*(value)

/**
 * @brief The CalculateAreasImpl class
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
      float vecA[3];
      float vecB[3];
      float cross[3];
      for (size_t i = start; i < end; i++)
      {
        nIdx0 = triangles[i*3];
        nIdx1 = triangles[i*3+1];
        nIdx2 = triangles[i*3+2];
        float* A = m_Nodes->getPointer(nIdx0 * 3);
        float* B = m_Nodes->getPointer(nIdx1 * 3);
        float* C = m_Nodes->getPointer(nIdx2 * 3);

        MatrixMath::Subtract3x1s(A, B, vecA);
        MatrixMath::Subtract3x1s(A, C, vecB);
        MatrixMath::CrossProduct(vecA, vecB, cross);
        float area = 0.5 * MatrixMath::Magnitude3x1(cross);
        m_Areas[i] = area;
      }
    }

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
    void operator()(const tbb::blocked_range<size_t>& r) const
    {
      generate(r.begin(), r.end());
    }
#endif


};


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TriangleAreaFilter::TriangleAreaFilter() :
  SurfaceMeshFilter(),
  m_FaceAttributeMatrixName(DREAM3D::Defaults::DataContainerName, DREAM3D::Defaults::FaceAttributeMatrixName, ""),
  m_SurfaceMeshTriangleAreasArrayName(DREAM3D::FaceData::SurfaceMeshFaceAreas),
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
  parameters.push_back(FilterParameter::New("Required Information", "", FilterParameterWidgetType::SeparatorWidget, "", true));
  parameters.push_back(FilterParameter::New("Face Attribute Matrix Name", "FaceAttributeMatrixName", FilterParameterWidgetType::AttributeMatrixSelectionWidget, getFaceAttributeMatrixName(), true, ""));
  parameters.push_back(FilterParameter::New("Created Information", "", FilterParameterWidgetType::SeparatorWidget, "", true));
  parameters.push_back(FilterParameter::New("SurfaceMeshTriangleAreas", "SurfaceMeshTriangleAreasArrayName", FilterParameterWidgetType::StringWidget, getSurfaceMeshTriangleAreasArrayName(), true, ""));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TriangleAreaFilter::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setFaceAttributeMatrixName(reader->readDataArrayPath("FaceAttributeMatrixName", getFaceAttributeMatrixName() ) );
  setSurfaceMeshTriangleAreasArrayName(reader->readString("SurfaceMeshTriangleAreasArrayName", getSurfaceMeshTriangleAreasArrayName() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int TriangleAreaFilter::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  DREAM3D_FILTER_WRITE_PARAMETER(FilterVersion)
  DREAM3D_FILTER_WRITE_PARAMETER(FaceAttributeMatrixName)
  DREAM3D_FILTER_WRITE_PARAMETER(SurfaceMeshTriangleAreasArrayName)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TriangleAreaFilter::dataCheck()
{
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
    QVector<size_t> dims(1, 1);
    tempPath.update(getFaceAttributeMatrixName().getDataContainerName(), getFaceAttributeMatrixName().getAttributeMatrixName(), getSurfaceMeshTriangleAreasArrayName() );
    m_SurfaceMeshTriangleAreasPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<double>, AbstractFilter, double>(this, tempPath, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if( NULL != m_SurfaceMeshTriangleAreasPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    { m_SurfaceMeshTriangleAreas = m_SurfaceMeshTriangleAreasPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  }
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
  int err = 0;
  setErrorCondition(err);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  DataContainer::Pointer sm = getDataContainerArray()->getDataContainer(getFaceAttributeMatrixName().getDataContainerName());
  notifyStatusMessage(getMessagePrefix(), getHumanLabel(), "Starting");

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
  tbb::task_scheduler_init init;
  bool doParallel = true;
#endif

  // No check because datacheck() made sure we can do the next line.
  TriangleGeom::Pointer triangleGeom = sm->getGeometryAs<TriangleGeom>();

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
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

