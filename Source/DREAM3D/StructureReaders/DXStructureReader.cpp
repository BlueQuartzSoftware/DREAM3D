/* ============================================================================
 * Copyright (c) 2011, Michael A. Jackson (BlueQuartz Software)
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
 * Neither the name of Michael A. Jackson nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
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
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "DXStructureReader.h"

#include <string.h>

#include <boost/shared_ptr.hpp>

#include "MXA/Common/LogTime.h"
#include "MXA/Common/MXAEndian.h"

#include "DREAM3D/DREAM3DConfiguration.h"

#include "DREAM3D/GrainGenerator/GrainGeneratorFunc.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DXStructureReader::DXStructureReader()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DXStructureReader::~DXStructureReader()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int DXStructureReader::readStructure(GrainGeneratorFunc* m)
{
  int err = 0;
  if (m_InputFileName.empty() == true)
  {
    std::cout << "DXStructureReader Error: Filename was not set" << std::endl;
    return -1;
  }

  FILE* file = fopen(m_InputFileName.c_str(), "r");
  if (NULL == file)
  {
    std::cout << "DXStructureReader Error: Could not open file '" << m_InputFileName << "'" << std::endl;
    return -2;
  }

  char object[16]; ::memset(object, 0, 16);
  int objIndex = 0;
  char cls[16]; ::memset(cls, 0, 16);
  char clsName[256]; ::memset(clsName, 0, 256);
  char counts[16]; ::memset(counts, 0, 16);
  int xdim = 0;
  int ydim = 0;
  int zdim = 0;

  // Read the first line
  int nFields = fscanf(file, "%s %d %s %s %s %d %d %d\n", object, &objIndex, cls, clsName, counts, &xdim, &ydim, &zdim );
  if (nFields != 8)
  {
    std::cout << "DXStructureReader Error: Line 1 not enough fields. Expected 8, got " << nFields << std::endl;
    fclose(file);
    return -3;
  }

  // Read Lines 2
  char origin[16]; ::memset(origin, 0, 16);
  int x, y, z;
  nFields = fscanf(file, "%s %d %d %d\n", origin, &x, &y, &z);
  if (nFields != 4)
  {
    std::cout << "DXStructureReader Error: Line 2 not enough fields. Expected 8, got " << nFields << std::endl;
    fclose(file);
    return -4;
  }

  // Read Lines 3,4,5
  char delta[16]; ::memset(delta, 0, 16);
  for (int i = 0; i < 3; ++i) {
    nFields = fscanf(file, "%s %d %d %d\n", delta, &x, &y, &z);
    if (nFields != 4)
    {
      std::cout << "DXStructureReader Error: Line 3 not enough fields. Expected 8, got " << nFields << std::endl;
      fclose(file);
      return -5;
    }
  }

  // Read the empty line
  fscanf(file, "\n");

  // Read line 7
  nFields = fscanf(file, "%s %d %s %s %s %d %d %d\n", object, &objIndex, cls, clsName, counts, &x, &y, &z );
  if (nFields != 8)
  {
    std::cout << "DXStructureReader Error: Line 1 not enough fields. Expected 8, got " << nFields << std::endl;
    fclose(file);
    return -3;
  }

  // Read the empty line at 8
  fscanf(file, "\n");

  // Read line 9
  char objType[16]; ::memset(objType, 0, 16);
  char dType[16]; ::memset(dType, 0, 16);
  char rankLabel[16]; ::memset(rankLabel, 0, 16);
  int rank = 0;
  char itemLabel[16]; ::memset(itemLabel, 0, 16);
  int itemCount = 0;
  char t1[8]; ::memset(t1, 0, 8);
  char t2[8]; ::memset(t2, 0, 8);
  nFields = fscanf(file, "%s %d %s %s %s %s %s %d %s %d %s %s\n",
                   object, &objIndex, cls, clsName, objType, dType, rankLabel, &rank, itemLabel, &itemCount, t1, t2 );
  if (nFields != 12)
  {
    std::cout << "DXStructureReader Error: Line 9 not enough fields. Expected 12, got " << nFields << std::endl;
    fclose(file);
    return -3;
  }

  m->resx = m->resy = m->resz = 1.0;
  // Allocate all the Voxels
  m->xpoints = xdim-1;
  m->ypoints = ydim-1;
  m->zpoints = zdim-1;
  m->totalpoints = m->xpoints * m->ypoints * m->zpoints;
  m->totalvol = float(m->totalpoints)*m->resx*m->resy*m->resz;
  m->sizex = m->xpoints * m->resx;
  m->sizey = m->ypoints * m->resy;
  m->sizez = m->zpoints * m->resz;
  m->voxels.reset(new GrainGeneratorVoxel[m->totalpoints]);


  std::map<int, int> grainIdMap;
  int grain_index = 0;
  for (int i = 0; i < itemCount; ++i)
  {
    fscanf(file, "%d", &grain_index);
    m->voxels[i].grain_index = grain_index;
    m->voxels[i].phase = 1;
    grainIdMap[grain_index]++;
  }
  // We now have our list of grains so allocate that many grains
  // Currently we assume a dense array starting at index 1. Index Zero is
  // NOT used, at least internally
  m->m_Grains.resize(grainIdMap.size() + 1);
  size_t g = 0;
  for (std::map<int, int>::iterator iter = grainIdMap.begin(); iter != grainIdMap.end(); ++iter )
  {
    //int gid = (*iter).first;
    m->m_Grains[g] = Grain::New();
    m->m_Grains[g]->numvoxels = (*iter).second;
    m->m_Grains[g]->active = 1;
    ++g;
  }

  fclose(file);
  file = NULL;
  return err;
}
