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

#include "CubeOctohedronOps.h"
#include "SIMPLib/Math/SIMPLibMath.h"



float root3 = static_cast<float>( sqrt(3.0) );
float ShapeClass3Omega3[41][2] = {{0.787873524f, 0.0f},
  {0.78793553f, 0.05f},
  {0.788341216f, 0.1f},
  {0.789359741f, 0.15f},
  {0.791186818f, 0.2f},
  {0.793953966f, 0.25f},
  {0.797737494f, 0.3f},
  {0.802566619f, 0.35f},
  {0.808430467f, 0.4f},
  {0.815283954f, 0.45f},
  {0.823052718f, 0.5f},
  {0.831637359f, 0.55f},
  {0.840917349f, 0.6f},
  {0.850755028f, 0.65f},
  {0.86100021f, 0.7f},
  {0.871496036f, 0.75f},
  {0.882086906f, 0.8f},
  {0.892629636f, 0.85f},
  {0.903009489f, 0.9f},
  {0.913163591f, 0.95f},
  {0.92311574f, 1.00f},
  {0.932874613f, 1.05f},
  {0.941981628f, 1.1f},
  {0.949904418f, 1.15f},
  {0.956171947f, 1.2f},
  {0.96037277f, 1.25f},
  {0.962158855f, 1.3f},
  {0.961254001f, 1.35f},
  {0.957466141f, 1.4f},
  {0.950703099f, 1.45f},
  {0.940991385f, 1.5f},
  {0.92849772f, 1.55f},
  {0.913552923f, 1.6f},
  {0.89667764f, 1.65f},
  {0.878608694f, 1.7f},
  {0.860322715f, 1.75f},
  {0.843047317f, 1.8f},
  {0.828232275f, 1.85f},
  {0.81740437f, 1.9f},
  {0.811701359f, 1.95f},
  {0.810569469f, 2.0f}
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CubeOctohedronOps::CubeOctohedronOps() :
  Gvalue(0.0f)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CubeOctohedronOps::~CubeOctohedronOps()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
float CubeOctohedronOps::radcur1(QMap<ArgName, float> args)
{
  float radcur1 = 0.0f;
  float Gvaluedist = 0.0f;
  float bestGvaluedist = 1000000.0f;

  float omega3 = args[Omega3];
  float volcur = args[VolCur];

  for (int i = 0; i < 41; i++)
  {
    Gvaluedist = fabsf(omega3 - ShapeClass3Omega3[i][0]);
    if (Gvaluedist < bestGvaluedist)
    {
      bestGvaluedist = Gvaluedist;
      Gvalue = ShapeClass3Omega3[i][1];
    }
  }
  if (Gvalue >= 0 && Gvalue <= 1)
  {
    radcur1 = static_cast<float>( (volcur * 6.0) / (6 - (Gvalue * Gvalue * Gvalue)) );
  }
  if (Gvalue > 1 && Gvalue <= 2)
  {
    radcur1 = static_cast<float>( (volcur * 6.0) / (3 + (9 * Gvalue) - (9 * Gvalue * Gvalue) + (2 * Gvalue * Gvalue * Gvalue)) );
  }
  radcur1 = powf(radcur1, 0.333333333333f);
  radcur1 = radcur1 * 0.5f;
  return radcur1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
float CubeOctohedronOps::inside(float axis1comp, float axis2comp, float axis3comp)
{
  float inside = 0;
  inside = 1 - fabs(axis1comp);
  if((1 - fabs(axis2comp)) < inside)
  {
    inside = (1 - fabs(axis2comp));
  }
  if((1 - fabs(axis3comp)) < inside)
  {
    inside = (1 - fabs(axis3comp));
  }
  axis1comp = static_cast<float>( axis1comp + 1.0 );
  axis2comp = static_cast<float>( axis2comp + 1.0 );
  axis3comp = static_cast<float>( axis3comp + 1.0 );

  //Above -1,-1,1 plane check
  float plane1comp = ((-axis1comp) + (-axis2comp) + (axis3comp) - ((-0.5f * Gvalue) + (-0.5f * Gvalue) + 2.0f));
  plane1comp = plane1comp / ((-1) + (-1) + (1) - ((-0.5f * Gvalue) + (-0.5f * Gvalue) + 2.0f));

  float plane2comp = ((axis1comp) + (-axis2comp) + (axis3comp) - ((2.0f - (0.5f * Gvalue)) + (-0.5f * Gvalue) + 2.0f));
  plane2comp = plane2comp / ((1) + (-1) + (1) - ((2.0f - (0.5f * Gvalue)) + (-0.5f * Gvalue) + 2.0f));

  float plane3comp = ((axis1comp) + (axis2comp) + (axis3comp) - ((2.0f - (0.5f * Gvalue)) + (2.0f - (0.5f * Gvalue)) + 2.0f));
  plane3comp = plane3comp / ((1) + (1) + (1) - ((2.0f - (0.5f * Gvalue)) + (2.0f - (0.5f * Gvalue)) + 2.0f));

  float plane4comp = static_cast<float>( ((-axis1comp) + (axis2comp) + (axis3comp) - ((-0.5f * Gvalue) + (2.0f - (0.5 * Gvalue)) + 2.0f)) );
  plane4comp = plane4comp / ((-1) + (1) + (1) - ((-0.5f * Gvalue) + (2.0f - (0.5f * Gvalue)) + 2.0f));

  float plane5comp = ((-axis1comp) + (-axis2comp) + (-axis3comp) - ((-0.5f * Gvalue) + (-0.5f * Gvalue)));
  plane5comp = plane5comp / ((-1) + (-1) + (-1) - ((-0.5f * Gvalue) + (-0.5f * Gvalue)));

  float plane6comp = ((axis1comp) + (-axis2comp) + (-axis3comp) - ((2.0f - (0.5f * Gvalue)) + (-0.5f * Gvalue)));
  plane6comp = plane6comp / ((1) + (-1) + (-1) - ((2.0f - (0.5f * Gvalue)) + (-0.5f * Gvalue)));

  float plane7comp = ((axis1comp) + (axis2comp) + (-axis3comp) - ((2.0f - (0.5f * Gvalue)) + (2.0f - (0.5f * Gvalue))));
  plane7comp = static_cast<float>( plane7comp / ((1) + (1) + (-1) - ((2.0f - (0.5f * Gvalue)) + (2.0f - (0.5 * Gvalue)))) );

  float plane8comp = ((-axis1comp) + (axis2comp) + (-axis3comp) - ((-0.5f * Gvalue) + (2.0f - (0.5f * Gvalue))));
  plane8comp = plane8comp / ((-1) + (1) + (-1) - ((-0.5f * Gvalue) + (2 - (0.5f * Gvalue))));

  if(plane1comp < inside)
  {
    inside = plane1comp;
  }
  if(plane2comp < inside)
  {
    inside = plane2comp;
  }
  if(plane3comp < inside)
  {
    inside = plane3comp;
  }
  if(plane4comp < inside)
  {
    inside = plane4comp;
  }
  if(plane5comp < inside)
  {
    inside = plane5comp;
  }
  if(plane6comp < inside)
  {
    inside = plane6comp;
  }
  if(plane7comp < inside)
  {
    inside = plane7comp;
  }
  if(plane8comp < inside)
  {
    inside = plane8comp;
  }
  return inside;
}
