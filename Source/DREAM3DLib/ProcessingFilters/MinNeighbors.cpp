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

#include "MinNeighbors.h"


#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Math/DREAM3DMath.h"
#include "DREAM3DLib/Utilities/DREAM3DRandom.h"

#include "DREAM3DLib/StatisticsFilters/FindNeighbors.h"
#include "DREAM3DLib/GenericFilters/FindGrainPhases.h"
#include "DREAM3DLib/GenericFilters/RenumberGrains.h"

const static float m_pi = static_cast<float>(M_PI);

#define NEW_SHARED_ARRAY(var, m_msgType, size)\
  boost::shared_array<m_msgType> var##Array(new m_msgType[size]);\
  m_msgType* var = var##Array.get();

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MinNeighbors::MinNeighbors() :
AbstractFilter(),
m_GrainIdsArrayName(DREAM3D::CellData::GrainIds),
m_NumNeighborsArrayName(DREAM3D::FieldData::NumNeighbors),
m_ActiveArrayName(DREAM3D::FieldData::Active),
m_MinNumNeighbors(1),
m_AlreadyChecked(NULL),
m_Neighbors(NULL),
m_GrainIds(NULL),
m_NumNeighbors(NULL),
m_Active(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MinNeighbors::~MinNeighbors()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MinNeighbors::setupFilterParameters()
{
  std::vector<FilterParameter::Pointer> parameters;
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Minimum Number Neighbors");
    option->setPropertyName("MinNumNeighbors");
    option->setWidgetType(FilterParameter::IntWidget);
    option->setValueType("int");
    parameters.push_back(option);
  }

  setFilterParameters(parameters);
}
// -----------------------------------------------------------------------------
void MinNeighbors::readFilterParameters(AbstractFilterParametersReader* reader)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MinNeighbors::writeFilterParameters(AbstractFilterParametersWriter* writer)

{
  writer->writeValue("MinNumNeighbors", getMinNumNeighbors() );
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MinNeighbors::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;
  VoxelDataContainer* m = getVoxelDataContainer();
  GET_PREREQ_DATA(m, DREAM3D, CellData, GrainIds, ss, -301, int32_t, Int32ArrayType, voxels, 1)

  CREATE_NON_PREREQ_DATA(m, DREAM3D, FieldData, Active, ss, bool, BoolArrayType, true, fields, 1)

  GET_PREREQ_DATA(m, DREAM3D, FieldData, NumNeighbors, ss, -304, int32_t, Int32ArrayType, fields, 1)


}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MinNeighbors::preflight()
{
  dataCheck(true, 1, 1, 1);

  VoxelDataContainer* m = getVoxelDataContainer();
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The DataContainer Object was NULL", -999);
    return;
  }

  RenumberGrains::Pointer renumber_grains = RenumberGrains::New();
  renumber_grains->setObservers(this->getObservers());
  renumber_grains->setVoxelDataContainer(m);
  renumber_grains->setMessagePrefix(getMessagePrefix());
  renumber_grains->preflight();
  int err = renumber_grains->getErrorCondition();
  if (err < 0)
  {
    setErrorCondition(renumber_grains->getErrorCondition());
    addErrorMessages(renumber_grains->getPipelineMessages());
    return;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MinNeighbors::execute()
{
  setErrorCondition(0);
 // int err = 0;
  VoxelDataContainer* m = getVoxelDataContainer();
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The DataContainer Object was NULL", -999);
    return;
  }


  int64_t totalPoints = m->getTotalPoints();
  dataCheck(false, totalPoints, m->getNumFieldTuples(), m->getNumEnsembleTuples());
  if (getErrorCondition() < 0)
  {
    return;
  }
  setErrorCondition(0);

  Int32ArrayType::Pointer neighborsPtr = Int32ArrayType::CreateArray(totalPoints, "Neighbors");
  m_Neighbors = neighborsPtr->GetPointer(0);
  neighborsPtr->initializeWithZeros();

  BoolArrayType::Pointer alreadCheckedPtr = BoolArrayType::CreateArray(totalPoints, "AlreadyChecked");
  m_AlreadyChecked = alreadCheckedPtr->GetPointer(0);
  alreadCheckedPtr->initializeWithZeros();

  merge_containedgrains();
  assign_badpoints();

  RenumberGrains::Pointer renumber_grains = RenumberGrains::New();
  renumber_grains->setObservers(this->getObservers());
  renumber_grains->setVoxelDataContainer(m);
  renumber_grains->setMessagePrefix(getMessagePrefix());
  renumber_grains->execute();
  int err = renumber_grains->getErrorCondition();
  if (err < 0)
  {
    setErrorCondition(renumber_grains->getErrorCondition());
    addErrorMessages(renumber_grains->getPipelineMessages());
    return;
  }

  // If there is an error set this to something negative and also set a message
 notifyStatusMessage("Minimum Number of Neighbors Filter Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MinNeighbors::assign_badpoints()
{
  VoxelDataContainer* m = getVoxelDataContainer();
  int64_t totalPoints = m->getTotalPoints();
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

  Int32ArrayType::Pointer neighborsPtr = Int32ArrayType::CreateArray(totalPoints, "Neighbors");
  m_Neighbors = neighborsPtr->GetPointer(0);
  neighborsPtr->initializeWithValues(-1);

  std::vector<int > remove;
  int good = 1;
//  int neighbor;
//  int index = 0;
//  float x, y, z;
  int current = 0;
  int most = 0;
//  int curgrain = 0;
 // DimType row, plane;
  int neighpoint;
  size_t numgrains = m->getNumFieldTuples();

  int neighpoints[6];
  neighpoints[0] = static_cast<int>(-dims[0] * dims[1]);
  neighpoints[1] = static_cast<int>(-dims[0]);
  neighpoints[2] = static_cast<int>(-1);
  neighpoints[3] = static_cast<int>(1);
  neighpoints[4] = static_cast<int>(dims[0]);
  neighpoints[5] = static_cast<int>(dims[0] * dims[1]);
  std::vector<int> currentvlist;

  size_t counter = 1;
  size_t count = 0;
  int kstride, jstride;
  int grainname, grain;
  int neighbor;
  std::vector<int > n(numgrains + 1,0);
  while (counter != 0)
  {
    counter = 0;
    for (int k = 0; k < dims[2]; k++)
    {
		kstride = static_cast<int>( dims[0]*dims[1]*k );
	    for (int j = 0; j < dims[1]; j++)
	    {
			jstride = static_cast<int>( dims[0]*j );
		    for (int i = 0; i < dims[0]; i++)
		    {
			  count = kstride+jstride+i;
			  std::stringstream ss;
			  grainname = m_GrainIds[count];
			  if (grainname < 0)
			  {
			    counter++;
				current = 0;
				most = 0;
				for (int l = 0; l < 6; l++)
				{
				  good = 1;
				  neighpoint = static_cast<int>( count + neighpoints[l] );
				  if (l == 0 && k == 0) good = 0;
				  if (l == 5 && k == (dims[2] - 1)) good = 0;
				  if (l == 1 && j == 0) good = 0;
				  if (l == 4 && j == (dims[1] - 1)) good = 0;
				  if (l == 2 && i == 0) good = 0;
				  if (l == 3 && i == (dims[0] - 1)) good = 0;
				  if (good == 1)
				  {
					grain = m_GrainIds[neighpoint];
					if (grain >= 0)
					{
					  n[grain]++;
					  current = n[grain];
					  if (current > most)
					  {
						most = current;
					    m_Neighbors[count] = neighpoint;
					  }
					}
				  }
				}
				for (int l = 0; l < 6; l++)
				{
				  good = 1;
				  neighpoint = static_cast<int>( count + neighpoints[l] );
				  if (l == 0 && k == 0) good = 0;
				  if (l == 5 && k == (dims[2] - 1)) good = 0;
				  if (l == 1 && j == 0) good = 0;
				  if (l == 4 && j == (dims[1] - 1)) good = 0;
				  if (l == 2 && i == 0) good = 0;
				  if (l == 3 && i == (dims[0] - 1)) good = 0;
				  if (good == 1)
				  {
					grain = m_GrainIds[neighpoint];
					if(grain >= 0) n[grain] = 0;
				  }
				}
			}
		  }
		}
	}
    std::list<std::string> voxelArrayNames = m->getCellArrayNameList();
    for (int j = 0; j < totalPoints; j++)
    {
      grainname = m_GrainIds[j];
      neighbor = m_Neighbors[j];
      if (grainname < 0 && neighbor >= 0 && m_GrainIds[neighbor] >= 0)
      {
          for(std::list<std::string>::iterator iter = voxelArrayNames.begin(); iter != voxelArrayNames.end(); ++iter)
          {
            std::string name = *iter;
            IDataArray::Pointer p = m->getCellData(*iter);
            p->CopyTuple(neighbor, j);
          }
      }
    }
  }
}


void MinNeighbors::merge_containedgrains()
{
  // Since this method is called from the 'execute' and the DataContainer validity
  // was checked there we are just going to get the Shared Pointer to the DataContainer
  VoxelDataContainer* m = getVoxelDataContainer();

  bool good = false;

  size_t totalPoints = static_cast<size_t>(m->getTotalPoints());
  size_t totalFields = static_cast<size_t>(m->getNumFieldTuples());
  for (size_t i = 0; i < totalFields; i++)
  {
	  m_Active[i] = true;
	  if(m_NumNeighbors[i] >= m_MinNumNeighbors) good = true;
  }
  if(good == false)
  {
    setErrorCondition(-1);
    notifyErrorMessage("The minimum number of neighbors is larger than the Field with the most neighbors.  All Fields would be removed.  The filter has quit.", -1);
    return;
  }
  for (size_t i = 0; i < totalPoints; i++)
  {
	std::stringstream ss;
    int grainname = m_GrainIds[i];
    if(m_NumNeighbors[grainname] < m_MinNumNeighbors && grainname > 0)
    {
      m_Active[grainname] = false;
      m_GrainIds[i] = -1;
    }
  }
}
