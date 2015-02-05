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
#include "TriangleAreaFilter.h"


#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
#include <tbb/parallel_for.h>
#include <tbb/blocked_range.h>
#include <tbb/partitioner.h>
#include <tbb/task_scheduler_init.h>
#endif

#include "DREAM3DLib/Math/DREAM3DMath.h"

/**
 * @brief The CalculateAreasImpl class
 */
class CalculateAreasImpl
{
    VertexArray::Pointer m_Nodes;
    FaceArray::Pointer m_Triangles;
    double* m_Areas;

  public:
    CalculateAreasImpl(VertexArray::Pointer nodes, FaceArray::Pointer triangles, double* Areas) :
      m_Nodes(nodes),
      m_Triangles(triangles),
      m_Areas(Areas)
    {}
    virtual ~CalculateAreasImpl() {}

    void generate(size_t start, size_t end) const
    {

      VertexArray::Vert_t* nodes = m_Nodes->getPointer(0);
      FaceArray::Face_t* triangles = m_Triangles->getPointer(0);

      float ABx, ABy, ABz, ACx, ACy, ACz;
      for (size_t i = start; i < end; i++)
      {
        ABx = nodes[triangles[i].verts[0]].pos[0] - nodes[triangles[i].verts[1]].pos[0];
        ABy = nodes[triangles[i].verts[0]].pos[1] - nodes[triangles[i].verts[1]].pos[1];
        ABz = nodes[triangles[i].verts[0]].pos[2] - nodes[triangles[i].verts[1]].pos[2];
        ACx = nodes[triangles[i].verts[0]].pos[0] - nodes[triangles[i].verts[2]].pos[0];
        ACy = nodes[triangles[i].verts[0]].pos[1] - nodes[triangles[i].verts[2]].pos[1];
        ACz = nodes[triangles[i].verts[0]].pos[2] - nodes[triangles[i].verts[2]].pos[2];
        m_Areas[i]  = 0.5 * sqrt(((ABy * ACz - ABz * ACy) * (ABy * ACz - ABz * ACy)) + ((ABz * ACx - ABx * ACz) * (ABz * ACx - ABx * ACz)) + ((ABx * ACy - ABy * ACx) * (ABx * ACy - ABy * ACx)));
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
  m_FaceAttributeMatrixName(DREAM3D::Defaults::SurfaceDataContainerName, DREAM3D::Defaults::FaceAttributeMatrixName, ""),
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
  SurfaceDataContainer* sm = getDataContainerArray()->getPrereqDataContainer<SurfaceDataContainer, AbstractFilter>(this, getFaceAttributeMatrixName().getDataContainerName(), false);
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

  SurfaceDataContainer* sm = getDataContainerArray()->getDataContainerAs<SurfaceDataContainer>(getFaceAttributeMatrixName().getDataContainerName());
  notifyStatusMessage(getMessagePrefix(), getHumanLabel(), "Starting");

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
  tbb::task_scheduler_init init;
  bool doParallel = true;
#endif

  VertexArray::Pointer nodesPtr = sm->getVertices();

  FaceArray::Pointer trianglesPtr = sm->getFaces();
  size_t numTriangles = trianglesPtr->getNumberOfTuples();

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
  if (doParallel == true)
  {
    tbb::parallel_for(tbb::blocked_range<size_t>(0, numTriangles),
                      CalculateAreasImpl(nodesPtr, trianglesPtr, m_SurfaceMeshTriangleAreas), tbb::auto_partitioner());

  }
  else
#endif
  {
    CalculateAreasImpl serial(nodesPtr, trianglesPtr, m_SurfaceMeshTriangleAreas);
    serial.generate(0, numTriangles);
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
{ return SurfaceMeshing::SurfaceMeshingBaseName; }


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

