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

#include "CalculateTriangleCurvature.h"

#include "DREAM3DLib/Common/SurfaceMeshDataContainer.h"
#include "DREAM3DLib/Common/MatrixMath.h"
#include "FindNRingNeighbors.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CalculateTriangleCurvature::CalculateTriangleCurvature(int nring, std::vector<int> triangleIds,
                                                      int grainId,
                                                      NodeTrianglesMap_t* node2Triangle,
                                                      SurfaceMeshDataContainer* sm) :
m_NRing(nring),
m_TriangleIds(triangleIds),
m_GrainId(grainId),
m_NodeTrianglesMap(node2Triangle),
m_SurfaceMeshDataContainer(sm)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CalculateTriangleCurvature::~CalculateTriangleCurvature()
{

}


/*
 For i=0, ntri-1 do ...
    tripatch = get_nring_neighborhood(i, nring)
    ;(triptych):Array of pointers to the triangles in the nring neighborhood of the i-th triangle. Assume the first pointer is the pointer to the i-th triangle
    points = triloc[0:2, tripatch]
    norm = trinorm[0:2, tripatch]

    points -= points[0:2,0] ;center at [0,0,0]
    np = norm[0:2,0] ; get the normal at the ith triangle
    ; calculate the projection of an inplane vector
    vp = Normalize(Cross_Product(np, (points[0:2,1]-points[0:2,0]) ) )
    ; get the third orthogonal vector
    up = Cross_product(vp,np)
    ; this consitutes a rotation matrix to a local coordinate system
    rot = [[up],[vp],[np]]

    points = matrix_multiply(points, rot)
    norms = matrix_multiply(norm, rot)
\end{verbatim}

Now we can fit the points to a quadratic:
\begin{eqnarray*}
z = \frac{1}{2}A \* x^2 + Bxy + \frac{1}{2}Cy^2
\end{eqnarray*}

This is done by implementing a least squares fit to a system of linear equations.

\begin{verbatim}
    coeff = [0.5*x^2, x*y, 0.5*y^2]
    val = z

    ABC = LA_LEAST_SQUARES(coeff, val)
\end{verbatim}
This is the IDL least squares routine. In my brief googling, it looks like the DGELS routine of the LAPACK/BLAS accomplishes much the same.
Now solve the eigenvalue problem W = [[A,B],[B,C]]

\begin{verbatim}
    W = [[A,B],[B,C]]
    k12 = Eigenql(W, eigenvectors=evect)
  endfor
\end{verbatim}
*/

void subtractVector3d(DataArray<double>::Pointer data, double* v)
{

  size_t count = data->GetNumberOfTuples();
  for(size_t i = 0; i < count; ++i)
  {
    double* ptr = data->GetPointer(i*3);
    ptr[0] = ptr[0] - v[0];
    ptr[1] = ptr[1] - v[1];
    ptr[2] = ptr[2] - v[2];
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
#if defined (DREAM3D_USE_PARALLEL_ALGORITHMS)
tbb::task*
#else
void
#endif
CalculateTriangleCurvature::execute()
{
  std::vector<int>::size_type tCount = m_TriangleIds.size();
  FindNRingNeighbors::Pointer nRingNeighborAlg = FindNRingNeighbors::New();
//  size_t totalTriangles = m_SurfaceMeshDataContainer->getTriangles()->GetNumberOfTuples();

  IDataArray::Pointer centroidPtr = m_SurfaceMeshDataContainer->getCellData(DREAM3D::CellData::SurfaceMeshTriangleCentroids);
  if (NULL == centroidPtr.get())
  {
    std::cout << "Triangle Centroids are required for this algorithm" << std::endl;
    return
    #if defined (DREAM3D_USE_PARALLEL_ALGORITHMS)
    NULL
    #endif
    ;
  }
  DataArray<double>* centroids = DataArray<double>::SafePointerDownCast(centroidPtr.get());

  IDataArray::Pointer normalPtr = m_SurfaceMeshDataContainer->getCellData(DREAM3D::CellData::SurfaceMeshTriangleNormals);
  if (NULL == normalPtr.get())
  {
    std::cout << "Triangle Normals are required for this algorithm" << std::endl;
    return
    #if defined (DREAM3D_USE_PARALLEL_ALGORITHMS)
    NULL
    #endif
    ;
  }
  DataArray<double>* normals = DataArray<double>::SafePointerDownCast(normalPtr.get());
  std::stringstream ss;

  // For each triangle in the group
  for(std::vector<int>::size_type i = 0; i < tCount; ++i)
  {
    int triId = m_TriangleIds[i];
    if (triId != 65764) { continue; }
    nRingNeighborAlg->setTriangleId(triId);
    nRingNeighborAlg->setRegionId(m_GrainId);
    nRingNeighborAlg->setRing(m_NRing);
    nRingNeighborAlg->setSurfaceMeshDataContainer(m_SurfaceMeshDataContainer);
    nRingNeighborAlg->generate( *m_NodeTrianglesMap);
    ss.str("");
    ss << "/tmp/nring_" << triId << ".vtk";
    nRingNeighborAlg->writeVTKFile(ss.str());

    UniqueTriangleIds_t triPatch = nRingNeighborAlg->getNRingTriangles();

    DataArray<double>::Pointer patchCentroids = extractPatchData(triId, triPatch, centroids->GetPointer(0), std::string("Patch_Centroids"));
    DataArray<double>::Pointer patchNormals = extractPatchData(triId, triPatch, normals->GetPointer(0), std::string("Patch_Normals"));

    for(int u=0; u < 2; ++u)
    {
      std::cout << "Centroid[" << u  << "] =" << patchCentroids->GetComponent(u, 0) << ", " <<patchCentroids->GetComponent(u, 1) << ", " << patchCentroids->GetComponent(u, 2) << std::endl;
    }

    // Translate the patch to the 0,0,0 origin
    double sub[3] = {patchCentroids->GetComponent(0,0),patchCentroids->GetComponent(0,1), patchCentroids->GetComponent(0,2)};
    subtractVector3d(patchCentroids, sub);
    for(int u=0; u < 2; ++u)
    {
      std::cout << "Centroid[" << u  << "] =" << patchCentroids->GetComponent(u, 0) << ", " <<patchCentroids->GetComponent(u, 1) << ", " << patchCentroids->GetComponent(u, 2) << std::endl;
    }

    double np[3] = {patchNormals->GetComponent(0,0), patchNormals->GetComponent(0,1), patchNormals->GetComponent(0, 2) };

    double seedCentroid[3] = {patchCentroids->GetComponent(0,0), patchCentroids->GetComponent(0,1), patchCentroids->GetComponent(0,2) };
    double firstCentroid[3] = {patchCentroids->GetComponent(1,0), patchCentroids->GetComponent(1,1), patchCentroids->GetComponent(1,2) };

    double temp[3] = {firstCentroid[0] - seedCentroid[0], firstCentroid[1] - seedCentroid[1], firstCentroid[2] - seedCentroid[2]};
    double vp[3] = {0.0, 0.0, 0.0};

    // Cross Product of np and temp
    MatrixMath::crossProduct(np, temp, vp);
    MatrixMath::normalize(vp);

    // get the third orthogonal vector
    double up[3] = {0.0, 0.0, 0.0};
    MatrixMath::crossProduct(vp, np, up);

    MatrixMath::crossProduct(np,up, vp);

    // this consitutes a rotation matrix to a local coordinate system
#if 1
    double rot[3][3] = {{up[0], up[1], up[2]},
                        {vp[0], vp[1], vp[2]},
                        {np[0], np[1], np[2]} };
#else
    double rot[3][3] = {{up[0], vp[0], np[0]},
                        {up[1], vp[1], np[1]},
                        {up[2], vp[2], np[2]} };
#endif
    // Transform all
    double out[3];
    for(size_t m = 0; m < patchCentroids->GetNumberOfTuples(); ++m)
    {
      ::memcpy(out, patchCentroids->GetPointer(m*3), 3*sizeof(double));
      MatrixMath::multiply3x3with3x1(rot, patchCentroids->GetPointer(m*3), out);
      ::memcpy(patchCentroids->GetPointer(m*3), out, 3*sizeof(double));

      ::memcpy(out, patchNormals->GetPointer(m*3), 3*sizeof(double));
      MatrixMath::multiply3x3with3x1(rot, patchNormals->GetPointer(m*3), out);
      ::memcpy(patchNormals->GetPointer(m*3), out, 3*sizeof(double));
    }

    for(int u=0; u < 2; ++u)
    {
      std::cout << "Centroid[" << u  << "] =" << patchCentroids->GetComponent(u, 0) << ", " <<patchCentroids->GetComponent(u, 1) << ", " << patchCentroids->GetComponent(u, 2) << std::endl;
      std::cout << "Normal[" << u  << "] =" << patchNormals->GetComponent(u, 0) << ", " <<patchNormals->GetComponent(u, 1) << ", " << patchNormals->GetComponent(u, 2) << std::endl;
    }

  }

#if defined (DREAM3D_USE_PARALLEL_ALGORITHMS)
  return NULL;
#endif
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataArray<double>::Pointer CalculateTriangleCurvature::extractPatchData(int triId, UniqueTriangleIds_t &triPatch,
                                                                            double* data,
                                                                            const std::string &name)
{
  DataArray<double>::Pointer extractedData = DataArray<double>::CreateArray(triPatch.size() * 3, name);
  extractedData->SetNumberOfComponents(3);
  // This little chunk makes sure the current seed triangles centroid and normal data appear
  // first in the returned arrays which makes the next steps a tad easier.
  int i = 0;
  extractedData->SetComponent(i, 0, data[triId*3]);
  extractedData->SetComponent(i, 1, data[triId*3 + 1]);
  extractedData->SetComponent(i, 2, data[triId*3 + 2]);
  ++i;
  triPatch.erase(triId);

  for(std::set<int32_t>::iterator iter = triPatch.begin(); iter != triPatch.end(); ++iter)
  {
    int32_t t = *iter;
    extractedData->SetComponent(i, 0, data[t*3]);
    extractedData->SetComponent(i, 1, data[t*3 + 1]);
    extractedData->SetComponent(i, 2, data[t*3 + 2]);
    ++i;
  }
  triPatch.insert(triId);

  return extractedData;
}


