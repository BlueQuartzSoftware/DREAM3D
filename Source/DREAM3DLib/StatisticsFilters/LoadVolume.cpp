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
  if (NULL == m)
  {
    setErrorCondition(-1);
    std::stringstream ss;
    ss << getNameOfClass() << " DataContainer was NULL";
    setErrorMessage(ss.str());
    return;
  }
  int64_t totalPoints = m->totalPoints();

  setErrorCondition(0);
  int err = 0;

  H5VoxelReader::Pointer h5Reader = H5VoxelReader::New();
  h5Reader->setFileName(getInputFile());
  int64_t dims[3];
  float spacing[3];
  float origin[3];
  err = h5Reader->getSizeResolutionOrigin(dims, spacing, origin);
  if (err < 0)
  {
    return;
  }

  /* Sanity check what we are trying to load to make sure it can fit in our address space.
   * Note that this does not guarantee the user has enough left, just that the
   * size of the volume can fit in the address space of the program
   */
  size_t max = std::numeric_limits<size_t>::max();
  if (dims[0] * dims[1] * dims[2] > max )
  {
    err = -1;
    std::stringstream s;
    s << "The total number of elements '" << (dims[0] * dims[1] * dims[2])
                << "' is greater than this program can hold. Try the 64 bit version.";
    setErrorCondition(err);
    setErrorMessage(s.str());
    return;
  }

  if (dims[0] > max || dims[1] > max || dims[2] > max)
  {
    err = -1;
    std::stringstream s;
    s << "One of the dimensions is greater than the max index for this sysem. Try the 64 bit version.";
    s << " dim[0]="<< dims[0] << "  dim[1]="<<dims[1] << "  dim[2]=" << dims[2];
    setErrorCondition(err);
    setErrorMessage(s.str());
    return;
  }
  /* ************ End Sanity Check *************************** */
  size_t dcDims[3] = {dims[0], dims[1], dims[2]};

  m->setDimensions(dcDims);
  m->setResolution(spacing);
  totalPoints = m->totalPoints();

  initializeAttributes();
  if (getErrorCondition() < 0)
  {
    return;
  }


  err = h5Reader->readVoxelData(grain_indicies, phases, euler1s, euler2s, euler3s, m->crystruct, m->phaseType, totalPoints);
  m->phasefraction.resize(m->crystruct.size());
  m->pptFractions.resize(m->crystruct.size());
  initializeGrains();
  if (getErrorCondition() < 0)
  {
    return;
  }
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
//  size_t curGrainSize = 1;
  size_t grainIndex = 0;
  Field::Pointer grain;
  int64_t totalPoints = m->totalPoints();
  for (int i = 0; i < totalPoints; ++i)
  {
    grainIndex = grain_indicies[i];
  //  curGrainSize = m->m_Grains.size();
    if (grainIndex > m->m_Grains.size() - 1)
    {
      // Resize the Grain Vector to be as large as this index. The other Grain
      // objects will be copied to the resized Vector. This probably isn't really
      // efficient at all.
      m->m_Grains.resize(grainIndex + 1);
    }
  //  curGrainSize = m->m_Grains.size();
    grain = m->m_Grains[grainIndex];
    if (NULL == grain.get())
    {
      m->m_Grains[grainIndex] = Field::New();
      grain = m->m_Grains[grainIndex];
      grain->phase = phases[i];
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

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void LoadVolume::initializeAttributes()
{
  DataContainer* m = getDataContainer();
  int64_t totalPoints = m->totalPoints();

  grain_indicies = m->createVoxelData<int32_t, Int32ArrayType, AbstractFilter>(DREAM3D::VoxelData::GrainIds, totalPoints, 1, this);
  if (grain_indicies == NULL) { return; }
  phases = m->createVoxelData<int32_t, Int32ArrayType, AbstractFilter>(DREAM3D::VoxelData::Phases, totalPoints, 1, this);
  if (phases == NULL) { return; }
  euler1s = m->createVoxelData<float, FloatArrayType, AbstractFilter>(DREAM3D::VoxelData::Euler1, totalPoints, 1, this);
  if (NULL == euler1s) {return;}
  euler2s = m->createVoxelData<float, FloatArrayType, AbstractFilter>(DREAM3D::VoxelData::Euler2, totalPoints, 1, this);
  if (NULL == euler2s) {return;}
  euler3s = m->createVoxelData<float, FloatArrayType, AbstractFilter>(DREAM3D::VoxelData::Euler3, totalPoints, 1, this);
  if (NULL == euler3s) {return;}
  surfacevoxels = m->createVoxelData<int8_t, Int8ArrayType, AbstractFilter>(DREAM3D::VoxelData::SurfaceVoxels, totalPoints, 1, this);
  if (NULL == surfacevoxels) {return;}
  neighbors = m->createVoxelData<int32_t, Int32ArrayType, AbstractFilter>(DREAM3D::VoxelData::Neighbors, totalPoints, 1, this);
  if (neighbors == NULL) { return; }
  quats = m->createVoxelData<float, FloatArrayType, AbstractFilter>(DREAM3D::VoxelData::Quats, totalPoints*5, 1, this);
  if (NULL == quats) {return;}

  for (int i = 0; i < totalPoints; ++i)
  {
    euler1s[i] = -1.0f;
    euler2s[i] = -1.0f;
    euler3s[i] = -1.0f;
    neighbors[i] = -1.0f;
  }

}


