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

#include "FindLocalMisorientationGradients.h"

#include "DREAM3DLib/Common/DREAM3DMath.h"
#include "DREAM3DLib/Common/Constants.h"

const static float m_pi = M_PI;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindLocalMisorientationGradients::FindLocalMisorientationGradients()
{
  m_HexOps = HexagonalOps::New();
  m_OrientationOps.push_back(dynamic_cast<OrientationMath*> (m_HexOps.get()));

  m_CubicOps = CubicOps::New();
  m_OrientationOps.push_back(dynamic_cast<OrientationMath*> (m_CubicOps.get()));

  m_OrthoOps = OrthoRhombicOps::New();
  m_OrientationOps.push_back(dynamic_cast<OrientationMath*> (m_OrthoOps.get()));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindLocalMisorientationGradients::~FindLocalMisorientationGradients()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindLocalMisorientationGradients::execute()
{
  setErrorCondition(0);

  find_localmisorientationgradients();

  notify("FindLocalMisorientationGradients Completed", 0, Observable::UpdateProgressMessage);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindLocalMisorientationGradients::find_localmisorientationgradients()
{
  DataContainer* m = getDataContainer();
  m->grainmisorientations = m->m_GrainMisorientations->WritePointer(0, m->totalpoints);
  m->misorientationgradients = m->m_MisorientationGradients->WritePointer(0, m->totalpoints);
  m->kernelmisorientations = m->m_KernelMisorientations->WritePointer(0, m->totalpoints);
  std::vector<float> gamVec(m->totalpoints);
  float* gam = &(gamVec.front());

  float** avgmiso = new float *[m->m_Grains.size()];
  for (size_t i = 1; i < m->m_Grains.size(); i++)
  {
    avgmiso[i] = new float[2];
    for (int j = 0; j < 2; j++)
    {
      avgmiso[i][j] = 0.0;
    }
  }
  for (int i = 0; i < m->totalpoints; ++i)
  {
    gam[i] = 0.0;
  }

  float q1[5];
  float q2[5];
  int numVoxel; // number of voxels in the grain...
  int numchecks; // number of voxels in the grain...
  int good = 0;
  int neighbor;
  int point;
  float w, totalmisorientation;
  float n1, n2, n3;
  Ebsd::CrystalStructure phase1 = Ebsd::UnknownCrystalStructure;
  Ebsd::CrystalStructure phase2 = Ebsd::UnknownCrystalStructure;

  int steps = 1;
  int jStride;
  int kStride;

  for (int col = 0; col < m->xpoints; col++)
  {
    for (int row = 0; row < m->ypoints; row++)
    {
      for (int plane = 0; plane < m->zpoints; plane++)
      {
        point = (plane * m->xpoints * m->ypoints) + (row * m->xpoints) + col;
        if (m->grain_indicies[point] > 0 && m->phases[point] > 0)
        {
          totalmisorientation = 0.0;
          numVoxel = 0;
          q1[1] = m->quats[point*5 + 1];
          q1[2] = m->quats[point*5 + 2];
          q1[3] = m->quats[point*5 + 3];
          q1[4] = m->quats[point*5 + 4];
          phase1 = m->crystruct[m->phases[point]];
          for (int j = -steps; j < steps + 1; j++)
          {
            jStride = j * m->xpoints * m->ypoints;
            for (int k = -steps; k < steps + 1; k++)
            {
              kStride = k * m->xpoints;
              for (int l = -steps; l < steps + 1; l++)
              {
                good = 1;
                neighbor = point + (jStride) + (kStride) + (l);
                if (plane + j < 0) good = 0;
                else if (plane + j > m->zpoints - 1) good = 0;
                else if (row + k < 0) good = 0;
                else if (row + k > m->ypoints - 1) good = 0;
                else if (col + l < 0) good = 0;
                else if (col + l > m->xpoints - 1) good = 0;
                if (good == 1)
                {
                  w = 10000.0;
                  q2[1] = m->quats[neighbor*5 + 1];
                  q2[2] = m->quats[neighbor*5 + 2];
                  q2[3] = m->quats[neighbor*5 + 3];
                  q2[4] = m->quats[neighbor*5 + 4];
                  phase2 = m->crystruct[m->phases[neighbor]];
                  if (phase1 == phase2) w = m_OrientationOps[phase1]->getMisoQuat( q1, q2, n1, n2, n3);
                  if (w < 5.0)
                  {
                    totalmisorientation = totalmisorientation + w;
                    numVoxel++;
                  }
                }
              }
            }
          }
          if(numVoxel == 0)
          {
            m->kernelmisorientations[point] = 0;
          }
          else if(neighbor < m->m_KernelMisorientations->GetNumberOfTuples())
          {
            m->kernelmisorientations[point] = totalmisorientation / (float)numVoxel;
          }
          else
          {
            m->kernelmisorientations[point] = 0;
          }

          q2[0] = 1;
          q2[1] = m->m_Grains[m->grain_indicies[point]]->avg_quat[1] / m->m_Grains[m->grain_indicies[point]]->avg_quat[0];
          q2[2] = m->m_Grains[m->grain_indicies[point]]->avg_quat[2] / m->m_Grains[m->grain_indicies[point]]->avg_quat[0];
          q2[3] = m->m_Grains[m->grain_indicies[point]]->avg_quat[3] / m->m_Grains[m->grain_indicies[point]]->avg_quat[0];
          q2[4] = m->m_Grains[m->grain_indicies[point]]->avg_quat[4] / m->m_Grains[m->grain_indicies[point]]->avg_quat[0];
          w = m_OrientationOps[phase1]->getMisoQuat( q1, q2, n1, n2, n3);
          m->grainmisorientations[point] = w;
          gam[point] = w;
          avgmiso[m->grain_indicies[point]][0]++;
          avgmiso[m->grain_indicies[point]][1] = avgmiso[m->grain_indicies[point]][1] + w;
        }
        if (m->grain_indicies[point] == 0 || m->phases[point] == 0)
        {
          m->kernelmisorientations[point] = 0;
          m->grainmisorientations[point] = 0;
          gam[point] = 0;
        }
      }
    }
  }

  size_t grainsSize = m->m_Grains.size();
  for (size_t i = 1; i < grainsSize; i++)
  {
    m->m_Grains[i]->averagemisorientation = avgmiso[i][1] / avgmiso[i][0];
	if(avgmiso[i][0] == 0) m->m_Grains[i]->averagemisorientation = 0.0;
  }

  steps = 1;
  for (int col = 0; col < m->xpoints; col++)
  {
    for (int row = 0; row < m->ypoints; row++)
    {
      for (int plane = 0; plane < m->zpoints; plane++)
      {
        point = (plane * m->xpoints * m->ypoints) + (row * m->xpoints) + col;
        if (m->grain_indicies[point] > 0 && m->phases[point] > 0)
        {
          totalmisorientation = 0.0;
          numchecks = 0;
          for (int j = -steps; j < steps + 1; j++)
          {
            jStride = j * m->xpoints * m->ypoints;
            for (int k = -steps; k < steps + 1; k++)
            {
              kStride = k * m->xpoints;
              for (int l = -steps; l < steps + 1; l++)
              {
                good = 1;
                neighbor = point + (jStride) + (kStride) + (l);
                if (plane + j < 0) good = 0;
                if (plane + j > m->zpoints - 1) good = 0;
                if (row + k < 0) good = 0;
                if (row + k > m->ypoints - 1) good = 0;
                if (col + l < 0) good = 0;
                if (col + l > m->xpoints - 1) good = 0;
                if (good == 1 && m->grain_indicies[point] == m->grain_indicies[neighbor])
                {
                  numchecks++;
                  totalmisorientation = totalmisorientation + fabs(gam[point] - gam[neighbor]);
                }
              }
            }
          }
          m->misorientationgradients[point] = totalmisorientation / (float)numchecks;
        }
        if (m->grain_indicies[point] == 0 || m->phases[point] == 0)
        {
          m->misorientationgradients[point] = 0;
        }
      }
    }
  }

  // Clean up all the heap allocated memory
  for (size_t i = 1; i < m->m_Grains.size(); i++)
  {
    delete[] avgmiso[i];
  }
  delete avgmiso;
}

