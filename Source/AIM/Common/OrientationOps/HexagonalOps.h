/* ============================================================================
 * Copyright (c) 2011, Michael A. Jackson (BlueQuartz Software)
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

#ifndef HEXAGONALOPS_H_
#define HEXAGONALOPS_H_

#include "MXA/Common/MXASetGetMacros.h"
#include "AIM/Common/OrientationMath.h"

/**
 * @class HexagonalOps HexagonalOps.h AIM/Common/OrientationOps/HexagonalOps.h
 * @brief
 * @author Michael A. Jackson for BlueQuartz Software
 * @author Michael A. Groeber for USAF Research Laboratory, Materials Lab
 * @date May 5, 2011
 * @version 1.0
 */
class AIMCOMMON_EXPORT HexagonalOps : public OrientationMath
{
  public:
    MXA_SHARED_POINTERS(HexagonalOps)
    MXA_TYPE_MACRO(HexagonalOps)
    MXA_STATIC_NEW_SUPERCLASS(OrientationMath, HexagonalOps)

    HexagonalOps();
    virtual ~HexagonalOps();

    static const int ODFSize = 15552;
    static const int MDFSize = 15552;

    virtual double getMisoQuat( double q1[5],double q2[5],double &n1,double &n2,double &n3);
    virtual void getFZRod(double &r1,double &r2, double &r3);
    virtual void getNearestQuat(double *q1, double *q2);
    virtual void getFZQuat(double *qr);
    virtual int getMisoBin(double n1, double n2, double n3);
    virtual void determineEulerAngles(int choose, double &synea1, double &synea2, double &synea3);
    virtual void determineAxisAngle(int choose, double &w, double &n1, double &n2, double &n3);
    virtual int getOdfBin(double q1[5], double qref[5]);


  protected:


  private:
    HexagonalOps(const HexagonalOps&); // Copy Constructor Not Implemented
    void operator=(const HexagonalOps&); // Operator '=' Not Implemented
};

#endif /* HEXAGONALOPS_H_ */
