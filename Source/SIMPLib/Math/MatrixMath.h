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


#ifndef _MatrixMath_H_
#define _MatrixMath_H_


#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"


/*
 * @class MatrixMath MatrixMath.h DREAM3DLib/Common/MatrixMath.h
 * @brief This class performs Crystallographic Misorientation Calculations
 * @author Michael A. Jackson (BlueQuartz Software)
 * @author Michael A. Groeber (US Air Force Research Laboratory)
 * @author Joseph C. Tucker (UES, Inc.)
 * @date Jan 30, 2014
 * @version 5.0
 */
class SIMPLib_EXPORT MatrixMath
{
  public:
    SIMPL_SHARED_POINTERS(MatrixMath)
    SIMPL_TYPE_MACRO(MatrixMath)

    virtual ~MatrixMath();

    /**
     * @brief Performs the Matrix Multiplication of g1 and g2 and puts the result into outMat. (Single Precision version)
     * @param g1
     * @param g2
     * @param outMat
     */
    static void Multiply3x3with3x3(float g1[3][3], float g2[3][3], float outMat[3][3]);

    /**
     * @brief Performs the Matrix Multiplication of g1 and g2 and puts the result into outMat. (Single Precision version)
     * @param g1
     * @param g2
     * @param outMat
     */
    static void Multiply3x3with3x1(const float g1[3][3], float g2[3], float outMat[3]);

    /**
     * @brief Performs the Matrix Multiplication of g1 and g2 and puts the result into outMat. (Double Precision Version)
     * @param g1
     * @param g2
     * @param outMat
     */
    static void Multiply3x3with3x1(double g1[3][3], double g2[3], double outMat[3]);

    /**
     * @brief Performs the Matrix Addition of g1 and g2 and puts the result into outMat. (Single Precision version)
     * @param g1
     * @param g2
     * @param outMat
     */
    static void Add3x1s(const float g1[3], float g2[3], float outMat[3]);

    /**
     * @brief Performs the Matrix Subtraction of g2 from g1 and puts the result into outMat. (Single Precision version)
     * @param g1
     * @param g2
     * @param outMat
     */
    static void Subtract3x1s(const float g1[3], float g2[3], float outMat[3]);

    /**
     * @brief Performs the Matrix Addition of g1 and g2 and puts the result into outMat. (Single Precision version)
     * @param g1
     * @param g2
     * @param outMat
     */
    static void Add3x3s(const float g1[3][3], float g2[3][3], float outMat[3][3]);

    /**
     * @brief Performs the Matrix Subtraction of g2 from g1 and puts the result into outMat. (Single Precision version)
     * @param g1
     * @param g2
     * @param outMat
     */
    static void Subtract3x3s(const float g1[3][3], float g2[3][3], float outMat[3][3]);

    /**
     * @brief Multiplies each element of a 3x1 matrix by the value v.
     * @param g Input Matrix
     * @param v Value to mutliply each element by.
     */
    static void Multiply3x1withConstant(float g[3], float v);

    /**
     * @brief Multiplies each element of a 3x1 matrix by the value v.
     * @param g Input Matrix
     * @param v Value to mutliply each element by.
     */
    template<typename T>
    static void Divide4x1withConstant(T g[4], T v)
    {
      g[0] = g[0] / v;
      g[1] = g[1] / v;
      g[2] = g[2] / v;
      g[3] = g[3] / v;
    }


    /**
     * @brief Multiplies each element of a 3x1 matrix by the value v.
     * @param g Input Matrix
     * @param v Value to mutliply each element by.
     */
    static void Multiply3x3withConstant(float g[3][3], float v);

    /**
     * @brief Transposes the 3x3 matrix and places the result into outMat
     * @param g
     * @param outMat
     */
    static void Transpose3x3(float g[3][3], float outMat[3][3]);

    /**
     * @brief Inverts the 3x3 matrix and places the result into outMat
     * @param g
     * @param outMat
     */
    static void Invert3x3(float g[3][3], float outMat[3][3]);

    /**
     * @brief Calculates the Adjoint matrix of the 3x3 matrix and places the result into outMat
     * @param g
     * @param outMat
     */
    static void Adjoint3x3(float g[3][3], float outMat[3][3]);

    /**
     * @brief Calculates the cofactor matrix of the 3x3 matrix and places the result into outMat
     * @param g
     * @param outMat
     */
    static void Cofactor3x3(float g[3][3], float outMat[3][3]);

    /**
     * @brief Calculates the matrix of minors of the 3x3 matrix and places the result into outMat
     * @param g
     * @param outMat
     */
    static void Minors3x3(float g[3][3], float outMat[3][3]);

    /**
     * @brief The determinant of a 3x3 matrix
     * @param g 3x3 Vector
     * @return
     */
    static float Determinant3x3(float g[3][3]);

    /**
     * @brief Copies a 3x3 matrix into another 3x3 matrix
     * @param g
     * @param outMat
     */
    static void Copy3x3(float g[3][3], float outMat[3][3]);

    /**
     * @brief Copies a 3x1 matrix into another 3x1 matrix
     * @param g
     * @param outMat
     */
    static void Copy3x1(float g[3], float outMat[3]);

    /**
     * @brief Initializes the 3x3 matrix to the "Identity" matrix
     * @param g
     */
    static void Identity3x3(float g[3][3]);

    /**
     * @brief Performs an "in place" normalization of the 3x1 vector.
     * @param g
     */
    static void Normalize3x3(float g[3][3]);

    /**
     * @brief Performs an "in place" normalization of the 3x1 vector. Single Precision Variant
     * @param g
     */
    static void Normalize3x1(float g[3]);

    /**
     * @brief Performs an "in place" sort of the 3x1 vector in ascending order. Single Precision Variant
     * @param g
     */
    static void Sort3x1Ascending(float g[3]);

    /**
     * @brief Returns index of maximum value. Single Precision Variant
     * @param g
     */
    static int FindIndexOfMaxVal3x1(float g[3]);

    /**
    * @brief Performs an "in place" normalization of the 3x1 vector. Double Precision Variant
    * @param g
    */
    static void Normalize3x1(double g[3]);

    /**
     * @brief Performs an "in place" normalization of the 3x1 vector. Double Precision Variant
     * @param i
     * @param j
     * @param k
     */
    static void Normalize3x1(double& i, double& j, double& k);

    /**
     * @brief Performs an "in place" normalization of the 3x1 vector. Single Precision Variant
     * @param i
     * @param j
     * @param k
     */
    static void Normalize3x1(float& i, float& j, float& k);

    /**
     * @brief Returns the magnitude of the 3x1 vector. Double Precision Variant
     * @param a
     */
    template<typename T>
    static T Magnitude3x1(T a[3])
    {
      return (sqrt((a[0] * a[0]) + (a[1] * a[1]) + (a[2] * a[2])));
    }

    /**
     * @brief Returns the magnitude of the 4x1 vector. Single Precision Variant
     * @param a
     */
    static float Magnitude4x1(float a[4]);

    /**
     * @brief Returns the magnitude of the 4x1 vector. Single Precision Variant
     * @param a
     */
    static double Magnitude4x1(double a[4]);

    /**
     * @brief The dot product of 2 vectors a & b. Double Precision Variant
     * @param a 1x3 Vector
     * @param b 1x3 Vector
     * @return
     */
    static float DotProduct3x1(double a[3], double b[3]);

    /**
     * @brief The dot product of 2 vectors a & b. Single Precision Variant
     * @param a 1x3 Vector
     * @param b 1x3 Vector
     * @return
     */
    static float DotProduct3x1(float a[3], float b[3]);

    /**
     * @brief Performs a Cross Product of "a into b" and places the result into c. Double Precision Variant
     * A X B = C
     * @param a
     * @param b
     * @param c
     */
    static void CrossProduct(double a[3], double b[3], double c[3]);

    /**
     * @brief Performs a Cross Product of "a into b" and places the result into c. Single Precision Variant
     * A X B = C
     * @param a
     * @param b
     * @param c
     */
    static void CrossProduct(float a[3], float b[3], float c[3]);


  protected:
    MatrixMath();

  private:
    MatrixMath(const MatrixMath&); // Copy Constructor Not Implemented
    void operator=(const MatrixMath&); // Operator '=' Not Implemented
};

#endif /* _MatrixMath_H_ */

