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

#include "MergeColonies.h"

#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Common/DREAM3DMath.h"
#include "DREAM3DLib/Common/OrientationMath.h"
#include "DREAM3DLib/Common/DREAM3DRandom.h"

#include "DREAM3DLib/GenericFilters/FindNeighbors.h"
#include "DREAM3DLib/GenericFilters/RenumberGrains.h"

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
MergeColonies::MergeColonies() :
AbstractFilter(),
m_GrainIds(NULL),
m_AvgQuats(NULL),
m_EulerAngles(NULL),
m_NumCells(NULL),
m_NumNeighbors(NULL),
m_PhasesF(NULL),
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
MergeColonies::~MergeColonies()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MergeColonies::setupFilterOptions()
{
  std::vector<FilterOption::Pointer> options;
  {
    FilterOption::Pointer option = FilterOption::New();
    option->setHumanLabel("Axis Tolerance");
    option->setPropertyName("AxisTolerance");
    option->setWidgetType(FilterOption::DoubleWidget);
    option->setValueType("float");
    options.push_back(option);
  }
  {
    FilterOption::Pointer option = FilterOption::New();
    option->setHumanLabel("Angle Tolerance");
    option->setPropertyName("AngleTolerance");
    option->setWidgetType(FilterOption::DoubleWidget);
    option->setValueType("float");
    options.push_back(option);
  }

  setFilterOptions(options);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MergeColonies::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;
  DataContainer* m = getDataContainer();

  // Cell Data
  GET_PREREQ_DATA( m, DREAM3D, VoxelData, GrainIds, ss, -300, int32_t, Int32ArrayType, voxels, 1);

  // Field Data
  GET_PREREQ_DATA(m, DREAM3D, FieldData, AvgQuats, ss, -301, float, FloatArrayType, fields, 3);
  GET_PREREQ_DATA(m, DREAM3D, FieldData, EulerAngles, ss, -301, float, FloatArrayType, fields, 3);
  GET_PREREQ_DATA(m, DREAM3D, FieldData, NumCells, ss, -302, int32_t, Int32ArrayType, fields, 1);
  GET_PREREQ_DATA(m, DREAM3D, FieldData, NumNeighbors, ss, -306, int32_t, Int32ArrayType, fields, 1);
  GET_PREREQ_DATA_SUFFIX(m, DREAM3D, FieldData, Phases, F, ss, -303,  int32_t, Int32ArrayType, fields, 1);

  // Now we are going to get a "Pointer" to the NeighborList object out of the DataContainer
  m_NeighborList = NeighborList<int>::SafeObjectDownCast<IDataArray*, NeighborList<int>* >
                                          (m->getFieldData(DREAM3D::FieldData::NeighborList).get());
  if(m_NeighborList == NULL)
  {
    ss << "NeighborLists Array Not Initialized At Beginning of " << getNameOfClass() << " Filter" << std::endl;
    setErrorCondition(-308);
  }


  setErrorMessage(ss.str());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MergeColonies::preflight()
{
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MergeColonies::execute()
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
  setErrorCondition(0);
  dataCheck(false, m->totalPoints(), m->getTotalFields(), m->crystruct.size());

  notify("Merge Colonies - Merging Colonies", 0, Observable::UpdateProgressMessage);
  merge_colonies();

  notify("Merge Colonies - Characterizing Colonies", 0, Observable::UpdateProgressMessage);
  characterize_colonies();

  notify("Merge Colonies - Renumbering Grains", 0, Observable::UpdateProgressMessage);
  RenumberGrains::Pointer renumber_grains = RenumberGrains::New();
  renumber_grains->setObservers(this->getObservers());
  renumber_grains->setDataContainer(m);
  renumber_grains->execute();
  int err = renumber_grains->getErrorCondition();
  if (err < 0)
  {
    return;
  }

  // If there is an error set this to something negative and also set a message
  notify("MergeColonies Completed", 0, Observable::UpdateProgressMessage);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MergeColonies::merge_colonies()
{
  // Since this method is called from the 'execute' and the DataContainer validity
  // was checked there we are just going to get the Shared Pointer to the DataContainer
  DataContainer* m = getDataContainer();

  NeighborList<int>& neighborlist = *m_NeighborList;


  float angcur = 180.0f;
  std::vector<int> colonylist;
  float w;
  float n1, n2, n3;
  float r1, r2, r3;
  float q1[5];
  float q2[5];
  size_t numgrains = m->getTotalFields();
  Ebsd::CrystalStructure phase1, phase2;
  colonynewnumbers.resize(numgrains, -1);

  for (size_t i = 1; i < numgrains; i++)
  {
    if (colonynewnumbers[i] == -1 && m_PhasesF[i] > 0)
    {
      colonylist.push_back(i);
      int csize = int(colonylist.size());
      for (int j = 0; j < csize; j++)
      {
        csize = int(colonylist.size());
        int firstgrain = colonylist[j];
        int size = int(neighborlist[firstgrain].size());
        for (int l = 0; l < size; l++)
        {
          angcur = 180.0f;
          int colony = 0;
          size_t neigh = neighborlist[firstgrain][l];
          if (neigh != i && colonynewnumbers[neigh] != -1 && m_PhasesF[neigh] > 0)
          {
		    w = 10000.0f;
            q1[1] = m_AvgQuats[5*firstgrain+1]/m_AvgQuats[5*firstgrain];
            q1[2] = m_AvgQuats[5*firstgrain+2]/m_AvgQuats[5*firstgrain];
            q1[3] = m_AvgQuats[5*firstgrain+3]/m_AvgQuats[5*firstgrain];
            q1[4] = m_AvgQuats[5*firstgrain+4]/m_AvgQuats[5*firstgrain];
            phase1 = m->crystruct[m_PhasesF[firstgrain]];
            q2[1] = m_AvgQuats[5*neigh+1]/m_AvgQuats[5*neigh];
            q2[2] = m_AvgQuats[5*neigh+2]/m_AvgQuats[5*neigh];
            q2[3] = m_AvgQuats[5*neigh+3]/m_AvgQuats[5*neigh];
            q2[4] = m_AvgQuats[5*neigh+4]/m_AvgQuats[5*neigh];
            phase2 = m->crystruct[m_PhasesF[neigh]];
			if (phase1 == phase2 && phase1 > 0) w = m_OrientationOps[phase1]->getMisoQuat( q1, q2, n1, n2, n3);
			OrientationMath::axisAngletoRod(w, n1, n2, n3, r1, r2, r3);
			float vecttol = 0.03f;
            if (fabs(r1) < vecttol && fabs(r2) < vecttol && fabs(fabs(r3) - 0.0919f) < vecttol) colony = 1;
            if (fabs(fabs(r1) - 0.289f) < vecttol && fabs(fabs(r2) - 0.5f) < vecttol && fabs(r3) < vecttol) colony = 1;
            if (fabs(fabs(r1) - 0.57735f) < vecttol && fabs(r2) < vecttol && fabs(r3) < vecttol) colony = 1;
            if (fabs(fabs(r1) - 0.33f) < vecttol && fabs(fabs(r2) - 0.473f) < vecttol && fabs(fabs(r3) - 0.093f) < vecttol) colony = 1;
            if (fabs(fabs(r1) - 0.577f) < vecttol && fabs(fabs(r2) - 0.053f) < vecttol && fabs(fabs(r3) - 0.093f) < vecttol) colony = 1;
            if (fabs(fabs(r1) - 0.293f) < vecttol && fabs(fabs(r2) - 0.508f) < vecttol && fabs(fabs(r3) - 0.188f) < vecttol) colony = 1;
            if (fabs(fabs(r1) - 0.5866f) < vecttol && fabs(r2) < vecttol && fabs(fabs(r3) - 0.188) < vecttol) colony = 1;
            if (fabs(fabs(r1) - 0.5769f) < vecttol && fabs(fabs(r2) - 0.8168f) < vecttol && fabs(r3) < vecttol) colony = 1;
            if (fabs(fabs(r1) - 0.9958f) < vecttol && fabs(fabs(r2) - 0.0912f) < vecttol && fabs(r3) < vecttol) colony = 1;
            if (w < angcur)
            {
              angcur = w;
            }
            if (colony == 1)
            {
              colonynewnumbers[neigh] = i;
              colonylist.push_back(neigh);
            }
          }
        }
      }
    }
    colonylist.clear();
  }
  size_t totalPoints = static_cast<size_t>(m->totalPoints());
  for (size_t k = 0; k < totalPoints; k++)
  {
    int grainname = m_GrainIds[k];
	if (colonynewnumbers[grainname] != -1) { m_GrainIds[k] = colonynewnumbers[grainname];}
  }
}

void MergeColonies::characterize_colonies()
{
  DataContainer* m = getDataContainer();
  size_t numgrains = m->getTotalFields();
  for (size_t i = 0; i < numgrains; i++)
  {

  }
}
