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
 * DerivativeHelpers uses code adapated from the following vtk modules:
 *
 * * vtkLine.cxx
 *   - adapted vtkLine::Derivatives to EdgeDeriv::operator()
 * * vtkTriangle.cxx
 *   - adapted vtkTriangle::Derivatives to TriangleDeriv::operator()
 * * vtkQuad.cxx
 *   - adapted vtkQuad::Derivatives to QuadDeriv::operator()
 *
 * The vtk license is reproduced below.
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* ============================================================================
 * Copyright (c) 1993-2008 Ken Martin, Will Schroeder, Bill Lorensen
 * All rights reserved.

 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * * Neither name of Ken Martin, Will Schroeder, or Bill Lorensen nor the names of
 * any contributors may be used to endorse or promote products derived from this
 * software without specific prior written permission.

 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
  float x0f[3], x1f[3];
  double x0[3], x1[3], deltaX[3];
  int64_t verts[2];

  edges->getVertsAtEdge(edgeId, verts);
  edges->getCoords(verts[0], x0f);
  edges->getCoords(verts[1], x1f);

  for (size_t i = 0; i < 3; i++)
  {
    x0[i] = static_cast<double>(x0f[i]);
    x1[i] = static_cast<double>(x1f[i]);
  }

  for (size_t i = 0; i < 3; i++)
  {
    deltaX[i] = x1[i] - x0[i];
  }

  for (size_t i = 0; i < 3; i++)
  {
    if ( deltaX[i] != 0 )
    {
      derivs[i] = (values[1] - values[0]) / deltaX[i];
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
  float x0f[3], x1f[3], x2f[3];
  double v0[2], v1[2], v2[2], v[3], v10[3], v20[3], lenX, lenTmp;
  double x0[3], x1[3], x2[3], n[3];
  double funcDerivs[6], sum[2], dBydx, dBydy;
  int64_t verts[3];

  // Project points of triangle into 2D system
  triangles->getVertsAtTri(triId, verts);
  triangles->getCoords(verts[0], x0f);
  triangles->getCoords(verts[1], x1f);
  triangles->getCoords(verts[2], x2f);

  for (size_t i = 0; i < 3; i++)
  {
    x0[i] = static_cast<double>(x0f[i]);
    x1[i] = static_cast<double>(x1f[i]);
    x2[i] = static_cast<double>(x2f[i]);
  }

  GeometryMath::FindPlaneNormalVector(x0, x1, x2, n);
  MatrixMath::Normalize3x1(n);

  for (size_t i = 0; i < 3; i++)
  {
    v10[i] = x1[i] - x0[i];
    v[i] = x2[i] - x0[i];
  }

  MatrixMath::CrossProduct(n, v10, v20);

  lenX = MatrixMath::Magnitude3x1(v10);
  lenTmp = MatrixMath::Magnitude3x1(v20);

  if ( lenX <= 0.0
       || lenTmp <= 0.0 ) // degenerate
  {
    for (size_t i = 0; i < 3; i++ )
    {
      derivs[i] = 0.0;
    }
    return;
  }

  MatrixMath::Normalize3x1(v10);
  MatrixMath::Normalize3x1(v20);

  v0[0] = v0[1] = 0.0; //convert points to 2D (i.e., local system)
  v1[0] = lenX;
  v1[1] = 0.0;
  v2[0] = MatrixMath::DotProduct3x1(v, v10);
  v2[1] = MatrixMath::DotProduct3x1(v, v20);

  // Compute interpolation function derivatives
  triangles->getShapeFunctions(NULL, funcDerivs);

  // Compute Jacobian and inverse Jacobian using Eigen
  // Jacobian is constant for a triangle
  double jPtr[4];

  jPtr[0] = v1[0] - v0[0];
  jPtr[1] = v1[1] - v0[1];
  jPtr[2] = v2[0] - v0[0];
  jPtr[3] = v2[1] - v0[1];

  Eigen::Map<TriangleJacobian> jMat(jPtr);
  TriangleJacobian jMatI;

  jMatI = jMat.inverse();

  // Loop over derivative values. For each set of values, compute
  // derivatives in local system and then transform into modelling system.
  // First compute derivatives in local x'-y' coordinate system
  sum[0] = sum[1] = 0.0;
  for (size_t i = 0; i < 3; i++) //loop over interp. function derivatives
  {
    sum[0] += funcDerivs[i] * values[i];
    sum[1] += funcDerivs[3 + i] * values[i];
  }

  dBydx = sum[0] * jMatI(0, 0) + sum[1] * jMatI(0, 1);
  dBydy = sum[0] * jMatI(1, 0) + sum[1] * jMatI(1, 1);

  // Transform into global system (dot product with global axes)
  derivs[0] = dBydx * v10[0] + dBydy * v20[0];
  derivs[1] = dBydx * v10[1] + dBydy * v20[1];
  derivs[2] = dBydx * v10[2] + dBydy * v20[2];
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DerivativeHelpers::QuadDeriv::operator()(QuadGeom* quads, int64_t quadId, double values[4], double derivs[3])
{
  float x0f[3], x1f[3], x2f[3], x3f[3];
  double v0[2], v1[2], v2[2], v3[2], v10[3], v20[3], lenX, lenTmp;
  double x0[3], x1[3], x2[3], x3[3], n[3], vec20[3], vec30[3], pCoords[3];
  double funcDerivs[8], sum[2], dBydx, dBydy;
  int64_t verts[4];

  // Project points of quad into 2D system
  quads->getVertsAtQuad(quadId, verts);
  quads->getCoords(verts[0], x0f);
  quads->getCoords(verts[1], x1f);
  quads->getCoords(verts[2], x2f);
  quads->getCoords(verts[3], x3f);

  for (size_t i = 0; i < 3; i++)
  {
    x0[i] = static_cast<double>(x0f[i]);
    x1[i] = static_cast<double>(x1f[i]);
    x2[i] = static_cast<double>(x2f[i]);
    x3[i] = static_cast<double>(x3f[i]);
  }

  GeometryMath::FindPlaneNormalVector(x0, x1, x2, n);
  MatrixMath::Normalize3x1(n);

  // If the x0, x1, and x2 are co-linear, use x3 to find the normal
  if (n[0] == 0.0 && n[1] == 0.0 && n[2] == 0.0)
  {
    GeometryMath::FindPlaneNormalVector(x0, x1, x2, n);
    MatrixMath::Normalize3x1(n);
  }

  for (size_t i = 0; i < 3; i++)
  {
    v10[i] = x1[i] - x0[i];
    vec20[i] = x2[i] - x0[i];
    vec30[i] = x3[i] - x0[i];
  }

  MatrixMath::CrossProduct(n, v10, v20);

  lenX = MatrixMath::Magnitude3x1(v10);
  lenTmp = MatrixMath::Magnitude3x1(v20);

  if ( lenX <= 0.0
       || lenTmp <= 0.0 ) // degenerate
  {
    for (size_t i = 0; i < 3; i++ )
    {
      derivs[i] = 0.0;
    }
    return;
  }

  MatrixMath::Normalize3x1(v10);
  MatrixMath::Normalize3x1(v20);

  v0[0] = v0[1] = 0.0; // convert points to 2D (i.e., local system)
  v1[0] = lenX;
  v1[1] = 0.0;
  v2[0] = MatrixMath::DotProduct3x1(vec20, v10);
  v2[1] = MatrixMath::DotProduct3x1(vec20, v20);
  v3[0] = MatrixMath::DotProduct3x1(vec30, v10);
  v3[1] = MatrixMath::DotProduct3x1(vec30, v20);

  quads->getParametricCenter(pCoords);
  quads->getShapeFunctions(pCoords, funcDerivs);

  // Compute Jacobian and inverse Jacobian using Eigen
  double jPtr[4];

  jPtr[0] = v0[0] * funcDerivs[0] + v1[0] * funcDerivs[1] +
            v2[0] * funcDerivs[2] + v3[0] * funcDerivs[3];
  jPtr[1] = v0[1] * funcDerivs[0] + v1[1] * funcDerivs[1] +
            v2[1] * funcDerivs[2] + v3[1] * funcDerivs[3];
  jPtr[2] = v0[0] * funcDerivs[4] + v1[0] * funcDerivs[5] +
            v2[0] * funcDerivs[6] + v3[0] * funcDerivs[7];
  jPtr[3] = v0[1] * funcDerivs[4] + v1[1] * funcDerivs[5] +
            v2[1] * funcDerivs[6] + v3[1] * funcDerivs[7];

  Eigen::Map<QuadJacobian> jMat(jPtr);
  QuadJacobian jMatI;
  bool invertible;

  jMat.computeInverseWithCheck(jMatI, invertible);

  if (!invertible)
  {
    for (int i = 0; i < 3; i++ )
    {
      derivs[i] = 0.0;
    }
  }

  // Loop over derivative values. For each set of values,
  // compute derivatives
  // in local system and then transform into modelling system.
  // First compute derivatives in local x'-y' coordinate system
  sum[0] = sum[1] = 0.0;
  for (size_t i = 0; i < 4; i++) // loop over interp. function derivatives
  {
    sum[0] += funcDerivs[i] * values[i];
    sum[1] += funcDerivs[4 + i] * values[i];
  }

  dBydx = sum[0] * jMatI(0, 0) + sum[1] * jMatI(0, 1);
  dBydy = sum[0] * jMatI(1, 0) + sum[1] * jMatI(1, 1);

  // Transform into global system (dot product with global axes)
  derivs[0] = dBydx * v10[0] + dBydy * v20[0];
  derivs[1] = dBydx * v10[1] + dBydy * v20[1];
  derivs[2] = dBydx * v10[2] + dBydy * v20[2];
}
