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

#include <sstream>

#include "DREAM3DLib/Common/DREAM3DMath.h"
#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Common/NeighborList.hpp"
#include "DREAM3DLib/Common/IDataArray.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindNeighbors::FindNeighbors() :
m_GrainIds(NULL),
m_SurfaceVoxels(NULL),
m_SurfaceFields(NULL),
m_Phases(NULL),
m_NumNeighbors(NULL),
m_TotalSurfaceArea(NULL),
m_NeighborList(NULL),
m_SharedSurfaceAreaList(NULL)
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
void FindNeighbors::preflight()
{
  int err = 0;
  std::stringstream ss;
  DataContainer::Pointer m = DataContainer::New();
  IDataArray::Pointer d = m->getVoxelData(DREAM3D::VoxelData::GrainIds);
  if(d.get() == NULL)
  {
	  ss << "Graid Ids Array Not Initialized At Beginning of FindNeighbors Filter" << std::endl;
	  err = -300;
  }
  d = m->getFieldData(DREAM3D::FieldData::Phases);
  if(d.get() == NULL)
  {
	  ss << "Phases (Fields) Array Not Initialized At Beginning of FindNeighbors Filter" << std::endl;
	  err = -300;
  }

  PFInt32ArrayType::Pointer p = PFInt32ArrayType::CreateArray(1);
  m->addVoxelData(DREAM3D::VoxelData::SurfaceVoxels, p);
  PFBoolArrayType::Pointer q = PFBoolArrayType::CreateArray(1);
  m->addFieldData(DREAM3D::FieldData::SurfaceFields, q);
  PFInt32ArrayType::Pointer r = PFInt32ArrayType::CreateArray(1);
  m->addFieldData(DREAM3D::FieldData::NumNeighbors, r);
  PFFloatArrayType::Pointer s = PFFloatArrayType::CreateArray(1);
  m->addEnsembleData(DREAM3D::EnsembleData::TotalSurfaceArea, s);
  NeighborList<int>::Pointer neighborlistPtr = NeighborList<int>::New();
  m->addFieldData(DREAM3D::FieldData::NeighborList, neighborlistPtr);
  NeighborList<float>::Pointer sharedSurfaceAreaListPtr = NeighborList<float>::New();
  m->addFieldData(DREAM3D::FieldData::SharedSurfaceAreaList, sharedSurfaceAreaListPtr);

  setErrorCondition(err);
  setErrorMessage(ss.str());
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindNeighbors::execute()
{
  setErrorCondition(0);

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
  m_GrainIds = m->getVoxelDataSizeCheck<int32_t, Int32ArrayType, AbstractFilter>(DREAM3D::VoxelData::GrainIds, totalPoints, this);
  if(NULL == m_GrainIds)
  {
    return;
  }
  m_SurfaceVoxels = m->getVoxelDataSizeCheck<int8_t, Int8ArrayType, AbstractFilter>(DREAM3D::VoxelData::SurfaceVoxels, totalPoints, this);
  if(NULL == m_SurfaceVoxels)
  {
    return;
  }

  size_t udims[3] = {0,0,0};
  m->getDimensions(udims);
#if (CMP_SIZEOF_SIZE_T == 4)
  typedef int32_t DimType;
#else
  typedef int64_t DimType;
#endif
  DimType dims[3] = {
    static_cast<DimType>(udims[0]),
    static_cast<DimType>(udims[1]),
    static_cast<DimType>(udims[2]),
  };

  int neighpoints[6];
  neighpoints[0] = -dims[0]*dims[1];
  neighpoints[1] = -dims[0];
  neighpoints[2] = -1;
  neighpoints[3] = 1;
  neighpoints[4] = dims[0];
  neighpoints[5] = dims[0]*dims[1];

  float column, row, plane;
  int grain;
  size_t nnum;
  int onsurf = 0;
  int good = 0;
  int neighbor = 0;
  size_t xtalCount = m->crystruct.size();

  m_TotalSurfaceArea = m->createEnsembleData<float, FloatArrayType, AbstractFilter>(DREAM3D::EnsembleData::TotalSurfaceArea, xtalCount, 1, this);
  if (NULL == m_TotalSurfaceArea) {return;}
  for (size_t i = 1; i < xtalCount; ++i)
  {
    m_TotalSurfaceArea[i] = 0.0f;
  }

    std::vector<std::vector<int> > neighborlist;
    std::vector<std::vector<float> > neighborsurfacearealist;

  notify("FindNeighbors: Working through all Grains", 0, Observable::UpdateProgressMessage);
  int nListSize = 100;
  neighborlist.resize(m->m_Grains.size());
  neighborsurfacearealist.resize(m->m_Grains.size());
  for (size_t i = 1; i < m->m_Grains.size(); i++)
  {
    m_NumNeighbors[i] = 0;
    neighborlist[i].resize(nListSize);
    neighborsurfacearealist[i].resize(nListSize, -1.0);
    for (int j = 0; j < 3; j++)
    {
      m_Neighborhoods[i] = 0;
    }
  }

  notify("FindNeighbors: Working through all Voxels", 0, Observable::UpdateProgressMessage);
  totalPoints = m->totalPoints();

  for (int64_t j = 0; j < totalPoints; j++)
  {
    onsurf = 0;
    grain = m_GrainIds[j];
    if(grain > 0)
    {
      column = j % m->getXPoints();
      row = (j / m->getXPoints()) % m->getYPoints();
      plane = j / (m->getXPoints() * m->getYPoints());
      if((column == 0 || column == (m->getXPoints() - 1) || row == 0 || row == (m->getYPoints() - 1) || plane == 0 || plane == (m->getZPoints() - 1)) && m->getZPoints() != 1)
      {
        m_SurfaceFields[grain] = true;
      }
      if((column == 0 || column == (m->getXPoints() - 1) || row == 0 || row == (m->getYPoints() - 1)) && m->getZPoints() == 1)
      {
        m_SurfaceFields[grain] = true;
      }
      for (int k = 0; k < 6; k++)
      {
        good = 1;
        neighbor = j + neighpoints[k];
        if(k == 0 && plane == 0) good = 0;
        if(k == 5 && plane == (m->getZPoints() - 1)) good = 0;
        if(k == 1 && row == 0) good = 0;
        if(k == 4 && row == (m->getYPoints() - 1)) good = 0;
        if(k == 2 && column == 0) good = 0;
        if(k == 3 && column == (m->getXPoints() - 1)) good = 0;
        if(good == 1 && m_GrainIds[neighbor] != grain && m_GrainIds[neighbor] > 0)
        {
          onsurf++;
          nnum = m_NumNeighbors[grain];
          neighborlist[grain].push_back(m_GrainIds[neighbor]);
          nnum++;
          m_NumNeighbors[grain] = nnum;
        }
      }
    }
    m_SurfaceVoxels[j] = onsurf;
  }

  NeighborList<int>::Pointer neighborlistPtr = NeighborList<int>::New();
  m->addFieldData(DREAM3D::FieldData::NeighborList, neighborlistPtr);
  NeighborList<float>::Pointer sharedSurfaceAreaListPtr = NeighborList<float>::New();
  m->addFieldData(DREAM3D::FieldData::SharedSurfaceAreaList, sharedSurfaceAreaListPtr);

  notify("FindNeighbors: Working through all Grains - Second Time", 0, Observable::UpdateProgressMessage);
  for (size_t i = 1; i < m->m_Grains.size(); i++)
  {
    int phase = m_Phases[i];

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
      float area = number * m->getXRes() * m->getYRes();
      if(m_SurfaceFields[i] == 0 && (neigh > i || m_SurfaceFields[neigh] == 1))
      {
        m_TotalSurfaceArea[phase] = m_TotalSurfaceArea[phase] + area;
      }

      // Push the neighbor grain id back onto the list so we stay synced up
      neighborlist[i].push_back(neigh);
      neighborsurfacearealist[i].push_back(area);
    }
    m_NumNeighbors[i] = neighborlist[i].size();

    // Set the vector for each list into the NeighborList Object
    NeighborList<int>::SharedVectorType sharedNeiLst(new std::vector<int>);
    sharedNeiLst->assign(neighborlist[i].begin(), neighborlist[i].end());
    m_NeighborList->setList(i, sharedNeiLst);

    NeighborList<float>::SharedVectorType sharedSAL(new std::vector<float>);
    sharedSAL->assign(neighborsurfacearealist[i].begin(), neighborsurfacearealist[i].end());
    m_SharedSurfaceAreaList->setList(i, sharedSAL);
  }

  notify("FindNeighbors Completed", 0, Observable::UpdateProgressMessage);
}

