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

#ifndef CUBICOPS_H_
#define CUBICOPS_H_


#include "MXA/Common/MXASetGetMacros.h"
#include "AIM/Common/OrientationMath.h"

/**
 * @class CubicOps CubicOps.h AIM/Common/OrientationOps/CubicOps.h
 * @brief
 * @author Michael A. Jackson for BlueQuartz Software
 * @author Michael A. Groeber for USAF Research Laboratory, Materials Lab
 * @date May 5, 2011
 * @version 1.0
 */
class AIMCOMMON_EXPORT CubicOps : public OrientationMath
{
  public:
    MXA_SHARED_POINTERS(CubicOps)
    MXA_TYPE_MACRO(CubicOps)
    MXA_STATIC_NEW_SUPERCLASS(OrientationMath, CubicOps)

    CubicOps();
    virtual ~CubicOps();

    int getODFSize() { return 5832; }
    int getMDFSize() { return 5832; }

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
    CubicOps(const CubicOps&); // Copy Constructor Not Implemented
    void operator=(const CubicOps&); // Operator '=' Not Implemented
};

#endif /* CUBICOPS_H_ */
