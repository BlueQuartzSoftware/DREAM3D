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


#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Common/DREAM3DMath.h"
#include "DREAM3DLib/Common/OrientationMath.h"
#include "DREAM3DLib/Common/DREAM3DRandom.h"

#include "DREAM3DLib/OrientationOps/CubicOps.h"
#include "DREAM3DLib/OrientationOps/HexagonalOps.h"
#include "DREAM3DLib/OrientationOps/OrthoRhombicOps.h"

#include "DREAM3DLib/GenericFilters/FindNeighbors.h"

const static float m_pi = M_PI;

#define NEW_SHARED_ARRAY(var, type, size)\
  boost::shared_array<type> var##Array(new type[size]);\
  type* var = var##Array.get();

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CleanupGrains::CleanupGrains() :
m_GrainIds(NULL),
m_Quats(NULL),
m_EulerAngles(NULL),
m_AvgQuats(NULL),
m_PhasesC(NULL),
m_PhasesF(NULL),
m_NumNeighbors(NULL),
m_Neighbors(NULL),
m_Active(NULL),
m_AlreadyChecked(NULL),
m_NeighborList(NULL)
{
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
void CleanupGrains::preflight()
{
  int err = 0;
  std::stringstream ss;
  DataContainer::Pointer m = DataContainer::New();
  IDataArray::Pointer d = m->getVoxelData(DREAM3D::VoxelData::GrainIds);
  if(d.get() == NULL)
  {
	  ss << "GrainIds Array Not Initialized At Beginning of CleanupGrains Filter" << std::endl;
	  err = -300;
  }
  d = m->getVoxelData(DREAM3D::VoxelData::Phases);
  if(d.get() == NULL)
  {
	  ss << "Phases (Cells) Array Not Initialized At Beginning of CleanupGrains Filter" << std::endl;
	  err = -300;
  }
  d = m->getVoxelData(DREAM3D::VoxelData::Quats);
  if(d.get() == NULL)
  {
	  ss << "Quats Array Not Initialized At Beginning of CleanupGrains Filter" << std::endl;
	  err = -300;
  }
  d = m->getFieldData(DREAM3D::FieldData::Phases);
  if(d.get() == NULL)
  {
	  ss << "Phases (Field) Array Not Initialized At Beginning of CleanupGrains Filter" << std::endl;
	  err = -300;
  }
  d = m->getFieldData(DREAM3D::FieldData::NeighborList);
  if(d.get() == NULL)
  {
	  ss << "NeighborLists Array Not Initialized At Beginning of CleanupGrains Filter" << std::endl;
	  err = -300;
  }
  d = m->getFieldData(DREAM3D::FieldData::NumNeighbors);
  if(d.get() == NULL)
  {
	  ss << "NumNeighbor Array Not Initialized At Beginning of CleanupGrains Filter" << std::endl;
	  err = -300;
  }

  PFBoolArrayType::Pointer p = PFBoolArrayType::CreateArray(1);
  m->addVoxelData(DREAM3D::VoxelData::AlreadyChecked, p);
  PFInt32ArrayType::Pointer q = PFInt32ArrayType::CreateArray(1);
  m->addVoxelData(DREAM3D::VoxelData::Neighbors, q);
  PFFloatArrayType::Pointer r = PFFloatArrayType::CreateArray(1);
  m->addFieldData(DREAM3D::FieldData::AvgQuats, r);
  PFFloatArrayType::Pointer s = PFFloatArrayType::CreateArray(1);
  m->addFieldData(DREAM3D::FieldData::EulerAngles, s);
  PFBoolArrayType::Pointer t = PFBoolArrayType::CreateArray(1);
  m->addFieldData(DREAM3D::FieldData::Active, t);

  setErrorCondition(err);
  setErrorMessage(ss.str());
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CleanupGrains::execute()
{
  setErrorCondition(0);
  int err = 0;
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

	// Make sure we have all the arrays available and allocated
  m_GrainIds = m->getVoxelDataSizeCheck<int32_t, Int32ArrayType, AbstractFilter>(DREAM3D::VoxelData::GrainIds, totalPoints, this);
  if (NULL == m_GrainIds) { return; }
  m_PhasesC = m->getVoxelDataSizeCheck<int32_t, Int32ArrayType, AbstractFilter>(DREAM3D::VoxelData::Phases, totalPoints, this);
  if (NULL == m_PhasesC) { return; }
  m_AlreadyChecked = m->getVoxelDataSizeCheck<bool, BoolArrayType, AbstractFilter>(DREAM3D::VoxelData::AlreadyChecked, totalPoints, this);
  if (NULL == m_AlreadyChecked) { return; }
  m_Quats = m->getVoxelDataSizeCheck<float, FloatArrayType, AbstractFilter>(DREAM3D::VoxelData::Quats, totalPoints * 5, this);
  if (NULL == m_Quats) { return; }
  m_Neighbors = m->getVoxelDataSizeCheck<int32_t, Int32ArrayType, AbstractFilter>(DREAM3D::VoxelData::Neighbors, totalPoints, this);
  if (NULL == m_Neighbors) { return; }


  notify("Cleanup Grains - Removing Small Grains", 0, Observable::UpdateProgressMessage);
  remove_smallgrains();

  notify("Cleanup Grains - Assigning Bad Points", 0, Observable::UpdateProgressMessage);
  assign_badpoints();

  FindNeighbors::Pointer find_neighbors = FindNeighbors::New();
  find_neighbors->setObservers(this->getObservers());
  find_neighbors->setDataContainer(m);
  find_neighbors->execute();
  err = find_neighbors->getErrorCondition();
  if (err < 0)
  {
    return;
  }

  notify("Cleanup Grains - Merging Grains", 0, Observable::UpdateProgressMessage);
  merge_containedgrains();

  notify("Cleanup Grains - Reorder Grains", 0, Observable::UpdateProgressMessage);
  reorder_grains();

  // If there is an error set this to something negative and also set a message
  notify("CleanupGrains Completed", 0, Observable::UpdateProgressMessage);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CleanupGrains::assign_badpoints()
{
  DataContainer* m = getDataContainer();
  int64_t totalPoints = m->totalPoints();
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

  std::vector<int > neighs;
  std::vector<int > remove;
  size_t count = 1;
  int good = 1;
  int neighbor;
  int index = 0;
  float x, y, z;
  size_t column, row, plane;
  int neighpoint;
  size_t numgrains = m->getTotalFields();

  int neighpoints[6];
  neighpoints[0] = -dims[0] * dims[1];
  neighpoints[1] = -dims[0];
  neighpoints[2] = -1;
  neighpoints[3] = 1;
  neighpoints[4] = dims[0];
  neighpoints[5] = dims[0] * dims[1];
  std::vector<int> currentvlist;

  notify("Assigning Bad Voxels", 0, Observable::UpdateProgressMessage);

  for (int64_t iter = 0; iter < totalPoints; iter++)
  {
    m_AlreadyChecked[iter] = false;
	if (m_GrainIds[iter] > 0) m_AlreadyChecked[iter] = true;
  }
  for (int64_t i = 0; i < totalPoints; i++)
  {
		if(m_AlreadyChecked[i] == false && m_GrainIds[i] == 0)
		{
			currentvlist.push_back(i);
			count = 0;
			while(count < currentvlist.size())
			{
				index = currentvlist[count];
				column = index % dims[0];
				row = (index / dims[0]) % dims[1];
				plane = index / (dims[0] * dims[1]);
				for (size_t j = 0; j < 6; j++)
				{
					good = 1;
					neighbor = index + neighpoints[j];
					if (j == 0 && plane == 0) good = 0;
					if (j == 5 && plane == (dims[2] - 1)) good = 0;
					if (j == 1 && row == 0) good = 0;
					if (j == 4 && row == (dims[1] - 1)) good = 0;
					if (j == 2 && column == 0) good = 0;
					if (j == 3 && column == (dims[0] - 1)) good = 0;
					if (good == 1 && m_GrainIds[neighbor] <= 0 && m_AlreadyChecked[neighbor] == false)
					{
						currentvlist.push_back(neighbor);
						m_AlreadyChecked[neighbor] = true;
					}
				}
				count++;
			}
			if((int)currentvlist.size() >= m_minallowedgrainsize*100)
			{
				for (size_t k = 0; k < currentvlist.size(); k++)
				{
					m_GrainIds[currentvlist[k]] = 0;
					m_PhasesC[currentvlist[k]] = 0;
				}
				m_PhasesF[0] = 0;
			}
			if((int)currentvlist.size() < m_minallowedgrainsize*100)
			{
				for (size_t k = 0; k < currentvlist.size(); k++)
				{
					m_GrainIds[currentvlist[k]] = -1;
					m_PhasesC[currentvlist[k]] = 0;
				}
			}
			currentvlist.clear();
		}
  }

  std::vector<int > n(numgrains + 1);
  while (count != 0)
  {
    count = 0;
    for (int i = 0; i < totalPoints; i++)
    {
      int grainname = m_GrainIds[i];
      if (grainname < 0)
      {
        count++;
        for (size_t c = 1; c < numgrains; c++)
        {
          n[c] = 0;
        }
        x = static_cast<float>(i % dims[0]);
        y = static_cast<float>((i / dims[0]) % dims[1]);
        z = static_cast<float>(i / (dims[0] * dims[1]));
        for (int j = 0; j < 6; j++)
        {
          good = 1;
          neighpoint = i + neighpoints[j];
          if (j == 0 && z == 0) good = 0;
          if (j == 5 && z == (dims[2] - 1)) good = 0;
          if (j == 1 && y == 0) good = 0;
          if (j == 4 && y == (dims[1] - 1)) good = 0;
          if (j == 2 && x == 0) good = 0;
          if (j == 3 && x == (dims[0] - 1)) good = 0;
          if (good == 1)
          {
            int grain = m_GrainIds[neighpoint];
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
          m_Neighbors[i] = curgrain;
          neighs.clear();
        }
      }
    }
    for (int j = 0; j < totalPoints; j++)
    {
      int grainname = m_GrainIds[j];
      int neighbor = m_Neighbors[j];
      if (grainname < 0 && neighbor > 0)
      {
        m_GrainIds[j] = neighbor;
		m_PhasesC[j] = m_PhasesF[neighbor];
      }
    }
//    std::stringstream ss;
//     ss << "Assigning Bad Voxels count = " << count;
//    notify(ss.str().c_str(), 0, Observable::UpdateProgressMessage);
  }
}


void CleanupGrains::merge_containedgrains()
{
  // Since this method is called from the 'execute' and the DataContainer validity
  // was checked there we are just going to get the Shared Pointer to the DataContainer
  DataContainer* m = getDataContainer();
  // Now we are going to get a "Pointer" to the NeighborList object out of the DataContainer
  m_NeighborList = NeighborList<int>::SafeObjectDownCast<IDataArray*, NeighborList<int>* >(m->getFieldData(DREAM3D::FieldData::NeighborList).get());
  // But since a pointer is difficult to use operators with we will now create a
  // reference variable to the pointer with the correct variable name that allows
  // us to use the same syntax as the "vector of vectors"
  NeighborList<int>& neighborlist = *m_NeighborList;

  size_t totalPoints = static_cast<size_t>(m->totalPoints());
  for (size_t i = 0; i < totalPoints; i++)
  {
    int grainname = m_GrainIds[i];
    if(m_NumNeighbors[grainname] == 1 && m_PhasesF[grainname] > 0)
    {
      m_GrainIds[i] = neighborlist[grainname][0];
    }
    if(m_NumNeighbors[grainname] == 0 && m_PhasesF[grainname] > 0)
    {
      m_GrainIds[i] = 0;
    }
  }

}

void CleanupGrains::reorder_grains()
{
  DataContainer* m = getDataContainer();
  int64_t totalPoints = m->totalPoints();
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

  size_t initialVoxellistsSize = 1000;
  size_t size = 0;
  int neighpoints[6];
  int good = 0;
  int neighbor = 0;
  size_t col, row, plane;
  int gnum;
  float q1[5];
  float q2[5];
  float q[5];
  float ea1, ea2, ea3;
  size_t currentgrain = 1;
  Ebsd::CrystalStructure phase;

  neighpoints[0] = -dims[0] * dims[1];
  neighpoints[1] = -dims[0];
  neighpoints[2] = -1;
  neighpoints[3] = 1;
  neighpoints[4] = dims[0];
  neighpoints[5] = dims[0] * dims[1];
  size_t numgrains = m->getTotalFields();
  nuclei.resize(numgrains, -1);

  size_t maxGrain = 0;
  // Reset all the Grain nucleus values to -1;
  for (size_t i = 1; i < numgrains; i++)
  {
    nuclei[i] = -1;
  }

  // Reset the "already checked" to 0 for all voxels
  for (int i = 0; i < totalPoints; i++)
  {
    m_AlreadyChecked[i] = false;
    gnum = m_GrainIds[i];
    nuclei[gnum] = i;
  }
  voxellists.resize(numgrains);
  for (size_t i = 1; i < numgrains; i++)
  {
    voxellists[i].clear();
    voxellists[i].resize(initialVoxellistsSize);
    if(nuclei[i] != -1)
    {
      size = 0;
      int nucleus = nuclei[i];
	  if(m_PhasesC[nucleus] > 0) phase = m->crystruct[m_PhasesC[nucleus]];
	  if(m_PhasesC[nucleus] <= 0) phase = Ebsd::UnknownCrystalStructure;
      voxellists[currentgrain][size] = nucleus;
      m_AlreadyChecked[nucleus] = true;
      m_GrainIds[nucleus] = currentgrain;

      if (currentgrain > maxGrain) maxGrain = currentgrain;
      size++;
      for (size_t k = 0; k < 5; k++)
      {
        m_AvgQuats[5*currentgrain+k] = 0.0;
      }
      for (size_t j = 0; j < size; j++)
      {
        int currentpoint = voxellists[currentgrain][j];
        col = currentpoint % dims[0];
        row = (currentpoint / dims[0]) % dims[1];
        plane = currentpoint / (dims[0] * dims[1]);
        for (int k = 0; k < 5; k++)
        {
            q1[k] = m_Quats[nucleus*5 + k];
            q2[k] = m_Quats[currentpoint*5 + k];
        }
        if(m_PhasesC[nucleus] > 0) m_OrientationOps[phase]->getNearestQuat(q1,q2);
		if(m_PhasesC[nucleus] <= 0) q2[0] = 1.0, q2[1] = 0.0, q2[2] = 0.0, q2[3] = 0.0, q2[4] = 0.0;
        for (int k = 0; k < 5; k++)
        {
            m_Quats[currentpoint*5 + k] = q2[k];
            m_AvgQuats[5*currentgrain+k] = m_AvgQuats[5*currentgrain+k] + m_Quats[currentpoint*5 + k];
        }
        for (int k = 0; k < 6; k++)
        {
          good = 1;
          neighbor = currentpoint + neighpoints[k];
          if (k == 0 && plane == 0) good = 0;
          if (k == 5 && plane == (dims[2] - 1)) good = 0;
          if (k == 1 && row == 0) good = 0;
          if (k == 4 && row == (dims[1] - 1)) good = 0;
          if (k == 2 && col == 0) good = 0;
          if (k == 3 && col == (dims[0] - 1)) good = 0;
          if (good == 1 && m_AlreadyChecked[neighbor] == false)
          {
            size_t grainname = m_GrainIds[neighbor];
            if (grainname == i)
            {

              voxellists[currentgrain][size] = neighbor;
              m_AlreadyChecked[neighbor] = true;
              m_GrainIds[neighbor] = currentgrain;

              if (currentgrain > maxGrain) maxGrain = currentgrain;
              size++;
              if (size >= voxellists[currentgrain].size())
              {
                voxellists[currentgrain].resize(size + initialVoxellistsSize, -1);
              }
            }
          }
        }
      }
      voxellists[currentgrain].erase(std::remove(voxellists[currentgrain].begin(), voxellists[currentgrain].end(), -1), voxellists[currentgrain].end());
      m_Active[currentgrain] = 1;
	  m_PhasesF[currentgrain] = m_PhasesC[nucleus];
      q[1] = m_AvgQuats[5*currentgrain+1]/m_AvgQuats[5*currentgrain];
      q[2] = m_AvgQuats[5*currentgrain+2]/m_AvgQuats[5*currentgrain];
      q[3] = m_AvgQuats[5*currentgrain+3]/m_AvgQuats[5*currentgrain];
      q[4] = m_AvgQuats[5*currentgrain+4]/m_AvgQuats[5*currentgrain];
	  OrientationMath::QuattoEuler(q, ea1, ea2, ea3);
	  m_EulerAngles[3*currentgrain] = ea1;
      m_EulerAngles[3*currentgrain+1] = ea2;
      m_EulerAngles[3*currentgrain+2] = ea3;
      currentgrain++;
    }
  }

  for (int i = 0; i < totalPoints; i++)
  {
	if(m_GrainIds[i] >= (int)(currentgrain) )
    {
      m_GrainIds[i] = -2;
    }
  }
  assign_badpoints();

  numgrains = currentgrain;
  // Resize the m->m_Grains vector by the appropriate amount
  m->resizeFieldDataArrays(currentgrain);
}

void CleanupGrains::remove_smallgrains()
{
  DataContainer* m = getDataContainer();
  int64_t totalPoints = m->totalPoints();
  size_t size = 0;
  
  int good = 0;
  int neighbor = 0;
  size_t col, row, plane;
  int gnum;
  int currentgrain = 1;

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

  int numgrains = m->getTotalFields();
  nuclei.resize(numgrains, -1);

  size_t maxGrain = 0;
  // Reset all the Grain nucleus values to -1;
  for (size_t i = 1; i < numgrains; i++)
  {
    nuclei[i] = -1;
  }  for (int64_t i = 0; i < totalPoints; i++)
  {
    m_AlreadyChecked[i] = false;
    gnum = m_GrainIds[i];
    if(gnum >= 0) nuclei[gnum] = i;
  }
  voxellists.resize(numgrains);
  for (size_t i = 1; i <  static_cast<size_t>(numgrains); i++)
  {
      size = 0;
      int nucleus = nuclei[i];
      voxellists[i].push_back(nucleus);
      m_AlreadyChecked[nucleus] = true;
      m_GrainIds[nucleus] = currentgrain;
      size++;
      for (size_t j = 0; j < size; j++)
      {
        int currentpoint = voxellists[i][j];
        col = currentpoint % dims[0];
        row = (currentpoint / dims[0]) % dims[1];
        plane = currentpoint / (dims[0] * dims[1]);
        for (size_t k = 0; k < 6; k++)
        {
          good = 1;
          neighbor = currentpoint + neighpoints[k];
          if (k == 0 && plane == 0) good = 0;
          if (k == 5 && plane == (dims[2] - 1)) good = 0;
          if (k == 1 && row == 0) good = 0;
          if (k == 4 && row == (dims[1] - 1)) good = 0;
          if (k == 2 && col == 0) good = 0;
          if (k == 3 && col == (dims[0] - 1)) good = 0;
          if (good == 1 && m_AlreadyChecked[neighbor] == false)
          {
            size_t grainname = static_cast<size_t>(m_GrainIds[neighbor]);
            if (grainname == i)
            {
              voxellists[i].push_back(neighbor);
              m_AlreadyChecked[neighbor] = true;
              m_GrainIds[neighbor] = currentgrain;
              size++;
            }
          }
        }
      }
      if(voxellists[i].size() >= static_cast<size_t>(m_minallowedgrainsize) )
      {
		m_Active[currentgrain] = true;
		currentgrain++;
      }
      if(voxellists[i].size() < static_cast<size_t>(m_minallowedgrainsize) )
      {
        for (size_t b = 0; b < voxellists[i].size(); b++)
        {
          int index = voxellists[i][b];
          m_GrainIds[index] = 0;
        }
      }
  }
  m->resizeFieldDataArrays(currentgrain);
}


