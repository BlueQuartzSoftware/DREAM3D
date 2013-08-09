/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2010, Dr. Michael A. Groeber (US Air Force Research Laboratories
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

#ifndef TetragonalOPS_H_
#define TetragonalOPS_H_

#include "MXA/Common/MXASetGetMacros.h"

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/Common/DataArray.hpp"
#include "DREAM3DLib/OrientationOps/OrientationOps.h"
#include "DREAM3DLib/Math/QuaternionMath.hpp"


/**
 * @class TetragonalOps TetragonalOps.h DREAM3DLib/Common/OrientationOps/TetragonalOps.h
 * @brief
 * @author Michael A. Jackson for BlueQuartz Software
 * @author Michael A. Groeber for USAF Research Laboratory, Materials Lab
 * @date May 5, 2011
 * @version 1.0
 */
class DREAM3DLib_EXPORT TetragonalOps : public OrientationOps
{
  public:
    MXA_SHARED_POINTERS(TetragonalOps)
    MXA_TYPE_MACRO_SUPER(TetragonalOps, OrientationOps)
    MXA_STATIC_NEW_MACRO(TetragonalOps)


    TetragonalOps();
    virtual ~TetragonalOps();

    static const int k_OdfSize = 23328;
    static const int k_MdfSize = 23328;
    static const int k_NumSymQuats = 8;

    virtual int getODFSize() { return k_OdfSize; }
    virtual int getMDFSize() { return k_MdfSize; }
    virtual int getNumSymOps() { return k_NumSymQuats; }

    virtual float getMisoQuat(QuatF &q1, QuatF &q2, float &n1, float &n2, float &n3);
    virtual void getQuatSymOp(int i, QuatF &q);
    virtual void getRodSymOp(int i, float *r);
    virtual void getMatSymOp(int i, float g[3][3]);
    virtual void getODFFZRod(float &r1, float &r2, float &r3);
    virtual void getMDFFZRod(float &r1, float &r2, float &r3);
    virtual void getNearestQuat(QuatF &q1, QuatF &q2);
    virtual void getFZQuat(QuatF &qr);
    virtual int getMisoBin(float r1, float r2, float r3);
    virtual void determineEulerAngles(int choose, float &synea1, float &synea2, float &synea3);
    virtual void determineRodriguesVector(int choose, float &r1, float &r2, float &r3);
    virtual int getOdfBin(float r1, float r2, float r3);
    virtual void getSchmidFactorAndSS(float loadx, float loady, float loadz, float &schmidfactor, int &slipsys);
    virtual void getmPrime(QuatF &q1, QuatF &q2, float LD[3], float &mPrime);
    virtual void getF1(QuatF &q1, QuatF &q2, float LD[3], bool maxSF, float &F1);
    virtual void getF1spt(QuatF &q1, QuatF &q2, float LD[3], bool maxSF, float &F1spt);
    virtual void getF7(QuatF &q1, QuatF &q2, float LD[3], bool maxSF, float &F7);
    virtual void generateSphereCoordsFromEulers(FloatArrayType *eulers, FloatArrayType *c1, FloatArrayType *c2, FloatArrayType *c3);

    virtual void generateIPFColor(double* eulers, double* refDir, uint8_t* rgb, bool convertDegrees);
    virtual void generateIPFColor(double e0, double e1, double e2, double dir0, double dir1, double dir2, uint8_t* rgb, bool convertDegrees);
    virtual void generateRodriguesColor(float r1, float r2, float r3, unsigned char* rgb);


  protected:
    float _calcMisoQuat(const QuatF quatsym[8], int numsym,
                  QuatF &q1, QuatF &q2,
                  float &n1, float &n2, float &n3);

  private:
    TetragonalOps(const TetragonalOps&); // Copy Constructor Not Implemented
    void operator=(const TetragonalOps&); // Operator '=' Not Implemented
};

#endif /* TetragonalOPS_H_ */
