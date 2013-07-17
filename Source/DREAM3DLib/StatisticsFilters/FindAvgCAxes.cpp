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

#include "FindAvgCAxes.h"

#include "DREAM3DLib/Common/DREAM3DMath.h"
#include "DREAM3DLib/Common/MatrixMath.h"
#include "DREAM3DLib/Common/Constants.h"

#include "DREAM3DLib/GenericFilters/FindCellQuats.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindAvgCAxes::FindAvgCAxes() :
AbstractFilter(),
m_GrainIdsArrayName(DREAM3D::CellData::GrainIds),
m_QuatsArrayName(DREAM3D::CellData::Quats),
m_AvgCAxesArrayName(DREAM3D::FieldData::AvgCAxes),
m_GrainIds(NULL),
m_Quats(NULL),
m_AvgCAxes(NULL)
{
  m_OrientationOps = OrientationMath::getOrientationOpsVector();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindAvgCAxes::~FindAvgCAxes()
{
}
// -----------------------------------------------------------------------------
void FindAvgCAxes::readFilterParameters(AbstractFilterParametersReader* reader)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int FindAvgCAxes::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindAvgCAxes::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;
  VoxelDataContainer* m = getVoxelDataContainer();
  GET_PREREQ_DATA(m, DREAM3D, CellData, GrainIds, ss, -300, int32_t, Int32ArrayType,  voxels, 1)

  GET_PREREQ_DATA(m, DREAM3D, CellData, Quats, ss, -303, float, FloatArrayType, voxels, 5)

  CREATE_NON_PREREQ_DATA(m, DREAM3D, FieldData, AvgCAxes, ss, float, FloatArrayType, 0, fields, 3)
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindAvgCAxes::preflight()
{
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindAvgCAxes::execute()
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
  //int phase;
  float q1[5];
  float g1[3][3];
  float g1t[3][3];
 // float n1, n2, n3;
//  unsigned int phase1, phase2;
  float caxis[3] = {0,0,1};
  float c1[3];

  std::vector<int> counter;
  counter.resize(numgrains,0);

  for (size_t i = 1; i < numgrains; i++)
  {
    m_AvgCAxes[3*i] = 0.0;
    m_AvgCAxes[3*i+1] = 0.0;
    m_AvgCAxes[3*i+2] = 0.0;
  }
//  float qr[5];
  for(int i = 0; i < totalPoints; i++)
  {
    if(m_GrainIds[i] > 0)
    {
      q1[0] = m_Quats[i*5 + 0];
      q1[1] = m_Quats[i*5 + 1];
      q1[2] = m_Quats[i*5 + 2];
      q1[3] = m_Quats[i*5 + 3];
      q1[4] = m_Quats[i*5 + 4];

      OrientationMath::QuattoMat(q1, g1);
      //transpose the g matricies so when caxis is multiplied by it
      //it will give the sample direction that the caxis is along
      MatrixMath::transpose3x3(g1, g1t);
      MatrixMath::multiply3x3with3x1(g1t, caxis, c1);
      //normalize so that the magnitude is 1
      MatrixMath::normalize3x1(c1);
    if(c1[2] < 0) c1[0] = -c1[0], c1[1] = -c1[1], c1[2] = -c1[2];

    counter[m_GrainIds[i]]++;
    m_AvgCAxes[3*m_GrainIds[i]] = m_AvgCAxes[3*m_GrainIds[i]] + c1[0];
    m_AvgCAxes[3*m_GrainIds[i]+1] = m_AvgCAxes[3*m_GrainIds[i]+1] + c1[1];
    m_AvgCAxes[3*m_GrainIds[i]+2] = m_AvgCAxes[3*m_GrainIds[i]+2] + c1[2];
    }
  }
  for (size_t i = 1; i < numgrains; i++)
  {
    if(counter[i] == 0)
    {
    m_AvgCAxes[3*i] = 0;
    m_AvgCAxes[3*i+1] = 0;
    m_AvgCAxes[3*i+2] = 1;
    }
    else
    {
    m_AvgCAxes[3*i] = m_AvgCAxes[3*i]/counter[i];
    m_AvgCAxes[3*i+1] = m_AvgCAxes[3*i+1]/counter[i];
    m_AvgCAxes[3*i+2] = m_AvgCAxes[3*i+2]/counter[i];
    }
  }

 notifyStatusMessage("Completed");
}


