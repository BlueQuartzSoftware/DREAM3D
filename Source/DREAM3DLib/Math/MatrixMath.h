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

    static void Multiply3x3with3x3(float g1[3][3], float g2[3][3], float outMat[3][3]);
    static void Multiply3x3with3x1(float g1[3][3], float g2[3], float outMat[3]);
    static void Multiply3x3with3x1(const float g1[3][3], float g2[3], float outMat[3]);
    static void Multiply3x3with3x1(double g1[3][3], double g2[3], double outMat[3]);

    static void Transpose3x3(float g[3][3], float outMat[3][3]);
    static void Copy3x3(float g[3][3], float outMat[3][3]);
    static void Normalize3x3(float g[3][3]);
    static void Normalize3x1(float g[3]);
    static float DotProduct(float a[3], float b[3]);

    static void CrossProduct(double a[3], double b[3], double c[3]);



    static void NormalizeVector(double a[3]);
    static void NormalizeVector(double &i, double &j, double &k);
    static void NormalizeVector(float a[3]);
    static void NormalizeVector(float &i, float &j, float &k);

  protected:
    MatrixMath();

  private:
    MatrixMath(const MatrixMath&); // Copy Constructor Not Implemented
    void operator=(const MatrixMath&); // Operator '=' Not Implemented
};

#endif /* _MatrixMath_H_ */
