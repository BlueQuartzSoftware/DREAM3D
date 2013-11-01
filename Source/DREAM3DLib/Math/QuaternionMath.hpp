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
#ifndef _QuaternionMath_H_
#define _QuaternionMath_H_

#include <stdlib.h>
#include "DREAM3DLib/Math/DREAM3DMath.h"

/**
 * @brief
 * @author
 * @version
 */
template<typename T>
class QuaternionMath
{
  public:
    QuaternionMath();
    virtual ~QuaternionMath();


    // -----------------------------------------------------------------------------
    /**
    * @brief
    */
    typedef struct {
        T x;
        T y;
        T z;
        T w;
    } Quaternion;

    /**
    * @brief
    */
//    typedef struct {
//        T d[4];
//    } Vec4_t;

    /**
    * @brief The Union of the 2 structures
    */
//    typedef union {
//        Quaternion quat;
//        Vec4_t vec4;
//    } Quaternion_t;

    // -----------------------------------------------------------------------------

    static Quaternion New(T x, T y, T z, T w)
    {
      Quaternion q;
      q.x = x;
      q.y = y;
      q.z = z;
      q.w = w;
      return q;
    }

    // -----------------------------------------------------------------------------
    static void Identity(Quaternion &q)
    {
      q.x = 0.0;
      q.y = 0.0;
      q.z = 0.0;
      q.w = 1.0;
    }

    // -----------------------------------------------------------------------------

    static void Copy(const Quaternion &src, Quaternion &dest)
    {
      dest.x = src.x;
      dest.y = src.y;
      dest.z = src.z;
      dest.w = src.w;
    }

    // -----------------------------------------------------------------------------
    static void ElementWiseAbs(Quaternion &q)
    {
      q.x = fabs(q.x);
      q.y = fabs(q.y);
      q.z = fabs(q.z);
      q.w = fabs(q.w);
    }

    static void ScalarMultiply(Quaternion &q, T v)
    {
      q.x = q.x * v;
      q.y = q.y * v;
      q.z = q.z * v;
      q.w = q.w * v;
    }

    static void ScalarDivide(Quaternion &q, T v)
    {
      q.x = q.x / v;
      q.y = q.y / v;
      q.z = q.z / v;
      q.w = q.w / v;
    }


    /**
     * @brief ScalarAdd Adds value to each element of the vector and scalar part of the Quaternion
     * @param q Quat that has values stored in it
     * @param v Input Quat to add elements
     */
    static void ScalarAdd(Quaternion &q, T v)
    {
      q.x = q.x + v;
      q.y = q.y + v;
      q.z = q.z + v;
      q.w = q.w + v;
    }

    /**
     * @brief ElementWiseAdd Adds each element in v to q and stores the results in q
     * @param q Quat that has values stored in it
     * @param v Input Quat to add elements
     */

    static void ElementWiseAssign(Quaternion &q, T v)
    {
      q.x = v;
      q.y = v;
      q.z = v;
      q.w = v;
    }

    // ----------------------------------------------------------------------------

    /**
     * @brief Negate  −q = (−a, −v)
     * @param q
     */
    static void Negate(Quaternion &q)
    {
      q.x = -q.x;
      q.y = -q.y;
      q.z = -q.z;
      q.w = -q.w;
    }


    // -----------------------------------------------------------------------------

    /**
     * @brief Add   q1 + q2 = (w1+w2, v1+v2)
     * @param q1
     * @param q2
     * @param out
     */
    static void Add(const Quaternion &q1, const Quaternion &q2, Quaternion &out)
    {
      out.x = q2.x + q1.x;
      out.y = q2.y + q1.y;
      out.z = q2.z + q1.z;
      out.w = q2.w + q1.w;
    }

    // -----------------------------------------------------------------------------

    /**
     * @brief Add   q1 + q2 = (w1+w2, v1+v2)
     * @param q1
     * @param q2
     * @param out
     */
    static void Subtract(const Quaternion &q1, const Quaternion &q2, Quaternion &out)
    {
      out.x = q2.x - q1.x;
      out.y = q2.y - q1.y;
      out.z = q2.z - q1.z;
      out.w = q2.w - q1.w;
    }

    // -----------------------------------------------------------------------------

    /**
     * @brief Multiply
     * @param q1
     * @param q2
     * @param out
     */
    static void Multiply(const Quaternion &q1, const Quaternion &q2, Quaternion &out)
    {
      out.x = q2.x * q1.w + q2.w * q1.x + q2.z * q1.y - q2.y * q1.z;
      out.y = q2.y * q1.w + q2.w * q1.y + q2.x * q1.z - q2.z * q1.x;
      out.z = q2.z * q1.w + q2.w * q1.z + q2.y * q1.x - q2.x * q1.y;
      out.w = q2.w * q1.w - q2.x * q1.x - q2.y * q1.y - q2.z * q1.z;
    }

    // -----------------------------------------------------------------------------

    /**
     * @brief Conjugate
     * @param q
     */
    static void Conjugate(Quaternion &q)
    {
      q.x = -q.x;
      q.y = -q.y;
      q.z = -q.z;
      q.w = q.w;
    }

    // -----------------------------------------------------------------------------

    /**
     * @brief Norm
     * @param q
     */
    static T Norm(Quaternion &q)
    {
      return q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w;
    }


    /**
     * @brief Length
     * @param q
     */
    static T Length(Quaternion &q)
    {
      return sqrt(q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w);
    }

    // -----------------------------------------------------------------------------
    /**
    * @brief UnitQuaternion (Normalize)
    * @param qr
    */
    static void UnitQuaternion(Quaternion &qr)
    {
      double length = Length(qr);
      qr.x = static_cast<T>( qr.x/length );
      qr.y = static_cast<T>( qr.y/length );
      qr.z = static_cast<T>( qr.z/length );
      qr.w = static_cast<T>( qr.w/length );
    }


  protected:


  private:
    QuaternionMath(const QuaternionMath&); // Copy Constructor Not Implemented
    void operator=(const QuaternionMath&); // Operator '=' Not Implemented
};

/**
 * @brief QuaternionMathF Typedef for 32 bit floats for convenience
 */
typedef QuaternionMath<float> QuaternionMathF;
typedef QuaternionMath<double> QuaternionMathD;

/**
 * @brief QuatF 32 Bit Floating point Quaternion for convenience.
 */
typedef QuaternionMath<float>::Quaternion  QuatF;
typedef QuaternionMath<double>::Quaternion  QuatD;


#endif /* _QuaternionMath_H_ */

//    static void Multiply(const Quaternion_t &q1, const Quaternion_t &q2, Quaternion_t &out)
//    {
//      out.quat.x = q2.quat.x * q1.quat.w + q2.quat.w * q1.quat.x + q2.quat.z * q1.quat.y - q2.quat.y * q1.quat.z;
//      out.quat.y = q2.quat.y * q1.quat.w + q2.quat.w * q1.quat.y + q2.quat.x * q1.quat.z - q2.quat.z * q1.quat.x;
//      out.quat.z = q2.quat.z * q1.quat.w + q2.quat.w * q1.quat.z + q2.quat.y * q1.quat.x - q2.quat.x * q1.quat.y;
//      out.quat.w = q2.quat.w * q1.quat.w - q2.quat.x * q1.quat.x - q2.quat.y * q1.quat.y - q2.quat.z * q1.quat.z;
//    }
//    static void Multiply(const Vec4_t &q1, const Vec4_t &q2, Vec4_t &out)
//    {
//      out.d[0] = q2.d[0] * q1.d[3] + q2.d[3] * q1.d[0] + q2.d[2] * q1.d[1] - q2.d[1] * q1.d[2];
//      out.d[1] = q2.d[1] * q1.d[3] + q2.d[3] * q1.d[1] + q2.d[0] * q1.d[2] - q2.d[2] * q1.d[0];
//      out.d[2] = q2.d[2] * q1.d[3] + q2.d[3] * q1.d[2] + q2.d[1] * q1.d[0] - q2.d[0] * q1.d[1];
//      out.d[3] = q2.d[3] * q1.d[3] - q2.d[0] * q1.d[0] - q2.d[1] * q1.d[1] - q2.d[2] * q1.d[2];
//    }
//    static void Multiply(const T* q1, const T* q2, T* out)
//    {
//      out[0] = q2[0] * q1[3] + q2[3] * q1[0] + q2[2] * q1[1] - q2[1] * q1[2];
//      out[1] = q2[1] * q1[3] + q2[3] * q1[1] + q2[0] * q1[2] - q2[2] * q1[0];
//      out[2] = q2[2] * q1[3] + q2[3] * q1[2] + q2[1] * q1[0] - q2[0] * q1[1];
//      out[3] = q2[3] * q1[3] - q2[0] * q1[0] - q2[1] * q1[1] - q2[2] * q1[2];
//    }

//    static void InvertQuaternion(Quaternion_t &q)
//    {
//      q.quat.x = -q.quat.x;
//      q.quat.y = -q.quat.y;
//      q.quat.z = -q.quat.z;
//      q.quat.w = q.quat.w;
//    }
//    static void InvertQuaternion(Vec4_t &q)
//    {
//      q.d[0] = -q.d[0];
//      q.d[1] = -q.d[1];
//      q.d[2] = -q.d[2];
//      q.d[3] = q.d[3];
//    }

//    static void InvertQuaternion(T* q)
//    {
//      q[0] = -q[0];
//      q[1] = -q[1];
//      q[2] = -q[2];
//      q[3] = q[3];
//    }
//    static void Normalize(Quaternion_t &qr)
//    {
//      double norm = (qr.quat.x*qr.quat.x) + (qr.quat.y*qr.quat.y) + (qr.quat.z*qr.quat.z) + (qr.quat.w*qr.quat.w);
//      norm = sqrt(norm);
//      qr.quat.x = static_cast<T>( qr.quat.x/norm );
//      qr.quat.y = static_cast<T>( qr.quat.y/norm );
//      qr.quat.z = static_cast<T>( qr.quat.z/norm );
//      qr.quat.w = static_cast<T>( qr.quat.w/norm );
//    }
//    static void Normalize(Vec4_t &qr)
//    {
//      double norm = (qr[0]*qr[0]) + (qr[1]*qr[1]) + (qr[2]*qr[2]) + (qr[3]*qr[3]);
//      norm = sqrt(norm);
//      qr.d[0] = static_cast<T>( qr.d[0]/norm );
//      qr.d[1] = static_cast<T>( qr.d[1]/norm );
//      qr.d[2] = static_cast<T>( qr.d[2]/norm );
//      qr.d[3] = static_cast<T>( qr.d[3]/norm );
//    }

//    static void Normalize(T* qr)
//    {
//      double norm = (qr[0]*qr[0]) + (qr[1]*qr[1]) + (qr[2]*qr[2]) + (qr[3]*qr[3]);
//      norm = sqrt(norm);
//      qr[1] = static_cast<T>( qr[1]/norm );
//      qr[2] = static_cast<T>( qr[2]/norm );
//      qr[3] = static_cast<T>( qr[3]/norm );
//      qr[4] = static_cast<T>( qr[4]/norm );
//    }
