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

#include "GroupMicroTextureRegions.h"

#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Common/DREAM3DMath.h"
#include "DREAM3DLib/Common/OrientationMath.h"
#include "DREAM3DLib/Common/DREAM3DRandom.h"

#include "DREAM3DLib/PrivateFilters/FindNeighbors.h"
#include "DREAM3DLib/PrivateFilters/FindGrainPhases.h"
#include "DREAM3DLib/PrivateFilters/RenumberGrains.h"

#include "DREAM3DLib/OrientationOps/CubicOps.h"
#include "DREAM3DLib/OrientationOps/HexagonalOps.h"
#include "DREAM3DLib/OrientationOps/OrthoRhombicOps.h"


#define ERROR_TXT_OUT 1
#define ERROR_TXT_OUT1 1

const static float m_pi = M_PI;


#define NEW_SHARED_ARRAY(var, type, size)\
  boost::shared_array<type> var##Array(new type[size]);\
  type* var = var##Array.get();

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GroupMicroTextureRegions::GroupMicroTextureRegions() :
AbstractFilter(),
m_CAxisTolerance(1.0f),
m_GrainIds(NULL),
m_AvgQuats(NULL),
m_Active(NULL),
m_PhasesF(NULL),
m_NeighborList(NULL),
m_CrystalStructures(NULL),
m_NumFields(NULL)
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
GroupMicroTextureRegions::~GroupMicroTextureRegions()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GroupMicroTextureRegions::setupFilterOptions()
{
  std::vector<FilterOption::Pointer> options;
  {
    FilterOption::Pointer option = FilterOption::New();
    option->setHumanLabel("C-Axis Alignment Tolerance");
    option->setPropertyName("CAxisTolerance");
    option->setWidgetType(FilterOption::DoubleWidget);
    option->setValueType("float");
    option->setCastableValueType("double");
    options.push_back(option);
  }

  setFilterOptions(options);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GroupMicroTextureRegions::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;
  DataContainer* m = getDataContainer();

  // Cell Data
  GET_PREREQ_DATA( m, DREAM3D, CellData, GrainIds, ss, -301, int32_t, Int32ArrayType, voxels, 1);

  // Field Data
  GET_PREREQ_DATA(m, DREAM3D, FieldData, AvgQuats, ss, -302, float, FloatArrayType, fields, 5);
  GET_PREREQ_DATA_SUFFIX(m, DREAM3D, FieldData, Phases, F, ss, -303,  int32_t, Int32ArrayType, fields, 1);
  if(getErrorCondition() == -303)
  {
	setErrorCondition(0);
	FindGrainPhases::Pointer find_grainphases = FindGrainPhases::New();
	find_grainphases->setObservers(this->getObservers());
	find_grainphases->setDataContainer(getDataContainer());
	if(preflight == true) find_grainphases->preflight();
	if(preflight == false) find_grainphases->execute();
	GET_PREREQ_DATA_SUFFIX(m, DREAM3D, FieldData, Phases, F, ss, -303, int32_t, Int32ArrayType, fields, 1);
  }
  CREATE_NON_PREREQ_DATA(m, DREAM3D, FieldData, Active, ss, bool, BoolArrayType, true, fields, 1);
  // Now we are going to get a "Pointer" to the NeighborList object out of the DataContainer
  m_NeighborList = NeighborList<int>::SafeObjectDownCast<IDataArray*, NeighborList<int>*>(m->getFieldData(DREAM3D::FieldData::NeighborList).get());
  if(m_NeighborList == NULL)
  {
	setErrorCondition(0);
	FindNeighbors::Pointer find_neighbors = FindNeighbors::New();
	find_neighbors->setObservers(this->getObservers());
	find_neighbors->setDataContainer(getDataContainer());
	if(preflight == true) find_neighbors->preflight();
	if(preflight == false) find_neighbors->execute();
	m_NeighborList = NeighborList<int>::SafeObjectDownCast<IDataArray*, NeighborList<int>*>(m->getFieldData(DREAM3D::FieldData::NeighborList).get());
	if(m_NeighborList == NULL)
	{
		ss << "NeighborLists Array Not Initialized At Beginning of '" << getNameOfClass() << "' Filter" << std::endl;
		setErrorCondition(-304);
	}
  }

  typedef DataArray<unsigned int> XTalStructArrayType;
  GET_PREREQ_DATA(m, DREAM3D, EnsembleData, CrystalStructures, ss, -305, unsigned int, XTalStructArrayType, ensembles, 1);
  CREATE_NON_PREREQ_DATA(m, DREAM3D, EnsembleData, NumFields, ss, int32_t, Int32ArrayType, 0, ensembles, 1);

  setErrorMessage(ss.str());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GroupMicroTextureRegions::preflight()
{
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GroupMicroTextureRegions::execute()
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
//  m->clearFieldData();
//  m->clearEnsembleData();

  setErrorCondition(0);
  dataCheck(false, m->getTotalPoints(), m->getNumFieldTuples(), m->getNumEnsembleTuples());
  if (getErrorCondition() < 0)
  {
    return;
  }

  notify("Grouping MicroTexture Regions", 0, Observable::UpdateProgressMessage);
  merge_micro_texture_regions();

  notify("Characterizing MicroTexture Regions", 0, Observable::UpdateProgressMessage);
  characterize_micro_texture_regions();

  notify("Renumbering Fields", 0, Observable::UpdateProgressMessage);
  RenumberGrains::Pointer renumber_grains = RenumberGrains::New();
  renumber_grains->setObservers(this->getObservers());
  renumber_grains->setDataContainer(m);
  renumber_grains->execute();
  int err = renumber_grains->getErrorCondition();
  if (err < 0)
  {
    return;
  }

  for(size_t i = 1; i < m->getNumEnsembleTuples(); i++)
  {
	m_NumFields[i] = 0;
  }
  for(size_t i = 1; i < m->getNumEnsembleTuples(); i++)
  {
	m_NumFields[m_PhasesF[i]]++;
  }

  // If there is an error set this to something negative and also set a message
  notify("GroupMicroTextureRegions Completed", 0, Observable::UpdateProgressMessage);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GroupMicroTextureRegions::merge_micro_texture_regions()
{
  // Since this method is called from the 'execute' and the DataContainer validity
  // was checked there we are just going to get the Shared Pointer to the DataContainer
  DataContainer* m = getDataContainer();

  NeighborList<int>& neighborlist = *m_NeighborList;

  float angcur = 180.0f;
  std::vector<int> microtexturelist;
  float w;
  float cx1, cy1, cz1, denom1;
  float cx2, cy2, cz2, denom2;
  float q1[5];
  float q2[5];
  size_t numgrains = m->getNumFieldTuples();
  unsigned int phase1, phase2;
  newnumbers.resize(numgrains, -1);

  for (size_t i = 1; i < numgrains; i++)
  {
    if (newnumbers[i] == -1 && m_PhasesF[i] > 0)
    {
	  m_Active[i] = true;
      microtexturelist.push_back(i);
      int csize = int(microtexturelist.size());
      for (int j = 0; j < csize; j++)
      {
        csize = int(microtexturelist.size());
        int firstgrain = microtexturelist[j];
        int size = int(neighborlist[firstgrain].size());
        q1[1] = m_AvgQuats[5*firstgrain+1]/m_AvgQuats[5*firstgrain];
        q1[2] = m_AvgQuats[5*firstgrain+2]/m_AvgQuats[5*firstgrain];
        q1[3] = m_AvgQuats[5*firstgrain+3]/m_AvgQuats[5*firstgrain];
        q1[4] = m_AvgQuats[5*firstgrain+4]/m_AvgQuats[5*firstgrain];
        phase1 = m_CrystalStructures[m_PhasesF[firstgrain]];
	    cx1 = (2 * q1[1] * q1[3] + 2 * q1[2] * q1[4]) * 1;
	    cy1 = (2 * q1[2] * q1[3] - 2 * q1[1] * q1[4]) * 1;
	    cz1 = (1 - 2 * q1[1] * q1[1] - 2 * q1[2] * q1[2]) * 1;
		denom1 = sqrt((cx1*cx1)+(cy1*cy1)+(cz1*cz1));
        for (int l = 0; l < size; l++)
        {
          angcur = 180.0f;
          size_t neigh = neighborlist[firstgrain][l];
          if (neigh != i && newnumbers[neigh] == -1 && m_PhasesF[neigh] > 0)
          {
            phase2 = m_CrystalStructures[m_PhasesF[neigh]];
			if (phase1 == phase2 && phase1 == Ebsd::CrystalStructure::Hexagonal) 
			{
              q2[1] = m_AvgQuats[5*neigh+1]/m_AvgQuats[5*neigh];
              q2[2] = m_AvgQuats[5*neigh+2]/m_AvgQuats[5*neigh];
              q2[3] = m_AvgQuats[5*neigh+3]/m_AvgQuats[5*neigh];
              q2[4] = m_AvgQuats[5*neigh+4]/m_AvgQuats[5*neigh];
			  cx2 = (2 * q2[1] * q2[3] + 2 * q2[2] * q2[4]) * 1;
			  cy2 = (2 * q2[2] * q2[3] - 2 * q2[1] * q2[4]) * 1;
			  cz2 = (1 - 2 * q2[1] * q2[1] - 2 * q2[2] * q2[2]) * 1;
			  denom2 = sqrt((cx2*cx2)+(cy2*cy2)+(cz2*cz2));
			  w = ((cx1*cx2)+(cy1*cy2)+(cz1*cz2))/(denom1*denom2);
			  w = 180.0*acosf(w)/m_pi;
              if (w <= m_CAxisTolerance)
              {
                newnumbers[neigh] = i;
                microtexturelist.push_back(neigh);
			    m_Active[neigh] = false;
			  }
            }
          }
        }
      }
    }
    microtexturelist.clear();
  }
  size_t totalPoints = static_cast<size_t>(m->getTotalPoints());
  for (size_t k = 0; k < totalPoints; k++)
  {
    int grainname = m_GrainIds[k];
	if (newnumbers[grainname] != -1) { m_GrainIds[k] = newnumbers[grainname];}
  }
}

void GroupMicroTextureRegions::characterize_micro_texture_regions()
{
  DataContainer* m = getDataContainer();
  size_t numgrains = m->getNumFieldTuples();
  for (size_t i = 0; i < numgrains; i++)
  {

  }
}
