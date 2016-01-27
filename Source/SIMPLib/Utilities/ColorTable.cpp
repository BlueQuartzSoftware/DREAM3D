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


#include "ColorTable.h"

#include "SIMPLib/Common/SIMPLibSetGetMacros.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DREAM3DColorTable::DREAM3DColorTable()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DREAM3DColorTable::~DREAM3DColorTable()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DColorTable::GetColorTable(int numColors, QVector<float>& colors)
{
  static const int numColorNodes = 8;
  float color[numColorNodes][3] =
  {
    {0.0f, 0.0f / 255.0f, 255.0f / 255.0f}, // blue
    {105.0f / 255.0f, 145.0f / 255.0f, 2.0f / 255.0f}, // yellow
    {0.0f / 255.0f, 255.0f / 255.0f, 29.0f / 255.0f}, // Green
    {180.0f / 255.0f, 255.0f / 255.0f, 0.0f / 255.0f},
    {255.0f / 255.0f, 215.0f / 255.0f, 6.0f / 255.0f},
    {255.0f / 255.0f, 143.0f / 255.0f, 1.0f / 255.0f},
    {255.0f / 255.0f, 69.0f / 255.0f, 0.0f / 255.0f},
    {255.0f / 255.0f, 0.0f / 255.0f, 0.0f / 255.0f} // red
  };

  float val = 0.0f, r = 0.0f, g = 0.0f, b = 0.0f;
  float step = 1.0 / float(numColors);
  float nodeStep = 1.0f / float(numColorNodes - 1);
  for (int i = 0; i < (numColors); i++)
  {
    val = float(i) * step;
    int currColorBin = int(val / nodeStep);
    float currFraction = (val / nodeStep) - currColorBin;
    if(currColorBin > numColorNodes - 1)
    {
      currColorBin = numColorNodes - 1;
    }
    r = color[currColorBin][0] * (1.0 - currFraction) + color[currColorBin + 1][0] * currFraction;
    g = color[currColorBin][1] * (1.0 - currFraction) + color[currColorBin + 1][1] * currFraction;
    b = color[currColorBin][2] * (1.0 - currFraction) + color[currColorBin + 1][2] * currFraction;
    colors[3 * i] = r;
    colors[3 * i + 1] = g;
    colors[3 * i + 2] = b;
  }
}
