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

#include "FindGrainReferenceCAxisMisorientations.h"

#include <limits>


#include "DREAM3DLib/Math/DREAM3DMath.h"
#include "DREAM3DLib/Math/MatrixMath.h"
#include "DREAM3DLib/Common/Constants.h"

#include "DREAM3DLib/GenericFilters/FindCellQuats.h"
#include "DREAM3DLib/StatisticsFilters/FindAvgCAxes.h"
#include "DREAM3DLib/StatisticsFilters/FindEuclideanDistMap.h"



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindGrainReferenceCAxisMisorientations::FindGrainReferenceCAxisMisorientations() :
  AbstractFilter(),
  m_GrainIdsArrayName(DREAM3D::CellData::GrainIds),
  m_CellPhasesArrayName(DREAM3D::CellData::Phases),
  m_QuatsArrayName(DREAM3D::CellData::Quats),
  m_GrainReferenceCAxisMisorientationsArrayName(DREAM3D::CellData::GrainReferenceCAxisMisorientations),
  m_AvgCAxesArrayName(DREAM3D::FieldData::AvgCAxes),
  m_GrainAvgCAxisMisorientationsArrayName(DREAM3D::FieldData::GrainAvgCAxisMisorientations),
  m_GrainStdevCAxisMisorientationsArrayName(DREAM3D::FieldData::GrainStdevCAxisMisorientations),
  m_GrainIds(NULL),
  m_CellPhases(NULL),
  m_GrainReferenceCAxisMisorientations(NULL),
  m_AvgCAxes(NULL),
  m_GrainAvgCAxisMisorientations(NULL),
  m_GrainStdevCAxisMisorientations(NULL),
  m_Quats(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindGrainReferenceCAxisMisorientations::~FindGrainReferenceCAxisMisorientations()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindGrainReferenceCAxisMisorientations::setupFilterParameters()
{
}

// -----------------------------------------------------------------------------
void FindGrainReferenceCAxisMisorientations::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  /* Code to read the values goes between these statements */
/* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE BEGIN*/
/* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE END*/
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int FindGrainReferenceCAxisMisorientations::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindGrainReferenceCAxisMisorientations::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;
  VolumeDataContainer* m = getVolumeDataContainer();

  GET_PREREQ_DATA(m, DREAM3D, CellData, GrainIds, -300, int32_t, Int32ArrayType, voxels, 1)
  GET_PREREQ_DATA(m, DREAM3D, CellData, CellPhases, -300, int32_t, Int32ArrayType,  voxels, 1)
  GET_PREREQ_DATA(m, DREAM3D, CellData, Quats, -303, float, FloatArrayType, voxels, 4)

  CREATE_NON_PREREQ_DATA(m, DREAM3D, CellData, GrainReferenceCAxisMisorientations, float, FloatArrayType, 0, voxels, 1)

  GET_PREREQ_DATA(m, DREAM3D, FieldData, AvgCAxes, -303, float, FloatArrayType, fields, 3)

  CREATE_NON_PREREQ_DATA(m, DREAM3D, FieldData, GrainAvgCAxisMisorientations, float, FloatArrayType, 0, fields, 1)
  CREATE_NON_PREREQ_DATA(m, DREAM3D, FieldData, GrainStdevCAxisMisorientations, float, FloatArrayType, 0, fields, 1)
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindGrainReferenceCAxisMisorientations::preflight()
{
  dataCheck(true, 1,1,1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindGrainReferenceCAxisMisorientations::execute()
{
  setErrorCondition(0);
  std::stringstream ss;
  VolumeDataContainer* m = getVolumeDataContainer();
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

  size_t numFields = m->getNumFieldTuples();
  int avgMisoComps = 3;
  FloatArrayType::Pointer avgmisoPtr = FloatArrayType::CreateArray(numFields, avgMisoComps, "AvgMiso_Temp");
  avgmisoPtr->initializeWithZeros();
  float* avgmiso = avgmisoPtr->GetPointer(0);

  QuatF q1;
  QuatF* quats = reinterpret_cast<QuatF*>(m_Quats);
  typedef DataArray<unsigned int> XTalType;

  float w;
  size_t udims[3] = {0,0,0};
  m->getDimensions(udims);
#if (CMP_SIZEOF_SIZE_T == 4)
  typedef uint32_t DimType;
  uint32_t maxUInt32 = std::numeric_limits<uint32_t>::max();
  // We have more points than can be allocated on a 32 bit machine. Assert Now.
  if(totalPoints > maxUInt32)
  {
    setErrorCondition(-666);
    ss.str("");
    ss << "The volume is too large for a 32 bit machine. Try reducing the input volume size. Total Voxels: " << totalPoints;
    notifyErrorMessage(ss.str(), getErrorCondition());
    return;
  }
#else
  typedef int64_t DimType;
#endif

  DimType xPoints = static_cast<DimType>(udims[0]);
  DimType yPoints = static_cast<DimType>(udims[1]);
  DimType zPoints = static_cast<DimType>(udims[2]);

  DimType point;

  float g1[3][3];
  float g1t[3][3];
  float caxis[3] = {0,0,1};
  float c1[3];
  float AvgCAxis[3];
  size_t index = 0;

  for (DimType col = 0; col < xPoints; col++)
  {
    for (DimType row = 0; row < yPoints; row++)
    {
      for (DimType plane = 0; plane < zPoints; plane++)
      {
        point = (plane * xPoints * yPoints) + (row * xPoints) + col;
        if (m_GrainIds[point] > 0 && m_CellPhases[point] > 0)
        {
          QuaternionMathF::Copy(quats[point], q1);
          OrientationMath::QuattoMat(q1, g1);
          //transpose the g matricies so when caxis is multiplied by it
          //it will give the sample direction that the caxis is along
          MatrixMath::Transpose3x3(g1, g1t);
          MatrixMath::Multiply3x3with3x1(g1t, caxis, c1);
          //normalize so that the magnitude is 1
          MatrixMath::Normalize3x1(c1);

          AvgCAxis[0] = m_AvgCAxes[3*m_GrainIds[point]];
          AvgCAxis[1] = m_AvgCAxes[3*m_GrainIds[point]+1];
          AvgCAxis[2] = m_AvgCAxes[3*m_GrainIds[point]+2];
          //normalize so that the magnitude is 1
          MatrixMath::Normalize3x1(AvgCAxis);
          w = MatrixMath::CosThetaBetweenVectors(c1,AvgCAxis);
          DREAM3DMath::boundF(w,-1,1);
          w = acosf(w);
          w = w *DREAM3D::Constants::k_180OverPi;
          if(w > 90.0) w = 180.0-w;

          m_GrainReferenceCAxisMisorientations[point] = w;
          index = m_GrainIds[point] * avgMisoComps;
          avgmiso[index]++;
          avgmiso[index+1] += w;
        }
        if (m_GrainIds[point] == 0 || m_CellPhases[point] == 0)
        {
          m_GrainReferenceCAxisMisorientations[point] = 0;
        }
      }
    }
  }

  for (size_t i = 1; i < numFields; i++)
  {
    if (i%1000 == 0)
    {
      ss.str("");
      ss << "Working On Grain " << i << " of " << totalFields;
      notifyStatusMessage(ss.str());
    }
    index = i * avgMisoComps;
    m_GrainAvgCAxisMisorientations[i] = avgmiso[index+1] / avgmiso[index];
    if(avgmiso[index] == 0) { m_GrainAvgCAxisMisorientations[i] = 0.0; }
  }

    size_t gNum;
  for (size_t j = 0; j < totalPoints; j++)
  {
    gNum = m_GrainIds[j];
    avgmiso[(gNum*avgMisoComps)+2] += ((m_GrainReferenceCAxisMisorientations[j] - m_GrainAvgCAxisMisorientations[gNum]) * (m_GrainReferenceCAxisMisorientations[j] - m_GrainAvgCAxisMisorientations[gNum]));
  }

  for (size_t i = 1; i < numFields; i++)
  {
    index = i * avgMisoComps;
    m_GrainStdevCAxisMisorientations[i] = sqrtf((1 / avgmiso[index]) * avgmiso[index+2]);
  }

  notifyStatusMessage("Completed");
}
