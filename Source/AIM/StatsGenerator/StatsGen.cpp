/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
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

#include "StatsGen.h"

#include <math.h>

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------

StatsGen::StatsGen()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------

StatsGen::~StatsGen()
{
}

#if 0
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int StatsGen::GenBeta(double p, double q, std::vector<float> &x, std::vector<float> &y, int size)
{
	int err=0;
	double value, gammapq, gammap, gammaq, betain, betaout;
	x.resize(size);
	y.resize(size);
	value = p;
	gammap = gamma(value);
	value = q;
	gammaq = gamma(value);
	value = p+q;
	gammapq = gamma(value);
	for(int i=0;i<size;i++)
	{
		betain = (i*(1.0/double(size)))+((1.0/double(size))/2.0);
		betaout = (gammapq/(gammap*gammaq))*pow(betain,(p-1))*pow((1-betain),(q-1));
		x[i] = betain;
		y[i] = betaout*(1.0/double(size));
		if(betaout < 0) err = 1;
	}
	return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int StatsGen::GenLogNormal(double m, double s, std::vector<float> &x, std::vector<float> &y, int size)
{
	int err=0;
	double lognormin, lognormout, max, min;
	double s2 = pow(s,2);
	double root2pi = pow((2.0*3.1415926535897),0.5);
	x.resize(size);
	y.resize(size);
	min = exp(m - (10*s));
	max = exp(m + (10*s));
	for(int i=0;i<size;i++)
	{
		lognormin = (i*((max-min)/double(size)))+(((max-min)/double(size))/2.0);
		lognormout = (1.0/(lognormin*s*root2pi))*exp(-((log(lognormin)-m)*(log(lognormin)-m))/(2*s2));
		x[i] = lognormin;
		y[i] = lognormout*((max-min)/double(size));
		if(lognormout < 0) err = 1;
	}
	return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int StatsGen::GenCutOff(double m, double s, double value, std::vector<int> &cutoffs)
{
	int err=0;
	double max, min;
	cutoffs.resize(2);
	min = exp(m - (value*s));
	max = exp(m + (value*s));
	cutoffs[0] = min;
	cutoffs[1] = max;
	return err;
}
#endif

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
double StatsGen::gamma(double value)
{
    int i,k,m;
    double ga,gr,r,z;
	double m_pi = 3.1415926535897;


    static double g[] = {
        1.0,
        0.5772156649015329,
       -0.6558780715202538,
       -0.420026350340952e-1,
        0.1665386113822915,
       -0.421977345555443e-1,
       -0.9621971527877e-2,
        0.7218943246663e-2,
       -0.11651675918591e-2,
       -0.2152416741149e-3,
        0.1280502823882e-3,
       -0.201348547807e-4,
       -0.12504934821e-5,
        0.1133027232e-5,
       -0.2056338417e-6,
        0.6116095e-8,
        0.50020075e-8,
       -0.11812746e-8,
        0.1043427e-9,
        0.77823e-11,
       -0.36968e-11,
        0.51e-12,
       -0.206e-13,
       -0.54e-14,
        0.14e-14};

    if (value > 171.0) return 1e308;    // This value is an overflow flag.
    if (value == (int)value) {
        if (value > 0.0) {
            ga = 1.0;               // use factorial
            for (i=2;i<value;i++) {
               ga *= i;
            }
         }
         else
            ga = 1e308;
     }
     else {
        if (fabs(value) > 1.0) {
            z = fabs(value);
            m = (int)z;
            r = 1.0;
            for (k=1;k<=m;k++) {
                r *= (z-k);
            }
            z -= m;
        }
        else
            z = value;
        gr = g[24];
        for (k=23;k>=0;k--) {
            gr = gr*z+g[k];
        }
        ga = 1.0/(gr*z);
        if (fabs(value) > 1.0) {
            ga *= r;
            if (value < 0.0) {
                ga = -1 * m_pi/(value*ga*sin(m_pi*value));
            }
        }
    }
    return ga;
}






