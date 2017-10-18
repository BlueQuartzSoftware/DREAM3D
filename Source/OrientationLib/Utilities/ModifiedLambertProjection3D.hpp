/* ============================================================================
 * Copyright (c) 2015 BlueQuartz Software, LLC
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
 * Neither the names of any of the BlueQuartz Software contributors
 * may be used to endorse or promote products derived from this software without
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
 *
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#ifndef _ModifiedLamberProjection3D_H_
#define _ModifiedLamberProjection3D_H_

#include <assert.h>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/Math/ArrayHelpers.hpp"

#include "OrientationLib/OrientationLib.h"
#include "OrientationLib/OrientationLibConstants.h"

/**
 * @brief The ModifiedLambertProjection3D class provides methods to convert between
 * a 3D Lambert Cube and a Sphere (XYZ) coordinates.
 */
template<typename T, typename K>
class ModifiedLambertProjection3D
{
  public:
    SIMPL_SHARED_POINTERS(ModifiedLambertProjection3D)
    SIMPL_STATIC_NEW_MACRO(ModifiedLambertProjection3D)
    SIMPL_TYPE_MACRO(ModifiedLambertProjection3D)

    virtual ~ModifiedLambertProjection3D();

    typedef ArrayHelpers<T, K> OMHelperType;

    /**
     * @brief GetPyramid
     * @param xyz
     * @return
     */
    static int GetPyramid(const T& xyz)
    {
      if ((fabs(xyz[0]) <= xyz[2]) && (fabs(xyz[1]) <= xyz[2])) {
        return 1;                        // pyramid 1

      }
      if ((fabs(xyz[0]) <= -xyz[2]) && (fabs(xyz[1]) <= -xyz[2])) {
        return 2;                         // pyramid 2
      }

      if ((fabs(xyz[2]) <= xyz[0]) && (fabs(xyz[1]) <= xyz[0])) {
        return 3;                         // pyramid 3
      }

      if ((fabs(xyz[2]) <= -xyz[0]) && (fabs(xyz[1]) <= -xyz[0])) {
        return 4;                         // pyramid 4
      }


      if ((fabs(xyz[0]) <= xyz[1]) && (fabs(xyz[2]) <= xyz[1])) {
        return 5;                         // pyramid 5
      }

      if ((fabs(xyz[0]) <= -xyz[1]) && (fabs(xyz[2]) <= -xyz[1])) {
        return 6;                        // pyramid 6
      }

      assert(false);

      return 0; // THIS HAD BETTER NOT HAPPEN!!
    }


    /**
     * @brief LambertCubeToBall Converts a 3D Cube Coordinate to a 3D Sphere
     * coordinate.
     * @param xyzin
     * @param ierr
     * @return
     */
    static T LambertCubeToBall(const T& xyzin, int& ierr)
    {
      T XYZ(3), sXYZ(3), LamXYZ(3);
      K T1, T2, c, s, q;
      int p = 0;
      T res(3);

      ierr = 0;
      if (OMHelperType::maxval(OMHelperType::absValue(xyzin)) > ((LPs::ap/2.0)  + 1.0E-8 ))
      {
        OMHelperType::splat(res, 0.0);
        ierr = -1;
        return res;
      }

      // determine which pyramid pair the point lies in and copy coordinates in correct order (see paper)
      p = ModifiedLambertProjection3D<T, K>::GetPyramid(xyzin);

      switch  (p)
      {
        case 1:
        case 2:
          sXYZ = xyzin;
          break;
        case 3:
        case 4:
          sXYZ[0] = xyzin[1]; sXYZ[1] = xyzin[2]; sXYZ[2] = xyzin[0];
          break;
        case 5:
        case 6:
          sXYZ[0] = xyzin[2]; sXYZ[1] = xyzin[0]; sXYZ[2] = xyzin[1];
          break;
        default:
          assert(false);
          break;
      }

      // scale by grid parameter ratio sc
      OMHelperType::scalarMultiply(sXYZ, LPs::sc);
      XYZ = sXYZ;
      // transform to the sphere grid via the curved square, and intercept the zero point
      if (OMHelperType::maxval(OMHelperType::absValue(XYZ)) == 0.0)
      {
        OMHelperType::splat(LamXYZ, 0.0);
      } else {
        T temp(2);
        temp[0] = XYZ[0];
        temp[1] = XYZ[1];
        // intercept all the points along the z-axis
        if (OMHelperType::maxval(OMHelperType::absValue(temp)) == 0.0)
        {
          LamXYZ[0] = 0.0;
          LamXYZ[1] = 0.0;
          LamXYZ[2] = LPs::pref * XYZ[2];
        } else {  // this is a general grid point
          if (fabs(XYZ[1]) <= fabs(XYZ[0]))
          {
            q = LPs::pi12 * XYZ[1]/XYZ[0];
            c = cos(q);
            s = sin(q);
            q = LPs::prek * XYZ[0] / sqrt(LPs::r2-c);
            T1 = (LPs::r2*c - 1.0) * q;
            T2 = LPs::r2 * s * q;
          } else {
            q = LPs::pi12 * XYZ[0]/XYZ[1];
            c = cos(q);
            s = sin(q);
            q = LPs::prek * XYZ[1] / sqrt(LPs::r2-c);
            T1 = LPs::r2 * s * q;
            T2 = (LPs::r2*c - 1.0) * q;
          }

          // transform to sphere grid (inverse Lambert)
          // [note that there is no need to worry about dividing by zero, since XYZ[2] can not become zero]
          c = T1*T1 + T2*T2;
          s = M_PI * c / (24.0 * XYZ[2] * XYZ[2]);
          c = LPs::sPi * c / LPs::r24 / XYZ[2];
          q = sqrt( 1.0 - s );

          LamXYZ[0] = T1 * q;
          LamXYZ[1] = T2 * q;
          LamXYZ[2] = LPs::pref * XYZ[2] - c;
        }
      }


      // reverst the coordinates back to the regular order according to the original pyramid number
      switch (p)
      {
        case 1:
        case 2:
          res = LamXYZ;
          break;
        case 3:
        case 4:
          res[0] = LamXYZ[2]; res[1] = LamXYZ[0]; res[2] = LamXYZ[1];
          break;
        case 5:
        case 6:
          res[0] = LamXYZ[1]; res[1] = LamXYZ[2]; res[2] = LamXYZ[0];
          break;
        default:
          assert(false);
          break;
      }
      return res;
    }



    /**
     * @brief LambertBallToCube
     * @param xyz
     * @param ierr
     * @return
     */
    static T LambertBallToCube(const T& xyz, int& ierr)
    {
      K rs;
      T xyz3(3), xyz2(3), xyz1(3);
      K qxy, q2xy, sq2xy, q, ac, T1inv, T2inv, sx, sy, qx2y, sqx2y, tt;
      int p = 0;
      T res(3);

      ierr = 0;

      rs = sqrt(OMHelperType::sumofSquares(xyz));
      if (rs > LPs::R1)
      {
        OMHelperType::splat(res, 0.0);
        ierr = -1;
        return res;
      }

      if (OMHelperType::maxval(OMHelperType::absValue(xyz))  == 0.0)
      {
        OMHelperType::splat(res, 0.0);
        return res;
      }

      // determine pyramid
      p = ModifiedLambertProjection3D<T, K>::GetPyramid(xyz);

      switch  (p)
      {
        case 1:
        case 2:
          xyz3 = xyz;
          break;
        case 3:
        case 4:
          xyz3[0] = xyz[1]; xyz3[1] = xyz[2]; xyz3[2] = xyz[0];
          break;
        case 5:
        case 6:
          xyz3[0] = xyz[2]; xyz3[1] = xyz[0]; xyz3[2] = xyz[1];
          break;

        default:
          break;
      }

      // inverse M_3
      q = sqrt( 2.0 * rs/(rs+fabs(xyz3[2])) );
      xyz2[0] = xyz3[0] * q;
      xyz2[1] = xyz3[1] * q;
      xyz2[2] = (fabs(xyz3[2])/xyz3[2]) * rs / LPs::pref;

      // inverse M_2
      qxy = xyz2[0]*xyz2[0]+xyz2[1]*xyz2[1];
      sx = 1.0;
      if (xyz2[0] != 0.0)  sx = fabs(xyz2[0])/xyz2[0];
      sy = 1.0;
      if (xyz2[1] != 0.0)  sy = fabs(xyz2[1])/xyz2[1];

      if (qxy != 0.0) {
        if (fabs(xyz2[1]) <= fabs(xyz2[0])) {
          q2xy = qxy + xyz2[0]*xyz2[0];
          sq2xy = sqrt(q2xy);
          q = (LPs::beta/LPs::r2/LPs::R1) * sqrt(q2xy*qxy/(q2xy-fabs(xyz2[0])*sq2xy));
          tt = (xyz2[1]*xyz2[1]+fabs(xyz2[0])*sq2xy)/LPs::r2/qxy;
          if (tt > 1.0) tt = 1.0;
          if (tt < -1.0) tt = -1.0;
          ac = acos(tt);
          T1inv = q * sx;
          T2inv = q * sy * ac/LPs::pi12;
        } else {
          qx2y = qxy + xyz2[1]*xyz2[1];
          sqx2y = sqrt(qx2y);
          q = (LPs::beta/LPs::r2/LPs::R1) * sqrt(qx2y*qxy/(qx2y-fabs(xyz2[1])*sqx2y));
          tt = (xyz2[0]*xyz2[0]+fabs(xyz2[1])*sqx2y)/LPs::r2/qxy;
          if (tt > 1.0) tt = 1.0;
          if (tt < -1.0) tt = -1.0;
          ac = acos(tt);
          T1inv = q * sx * ac/LPs::pi12;
          T2inv = q * sy;
        }
      } else {
        T1inv = 0.0;
        T2inv = 0.0;
      }
      xyz1[0] = T1inv;
      xyz1[1] = T2inv;
      xyz1[2] = xyz2[2];

      // inverse M_1
      OMHelperType::scalarDivide(xyz1, LPs::sc);

      // reverse the coordinates back to the regular order according to the original pyramid number
      switch (p)
      {
        case 1:
        case 2:
          res = xyz1;
          break;
        case 3:
        case 4:
          res[0] = xyz1[2]; res[1] = xyz1[0]; res[2] = xyz1[1];
          break;
        case 5:
        case 6:
          res[0] = xyz1[1]; res[1] = xyz1[2]; res[2] = xyz1[0];
          break;

        default:
          break;
      }
      return res;
    }

  protected:
    ModifiedLambertProjection3D();

  private:
    ModifiedLambertProjection3D(const ModifiedLambertProjection3D&); // Copy Constructor Not Implemented
    void operator=(const ModifiedLambertProjection3D&); // Operator '=' Not Implemented
};

#endif /* _ModifiedLamberProjection3D_H_ */
