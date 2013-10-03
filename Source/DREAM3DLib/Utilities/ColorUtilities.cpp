/* ============================================================================
 * Copyright (c) 2012 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2012 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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
 * Neither the name of Michael A. Groeber, Michael A. Jackson, the US Air Force,
 * BlueQuartz Software nor the names of its contributors may be used to endorse
 * or promote products derived from this software without specific prior written
 * permission.
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
 *  This code was written under United States Air Force Contract number
 *                           FA8650-10-D-5210
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include "ColorUtilities.h"

#include "DREAM3DLib/Math/DREAM3DMath.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ColorUtilities::ColorUtilities()
{}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ColorUtilities::~ColorUtilities()
{}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DREAM3D::Rgb ColorUtilities::convertHSVtoRgb(float h, float s, float v)
{
//hsv to rgb (from wikipedia hsv/hsl page)
  float c = v * s;
  float k = c * (1.0f - fabs(fmod(h * 6.0f, 2.0f) - 1.0f)); //x in wiki article
  h = h * 6;
  float r = 0.0f;
  float g = 0.0f;
  float b = 0.0f;

  if(h >= 0.0f)
  {
    if(h < 1.0f)
    {
      r = c;
      g = k;
    }
    else if(h < 2.0f)
    {
      r = k;
      g = c;
    }
    else if(h < 3.0f)
    {
      g = c;
      b = k;
    }
    else if(h < 4.0f)
    {
      g = k;
      b = c;
    }
    else if (h < 5.0f)
    {
      r = k;
      b = c;
    }
    else if(h < 6.0f)
    {
      r = c;
      b = k;
    }
  }

  //adjust lumosity and invert
  r = (r + (v - c));
  g = (g + (v - c));
  b = (b + (v - c));

  //now standard 0-1 rgb, needs rotation
  k = r;
  r = 1 - g;
  g = b;
  b = k;


  if(r > 1.0f)
  { r = 1.0f; }
  if(g > 1.0f)
  { g = 1.0f; }
  if(b > 1.0f)
  { b = 1.0f; }
  if(r < 0.0f)
  { r = 0.0f; }
  if(g < 0.0f)
  { g = 0.0f; }
  if(b < 0.0f)
  { b = 0.0f; }

  return RgbColor::dRgb(r * 255, g * 255, b * 255, 0);
}
