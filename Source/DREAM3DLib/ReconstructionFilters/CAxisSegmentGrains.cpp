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
#include "DREAM3DLib/Math/DREAM3DMath.h"
#include "DREAM3DLib/Math/MatrixMath.h"
#include "DREAM3DLib/OrientationOps/OrientationOps.h"
#include "DREAM3DLib/Utilities/DREAM3DRandom.h"

#include "DREAM3DLib/GenericFilters/FindCellQuats.h"

#define ERROR_TXT_OUT 1
#define ERROR_TXT_OUT1 1




#define NEW_SHARED_ARRAY(var, m_msgType, size)\
  boost::shared_array<m_msgType> var##Array(new m_msgType[size]);\
  m_msgType* var = var##Array.get();



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
m_CrystalStructuresArrayName(DREAM3D::EnsembleData::CrystalStructures),
m_MisorientationTolerance(5.0f),
m_RandomizeGrainIds(true),
m_GrainIds(NULL),
m_Quats(NULL),
m_CellPhases(NULL),
m_GoodVoxels(NULL),
m_Active(NULL),
m_CrystalStructures(NULL)
{
  m_OrientationOps = OrientationOps::getOrientationOpsVector();

  setupFilterParameters();
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

void CAxisSegmentGrains::setupFilterParameters()
{
  std::vector<FilterParameter::Pointer> parameters;
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setPropertyName("MisorientationTolerance");
    option->setHumanLabel("C-Axis Misorientation Tolerance");
    option->setWidgetType(FilterParameter::DoubleWidget);
    option->setValueType("float");
    option->setCastableValueType("double");
  option->setUnits("Degrees");
    parameters.push_back(option);
  }
#if 0
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Randomly Reorder Generated Grain Ids");
    option->setPropertyName("RandomizeGrainIds");
    option->setWidgetType(FilterParameter::BooleanWidget);
    option->setValueType("bool");
    parameters.push_back(option);
  }
#endif
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CAxisSegmentGrains::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  /* Code to read the values goes between these statements */
/* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE BEGIN*/
  setMisorientationTolerance( reader->readValue("MisorientationTolerance", getMisorientationTolerance()) );
/* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE END*/
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int CAxisSegmentGrains::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("MisorientationTolerance", getMisorientationTolerance() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CAxisSegmentGrains::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;
  VolumeDataContainer* m = getVolumeDataContainer();


  CREATE_NON_PREREQ_DATA(m, DREAM3D, CellData, GoodVoxels, ss, bool, BoolArrayType,  true, voxels, 1)
  GET_PREREQ_DATA(m, DREAM3D, CellData, CellPhases, ss, -302, int32_t, Int32ArrayType,  voxels, 1)

  GET_PREREQ_DATA(m, DREAM3D, CellData, Quats, ss, -303, float, FloatArrayType, voxels, 4)

  CREATE_NON_PREREQ_DATA(m, DREAM3D, CellData, GrainIds, ss, int32_t, Int32ArrayType, 0, voxels, 1)
  CREATE_NON_PREREQ_DATA(m, DREAM3D, FieldData, Active, ss, bool, BoolArrayType, true, fields, 1)

  typedef DataArray<unsigned int> XTalStructArrayType;
  GET_PREREQ_DATA(m, DREAM3D, EnsembleData, CrystalStructures, ss, -304, unsigned int, XTalStructArrayType, ensembles, 1)

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
  VolumeDataContainer* m = getVolumeDataContainer();
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The DataContainer Object was NULL", -999);
    return;
  }

  int64_t totalPoints = m->getTotalPoints();
  m->resizeFieldDataArrays(1);
  dataCheck(false, totalPoints, m->getNumFieldTuples(), m->getNumEnsembleTuples());
  if (getErrorCondition() < 0)
  {
    return;
  }

  //Convert user defined tolerance to radians.
  m_MisorientationTolerance = m_MisorientationTolerance * DREAM3D::Constants::k_Pi/180.0f;
  for(int64_t i=0;i<totalPoints;i++)
  {
    m_GrainIds[i] = 0;
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
    generator.seed(static_cast<boost::uint32_t>( MXA::getMilliSeconds() )); // seed with the current time

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
  for(size_t i = 0; i < totalFields; i++)
  {
    m_Active[i] = true;
  }
  }

  // If there is an error set this to something negative and also set a message
 notifyStatusMessage("Completed");
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int64_t CAxisSegmentGrains::getSeed(size_t gnum)
{
  setErrorCondition(0);
  VolumeDataContainer* m = getVolumeDataContainer();
  if (NULL == m)
  {
    setErrorCondition(-1);
    std::stringstream ss;
    ss << " DataContainer was NULL";
    addErrorMessage(getHumanLabel(), ss.str(), -1);
    return -1;
  }

  int64_t totalPoints = m->getTotalPoints();

  DREAM3D_RANDOMNG_NEW()
  int64_t seed = -1;
  int64_t randpoint = 0;

  // Precalculate some constants
  int64_t totalPMinus1 = totalPoints - 1;

  int64_t counter = 0;
  randpoint = int64_t(float(rg.genrand_res53()) * float(totalPMinus1));
  while (seed == -1 && counter < totalPoints)
  {
      if (randpoint > totalPMinus1) randpoint = static_cast<int64_t>( randpoint - totalPoints );
      if (m_GoodVoxels[randpoint] == true && m_GrainIds[randpoint] == 0 && m_CellPhases[randpoint] > 0) seed = randpoint;
      randpoint++;
      counter++;
  }
  if (seed >= 0)
  {
    m_GrainIds[seed] = gnum;
    m->resizeFieldDataArrays(gnum+1);
    dataCheck(false, totalPoints, m->getNumFieldTuples(), m->getNumEnsembleTuples());
  }
  return seed;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool CAxisSegmentGrains::determineGrouping(int64_t referencepoint, int64_t neighborpoint, size_t gnum)
{
  bool group = false;
  float w = 10000.0;
  QuatF q1;
  QuatF q2;
  QuatF* quats = reinterpret_cast<QuatF*>(m_Quats);
  float g1[3][3];
  float g2[3][3];
  float g1t[3][3];
  float g2t[3][3];
 // float n1, n2, n3;
  unsigned int phase1, phase2;
  float caxis[3] = {0,0,1};
  float c1[3];
  float c2[3];

  if(m_GrainIds[neighborpoint] == 0 && m_GoodVoxels[neighborpoint] == true)
  {
    phase1 = m_CrystalStructures[m_CellPhases[referencepoint]];
    QuaternionMathF::Copy(quats[referencepoint], q1);

    phase2 = m_CrystalStructures[m_CellPhases[neighborpoint]];
    QuaternionMathF::Copy(quats[neighborpoint], q2);

    if (m_CellPhases[referencepoint] == m_CellPhases[neighborpoint])
    {
      OrientationMath::QuattoMat(q1, g1);
      OrientationMath::QuattoMat(q2, g2);

      //transpose the g matricies so when caxis is multiplied by it
      //it will give the sample direction that the caxis is along
      MatrixMath::Transpose3x3(g1, g1t);
      MatrixMath::Transpose3x3(g2, g2t);

      MatrixMath::Multiply3x3with3x1(g1t, caxis, c1);
      MatrixMath::Multiply3x3with3x1(g2t, caxis, c2);

      //normalize so that the dot product can be taken below without
      //dividing by the magnitudes (they would be 1)
      MatrixMath::Normalize3x1(c1);
      MatrixMath::Normalize3x1(c2);

      w = ((c1[0]*c2[0])+(c1[1]*c2[1])+(c1[2]*c2[2]));
      w = acosf(w);
      if (w <= m_MisorientationTolerance || (DREAM3D::Constants::k_Pi-w) <= m_MisorientationTolerance)
      {
        group = true;
        m_GrainIds[neighborpoint] = gnum;
      }

    }
  }

  return group;
}
