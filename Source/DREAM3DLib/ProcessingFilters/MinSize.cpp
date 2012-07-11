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

#include "MinSize.h"


#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Common/DREAM3DMath.h"
#include "DREAM3DLib/Common/DREAM3DRandom.h"

#include "DREAM3DLib/GenericFilters/FindGrainPhases.h"
#include "DREAM3DLib/GenericFilters/RenumberGrains.h"

const static float m_pi = static_cast<float>(M_PI);

#define NEW_SHARED_ARRAY(var, m_msgType, size)\
  boost::shared_array<m_msgType> var##Array(new m_msgType[size]);\
  m_msgType* var = var##Array.get();

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MinSize::MinSize() :
AbstractFilter(),
m_GrainIdsArrayName(DREAM3D::CellData::GrainIds),
m_CellPhasesArrayName(DREAM3D::CellData::Phases),
m_FieldPhasesArrayName(DREAM3D::FieldData::Phases),
m_ActiveArrayName(DREAM3D::FieldData::Active),
m_MinAllowedGrainSize(1),
m_GrainIds(NULL),
m_CellPhases(NULL),
m_FieldPhases(NULL),
m_Active(NULL)
{
  setupFilterOptions();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MinSize::~MinSize()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MinSize::setupFilterOptions()
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

  setFilterOptions(options);
}
// -----------------------------------------------------------------------------
void MinSize::writeFilterOptions(AbstractFilterOptionsWriter* writer)
{
  writer->writeValue("MinAllowedGrainSize", getMinAllowedGrainSize() );
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MinSize::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;
  DataContainer* m = getDataContainer();
  int err = 0;

  GET_PREREQ_DATA(m, DREAM3D, CellData, GrainIds, ss, -301, int32_t, Int32ArrayType, voxels, 1)
  GET_PREREQ_DATA(m, DREAM3D, CellData, CellPhases, ss, -302, int32_t, Int32ArrayType, voxels, 1)

  TEST_PREREQ_DATA(m, DREAM3D, FieldData, FieldPhases, err, -302, int32_t, Int32ArrayType, fields, 1)
  if(err == -302)
  {
    setErrorCondition(0);
    FindGrainPhases::Pointer find_grainphases = FindGrainPhases::New();
    find_grainphases->setObservers(this->getObservers());
    find_grainphases->setDataContainer(getDataContainer());
    if(preflight == true) find_grainphases->preflight();
    if(preflight == false) find_grainphases->execute();
  }
  GET_PREREQ_DATA(m, DREAM3D, FieldData, FieldPhases, ss, -302, int32_t, Int32ArrayType, fields, 1)



  CREATE_NON_PREREQ_DATA(m, DREAM3D, FieldData, Active, ss, bool, BoolArrayType, true, fields, 1)
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MinSize::preflight()
{
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MinSize::execute()
{
  setErrorCondition(0);
 // int err = 0;
  DataContainer* m = getDataContainer();
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The DataContainer Object was NULL", -999);
    return;
  }


  int64_t totalPoints = m->getTotalPoints();
  dataCheck(false, totalPoints, m->getNumFieldTuples(), m->getNumEnsembleTuples());
  if (getErrorCondition() < 0 && getErrorCondition() != -305)
  {
    return;
  }
  setErrorCondition(0);

  remove_smallgrains();
  assign_badpoints();

  RenumberGrains::Pointer renumber_grains = RenumberGrains::New();
  renumber_grains->setObservers(this->getObservers());
  renumber_grains->setDataContainer(m);
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
 notifyStatusMessage("Minimum Size Filter Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MinSize::assign_badpoints()
{
  DataContainer* m = getDataContainer();
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
		//	  ss << "Cleaning Up Grains - Removing Bad Points - Cycle " << count << " - " << ((float)i/totalPoints)*100 << "Percent Complete";
		//	  notify(ss.str(), 0, Observable::UpdateProgressMessage);
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
					    m_Neighbors[count] = grain;
					  }
					}
				  }
				}
				for (int l = 0; l < 6; l++)
				{
//				  good = 1;
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

    for (int j = 0; j < totalPoints; j++)
    {
      int grainname = m_GrainIds[j];
      int neighbor = m_Neighbors[j];
      if (grainname < 0 && neighbor >= 0)
      {
        m_GrainIds[j] = neighbor;
		m_CellPhases[j] = m_FieldPhases[neighbor];
      }
    }
//    std::stringstream ss;
//     ss << "Assigning Bad Voxels count = " << count;
//    notify(ss.str().c_str(), 0, Observable::UpdateProgressMessage);
  }
}

void MinSize::remove_smallgrains()
{
  DataContainer* m = getDataContainer();
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

  BoolArrayType::Pointer alreadCheckedPtr = BoolArrayType::CreateArray(totalPoints, "AlreadyChecked");
  m_AlreadyChecked = alreadCheckedPtr->GetPointer(0);
  alreadCheckedPtr->initializeWithZeros();

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

  DimType numgrains = m->getNumFieldTuples();

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
//	  notifyStatusMessage(ss.str());
      size = 0;
      int nucleus = nuclei[i];
	  if(nucleus >= 0)
	  {
		  voxellists[i].push_back(nucleus);
		  assert(nucleus <= totalPoints);
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
				  assert(neighbor <= totalPoints);
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
			  m_GrainIds[index] = -1;
			}
		  }
	  }
	  else if(nucleus < 0)
	  {
		m_Active[i] = false;
	  }
  }
}
