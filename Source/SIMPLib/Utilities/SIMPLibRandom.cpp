/* ============================================================================
* Copyright (c) 2009-2015 BlueQuartz Software, LLC
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
* Neither the name of BlueQuartz Software, the US Air Force, nor the names of its
* contributors may be used to endorse or promote products derived from this software
* without specific prior written permission.
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
*
* The code contained herein was partially funded by the followig contracts:
*    United States Air Force Prime Contract FA8650-07-D-5800
*    United States Air Force Prime Contract FA8650-10-D-5210
*    United States Prime Contract Navy N00173-07-C-2068
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */


/*
   A C-program for MT19937, with initialization improved 2002/1/26.
   Coded by Takuji Nishimura and Makoto Matsumoto.

   Before using, initialize the state by using init_genrand(seed)
   or init_by_array(init_key, key_length).

   Copyright (C) 1997 - 2002, Makoto Matsumoto and Takuji Nishimura,
   All rights reserved.
   Copyright (C) 2005, Mutsuo Saito
   All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:

     1. Redistributions of source code must retain the above copyright
        notice, this list of conditions and the following disclaimer.

     2. Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.

     3. The names of its contributors may not be used to endorse or promote
        products derived from this software without specific prior written
        permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


   Any feedback is very welcome.
   http://www.math.sci.hiroshima-u.ac.jp/~m_DataContainer-mat/MT/emt.html
   email: m_DataContainer-mat @ math.sci.hiroshima-u.ac.jp (remove space)
*/

#include "SIMPLibRandom.h"

#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

#include <math.h>


/* Period parameters */

#define MATRIX_A 0x9908b0dfUL   /* constant vector a */
#define UPPER_MASK 0x80000000UL /* most significant w-r bits */
#define LOWER_MASK 0x7fffffffUL /* least significant r bits */

//static unsigned long mt[N]; /* the array for the state vector  */
//static int mti=N+1; /* mti==N+1 means mt[N] is not initialized */



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SIMPLibRandom::SIMPLibRandom()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SIMPLibRandom::~SIMPLibRandom()
{
}

/* initializes mt[N] with a seed */
void SIMPLibRandom::init_genrand(unsigned long s)
{

  mti = MERSENNNE_TWISTER_N + 1;
  mt[0] = s & 0xffffffffUL;
  for (mti = 1; mti < MERSENNNE_TWISTER_N; mti++)
  {
    mt[mti] =
      (1812433253UL * (mt[mti - 1] ^ (mt[mti - 1] >> 30)) + mti);
    /* See Knuth TAOCP Vol2. 3rd Ed. P.106 for multiplier. */
    /* In the previous versions, MSBs of the seed affect   */
    /* only MSBs of the array mt[].                        */
    /* 2002/01/09 modified by Makoto Matsumoto             */
    mt[mti] &= 0xffffffffUL;
    /* for >32 bit machines */
  }

}

/* initialize by an array with array-length */
/* init_key is the array for initializing keys */
/* key_length is its length */
/* slight change for C++, 2004/2/26 */
void SIMPLibRandom::init_by_array(unsigned long init_key[], int key_length)
{
  int i, j, k;

  init_genrand(19650218UL);
  i = 1;
  j = 0;
  k = (MERSENNNE_TWISTER_N > key_length ? MERSENNNE_TWISTER_N : key_length);
  for (; k; k--)
  {
    mt[i] = (mt[i] ^ ((mt[i - 1] ^ (mt[i - 1] >> 30)) * 1664525UL))
            + init_key[j] + j; /* non linear */
    mt[i] &= 0xffffffffUL; /* for WORDSIZE > 32 machines */
    i++;
    j++;
    if (i >= MERSENNNE_TWISTER_N)
    {
      mt[0] = mt[MERSENNNE_TWISTER_N - 1];
      i = 1;
    }
    if (j >= key_length)
    {
      j = 0;
    }
  }
  for (k = MERSENNNE_TWISTER_N - 1; k; k--)
  {
    mt[i] = (mt[i] ^ ((mt[i - 1] ^ (mt[i - 1] >> 30)) * 1566083941UL))
            - i; /* non linear */
    mt[i] &= 0xffffffffUL; /* for WORDSIZE > 32 machines */
    i++;
    if (i >= MERSENNNE_TWISTER_N)
    {
      mt[0] = mt[MERSENNNE_TWISTER_N - 1];
      i = 1;
    }
  }

  mt[0] = 0x80000000UL; /* MSB is 1; assuring non-zero initial array */
}

/* generates a random number on [0,0xffffffff]-interval */
unsigned long SIMPLibRandom::genrand_int32()
{
  unsigned long y;
  static unsigned long mag01[2] = {0x0UL, MATRIX_A};

  /* mag01[x] = x * MATRIX_A  for x=0,1 */

  if (mti >= MERSENNNE_TWISTER_N)   /* generate N words at one time */
  {
    int kk;

    if (mti == MERSENNNE_TWISTER_N + 1) /* if init_genrand() has not been called, */
    {
      init_genrand(5489UL);    /* a default initial seed is used */
    }

    for (kk = 0; kk < MERSENNNE_TWISTER_N - MERSENNNE_TWISTER_M; kk++)
    {
      y = (mt[kk] & UPPER_MASK) | (mt[kk + 1] & LOWER_MASK);
      mt[kk] = mt[kk + MERSENNNE_TWISTER_M] ^ (y >> 1) ^ mag01[y & 0x1UL];
    }
    for (; kk < MERSENNNE_TWISTER_N - 1; kk++)
    {
      y = (mt[kk] & UPPER_MASK) | (mt[kk + 1] & LOWER_MASK);
      mt[kk] = mt[kk + (MERSENNNE_TWISTER_M - MERSENNNE_TWISTER_N)] ^ (y >> 1) ^ mag01[y & 0x1UL];
    }
    y = (mt[MERSENNNE_TWISTER_N - 1] & UPPER_MASK) | (mt[0] & LOWER_MASK);
    mt[MERSENNNE_TWISTER_N - 1] = mt[MERSENNNE_TWISTER_M - 1] ^ (y >> 1) ^ mag01[y & 0x1UL];

    mti = 0;
  }

  y = mt[mti++];

  /* Tempering */
  y ^= (y >> 11);
  y ^= (y << 7) & 0x9d2c5680UL;
  y ^= (y << 15) & 0xefc60000UL;
  y ^= (y >> 18);

  return y;
}

/* generates a random number on [0,0x7fffffff]-interval */
long SIMPLibRandom::genrand_int31()
{
  return (long)(genrand_int32() >> 1);
}

/* generates a random number on [0,1]-real-interval */
double SIMPLibRandom::genrand_real1()
{
  return genrand_int32() * (1.0 / 4294967295.0);
  /* divided by 2^32-1 */
}

/* These real versions are due to Isaku Wada, 2002/01/09 added */
/* generates a random number on [0,1)-real-interval */
double SIMPLibRandom::genrand_real2()
{
  return genrand_int32() * (1.0 / 4294967296.0);
  /* divided by 2^32 */
}

/* generates a random number on (0,1)-real-interval */
double SIMPLibRandom::genrand_real3()
{
  return (((double)genrand_int32()) + 0.5) * (1.0 / 4294967296.0);
  /* divided by 2^32 */
}

/* generates a random number on [0,1) with 53-bit resolution*/
double SIMPLibRandom::genrand_res53()
{
  unsigned long a = genrand_int32() >> 5, b = genrand_int32() >> 6;
  return(a * 67108864.0 + b) * (1.0 / 9007199254740992.0);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
double SIMPLibRandom::genrand_beta(double aa, double bb)
{
#define expmax 89.0
#define infnty 1.0E38

  static double olda = -1.0;
  static double oldb = -1.0;
  static double genbet, u1, u2, a, alpha, b, beta, delta, gamma, k1, k2, r, s, t, v, w, y, z;
  static long qsame;

  qsame = olda == aa && oldb == bb;
  if(qsame)
  {
    goto S20;
  }
  olda = aa;
  oldb = bb;
S20:

  if(aa > 1.0 && bb > 1.0)
  {
    goto S100;
  }
  if(qsame)
  {
    goto S30;
  }
  a = aa;
  b = bb;
  if(a > b)
  {
    double temp = a;
    a = b;
    b = temp;
  }
  alpha = a + b;
  beta = sqrt((alpha - 2.0) / (2.0 * a * b - alpha));
  gamma = a + 1.0 / beta;
S30:
S40:
  u1 = genrand_res53();
  v = beta * log(u1 / (1.0 - u1));
  if(!(v > expmax))
  {
    goto S50;
  }
  w = infnty;
  goto S60;
S50:
  w = a * exp(v);
S60:
  z = powf(static_cast<float>(u1), 2.0f) * static_cast<float>(u2);
  r = gamma * v - 1.3862944;
  s = a + r - w;
  if(s + 2.609438 >= 5.0 * z)
  {
    goto S70;
  }
  t = log(z);
  if(s > t)
  {
    goto S70;
  }
  if(r + alpha * log(alpha / (b + w)) < t)
  {
    goto S40;
  }
S70:
  if(!(aa == a))
  {
    goto S80;
  }
  genbet = w / (b + w);
  goto S90;
S80:
  genbet = b / (b + w);
S90:
  goto S230;
S100:
  if(qsame)
  {
    goto S110;
  }
  a = aa;
  b = bb;
  if(b > a)
  {
    double temp = a;
    a = b;
    b = temp;
  }
  alpha = a + b;
  beta = 1.0 / b;
  delta = 1.0 + a - b;
  k1 = delta * (1.38889E-2 + 4.16667E-2 * b) / (a * beta - 0.777778);
  k2 = 0.25 + (0.5 + 0.25 / delta) * b;
S110:
S120:
  u1 = genrand_res53();
  u2 = genrand_res53();
  if(u1 >= 0.5)
  {
    goto S130;
  }
  y = u1 * u2;
  z = u1 * y;
  if(0.25 * u2 + z - y >= k1)
  {
    goto S120;
  }
  goto S170;
S130:
  z = powf(static_cast<float>(u1), 2.0f) * static_cast<float>(u2);
  if(!(z <= 0.25))
  {
    goto S160;
  }
  v = beta * log(u1 / (1.0 - u1));
  if(!(v > expmax))
  {
    goto S140;
  }
  w = infnty;
  goto S150;
S140:
  w = a * exp(v);
S150:
  goto S200;
S160:
  if(z >= k2)
  {
    goto S120;
  }
S170:
  v = beta * log(u1 / (1.0 - u1));
  if(!(v > expmax))
  {
    goto S180;
  }
  w = infnty;
  goto S190;
S180:
  w = a * exp(v);
S190:
  if(alpha * (log(alpha / (b + w)) + v) - 1.3862944 < log(z))
  {
    goto S120;
  }
S200:
  if(!(a == aa))
  {
    goto S210;
  }
  genbet = w / (b + w);
  goto S220;
S210:
  genbet = b / (b + w);
S230:
S220:
  return genbet;
#undef expmax
#undef infnty
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
double SIMPLibRandom::genrand_norm(double m, double s)
{
  const double p0 = 0.322232431088;
  const double q0 = 0.099348462606;
  const double p1 = 1.0;
  const double q1 = 0.588581570495;
  const double p2 = 0.342242088547;
  const double q2 = 0.531103462366;
  const double p3 = 0.204231210245e-1;
  const double q3 = 0.103537752850;
  const double p4 = 0.453642210148e-4;
  const double q4 = 0.385607006340e-2;
  double u, t, p, q, z;

  u = genrand_res53();
  if (u < 0.5)
  {
    t = sqrt(-2.0 * log(u));
  }
  else
  {
    t = sqrt(-2.0 * log(1.0 - u));
  }
  p   = p0 + t * (p1 + t * (p2 + t * (p3 + t * p4)));
  q   = q0 + t * (q1 + t * (q2 + t * (q3 + t * q4)));
  if (u < 0.5)
  {
    z = (p / q) - t;
  }
  else
  {
    z = t - (p / q);
  }
  return (m + s * z);
}
