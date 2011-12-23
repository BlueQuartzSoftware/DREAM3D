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

#include "FindNeighbors.h"

#include "DREAM3DLib/Common/DREAM3DMath.h"
#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Common/NeighborList.hpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindNeighbors::FindNeighbors()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindNeighbors::~FindNeighbors()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindNeighbors::execute()
{
  setErrorCondition(0);

  find_neighbors();
  notify("FindNeighbors Completed", 0, Observable::UpdateProgressMessage);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindNeighbors::find_neighbors()
{
  DataContainer* m = getDataContainer();
  if(NULL == m)
  {
    setErrorCondition(-1);
    std::stringstream ss;
    ss << getNameOfClass() << " DataContainer was NULL";
    setErrorMessage(ss.str());
    return;
  }

  int64_t totalPoints = m->totalPoints();
  int32_t* grain_indicies = m->getVoxelDataSizeCheck<int32_t, Int32ArrayType, AbstractFilter>(DREAM3D::VoxelData::GrainIds, totalPoints, this);
  if(NULL == grain_indicies)
  {
    return;
  }
  int8_t* surfacevoxels = m->getVoxelDataSizeCheck<int8_t, Int8ArrayType, AbstractFilter>(DREAM3D::VoxelData::SurfaceVoxels, totalPoints, this);
  if(NULL == surfacevoxels)
  {
    return;
  }

  int neighpoints[6];
  neighpoints[0] = -(m->xpoints * m->ypoints);
  neighpoints[1] = -m->xpoints;
  neighpoints[2] = -1;
  neighpoints[3] = 1;
  neighpoints[4] = m->xpoints;
  neighpoints[5] = (m->xpoints * m->ypoints);
  float column, row, plane;
  int grain;
  size_t nnum;
  int onsurf = 0;
  int good = 0;
  int neighbor = 0;
  size_t xtalCount = m->crystruct.size();


  float* totalsurfacearea = m->createEnsembleData<float, FloatArrayType, AbstractFilter>(DREAM3D::EnsembleData::TotalSurfaceArea, xtalCount, 1, this);
  if (NULL == totalsurfacearea) {return;}
  for (size_t i = 1; i < xtalCount; ++i)
  {
    totalsurfacearea[i] = 0.0f;
  }

    std::vector<std::vector<float> > neighborlist;
    std::vector<std::vector<float> > neighborsurfacearealist;

  notify("FindNeighbors: Working through all Grains", 0, Observable::UpdateProgressMessage);
  int nListSize = 100;
  neighborlist.resize(m->m_Grains.size());
  neighborsurfacearealist.resize(m->m_Grains.size());
  for (size_t i = 1; i < m->m_Grains.size(); i++)
  {
    m->m_Grains[i]->numneighbors = 0;
    neighborlist[i].resize(nListSize);
    neighborsurfacearealist[i].resize(nListSize, -1.0);
    for (int j = 0; j < 3; j++)
    {
      m->m_Grains[i]->neighbordistfunc[j] = 0;
    }
  }

  notify("FindNeighbors: Working through all Voxels", 0, Observable::UpdateProgressMessage);
  totalPoints = m->totalPoints();

  for (int64_t j = 0; j < totalPoints; j++)
  {
    onsurf = 0;
    grain = grain_indicies[j];
    if(grain > 0)
    {
      column = j % m->xpoints;
      row = (j / m->xpoints) % m->ypoints;
      plane = j / (m->xpoints * m->ypoints);
      if((column == 0 || column == (m->xpoints - 1) || row == 0 || row == (m->ypoints - 1) || plane == 0 || plane == (m->zpoints - 1)) && m->zpoints != 1)
      {
        m->m_Grains[grain]->surfacefield = true;
      }
      if((column == 0 || column == (m->xpoints - 1) || row == 0 || row == (m->ypoints - 1)) && m->zpoints == 1)
      {
        m->m_Grains[grain]->surfacefield = true;
      }
      for (int k = 0; k < 6; k++)
      {
        good = 1;
        neighbor = j + neighpoints[k];
        if(k == 0 && plane == 0) good = 0;
        if(k == 5 && plane == (m->zpoints - 1)) good = 0;
        if(k == 1 && row == 0) good = 0;
        if(k == 4 && row == (m->ypoints - 1)) good = 0;
        if(k == 2 && column == 0) good = 0;
        if(k == 3 && column == (m->xpoints - 1)) good = 0;
        if(good == 1 && grain_indicies[neighbor] != grain && grain_indicies[neighbor] > 0)
        {
          onsurf++;
          nnum = m->m_Grains[grain]->numneighbors;
          neighborlist[grain].push_back(grain_indicies[neighbor]);
          nnum++;
          m->m_Grains[grain]->numneighbors = nnum;
        }
      }
    }
    surfacevoxels[j] = onsurf;
  }

  NeighborList<int>::Pointer neighborlistPtr = NeighborList<int>::New();
  m->addFieldData(DREAM3D::FieldData::NeighborList, neighborlistPtr);
  NeighborList<float>::Pointer sharedSurfaceAreaListPtr = NeighborList<float>::New();
  m->addFieldData(DREAM3D::FieldData::SharedSurfaceAreaList, sharedSurfaceAreaListPtr);

  notify("FindNeighbors: Working through all Grains - Second Time", 0, Observable::UpdateProgressMessage);
  for (size_t i = 1; i < m->m_Grains.size(); i++)
  {
    int phase = m->m_Grains[i]->phase;

    std::map<int, int> neighToCount;
    int numneighs = int(neighborlist[i].size());

    // this increments the voxel counts for each grain
    for (int j = 0; j < numneighs; j++)
    {
      neighToCount[neighborlist[i][j]]++;
    }

    neighToCount.erase(0);
    neighToCount.erase(-1);
    //Resize the grains neighbor list to zero
    neighborlist[i].resize(0);
    neighborsurfacearealist[i].resize(0);

    for (std::map<int, int>::iterator iter = neighToCount.begin(); iter != neighToCount.end(); ++iter)
    {
      int neigh = iter->first; // get the neighbor grain
      int number = iter->second; // get the number of voxels
      float area = number * m->resx * m->resy;
      if(m->m_Grains[i]->surfacefield == 0 && (neigh > i || m->m_Grains[neigh]->surfacefield == 1))
      {
        totalsurfacearea[phase] = totalsurfacearea[phase] + area;
      }

      // Push the neighbor grain id back onto the list so we stay synced up
      neighborlist[i].push_back(neigh);
      neighborsurfacearealist[i].push_back(area);
    }
    m->m_Grains[i]->numneighbors = neighborlist[i].size();

    // Set the vector for each list into the NeighborList Object
    NeighborList<int>::SharedVectorType sharedNeiLst(new std::vector<int>);
    sharedNeiLst->assign(neighborlist[i].begin(), neighborlist[i].end());
    neighborlistPtr->setList(i, sharedNeiLst);

    NeighborList<float>::SharedVectorType sharedSAL(new std::vector<float>);
    sharedSAL->assign(neighborsurfacearealist[i].begin(), neighborsurfacearealist[i].end());
    sharedSurfaceAreaListPtr->setList(i, sharedSAL);
  }
}

