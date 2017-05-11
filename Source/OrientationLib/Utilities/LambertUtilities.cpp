/* ============================================================================
 * Copyright (c) 2017 BlueQuartz Softwae, LLC
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
 * Neither the names of any of the BlueQuartz Software contributors
 * may be used to endorse or promote products derived from this software without
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
 *
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include "LambertUtilities.h"

#include <assert.h>

#include <cmath>

#include "SIMPLib/Math/SIMPLibMath.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
LambertUtilities::LambertUtilities()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
LambertUtilities::~LambertUtilities()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int32_t LambertUtilities::LambertSquareVertToSphereVert(float* vert, Hemisphere hemi)
{
  static const float epsilon = 0.00001f;
  static const float L = static_cast<float>(SIMPLib::Constants::k_SqrtHalfPi);
  static const float r = 1.0f; // radius of unit sphere

  float x = 0.0f;
  float y = 0.0f;
  float z = 0.0f;
  float a = vert[0];
  float b = vert[1];

  float hemiFactor = (hemi == Hemisphere::North) ? -1.0f : 1.0;

  // force points inside the SEAP square
  if(std::abs(a) > L)
  {
    a = L;
  }
  if(std::abs(b) > L)
  {
    b = L;
  }

  if(std::abs(a) < epsilon && std::abs(b) < epsilon) // Handle the Origin
  {
    x = 0.0f;
    y = 0.0f;
    z = 1.0f;
  }
  else if(std::abs(b) <= std::abs(a))
  {
    float var0 = (2.0f * a / SIMPLib::Constants::k_Pif) * std::sqrt(SIMPLib::Constants::k_Pif - (a * a / r * r));
    float var1 = b * SIMPLib::Constants::k_Pif / (4.0f * a);
    x = var0 * std::cos(var1);
    y = var0 * std::sin(var1);
    z = hemiFactor * ((2 * a * a / SIMPLib::Constants::k_Pif * r) - r);
  }
  else if(std::abs(a) <= std::abs(b))
  {
    float var0 = (2.0f * b / SIMPLib::Constants::k_Pif) * std::sqrt(SIMPLib::Constants::k_Pif - (b * b / r * r));
    float var1 = a * SIMPLib::Constants::k_Pif / (4.0f * b);
    x = var0 * std::sin(var1);
    y = var0 * std::cos(var1);
    z = hemiFactor * ((2 * b * b / SIMPLib::Constants::k_Pif * r) - r);
  }

  if(std::abs(x * x + y * y + z * z - 1.0f) > epsilon)
  {
    return -1;
  }

  vert[0] = x;
  vert[1] = y;
  vert[2] = z;
  return  0;
}
