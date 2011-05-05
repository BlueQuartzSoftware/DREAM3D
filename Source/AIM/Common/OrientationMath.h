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
#include "AIM/Common/AIMCommonConfiguration.h"
#include "AIM/Common/Constants.h"
#include "AIM/Common/AIMRandomNG.h"



/**
 * @class OrientationMath OrientationMath.h AIM/Common/OrientationMath.h
 * @brief This class performs Crystallographic Misorientation Calculations
 * @author Michael A. Jackson (BlueQuartz Software)
 * @author Michael A. Groeber (US Air Force Research Laboratory)
 * @date Feb 19, 2011
 * @version 1.0
 */
class AIMCOMMON_EXPORT OrientationMath
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
    virtual double getMisoQuat(double q1[5], double q2[5], double &n1, double &n2, double &n3) = 0;

    virtual void getFZRod(double &r1, double &r2, double &r3) = 0;

    virtual void getNearestQuat(double *q1, double *q2) = 0;

    virtual void getFZQuat(double *qr) = 0;

    virtual int getMisoBin(double n1, double n2, double n3) = 0;

    virtual void determineEulerAngles(int choose, double &synea1, double &synea2, double &synea3) = 0;

    virtual double determineAxisAngle(int choose) = 0;

    virtual int getOdfBin(double q1[5], double qref[5]) = 0;


    static void axisAngletoHomochoric(double &w, double &miso1, double &miso2, double &miso3);
    static void getSlipMisalignment(int ss1, double q1[5], double q2[5], double &ssap);
    static void multiplyQuaternions(double inQuat[5], double multQuat[5], double outQuat[5]);
    static void eulertoQuat(double *q, double ea1, double ea2, double ea3);

  protected:
    OrientationMath();

    double _calcMisoQuat(double quatsym[24][5], int numsym,
                  double q1[5], double q2[5],
                  double &n1, double &n2, double &n3);

    void _calcFZRod(double rodsym[24][3], int numsym, double &r1,double &r2, double &r3);
    void _calcNearestQuat(double quatsym[24][5], int numsym, double *q1, double *q2);
    void _calcFZQuat(double quatsym[24][5], int numsym, double *qr);

    int _calcMisoBin(double dim[3], double bins[3], double n1, double n2, double n3);
    void _calcDetermineEulerAngles(double init[3], double step[3], double phi[3],
                                   int choose, double &synea1, double &synea2, double &synea3);
    double _calcDetermineAxisAngle( double step[3], double phi[3], int choose);
    int _calcODFBin(double dim[3], double bins[3], double q1[5], double qref[5]);

  private:
    OrientationMath(const OrientationMath&); // Copy Constructor Not Implemented
    void operator=(const OrientationMath&); // Operator '=' Not Implemented
};

#endif /* _OrientationMath_H_ */
