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


/**
 * @brief This class performs calculations on a Quaternion or pair of Quaternions. The class is templated on the type
 * of primitive used and there are a few convenience typedefs for float and double Quaternions. Inside the class definition
 * is a typedef for the Quaternion struct itself. If you want to create a Quaternion there are a few ways to do it.
 * @code
 *  // As a normal stack allocated variable
 *  QuaternionMath<float>::Quaternion quat;
 *  quat.x = 0, quat.y = 0, quat.z = 0, quat.w = 1;
 *  // Inline using the static convenience constructor which initializes the quaternion with the arguments to the "New" method.
 *  QuaternionMath<float>::Quaternion quat = QuaternionMath::New(1, 2, 3, 4);
 * @endcode
 *
 * The memory layout for the quaternion is such that x, y, z, w is the order in memory.
 * @author Michael A. Jackson
 * @version 1.0
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
    /**
     * @brief Identity Sets the quaternion q to the identity quaternion (<0,0,0>,1)
     * @param q
     */
    static void Identity(Quaternion &q)
    {
      q.x = 0.0;
      q.y = 0.0;
      q.z = 0.0;
      q.w = 1.0;
    }

    // -----------------------------------------------------------------------------
    /**
     * @brief Copy Copies the values from src into dest
     * @param src
     * @param dest
     */
    static void Copy(const Quaternion &src, Quaternion &dest)
    {
      dest.x = src.x;
      dest.y = src.y;
      dest.z = src.z;
      dest.w = src.w;
    }

    // -----------------------------------------------------------------------------

    /**
     * @brief ElementWiseAbs inline assigns the absolute value of each element to itself
     * @param q
     */
    static void ElementWiseAbs(Quaternion &q)
    {
      q.x = fabs(q.x);
      q.y = fabs(q.y);
      q.z = fabs(q.z);
      q.w = fabs(q.w);
    }

    /**
     * @brief ScalarMultiply Multiplies each element in the quaternion by the argument v
     * @param q
     * @param v
     */
    static void ScalarMultiply(Quaternion &q, T v)
    {
      q.x = q.x * v;
      q.y = q.y * v;
      q.z = q.z * v;
      q.w = q.w * v;
    }

    /**
     * @brief ScalarDivide Divides each element in the quaternion by the argument v
     * @param q
     * @param v
     */
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
     * @brief Multiply Multiplies 2 quaternions according to quaternion multiplication. Note that the order of the quaternions
     * is critical as Quaternioin multiplication is NOT cummunitive i.e., A * B != B * A
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
     * @brief Conjugate Converts quaternion q into its conjugate.
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
     * @brief Norm Computes and returns the "norm" of the quaternion
     * @param q
     */
    static T Norm(Quaternion &q)
    {
      return q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w;
    }


    /**
     * @brief Length Computes are returns the "length" of the quaternion which is the square root of the norm.
     * @param q
     */
    static T Length(Quaternion &q)
    {
      return sqrt(q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w);
    }

    // -----------------------------------------------------------------------------
    /**
    * @brief UnitQuaternion (Normalize) Converts the quaternion into its normalized values
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

/**
 * @brief QuaternionMathD Typedef for 64 bit floats for convenience
 */
typedef QuaternionMath<double> QuaternionMathD;

/**
 * @brief QuatF 32 Bit Floating point Quaternion for convenience.
 */
typedef QuaternionMath<float>::Quaternion  QuatF;
/**
 * @brief QuatD 64 Bit Floating point Quaternion for convenience.
 */
 typedef QuaternionMath<double>::Quaternion  QuatD;


#endif /* _QuaternionMath_H_ */

