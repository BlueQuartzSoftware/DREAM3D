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

#include "FindGrainReferenceMisorientations.h"

#include <limits>


#include "DREAM3DLib/Common/DREAM3DMath.h"
#include "DREAM3DLib/Common/Constants.h"

#include "DREAM3DLib/GenericFilters/FindCellQuats.h"
#include "DREAM3DLib/StatisticsFilters/FindAvgOrientations.h"
#include "DREAM3DLib/StatisticsFilters/FindEuclideanDistMap.h"

const static float m_pi = static_cast<float>(M_PI);

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindGrainReferenceMisorientations::FindGrainReferenceMisorientations() :
  AbstractFilter(),
  m_GrainIdsArrayName(DREAM3D::CellData::GrainIds),
  m_CellPhasesArrayName(DREAM3D::CellData::Phases),
  m_QuatsArrayName(DREAM3D::CellData::Quats),
  m_GBEuclideanDistancesArrayName(DREAM3D::CellData::GBEuclideanDistances),
  m_GrainReferenceMisorientationsArrayName(DREAM3D::CellData::GrainReferenceMisorientations),
  m_AvgQuatsArrayName(DREAM3D::FieldData::AvgQuats),
  m_GrainAvgMisorientationsArrayName(DREAM3D::FieldData::GrainAvgMisorientations),
  m_CrystalStructuresArrayName(DREAM3D::EnsembleData::CrystalStructures),
  m_ReferenceOrientation(0),
  m_GrainIds(NULL),
  m_CellPhases(NULL),
  m_GBEuclideanDistances(NULL),
  m_GrainReferenceMisorientations(NULL),
  m_AvgQuats(NULL),
  m_GrainAvgMisorientations(NULL),
  m_Quats(NULL),
  m_CrystalStructures(NULL)
{
  m_OrientationOps = OrientationMath::getOrientationOpsVector();

  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindGrainReferenceMisorientations::~FindGrainReferenceMisorientations()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindGrainReferenceMisorientations::setupFilterParameters()
{
  std::vector<FilterParameter::Pointer> parameters;
  {
    ChoiceFilterParameter::Pointer option = ChoiceFilterParameter::New();
    option->setHumanLabel("Reference Orientation");
    option->setPropertyName("ReferenceOrientation");
    option->setWidgetType(FilterParameter::ChoiceWidget);
    option->setValueType("unsigned int");
    std::vector<std::string> choices;
    choices.push_back("Grain's Average Orientation");
    choices.push_back("Orientation at Grain's Centroid");
    option->setChoices(choices);
    parameters.push_back(option);
  }
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
void FindGrainReferenceMisorientations::writeFilterParameters(AbstractFilterParametersWriter* writer)
{
  writer->writeValue("ReferenceOrientation", getReferenceOrientation() );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindGrainReferenceMisorientations::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;
  VoxelDataContainer* m = getVoxelDataContainer();

  GET_PREREQ_DATA(m, DREAM3D, CellData, GrainIds, ss, -300, int32_t, Int32ArrayType, voxels, 1)
      GET_PREREQ_DATA(m, DREAM3D, CellData, CellPhases, ss, -300, int32_t, Int32ArrayType,  voxels, 1)

  GET_PREREQ_DATA(m, DREAM3D, CellData, Quats, ss, -303, float, FloatArrayType, voxels, 5)

      CREATE_NON_PREREQ_DATA(m, DREAM3D, CellData, GrainReferenceMisorientations, ss, float, FloatArrayType, 0, voxels, 1)

      if(m_ReferenceOrientation == 0)
  {
    GET_PREREQ_DATA(m, DREAM3D, FieldData, AvgQuats, ss, -303, float, FloatArrayType, fields, 5)
  }
  else if(m_ReferenceOrientation == 1)
  {
    GET_PREREQ_DATA(m, DREAM3D, CellData, GBEuclideanDistances, ss, -301, float, FloatArrayType, voxels, 1)
  }

  CREATE_NON_PREREQ_DATA(m, DREAM3D, FieldData, GrainAvgMisorientations, ss, float, FloatArrayType, 0, fields, 1)

  typedef DataArray<unsigned int> XTalStructArrayType;
  GET_PREREQ_DATA(m, DREAM3D, EnsembleData, CrystalStructures, ss, -305, unsigned int, XTalStructArrayType, ensembles, 1)
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindGrainReferenceMisorientations::preflight()
{
  dataCheck(true, 1,1,1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindGrainReferenceMisorientations::execute()
{
  setErrorCondition(0);

  VoxelDataContainer* m = getVoxelDataContainer();
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The DataContainer Object was NULL", -999);
    return;
  }

  int64_t totalPoints = m->getTotalPoints();
  int64_t totalFields = m->getNumFieldTuples();

  dataCheck(false, totalPoints, totalFields, m->getNumEnsembleTuples());
  if (getErrorCondition() < 0)
  {
    return;
  }

  float** avgmiso = new float *[m->getNumFieldTuples()];
  for (size_t i = 1; i < m->getNumFieldTuples(); i++)
  {
    avgmiso[i] = new float[2];
    for (int j = 0; j < 2; j++)
    {
      avgmiso[i][j] = 0.0;
    }
  }

  float q1[5];
  float q2[5];

  float w;
  float n1, n2, n3;
  float r1, r2, r3;
  unsigned int phase1 = Ebsd::CrystalStructure::UnknownCrystalStructure;
  unsigned int phase2 = Ebsd::CrystalStructure::UnknownCrystalStructure;
  size_t udims[3] = {0,0,0};
  m->getDimensions(udims);
#if (CMP_SIZEOF_SIZE_T == 4)
  typedef uint32_t DimType;
  uint32_t maxUInt32 = std::numeric_limits<uint32_t>::max();
  // We have more points than can be allocated on a 32 bit machine. Assert Now.
  if(totalPoints > maxUInt32)
  {
    setErrorCondition(-666);
    notifyErrorMessage("More Points than can be help in memory on a 32 bit machine. Try reducing the size of the input volume.", -666);
    return;
  }
#else
  typedef int64_t DimType;
#endif



  size_t gnum;
  float dist;
  std::vector<size_t> m_Centers(totalFields,0);
  std::vector<float> m_CenterDists(totalFields,0);
  if(m_ReferenceOrientation == 1)
  {
    for (int64_t i = 0; i < totalPoints; i++)
    {
      gnum = m_GrainIds[i];
      dist = m_GBEuclideanDistances[i];
      if(dist >= m_CenterDists[gnum])
      {
        m_CenterDists[gnum] = dist;
        m_Centers[gnum] = i;
      }
    }
  }

  DimType xPoints = static_cast<DimType>(udims[0]);
  DimType yPoints = static_cast<DimType>(udims[1]);
  DimType zPoints = static_cast<DimType>(udims[2]);
  DimType point;
  for (DimType col = 0; col < xPoints; col++)
  {
    for (DimType row = 0; row < yPoints; row++)
    {
      for (DimType plane = 0; plane < zPoints; plane++)
      {
        point = (plane * xPoints * yPoints) + (row * xPoints) + col;
        if (m_GrainIds[point] > 0 && m_CellPhases[point] > 0)
        {
          q1[1] = m_Quats[point*5 + 1];
          q1[2] = m_Quats[point*5 + 2];
          q1[3] = m_Quats[point*5 + 3];
          q1[4] = m_Quats[point*5 + 4];
          phase1 = m_CrystalStructures[m_CellPhases[point]];
          if(m_ReferenceOrientation == 0)
          {
            q2[0] = m_AvgQuats[5*m_GrainIds[point]];
            q2[1] = m_AvgQuats[5*m_GrainIds[point]+1];
            q2[2] = m_AvgQuats[5*m_GrainIds[point]+2];
            q2[3] = m_AvgQuats[5*m_GrainIds[point]+3];
            q2[4] = m_AvgQuats[5*m_GrainIds[point]+4];
          }
          else if(m_ReferenceOrientation == 1)
          {
            gnum = m_GrainIds[point];
            q2[1] = m_Quats[m_Centers[gnum]*5 + 1];
            q2[2] = m_Quats[m_Centers[gnum]*5 + 2];
            q2[3] = m_Quats[m_Centers[gnum]*5 + 3];
            q2[4] = m_Quats[m_Centers[gnum]*5 + 4];
            phase2 = m_CrystalStructures[m_CellPhases[m_Centers[gnum]]];
          }
          w = m_OrientationOps[phase1]->getMisoQuat( q1, q2, n1, n2, n3);
          OrientationMath::axisAngletoRod(w, n1, n2, n3, r1, r2, r3);
          m_OrientationOps[phase1]->getMDFFZRod(r1, r2, r3);
          w = w *(180.0f/m_pi);
          m_GrainReferenceMisorientations[point] = w;
          avgmiso[m_GrainIds[point]][0]++;
          avgmiso[m_GrainIds[point]][1] = avgmiso[m_GrainIds[point]][1] + w;
        }
        if (m_GrainIds[point] == 0 || m_CellPhases[point] == 0)
        {
          m_GrainReferenceMisorientations[point] = 0;
        }
      }
    }
  }

  size_t grainsSize = m->getNumFieldTuples();
  for (size_t i = 1; i < grainsSize; i++)
  {
    m_GrainAvgMisorientations[i] = avgmiso[i][1] / avgmiso[i][0];
    if(avgmiso[i][0] == 0) m_GrainAvgMisorientations[i] = 0.0;
  }

  // Clean up all the heap allocated memory
  for (size_t i = 1; i < m->getNumFieldTuples(); i++)
  {
    delete[] avgmiso[i];
  }
  delete avgmiso;

  notifyStatusMessage("FindGrainReferenceMisorientations Completed");
}
