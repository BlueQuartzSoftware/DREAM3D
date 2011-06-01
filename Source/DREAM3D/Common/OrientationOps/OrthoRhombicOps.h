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

#ifndef ORTHORHOMBICOPS_H_
#define ORTHORHOMBICOPS_H_

#include "MXA/Common/MXASetGetMacros.h"
#include "DREAM3D/Common/OrientationMath.h"

/**
 * @class OrthoRhombicOps OrthoRhombicOps.h AIM/Common/OrientationOps/OrthoRhombicOps.h
 * @brief
 * @author Michael A. Jackson for BlueQuartz Software
 * @author Michael A. Groeber for USAF Research Laboratory, Materials Lab
 * @date May 5, 2011
 * @version 1.0
 */
class DREAM3DLib_EXPORT OrthoRhombicOps : public OrientationMath
{
  public:
    MXA_SHARED_POINTERS(OrthoRhombicOps)
    MXA_TYPE_MACRO(OrthoRhombicOps)
 //   MXA_STATIC_NEW_SUPERCLASS(OrientationMath, OrthoRhombicOps)
    MXA_STATIC_NEW_MACRO(OrthoRhombicOps)


    OrthoRhombicOps();
    virtual ~OrthoRhombicOps();

    //FIXME: These need actual values assigned to them.
    int getODFSize() { return 0; }
    int getMDFSize() { return 0; }

    virtual float getMisoQuat( float q1[5],float q2[5],float &n1,float &n2,float &n3);
    virtual void getFZRod(float &r1,float &r2, float &r3);
    virtual void getNearestQuat( float *q1, float *q2);
    virtual void getFZQuat(float *qr);
    virtual int getMisoBin(float n1, float n2, float n3);
    virtual void determineEulerAngles( int choose, float &synea1, float &synea2, float &synea3);
	virtual void determineHomochoricValues(int choose, float &r1, float &r2, float &r3);
    virtual int getOdfBin(float r1, float r2, float r3);


  protected:


  private:
    OrthoRhombicOps(const OrthoRhombicOps&); // Copy Constructor Not Implemented
    void operator=(const OrthoRhombicOps&); // Operator '=' Not Implemented
};

#endif /* ORTHORHOMBICOPS_H_ */
