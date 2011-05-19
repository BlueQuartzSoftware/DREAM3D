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
#include "VTKStructureReader.h"

#include <fstream>

#include "AIM/Common/GrainGeneratorFunc.h"

#define kBufferSize 1024

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VTKStructureReader::VTKStructureReader()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VTKStructureReader::~VTKStructureReader()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VTKStructureReader::readStructure(GrainGeneratorFunc* m)
{
  int err = 0;

  err = readHeader();
  getDims(m->xpoints, m->ypoints, m->zpoints);
  getScaling(m->resx, m->resy, m->resz);
  m->totalpoints = m->xpoints * m->ypoints * m->zpoints;
  m->totalvol = float(m->totalpoints)*m->resx*m->resy*m->resz;
  m->sizex = m->xpoints * m->resx;
  m->sizey = m->ypoints * m->resy;
  m->sizez = m->zpoints * m->resz;
  m->voxels.reset(new GrainGeneratorVoxel[m->totalpoints]);

  std::string filename = getInputFileName();
  std::ifstream instream;
  instream.open(filename.c_str());
  if (!instream.is_open())
   {
     std::cout << logTime() << " vtk file could not be opened: " << filename << std::endl;
     return -1;
   }
  char buf[kBufferSize];
  for (int i = 0; i < 8; ++i)
  {
    instream.getline(buf, kBufferSize);
  }

  // Now we need to search for the 'GrainID' and
  char text1[kBufferSize]; ::memset(text1, 0, kBufferSize);
  char text2[kBufferSize]; ::memset(text2, 0, kBufferSize);
  char text3[kBufferSize]; ::memset(text3, 0, kBufferSize);
  int fieldNum = 0;
  bool needGrainIds = true;
  bool needPhaseIds = true;
  std::string scalarName;
  int typeByteSize = 0;
  int planeVoxels = m->xpoints * m->ypoints;
  size_t index = 0;
  int* ids = (int*)(malloc(sizeof(int) * planeVoxels));
  int i = 0;
  while (needGrainIds == true && needPhaseIds == true)
  {
    instream.getline(buf, kBufferSize); // Read Line 9
    ::memset(buf, 0, kBufferSize);

    readLine(instream, buf, kBufferSize); // Read Line 10
    int n = sscanf(buf, "%s %s %s %d", text1, text2, text3, &fieldNum);
    if (n != 4)
    {
      std::cout << "Error reading SCALARS header section of VTK file." << std::endl;
      return -1;
    }
    scalarName = std::string(text2);
    typeByteSize = parseByteSize(text3);
    if (typeByteSize == 0)
    {
      return -1;
    }
    ::memset(text1, 0, kBufferSize);
    ::memset(text2, 0, kBufferSize);
    ::memset(text3, 0, kBufferSize);
    readLine(instream, buf, kBufferSize); // Read Line 11

    // Check to make sure we are reading the correct set of scalars and if we are
    // NOT then read all this particular Scalar Data and try again


    if (m_GrainIdScalarName.compare(scalarName) == 0)
    {
      std::map<int, int> grainIdMap;
      for (int z = 0; z < m->zpoints; ++z)
      {
        // Read an entire plane of data
        instream.read(reinterpret_cast<char*>(ids), sizeof(int) * planeVoxels);
        i = 0;
        for (int y = 0; y < m->ypoints; ++y)
        {
          for (int x = 0; x < m->xpoints; ++x)
          {
            MXA::Endian::FromBigToSystem::convert<int>(ids[i]);
            m->voxels[index].grain_index = ids[i];
            grainIdMap[ids[i]]++;
            ++i;
            ++index;
          }
        }
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
      needGrainIds = false;
    }
    else if ( m_PhaseIdScalarName.compare(scalarName) == 0)
    {
      for (int z = 0; z < m->zpoints; ++z)
      {
        // Read an entire plane of data
        instream.read(reinterpret_cast<char*>(ids), sizeof(int) * planeVoxels);
        i = 0;
        for (int y = 0; y < m->ypoints; ++y)
        {
          for (int x = 0; x < m->xpoints; ++x)
          {
            MXA::Endian::FromBigToSystem::convert<int>(ids[i]);
            m->voxels[index].phase = ids[i];
            ++i;
            ++index;
          }
        }
      }
      needPhaseIds = false;
    }
    else
    {
      for (int z = 0; z < m->zpoints - 1; ++z)
      {
        skipVolume(instream, typeByteSize, m->xpoints, m->ypoints, m->zpoints);
      }
    }

  }

  free(ids);
  instream.close();
  return err;
}


