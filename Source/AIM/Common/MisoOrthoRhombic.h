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

#ifndef MISOORTHORHOMBIC_H_
#define MISOORTHORHOMBIC_H_

#include "MisorientationCalculations.h"

/*
 *
 */
class MisoOrthoRhombic
{
  public:
    MisoOrthoRhombic();
    virtual ~MisoOrthoRhombic();

    /**
     *
     * @param q1
     * @param q2
     * @param n1
     * @param n2
     * @param n3
     * @return
     */
    virtual double getQuaternion(double q1[5],double q2[5],double &n1,double &n2,double &n3);

    /**
     *
     * @param r1
     * @param r2
     * @param r3
     */
    virtual void getFZRod(double &r1,double &r2, double &r3);

    /**
     *
     * @param q1
     * @param q2
     */
    virtual void getNearestQuaternion(double* q1, double* q2);

    /**
     *
     * @param qr
     */
    virtual void getFZQuaternion(double* qr);

    /**
     *
     * @param n1
     * @param n2
     * @param n3
     * @return
     */
    virtual int getMisoBin(double n1, double n2, double n3);

    /**
     *
     * @param q1
     * @param qref
     * @return
     */
    virtual size_t calculateODFBin( double q1[5], double qref[5]);

    /**
     *
     * @param choose
     * @param synea1
     * @param synea2
     * @param synea3
     */
    virtual void determineEulerAngles(int choose, double &synea1, double &synea2, double &synea3);

    /**
     *
     * @param dim1
     * @param dim2
     * @param dim3
     * @param numbins
     */
    virtual void initializeDims(double &dim1, double &dim2,  double &dim3, int &numbins);


  private:
    MisoOrthoRhombic(const MisoOrthoRhombic&); // Copy Constructor Not Implemented
    void operator=(const MisoOrthoRhombic&); // Operator '=' Not Implemented

};

#endif /* MISOORTHORHOMBIC_H_ */
