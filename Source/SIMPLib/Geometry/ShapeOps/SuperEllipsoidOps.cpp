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


#include "SuperEllipsoidOps.h"

#include "SIMPLib/Math/SIMPLibMath.h"





float ShapeClass2Omega3[41][2] = {{0.0f, 0.0f},
  {0.0f, 0.25f},
  {0.0f, 0.5f},
  {0.0f, 0.75f},
  {0.0f, 1.0f},
  {0.0f, 1.25f},
  {0.0f, 1.5f},
  {0.0f, 1.75f},
  {0.0f, 2.0f},
  {0.0f, 2.25f},
  {0.0f, 2.5f},
  {0.0f, 2.75f},
  {0.0f, 3.0f},
  {0.0f, 3.25f},
  {0.0f, 3.5f},
  {0.0f, 3.75f},
  {0.0f, 4.0f},
  {0.0f, 4.25f},
  {0.0f, 4.5f},
  {0.0f, 4.75f},
  {0.0f, 5.0f},
  {0.0f, 5.25f},
  {0.0f, 5.5f},
  {0.0f, 5.75f},
  {0.0f, 6.0f},
  {0.0f, 6.25f},
  {0.0f, 6.5f},
  {0.0f, 6.75f},
  {0.0f, 7.0f},
  {0.0f, 7.25f},
  {0.0f, 7.5f},
  {0.0f, 7.75f},
  {0.0f, 8.0f},
  {0.0f, 8.25f},
  {0.0f, 8.5f},
  {0.0f, 8.75f},
  {0.0f, 9.0f},
  {0.0f, 9.25f},
  {0.0f, 9.5f},
  {0.0f, 9.75f},
  {0.0f, 10.0f}
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SuperEllipsoidOps::SuperEllipsoidOps() :
  Nvalue(0.0f)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SuperEllipsoidOps::~SuperEllipsoidOps()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
float SuperEllipsoidOps::radcur1(QMap<ArgName, float> args)
{
  float radcur1 = 0.0f;
  float Nvaluedist = 0.0f;
  float bestNvaluedist = 1000000.0f;

  float omega3 = args[Omega3];
  float volcur = args[VolCur];
  float bovera = args[B_OverA];
  float covera = args[C_OverA];

  for (int i = 0; i < 41; i++)
  {
    float a = SIMPLibMath::Gamma(1.0f + 1.0f / ShapeClass2Omega3[i][1]);
    float b = SIMPLibMath::Gamma(5.0f / ShapeClass2Omega3[i][1]);
    float c = SIMPLibMath::Gamma(3.0f / ShapeClass2Omega3[i][1]);
    float d = SIMPLibMath::Gamma(1.0f + 3.0f / ShapeClass2Omega3[i][1]);
    ShapeClass2Omega3[i][0] = static_cast<float>( powf(20.0f * ((a * a * a) * b) / (c * powf(d, 5.0f / 3.0f)), 3.0f) / (2000.0f * M_PI * M_PI / 9.0f) );
    Nvaluedist = fabsf(omega3 - ShapeClass2Omega3[i][0]);
    if (Nvaluedist < bestNvaluedist)
    {
      bestNvaluedist = Nvaluedist;
      Nvalue = ShapeClass2Omega3[i][1];
    }
  }
  float beta1 = (SIMPLibMath::Gamma((1.0f / Nvalue)) * SIMPLibMath::Gamma((1.0f / Nvalue))) / SIMPLibMath::Gamma((2.0f / Nvalue));
  float beta2 = (SIMPLibMath::Gamma((2.0f / Nvalue)) * SIMPLibMath::Gamma((1.0f / Nvalue))) / SIMPLibMath::Gamma((3.0f / Nvalue));
  radcur1 = (volcur * (3.0f / 2.0f) * (1.0f / bovera) * (1.0f / covera) * ((Nvalue * Nvalue) / 4.0f) * (1.0f / beta1) * (1.0f / beta2));
  radcur1 = powf(radcur1, 0.333333333333f);
  return radcur1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
float SuperEllipsoidOps::inside(float axis1comp, float axis2comp, float axis3comp)
{
  float inside = 1.0f;
  axis1comp = fabs(axis1comp);
  axis2comp = fabs(axis2comp);
  axis3comp = fabs(axis3comp);
  axis1comp = powf(axis1comp, Nvalue);
  axis2comp = powf(axis2comp, Nvalue);
  axis3comp = powf(axis3comp, Nvalue);
  inside = 1.0f - axis1comp - axis2comp - axis3comp;
  return inside;
}
