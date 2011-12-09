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

#include "ChangeResolution.h"

#include <map>


#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Common/DREAM3DMath.h"
#include "DREAM3DLib/Common/DREAM3DRandom.h"

#include "DREAM3DLib/HDF5/H5VoxelReader.h"
#include "DREAM3DLib/HDF5/H5VoxelWriter.h"





// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ChangeResolution::ChangeResolution()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ChangeResolution::~ChangeResolution()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ChangeResolution::execute()
{
  int err = 0;
  setErrorCondition(err);
  DREAM3D_RANDOMNG_NEW()

  change_resolution();

  // If there is an error set this to something negative and also set a message
    notify("ChangeResolution Completed", 0, Observable::UpdateProgressMessage);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ChangeResolution::change_resolution()
{
  DataContainer* m = getDataContainer();
  DREAM3D_RANDOMNG_NEW()
  setErrorCondition(0);
  notify("Starting Resolution Change", 0, Observable::UpdateProgressValueAndMessage);
  int counter = 0;
  float x, y, z;
  int col, row, plane;
  int index;
  int index_old;
  for (int i = 0; i < m_ZP; i++)
  {
    notify("Changing Percent", ((float)i/m->zpoints)*100, Observable::UpdateProgressValue);
    for (int j = 0; j < m_YP; j++)
    {
      for (int k = 0; k < m_XP; k++)
      {
        x = (k * m_XRes);
        y = (j * m_YRes);
        z = (i * m_ZRes);
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
  notify("Resolution Change Complete", 100, Observable::UpdateProgressValueAndMessage);
}