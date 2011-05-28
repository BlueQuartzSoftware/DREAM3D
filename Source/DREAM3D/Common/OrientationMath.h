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

#ifndef _OrientationMath_H_
#define _OrientationMath_H_

#include "MXA/Common/MXASetGetMacros.h"
#include "DREAM3D/DREAM3DConfiguration.h"
#include "DREAM3D/Common/Constants.h"
#include "DREAM3D/Common/AIMRandomNG.h"
#include "DREAM3D/Common/AIMMath.h"

#define MULT_QUAT(q1, q2, out)\
    out[1] = q2[4] * q1[1] + q2[1] * q1[4] + q2[3] * q1[2] - q2[2] * q1[3];\
    out[2] = q2[4] * q1[2] + q2[2] * q1[4] + q2[1] * q1[3] - q2[3] * q1[1];\
    out[3] = q2[4] * q1[3] + q2[3] * q1[4] + q2[2] * q1[1] - q2[1] * q1[2];\
    out[4] = q2[4] * q1[4] - q2[1] * q1[1] - q2[2] * q1[2] - q2[3] * q1[3];\


/**
 * @class OrientationMath OrientationMath.h AIM/Common/OrientationMath.h
 * @brief This class performs Crystallographic Misorientation Calculations
 * @author Michael A. Jackson (BlueQuartz Software)
 * @author Michael A. Groeber (US Air Force Research Laboratory)
 * @date Feb 19, 2011
 * @version 1.0
 */
class DREAM3DLib_EXPORT OrientationMath
{
  public:
    MXA_SHARED_POINTERS(OrientationMath)
    MXA_TYPE_MACRO(OrientationMath)

    virtual ~OrientationMath();

    /**
     * @brief
     * @param q1
     * @param q2
     * @param n1
     * @param n2
     * @param n3
     */
    virtual float getMisoQuat(float q1[5], float q2[5], float &n1, float &n2, float &n3) = 0;

    virtual void getFZRod(float &r1, float &r2, float &r3) = 0;

    virtual void getNearestQuat(float *q1, float *q2) = 0;

    virtual void getFZQuat(float *qr) = 0;

    virtual int getMisoBin(float n1, float n2, float n3) = 0;

    virtual void determineEulerAngles(int choose, float &synea1, float &synea2, float &synea3) = 0;

    virtual void determineHomochoricValues(int choose, float &r1, float &r2, float &r3) = 0;

    virtual int getOdfBin(float r1, float r2, float r3) = 0;


    static void axisAngletoHomochoric(float w, float n1, float n2, float n3, float &r1, float &r2, float &r3);
    static void axisAngletoRod(float w, float n1, float n2, float n3, float &r1, float &r2, float &r3);
    static void HomochorictoRod(float &r1, float &r2, float &r3);
    static void RodtoHomochoric(float &r1, float &r2, float &r3);
    static void RodtoAxisAngle(float r1, float r2, float r3, float &w, float &n1, float &n2, float &n3);
    static void RodtoQuat(float *q, float r1, float r2, float r3);
    static void QuattoRod(float *q, float &r1, float &r2, float &r3);
    static void QuattoEuler(float *q, float &ea1, float &ea2, float &ea3);
    static void getSlipMisalignment(int ss1, float q1[5], float q2[5], float &ssap);
    static void multiplyQuaternions(float inQuat[5], float multQuat[5], float outQuat[5]);
    static void eulertoQuat(float *q, float ea1, float ea2, float ea3);
    static void eulertoRod(float &r1, float &r2, float &r3, float ea1, float ea2, float ea3);

  protected:
    OrientationMath();

    float _calcMisoQuat(const float quatsym[24][5], int numsym,
                  float q1[5], float q2[5],
                  float &n1, float &n2, float &n3);

    void _calcFZRod(const float rodsym[24][3], int numsym, float &r1,float &r2, float &r3);
    void _calcNearestQuat(const float quatsym[24][5], int numsym, float *q1, float *q2);
    void _calcFZQuat(const float quatsym[24][5], int numsym, float *qr);

    int _calcMisoBin(float dim[3], float bins[3], float n1, float n2, float n3);
    void _calcDetermineEulerAngles(float init[3], float step[3], float phi[3],
                                   int choose, float &synea1, float &synea2, float &synea3);
    void _calcDetermineHomochoricValues(float step[3], float phi[3], int choose, float &r1, float &r2, float &r3);
    int _calcODFBin(float dim[3], float bins[3], float r1, float r2, float r3);

  private:
    OrientationMath(const OrientationMath&); // Copy Constructor Not Implemented
    void operator=(const OrientationMath&); // Operator '=' Not Implemented
};

#endif /* _OrientationMath_H_ */
