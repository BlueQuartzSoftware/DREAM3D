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

#include "QSAMCurvature.h"

#if DREAM3D_USE_PARALLEL_ALGORITHMS
#include <tbb/parallel_for.h>
#include <tbb/blocked_range.h>
#include <tbb/partitioner.h>
#include <tbb/task_scheduler_init.h>
#endif

#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_cross.h>

#include "DREAM3DLib/Common/DREAM3DMath.h"

/**
 * @brief The CalculateTriangleCentroidsImpl class calculates the centroids of each triangle in parallel if
 * the Intel Threading Building Blocks (TBB) library is found.
 */
class CalculateTriangleCentroidsImpl
{
    StructArray<Triangle>::Pointer m_TrianglesPtr;
    StructArray<Node>::Pointer m_NodesPtr;
    DataArray<double>::Pointer m_CentroidsPtr;

  public:
    CalculateTriangleCentroidsImpl(StructArray<Triangle>::Pointer triangles,
                                   StructArray<Node>::Pointer nodesPtr,
                                   DataArray<double>::Pointer centroids) :
      m_TrianglesPtr(triangles),
      m_NodesPtr(nodesPtr),
      m_CentroidsPtr(centroids)
    {}

    virtual ~CalculateTriangleCentroidsImpl(){}

    /**
     * @brief execute This is the actual method that gets called to calculate the centroids. Call
     * this method directly if you do NOT have TBB installed and available. If you <b>are</b> running
     * this class in serial then the start value is Zero (0) and the ending value is the (numTriangles - 1).
     * If this is being used in a Parallel situation then start and end will be set by the thread that
     * is running this method.
     * @param start The starting triangle
     * @param end The ending triangle
     */
    void execute(size_t start, size_t end) const
    {
      double* triloc = m_CentroidsPtr->GetPointer(0);
      Node* nodes = m_NodesPtr->GetPointer(0);
      double value = 0.0;
      for (size_t i = start; i < end; i++)
      {
        Triangle& tri = *(m_TrianglesPtr->GetPointer(start));
        Node& n0 = nodes[tri.node_id[0]];
        Node& n1 = nodes[tri.node_id[1]];
        Node& n2 = nodes[tri.node_id[2]];

        value = (n0.coord[0] + n1.coord[0] + n2.coord[0])/3.0;
        triloc[i*3] = value;

        value = (n0.coord[1] + n1.coord[1] + n2.coord[1])/3.0;
        triloc[i*3+1] = value;

        value = (n0.coord[2] + n1.coord[2] + n2.coord[2])/3.0;
        triloc[i*3+2] = value;
      }
    }

#if DREAM3D_USE_PARALLEL_ALGORITHMS
    void operator()(const tbb::blocked_range<size_t> &r) const
    {
      execute(r.begin(), r.end());
    }
#endif
};

/**
 * @brief The CalculateTriangleCentroidsImpl class calculates the normals of each triangle in parallel if
 * the Intel Threading Building Blocks (TBB) library is found.
 */
class CalculateTriangleNormals
{
    StructArray<Triangle>::Pointer m_TrianglesPtr;
    StructArray<Node>::Pointer m_NodesPtr;
    DataArray<double>::Pointer m_NormalsPtr;

  public:
    CalculateTriangleNormals(StructArray<Triangle>::Pointer triangles,
                                   StructArray<Node>::Pointer nodesPtr,
                                   DataArray<double>::Pointer normals) :
      m_TrianglesPtr(triangles),
      m_NodesPtr(nodesPtr),
      m_NormalsPtr(normals)
    {}

    virtual ~CalculateTriangleNormals(){}

    /**
     * @brief execute This is the actual method that gets called to calculate the normals. Call
     * this method directly if you do NOT have TBB installed and available. If you <b>are</b> running
     * this class in serial then the start value is Zero (0) and the ending value is the (numTriangles - 1).
     * If this is being used in a Parallel situation then start and end will be set by the thread that
     * is running this method.
     * @param start The starting triangle
     * @param end The ending triangle
     */
    void execute(size_t start, size_t end) const
    {
      double* normals = m_NormalsPtr->GetPointer(0);
      Node* nodes = m_NodesPtr->GetPointer(0);
      typedef vnl_vector_fixed<double,3> Vec3d_t;
      for (size_t i = start; i < end; i++)
      {
        Triangle& tri = *(m_TrianglesPtr->GetPointer(start));
        Node& n0 = nodes[tri.node_id[0]];
        Node& n1 = nodes[tri.node_id[1]];
        Node& n2 = nodes[tri.node_id[2]];
        Vec3d_t edge1( (n1.coord[0]-n0.coord[0]),
                                            (n1.coord[1]-n0.coord[1]),
                                            (n1.coord[2]-n0.coord[2])  );
        Vec3d_t edge2( (n2.coord[0]-n0.coord[0]),
                                          (n2.coord[1]-n0.coord[1]),
                                          (n2.coord[2]-n0.coord[2])  );
        Vec3d_t normal = vnl_cross_3d<double>(edge1, edge2);
        normal.normalize();
        normals[i*3] = normal[0];
        normals[i*3+1] = normal[1];
        normals[i*3+2] = normal[2];
      }
    }

#if DREAM3D_USE_PARALLEL_ALGORITHMS
    void operator()(const tbb::blocked_range<size_t> &r) const
    {
      execute(r.begin(), r.end());
    }
#endif
};




// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QSAMCurvature::QSAMCurvature() :
  AbstractFilter()
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QSAMCurvature::~QSAMCurvature()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QSAMCurvature::setupFilterParameters()
{
  std::vector<FilterParameter::Pointer> parameters;

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QSAMCurvature::writeFilterParameters(AbstractFilterParametersWriter* writer)
{
  /* Place code that will write the inputs values into a file. reference the
   AbstractFilterParametersWriter class for the proper API to use. */
  /*  writer->writeValue("OutputFile", getOutputFile() ); */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QSAMCurvature::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
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
    if(sm->getTriangles().get() == NULL)
    {
      addErrorMessage(getHumanLabel(), "SurfaceMesh DataContainer missing Triangles", -383);
      setErrorCondition(-384);
    }

    if(sm->getNodes().get() == NULL)
    {
      addErrorMessage(getHumanLabel(), "SurfaceMesh DataContainer missing Nodes", -384);
      setErrorCondition(-384);
    }

  }

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QSAMCurvature::preflight()
{
  /* Place code here that sanity checks input arrays and input values. Look at some
  * of the other DREAM3DLib/Filters/.cpp files for sample codes */
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QSAMCurvature::execute()
{
  int err = 0;
  std::stringstream ss;
  setErrorCondition(err);
  SurfaceMeshDataContainer* sm = getSurfaceMeshDataContainer();
  if(NULL == sm)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The Surface Mesh DataContainer Object was NULL", -999);
    return;
  }
  setErrorCondition(0);

  /* Place all your code to execute your filter here. */
  StructArray<Node>::Pointer nodesPtr = sm->getNodes();
  StructArray<Triangle>::Pointer trianglesPtr = sm->getTriangles();
  size_t totalTriangles = trianglesPtr->GetNumberOfTuples();

  // Create the Centroids Array
  DataArray<double>::Pointer centroidsPtr = DataArray<double>::CreateArray(trianglesPtr->GetNumberOfTuples(), 3, "QSAMCurvature_TriangleCentroids");

  // Create the Normals Array
  DataArray<double>::Pointer normalsPtr = DataArray<double>::CreateArray(trianglesPtr->GetNumberOfTuples(), 3, "QSAMCurvature_TriangleCentroids");



#if DREAM3D_USE_PARALLEL_ALGORITHMS
  tbb::parallel_for(tbb::blocked_range<size_t>(0, totalTriangles),
                    CalculateTriangleCentroidsImpl(trianglesPtr, nodesPtr, centroidsPtr), tbb::auto_partitioner());

#else
  CalculateTriangleCentroidsImpl serial_centroids(trianglesPtr, nodesPtr, centroidsPtr);
  serial_centroids.execute(0, totalTriangles);
#endif


#if DREAM3D_USE_PARALLEL_ALGORITHMS
  tbb::parallel_for(tbb::blocked_range<size_t>(0, totalTriangles),
                    CalculateTriangleNormals(trianglesPtr, nodesPtr, normalsPtr), tbb::auto_partitioner());

#else
  CalculateTriangleNormals serial_normals(trianglesPtr, nodesPtr, normalsPtr);
  serial_normals.execute(0, totalTriangles);
#endif



  /* Let the GUI know we are done with this filter */
  notifyStatusMessage("Complete");
}
