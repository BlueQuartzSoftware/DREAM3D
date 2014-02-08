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
    VertexArray::Pointer m_Nodes;
    FaceArray::Pointer m_Triangles;
    double* m_Normals;

  public:
    CalculateNormalsImpl(VertexArray::Pointer nodes,
                         FaceArray::Pointer triangles,
                         double* normals) :
      m_Nodes(nodes),
      m_Triangles(triangles),
      m_Normals(normals)
    {}
    virtual ~CalculateNormalsImpl() {}

    /**
     * @brief generate Generates the Normals for the triangles
     * @param start The starting FaceArray::Face_t Index
     * @param end The ending FaceArray::Face_t Index
     */
    void generate(size_t start, size_t end) const
    {
      VertexArray::Vert_t* nodes = m_Nodes->getPointer(0);
      FaceArray::Face_t* triangles = m_Triangles->getPointer(0);
      for (size_t i = start; i < end; i++)
      {
        // Get the true indices of the 3 nodes
        VectorType normal = TriangleOps::computeNormal(nodes[triangles[i].verts[0]], nodes[triangles[i].verts[1]], nodes[triangles[i].verts[2]]);
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
  m_SurfaceDataContainerName(DREAM3D::Defaults::SurfaceDataContainerName),
  m_FaceAttributeMatrixName(DREAM3D::Defaults::FaceAttributeMatrixName),
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
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TriangleNormalFilter::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  /* Code to read the values goes between these statements */
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE BEGIN*/
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE END*/
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int TriangleNormalFilter::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  /* Place code that will write the inputs values into a file. reference the
   AbstractFilterParametersWriter class for the proper API to use. */
  /*  writer->writeValue("OutputFile", getOutputFile() ); */
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TriangleNormalFilter::dataCheck()
{
  SurfaceDataContainer* sm = getDataContainerArray()->getPrereqDataContainer<SurfaceDataContainer, AbstractFilter>(this, getSurfaceDataContainerName(), false);
  if(getErrorCondition() < 0) { return; }
  AttributeMatrix::Pointer faceAttrMat = sm->getPrereqAttributeMatrix<AbstractFilter>(this, getFaceAttributeMatrixName(), -300);
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
    QVector<size_t> dims(1, 3);
    m_SurfaceMeshTriangleNormalsPtr = faceAttrMat->createNonPrereqArray<DataArray<double>, AbstractFilter, double>(this, m_SurfaceMeshTriangleNormalsArrayName, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if( NULL != m_SurfaceMeshTriangleNormalsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    { m_SurfaceMeshTriangleNormals = m_SurfaceMeshTriangleNormalsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TriangleNormalFilter::preflight()
{
  emit preflightAboutToExecute();
  emit updateFilterParameters(this);
  dataCheck();
  emit preflightExecuted();
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

  SurfaceDataContainer* sm = getDataContainerArray()->getDataContainerAs<SurfaceDataContainer>(getSurfaceDataContainerName());
  notifyStatusMessage(getHumanLabel(), "Starting");

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
  bool doParallel = true;
#endif

  VertexArray::Pointer nodesPtr = sm->getVertices();

  FaceArray::Pointer trianglesPtr = sm->getFaces();
  size_t numTriangles = trianglesPtr->getNumberOfTuples();

  // Run the data check to allocate the memory for the centroid array
  dataCheck();

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
  if (doParallel == true)
  {
    tbb::parallel_for(tbb::blocked_range<size_t>(0, numTriangles),
                      CalculateNormalsImpl(nodesPtr, trianglesPtr, m_SurfaceMeshTriangleNormals), tbb::auto_partitioner());

  }
  else
#endif
  {
    CalculateNormalsImpl serial(nodesPtr, trianglesPtr, m_SurfaceMeshTriangleNormals);
    serial.generate(0, numTriangles);
  }


  /* Let the GUI know we are done with this filter */
  notifyStatusMessage(getHumanLabel(), "Complete");
}
