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

#include "DREAM3DLib/Common/DREAM3DMath.h"
#include "DREAM3DLib/Common/MatrixMath.h"
#include "DREAM3DLib/SurfaceMeshingFilters/util/Vector3.h"
#include "DREAM3DLib/SurfaceMeshingFilters/util/TriangleOps.h"

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
    DREAM3D::SurfaceMesh::VertListPointer_t m_Nodes;
    DREAM3D::SurfaceMesh::FaceListPointer_t m_Triangles;
    double* m_Normals;

  public:
    CalculateNormalsImpl(DREAM3D::SurfaceMesh::VertListPointer_t nodes,
                         DREAM3D::SurfaceMesh::FaceListPointer_t triangles,
                         double* normals) :
      m_Nodes(nodes),
      m_Triangles(triangles),
      m_Normals(normals)
    {}
    virtual ~CalculateNormalsImpl(){}

    /**
     * @brief generate Generates the Normals for the triangles
     * @param start The starting DREAM3D::SurfaceMesh::Face_t Index
     * @param end The ending DREAM3D::SurfaceMesh::Face_t Index
     */
    void generate(size_t start, size_t end) const
    {
      DREAM3D::SurfaceMesh::Vert_t* nodes = m_Nodes->GetPointer(0);
      DREAM3D::SurfaceMesh::Face_t* triangles = m_Triangles->GetPointer(0);
      for (size_t i = start; i < end; i++)
      {
        // Get the true indices of the 3 nodes
        VectorType normal = TriangleOps::computeNormal(nodes[triangles[i].verts[0]], nodes[triangles[i].verts[1]], nodes[triangles[i].verts[2]]);
        m_Normals[i*3+0] = normal.x;
        m_Normals[i*3+1] = normal.y;
        m_Normals[i*3+2] = normal.z;
      }
    }

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
    /**
     * @brief operator () This is called from the TBB stye of code
     * @param r The range to compute the values
     */
    void operator()(const tbb::blocked_range<size_t> &r) const
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
  std::vector<FilterParameter::Pointer> parameters;
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TriangleNormalFilter::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  /* Code to read the values goes between these statements */
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
void TriangleNormalFilter::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;
  SurfaceMeshDataContainer* sm = getSurfaceMeshDataContainer();
  if(NULL == sm)
  {
    setErrorCondition(-383);
    addErrorMessage(getHumanLabel(), "SurfaceMeshDataContainer is missing", getErrorCondition());
  }
  else
  {
    // We MUST have Nodes
    if(sm->getVertices().get() == NULL)
    {
      setErrorCondition(-384);
      addErrorMessage(getHumanLabel(), "SurfaceMesh DataContainer missing Nodes", getErrorCondition());
    }

    // We MUST have Triangles defined also.
    if(sm->getFaces().get() == NULL)
    {
      setErrorCondition(-385);
      addErrorMessage(getHumanLabel(), "SurfaceMesh DataContainer missing Triangles", getErrorCondition());
    }
    else
    {
      CREATE_NON_PREREQ_DATA(sm, DREAM3D, FaceData, SurfaceMeshTriangleNormals, ss, double, DoubleArrayType, 0, voxels, 3)
    }
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TriangleNormalFilter::preflight()
{
  /* Place code here that sanity checks input arrays and input values. Look at some
  * of the other DREAM3DLib/Filters/.cpp files for sample codes */
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TriangleNormalFilter::execute()
{
  int err = 0;
  std::stringstream ss;
  setErrorCondition(err);
  SurfaceMeshDataContainer* m = getSurfaceMeshDataContainer();
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The SurfaceMesh DataContainer Object was NULL", -999);
    return;
  }
  setErrorCondition(0);
  notifyStatusMessage("Starting");

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
  bool doParallel = true;
#endif

  DREAM3D::SurfaceMesh::VertListPointer_t nodesPtr = getSurfaceMeshDataContainer()->getVertices();

  DREAM3D::SurfaceMesh::FaceListPointer_t trianglesPtr = getSurfaceMeshDataContainer()->getFaces();
  size_t totalPoints = trianglesPtr->GetNumberOfTuples();

  // Run the data check to allocate the memory for the centroid array
  dataCheck(false, trianglesPtr->GetNumberOfTuples(), 0, 0);

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
  if (doParallel == true)
  {
    tbb::parallel_for(tbb::blocked_range<size_t>(0, totalPoints),
                      CalculateNormalsImpl(nodesPtr, trianglesPtr, m_SurfaceMeshTriangleNormals), tbb::auto_partitioner());

  }
  else
#endif
  {
    CalculateNormalsImpl serial(nodesPtr, trianglesPtr, m_SurfaceMeshTriangleNormals);
    serial.generate(0, totalPoints);
  }


  /* Let the GUI know we are done with this filter */
  notifyStatusMessage("Complete");
}
