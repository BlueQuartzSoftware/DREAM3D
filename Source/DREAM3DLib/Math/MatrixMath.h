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

#ifndef _MatrixMath_H_
#define _MatrixMath_H_


#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"


/*
 * @class MatrixMath MatrixMath.h DREAM3DLib/Common/MatrixMath.h
 * @brief This class performs Crystallographic Misorientation Calculations
 * @author Michael A. Jackson (BlueQuartz Software)
 * @author Michael A. Groeber (US Air Force Research Laboratory)
 * @date Feb 19, 2011
 * @version 1.0
 */
class DREAM3DLib_EXPORT MatrixMath
{
  public:
    DREAM3D_SHARED_POINTERS(MatrixMath)
    DREAM3D_TYPE_MACRO(MatrixMath)

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
    static void Multiply3x3withConstant(float g[3][3], float v);

    /**
     * @brief Transposes the 3x3 matrix and places the result into outMat
     * @param g
     * @param outMat
     */
    static void Transpose3x3(float g[3][3], float outMat[3][3]);

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
     * @brief returns index of maximum value. Single Precision Variant
     * @param g
     */
    static int FindIndexOfMaxVal3x1(float g[3]);

    /**
    * @brief Performs an "in place" normalization of the 3x1 vector. Double Precision Variant
    * @param g
    */
    static void Normalize3x1(double g[3]);


//    static void NormalizeVector(double g[3]);
    static void Normalize3x1(double& i, double& j, double& k);
//    static void NormalizeVector(float a[3]);
    static void Normalize3x1(float& i, float& j, float& k);

    /**
     * @brief The dot product of 2 vectors a & b
     * @param a 1x3 Vector
     * @param b 1x3 Vector
     * @return
     */
    static float DotProduct3x1(float a[3], float b[3]);

    /**
     * @brief Performs a Cross Product of "a into b" and places the result into c.
     * A X B = C
     * @param a
     * @param b
     * @param c
     */
    static void CrossProduct(double a[3], double b[3], double c[3]);

    static void CrossProduct(float a[3], float b[3], float c[3]);


  protected:
    MatrixMath();

  private:
    MatrixMath(const MatrixMath&); // Copy Constructor Not Implemented
    void operator=(const MatrixMath&); // Operator '=' Not Implemented
};

#endif /* _MatrixMath_H_ */
