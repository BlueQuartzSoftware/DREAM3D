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

#include "SegmentGrains.h"

#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Common/DREAM3DMath.h"
#include "DREAM3DLib/Common/OrientationMath.h"
#include "DREAM3DLib/Common/DREAM3DRandom.h"

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
SegmentGrains::SegmentGrains() :
AbstractFilter(),
m_MisorientationTolerance(5.0f),
m_GrainIds(NULL),
m_Quats(NULL),
m_PhasesC(NULL),
m_PhasesF(NULL),
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
SegmentGrains::~SegmentGrains()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------

void SegmentGrains::setupFilterOptions()
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
  setFilterOptions(options);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SegmentGrains::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;
  DataContainer* m = getDataContainer();

  GET_PREREQ_DATA(m, DREAM3D, CellData, GoodVoxels, ss, -301, bool, BoolArrayType,  voxels, 1);
  GET_PREREQ_DATA_SUFFIX(m, DREAM3D, CellData, Phases, C, ss, -302, int32_t, Int32ArrayType,  voxels, 1);
  GET_PREREQ_DATA(m, DREAM3D, CellData, Quats, ss, -303, float, FloatArrayType, voxels, 5);
  CREATE_NON_PREREQ_DATA(m, DREAM3D, CellData, GrainIds, ss, int32_t, Int32ArrayType, voxels, 1);

  CREATE_NON_PREREQ_DATA_SUFFIX(m, DREAM3D, FieldData, Phases, F, ss, int32_t, Int32ArrayType, fields, 1);
  CREATE_NON_PREREQ_DATA(m, DREAM3D, FieldData, Active, ss, bool, BoolArrayType, fields, 1);

  typedef DataArray<unsigned int> XTalStructArrayType;
  GET_PREREQ_DATA(m, DREAM3D, EnsembleData, CrystalStructures, ss, -304, unsigned int, XTalStructArrayType, ensembles, 1);

  setErrorMessage(ss.str());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SegmentGrains::preflight()
{
  DataContainer* m = getDataContainer();
  m->clearFieldData();
 // m->clearEnsembleData();

  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SegmentGrains::execute()
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
  m->clearFieldData();
//  m->clearEnsembleData();

  int64_t totalPoints = m->getTotalPoints();

  // Create at least 2 grains:
  m->resizeFieldDataArrays(2);
  // Update our pointers
  dataCheck(false, totalPoints, m->getNumFieldTuples(), m->getNumEnsembleTuples());
  if (getErrorCondition() < 0)
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

  DREAM3D_RANDOMNG_NEW()
  int seed = 0;
  int noseeds = 0;
  size_t graincount = 1;
  int neighbor;
  float q1[5];
  float q2[5];
  float w;
  float n1, n2, n3;
  int randpoint = 0;
  int good = 0;
  DimType col, row, plane;
  size_t size = 0;
  size_t initialVoxelsListSize = 1000;
  size_t initialMergeListSize = 10;
  std::vector<int> voxelslist(initialVoxelsListSize, -1);
  std::vector<int> mergelist(initialMergeListSize, -1);
  DimType neighpoints[6];
  neighpoints[0] = -(dims[0] * dims[1]);
  neighpoints[1] = -dims[0];
  neighpoints[2] = -1;
  neighpoints[3] = 1;
  neighpoints[4] = dims[0];
  neighpoints[5] = (dims[0] * dims[1]);
  unsigned int phase1, phase2;

  // Precalculate some constants
  int64_t totalPMinus1 = totalPoints - 1;
  for(int64_t i = 0; i < totalPoints; i++)
  {
    m_GrainIds[i] = 0;
  }

  // Burn volume with tight orientation tolerance to simulate simultaneous growth/aglomeration
  while (noseeds == 0)
  {
    seed = -1;
    int counter = 0;
    randpoint = int(float(rg.genrand_res53()) * float(totalPMinus1));
    while (seed == -1 && counter < totalPoints)
    {
      if (randpoint > totalPMinus1) randpoint = randpoint - totalPoints;
      if (m_GoodVoxels[randpoint] == true && m_GrainIds[randpoint] == 0 && m_PhasesC[randpoint] > 0) seed = randpoint;

      randpoint++;
      counter++;
    }
    if(seed == -1) noseeds = 1;
    if(seed >= 0)
    {
      size = 0;
      m_GrainIds[seed] = graincount;
      voxelslist[size] = seed;
      size++;
      for (size_t j = 0; j < size; ++j)
      {
        int currentpoint = voxelslist[j];
        col = currentpoint % dims[0];
        row = (currentpoint / dims[0]) % dims[1];
        plane = currentpoint / (dims[0] * dims[1]);
        phase1 = m_CrystalStructures[m_PhasesC[currentpoint]];
        for (int i = 0; i < 6; i++)
        {
          q1[0] = 1;
          q1[1] = m_Quats[currentpoint * 5 + 1];
          q1[2] = m_Quats[currentpoint * 5 + 2];
          q1[3] = m_Quats[currentpoint * 5 + 3];
          q1[4] = m_Quats[currentpoint * 5 + 4];
          good = 1;
          neighbor = currentpoint + neighpoints[i];

          if (i == 0 && plane == 0) good = 0;
          if (i == 5 && plane == (dims[2] - 1)) good = 0;
          if (i == 1 && row == 0) good = 0;
          if (i == 4 && row == (dims[1] - 1)) good = 0;
          if (i == 2 && col == 0) good = 0;
          if (i == 3 && col == (dims[0] - 1)) good = 0;
          if (good == 1 && m_GrainIds[neighbor] == 0 && m_PhasesC[neighbor] > 0)
          {
            w = 10000.0;
            q2[0] = 1;
            q2[1] = m_Quats[neighbor*5 + 1];
            q2[2] = m_Quats[neighbor*5 + 2];
            q2[3] = m_Quats[neighbor*5 + 3];
            q2[4] = m_Quats[neighbor*5 + 4];
            phase2 = m_CrystalStructures[m_PhasesC[neighbor]];
            if (phase1 == phase2) w = m_OrientationOps[phase1]->getMisoQuat( q1, q2, n1, n2, n3);
            if (w < m_MisorientationTolerance)
            {
              m_GrainIds[neighbor] = graincount;
              voxelslist[size] = neighbor;
              size++;
              if(size >= voxelslist.size()) voxelslist.resize(size + initialVoxelsListSize, -1);
            }
          }
        }
      }
      m_Active[graincount] = true;
      m_PhasesF[graincount] = m_PhasesC[seed];
      graincount++;
      if(graincount >= m->getNumFieldTuples())
      {
        size_t oldSize = m->getNumFieldTuples();
        m->resizeFieldDataArrays(m->getNumFieldTuples() + 100);
        dataCheck(false, m->getTotalPoints(), m->getNumFieldTuples(), m->getNumEnsembleTuples());
        for (size_t g = oldSize; g < m->getNumFieldTuples(); ++g)
        {
          //FIXME::THis needs to set some default data

        }
      }
      voxelslist.clear();
      voxelslist.resize(initialVoxelsListSize, -1);
    }
  }

  m->resizeFieldDataArrays(graincount);
  dataCheck(false, m->getTotalPoints(), m->getNumFieldTuples(), m->getNumEnsembleTuples());

  // If there is an error set this to something negative and also set a message
  notify("SegmentGrains Completed", 0, Observable::UpdateProgressMessage);
}
