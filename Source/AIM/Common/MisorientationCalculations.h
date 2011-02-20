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

    /**
     * @brief
     * @param q1
     * @param q2
     * @param n1
     * @param n2
     * @param n3
     * @return
     */
    static double getMisoQuatCubic(double q1[5],double q2[5],double &n1,double &n2,double &n3);

    /**
     * @brief
     * @param q1
     * @param q2
     * @param n1
     * @param n2
     * @param n3
     * @return
     */
    static double getMisoQuatHexagonal(double q1[5],double q2[5],double &n1,double &n2,double &n3);

    /**
     * @brief
     * @param r1
     * @param r2
     * @param r3
     * @return
     */
    static void getFZRodCubic(double &r1,double &r2, double &r3);

    /**
     * @brief
     * @param qr
     */
    static void getFZQuatCubic(double *qr);

    /**
     * @brief
     * @param qr
     */
    static void getFZQuatHexagonal(double *qr);

    /**
     * @brief
     * @param n1
     * @param n2
     * @param n3
     * @return
     */
    static int getMisoBinCubic(double n1, double n2, double n3);

    /**
     * @brief
     * @param n1
     * @param n2
     * @param n3
     * @return
     */
    static int getMisoBinHexagonal(double n1, double n2, double n3);

  protected:
    MisorientationCalculations();

  private:
    MisorientationCalculations(const MisorientationCalculations&); // Copy Constructor Not Implemented
    void operator=(const MisorientationCalculations&); // Operator '=' Not Implemented
};

#endif /* _MISORIENTATIONCALCULATIONS_H_ */
