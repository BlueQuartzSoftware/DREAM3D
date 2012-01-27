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

const static float m_pi = static_cast<float>(M_PI);

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindLocalMisorientationGradients::FindLocalMisorientationGradients() :
AbstractFilter(),
m_GrainIds(NULL),
m_PhasesC(NULL),
m_GrainMisorientations(NULL),
m_MisorientationGradients(NULL),
m_KernelAverageMisorientations(NULL),
m_AvgQuats(NULL),
m_GrainAvgMisorientations(NULL),
m_Quats(NULL)
{
  m_HexOps = HexagonalOps::New();
  m_OrientationOps.push_back(dynamic_cast<OrientationMath*>(m_HexOps.get()));

  m_CubicOps = CubicOps::New();
  m_OrientationOps.push_back(dynamic_cast<OrientationMath*>(m_CubicOps.get()));

  m_OrthoOps = OrthoRhombicOps::New();
  m_OrientationOps.push_back(dynamic_cast<OrientationMath*>(m_OrthoOps.get()));
  m_KernelSize = 1;
  setupFilterOptions();
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
void FindLocalMisorientationGradients::setupFilterOptions()
{
  std::vector<FilterOption::Pointer> options;
  {
    FilterOption::Pointer option = FilterOption::New();
    option->setHumanLabel("Kernel Size");
    option->setPropertyName("KernelSize");
    option->setWidgetType(FilterOption::IntWidget);
    option->setValueType("int");
    options.push_back(option);
  }
  setFilterOptions(options);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindLocalMisorientationGradients::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;
  DataContainer* m = getDataContainer();

  GET_PREREQ_DATA(m, DREAM3D, CellData, GrainIds, ss, -300, int32_t, Int32ArrayType, voxels, 1);
  GET_PREREQ_DATA_SUFFIX(m, DREAM3D, CellData, Phases, C, ss, -300, int32_t, Int32ArrayType,  voxels, 1);
  GET_PREREQ_DATA(m, DREAM3D, CellData, Quats, ss, -300, float, FloatArrayType, voxels, 5);
  CREATE_NON_PREREQ_DATA(m, DREAM3D, CellData, KernelAverageMisorientations, ss, float, FloatArrayType, voxels, 1);
  CREATE_NON_PREREQ_DATA(m, DREAM3D, CellData, GrainMisorientations, ss, float, FloatArrayType, voxels, 1);
  CREATE_NON_PREREQ_DATA(m, DREAM3D, CellData, MisorientationGradients, ss, float, FloatArrayType, voxels, 1);

  GET_PREREQ_DATA(m, DREAM3D, FieldData, AvgQuats, ss, -301, float, FloatArrayType, fields, 5);
  CREATE_NON_PREREQ_DATA(m, DREAM3D, FieldData, GrainAvgMisorientations, ss, float, FloatArrayType, fields, 3);


  setErrorMessage(ss.str());
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindLocalMisorientationGradients::preflight()
{
  dataCheck(true, 1,1,1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindLocalMisorientationGradients::execute()
{
  setErrorCondition(0);

  DataContainer* m = getDataContainer();
  if(NULL == m)
  {
    setErrorCondition(-1);
    std::stringstream ss;
    ss << getNameOfClass() << " DataContainer was NULL";
    setErrorMessage(ss.str());
    return;
  }

  int64_t totalPoints = m->totalPoints();

  dataCheck(false, m->totalPoints(), m->getTotalFields(), m->getNumEnsembleTuples());
  if (getErrorCondition() < 0)
  {
    return;
  }


  // We need to keep a reference to the wrapper DataArray class in addition to the raw pointer
  FloatArrayType* m_KernelMisorientations =
      FloatArrayType::SafeObjectDownCast<IDataArray*, FloatArrayType*>(m->getCellData(DREAM3D::CellData::KernelAverageMisorientations).get());

  std::vector<float> gamVec(totalPoints);
  float* gam = &(gamVec.front());

  float** avgmiso = new float *[m->getTotalFields()];
  for (size_t i = 1; i < m->getTotalFields(); i++)
  {
    avgmiso[i] = new float[2];
    for (int j = 0; j < 2; j++)
    {
      avgmiso[i][j] = 0.0;
    }
  }
  for (int i = 0; i < totalPoints; ++i)
  {
    gam[i] = 0.0;
  }

  float q1[5];
  float q2[5];
  int numVoxel; // number of voxels in the grain...
  int numchecks; // number of voxels in the grain...
  int good = 0;


  typedef DataArray<Ebsd::CrystalStructure> XTalType;
  XTalType* crystructPtr
      = XTalType::SafeObjectDownCast<IDataArray*, XTalType*>(m->getEnsembleData(DREAM3D::EnsembleData::CrystalStructure).get());
  Ebsd::CrystalStructure* crystruct = crystructPtr->GetPointer(0);
  size_t numXTals = crystructPtr->GetNumberOfTuples();

  float w, totalmisorientation;
  float n1, n2, n3;
  Ebsd::CrystalStructure phase1 = Ebsd::UnknownCrystalStructure;
  Ebsd::CrystalStructure phase2 = Ebsd::UnknownCrystalStructure;
  size_t udims[3] = {0,0,0};
  m->getDimensions(udims);
#if (CMP_SIZEOF_SIZE_T == 4)
  typedef int32_t DimType;
#else
  typedef int64_t DimType;
#endif
  DimType xPoints = static_cast<DimType>(udims[0]);
  DimType yPoints = static_cast<DimType>(udims[1]);
  DimType zPoints = static_cast<DimType>(udims[2]);
  DimType point;
  size_t neighbor = 0;
//  int m_KernelSize = 1;
  DimType jStride;
  DimType kStride;
  for (DimType col = 0; col < xPoints; col++)
  {
    for (DimType row = 0; row < yPoints; row++)
    {
      for (DimType plane = 0; plane < zPoints; plane++)
      {
        point = (plane * xPoints * yPoints) + (row * xPoints) + col;
        if (m_GrainIds[point] > 0 && m_PhasesC[point] > 0)
        {
          totalmisorientation = 0.0;
          numVoxel = 0;
          q1[1] = m_Quats[point*5 + 1];
          q1[2] = m_Quats[point*5 + 2];
          q1[3] = m_Quats[point*5 + 3];
          q1[4] = m_Quats[point*5 + 4];
          phase1 = crystruct[m_PhasesC[point]];
          for (int j = -m_KernelSize; j < m_KernelSize + 1; j++)
          {
            jStride = j * xPoints * yPoints;
            for (int k = -m_KernelSize; k < m_KernelSize + 1; k++)
            {
              kStride = k * xPoints;
              for (int l = -m_KernelSize; l < m_KernelSize + 1; l++)
              {
                good = 1;
                neighbor = point + (jStride) + (kStride) + (l);
                if(plane + j < 0) good = 0;
                else if(plane + j > zPoints - 1) good = 0;
                else if(row + k < 0) good = 0;
                else if(row + k > yPoints - 1) good = 0;
                else if(col + l < 0) good = 0;
                else if(col + l > xPoints - 1) good = 0;
                if(good == 1)
                {
                  w = 10000.0;
                  q2[1] = m_Quats[neighbor*5 + 1];
                  q2[2] = m_Quats[neighbor*5 + 2];
                  q2[3] = m_Quats[neighbor*5 + 3];
                  q2[4] = m_Quats[neighbor*5 + 4];
                  phase2 = crystruct[m_PhasesC[neighbor]];
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
            m_KernelAverageMisorientations[point] = 0;
          }
          else if(neighbor < m_KernelMisorientations->GetNumberOfTuples())
          {
            m_KernelAverageMisorientations[point] = totalmisorientation / (float)numVoxel;
          }
          else
          {
            m_KernelAverageMisorientations[point] = 0;
          }

          q2[0] = 1;
          q2[1] = m_AvgQuats[5*m_GrainIds[point]+1] / m_AvgQuats[5*m_GrainIds[point]];
          q2[2] = m_AvgQuats[5*m_GrainIds[point]+2] / m_AvgQuats[5*m_GrainIds[point]];
          q2[3] = m_AvgQuats[5*m_GrainIds[point]+3] / m_AvgQuats[5*m_GrainIds[point]];
          q2[4] = m_AvgQuats[5*m_GrainIds[point]+4] / m_AvgQuats[5*m_GrainIds[point]];
          w = m_OrientationOps[phase1]->getMisoQuat( q1, q2, n1, n2, n3);
          m_GrainMisorientations[point] = w;

          gam[point] = w;
          avgmiso[m_GrainIds[point]][0]++;
          avgmiso[m_GrainIds[point]][1] = avgmiso[m_GrainIds[point]][1] + w;
        }
        if (m_GrainIds[point] == 0 || m_PhasesC[point] == 0)
        {
          m_KernelAverageMisorientations[point] = 0;
          m_GrainMisorientations[point] = 0;
          gam[point] = 0;
        }
      }
    }
  }

  size_t grainsSize = m->getTotalFields();
  for (size_t i = 1; i < grainsSize; i++)
  {
    m_GrainAvgMisorientations[i] = avgmiso[i][1] / avgmiso[i][0];
	  if(avgmiso[i][0] == 0) m_GrainAvgMisorientations[i] = 0.0;
  }

//  m_KernelSize = 1;
  for (DimType col = 0; col < xPoints; col++)
  {
    for (DimType row = 0; row < yPoints; row++)
    {
      for (DimType plane = 0; plane < zPoints; plane++)
      {
        point = (plane *xPoints * yPoints) + (row * xPoints) + col;
        if (m_GrainIds[point] > 0 && m_PhasesC[point] > 0)\
        {
          totalmisorientation = 0.0f;
          numchecks = 0;
          for (int j = -m_KernelSize; j < m_KernelSize + 1; j++)
          {
            jStride = j * xPoints * yPoints;
            for (int k = -m_KernelSize; k < m_KernelSize + 1; k++)
            {
              kStride = k * xPoints;
              for (int l = -m_KernelSize; l < m_KernelSize + 1; l++)
              {
                good = 1;
                neighbor = point + (jStride) + (kStride) + (l);
                if (plane + j < 0) good = 0;
                if (plane + j > zPoints - 1) good = 0;
                if (row + k < 0) good = 0;
                if (row + k > yPoints - 1) good = 0;
                if (col + l < 0) good = 0;
                if (col + l > xPoints - 1) good = 0;
                if (good == 1 && m_GrainIds[point] == m_GrainIds[neighbor])
                {
                  numchecks++;
                  totalmisorientation = totalmisorientation + fabs(gam[point] - gam[neighbor]);
                }
              }
            }
          }
          m_MisorientationGradients[point] = totalmisorientation / (float)numchecks;
        }

        if (m_GrainIds[point] == 0 || m_PhasesC[point] == 0)
        {
          m_MisorientationGradients[point] = 0;
        }
      }
    }
  }

  // Clean up all the heap allocated memory
  for (size_t i = 1; i < m->getTotalFields(); i++)
  {
    delete[] avgmiso[i];
  }
  delete avgmiso;

  notify("FindLocalMisorientationGradients Completed", 0, Observable::UpdateProgressMessage);
}
