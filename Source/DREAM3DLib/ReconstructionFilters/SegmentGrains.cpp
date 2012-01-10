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
m_GrainIds(NULL),
m_Quats(NULL),
m_PhasesC(NULL),
m_PhasesF(NULL),
m_Active(NULL)
{
  m_HexOps = HexagonalOps::New();
  m_OrientationOps.push_back(m_HexOps.get());
  m_CubicOps = CubicOps::New();
  m_OrientationOps.push_back(m_CubicOps.get());
  m_OrthoOps = OrthoRhombicOps::New();
  m_OrientationOps.push_back(m_OrthoOps.get());

  m_MisorientationTolerance = 0.0f;

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
    options.push_back(option);
  }
  setFilterOptions(options);
}


void SegmentGrains::preflight()
{
  int err = 0;
  std::stringstream ss;
  DataContainer::Pointer m = DataContainer::New();
  IDataArray::Pointer d = m->getVoxelData(DREAM3D::VoxelData::Phases);
  if(d.get() == NULL)
  {
	  ss << "Phases (Cells) Array Not Initialized At Beginning of CleanupGrains Filter" << std::endl;
	  err = -300;
  }
  d = m->getVoxelData(DREAM3D::VoxelData::Quats);
  if(d.get() == NULL)
  {
	  ss << "Quats Array Not Initialized At Beginning of SegmentGrains Filter" << std::endl;
	  err = -300;
  }
  PFInt32ArrayType::Pointer p = PFInt32ArrayType::CreateArray(1);
  m->addVoxelData(DREAM3D::VoxelData::GrainIds, p);
  PFInt32ArrayType::Pointer q = PFInt32ArrayType::CreateArray(1);
  m->addFieldData(DREAM3D::FieldData::Phases, q);
  PFBoolArrayType::Pointer r = PFBoolArrayType::CreateArray(1);
  m->addFieldData(DREAM3D::FieldData::Active, r);

  setErrorCondition(err);
  setErrorMessage(ss.str());
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
  int64_t totalPoints = m->totalPoints();

  m_GrainIds = m->getVoxelDataSizeCheck<int32_t, Int32ArrayType, AbstractFilter>(DREAM3D::VoxelData::GrainIds, totalPoints, this);
  if (NULL == m_GrainIds) { return; }
  m_PhasesC = m->getVoxelDataSizeCheck<int32_t, Int32ArrayType, AbstractFilter>(DREAM3D::VoxelData::Phases, totalPoints, this);
  if (NULL == m_PhasesC) { return; }
  m_Quats = m->getVoxelDataSizeCheck<float, FloatArrayType, AbstractFilter>(DREAM3D::VoxelData::Quats, (totalPoints*5), this);
  if (NULL == m_Quats) { return; }

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
  int neighpoints[6];
  neighpoints[0] = -(dims[0] * dims[1]);
  neighpoints[1] = -dims[0];
  neighpoints[2] = -1;
  neighpoints[3] = 1;
  neighpoints[4] = dims[0];
  neighpoints[5] = (dims[0] * dims[1]);
  Ebsd::CrystalStructure phase1, phase2;

  // Precalculate some constants
  int64_t totalPMinus1 = totalPoints - 1;

  // Burn volume with tight orientation tolerance to simulate simultaneous growth/aglomeration
  while (noseeds == 0)
  {
    seed = -1;
    int counter = 0;
    randpoint = int(float(rg.genrand_res53()) * float(totalPMinus1));
    while (seed == -1 && counter < totalPoints)
    {
      if (randpoint > totalPMinus1) randpoint = randpoint - totalPoints;
      if (m_GrainIds[randpoint] == -1 && m_PhasesC[randpoint] > 0) seed = randpoint;

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
        phase1 = m->crystruct[m_PhasesC[currentpoint]];
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
          if (good == 1 && m_GrainIds[neighbor] == -1 && m_PhasesC[neighbor] > 0)
          {
            w = 10000.0;
            q2[0] = 1;
            q2[1] = m_Quats[neighbor*5 + 1];
            q2[2] = m_Quats[neighbor*5 + 2];
            q2[3] = m_Quats[neighbor*5 + 3];
            q2[4] = m_Quats[neighbor*5 + 4];
            phase2 = m->crystruct[m_PhasesC[neighbor]];
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
      m_Active[graincount] = 1;
      m_PhasesF[graincount] = m_PhasesC[seed];
      graincount++;
      if(graincount >= m->getTotalFields())
      {
        size_t oldSize = m->getTotalFields();
        m->resizeFieldDataArrays(m->getTotalFields() + 100);
        for (size_t g = oldSize; g < m->getTotalFields(); ++g)
        {
          m->m_Grains[g] = Field::New();
        }
      }
      voxelslist.clear();
      voxelslist.resize(initialVoxelsListSize, -1);
    }
  }

  // If there is an error set this to something negative and also set a message
  notify("SegmentGrains Completed", 0, Observable::UpdateProgressMessage);
}
