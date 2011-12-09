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

#include <map>


#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Common/DREAM3DMath.h"
#include "DREAM3DLib/Common/DREAM3DRandom.h"

#include "DREAM3DLib/HDF5/H5VoxelReader.h"
#include "DREAM3DLib/HDF5/H5VoxelWriter.h"





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
  int err = 0;
  setErrorCondition(err);

  crop_volume();

  // If there is an error set this to something negative and also set a message
    notify("CropVolume Completed", 0, Observable::UpdateProgressMessage);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CropVolume::crop_volume()
{
  DataContainer* m = getDataContainer();
  DREAM3D_RANDOMNG_NEW()
  setErrorCondition(0);
  notify("Starting Crop Volume", 0, Observable::UpdateProgressValueAndMessage);
  int err = 1;
  int counter = 0;
  float x, y, z;
  int col, row, plane;
  int index;
  int index_old;
  for (int i = 0; i < m_ZP; i++)
  {
    notify("Cropping Percent", ((float)i/m->zpoints)*100, Observable::UpdateProgressValue);
    for (int j = 0; j < m_YP; j++)
    {
      for (int k = 0; k < m_XP; k++)
      {
        x = (k * m->resx) + m_XStart;
        y = (j * m->resy) + m_YStart;
        z = (i * m->resz) + m_ZStart;
        col = int(x / m->resx);
        row = int(y / m->resy);
        plane = int(z / m->resz);
		index_old = (plane * m->xpoints * m->ypoints) + (row * m->xpoints) + col;
        index = (i * m_XP * m_YP) + (j * m_XP) + k;
        m->grain_indicies[index] = m->grain_indicies[index_old];
        m->phases[index] = m->phases[index_old];
        m->euler1s[index] = m->euler1s[index_old];
        m->euler2s[index] = m->euler2s[index_old];
        m->euler3s[index] = m->euler3s[index_old];
      }
    }
  }
  m->xpoints = m_XP;
  m->ypoints = m_YP;
  m->zpoints = m_ZP;
  m->totalpoints = m_XP*m_YP*m_ZP;
  err = m->m_GrainIndicies->Resize(m->totalpoints);
  err = m->m_Phases->Resize(m->totalpoints);
  err = m->m_Euler1s->Resize(m->totalpoints);
  err = m->m_Euler2s->Resize(m->totalpoints);
  err = m->m_Euler3s->Resize(m->totalpoints);
  m->grain_indicies = m->m_GrainIndicies->GetPointer(0);
  m->phases = m->m_Phases->GetPointer(0);
  m->euler1s = m->m_Euler1s->GetPointer(0);
  m->euler2s = m->m_Euler2s->GetPointer(0);
  m->euler3s = m->m_Euler3s->GetPointer(0);

  notify("Crop Volume Complete", 100, Observable::UpdateProgressValueAndMessage);
}