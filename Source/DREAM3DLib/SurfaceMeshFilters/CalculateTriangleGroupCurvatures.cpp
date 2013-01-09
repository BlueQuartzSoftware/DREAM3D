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

#include "CalculateTriangleGroupCurvatures.h"

#if DREAM3D_USE_PARALLEL_ALGORITHMS
#include <tbb/task_scheduler_init.h>
#include <tbb/task_group.h>
#include <tbb/task.h>
#endif


#include <Eigen/Dense>

#include "DREAM3DLib/Common/SurfaceMeshDataContainer.h"
#include "DREAM3DLib/Common/MatrixMath.h"
#include "DREAM3DLib/SurfaceMeshFilters/FindNRingNeighbors.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CalculateTriangleGroupCurvatures::CalculateTriangleGroupCurvatures(int nring,
                                                                   std::vector<int> triangleIds,
                                                                   NodeTrianglesMap_t* node2Triangle,
                                                                   DoubleArrayType::Pointer principleCurvature1,
                                                                   DoubleArrayType::Pointer principleCurvature2,
                                                                   SurfaceMeshDataContainer* sm,
                                                                   AbstractFilter* parent):
  m_NRing(nring),
  m_TriangleIds(triangleIds),
  m_NodeTrianglesMap(node2Triangle),
  m_PrincipleCurvature1(principleCurvature1),
  m_PrincipleCurvature2(principleCurvature2),
  m_SurfaceMeshDataContainer(sm),
  m_ParentFilter(parent)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CalculateTriangleGroupCurvatures::~CalculateTriangleGroupCurvatures()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
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
void CalculateTriangleGroupCurvatures::operator()() const
{


  StructArray<Triangle>::Pointer trianglesPtr = m_SurfaceMeshDataContainer->getTriangles();
  Triangle* triangles = trianglesPtr->GetPointer(0);

  FindNRingNeighbors::Pointer nRingNeighborAlg = FindNRingNeighbors::New();
  //  size_t totalTriangles = m_SurfaceMeshDataContainer->getTriangles()->GetNumberOfTuples();

  IDataArray::Pointer centroidPtr = m_SurfaceMeshDataContainer->getCellData(DREAM3D::CellData::SurfaceMeshTriangleCentroids);
  if (NULL == centroidPtr.get())
  {
    std::cout << "Triangle Centroids are required for this algorithm" << std::endl;
    return;
  }
  DataArray<double>* centroids = DataArray<double>::SafePointerDownCast(centroidPtr.get());

  IDataArray::Pointer normalPtr = m_SurfaceMeshDataContainer->getCellData(DREAM3D::CellData::SurfaceMeshTriangleNormals);
  if (NULL == normalPtr.get())
  {
    std::cout << "Triangle Normals are required for this algorithm" << std::endl;
    return;
  }
  DataArray<double>* normals = DataArray<double>::SafePointerDownCast(normalPtr.get());

#if 0
  IDataArray::Pointer grainFaceIdPtr = m_SurfaceMeshDataContainer->getCellData(DREAM3D::CellData::SurfaceMeshGrainFaceId);
  if (NULL == grainFaceIdPtr.get())
  {
    std::cout << "Triangle Grain Face Ids are required for this algorithm" << std::endl;
    return;
  }
  DataArray<int32_t>* grainFaceIds = DataArray<int32_t>::SafePointerDownCast(grainFaceIdPtr.get());
#endif

  Triangle& tri = triangles[m_TriangleIds[0]];
  int grain0 = 0;
  int grain1 = 0;
  if (tri.nSpin[0] < tri.nSpin[1])
  {
    grain0 = tri.nSpin[0];
    grain1 = tri.nSpin[1];
  }
  else
  {
    grain0 = tri.nSpin[1];
    grain1 = tri.nSpin[0];
  }


  std::stringstream ss;
  std::vector<int>::size_type tCount = m_TriangleIds.size();
  // For each triangle in the group
  for(std::vector<int>::size_type i = 0; i < tCount; ++i)
  {
    int triId = m_TriangleIds[i];
    nRingNeighborAlg->setTriangleId(triId);
    nRingNeighborAlg->setRegionId0(grain0);
    nRingNeighborAlg->setRegionId1(grain1);
    nRingNeighborAlg->setRing(m_NRing);
    nRingNeighborAlg->setSurfaceMeshDataContainer(m_SurfaceMeshDataContainer);
    nRingNeighborAlg->generate( *m_NodeTrianglesMap);

    UniqueTriangleIds_t triPatch = nRingNeighborAlg->getNRingTriangles();
    assert(triPatch.size() > 0);

    DataArray<double>::Pointer patchCentroids = extractPatchData(triId, triPatch, centroids->GetPointer(0), std::string("Patch_Centroids"));
    DataArray<double>::Pointer patchNormals = extractPatchData(triId, triPatch, normals->GetPointer(0), std::string("Patch_Normals"));
#if 0
    double* normTuple = NULL;
    // Set all the Normals to be consistent
    for(std::set<int32_t>::iterator iter = triPatch.begin(); iter != triPatch.end(); ++iter)
    {
      int32_t t = *iter;
      Triangle& tt = triangles[t];
      if (tt.nSpin[0] != grain0)
      {
        normTuple = patchNormals->GetPointer(i * 3);
        normTuple[0] = normTuple[0]*-1.0;
        normTuple[1] = normTuple[1]*-1.0;
        normTuple[2] = normTuple[2]*-1.0;
      }
    }
#endif

    // Translate the patch to the 0,0,0 origin
    double sub[3] = {patchCentroids->GetComponent(0,0),patchCentroids->GetComponent(0,1), patchCentroids->GetComponent(0,2)};
    subtractVector3d(patchCentroids, sub);

    double np[3] = {patchNormals->GetComponent(0,0), patchNormals->GetComponent(0,1), patchNormals->GetComponent(0, 2) };

    double seedCentroid[3] = {patchCentroids->GetComponent(0,0), patchCentroids->GetComponent(0,1), patchCentroids->GetComponent(0,2) };
    double firstCentroid[3] = {patchCentroids->GetComponent(1,0), patchCentroids->GetComponent(1,1), patchCentroids->GetComponent(1,2) };

    double temp[3] = {firstCentroid[0] - seedCentroid[0], firstCentroid[1] - seedCentroid[1], firstCentroid[2] - seedCentroid[2]};
    double vp[3] = {0.0, 0.0, 0.0};

    // Cross Product of np and temp
    MatrixMath::normalizeVector(np);
    MatrixMath::crossProduct(np, temp, vp);
    MatrixMath::normalizeVector(vp);

    // get the third orthogonal vector
    double up[3] = {0.0, 0.0, 0.0};
    MatrixMath::crossProduct(vp, np, up);

   // this constitutes a rotation matrix to a local coordinate system

    double rot[3][3] = {{up[0], up[1], up[2]},
                        {vp[0], vp[1], vp[2]},
                        {np[0], np[1], np[2]} };

    // Transform all centroids and normals to new coordinate system
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

    {
      // Solve the Least Squares fit for f(x,y) = 0.5 * A * x^2 + Bxy + 0.5*C*y^2 where
      // we are solving for the A, B & C constants.
      int cols = 3;
      int rows = patchCentroids->GetNumberOfTuples();
      Eigen::MatrixXd A(rows, cols);
      Eigen::VectorXd b(rows);
      double x, y, z;
      for(int m = 0; m < rows; ++m)
      {
        x = patchCentroids->GetComponent(m, 0);
        y = patchCentroids->GetComponent(m, 1);
        z = patchCentroids->GetComponent(m, 2);

        A(m) = 0.5 * x * x;  // 1/2 x^2
        A(m + rows) = x * y; // x*y
        A(m + rows*2) = 0.5 * y * y;  // 1/2 y^2
        b[m] = z; // The Z Values
      }
      Eigen::Vector3d sln1 = A.colPivHouseholderQr().solve(b);
      // Now that we have the A, B & C constants we can solve the Eigen value/vector problem
      // to get the principal curvatures and pricipal directions.
      Eigen::Matrix2d M;
      M << sln1(0), sln1(1), sln1(1), sln1(2);
      Eigen::SelfAdjointEigenSolver<Eigen::Matrix2d> eig(M);
      Eigen::SelfAdjointEigenSolver<Eigen::Matrix2d>::RealVectorType eValues = eig.eigenvalues();
      Eigen::SelfAdjointEigenSolver<Eigen::Matrix2d>::MatrixType eVectors = eig.eigenvectors();

      double gaussianCurvature = eValues(0);// Kappa 1
      double meanCurvature = eValues(1); //kappa 2

      m_PrincipleCurvature1->SetValue(triId, meanCurvature);
      m_PrincipleCurvature2->SetValue(triId, gaussianCurvature);

//      std::cout << "Eigen Values: " << eValues << std::endl;
//      std::cout << "Eigen Vectors: " << eVectors << std::endl;

    }

  } // End Loop over this triangle

  m_ParentFilter->tbbTaskProgress();
}
#if 0
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataArray<double>::Pointer CalculateTriangleGroupCurvatures::extractNormals(int triId, UniqueTriangleIds_t &triPatch,
                                                                              double* globalTriangleNormals,
                                                                              const std::string &name) const
{

  StructArray<Triangle>::Pointer trianglesPtr = m_SurfaceMeshDataContainer->getTriangles();
  Triangle* triangles = trianglesPtr->GetPointer(0);

  int g0 = triangles[triId].nSpin[0];
  int g1 = triangles[triId].nSpin[1];


  DataArray<double>::Pointer normals = DataArray<double>::CreateArray(triPatch.size() * 3, name);
  normals->SetNumberOfComponents(3);
  // This little chunk makes sure the current seed triangles centroid and normal data appear
  // first in the returned arrays which makes the next steps a tad easier.
  int i = 0;
  normals->SetComponent(i, 0, globalTriangleNormals[triId*3]);
  normals->SetComponent(i, 1, globalTriangleNormals[triId*3 + 1]);
  normals->SetComponent(i, 2, globalTriangleNormals[triId*3 + 2]);
  ++i;
  triPatch.erase(triId);

  for(std::set<int32_t>::iterator iter = triPatch.begin(); iter != triPatch.end(); ++iter)
  {
    int32_t t = *iter;
    normals->SetComponent(i, 0, globalTriangleNormals[t*3]);
    normals->SetComponent(i, 1, globalTriangleNormals[t*3 + 1]);
    normals->SetComponent(i, 2, globalTriangleNormals[t*3 + 2]);
    ++i;
  }
  triPatch.insert(triId);

  return normals;
}
#endif
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataArray<double>::Pointer CalculateTriangleGroupCurvatures::extractPatchData(int triId, UniqueTriangleIds_t &triPatch,
                                                                              double* data,
                                                                              const std::string &name) const
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


