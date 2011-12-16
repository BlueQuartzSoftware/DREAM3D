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

#include "LoadVolume.h"

#include "DREAM3DLib/Common/DREAM3DMath.h"
#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/HDF5/H5VoxelReader.h"

const static float m_pi = M_PI;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
LoadVolume::LoadVolume()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
LoadVolume::~LoadVolume()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void LoadVolume::execute()
{
  DataContainer* m = getDataContainer();
  setErrorCondition(0);
  int err = 0;

  H5VoxelReader::Pointer h5Reader = H5VoxelReader::New();
  h5Reader->setFileName(getInputFile());
  int dims[3];
  float spacing[3];
  float origin[3];
  err = h5Reader->getSizeResolutionOrigin(dims, spacing, origin);
  if (err < 0)
  {
    return;
  }

  m->xpoints = dims[0];
  m->ypoints = dims[1];
  m->zpoints = dims[2];
  m->totalpoints = dims[0] * dims[1] * dims[2];
  m->resx = spacing[0];
  m->resy = spacing[1];
  m->resz = spacing[2];
  initializeAttributes();
  err = h5Reader->readVoxelData(m->m_GrainIndicies, m->m_Phases, m->m_Euler1s, m->m_Euler2s, m->m_Euler3s, m->crystruct, m->phaseType, m->totalpoints);
  m->phasefraction.resize(m->crystruct.size());
  m->pptFractions.resize(m->crystruct.size());
  initializeGrains();

  notify("LoadVolume Completed", 0, Observable::UpdateProgressMessage);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void LoadVolume::initializeGrains()
{
  /*
   * This assumes a dense packed grain list which probably isn't that safe based
   * on some of the data that is floating around. For our data though this is a
   * reasonable assumption.
   */
  // Put at least 1 Grain in the Vector
  DataContainer* m = getDataContainer();
  m->m_Grains.resize(1);
  size_t curGrainSize = 1;
  size_t grainIndex = 0;
  Field::Pointer grain;
  int totpoints = m->totalpoints;
  for (int i = 0; i < totpoints; ++i)
  {
    grainIndex = m->grain_indicies[i];
    curGrainSize = m->m_Grains.size();
    if (grainIndex > m->m_Grains.size() - 1)
    {
      // Resize the Grain Vector to be as large as this index. The other Grain
      // objects will be copied to the resized Vector. This probably isn't really
      // efficient at all.
      m->m_Grains.resize(grainIndex + 1);
    }
    curGrainSize = m->m_Grains.size();
    grain = m->m_Grains[grainIndex];
    if (NULL == grain.get())
    {
      m->m_Grains[grainIndex] = Field::New();
      grain = m->m_Grains[grainIndex];
      grain->phase = m->phases[i];
    }
    grain->numvoxels++;
	grain->active = true;
  }

  // Loop over the Grains and initialize them as necessary
  size_t gSize = m->m_Grains.size();
  for (size_t g = 0; g < gSize; ++g)
  {
    grain = m->m_Grains[g];
    if (NULL == grain.get())
    {
      m->m_Grains[g] = Field::New();
      grain = m->m_Grains[g];
	  grain->phase = 0;
    }
  }
}


void LoadVolume::initializeAttributes()
{
  DataContainer* m = getDataContainer();
  m->grain_indicies = m->m_GrainIndicies->WritePointer(0, m->totalpoints);
  m->phases = m->m_Phases->WritePointer(0, m->totalpoints);
  m->euler1s = m->m_Euler1s->WritePointer(0, m->totalpoints);
  m->euler2s = m->m_Euler2s->WritePointer(0, m->totalpoints);
  m->euler3s = m->m_Euler3s->WritePointer(0, m->totalpoints);

  m->surfacevoxels = m->m_SurfaceVoxels->WritePointer(0, m->totalpoints);
  m->neighbors = m->m_Neighbors->WritePointer(0, m->totalpoints);
  for (int i = 0; i < m->totalpoints; ++i)
  {
    m->euler1s[i] = -1.0f;
    m->euler2s[i] = -1.0f;
    m->euler3s[i] = -1.0f;
    m->neighbors[i] = -1.0f;
  }
  m->quats = m->m_Quats->WritePointer(0, m->totalpoints*5);
  m->m_Quats->SetNumberOfComponents(5);
}


