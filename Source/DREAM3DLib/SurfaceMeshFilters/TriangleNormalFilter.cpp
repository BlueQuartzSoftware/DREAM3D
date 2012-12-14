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
/*
 * Your License or Copyright Information can go here
 */

#include "TriangleNormalFilter.h"
#include "DREAM3DLib/Common/DREAM3DMath.h"

#if DREAM3D_USE_PARALLEL_ALGORITHMS
#include <tbb/parallel_for.h>
#include <tbb/blocked_range.h>
#include <tbb/partitioner.h>
#include <tbb/task_scheduler_init.h>
#endif

#include "DREAM3DLib/Common/DREAM3DMath.h"

/**
 * @brief The CalculateNormalsImpl class is the actual code that does the computation and can be called either
 * from serial code or from Parallelized code (using TBB).
 */
class CalculateNormalsImpl
{
    StructArray<Node>::Pointer m_Nodes;
    StructArray<Triangle>::Pointer m_Triangles;
    double* m_Normals;

  public:
    CalculateNormalsImpl(StructArray<Node>::Pointer nodes, StructArray<Triangle>::Pointer triangles, double* normals) :
      m_Nodes(nodes),
      m_Triangles(triangles),
      m_Normals(normals)
    {}
    virtual ~CalculateNormalsImpl(){}

    /**
     * @brief generate Generates the Normals for the triangles
     * @param start The starting Triangle Index
     * @param end The ending Triangle Index
     */
    void generate(size_t start, size_t end) const
    {
      Node* nodes = m_Nodes->GetPointer(0);
      Triangle* triangles = m_Triangles->GetPointer(0);
      double vert1[3];
      double vert2[3];
      double vert3[3];
      double u[3];
      double w[3];
      double normal[3];
      double length;

      for (size_t i = start; i < end; i++)
      {
        // Get the true indices of the 3 nodes
        int nId0 = triangles[i].node_id[0];
        int nId1 = triangles[i].node_id[1];
        int nId2 = triangles[i].node_id[2];

        vert1[0] = static_cast<float>(nodes[nId0].coord[0]);
        vert1[1] = static_cast<float>(nodes[nId0].coord[1]);
        vert1[2] = static_cast<float>(nodes[nId0].coord[2]);

        vert2[0] = static_cast<float>(nodes[nId1].coord[0]);
        vert2[1] = static_cast<float>(nodes[nId1].coord[1]);
        vert2[2] = static_cast<float>(nodes[nId1].coord[2]);

        vert3[0] = static_cast<float>(nodes[nId2].coord[0]);
        vert3[1] = static_cast<float>(nodes[nId2].coord[1]);
        vert3[2] = static_cast<float>(nodes[nId2].coord[2]);

        //
        // Compute the normal
        u[0] = vert2[0] - vert1[0];
        u[1] = vert2[1] - vert1[1];
        u[2] = vert2[2] - vert1[2];

        w[0] = vert3[0] - vert1[0];
        w[1] = vert3[1] - vert1[1];
        w[2] = vert3[2] - vert1[2];

        normal[0] = u[1] * w[2] - u[2] * w[1];
        normal[1] = u[2] * w[0] - u[0] * w[2];
        normal[2] = u[0] * w[1] - u[1] * w[0];

        length = sqrt(normal[0] * normal[0] + normal[1] * normal[1] + normal[2] * normal[2]);
        m_Normals[i*3] = normal[0] / length;
        m_Normals[i*3+1] = normal[1] / length;
        m_Normals[i*3+2] = normal[2] / length;

      }
    }

#if DREAM3D_USE_PARALLEL_ALGORITHMS
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
AbstractFilter(),
m_SurfaceMeshTriangleNormalsArrayName(DREAM3D::CellData::SurfaceMeshTriangleNormals),
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
void TriangleNormalFilter::writeFilterParameters(AbstractFilterParametersWriter* writer)
{
  /* Place code that will write the inputs values into a file. reference the
   AbstractFilterParametersWriter class for the proper API to use. */
  /*  writer->writeValue("OutputFile", getOutputFile() ); */
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
    addErrorMessage(getHumanLabel(), "SurfaceMeshDataContainer is missing", -383);
    setErrorCondition(-384);
  }
  else
  {
      // We MUST have Nodes
    if(sm->getNodes().get() == NULL)
    {
      addErrorMessage(getHumanLabel(), "SurfaceMesh DataContainer missing Nodes", -384);
      setErrorCondition(-384);
    }

    // We MUST have Triangles defined also.
    if(sm->getTriangles().get() == NULL)
    {
      addErrorMessage(getHumanLabel(), "SurfaceMesh DataContainer missing Triangles", -383);
      setErrorCondition(-384);
    }
    else
    {
      CREATE_NON_PREREQ_DATA(sm, DREAM3D, CellData, SurfaceMeshTriangleNormals, ss, double, DoubleArrayType, 0, voxels, 3)
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
  VoxelDataContainer* m = getVoxelDataContainer();
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The Voxel DataContainer Object was NULL", -999);
    return;
  }
  setErrorCondition(0);


  StructArray<Node>::Pointer nodesPtr = getSurfaceMeshDataContainer()->getNodes();

  StructArray<Triangle>::Pointer trianglesPtr = getSurfaceMeshDataContainer()->getTriangles();
  size_t totalPoints = trianglesPtr->GetNumberOfTuples();

  // Run the data check to allocate the memory for the centroid array
  dataCheck(false, trianglesPtr->GetNumberOfTuples(), 0, 0);

#if DREAM3D_USE_PARALLEL_ALGORITHMS
  tbb::parallel_for(tbb::blocked_range<size_t>(0, totalPoints),
                    CalculateNormalsImpl(nodesPtr, trianglesPtr, m_SurfaceMeshTriangleNormals), tbb::auto_partitioner());

#else
  CalculateNormalsImpl serial(nodesPtr, trianglesPtr, m_SurfaceMeshTriangleNormals);
  serial.generate(0, totalPoints);
#endif

  for (int i = 0; i < 10; ++i) {
    std::cout << i << "  Normal: " << m_SurfaceMeshTriangleNormals[i*3] << " " << m_SurfaceMeshTriangleNormals[i*3+ 1] << " " << m_SurfaceMeshTriangleNormals[i*3+2]<< std::endl;
  }


  /* Let the GUI know we are done with this filter */
  notifyStatusMessage("Complete");
}
