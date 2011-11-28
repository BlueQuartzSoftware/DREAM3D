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

#include "CleanupGrains.h"
#include <map>
#include <assert.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

#include <cstddef>
#include <vector>
#include <string>
#include <iostream>
#include <cmath>
#include <fstream>
#include <list>
#include <algorithm>
#include <numeric>

#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Common/DREAM3DMath.h"
#include "DREAM3DLib/Common/OrientationMath.h"
#include "DREAM3DLib/Common/DREAM3DRandom.h"

#include "DREAM3DLib/OrientationOps/CubicOps.h"
#include "DREAM3DLib/OrientationOps/HexagonalOps.h"
#include "DREAM3DLib/OrientationOps/OrthoRhombicOps.h"

#include "DREAM3DLib/GenericFilters/FindNeighbors.h"

#define ERROR_TXT_OUT 1
#define ERROR_TXT_OUT1 1

const static float m_pi = M_PI;

using namespace std;

#define NEW_SHARED_ARRAY(var, type, size)\
  boost::shared_array<type> var##Array(new type[size]);\
  type* var = var##Array.get();

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CleanupGrains::CleanupGrains() :
    m_ErrorCondition(0)
{
  Seed = MXA::getMilliSeconds();

  m_HexOps = HexagonalOps::New();
  m_OrientationOps.push_back(m_HexOps.get());
  m_CubicOps = CubicOps::New();
  m_OrientationOps.push_back(m_CubicOps.get());
  m_OrthoOps = OrthoRhombicOps::New();
  m_OrientationOps.push_back(m_OrthoOps.get());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CleanupGrains::~CleanupGrains()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CleanupGrains::execute()
{

	int err = 0;
  DREAM3D_RANDOMNG_NEW()

  notify("Cleanup Grains - Removing Small Grains", 0, Observable::UpdateProgressMessage);
  remove_smallgrains();

  notify("Cleanup Grains - Assigning Bad Points", 0, Observable::UpdateProgressMessage);
  assign_badpoints();

  FindNeighbors::Pointer find_neighbors = FindNeighbors::New();
  find_neighbors->setObservers(this->getObservers());
  find_neighbors->setDataContainer(m_DataContainer);
  find_neighbors->execute();
  err = find_neighbors->getErrorCondition();

  notify("Cleanup Grains - Merging Grains", 0, Observable::UpdateProgressMessage);
  merge_containedgrains();

  notify("Cleanup Grains - Reorder Grains", 0, Observable::UpdateProgressMessage);
  reorder_grains();

  find_neighbors->execute();
  err = find_neighbors->getErrorCondition();

  // If there is an error set this to something negative and also set a message
  m_ErrorMessage = "CleanupGrains Completed";
  m_ErrorCondition = 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CleanupGrains::assign_badpoints()
{
  vector<int > neighs;
  vector<int > remove;
  int count = 1;
  int good = 1;
  int neighbor;
  int index = 0;
  float x, y, z;
  int column, row, plane;
  int neighpoint;
  int neighpoints[6];
  size_t numgrains = m_DataContainer->m_Grains.size();
  neighpoints[0] = -m_DataContainer->xpoints * m_DataContainer->ypoints;
  neighpoints[1] = -m_DataContainer->xpoints;
  neighpoints[2] = -1;
  neighpoints[3] = 1;
  neighpoints[4] = m_DataContainer->xpoints;
  neighpoints[5] = m_DataContainer->xpoints * m_DataContainer->ypoints;
  vector<int> currentvlist;

  notify("Assigning Bad Voxels", 0, Observable::UpdateProgressMessage);

  for (int iter = 0; iter < (m_DataContainer->totalpoints); iter++)
  {
    m_DataContainer->alreadychecked[iter] = false;
	if (m_DataContainer->grain_indicies[iter] > 0) m_DataContainer->alreadychecked[iter] = true;
  }
  for (int i = 0; i < m_DataContainer->totalpoints; i++)
  {
		if(m_DataContainer->alreadychecked[i] == false && m_DataContainer->grain_indicies[i] == 0)
		{
			currentvlist.push_back(i);
			count = 0;
			while(count < currentvlist.size())
			{
				index = currentvlist[count];
				column = index % m_DataContainer->xpoints;
				row = (index / m_DataContainer->xpoints) % m_DataContainer->ypoints;
				plane = index / (m_DataContainer->xpoints * m_DataContainer->ypoints);
				for (int j = 0; j < 6; j++)
				{
					good = 1;
					neighbor = index + neighpoints[j];
					if (j == 0 && plane == 0) good = 0;
					if (j == 5 && plane == (m_DataContainer->zpoints - 1)) good = 0;
					if (j == 1 && row == 0) good = 0;
					if (j == 4 && row == (m_DataContainer->ypoints - 1)) good = 0;
					if (j == 2 && column == 0) good = 0;
					if (j == 3 && column == (m_DataContainer->xpoints - 1)) good = 0;
					if (good == 1 && m_DataContainer->grain_indicies[neighbor] <= 0 && m_DataContainer->alreadychecked[neighbor] == false)
					{
						currentvlist.push_back(neighbor);
						m_DataContainer->alreadychecked[neighbor] = true;
					}
				}
				count++;
			}
			if(currentvlist.size() >= m_minallowedgrainsize*100)
			{
				for (size_t k = 0; k < currentvlist.size(); k++)
				{
					m_DataContainer->grain_indicies[currentvlist[k]] = 0;
					m_DataContainer->phases[currentvlist[k]] = 0;
				}
				m_DataContainer->m_Grains[0]->phase = 0;
			}
			if(currentvlist.size() < m_minallowedgrainsize*100)
			{
				for (size_t k = 0; k < currentvlist.size(); k++)
				{
					m_DataContainer->grain_indicies[currentvlist[k]] = -1;
					m_DataContainer->phases[currentvlist[k]] = 0;
				}
			}
			currentvlist.clear();
		}
  }

  std::vector<int > n(numgrains + 1);
  while (count != 0)
  {
    count = 0;
    for (int i = 0; i < m_DataContainer->totalpoints; i++)
    {
      int grainname = m_DataContainer->grain_indicies[i];
      if (grainname < 0)
      {
        count++;
        for (size_t c = 1; c < numgrains; c++)
        {
          n[c] = 0;
        }
        x = i % m_DataContainer->xpoints;
        y = (i / m_DataContainer->xpoints) % m_DataContainer->ypoints;
        z = i / (m_DataContainer->xpoints * m_DataContainer->ypoints);
        for (int j = 0; j < 6; j++)
        {
          good = 1;
          neighpoint = i + neighpoints[j];
          if (j == 0 && z == 0) good = 0;
          if (j == 5 && z == (m_DataContainer->zpoints - 1)) good = 0;
          if (j == 1 && y == 0) good = 0;
          if (j == 4 && y == (m_DataContainer->ypoints - 1)) good = 0;
          if (j == 2 && x == 0) good = 0;
          if (j == 3 && x == (m_DataContainer->xpoints - 1)) good = 0;
          if (good == 1)
          {
            int grain = m_DataContainer->grain_indicies[neighpoint];
			if (grain >= 0)
            {
              neighs.push_back(grain);
            }
          }
        }
        int current = 0;
        int most = 0;
        int curgrain = 0;
        int size = int(neighs.size());
        for (int k = 0; k < size; k++)
        {
          int neighbor = neighs[k];
          n[neighbor]++;
          current = n[neighbor];
          if (current > most)
          {
            most = current;
            curgrain = neighbor;
          }
        }
        if (size > 0)
        {
          m_DataContainer->neighbors[i] = curgrain;
          neighs.clear();
        }
      }
    }
    for (int j = 0; j < m_DataContainer->totalpoints; j++)
    {
      int grainname = m_DataContainer->grain_indicies[j];
      int neighbor = m_DataContainer->neighbors[j];
      if (grainname < 0 && neighbor > 0)
      {
        m_DataContainer->grain_indicies[j] = neighbor;
		m_DataContainer->phases[j] = m_DataContainer->m_Grains[neighbor]->phase;
      }
    }
//    std::stringstream ss;
//     ss << "Assigning Bad Voxels count = " << count;
//    notify(ss.str().c_str(), 0, Observable::UpdateProgressMessage);
  }
}


void CleanupGrains::merge_containedgrains()
{
  for (int i = 0; i < (m_DataContainer->xpoints * m_DataContainer->ypoints * m_DataContainer->zpoints); i++)
  {
    int grainname = m_DataContainer->grain_indicies[i];
	if (m_DataContainer->m_Grains[grainname]->numneighbors == 1 && m_DataContainer->m_Grains[grainname]->phase > 0)
    {
      m_DataContainer->m_Grains[grainname]->gotcontainedmerged = true;
      m_DataContainer->grain_indicies[i] = m_DataContainer->m_Grains[grainname]->neighborlist->at(0);
      m_DataContainer->m_Grains[m_DataContainer->m_Grains[grainname]->neighborlist->at(0)]->numvoxels++;
    }
	if (m_DataContainer->m_Grains[grainname]->numneighbors == 0 && m_DataContainer->m_Grains[grainname]->phase > 0)
	{
      m_DataContainer->m_Grains[grainname]->gotcontainedmerged = true;
      m_DataContainer->grain_indicies[i] = 0;
	}
  }

}

void CleanupGrains::reorder_grains()
{
  size_t initialVoxelsListSize = 1000;
  size_t size = 0;
  int neighpoints[6];
  int good = 0;
  int neighbor = 0;
  int col, row, plane;
  int gnum;
  float q1[5];
  float q2[5];
  float q[5];
  float ea1, ea2, ea3;
  size_t currentgrain = 1;
  Ebsd::CrystalStructure phase;

  neighpoints[0] = -m_DataContainer->xpoints * m_DataContainer->ypoints;
  neighpoints[1] = -m_DataContainer->xpoints;
  neighpoints[2] = -1;
  neighpoints[3] = 1;
  neighpoints[4] = m_DataContainer->xpoints;
  neighpoints[5] = m_DataContainer->xpoints * m_DataContainer->ypoints;
  size_t numgrains = m_DataContainer->m_Grains.size();

  size_t maxGrain = 0;
  // Reset all the Grain nucleus values to -1;
  for (size_t i = 1; i < numgrains; i++)
  {
    m_DataContainer->m_Grains[i]->nucleus = -1;
	m_DataContainer->m_Grains[i]->gotcontainedmerged = false;
  }

  // Reset the "already checked" to 0 for all voxels
  for (int i = 0; i < m_DataContainer->totalpoints; i++)
  {
    m_DataContainer->alreadychecked[i] = false;
    gnum = m_DataContainer->grain_indicies[i];
    m_DataContainer->m_Grains[gnum]->nucleus = i;
  }
  for (size_t i = 1; i < numgrains; i++)
  {
    if(m_DataContainer->m_Grains[i]->nucleus != -1)
    {
      size = 0;
      int nucleus = m_DataContainer->m_Grains[i]->nucleus;
	  if(m_DataContainer->phases[nucleus] > 0) phase = m_DataContainer->crystruct[m_DataContainer->phases[nucleus]];
	  if(m_DataContainer->phases[nucleus] <= 0) phase = Ebsd::UnknownCrystalStructure;
      if(m_DataContainer->m_Grains[currentgrain]->voxellist != NULL)
      {
          delete m_DataContainer->m_Grains[currentgrain]->voxellist;
      }
      m_DataContainer->m_Grains[currentgrain]->voxellist = new std::vector<int>(initialVoxelsListSize,-1);
      m_DataContainer->m_Grains[currentgrain]->voxellist->at(size) = nucleus;
      m_DataContainer->alreadychecked[nucleus] = true;
      m_DataContainer->grain_indicies[nucleus] = currentgrain;
      if (currentgrain > maxGrain) maxGrain = currentgrain;
      size++;
      for (size_t k = 0; k < 5; k++)
      {
        m_DataContainer->m_Grains[currentgrain]->avg_quat[k] = 0.0;
      }
      for (size_t j = 0; j < size; j++)
      {
        int currentpoint = m_DataContainer->m_Grains[currentgrain]->voxellist->at(j);
        col = currentpoint % m_DataContainer->xpoints;
        row = (currentpoint / m_DataContainer->xpoints) % m_DataContainer->ypoints;
        plane = currentpoint / (m_DataContainer->xpoints * m_DataContainer->ypoints);
        for (int k = 0; k < 5; k++)
        {
            q1[k] = m_DataContainer->quats[nucleus*5 + k];
            q2[k] = m_DataContainer->quats[currentpoint*5 + k];
        }
        if(m_DataContainer->phases[nucleus] > 0) m_OrientationOps[phase]->getNearestQuat(q1,q2);
		if(m_DataContainer->phases[nucleus] <= 0) q2[0] = 1.0, q2[1] = 0.0, q2[2] = 0.0, q2[3] = 0.0, q2[4] = 0.0;
        for (int k = 0; k < 5; k++)
        {
            m_DataContainer->quats[currentpoint*5 + k] = q2[k];
            m_DataContainer->m_Grains[currentgrain]->avg_quat[k] = m_DataContainer->m_Grains[currentgrain]->avg_quat[k] + m_DataContainer->quats[currentpoint*5 + k];
        }
        for (int k = 0; k < 6; k++)
        {
          good = 1;
          neighbor = currentpoint + neighpoints[k];
          if (k == 0 && plane == 0) good = 0;
          if (k == 5 && plane == (m_DataContainer->zpoints - 1)) good = 0;
          if (k == 1 && row == 0) good = 0;
          if (k == 4 && row == (m_DataContainer->ypoints - 1)) good = 0;
          if (k == 2 && col == 0) good = 0;
          if (k == 3 && col == (m_DataContainer->xpoints - 1)) good = 0;
          if (good == 1 && m_DataContainer->alreadychecked[neighbor] == false)
          {
            size_t grainname = m_DataContainer->grain_indicies[neighbor];
            if (grainname == i)
            {
              m_DataContainer->m_Grains[currentgrain]->voxellist->at(size) = neighbor;
              m_DataContainer->alreadychecked[neighbor] = true;
              m_DataContainer->grain_indicies[neighbor] = currentgrain;
              if (currentgrain > maxGrain) maxGrain = currentgrain;
              size++;
              if (size >= m_DataContainer->m_Grains[currentgrain]->voxellist->size())
              {
                m_DataContainer->m_Grains[currentgrain]->voxellist->resize(size + initialVoxelsListSize, -1);
              }
            }
          }
        }
      }
      m_DataContainer->m_Grains[currentgrain]->voxellist->erase(std::remove(m_DataContainer->m_Grains[currentgrain]->voxellist->begin(), m_DataContainer->m_Grains[currentgrain]->voxellist->end(), -1), m_DataContainer->m_Grains[currentgrain]->voxellist->end());
      m_DataContainer->m_Grains[currentgrain]->active = 1;
      m_DataContainer->m_Grains[currentgrain]->nucleus = nucleus;
	  m_DataContainer->m_Grains[currentgrain]->phase = m_DataContainer->phases[nucleus];
      q[1] = m_DataContainer->m_Grains[currentgrain]->avg_quat[1]/m_DataContainer->m_Grains[currentgrain]->avg_quat[0];
      q[2] = m_DataContainer->m_Grains[currentgrain]->avg_quat[2]/m_DataContainer->m_Grains[currentgrain]->avg_quat[0];
      q[3] = m_DataContainer->m_Grains[currentgrain]->avg_quat[3]/m_DataContainer->m_Grains[currentgrain]->avg_quat[0];
      q[4] = m_DataContainer->m_Grains[currentgrain]->avg_quat[4]/m_DataContainer->m_Grains[currentgrain]->avg_quat[0];
	  OrientationMath::QuattoEuler(q, ea1, ea2, ea3);
	  m_DataContainer->m_Grains[currentgrain]->euler1 = ea1;
      m_DataContainer->m_Grains[currentgrain]->euler2 = ea2;
      m_DataContainer->m_Grains[currentgrain]->euler3 = ea3;
      currentgrain++;
    }
  }

  for (int i = 0; i < m_DataContainer->totalpoints; i++)
  {
	if(m_DataContainer->grain_indicies[i] >= (int)(currentgrain) )
    {
      m_DataContainer->grain_indicies[i] = -2;
    }
  }
  assign_badpoints();

  numgrains = currentgrain;
  // Resize the m_DataContainer->m_Grains vector by the appropriate amount
  m_DataContainer->m_Grains.resize(currentgrain);
}

void CleanupGrains::remove_smallgrains()
{
  size_t initialVoxelsListSize = 1000;
  std::vector<int > voxelslist;
  voxelslist.resize(initialVoxelsListSize,-1);
  size_t size = 0;
  int neighpoints[6];
  int good = 0;
  int neighbor = 0;
  int col, row, plane;
  int gnum;
  int currentgrain = 1;
  neighpoints[0] = -m_DataContainer->xpoints * m_DataContainer->ypoints;
  neighpoints[1] = -m_DataContainer->xpoints;
  neighpoints[2] = -1;
  neighpoints[3] = 1;
  neighpoints[4] = m_DataContainer->xpoints;
  neighpoints[5] = m_DataContainer->xpoints * m_DataContainer->ypoints;
  int numgrains = m_DataContainer->m_Grains.size();
  for (int i = 0; i < (m_DataContainer->xpoints * m_DataContainer->ypoints * m_DataContainer->zpoints); i++)
  {
    m_DataContainer->alreadychecked[i] = false;
	gnum = m_DataContainer->grain_indicies[i];
	if(gnum >= 0) m_DataContainer->m_Grains[gnum]->nucleus = i;
  }
  for (size_t i = 1; i < numgrains; i++)
  {
      size = 0;
      int nucleus = m_DataContainer->m_Grains[i]->nucleus;
      voxelslist[size] = nucleus;
      m_DataContainer->alreadychecked[nucleus] = true;
      m_DataContainer->grain_indicies[nucleus] = currentgrain;
      size++;
      for (size_t j = 0; j < size; j++)
      {
        int currentpoint = voxelslist[j];
        col = currentpoint % m_DataContainer->xpoints;
        row = (currentpoint / m_DataContainer->xpoints) % m_DataContainer->ypoints;
        plane = currentpoint / (m_DataContainer->xpoints * m_DataContainer->ypoints);
        for (size_t k = 0; k < 6; k++)
        {
          good = 1;
          neighbor = currentpoint + neighpoints[k];
          if (k == 0 && plane == 0) good = 0;
          if (k == 5 && plane == (m_DataContainer->zpoints - 1)) good = 0;
          if (k == 1 && row == 0) good = 0;
          if (k == 4 && row == (m_DataContainer->ypoints - 1)) good = 0;
          if (k == 2 && col == 0) good = 0;
          if (k == 3 && col == (m_DataContainer->xpoints - 1)) good = 0;
          if (good == 1 && m_DataContainer->alreadychecked[neighbor] == false)
          {
            size_t grainname = static_cast<size_t>(m_DataContainer->grain_indicies[neighbor]);
            if (grainname == i)
            {
              voxelslist[size] = neighbor;
              m_DataContainer->alreadychecked[neighbor] = true;
              m_DataContainer->grain_indicies[neighbor] = currentgrain;
              size++;
              if (size >= voxelslist.size()) voxelslist.resize(size + initialVoxelsListSize, -1);
            }
          }
        }
      }
      voxelslist.erase(std::remove(voxelslist.begin(), voxelslist.end(), -1), voxelslist.end());
    if(voxelslist.size() >= static_cast<size_t>(m_minallowedgrainsize) )
    {
      m_DataContainer->m_Grains[currentgrain]->active = true;
      currentgrain++;
      voxelslist.clear();
      voxelslist.resize(initialVoxelsListSize,-1);
    }
    if(voxelslist.size() < static_cast<size_t>(m_minallowedgrainsize) )
    {
        for (size_t b = 0; b < voxelslist.size(); b++)
        {
          int index = voxelslist[b];
          m_DataContainer->grain_indicies[index] = 0;
        }
        voxelslist.resize(initialVoxelsListSize, -1);
    }
  }
  m_DataContainer->m_Grains.resize(currentgrain);
}


