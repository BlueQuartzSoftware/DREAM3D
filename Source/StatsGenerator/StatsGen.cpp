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


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int StatsGen::computeNumberOfBins(float mu, float sigma, float cutoff,
                                  float binstep, float &max, float &min)
{
  min = expf(mu - (cutoff * sigma));
  max = expf(mu + (cutoff * sigma));
  return static_cast<int>((max -min)/binstep)+1;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
float StatsGen::gamma(float value)
{
  int i, k, m;
  float ga, gr, r, z;
  float m_pi = M_PI;

  static float g[] =
  {
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
  0.14e-14 };

  if (value > 171.0) return 1e308; // This value is an overflow flag.
  if (value == (int)value)
  {
    if (value > 0.0)
    {
      ga = 1.0; // use factorial
      for (i = 2; i < value; i++)
      {
        ga *= i;
      }
    }
    else ga = 1e308;
  }
  else
  {
    if (fabs(value) > 1.0)
    {
      z = fabs(value);
      m = (int)z;
      r = 1.0;
      for (k = 1; k <= m; k++)
      {
        r *= (z - k);
      }
      z -= m;
    }
    else z = value;
    gr = g[24];
    for (k = 23; k >= 0; k--)
    {
      gr = gr * z + g[k];
    }
    ga = 1.0 / (gr * z);
    if (fabs(value) > 1.0)
    {
      ga *= r;
      if (value < 0.0)
      {
        ga = -1 * m_pi / (value * ga * sin(m_pi * value));
      }
    }
  }
  return ga;
}
