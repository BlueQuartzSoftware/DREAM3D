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

#include "CAxisSegmentGrains.h"

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/variate_generator.hpp>

#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Common/DREAM3DMath.h"
#include "DREAM3DLib/Common/OrientationMath.h"
#include "DREAM3DLib/Common/DREAM3DRandom.h"

#include "DREAM3DLib/OrientationOps/CubicOps.h"
#include "DREAM3DLib/OrientationOps/HexagonalOps.h"
#include "DREAM3DLib/OrientationOps/OrthoRhombicOps.h"

#include "DREAM3DLib/GenericFilters/FindCellQuats.h"

#define ERROR_TXT_OUT 1
#define ERROR_TXT_OUT1 1

const static float m_pi = M_PI;


#define NEW_SHARED_ARRAY(var, type, size)\
  boost::shared_array<type> var##Array(new type[size]);\
  type* var = var##Array.get();



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CAxisSegmentGrains::CAxisSegmentGrains() :
SegmentGrains(),
m_GoodVoxelsArrayName(DREAM3D::CellData::GoodVoxels),
m_CellPhasesArrayName(DREAM3D::CellData::Phases),
m_QuatsArrayName(DREAM3D::CellData::Quats),
m_GrainIdsArrayName(DREAM3D::CellData::GrainIds),
m_ActiveArrayName(DREAM3D::FieldData::Active),
m_FieldPhasesArrayName(DREAM3D::FieldData::Phases),
m_CrystalStructuresArrayName(DREAM3D::EnsembleData::CrystalStructures),
m_MisorientationTolerance(5.0f),
m_RandomizeGrainIds(true),
m_GrainIds(NULL),
m_Quats(NULL),
m_CellPhases(NULL),
m_FieldPhases(NULL),
m_Active(NULL),
m_CrystalStructures(NULL)
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
CAxisSegmentGrains::~CAxisSegmentGrains()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------

void CAxisSegmentGrains::setupFilterOptions()
{
  std::vector<FilterOption::Pointer> options;
  {
    FilterOption::Pointer option = FilterOption::New();
    option->setPropertyName("MisorientationTolerance");
    option->setHumanLabel("Misorientation Tolerance");
    option->setWidgetType(FilterOption::DoubleWidget);
    option->setValueType("float");
    option->setCastableValueType("double");
    options.push_back(option);
  }
#if 0
  {
    FilterOption::Pointer option = FilterOption::New();
    option->setHumanLabel("Randomly Reorder Generated Grain Ids");
    option->setPropertyName("RandomizeGrainIds");
    option->setWidgetType(FilterOption::BooleanWidget);
    option->setValueType("bool");
    options.push_back(option);
  }
#endif
  setFilterOptions(options);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CAxisSegmentGrains::writeFilterOptions(AbstractFilterOptionsWriter* writer)
{
  writer->writeValue("MisorientationTolerance", getMisorientationTolerance() );
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CAxisSegmentGrains::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;
  DataContainer* m = getDataContainer();

  GET_PREREQ_DATA(m, DREAM3D, CellData, GoodVoxels, ss, -301, bool, BoolArrayType,  voxels, 1);
  GET_PREREQ_DATA(m, DREAM3D, CellData, CellPhases, ss, -302, int32_t, Int32ArrayType,  voxels, 1);
  GET_PREREQ_DATA(m, DREAM3D, CellData, Quats, ss, -303, float, FloatArrayType, voxels, 5);
  if(getErrorCondition() == -303)
  {
    setErrorCondition(0);
    FindCellQuats::Pointer find_cellquats = FindCellQuats::New();
    find_cellquats->setObservers(this->getObservers());
    find_cellquats->setDataContainer(getDataContainer());
    if(preflight == true) find_cellquats->preflight();
    if(preflight == false) find_cellquats->execute();
    GET_PREREQ_DATA(m, DREAM3D, CellData, Quats, ss, -303, float, FloatArrayType, voxels, 5);
  }
  CREATE_NON_PREREQ_DATA(m, DREAM3D, CellData, GrainIds, ss, int32_t, Int32ArrayType, 0, voxels, 1);

  CREATE_NON_PREREQ_DATA(m, DREAM3D, FieldData, FieldPhases, ss, int32_t, Int32ArrayType, 0, fields, 1);
  CREATE_NON_PREREQ_DATA(m, DREAM3D, FieldData, Active, ss, bool, BoolArrayType, true, fields, 1);

  typedef DataArray<unsigned int> XTalStructArrayType;
  GET_PREREQ_DATA(m, DREAM3D, EnsembleData, CrystalStructures, ss, -304, unsigned int, XTalStructArrayType, ensembles, 1);

  setErrorMessage(ss.str());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CAxisSegmentGrains::preflight()
{
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CAxisSegmentGrains::execute()
{
  setErrorCondition(0);
  DataContainer* m = getDataContainer();
  if (NULL == m)
  {
    setErrorCondition(-1);
    std::stringstream ss;
    ss << getNameOfClass() << " DataContainer was NULL";
    setErrorMessage(ss.str());
    return;
  }

  int64_t totalPoints = m->getTotalPoints();
  dataCheck(false, totalPoints, m->getNumFieldTuples(), m->getNumEnsembleTuples());
  if (getErrorCondition() < 0)
  {
    return;
  }

  SegmentGrains::execute();

  if (true == m_RandomizeGrainIds)
  {
    totalPoints = m->getTotalPoints();
    size_t totalFields = m->getNumFieldTuples();
    dataCheck(false, totalPoints, m->getNumFieldTuples(), m->getNumEnsembleTuples());
    if (getErrorCondition() < 0)
    {
      return;
    }

    // Generate all the numbers up front
    const int rangeMin = 1;
    const int rangeMax = totalFields-1;
    typedef boost::uniform_int<int> NumberDistribution;
    typedef boost::mt19937 RandomNumberGenerator;
    typedef boost::variate_generator<RandomNumberGenerator&,
                                     NumberDistribution> Generator;

    NumberDistribution distribution(rangeMin, rangeMax);
    RandomNumberGenerator generator;
    Generator numberGenerator(generator, distribution);
    generator.seed(MXA::getMilliSeconds()); // seed with the current time

    DataArray<int32_t>::Pointer rndNumbers = DataArray<int32_t>::CreateArray(totalFields, "New GrainIds");
    int32_t* gid = rndNumbers->GetPointer(0);
    gid[0] = 0;
    std::set<int32_t> grainIdSet;
    grainIdSet.insert(0);
    for(size_t i = 1; i < totalFields; ++i)
    {
      gid[i] = i; //numberGenerator();
      grainIdSet.insert(gid[i]);
    }

    size_t r;
    size_t temp;
    //--- Shuffle elements by randomly exchanging each with one other.
    for (size_t i=1; i< totalFields; i++) {
        r = numberGenerator(); // Random remaining position.

        temp = gid[i];
        gid[i] = gid[r];
        gid[r] = temp;
    }

    // Now adjust all the Grain Id values for each Voxel
    for(int64_t i = 0; i < totalPoints; ++i)
    {
       m_GrainIds[i] = gid[ m_GrainIds[i] ];
    }
  }


  // If there is an error set this to something negative and also set a message
  notify("Completed", 0, Observable::UpdateProgressMessage);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int CAxisSegmentGrains::getSeed(size_t gnum)
{
  setErrorCondition(0);
  DataContainer* m = getDataContainer();
  if (NULL == m)
  {
    setErrorCondition(-1);
    std::stringstream ss;
    ss << getNameOfClass() << " DataContainer was NULL";
    setErrorMessage(ss.str());
    return -1;
  }

  int64_t totalPoints = m->getTotalPoints();

  DREAM3D_RANDOMNG_NEW()
  int seed = -1;
  int randpoint = 0;

  // Precalculate some constants
  int64_t totalPMinus1 = totalPoints - 1;

  int counter = 0;
  randpoint = int(float(rg.genrand_res53()) * float(totalPMinus1));
  while (seed == -1 && counter < totalPoints)
  {
      if (randpoint > totalPMinus1) randpoint = randpoint - totalPoints;
      if (m_GoodVoxels[randpoint] == true && m_GrainIds[randpoint] == 0 && m_CellPhases[randpoint] > 0) seed = randpoint;
      randpoint++;
      counter++;
  }
  if (seed >= 0)
  {
	  m_GrainIds[seed] = gnum;
	  m->resizeFieldDataArrays(gnum+1);
	  dataCheck(false, totalPoints, m->getNumFieldTuples(), m->getNumEnsembleTuples());
	  m_Active[gnum] = true;
	  m_FieldPhases[gnum] = m_CellPhases[seed];
  }
  return seed;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool CAxisSegmentGrains::determineGrouping(int referencepoint, int neighborpoint, size_t gnum)
{
  bool group = false;
  float w = 10000.0;
  float q1[5];
  float q2[5];
  float n1, n2, n3;
  unsigned int phase1, phase2;
  float cx1, cx2, cy1, cy2, cz1, cz2;
  float denom1, denom2;

  if(m_GrainIds[neighborpoint] == 0)
  {
	  phase1 = m_CrystalStructures[m_CellPhases[referencepoint]];
	  q1[0] = 1;
	  q1[1] = m_Quats[referencepoint * 5 + 1];
	  q1[2] = m_Quats[referencepoint * 5 + 2];
	  q1[3] = m_Quats[referencepoint * 5 + 3];
	  q1[4] = m_Quats[referencepoint * 5 + 4];
	  cx1 = (2 * q1[1] * q1[3] + 2 * q1[2] * q1[4]) * 1;
	  cy1 = (2 * q1[2] * q1[3] - 2 * q1[1] * q1[4]) * 1;
	  cz1 = (1 - 2 * q1[1] * q1[1] - 2 * q1[2] * q1[2]) * 1;
	  denom1 = sqrt((cx1*cx1)+(cy1*cy1)+(cz1*cz1));

	  phase2 = m_CrystalStructures[m_CellPhases[neighborpoint]];
	  q2[0] = 1;
	  q2[1] = m_Quats[neighborpoint*5 + 1];
	  q2[2] = m_Quats[neighborpoint*5 + 2];
	  q2[3] = m_Quats[neighborpoint*5 + 3];
	  q2[4] = m_Quats[neighborpoint*5 + 4];
	  cx2 = (2 * q2[1] * q2[3] + 2 * q2[2] * q2[4]) * 1;
	  cy2 = (2 * q2[2] * q2[3] - 2 * q2[1] * q2[4]) * 1;
	  cz2 = (1 - 2 * q2[1] * q2[1] - 2 * q2[2] * q2[2]) * 1;
	  denom2 = sqrt((cx2*cx2)+(cy2*cy2)+(cz2*cz2));

	  w = ((cx1*cx2)+(cy1*cy2)+(cz1*cz2))/(denom1*denom2);
	  w = 180.0*acosf(w)/m_pi;
      if (w <= m_MisorientationTolerance)
	  {
		group = true;
		m_GrainIds[neighborpoint] = gnum;
	  }
  }

  return group;
}
