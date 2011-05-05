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

#ifndef _MISORIENTATIONCALCULATIONS_H_
#define _MISORIENTATIONCALCULATIONS_H_

#include "AIM/Common/AIMCommonConfiguration.h"
#include "AIM/Common/Constants.h"
#include "AIM/Common/AIMRandomNG.h"
#include <time.h>


/**
 * @class MisorientationCalculations MisorientationCalculations.h AIM/Common/MisorientationCalculations.h
 * @brief This class performs Crystallographic Misorientation Calculations
 * @author Michael A. Jackson (BlueQuartz Software)
 * @author Michael A. Groeber (US Air Force Research Laboratory)
 * @date Feb 19, 2011
 * @version 1.0
 */
class AIMCOMMON_EXPORT MisorientationCalculations
{
  public:

    virtual ~MisorientationCalculations();

    static double getMisoQuatCubic(double q1[5],double q2[5],double &n1,double &n2,double &n3);
    static double getMisoQuatHexagonal(double q1[5],double q2[5],double &n1,double &n2,double &n3);

    static void getFZRodCubic(double &r1,double &r2, double &r3);
    static void getFZRodHexagonal(double &r1,double &r2, double &r3);
    static void getFZRodOrtho(double &r1,double &r2, double &r3);

    static void getNearestQuatCubic(double *q1, double *q2);
    static void getNearestQuatHexagonal(double *q1, double *q2);

    static void getFZQuatCubic(double *qr);
    static void getFZQuatHexagonal(double *qr);

    static int getMisoBinCubic(double n1, double n2, double n3);
    static int getMisoBinHexagonal(double n1, double n2, double n3);

    static void determineEulerAngles(AIM::Reconstruction::CrystalStructure crystruct, int choose, double &synea1, double &synea2, double &synea3);
    static void determineAxisAngle(AIM::Reconstruction::CrystalStructure crystruct, int choose, double &w, double &n1, double &n2, double &n3);
    static void initializeDims( AIM::Reconstruction::CrystalStructure crystruct, double &dim1, double &dim2,  double &dim3, int &numbins);

    static void calculateMisorientationAngles(double &w, double &miso1, double &miso2, double &miso3);
    static void getSlipMisalignment(int ss1, double q1[5], double q2[5], double &ssap);
    static size_t calculateHexOdfBin( double q1[5], double qref[5]);
    static size_t calculateCubicOdfBin( double q1[5], double qref[5]);
    static void initializeQ(double *q, double ea1, double ea2, double ea3);

  protected:
    MisorientationCalculations();

  private:
    MisorientationCalculations(const MisorientationCalculations&); // Copy Constructor Not Implemented
    void operator=(const MisorientationCalculations&); // Operator '=' Not Implemented
};

#endif /* _MISORIENTATIONCALCULATIONS_H_ */
