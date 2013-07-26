/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2010, Michael A. Groeber (US Air Force Research Laboratory)
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
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#ifndef _OrientationMath_H_
#define _OrientationMath_H_

#include <vector>

#include "MXA/Common/LogTime.h"

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Common/DREAM3DRandom.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/Common/DREAM3DMath.h"
#include "DREAM3DLib/Math/QuaternionMath.hpp"
#include "DREAM3DLib/Math/MatrixMath.hpp"


// Here the order of multiplication is q1*q2 not q2*q1
#if 0
#define MULT_QUAT(q1, q2, out)\
  out[1] = q2[1] * q1[4] + q2[4] * q1[1] + q2[3] * q1[2] - q2[2] * q1[3];\
  out[2] = q2[2] * q1[4] + q2[4] * q1[2] + q2[1] * q1[3] - q2[3] * q1[1];\
  out[3] = q2[3] * q1[4] + q2[4] * q1[3] + q2[2] * q1[1] - q2[1] * q1[2];\
  out[4] = q2[4] * q1[4] - q2[1] * q1[1] - q2[2] * q1[2] - q2[3] * q1[3];\

#endif

/*
 * @class OrientationMath OrientationMath.h DREAM3DLib/Common/OrientationMath.h
 * @brief This class performs Crystallographic Misorientation Calculations
 * @author Michael A. Jackson (BlueQuartz Software)
 * @author Michael A. Groeber (US Air Force Research Laboratory)
 * @date Feb 19, 2011
 * @version 1.0
 */
template<typename T>
class OrientationMath
{
  public:

    virtual ~OrientationMath(){}



    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    static void AxisAngletoHomochoric(T w, T n1, T n2, T n3, T &r1, T &r2, T &r3)
    {
      T denom;

      denom = (n1 * n1) + (n2 * n2) + (n3 * n3);
      denom = sqrt(denom);
      n1 = n1 / denom;
      n2 = n2 / denom;
      n3 = n3 / denom;
      if(denom == 0.0) n1 = 0.0, n2 = 0.0, n3 = 1.0;
      r1 = n1 * powf(((0.75f) * (w - sinf(w))), (1.0f / 3.0f));
      r2 = n2 * powf(((0.75f) * (w - sinf(w))), (1.0f / 3.0f));
      r3 = n3 * powf(((0.75f) * (w - sinf(w))), (1.0f / 3.0f));
    }

    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    static void AxisAngletoRod(T w, T n1, T n2, T n3, T &r1, T &r2, T &r3)
    {
      T denom;

      denom = (n1 * n1) + (n2 * n2) + (n3 * n3);
      denom = sqrt(denom);
      n1 = n1 / denom;
      n2 = n2 / denom;
      n3 = n3 / denom;
      if(denom == 0.0) n1 = 0.0f, n2 = 0.0f, n3 = 1.0f;
      T const1 = tanf(w/2.0f);
      r1 = n1 * const1;
      r2 = n2 * const1;
      r3 = n3 * const1;
    }

    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    static void AxisAngletoQuat(T w, T n1, T n2, T n3, typename QuaternionMath<T>::Quaternion &q)
    {
      T denom;

      denom = (n1 * n1) + (n2 * n2) + (n3 * n3);
      denom = sqrt(denom);
      n1 = n1 / denom;
      n2 = n2 / denom;
      n3 = n3 / denom;
      if(denom == 0.0) n1 = 0.0f, n2 = 0.0f, n3 = 1.0f;
      T const1 = cosf(w/2.0f);
      T const2 = sinf(w/2.0f);
      q.x = n1 * const2;
      q.y = n2 * const2;
      q.z = n3 * const2;
      q.w = const1;
    }

    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    static void AxisAngletoMat(T w, T n1, T n2, T n3, T g[3][3])
    {
      T c, s, t, denom;

      c = cosf(w);
      s = sinf(w);
      t = 1-c;

      denom = (n1 * n1) + (n2 * n2) + (n3 * n3);
      denom = sqrt(denom);
      n1 = n1 / denom;
      n2 = n2 / denom;
      n3 = n3 / denom;
      if(denom == 0.0) n1 = 0.0f, n2 = 0.0f, n3 = 1.0f;

      g[0][0] = t*n1*n1+c;
      g[0][1] = t*n1*n2-n3*s;
      g[0][2] = t*n1*n3+n2*s;
      g[1][0] = t*n1*n2+n3*s;
      g[1][1] = t*n2*n2+c;
      g[1][2] = t*n2*n3-n1*s;
      g[2][0] = t*n1*n3-n2*s;
      g[2][1] = t*n2*n3+n1*s;
      g[2][2] = t*n3*n3+c;
    }

    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    static void RodtoHomochoric(T &r1, T &r2, T &r3)
    {
      T rmag, w;

      rmag = (r1 * r1) + (r2 * r2) + (r3 * r3);
      rmag = sqrt(rmag);
      r1 = r1 / rmag;
      r2 = r2 / rmag;
      r3 = r3 / rmag;
      if(rmag == 0.0) r1 = 0.0f, r2 = 0.0f, r3 = 0.0f;
      w = static_cast<T>( 2.0*atan(rmag) );
      T const1 = powf(((3.0f / 4.0f) * (w - sinf(w))), (1.0f / 3.0f));
      r1 = r1 * const1;
      r2 = r2 * const1;
      r3 = r3 * const1;
    }

    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    static void HomochorictoRod(T &r1, T &r2, T &r3)
    {
      T hmag, x, w, w_new;

      hmag = (r1 * r1) + (r2 * r2) + (r3 * r3);
      hmag = sqrt(hmag);
      r1 = r1 / hmag;
      r2 = r2 / hmag;
      r3 = r3 / hmag;
      if(hmag == 0.0) r1 = 0.0, r2 = 0.0, r3 = 0.0;
      w = static_cast<T>( powf((8*hmag*hmag*hmag),(1.0f/3.0f)) );
      x = DREAM3D::Constants::k_1Point3*hmag*hmag*hmag;
      for(size_t i = 1; i < 10; i++)
      {
        w_new = w - ((x-w+sin(w))/(-1+cos(w)));
        w = w_new;
      }
      T const1 = tanf(w/2.0f);
      r1 = r1 * const1;
      r2 = r2 * const1;
      r3 = r3 * const1;
    }


    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    static void RodtoAxisAngle(T r1, T r2, T r3, T &w, T &n1, T &n2, T &n3)
    {
      T rmag;

      rmag = (r1 * r1) + (r2 * r2) + (r3 * r3);
      rmag = sqrt(rmag);
      w = static_cast<T>( 2.0*atan(rmag) );
      n1 = r1 / rmag;
      n2 = r2 / rmag;
      n3 = r3 / rmag;
      if(rmag == 0.0) n1 = 0.0f, n2 = 0.0f, n3 = 1.0f;
      if(w > DREAM3D::Constants::k_Pi)
      {
        w = (2*DREAM3D::Constants::k_Pi)-w;
        n1 = -n1;
        n2 = -n2;
        n3 = -n3;
      }
    }


    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    static void QuattoAxisAngle(typename QuaternionMath<T>::Quaternion &q, T &w, T &n1, T &n2, T &n3)
    {
      w = static_cast<T>( 2.0*acos(q.w) );
      n1 = q.x / sqrt(1-(q.w*q.w));
      n2 = q.y / sqrt(1-(q.w*q.w));
      n3 = q.z / sqrt(1-(q.w*q.w));
      if(q.w == 1.0) n1 = 0.0f, n2 = 0.0f, n3 = 1.0f;
      if(w > DREAM3D::Constants::k_Pi)
      {
        w = (2*DREAM3D::Constants::k_Pi)-w;
        n1 = -n1;
        n2 = -n2;
        n3 = -n3;
      }
    }

    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    static void QuattoMat(typename QuaternionMath<T>::Quaternion &q, T g[3][3])
    {
      g[0][0] = (1 - (2 * q.y * q.y) - (2 * q.z * q.z));
      g[0][1] = ((2 * q.x * q.y) + (2 * q.z * q.w));
      g[0][2] = ((2 * q.x * q.z) - (2 * q.y * q.w));
      g[1][0] = ((2 * q.x * q.y) - (2 * q.z * q.w));
      g[1][1] = (1 - (2 * q.x * q.x) - (2 * q.z * q.z));
      g[1][2] = ((2 * q.y * q.z) + (2 * q.x * q.w));
      g[2][0] = ((2 * q.x * q.z) + (2 * q.y * q.w));
      g[2][1] = ((2 * q.y * q.z) - (2 * q.x * q.w));
      g[2][2] = (1 - (2 * q.x * q.x) - (2 * q.y * q.y));
    }

    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    static void RodtoQuat(typename QuaternionMath<T>::Quaternion &q, T r1, T r2, T r3)
    {
      T rmag, w;

      rmag = (r1 * r1) + (r2 * r2) + (r3 * r3);
      rmag = sqrt(rmag);
      r1 = r1 / rmag;
      r2 = r2 / rmag;
      r3 = r3 / rmag;
      if(rmag == 0.0) r1 = 0.0f, r2 = 0.0f, r3 = 0.0f;
      w = 2.0f*atan(rmag);
      T const1 = sinf(w/2.0f);
      q.x = r1 * const1;
      q.y = r2 * const1;
      q.z = r3 * const1;
      q.w = static_cast<T>( cosf(w/2.0f) );
    }

    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    static void QuattoRod(typename QuaternionMath<T>::Quaternion &q, T &r1, T &r2, T &r3)
    {
      T qmag, w;
      T n1, n2, n3;

      qmag = (q.x * q.x) + (q.y * q.y) + (q.z * q.z);
      qmag = sqrt(qmag);
      n1 = q.x / qmag;
      n2 = q.y / qmag;
      n3 = q.z / qmag;
      if(qmag == 0.0) n1 = 0.0f, n2 = 0.0f, n3 = 1.0f;
      w = static_cast<T>( 2.0*acos(q.w) );
      T const1 = tanf(w * 0.5f);
      r1 = n1 * const1;
      r2 = n2 * const1;
      r3 = n3 * const1;
    }

    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    static void QuattoEuler(typename QuaternionMath<T>::Quaternion &q, T &ea1, T &ea2, T &ea3)
    {
      T diff, sum, tmp;
      diff=atan2(-q.y,-q.x);
      sum=atan2(-q.z,-q.w);
      ea1=(diff+sum);
      ea3=(sum-diff);
      tmp=(q.z*q.z)+(q.w*q.w);
      tmp = sqrt(tmp);
      if(tmp > 1.0f) tmp=1.0f;
      ea2=2*acos(tmp);
      ea1=ea1+DREAM3D::Constants::k_2Pi;
      ea3=ea3+DREAM3D::Constants::k_2Pi;
      ea1 = fmodf(ea1,DREAM3D::Constants::k_2Pi);
      ea3 = fmodf(ea3,DREAM3D::Constants::k_2Pi);
    }

    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    static void EulertoQuat(typename QuaternionMath<T>::Quaternion &q, T e1, T e2, T e3)
    {
      T s, c, s1, c1, s2, c2;
      s = sinf(0.5f * e2);
      c = cosf(0.5f * e2);
      s1 = sinf(0.5f * (e1 - e3));
      c1 = cosf(0.5f * (e1 - e3));
      s2 = sinf(0.5f * (e1 + e3));
      c2 = cosf(0.5f * (e1 + e3));
      q.x = s*c1;
      q.y = s*s1;
      q.z = c*s2;
      q.w = c*c2;
    }


    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    static void EulertoMat(T ea1, T ea2, T ea3, T g[3][3])
    {
      // Calcuate all the values once
      T cos_phi1 = cosf(ea1);
      T sin_phi1 = sinf(ea1);
      T cos_phi = cosf(ea2);
      T sin_phi = sinf(ea2);
      T cos_phi2 = cosf(ea3);
      T sin_phi2 = sinf(ea3);

      // 1) find rotation matrix from Euler angles
      g[0][0] = cos_phi1 * cos_phi2 - sin_phi1 * sin_phi2 * cos_phi;
      g[0][1] = sin_phi1 * cos_phi2 + cos_phi1 * sin_phi2 * cos_phi;
      g[0][2] = sin_phi2 * sin_phi;
      g[1][0] = -cos_phi1 * sin_phi2 - sin_phi1 * cos_phi2 * cos_phi;
      g[1][1] = -sin_phi1 * sin_phi2 + cos_phi1 * cos_phi2 * cos_phi;
      g[1][2] = cos_phi2 * sin_phi;
      g[2][0] = sin_phi1 * sin_phi;
      g[2][1] = -cos_phi1 * sin_phi;
      g[2][2] = cos_phi;
    }

    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    static void MattoEuler(T g[3][3], T &ea1, T &ea2, T &ea3)
    {
      ea2 = acos(g[2][2]);
      T sin_ea2 = sin(ea2);
      T cosine3 = (g[1][2]/sin_ea2);
      T sine3 = (g[0][2]/sin_ea2);
      T cosine1 = (-g[2][1]/sin_ea2);
      T sine1 = (g[2][0]/sin_ea2);
      ea3 = acos(cosine3);
      ea1 = acos(cosine1);
      if(sine3 < 0) ea3 = (2.0f * DREAM3D::Constants::k_Pi)-ea3;
      if(sine1 < 0) ea1 = (2.0f * DREAM3D::Constants::k_Pi)-ea1;
    }

    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    static void EulertoRod(T &r1, T &r2, T &r3, T ea1, T ea2, T ea3)
    {
      T sum, diff, csum, cdiff, sdiff, t2;
      sum = (ea1+ea3)/2.0f;
      diff = (ea1-ea3)/2.0f;
      csum = cosf(sum);
      cdiff = cosf(diff);
      sdiff = sinf(diff);
      t2 = tanf(ea2/2.0f);
      r1 = t2*cdiff/csum;
      r2 = t2*sdiff/csum;
      r3 = tanf(sum);
    }

    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    static void RodtoEuler(T r1, T r2, T r3, T &ea1, T &ea2, T &ea3)
    {
      T sum = atan(r3);
      T diff = atan(r2 / r1);
      ea1 = sum + diff;
      ea2 = static_cast<T>( 2. * atan(r1 * cosf(sum) / cosf(diff)) );
      ea3 = sum - diff;
    }


    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    static void MultiplyQuaternionVector(typename QuaternionMath<T>::Quaternion &inQuat, T* inVec, T* outVec)
    {
      T g[3][3];
      QuattoMat(inQuat, g);
      MatrixMath<T>::Multiply3x3with3x1(g, inVec, outVec);
    }


    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    static T MatrixMisorientation(T g1[3][3], T g2[3][3])
    {
      T deltaG[3][3];
      //only need to calculate diagonal terms to allow for trace calculation
      deltaG[0][0] = g1[0][0]*g2[0][0] + g1[1][0]*g2[1][0] + g1[2][0]*g2[2][0];
      deltaG[1][1] = g1[0][1]*g2[0][1] + g1[1][1]*g2[1][1] + g1[2][1]*g2[2][1];
      deltaG[2][2] = g1[0][2]*g2[0][2] + g1[1][2]*g2[1][2] + g1[2][2]*g2[2][2];
      T value = ((deltaG[0][0]+deltaG[1][1]+deltaG[2][2])-1.0)/2.0;
      if(value > 1.0) value = 1.0;
      if(value < -1.0) value = -1.0;
      return acosf(value);
    }

    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    static void ChangeAxisReferenceFrame(typename QuaternionMath<T>::Quaternion &q, T &n1, T &n2, T &n3)
    {
      T g[3][3];
      T n[3];
      T nNew[3];

      n[0] = n1;
      n[1] = n2;
      n[2] = n3;

      QuattoMat(q, g);
      MatrixMath<T>::Multiply3x3with3x1(g, n, nNew);
      MatrixMath<T>::Normalize3x1(nNew);
      n1 = nNew[0];
      n2 = nNew[1];
      n3 = nNew[2];
    }

  protected:
    OrientationMath();


  private:
    OrientationMath(const OrientationMath&); // Copy Constructor Not Implemented
    void operator=(const OrientationMath&); // Operator '=' Not Implemented
};

#endif /* _OrientationMath_H_ */
