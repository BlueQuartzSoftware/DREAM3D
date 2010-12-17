/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
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
 * Neither the name of Michael A. Jackson nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.
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
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "Quaternions.h"


using namespace AIM;

/**
 * @brief Initialize our Symmetry Arrays
 */

double Quaternions::quat_symmcubic[24][5] = {
 {0.000000000, 0.000000000, 0.000000000, 0.000000000, 1.000000000},
 {0.000000000, 1.000000000, 0.000000000, 0.000000000, 0.000000000},
 {0.000000000, 0.000000000, 1.000000000, 0.000000000, 0.000000000},
 {0.000000000, 0.000000000, 0.000000000, 1.000000000, 0.000000000},
 {0.000000000, 0.707106781, 0.000000000, 0.000000000, 0.707106781},
 {0.000000000, 0.000000000, 0.707106781, 0.000000000, 0.707106781},
 {0.000000000, 0.000000000, 0.000000000, 0.707106781, 0.707106781},
 {0.000000000, -0.707106781, 0.000000000, 0.000000000, 0.707106781},
 {0.000000000, 0.000000000, -0.707106781, 0.000000000, 0.707106781},
 {0.000000000, 0.000000000, 0.000000000, -0.707106781, 0.707106781},
 {0.000000000, 0.707106781, 0.707106781, 0.000000000, 0.000000000},
 {0.000000000, -0.707106781, 0.707106781, 0.000000000, 0.000000000},
 {0.000000000, 0.000000000, 0.707106781, 0.707106781, 0.000000000},
 {0.000000000, 0.000000000, -0.707106781, 0.707106781, 0.000000000},
 {0.000000000, 0.707106781, 0.000000000, 0.707106781, 0.000000000},
 {0.000000000, -0.707106781, 0.000000000, 0.707106781, 0.000000000},
 {0.000000000, 0.500000000, 0.500000000, 0.500000000, 0.500000000},
 {0.000000000, -0.500000000, -0.500000000, -0.500000000, 0.500000000},
 {0.000000000, 0.500000000, -0.500000000, 0.500000000, 0.500000000},
 {0.000000000, -0.500000000, 0.500000000, -0.500000000, 0.500000000},
 {0.000000000, -0.500000000, 0.500000000, 0.500000000, 0.500000000},
 {0.000000000, 0.500000000, -0.500000000, -0.500000000, 0.500000000},
 {0.000000000, -0.500000000, -0.500000000, 0.500000000, 0.500000000},
 {0.000000000, 0.500000000, 0.500000000, -0.500000000, 0.500000000}
};

double Quaternions::quat_symmhex[12][5] = {
 {0.000000000, 0.000000000, 0.000000000, 0.000000000, 1.000000000},
 {0.000000000, 0.000000000, 0.000000000, 0.500000000, 0.866025400},
 {0.000000000, 0.000000000, 0.000000000, 0.866025400, 0.500000000},
 {0.000000000, 0.000000000, 0.000000000, 1.000000000, 0.000000000},
 {0.000000000, 0.000000000, 0.000000000, 0.866025400, -0.50000000},
 {0.000000000, 0.000000000, 0.000000000, 0.500000000, -0.86602540},
 {0.000000000, 1.000000000, 0.000000000, 0.000000000, 0.000000000},
 {0.000000000, 0.866025400, 0.500000000, 0.000000000, 0.000000000},
 {0.000000000, 0.500000000, 0.866025400, 0.000000000, 0.000000000},
 {0.000000000, 0.000000000, 1.000000000, 0.000000000, 0.000000000},
 {0.000000000, -0.50000000, 0.866025400, 0.000000000, 0.000000000},
 {0.000000000, -0.86602540, 0.500000000, 0.000000000, 0.000000000}
};



#define QUAT_SYMMHEX AIM::Quaternions::quat_symmhex
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Quaternions::Hex_MultiplyByUnitQuaterion(double* unitQuat, size_t i, double* outQuat)
{

//  if (Quaternions::isInitialized == false)
//  {
//    Quaternions::initialize();
//    // This will make sure things are initialized
//  }
  outQuat[0]=QUAT_SYMMHEX[i][1]*unitQuat[3]+QUAT_SYMMHEX[i][4]*unitQuat[0]-QUAT_SYMMHEX[i][2]*unitQuat[2]+QUAT_SYMMHEX[i][3]*unitQuat[1];
  outQuat[1]=QUAT_SYMMHEX[i][2]*unitQuat[3]+QUAT_SYMMHEX[i][4]*unitQuat[1]-QUAT_SYMMHEX[i][3]*unitQuat[0]+QUAT_SYMMHEX[i][1]*unitQuat[2];
  outQuat[2]=QUAT_SYMMHEX[i][3]*unitQuat[3]+QUAT_SYMMHEX[i][4]*unitQuat[2]-QUAT_SYMMHEX[i][1]*unitQuat[1]+QUAT_SYMMHEX[i][2]*unitQuat[0];
  outQuat[3]=QUAT_SYMMHEX[i][4]*unitQuat[3]-QUAT_SYMMHEX[i][1]*unitQuat[0]-QUAT_SYMMHEX[i][2]*unitQuat[1]-QUAT_SYMMHEX[i][3]*unitQuat[2];
}


#if 0
  This code was taken from a paper on Quaterions. Just used as reference.
typedef struct
{
    float x, y, z, w;
} Quat;
typedef float HMatrix[4][4];
#define X 0
#define Y 1
#define Z 2
#define W 3
/* Return quaternion product qL * qR. */
Quat Qt_Mul(Quat qL, Quat qR)
{
  Quat qq;
  qq.w = qL.w * qR.w - qL.x * qR.x - qL.y * qR.y - qL.z * qR.z;
  qq.x = qL.w * qR.x + qL.x * qR.w + qL.y * qR.z - qL.z * qR.y;
  qq.y = qL.w * qR.y + qL.y * qR.w + qL.z * qR.x - qL.x * qR.z;
  qq.z = qL.w * qR.z + qL.z * qR.w + qL.x * qR.y - qL.y * qR.x;
  return (qq);
}



/* Return norm of quaternion, the sum of the squares of the components. */
#define Qt_Norm(q) ((q).x*(q).x + (q).y*(q).y + (q).z*(q).z + (q).w*(q).w)
/* Construct rotation matrix from (possibly non-unit) quaternion.
 * Assumes matrix is used to multiply column vector on the left:
 * vnew = mat vold.  Works correctly for right-handed coordinate system
 * and right-handed rotations. */
void Qt_ToMatrix(Quat q, HMatrix mat)
{
  double Nq = Qt_Norm(q);
  double s = (Nq > 0.0) ? (2.0 / Nq) : 0.0;
  double xs = q.x * s, ys = q.y * s, zs = q.z * s;
  double wx = q.w * xs, wy = q.w * ys, wz = q.w * zs;
  double xx = q.x * xs, xy = q.x * ys, xz = q.x * zs;
  double yy = q.y * ys, yz = q.y * zs, zz = q.z * zs;
  mat[X][X] = 1.0 - (yy + zz);
  mat[Y][X] = xy + wz;
  mat[Z][X] = xz - wy;
  mat[X][Y] = xy - wz;
  mat[Y][Y] = 1.0 - (xx + zz);
  mat[Z][Y] = yz + wx;
  mat[X][Z] = xz + wy;
  mat[Y][Z] = yz - wx;
  mat[Z][Z] = 1.0 - (xx + yy);
  mat[X][W] = mat[Y][W] = mat[Z][W] = 0.0;
  mat[W][X] = mat[W][Y] = mat[W][Z] = 0.0;
  mat[W][W] = 1.0;
}
#endif

