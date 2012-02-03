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

#include "DREAM3DLib/PrivateFilters/FindNeighbors.h"
#include "DREAM3DLib/PrivateFilters/RenumberGrains.h"

const static float m_pi = static_cast<float>(M_PI);

#define NEW_SHARED_ARRAY(var, type, size)\
  boost::shared_array<type> var##Array(new type[size]);\
  type* var = var##Array.get();

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CleanupGrains::CleanupGrains() :
AbstractFilter(),
m_MinAllowedGrainSize(1),
m_MinNumNeighbors(1),
m_MisorientationTolerance(0.0f),
m_GrainIds(NULL),
m_PhasesC(NULL),
m_PhasesF(NULL),
m_Neighbors(NULL),
m_NumNeighbors(NULL),
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

  setupFilterOptions();
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
void CleanupGrains::setupFilterOptions()
{
  std::vector<FilterOption::Pointer> options;
  {
    FilterOption::Pointer option = FilterOption::New();
    option->setHumanLabel("Minimum Allowed Grain Size");
    option->setPropertyName("MinAllowedGrainSize");
    option->setWidgetType(FilterOption::IntWidget);
    option->setValueType("int");
    options.push_back(option);
  }
  {
    FilterOption::Pointer option = FilterOption::New();
    option->setHumanLabel("Minimum Number Neighbors");
    option->setPropertyName("MinNumNeighbors");
    option->setWidgetType(FilterOption::IntWidget);
    option->setValueType("int");
    options.push_back(option);
  }
  {
    FilterOption::Pointer option = FilterOption::New();
    option->setPropertyName("MisorientationTolerance");
    option->setHumanLabel("Misorientation Tolerance");
    option->setWidgetType(FilterOption::DoubleWidget);
    option->setValueType("float");
    options.push_back(option);
  }

  setFilterOptions(options);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CleanupGrains::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;
  DataContainer* m = getDataContainer();


  GET_PREREQ_DATA(m, DREAM3D, CellData, GrainIds, ss, -300, int32_t, Int32ArrayType, voxels, 1);
  GET_PREREQ_DATA_SUFFIX(m, DREAM3D, CellData, Phases, C, ss, -301, int32_t, Int32ArrayType, voxels, 1);
  CREATE_NON_PREREQ_DATA(m, DREAM3D, CellData, AlreadyChecked, ss, bool, BoolArrayType, voxels, 1);
  CREATE_NON_PREREQ_DATA(m, DREAM3D, CellData, Neighbors, ss, int32_t, Int32ArrayType, voxels, 1);

  GET_PREREQ_DATA_SUFFIX(m, DREAM3D, FieldData, Phases, F, ss, -303,  int32_t, Int32ArrayType, fields, 1);
  GET_PREREQ_DATA(m, DREAM3D, FieldData, NumNeighbors, ss, -350, int32_t, Int32ArrayType, fields, 1);
  CREATE_NON_PREREQ_DATA(m, DREAM3D, FieldData, Active, ss, bool, BoolArrayType, fields, 1);

  // Now we are going to get a "Pointer" to the NeighborList object out of the DataContainer
  m_NeighborList = NeighborList<int>::SafeObjectDownCast<IDataArray*, NeighborList<int>* >
                                          (m->getFieldData(DREAM3D::FieldData::NeighborList).get());
  if(m_NeighborList == NULL)
  {
    ss << "NeighborLists Array Not Initialized At Beginning of " << getNameOfClass() << " Filter" << std::endl;
    setErrorCondition(-350);
  }

  setErrorMessage(ss.str());
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CleanupGrains::preflight()
{
  DataContainer* m = getDataContainer();
//  m->clearFieldData();
//  m->clearEnsembleData();

  // Find Neighbors would be run first so run its PreFlight first before ours
  FindNeighbors::Pointer find_neighbors = FindNeighbors::New();
  find_neighbors->setObservers(this->getObservers());
  find_neighbors->setDataContainer(getDataContainer());
  find_neighbors->preflight();
  if (find_neighbors->getErrorCondition() < 0)
  {
    setErrorCondition(find_neighbors->getErrorCondition());
    setErrorMessage(find_neighbors->getErrorMessage());
    return;
  }

  dataCheck(true, 1, 1, 1);

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CleanupGrains::execute()
{
  setErrorCondition(0);
 // int err = 0;
  DataContainer* m = getDataContainer();
  if (NULL == m)
  {
    setErrorCondition(-1);
    std::stringstream ss;
    ss << getNameOfClass() << " DataContainer was NULL";
    setErrorMessage(ss.str());
    return;
  }
//  m->clearFieldData();
//  m->clearEnsembleData();

  int64_t totalPoints = m->totalPoints();
  dataCheck(false, totalPoints, m->getTotalFields(), m->getNumEnsembleTuples());
  if (getErrorCondition() < 0 && getErrorCondition() != -350)
  {
    return;
  }
  setErrorCondition(0);

  remove_smallgrains();

  assign_badpoints();

  FindNeighbors::Pointer find_neighbors = FindNeighbors::New();
  find_neighbors->setObservers(this->getObservers());
  find_neighbors->setDataContainer(m);
  find_neighbors->execute();
  int err = find_neighbors->getErrorCondition();
  if (err < 0)
  {
    setErrorCondition(find_neighbors->getErrorCondition());
    setErrorMessage(find_neighbors->getErrorMessage());
    return;
  }

  // FindNeighbors may have messed with the pointers so revalidate our internal pointers
  dataCheck(false, totalPoints, m->getTotalFields(), m->getNumEnsembleTuples());
  if (getErrorCondition() < 0)
  {
    return;
  }

  merge_containedgrains();

  RenumberGrains::Pointer renumber_grains = RenumberGrains::New();
  renumber_grains->setObservers(this->getObservers());
  renumber_grains->setDataContainer(m);
  renumber_grains->execute();
  err = renumber_grains->getErrorCondition();
  if (err < 0)
  {
    setErrorCondition(renumber_grains->getErrorCondition());
    setErrorMessage(renumber_grains->getErrorMessage());
    return;
  }


  // If there is an error set this to something negative and also set a message
  notify("Cleaning Up Grains Complete", 0, Observable::UpdateProgressMessage);
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
  DimType column, row, plane;
  int neighpoint;
  size_t numgrains = m->getTotalFields();

  int neighpoints[6];
  neighpoints[0] = static_cast<int>(-dims[0] * dims[1]);
  neighpoints[1] = static_cast<int>(-dims[0]);
  neighpoints[2] = static_cast<int>(-1);
  neighpoints[3] = static_cast<int>(1);
  neighpoints[4] = static_cast<int>(dims[0]);
  neighpoints[5] = static_cast<int>(dims[0] * dims[1]);
  std::vector<int> currentvlist;


  for (int64_t iter = 0; iter < totalPoints; iter++)
  {
    m_AlreadyChecked[iter] = false;
    if(m_GrainIds[iter] > 0) m_AlreadyChecked[iter] = true;
  }
  for (int64_t i = 0; i < totalPoints; i++)
  {
		std::stringstream ss;
	//	ss << "Cleaning Up Grains - Identifying Bad Points - " << ((float)i/totalPoints)*100 << " Percent Complete";
	//	notify(ss.str(), 0, Observable::UpdateProgressMessage);
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
				for (DimType j = 0; j < 6; j++)
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
			if((int)currentvlist.size() >= m_MinAllowedGrainSize*100)
			{
				for (size_t k = 0; k < currentvlist.size(); k++)
				{
					m_GrainIds[currentvlist[k]] = 0;
					m_PhasesC[currentvlist[k]] = 0;
				}
				m_PhasesF[0] = 0;
			}
			if((int)currentvlist.size() < m_MinAllowedGrainSize*100)
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
	  std::stringstream ss;
//	  ss << "Cleaning Up Grains - Removing Bad Points - Cycle " << count << " - " << ((float)i/totalPoints)*100 << "Percent Complete";
//	  notify(ss.str(), 0, Observable::UpdateProgressMessage);
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

  size_t totalPoints = static_cast<size_t>(m->totalPoints());
  for (size_t i = 0; i < totalPoints; i++)
  {
	std::stringstream ss;
//	ss << "Cleaning Up Grains - Removing Contained Fields" << ((float)i/totalPoints)*100 << "Percent Complete";
//	notify(ss.str(), 0, Observable::UpdateProgressMessage);
    int grainname = m_GrainIds[i];
    if(m_NumNeighbors[grainname] < m_MinNumNeighbors && m_PhasesF[grainname] > 0)
    {
      m_Active[grainname] = false;
      m_GrainIds[i] = 0;
    }
  }
  assign_badpoints();
}

void CleanupGrains::remove_smallgrains()
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

  size_t size = 0;
  int good = 0;
  int neighbor = 0;
  DimType col, row, plane;
  int gnum;
 // DimType currentgrain = 1;

  int neighpoints[6];
  neighpoints[0] = static_cast<int>(-dims[0] * dims[1]);
  neighpoints[1] = static_cast<int>(-dims[0]);
  neighpoints[2] = static_cast<int>(-1);
  neighpoints[3] = static_cast<int>(1);
  neighpoints[4] = static_cast<int>(dims[0]);
  neighpoints[5] = static_cast<int>(dims[0] * dims[1]);


  DimType numgrains = m->getTotalFields();

  nuclei.resize(numgrains, -1);

 // size_t maxGrain = 0;
  // Reset all the Grain nucleus values to -1;
  for (DimType i = 1; i < numgrains; i++)
  {
    nuclei[i] = -1;
  }
  for (int64_t i = 0; i < totalPoints; i++)
  {
    m_AlreadyChecked[i] = false;
    gnum = m_GrainIds[i];
    if(gnum >= 0) nuclei[gnum] = static_cast<int>(i);
  }
  voxellists.resize(numgrains);
  for (size_t i = 1; i <  static_cast<size_t>(numgrains); i++)
  {
	  std::stringstream ss;
//	  ss << "Cleaning Up Grains - Removing Small Fields" << ((float)i/totalPoints)*100 << "Percent Complete";
//	  notify(ss.str(), 0, Observable::UpdateProgressMessage);
      size = 0;
      int nucleus = nuclei[i];
      voxellists[i].push_back(nucleus);
      m_AlreadyChecked[nucleus] = true;
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
              size++;
            }
          }
        }
      }
      if(voxellists[i].size() >= static_cast<size_t>(m_MinAllowedGrainSize) )
      {
		m_Active[i] = true;
      }
      if(voxellists[i].size() < static_cast<size_t>(m_MinAllowedGrainSize) )
      {
		m_Active[i] = false;
        for (size_t b = 0; b < voxellists[i].size(); b++)
        {
          int index = voxellists[i][b];
          m_GrainIds[index] = 0;
        }
      }
  }
}


