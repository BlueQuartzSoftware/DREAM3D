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

#include "MergeTwins.h"

#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Common/DREAM3DMath.h"
#include "DREAM3DLib/Common/OrientationMath.h"
#include "DREAM3DLib/Common/DREAM3DRandom.h"

#include "DREAM3DLib/GenericFilters/FindNeighbors.h"
#include "DREAM3DLib/GenericFilters/FindGrainPhases.h"
#include "DREAM3DLib/GenericFilters/RenumberGrains.h"
#include "DREAM3DLib/StatisticsFilters/FindAvgOrientations.h"


#include "DREAM3DLib/OrientationOps/CubicOps.h"
#include "DREAM3DLib/OrientationOps/HexagonalOps.h"
#include "DREAM3DLib/OrientationOps/OrthoRhombicOps.h"


#define ERROR_TXT_OUT 1
#define ERROR_TXT_OUT1 1

const static float m_pi = static_cast<float>(M_PI);


#define NEW_SHARED_ARRAY(var, m_msgType, size)\
  boost::shared_array<m_msgType> var##Array(new m_msgType[size]);\
  m_msgType* var = var##Array.get();

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MergeTwins::MergeTwins() :
AbstractFilter(),
m_GrainIdsArrayName(DREAM3D::CellData::GrainIds),
m_ParentIdsArrayName(DREAM3D::CellData::ParentIds),
m_AvgQuatsArrayName(DREAM3D::FieldData::AvgQuats),
m_FieldPhasesArrayName(DREAM3D::FieldData::Phases),
m_ActiveArrayName(DREAM3D::FieldData::Active),
m_CrystalStructuresArrayName(DREAM3D::EnsembleData::CrystalStructures),
m_NumFieldsArrayName(DREAM3D::EnsembleData::NumFields),
m_AxisTolerance(1.0f),
m_AngleTolerance(1.0f),
m_GrainIds(NULL),
m_ParentIds(NULL),
m_AvgQuats(NULL),
m_Active(NULL),
m_FieldPhases(NULL),
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
  setupFilterParameters();


}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MergeTwins::~MergeTwins()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MergeTwins::setupFilterParameters()
{
  std::vector<FilterParameter::Pointer> parameters;
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Axis Tolerance");
    option->setPropertyName("AxisTolerance");
    option->setWidgetType(FilterParameter::DoubleWidget);
    option->setValueType("float");
    option->setCastableValueType("double");
	option->setUnits("Degrees");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Angle Tolerance");
    option->setPropertyName("AngleTolerance");
    option->setWidgetType(FilterParameter::DoubleWidget);
    option->setValueType("float");
    option->setCastableValueType("double");
	option->setUnits("Degrees");
    parameters.push_back(option);
  }

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MergeTwins::writeFilterParameters(AbstractFilterParametersWriter* writer)
{
  writer->writeValue("AxisTolerance", getAxisTolerance() );
  writer->writeValue("AngleTolerance", getAngleTolerance() );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MergeTwins::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;
  DataContainer* m = getDataContainer();
  int err = 0;

  // Cell Data
  GET_PREREQ_DATA( m, DREAM3D, CellData, GrainIds, ss, -301, int32_t, Int32ArrayType, voxels, 1)
  CREATE_NON_PREREQ_DATA(m, DREAM3D, CellData, ParentIds, ss, int32_t, Int32ArrayType, -1, voxels, 1)

  // Field Data
  TEST_PREREQ_DATA(m, DREAM3D, FieldData, AvgQuats, err, -303, float, FloatArrayType, fields, 5)
  if(err == -303)
  {
	setErrorCondition(0);
	FindAvgOrientations::Pointer find_avgorients = FindAvgOrientations::New();
	find_avgorients->setObservers(this->getObservers());
	find_avgorients->setDataContainer(getDataContainer());
	if(preflight == true) find_avgorients->preflight();
	if(preflight == false) find_avgorients->execute();
  }
  GET_PREREQ_DATA(m, DREAM3D, FieldData, AvgQuats, ss, -301, float, FloatArrayType, fields, 5)

  TEST_PREREQ_DATA(m, DREAM3D, FieldData, FieldPhases, err, -303, int32_t, Int32ArrayType, fields, 1)
  if(err == -303)
  {
    setErrorCondition(0);
    FindGrainPhases::Pointer find_grainphases = FindGrainPhases::New();
    find_grainphases->setObservers(this->getObservers());
    find_grainphases->setDataContainer(getDataContainer());
    if(preflight == true) find_grainphases->preflight();
    if(preflight == false) find_grainphases->execute();
  }
  GET_PREREQ_DATA(m, DREAM3D, FieldData, FieldPhases, ss, -303, int32_t, Int32ArrayType, fields, 1)


  CREATE_NON_PREREQ_DATA(m, DREAM3D, FieldData, Active, ss, bool, BoolArrayType, true, fields, 1)
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
      ss << "NeighborLists Array Not Initialized Correctly" << std::endl;
      setErrorCondition(-304);
      addErrorMessage(getHumanLabel(), ss.str(), -304);
    }
  }

  typedef DataArray<unsigned int> XTalStructArrayType;
  GET_PREREQ_DATA(m, DREAM3D, EnsembleData, CrystalStructures, ss, -305, unsigned int, XTalStructArrayType, ensembles, 1)
  CREATE_NON_PREREQ_DATA(m, DREAM3D, EnsembleData, NumFields, ss, int32_t, Int32ArrayType, 0, ensembles, 1)
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MergeTwins::preflight()
{
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MergeTwins::execute()
{
  DataContainer* m = getDataContainer();
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The DataContainer Object was NULL", -999);
    return;
  }

  setErrorCondition(0);

  dataCheck(false, m->getTotalPoints(), m->getNumFieldTuples(), m->getNumEnsembleTuples());
  if (getErrorCondition() < 0)
  {
    return;
  }

 notifyStatusMessage("Merging Twins");
  merge_twins();

 notifyStatusMessage("Characterizing Twins");
  characterize_twins();

 notifyStatusMessage("Renumbering Grains");
  RenumberGrains::Pointer renumber_grains = RenumberGrains::New();
  renumber_grains->setObservers(this->getObservers());
  renumber_grains->setDataContainer(m);
  renumber_grains->execute();
  int err = renumber_grains->getErrorCondition();
  if (err < 0)
  {
    return;
  }

 notifyStatusMessage("Completed");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MergeTwins::merge_twins()
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

  std::vector<int> twinlist;
  float w;
  float n1, n2, n3;
  float angtol = m_AngleTolerance;
  float axistol = static_cast<float>( m_AxisTolerance*M_PI/180.0f );
  float q1[5];
  float q2[5];
  size_t numgrains = m->getNumFieldTuples();
  unsigned int phase1, phase2;
  int parentcount = 0;
  parentnumbers.resize(numgrains, -1);

  for (size_t i = 1; i < numgrains; i++)
  {
	if (parentnumbers[i] == -1 && m_FieldPhases[i] > 0)
    {
	  parentcount++;
	  parentnumbers[i] = parentcount;
	  m_Active[i] = true;
      twinlist.push_back(i);
      for (size_t j = 0; j < twinlist.size(); j++)
      {
        int firstgrain = twinlist[j];
        int size = int(neighborlist[firstgrain].size());
        for (int l = 0; l < size; l++)
        {
          int twin = 0;
          size_t neigh = neighborlist[firstgrain][l];
          if (neigh != i && parentnumbers[neigh] == -1 && m_FieldPhases[neigh] > 0)
          {
            w = 10000.0f;
			q1[0] = 1;
            q1[1] = m_AvgQuats[5*firstgrain+1];
            q1[2] = m_AvgQuats[5*firstgrain+2];
            q1[3] = m_AvgQuats[5*firstgrain+3];
            q1[4] = m_AvgQuats[5*firstgrain+4];
            phase1 = m_CrystalStructures[m_FieldPhases[firstgrain]];
			q2[0] = 1;
            q2[1] = m_AvgQuats[5*neigh+1];
            q2[2] = m_AvgQuats[5*neigh+2];
            q2[3] = m_AvgQuats[5*neigh+3];
            q2[4] = m_AvgQuats[5*neigh+4];
            phase2 = m_CrystalStructures[m_FieldPhases[neigh]];
			if (phase1 == phase2 && phase1 == Ebsd::CrystalStructure::Cubic) 
			{ 
				w = m_OrientationOps[phase1]->getMisoQuat( q1, q2, n1, n2, n3);
				w = w * (180.0f/m_pi);
	            float axisdiff111 = acosf(fabs(n1)*0.57735f+fabs(n2)*0.57735f+fabs(n3)*0.57735f);
	            float angdiff60 = fabs(w-60.0f);
	            if (axisdiff111 < axistol && angdiff60 < angtol) twin = 1;
	            if (twin == 1)
	            {
	              parentnumbers[neigh] = parentcount;
	              twinlist.push_back(neigh);
				}
            }
          }
        }
      }
    }
    twinlist.clear();
  }
  size_t totalPoints = static_cast<size_t>(m->getTotalPoints());
  for (size_t k = 0; k < totalPoints; k++)
  {
    int grainname = m_GrainIds[k];
	m_ParentIds[k] = parentnumbers[grainname];
  }
}

void MergeTwins::characterize_twins()
{
  DataContainer* m = getDataContainer();
  size_t numgrains = m->getNumFieldTuples();
  for (size_t i = 0; i < numgrains; i++)
  {

  }
}

