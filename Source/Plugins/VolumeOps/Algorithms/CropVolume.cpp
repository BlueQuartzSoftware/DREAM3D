/* ============================================================================
 * Copyright (c) 2011 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2011 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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
 *                           FA8650-07-D-5800
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "CropVolume.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CropVolume::CropVolume()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CropVolume::~CropVolume()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CropVolume::execute()
{
  setErrorCondition(0);
  notify("Starting Crop Volume", 0, Observable::UpdateProgressValueAndMessage);
  int counter = 0;
  float x, y, z;
  int col, row, plane;
  int index;
  int index_old;
  for (int i = 0; i < m_DataPtr->zpoints; i++)
  {
    notify("Cropping Percent", ((float)i/m_DataPtr->zpoints)*100, Observable::UpdateProgressValue);
    if (getCancel() == true) { break; }
    for (int j = 0; j < m_DataPtr->ypoints; j++)
    {
      for (int k = 0; k < m_DataPtr->xpoints; k++)
      {
        x = (k * m_DataPtr->resx) + m_DataPtr->xstart;
        y = (j * m_DataPtr->resy) + m_DataPtr->ystart;
        z = (i * m_DataPtr->resz) + m_DataPtr->zstart;
        col = int(x / m_DataPtr->resx_old);
        row = int(y / m_DataPtr->resy_old);
        plane = int(z / m_DataPtr->resz_old);
        index_old = (plane * m_DataPtr->xpoints_old * m_DataPtr->ypoints_old) + (row * m_DataPtr->xpoints_old) + col;
        index = (i * m_DataPtr->xpoints * m_DataPtr->ypoints) + (j * m_DataPtr->xpoints) + k;
        m_DataPtr->grain_indicies[index] = m_DataPtr->grain_indicies_old[index_old];
        m_DataPtr->phases[index] = m_DataPtr->phases_old[index_old];
        m_DataPtr->euler1s[index] = m_DataPtr->euler1s_old[index_old];
        m_DataPtr->euler2s[index] = m_DataPtr->euler2s_old[index_old];
        m_DataPtr->euler3s[index] = m_DataPtr->euler3s_old[index_old];
      }
    }
  }
  notify("Crop Volume Complete", 100, Observable::UpdateProgressValueAndMessage);
}

