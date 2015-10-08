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


#include "SIMPLibMath.h"
#include <limits>



static const float k_MachineEpsilon = 5E-16f;
//static const float k_MaxRealNumber = 1E300;
//static const float k_MinRealNumber = 1E-300;


SIMPLibMath::SIMPLibMath()
{

}

SIMPLibMath::~SIMPLibMath()
{
}


float SIMPLibMath::Gamma(float x)
{
  int i, k, m;
  float ga, gr, r, z;


  static float g[] =
  {
    1.0f,
    0.5772156649015329f,
    -0.6558780715202538f,
    -0.420026350340952e-1f,
    0.1665386113822915f,
    -0.421977345555443e-1f,
    -0.9621971527877e-2f,
    0.7218943246663e-2f,
    -0.11651675918591e-2f,
    -0.2152416741149e-3f,
    0.1280502823882e-3f,
    -0.201348547807e-4f,
    -0.12504934821e-5f,
    0.1133027232e-5f,
    -0.2056338417e-6f,
    0.6116095e-8f,
    0.50020075e-8f,
    -0.11812746e-8f,
    0.1043427e-9f,
    0.77823e-11f,
    -0.36968e-11f,
    0.51e-12f,
    -0.206e-13f,
    -0.54e-14f,
    0.14e-14f
  };

  if (x > 34.0f)
  {
    return std::numeric_limits<float>::max();    // This value is an overflow flag.
  }
  if (x == (int)x)
  {
    if (x > 0.0)
    {
      ga = 1.0;               // use factorial
      for (i = 2; i < x; i++)
      {
        ga *= i;
      }
    }
    else
    {
      ga = std::numeric_limits<float>::max();
    }
  }
  else
  {
    if (fabs(x) > 1.0)
    {
      z = fabs(x);
      m = (int)z;
      r = 1.0;
      for (k = 1; k <= m; k++)
      {
        r *= (z - k);
      }
      z -= m;
    }
    else
    {
      z = x;
    }
    gr = g[24];
    for (k = 23; k >= 0; k--)
    {
      gr = gr * z + g[k];
    }
    ga = 1.0f / (gr * z);
    if (fabs(x) > 1.0f)
    {
      ga *= r;
      if (x < 0.0f)
      {
        ga = -1 * SIMPLib::Constants::k_Pif / (x * ga * sinf(SIMPLib::Constants::k_Pif * x));
      }
    }
  }
  return ga;
}


float SIMPLibMath::gammastirf(float x)
{
  float result;
  float y;
  float w;
  float v;
  float stir;

  w = 1 / x;
  stir = 7.87311395793093628397E-4f;
  stir = -2.29549961613378126380E-4f + w * stir;
  stir = -2.68132617805781232825E-3f + w * stir;
  stir = 3.47222221605458667310E-3f + w * stir;
  stir = 8.33333333333482257126E-2f + w * stir;
  w = 1.0f + w * stir;
  y = exp(x);
  if(x > 143.01608f)
  {
    v = powf(x, 0.5f * x - 0.25f);
    y = v * (v / y);
  }
  else
  {
    y = powf(x, x - 0.5f) / y;
  }
  result = 2.50662827463100050242f * y * w;
  return result;
}
float SIMPLibMath::LnGamma(float x, float& sgngam)
{
  float result;
  float a;
  float b;
  float c;
  float p;
  float q;
  float u;
  float w;
  float z;
  int i;
  float logpi;
  float ls2pi;
  float tmp;

  sgngam = 1;
  logpi = 1.14472988584940017414f;
  ls2pi = 0.91893853320467274178f;
  if(x < -34.0)
  {
    q = -x;
    w = LnGamma(q, tmp);
    p = (floor(q));
    i = static_cast<int>(floor(p + 0.5f));
    if( i % 2 == 0 )
    {
      sgngam = -1;
    }
    else
    {
      sgngam = 1;
    }
    z = q - p;
    if(z > 0.5)
    {
      p = p + 1;
      z = p - q;
    }
    z = q * sinf(SIMPLib::Constants::k_Pif * z);
    result = logpi - log(z) - w;
    return result;
  }
  if(x < 13)
  {
    z = 1;
    p = 0;
    u = x;
    while(u > 3)
    {
      p = p - 1;
      u = x + p;
      z = z * u;
    }
    while(u < 2)
    {
      z = z / u;
      p = p + 1;
      u = x + p;
    }
    if(z < 0)
    {
      sgngam = -1;
      z = -z;
    }
    else
    {
      sgngam = 1;
    }
    if(u == 2)
    {
      result = log(z);
      return result;
    }
    p = p - 2;
    x = x + p;
    b = -1378.25152569120859100f;
    b = -38801.6315134637840924f + x * b;
    b = -331612.992738871184744f + x * b;
    b = -1162370.97492762307383f + x * b;
    b = -1721737.00820839662146f + x * b;
    b = -853555.664245765465627f + x * b;
    c = 1.0f;
    c = -351.815701436523470549f + x * c;
    c = -17064.2106651881159223f + x * c;
    c = -220528.590553854454839f + x * c;
    c = -1139334.44367982507207f + x * c;
    c = -2532523.07177582951285f + x * c;
    c = -2018891.41433532773231f + x * c;
    p = x * b / c;
    result = log(z) + p;
    return result;
  }
  q = (x - 0.5f) * log(x) - x + ls2pi;
  if(x >= 100000000)
  {
    result = q;
    return result;
  }
  p = 1 / (x * x);
  if(x >= 1000.0)
  {
    q = q + ((7.9365079365079365079365f * 0.0001f * p - 2.7777777777777777777778f * 0.001f) * p + 0.0833333333333333333333f) / x;
  }
  else
  {
    a = 8.11614167470508450300f * 0.0001f;
    a = -5.95061904284301438324f * 0.0001f + p * a;
    a = 7.93650340457716943945f * 0.0001f + p * a;
    a = -2.77777777730099687205f * 0.001f + p * a;
    a = 8.33333333333331927722f * 0.01f + p * a;
    q = q + a / x;
  }
  result = q;
  return result;
}

float SIMPLibMath::erf(float x)
{
  float result;
  float xsq;
  float s;
  float p;
  float q;


  s = 1;
  if(x < 0)
  {
    s = -1;
  }
  x = fabs(x);
  if(x < 0.5)
  {
    xsq = x * x;
    p = 0.007547728033418631287834f;
    p = 0.288805137207594084924010f + xsq * p;
    p = 14.3383842191748205576712f + xsq * p;
    p = 38.0140318123903008244444f + xsq * p;
    p = 3017.82788536507577809226f + xsq * p;
    p = 7404.07142710151470082064f + xsq * p;
    p = 80437.3630960840172832162f + xsq * p;
    q = 0.0f;
    q = 1.00000000000000000000000f + xsq * q;
    q = 38.0190713951939403753468f + xsq * q;
    q = 658.070155459240506326937f + xsq * q;
    q = 6379.60017324428279487120f + xsq * q;
    q = 34216.5257924628539769006f + xsq * q;
    q = 80437.3630960840172826266f + xsq * q;
    result = s * 1.1283791670955125738961589031f * x * p / q;
    return result;
  }
  if(x >= 10)
  {
    result = s;
    return result;
  }
  result = s * (1 - erfc(x));
  return result;
}
float SIMPLibMath::erfc(float x)
{
  float result;
  float p;
  float q;

  if(x < 0)
  {
    result = 2 - erfc(-x);
    return result;
  }
  if(x < 0.5f)
  {
    result = 1.0f - SIMPLibMath::erf(x);
    return result;
  }
  if(x >= 10)
  {
    result = 0;
    return result;
  }
  p = 0.0f;
  p = 0.5641877825507397413087057563f + x * p;
  p = 9.675807882987265400604202961f + x * p;
  p = 77.08161730368428609781633646f + x * p;
  p = 368.5196154710010637133875746f + x * p;
  p = 1143.262070703886173606073338f + x * p;
  p = 2320.439590251635247384768711f + x * p;
  p = 2898.0293292167655611275846f + x * p;
  p = 1826.3348842295112592168999f + x * p;
  q = 1.0f;
  q = 17.14980943627607849376131193f + x * q;
  q = 137.1255960500622202878443578f + x * q;
  q = 661.7361207107653469211984771f + x * q;
  q = 2094.384367789539593790281779f + x * q;
  q = 4429.612803883682726711528526f + x * q;
  q = 6089.5424232724435504633068f + x * q;
  q = 4958.82756472114071495438422f + x * q;
  q = 1826.3348842295112595576438f + x * q;
  result = exp(-(x * x)) * p / q;
  return result;
}
float SIMPLibMath::incompletebeta(float a, float b, float x)
{
  float maxrealnumber = std::numeric_limits<float>::max();
  float minrealnumber = std::numeric_limits<float>::min();
  float result;
  float t;
  float xc;
  float w;
  float y;
  int flag;
  float sg;
  float big;
  float biginv;
  float maxgam;
  float minlog;
  float maxlog;

  big = 4.503599627370496e15f;
  biginv = 2.22044604925031308085e-16f;
  maxgam = 171.624376956302725f;
  minlog = log(minrealnumber);
  maxlog = log(maxrealnumber);
  if(x == 0)
  {
    result = 0;
    return result;
  }
  if(x == 1)
  {
    result = 1;
    return result;
  }
  flag = 0;
  if((b * x) <= 1.0f && x <= 0.95f)
  {
    result = incompletebetaps(a, b, x, maxgam);
    return result;
  }
  w = 1.0f - x;
  if(x > (a / (a + b)))
  {
    flag = 1;
    t = a;
    a = b;
    b = t;
    xc = x;
    x = w;
  }
  else
  {
    xc = w;
  }
  if(flag == 1 && (b * x) <= 1.0f && x <= 0.95f)
  {
    t = incompletebetaps(a, b, x, maxgam);
    if(t <= k_MachineEpsilon)
    {
      result = 1.0f - k_MachineEpsilon;
    }
    else
    {
      result = 1.0f - t;
    }
    return result;
  }
  y = x * (a + b - 2.0f) - (a - 1.0f);
  if(y < 0.0f)
  {
    w = incompletebetafe(a, b, x, big, biginv);
  }
  else
  {
    w = incompletebetafe2(a, b, x, big, biginv) / xc;
  }
  y = a * log(x);
  t = b * log(xc);
  if((a + b) < maxgam && fabs(y) < maxlog && fabs(t) < maxlog)
  {
    t = powf(xc, b);
    t = t * powf(x, a);
    t = t / a;
    t = t * w;
    t = t * (SIMPLibMath::Gamma(a + b) / (SIMPLibMath::Gamma(a) * SIMPLibMath::Gamma(b)));
    if( flag == 1 )
    {
      if(t <= k_MachineEpsilon)
      {
        result = 1.0f - k_MachineEpsilon;
      }
      else
      {
        result = 1.0f - t;
      }
    }
    else
    {
      result = t;
    }
    return result;
  }
  y = y + t + LnGamma(a + b, sg) - LnGamma(a, sg) - LnGamma(b, sg);
  y = y + log(w / a);
  if(y < minlog)
  {
    t = 0.0f;
  }
  else
  {
    t = exp(y);
  }
  if(flag == 1)
  {
    if(t <= k_MachineEpsilon)
    {
      t = 1.0f - k_MachineEpsilon;
    }
    else
    {
      t = 1.0f - t;
    }
  }
  result = t;
  return result;
}
float SIMPLibMath::incompletebetafe(float a, float b, float x, float big, float biginv)
{
  float result;
  float xk;
  float pk;
  float pkm1;
  float pkm2;
  float qk;
  float qkm1;
  float qkm2;
  float k1;
  float k2;
  float k3;
  float k4;
  float k5;
  float k6;
  float k7;
  float k8;
  float r;
  float t;
  float ans;
  float thresh;
  int n;

  k1 = a;
  k2 = a + b;
  k3 = a;
  k4 = a + 1.0f;
  k5 = 1.0f;
  k6 = b - 1.0f;
  k7 = k4;
  k8 = a + 2.0f;
  pkm2 = 0.0f;
  qkm2 = 1.0f;
  pkm1 = 1.0f;
  qkm1 = 1.0f;
  ans = 1.0f;
  r = 1.0f;
  n = 0;
  thresh = 3.0f * k_MachineEpsilon;
  do
  {
    xk = -x * k1 * k2 / (k3 * k4);
    pk = pkm1 + pkm2 * xk;
    qk = qkm1 + qkm2 * xk;
    pkm2 = pkm1;
    pkm1 = pk;
    qkm2 = qkm1;
    qkm1 = qk;
    xk = x * k5 * k6 / (k7 * k8);
    pk = pkm1 + pkm2 * xk;
    qk = qkm1 + qkm2 * xk;
    pkm2 = pkm1;
    pkm1 = pk;
    qkm2 = qkm1;
    qkm1 = qk;
    if(qk != 0)
    {
      r = pk / qk;
    }
    if(r != 0.0f)
    {
      t = fabs((ans - r) / r);
      ans = r;
    }
    else
    {
      t = 1.0f;
    }
    if(t < thresh)
    {
      break;
    }
    k1 = k1 + 1.0f;
    k2 = k2 + 1.0f;
    k3 = k3 + 2.0f;
    k4 = k4 + 2.0f;
    k5 = k5 + 1.0f;
    k6 = k6 - 1.0f;
    k7 = k7 + 2.0f;
    k8 = k8 + 2.0f;
    if((fabs(qk) + fabs(pk)) > big)
    {
      pkm2 = pkm2 * biginv;
      pkm1 = pkm1 * biginv;
      qkm2 = qkm2 * biginv;
      qkm1 = qkm1 * biginv;
    }
    if(fabs(qk) < biginv || fabs(pk) < biginv)
    {
      pkm2 = pkm2 * big;
      pkm1 = pkm1 * big;
      qkm2 = qkm2 * big;
      qkm1 = qkm1 * big;
    }
    n = n + 1;
  }
  while(n != 300);
  result = ans;
  return result;
}
float SIMPLibMath::incompletebetafe2(float a, float b, float x, float big, float biginv)
{
  float result;
  float xk;
  float pk;
  float pkm1;
  float pkm2;
  float qk;
  float qkm1;
  float qkm2;
  float k1;
  float k2;
  float k3;
  float k4;
  float k5;
  float k6;
  float k7;
  float k8;
  float r;
  float t;
  float ans;
  float z;
  float thresh;
  int n;

  k1 = a;
  k2 = b - 1.0f;
  k3 = a;
  k4 = a + 1.0f;
  k5 = 1.0f;
  k6 = a + b;
  k7 = a + 1.0f;
  k8 = a + 2.0f;
  pkm2 = 0.0f;
  qkm2 = 1.0f;
  pkm1 = 1.0f;
  qkm1 = 1.0f;
  z = x / (1.0f - x);
  ans = 1.0f;
  r = 1.0f;
  n = 0;
  thresh = 3.0f * k_MachineEpsilon;
  do
  {
    xk = -z * k1 * k2 / (k3 * k4);
    pk = pkm1 + pkm2 * xk;
    qk = qkm1 + qkm2 * xk;
    pkm2 = pkm1;
    pkm1 = pk;
    qkm2 = qkm1;
    qkm1 = qk;
    xk = z * k5 * k6 / (k7 * k8);
    pk = pkm1 + pkm2 * xk;
    qk = qkm1 + qkm2 * xk;
    pkm2 = pkm1;
    pkm1 = pk;
    qkm2 = qkm1;
    qkm1 = qk;
    if(qk != 0)
    {
      r = pk / qk;
    }
    if(r != 0.0f)
    {
      t = fabs((ans - r) / r);
      ans = r;
    }
    else
    {
      t = 1.0f;
    }
    if(t < thresh)
    {
      break;
    }
    k1 = k1 + 1.0f;
    k2 = k2 - 1.0f;
    k3 = k3 + 2.0f;
    k4 = k4 + 2.0f;
    k5 = k5 + 1.0f;
    k6 = k6 + 1.0f;
    k7 = k7 + 2.0f;
    k8 = k8 + 2.0f;
    if((fabs(qk) + fabs(pk)) > big)
    {
      pkm2 = pkm2 * biginv;
      pkm1 = pkm1 * biginv;
      qkm2 = qkm2 * biginv;
      qkm1 = qkm1 * biginv;
    }
    if(fabs(qk) < biginv || fabs(pk) < biginv)
    {
      pkm2 = pkm2 * big;
      pkm1 = pkm1 * big;
      qkm2 = qkm2 * big;
      qkm1 = qkm1 * big;
    }
    n = n + 1;
  }
  while(n != 300);
  result = ans;
  return result;
}
float SIMPLibMath::incompletebetaps(float a, float b, float x, float maxgam)
{
  float result;
  float s;
  float t;
  float u;
  float v;
  float n;
  float t1;
  float z;
  float ai;
  float sg;
  float maxrealnumber = std::numeric_limits<float>::max();
  float minrealnumber = std::numeric_limits<float>::min();
  ai = 1.0f / a;
  u = (1.0f - b) * x;
  v = u / (a + 1.0f);
  t1 = v;
  t = u;
  n = 2.0;
  s = 0.0;
  z = k_MachineEpsilon * ai;
  while(fabs(v) > z)
  {
    u = (n - b) * x / n;
    t = t * u;
    v = t / (a + n);
    s = s + v;
    n = n + 1.0f;
  }
  s = s + t1;
  s = s + ai;
  u = a * log(x);
  if((a + b) < maxgam && fabs(u) < log(maxrealnumber))
  {
    t = SIMPLibMath::Gamma(a + b) / (SIMPLibMath::Gamma(a) * SIMPLibMath::Gamma(b));
    s = s * t * powf(x, a);
  }
  else
  {
    t = LnGamma(a + b, sg) - LnGamma(a, sg) - LnGamma(b, sg) + u + log(s);
    if(t < log(minrealnumber))
    {
      s = 0.0;
    }
    else
    {
      s = exp(t);
    }
  }
  result = s;
  return result;
}

void SIMPLibMath::boundD(double& val, double min, double max)
{
  if(val < min)
  {
    val = min;
  }
  else if(val > max)
  {
    val = max;
  }
}

void SIMPLibMath::boundF(float& val, float min, float max)
{
  if(val < min)
  {
    val = min;
  }
  else if(val > max)
  {
    val = max;
  }
}

void SIMPLibMath::boundI(int& val, int min, int max)
{
  if(val < min)
  {
    val = min;
  }
  else if(val > max)
  {
    val = max;
  }
}

std::vector<double> SIMPLibMath::linspace(double minVal, double maxVal, int dim)
{
  std::vector<double> result;
  if(dim == 1)
  {
    result.push_back(maxVal);
  }
  else if(dim > 1)
  {
    double spacing = (maxVal - minVal) / (dim - 1);
    for(int i = 0; i < dim; i++)
    {
      result.push_back(minVal + spacing * i);
    }
  }
  return result;
}
