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

#include "FindAvgOrientations.h"

#include "DREAM3DLib/Common/DREAM3DMath.h"
#include "DREAM3DLib/Common/Constants.h"

#include "DREAM3DLib/GenericFilters/FindCellQuats.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindAvgOrientations::FindAvgOrientations() :
  AbstractFilter(),
  m_GrainIdsArrayName(DREAM3D::CellData::GrainIds),
  m_CellPhasesArrayName(DREAM3D::CellData::Phases),
  m_QuatsArrayName(DREAM3D::CellData::Quats),
  m_AvgQuatsArrayName(DREAM3D::FieldData::AvgQuats),
  m_FieldEulerAnglesArrayName(DREAM3D::FieldData::EulerAngles),
  m_CrystalStructuresArrayName(DREAM3D::EnsembleData::CrystalStructures),
  m_GrainIds(NULL),
  m_CellPhases(NULL),
  m_FieldEulerAngles(NULL),
  m_Quats(NULL),
  m_AvgQuats(NULL)
{
  m_OrientationOps = OrientationMath::getOrientationOpsVector();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindAvgOrientations::~FindAvgOrientations()
{
}
// -----------------------------------------------------------------------------
void FindAvgOrientations::writeFilterParameters(AbstractFilterParametersWriter* writer)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindAvgOrientations::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;
  VoxelDataContainer* m = getVoxelDataContainer();
  GET_PREREQ_DATA(m, DREAM3D, CellData, GrainIds, ss, -300, int32_t, Int32ArrayType,  voxels, 1)
  GET_PREREQ_DATA(m, DREAM3D, CellData, CellPhases, ss, -300, int32_t, Int32ArrayType,  voxels, 1)

  GET_PREREQ_DATA(m, DREAM3D, CellData, Quats, ss, -303, float, FloatArrayType, voxels, 5)

  CREATE_NON_PREREQ_DATA(m, DREAM3D, FieldData, AvgQuats, ss, float, FloatArrayType, 0, fields, 5)
  CREATE_NON_PREREQ_DATA(m, DREAM3D, FieldData, FieldEulerAngles, ss, float, FloatArrayType, 0, fields, 3)

  typedef DataArray<unsigned int> XTalStructArrayType;
  GET_PREREQ_DATA(m, DREAM3D, EnsembleData, CrystalStructures, ss, -305, unsigned int, XTalStructArrayType, ensembles, 1)
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindAvgOrientations::preflight()
{
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindAvgOrientations::execute()
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
  dataCheck(false, m->getTotalPoints(), m->getNumFieldTuples(), m->getNumEnsembleTuples());
  if (getErrorCondition() < 0)
  {
    return;
  }

  size_t numgrains = m->getNumFieldTuples();
  int phase;
  float voxquat[5];
  float curavgquat[5];


  for (size_t i = 1; i < numgrains; i++)
  {
    m_AvgQuats[5*i] = 0.0;
    m_AvgQuats[5*i+1] = 0.0;
    m_AvgQuats[5*i+2] = 0.0;
    m_AvgQuats[5*i+3] = 0.0;
    m_AvgQuats[5*i+4] = 0.0;
  }
  //  float qr[5];
  for(int i = 0; i < totalPoints; i++)
  {
    if(m_GrainIds[i] > 0 && m_CellPhases[i] > 0)
    {

      phase = m_CellPhases[i];
      voxquat[0] = m_Quats[i*5 + 0];
      voxquat[1] = m_Quats[i*5 + 1];
      voxquat[2] = m_Quats[i*5 + 2];
      voxquat[3] = m_Quats[i*5 + 3];
      voxquat[4] = m_Quats[i*5 + 4];
      curavgquat[0] = 1;
      curavgquat[1] = m_AvgQuats[5*m_GrainIds[i]+1]/m_AvgQuats[5*m_GrainIds[i]];
      curavgquat[2] = m_AvgQuats[5*m_GrainIds[i]+2]/m_AvgQuats[5*m_GrainIds[i]];
      curavgquat[3] = m_AvgQuats[5*m_GrainIds[i]+3]/m_AvgQuats[5*m_GrainIds[i]];
      curavgquat[4] = m_AvgQuats[5*m_GrainIds[i]+4]/m_AvgQuats[5*m_GrainIds[i]];
      if(m_AvgQuats[5*m_GrainIds[i]] == 0)
      {
        curavgquat[0] = 1;
        curavgquat[1] = 0;
        curavgquat[2] = 0;
        curavgquat[3] = 0;
        curavgquat[4] = 1;
      }
      m_OrientationOps[m_CrystalStructures[phase]]->getNearestQuat(curavgquat, voxquat);
      for (int k = 0; k < 5; k++)
      {
        m_AvgQuats[5*m_GrainIds[i]+k] = m_AvgQuats[5*m_GrainIds[i]+k] + voxquat[k];
      }
    }
  }
  float q[5];
  float ea1, ea2, ea3;
  for (size_t i = 1; i < numgrains; i++)
  {
    if(m_AvgQuats[5*i] == 0)
    {
      m_AvgQuats[5*i] = 1;
      m_AvgQuats[5*i+1] = 0;
      m_AvgQuats[5*i+2] = 0;
      m_AvgQuats[5*i+3] = 0;
      m_AvgQuats[5*i+4] = 1;
    }
    q[1] = m_AvgQuats[5*i+1]/m_AvgQuats[5*i];
    q[2] = m_AvgQuats[5*i+2]/m_AvgQuats[5*i];
    q[3] = m_AvgQuats[5*i+3]/m_AvgQuats[5*i];
    q[4] = m_AvgQuats[5*i+4]/m_AvgQuats[5*i];
    OrientationMath::normalizeQuat(q);
    OrientationMath::QuattoEuler(q, ea1, ea2, ea3);
    m_FieldEulerAngles[3*i] = ea1;
    m_FieldEulerAngles[3*i+1] = ea2;
    m_FieldEulerAngles[3*i+2] = ea3;
    m_AvgQuats[5*i+1] = q[1];
    m_AvgQuats[5*i+2] = q[2];
    m_AvgQuats[5*i+3] = q[3];
    m_AvgQuats[5*i+4] = q[4];
    m_AvgQuats[5*i] = 1;
  }

  notifyStatusMessage("Completed");
}


