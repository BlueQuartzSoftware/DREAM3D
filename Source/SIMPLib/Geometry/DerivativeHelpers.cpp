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


/* ============================================================================
 * DerivativeHelpers re-implements the exterior derivative calculations
 * from the following vtk modules:
 *
 * * vtkLine.cxx
 *   - re-implemented vtkLine::Derivatives to EdgeDeriv::operator()
 * * vtkTriangle.cxx
 *   - re-implemented vtkTriangle::Derivatives to TriangleDeriv::operator()
 * * vtkQuad.cxx
 *   - re-implemented vtkQuad::Derivatives to QuadDeriv::operator()
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "DerivativeHelpers.h"

#include "SIMPLib/Math/GeometryMath.h"
#include "SIMPLib/Math/MatrixMath.h"

#include "EdgeGeom.h"
#include "TriangleGeom.h"
#include "QuadGeom.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DerivativeHelpers::EdgeDeriv::operator()(EdgeGeom* edges, int64_t edgeId, double values[2], double derivs[3])
{
  float vert0_f[3] = { 0.0f, 0.0f, 0.0f };
  float vert1_f[3] = { 0.0f, 0.0f, 0.0f };
  double vert0[3] = { 0.0, 0.0, 0.0 };
  double vert1[3] = { 0.0, 0.0, 0.0 };
  double delta[3] = { 0.0, 0.0, 0.0 };
  int64_t verts[2] = { 0, 0 };

  edges->getVertsAtEdge(edgeId, verts);
  edges->getCoords(verts[0], vert0_f);
  edges->getCoords(verts[1], vert1_f);

  std::copy(vert0_f, vert0_f + 3, vert0);
  std::copy(vert1_f, vert1_f + 3, vert1);

  for (size_t i = 0; i < 3; i++)
  {
    delta[i] = vert1[i] - vert0[i];
  }

  for (size_t i = 0; i < 3; i++)
  {
    if (delta[i] != 0.0 )
    {
      derivs[i] = (values[1] - values[0]) / delta[i];
    }
    else
    {
      derivs[i] = 0.0;
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DerivativeHelpers::TriangleDeriv::operator()(TriangleGeom* triangles, int64_t triId, double values[3], double derivs[3])
{
  float vert0_f[3] = { 0.0f, 0.0f, 0.0f };
  float vert1_f[3] = { 0.0f, 0.0f, 0.0f };
  float vert2_f[3] = { 0.0f, 0.0f, 0.0f };
  double vert0[3] = { 0.0, 0.0, 0.0 };
  double vert1[3] = { 0.0, 0.0, 0.0 };
  double vert2[3] = { 0.0, 0.0, 0.0 };
  double vert0_2d[2] = { 0.0, 0.0 };
  double vert1_2d[2] = { 0.0, 0.0 };
  double vert2_2d[2] = { 0.0, 0.0 };
  double vector20[3] = { 0.0, 0.0, 0.0 };
  double basis1[3] = { 0.0, 0.0, 0.0 };
  double basis2[3] = { 0.0, 0.0, 0.0 };
  double mag_basis1 = 0.0;
  double mag_basis2 = 0.0;
  double normal[3] = { 0.0, 0.0, 0.0 };
  double shapeFunctions[6] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
  double sum[2] = { 0.0, 0.0 };
  double dBydx = 0.0;
  double dBydy = 0.0;
  int64_t verts[3] = { 0, 0, 0 };

  triangles->getVertsAtTri(triId, verts);
  triangles->getCoords(verts[0], vert0_f);
  triangles->getCoords(verts[1], vert1_f);
  triangles->getCoords(verts[2], vert2_f);

  std::copy(vert0_f, vert0_f + 3, vert0);
  std::copy(vert1_f, vert1_f + 3, vert1);
  std::copy(vert2_f, vert2_f + 3, vert2);

  GeometryMath::FindPlaneNormalVector(vert0, vert1, vert2, normal);
  MatrixMath::Normalize3x1(normal);

  for (size_t i = 0; i < 3; i++)
  {
    basis1[i] = vert1[i] - vert0[i];
    vector20[i] = vert2[i] - vert0[i];
  }

  MatrixMath::CrossProduct(normal, basis1, basis2);

  mag_basis1 = MatrixMath::Magnitude3x1(basis1);
  mag_basis2 = MatrixMath::Magnitude3x1(basis2);

  if ( mag_basis1 <= 0.0 || mag_basis2 <= 0.0 )
  {
    for (size_t i = 0; i < 3; i++ )
    {
      derivs[i] = 0.0;
    }
    return;
  }

  MatrixMath::Normalize3x1(basis1);
  MatrixMath::Normalize3x1(basis2);

  // Project the 3D triangle onto a 2D local system
  // Basis vectors for 2D system are the vector between triangle vertex 1 and 0 (basis1)
  // and the normal of the plane defined by the triangle normal and basis1 (basis2)
  vert0_2d[0] = vert0_2d[1] = 0.0;
  vert1_2d[0] = mag_basis1;
  vert1_2d[1] = 0.0;
  vert2_2d[0] = MatrixMath::DotProduct3x1(vector20, basis1);
  vert2_2d[1] = MatrixMath::DotProduct3x1(vector20, basis2);

  // Compute interpolation function derivatives
  triangles->getShapeFunctions(NULL, shapeFunctions);

  // Compute Jacobian and inverse Jacobian using Eigen
  // Jacobian is constant for a triangle, so inverse must exist
  double jPtr[4] = { 0.0, 0.0, 0.0, 0.0 };

  jPtr[0] = vert1_2d[0] - vert0_2d[0];
  jPtr[1] = vert1_2d[1] - vert0_2d[1];
  jPtr[2] = vert2_2d[0] - vert0_2d[0];
  jPtr[3] = vert2_2d[1] - vert0_2d[1];

  Eigen::Map<TriangleJacobian> jMat(jPtr);
  TriangleJacobian jMatI;

  jMatI = jMat.inverse();

  // Loop over derivative values. For each set of values, compute
  // derivatives in local 2D system and then transform into original 3D system
  for (size_t i = 0; i < 3; i++)
  {
    sum[0] += shapeFunctions[i] * values[i];
    sum[1] += shapeFunctions[3 + i] * values[i];
  }

  dBydx = sum[0] * jMatI(0, 0) + sum[1] * jMatI(0, 1);
  dBydy = sum[0] * jMatI(1, 0) + sum[1] * jMatI(1, 1);

  derivs[0] = dBydx * basis1[0] + dBydy * basis2[0];
  derivs[1] = dBydx * basis1[1] + dBydy * basis2[1];
  derivs[2] = dBydx * basis1[2] + dBydy * basis2[2];
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DerivativeHelpers::QuadDeriv::operator()(QuadGeom* quads, int64_t quadId, double values[4], double derivs[3])
{
  float vert0_f[3] = { 0.0f, 0.0f, 0.0f };
  float vert1_f[3] = { 0.0f, 0.0f, 0.0f };
  float vert2_f[3] = { 0.0f, 0.0f, 0.0f };
  float vert3_f[3] = { 0.0f, 0.0f, 0.0f };
  double vert0[3] = { 0.0, 0.0, 0.0 };
  double vert1[3] = { 0.0, 0.0, 0.0 };
  double vert2[3] = { 0.0, 0.0, 0.0 };
  double vert3[3] = { 0.0, 0.0, 0.0 };
  double vert0_2d[2] = { 0.0, 0.0 };
  double vert1_2d[2] = { 0.0, 0.0 };
  double vert2_2d[2] = { 0.0, 0.0 };
  double vert3_2d[2] = { 0.0, 0.0 };
  double vector20[3] = { 0.0, 0.0, 0.0 };
  double vector30[3] = { 0.0, 0.0, 0.0 };
  double basis1[3] = { 0.0, 0.0, 0.0 };
  double basis2[3] = { 0.0, 0.0, 0.0 };
  double mag_basis1 = 0.0;
  double mag_basis2 = 0.0;
  double normal[3] = { 0.0, 0.0, 0.0 };
  double shapeFunctions[8] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
  double pCoords[3] { 0.0, 0.0, 0.0 };
  double sum[2] = { 0.0, 0.0 };
  double dBydx = 0.0;
  double dBydy = 0.0;
  int64_t verts[4] = { 0, 0, 0 };

  quads->getVertsAtQuad(quadId, verts);
  quads->getCoords(verts[0], vert0_f);
  quads->getCoords(verts[1], vert1_f);
  quads->getCoords(verts[2], vert2_f);
  quads->getCoords(verts[3], vert3_f);

  std::copy(vert0_f, vert0_f + 3, vert0);
  std::copy(vert1_f, vert1_f + 3, vert1);
  std::copy(vert2_f, vert2_f + 3, vert2);
  std::copy(vert3_f, vert3_f + 3, vert3);

  GeometryMath::FindPlaneNormalVector(vert0, vert1, vert2, normal);
  MatrixMath::Normalize3x1(normal);

  // If vertices 0, 1, & 2 are co-linear, use vertex 3 to find the normal
  if (normal[0] == 0.0 && normal[1] == 0.0 && normal[2] == 0.0)
  {
    GeometryMath::FindPlaneNormalVector(vert0, vert1, vert2, normal);
    MatrixMath::Normalize3x1(normal);
  }

  for (size_t i = 0; i < 3; i++)
  {
    basis1[i] = vert1[i] - vert0[i];
    vector20[i] = vert2[i] - vert0[i];
    vector30[i] = vert3[i] - vert0[i];
  }

  MatrixMath::CrossProduct(normal, basis1, basis2);

  mag_basis1 = MatrixMath::Magnitude3x1(basis1);
  mag_basis2 = MatrixMath::Magnitude3x1(basis2);

  if ( mag_basis1 <= 0.0 || mag_basis2 <= 0.0 )
  {
    for (size_t i = 0; i < 3; i++ )
    {
      derivs[i] = 0.0;
    }
    return;
  }

  MatrixMath::Normalize3x1(basis1);
  MatrixMath::Normalize3x1(basis2);

  // Project the 3D quad onto a 2D local system
  // Basis vectors for 2D system are the vector between quad vertex 1 and 0 (basis1)
  // and the normal of the plane defined by the quad normal and basis1 (basis2)
  vert0_2d[0] = vert0_2d[1] = 0.0;
  vert1_2d[0] = mag_basis1;
  vert1_2d[1] = 0.0;
  vert2_2d[0] = MatrixMath::DotProduct3x1(vector20, basis1);
  vert2_2d[1] = MatrixMath::DotProduct3x1(vector20, basis2);
  vert3_2d[0] = MatrixMath::DotProduct3x1(vector30, basis1);
  vert3_2d[1] = MatrixMath::DotProduct3x1(vector30, basis2);

  quads->getParametricCenter(pCoords);
  quads->getShapeFunctions(pCoords, shapeFunctions);

  // Compute Jacobian and inverse Jacobian using Eigen
  double jPtr[4] = { 0.0, 0.0, 0.0, 0.0 };

  jPtr[0] = vert0_2d[0] * shapeFunctions[0] + vert1_2d[0] * shapeFunctions[1] +
            vert2_2d[0] * shapeFunctions[2] + vert3_2d[0] * shapeFunctions[3];
  jPtr[1] = vert0_2d[1] * shapeFunctions[0] + vert1_2d[1] * shapeFunctions[1] +
            vert2_2d[1] * shapeFunctions[2] + vert3_2d[1] * shapeFunctions[3];
  jPtr[2] = vert0_2d[0] * shapeFunctions[4] + vert1_2d[0] * shapeFunctions[5] +
            vert2_2d[0] * shapeFunctions[6] + vert3_2d[0] * shapeFunctions[7];
  jPtr[3] = vert0_2d[1] * shapeFunctions[4] + vert1_2d[1] * shapeFunctions[5] +
            vert2_2d[1] * shapeFunctions[6] + vert3_2d[1] * shapeFunctions[7];

  Eigen::Map<QuadJacobian> jMat(jPtr);
  QuadJacobian jMatI;
  bool invertible = false;

  jMat.computeInverseWithCheck(jMatI, invertible);

  // Jacobian is non-constant for a quad, so must check if must exists
  // If the Jacobian is not invertible, set derivatives to 0
  if (!invertible)
  {
    for (size_t i = 0; i < 3; i++ )
    {
      derivs[i] = 0.0;
    }
  }

  // Loop over derivative values. For each set of values, compute
  // derivatives in local 2D system and then transform into original 3D system
  for (size_t i = 0; i < 4; i++)
  {
    sum[0] += shapeFunctions[i] * values[i];
    sum[1] += shapeFunctions[4 + i] * values[i];
  }

  dBydx = sum[0] * jMatI(0, 0) + sum[1] * jMatI(0, 1);
  dBydy = sum[0] * jMatI(1, 0) + sum[1] * jMatI(1, 1);

  derivs[0] = dBydx * basis1[0] + dBydy * basis2[0];
  derivs[1] = dBydx * basis1[1] + dBydy * basis2[1];
  derivs[2] = dBydx * basis1[2] + dBydy * basis2[2];
}
