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

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
#include <tbb/task_scheduler_init.h>
#include <tbb/task_group.h>
#include <tbb/task.h>
#endif


#include <Eigen/Dense>

#include "DREAM3DLib/Common/SurfaceMeshDataContainer.h"
#include "DREAM3DLib/Common/MatrixMath.h"
#include "DREAM3DLib/SurfaceMeshingFilters/FindNRingNeighbors.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CalculateTriangleGroupCurvatures::CalculateTriangleGroupCurvatures(int nring,
                                                                   std::vector<int> triangleIds,
                                                                   DoubleArrayType::Pointer principleCurvature1,
                                                                   DoubleArrayType::Pointer principleCurvature2,
                                                                   DoubleArrayType::Pointer principleDirection1,
                                                                   DoubleArrayType::Pointer principleDirection2,
                                                                   DoubleArrayType::Pointer gaussianCurvature,
                                                                   DoubleArrayType::Pointer meanCurvature,
                                                                   SurfaceMeshDataContainer* sm,
                                                                   AbstractFilter* parent):
  m_NRing(nring),
  m_TriangleIds(triangleIds),
  m_PrincipleCurvature1(principleCurvature1),
  m_PrincipleCurvature2(principleCurvature2),
  m_PrincipleDirection1(principleDirection1),
  m_PrincipleDirection2(principleDirection2),
  m_GaussianCurvature(gaussianCurvature),
  m_MeanCurvature(meanCurvature),
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

  // Get the Triangles Array
//  SurfaceMesh::DataStructures::FaceList_t::Pointer trianglesPtr = m_SurfaceMeshDataContainer->getFaces();
//  SurfaceMesh::DataStructures::Face_t* triangles = trianglesPtr->GetPointer(0);

  IDataArray::Pointer flPtr = m_SurfaceMeshDataContainer->getFaceData(DREAM3D::FaceData::SurfaceMeshTriangleLabels);
  DataArray<int32_t>* faceLabelsPtr = DataArray<int32_t>::SafePointerDownCast(flPtr.get());
  int32_t* faceLabels = faceLabelsPtr->GetPointer(0);

  // Instantiate a FindNRingNeighbors class to use during the loop
  FindNRingNeighbors::Pointer nRingNeighborAlg = FindNRingNeighbors::New();

  // Make Sure we have triangle centroids calculated
  IDataArray::Pointer centroidPtr = m_SurfaceMeshDataContainer->getFaceData(DREAM3D::FaceData::SurfaceMeshTriangleCentroids);
  if (NULL == centroidPtr.get())
  {
    std::cout << "Triangle Centroids are required for this algorithm" << std::endl;
    return;
  }
  DataArray<double>* centroids = DataArray<double>::SafePointerDownCast(centroidPtr.get());

  // Make sure we have triangle normals calculated
  IDataArray::Pointer normalPtr = m_SurfaceMeshDataContainer->getFaceData(DREAM3D::FaceData::SurfaceMeshTriangleNormals);
  if (NULL == normalPtr.get())
  {
    std::cout << "Triangle Normals are required for this algorithm" << std::endl;
    return;
  }
  DataArray<double>* normals = DataArray<double>::SafePointerDownCast(normalPtr.get());


  int32_t* fl = faceLabels + m_TriangleIds[0] * 2;
  int grain0 = 0;
  int grain1 = 0;
  if (fl[0] < fl[1])
  {
    grain0 = fl[0];
    grain1 = fl[1];
  }
  else
  {
    grain0 = fl[1];
    grain1 = fl[0];
  }

  bool computeGaussian = (m_GaussianCurvature.get() != NULL);
  bool computeMean = (m_MeanCurvature.get() != NULL);
  bool computeDirection = (m_PrincipleDirection1.get() != NULL);

  std::stringstream ss;
  std::vector<int>::size_type tCount = m_TriangleIds.size();
  // For each triangle in the group
  for(std::vector<int>::size_type i = 0; i < tCount; ++i)
  {
    if (m_ParentFilter->getCancel() == true) { return; }
    int triId = m_TriangleIds[i];
    nRingNeighborAlg->setTriangleId(triId);
    nRingNeighborAlg->setRegionId0(grain0);
    nRingNeighborAlg->setRegionId1(grain1);
    nRingNeighborAlg->setRing(m_NRing);
    nRingNeighborAlg->setSurfaceMeshDataContainer(m_SurfaceMeshDataContainer);
    nRingNeighborAlg->generate();

    SurfaceMesh::DataStructures::UniqueTriangleIds_t triPatch = nRingNeighborAlg->getNRingTriangles();
    assert(triPatch.size() > 1);

    DataArray<double>::Pointer patchCentroids = extractPatchData(triId, triPatch, centroids->GetPointer(0), std::string("Patch_Centroids"));
    DataArray<double>::Pointer patchNormals = extractPatchData(triId, triPatch, normals->GetPointer(0), std::string("Patch_Normals"));

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
    double out[3];
    // Transform all centroids and normals to new coordinate system
    for(size_t m = 0; m < patchCentroids->GetNumberOfTuples(); ++m)
    {
      ::memcpy(out, patchCentroids->GetPointer(m*3), 3*sizeof(double));
      MatrixMath::multiply3x3with3x1(rot, patchCentroids->GetPointer(m*3), out);
      ::memcpy(patchCentroids->GetPointer(m*3), out, 3*sizeof(double));

      ::memcpy(out, patchNormals->GetPointer(m*3), 3*sizeof(double));
      MatrixMath::multiply3x3with3x1(rot, patchNormals->GetPointer(m*3), out);
      ::memcpy(patchNormals->GetPointer(m*3), out, 3*sizeof(double));

      // We rotate the normals now but we dont use them yet. If we start using part 3 of Goldfeathers paper then we
      // will need the normals.
    }

#if 0
    if (triId == 1837)
    {
      std::cout << "# vtk DataFile Version 2.0" << std::endl;
      std::cout << "Rotated Patch" << std::endl;
      std::cout << "ASCII" << std::endl;
      std::cout << "DATASET POLYDATA" << std::endl;
      std::cout << "POINTS " <<  patchCentroids->GetNumberOfTuples() << " float" << std::endl;
      for(size_t m = 0; m < patchCentroids->GetNumberOfTuples(); ++m)
      {
        std::cout << patchCentroids->GetComponent(m, 0) << " " << patchCentroids->GetComponent(m, 1) << " " << patchCentroids->GetComponent(m, 2) << std::endl;
      }
      std::cout << "\nPOINT_DATA " << patchCentroids->GetNumberOfTuples() << std::endl;

      std::cout << "VECTORS patch_normals float" << std::endl;
      for(size_t m = 0; m < patchCentroids->GetNumberOfTuples(); ++m)
      {
        std::cout << patchNormals->GetComponent(m, 0) << " " << patchNormals->GetComponent(m, 1) << " " << patchNormals->GetComponent(m, 2) << std::endl;
      }
      std::cout << "SCALARS triangle_id int 1" << std::endl;
      std::cout << "LOOKUP_TABLE default" << std::endl;
      for(size_t m = 0; m < patchCentroids->GetNumberOfTuples(); ++m)
      {
        std::cout << m << std::endl;
      }
    }
#endif


    {
      // Solve the Least Squares fit for f(x,y) = 0.5 * A * x^2 + Bxy + 0.5*C*y^2 where
      // we are solving for the A, B & C constants.
      int cols = 7;
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
        A(m + rows*3) = x*x*x;
        A(m + rows*4) = x*x*y;
        A(m + rows*5) = x*y*y;
        A(m + rows*6) = y*y*y;
        b[m] = z; // The Z Values
      }
      typedef Eigen::Matrix<double, 7, 1> Vector7d;
      Vector7d sln1 = A.colPivHouseholderQr().solve(b);
      // Now that we have the A, B, C, D, E, F & G constants we can solve the Eigen value/vector problem
      // to get the principal curvatures and pricipal directions.
      Eigen::Matrix2d M;
      M << sln1(0), sln1(1), sln1(1), sln1(2);
      Eigen::SelfAdjointEigenSolver<Eigen::Matrix2d> eig(M);
      Eigen::SelfAdjointEigenSolver<Eigen::Matrix2d>::RealVectorType eValues = eig.eigenvalues();
      Eigen::SelfAdjointEigenSolver<Eigen::Matrix2d>::MatrixType eVectors = eig.eigenvectors();

      // Kappa1 >= Kappa2
      double kappa1 = eValues(0) * -1;// Kappa 1
      double kappa2 = eValues(1) * -1; //kappa 2
      assert(kappa1 >= kappa2);
      m_PrincipleCurvature1->SetValue(triId, kappa1);
      m_PrincipleCurvature2->SetValue(triId, kappa2);

      if (computeGaussian == true)
      {
        m_GaussianCurvature->SetValue(triId, kappa1*kappa2);
      }
      if (computeMean == true)
      {
        m_MeanCurvature->SetValue(triId, (kappa1+kappa2)/2.0);
      }

      if (computeDirection == true)
      {
        Eigen::Matrix3d e_rot_T;
        e_rot_T.row(0) = Eigen::Vector3d(up[0], vp[0], np[0]);
        e_rot_T.row(1) = Eigen::Vector3d(up[1], vp[1], np[1]);
        e_rot_T.row(2) = Eigen::Vector3d(up[2], vp[2], np[2]);

        // Rotate our principal directions back into the original coordinate system
        Eigen::Vector3d dir1 ( eVectors.col(0)(0),  eVectors.col(0)(1), 0.0 );
        dir1 = e_rot_T * dir1;
        ::memcpy(m_PrincipleDirection1->GetPointer(triId * 3), dir1.data(), 3*sizeof(double) );

        Eigen::Vector3d dir2 ( eVectors.col(1)(0),  eVectors.col(1)(1), 0.0 );
        dir2 = e_rot_T * dir2;
        ::memcpy(m_PrincipleDirection2->GetPointer(triId * 3), dir2.data(), 3*sizeof(double) );
      }
    }

  } // End Loop over this triangle

  m_ParentFilter->tbbTaskProgress();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataArray<double>::Pointer CalculateTriangleGroupCurvatures::extractPatchData(int triId, SurfaceMesh::DataStructures::UniqueTriangleIds_t &triPatch,
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


