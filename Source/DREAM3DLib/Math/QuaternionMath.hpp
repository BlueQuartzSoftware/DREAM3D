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
#include "DREAM3DLib/Common/DREAM3DMath.h"

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
//    static void Copy(const Quaternion_t &src, Quaternion_t &dest)
//    {
//      dest.quat.x = src.quat.x;
//      dest.quat.y = src.quat.y;
//      dest.quat.z = src.quat.z;
//      dest.quat.w = src.quat.w;
//    }
    static void Copy(const Quaternion &src, Quaternion &dest)
    {
      dest.x = src.x;
      dest.y = src.y;
      dest.z = src.z;
      dest.w = src.w;
    }
//    static void Copy(const Vec4_t &src, Vec4_t &dest)
//    {
//      dest.vec4[0] = src.vec4[0];
//      dest.vec4[1] = src.vec4[1];
//      dest.vec4[2] = src.vec4[2];
//      dest.vec4[3] = src.vec4[3];
//    }
//    static void Copy(const T* src, T* dest)
//    {
//      dest[0] = src[0];
//      dest[1] = src[1];
//      dest[2] = src[2];
//      dest[3] = src[3];
//    }

    // -----------------------------------------------------------------------------
    static void ElementWiseAbs(Quaternion &q)
    {
      q.x = abs(q.x);
      q.y = abs(q.y);
      q.z = abs(q.z);
      q.w = abs(q.w);
    }

    static void ElementWiseMultiply(Quaternion &q, T v)
    {
      q.x = q.x * v;
      q.y = q.y * v;
      q.z = q.z * v;
      q.w = q.w * v;
    }

    static void ElementWiseDivide(Quaternion &q, T v)
    {
      q.x = q.x / v;
      q.y = q.y / v;
      q.z = q.z / v;
      q.w = q.w / v;
    }

    static void ElementWiseAssign(Quaternion &q, T v)
    {
      q.x = v;
      q.y = v;
      q.z = v;
      q.w = v;
    }

    /**
     * @brief ElementWiseAdd Adds each element in v to q and stores the results in q
     * @param q Quat that has values stored in it
     * @param v Input Quat to add elements
     */
    static void ElementWiseAdd(Quaternion &q, T v)
    {
      q.x = q.x + v;
      q.y = q.y + v;
      q.z = q.z + v;
      q.w = q.w + v;
    }


    // -----------------------------------------------------------------------------

    /**
     * @brief Multiply
     * @param q1
     * @param q2
     * @param out
     */
//    static void Multiply(const Quaternion_t &q1, const Quaternion_t &q2, Quaternion_t &out)
//    {
//      out.quat.x = q2.quat.x * q1.quat.w + q2.quat.w * q1.quat.x + q2.quat.z * q1.quat.y - q2.quat.y * q1.quat.z;
//      out.quat.y = q2.quat.y * q1.quat.w + q2.quat.w * q1.quat.y + q2.quat.x * q1.quat.z - q2.quat.z * q1.quat.x;
//      out.quat.z = q2.quat.z * q1.quat.w + q2.quat.w * q1.quat.z + q2.quat.y * q1.quat.x - q2.quat.x * q1.quat.y;
//      out.quat.w = q2.quat.w * q1.quat.w - q2.quat.x * q1.quat.x - q2.quat.y * q1.quat.y - q2.quat.z * q1.quat.z;
//    }

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

    /**
     * @brief Multiply
     * @param q1
     * @param q2
     * @param out
     */
//    static void Multiply(const Vec4_t &q1, const Vec4_t &q2, Vec4_t &out)
//    {
//      out.d[0] = q2.d[0] * q1.d[3] + q2.d[3] * q1.d[0] + q2.d[2] * q1.d[1] - q2.d[1] * q1.d[2];
//      out.d[1] = q2.d[1] * q1.d[3] + q2.d[3] * q1.d[1] + q2.d[0] * q1.d[2] - q2.d[2] * q1.d[0];
//      out.d[2] = q2.d[2] * q1.d[3] + q2.d[3] * q1.d[2] + q2.d[1] * q1.d[0] - q2.d[0] * q1.d[1];
//      out.d[3] = q2.d[3] * q1.d[3] - q2.d[0] * q1.d[0] - q2.d[1] * q1.d[1] - q2.d[2] * q1.d[2];
//    }

    /**
     * @brief Multiply
     * @param q1
     * @param q2
     * @param out
     */
//    static void Multiply(const T* q1, const T* q2, T* out)
//    {
//      out[0] = q2[0] * q1[3] + q2[3] * q1[0] + q2[2] * q1[1] - q2[1] * q1[2];
//      out[1] = q2[1] * q1[3] + q2[3] * q1[1] + q2[0] * q1[2] - q2[2] * q1[0];
//      out[2] = q2[2] * q1[3] + q2[3] * q1[2] + q2[1] * q1[0] - q2[0] * q1[1];
//      out[3] = q2[3] * q1[3] - q2[0] * q1[0] - q2[1] * q1[1] - q2[2] * q1[2];
//    }


    // -----------------------------------------------------------------------------
    /**
     * @brief InvertQuaternion
     * @param q
     */
//    static void InvertQuaternion(Quaternion_t &q)
//    {
//      q.quat.x = -q.quat.x;
//      q.quat.y = -q.quat.y;
//      q.quat.z = -q.quat.z;
//      q.quat.w = q.quat.w;
//    }

    static void InvertQuaternion(Quaternion &q)
    {
      q.x = -q.x;
      q.y = -q.y;
      q.z = -q.z;
      q.w = q.w;
    }

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


    // -----------------------------------------------------------------------------
    /**
    * @brief Normalize
    * @param qr
    */
//    static void Normalize(Quaternion_t &qr)
//    {
//      double norm = (qr.quat.x*qr.quat.x) + (qr.quat.y*qr.quat.y) + (qr.quat.z*qr.quat.z) + (qr.quat.w*qr.quat.w);
//      norm = sqrt(norm);
//      qr.quat.x = static_cast<T>( qr.quat.x/norm );
//      qr.quat.y = static_cast<T>( qr.quat.y/norm );
//      qr.quat.z = static_cast<T>( qr.quat.z/norm );
//      qr.quat.w = static_cast<T>( qr.quat.w/norm );
//    }

    static void Normalize(Quaternion &qr)
    {
      double norm = (qr.x*qr.x) + (qr.y*qr.y) + (qr.z*qr.z) + (qr.w*qr.w);
      norm = sqrt(norm);
      qr.x = static_cast<T>( qr.x/norm );
      qr.y = static_cast<T>( qr.y/norm );
      qr.z = static_cast<T>( qr.z/norm );
      qr.w = static_cast<T>( qr.w/norm );
    }

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
