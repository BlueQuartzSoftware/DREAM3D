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

#include "FindKernelAvgMisorientations.h"

#include "DREAM3DLib/Math/DREAM3DMath.h"
#include "DREAM3DLib/Common/Constants.h"

#include "DREAM3DLib/GenericFilters/FindCellQuats.h"



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindKernelAvgMisorientations::FindKernelAvgMisorientations() :
AbstractFilter(),
m_GrainIdsArrayName(DREAM3D::CellData::GrainIds),
m_CellPhasesArrayName(DREAM3D::CellData::Phases),
m_QuatsArrayName(DREAM3D::CellData::Quats),
m_KernelAverageMisorientationsArrayName(DREAM3D::CellData::KernelAverageMisorientations),
m_CrystalStructuresArrayName(DREAM3D::EnsembleData::CrystalStructures),
m_GrainIds(NULL),
m_CellPhases(NULL),
m_KernelAverageMisorientations(NULL),
m_Quats(NULL),
m_CrystalStructures(NULL)
{
  m_OrientationOps = OrientationOps::getOrientationOpsVector();

  m_KernelSize.x = 1;
  m_KernelSize.y = 1;
  m_KernelSize.z = 1;

  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindKernelAvgMisorientations::~FindKernelAvgMisorientations()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindKernelAvgMisorientations::setupFilterParameters()
{
  FilterParameterVector parameters;
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Kernel Radius");
    option->setPropertyName("KernelSize");
    option->setWidgetType(FilterParameter::IntVec3Widget);
    option->setValueType("IntVec3Widget_t");
    option->setUnits("Pixels");
    parameters.push_back(option);
  }

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
void FindKernelAvgMisorientations::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  /* Code to read the values goes between these statements */
/* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE BEGIN*/
  setKernelSize( reader->readValue("KernelSize", getKernelSize() ) );
/* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE END*/
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int FindKernelAvgMisorientations::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("KernelSize", getKernelSize() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindKernelAvgMisorientations::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;
  VolumeDataContainer* m = getVolumeDataContainer();

  GET_PREREQ_DATA(m, DREAM3D, CellData, GrainIds, -300, int32_t, Int32ArrayType, voxels, 1)
  GET_PREREQ_DATA(m, DREAM3D, CellData, CellPhases, -300, int32_t, Int32ArrayType,  voxels, 1)

  GET_PREREQ_DATA(m, DREAM3D, CellData, Quats, -303, float, FloatArrayType, voxels, 4)

  CREATE_NON_PREREQ_DATA(m, DREAM3D, CellData, KernelAverageMisorientations, float, FloatArrayType, 0, voxels, 1)

  typedef DataArray<unsigned int> XTalStructArrayType;
  GET_PREREQ_DATA(m, DREAM3D, EnsembleData, CrystalStructures, -305, unsigned int, XTalStructArrayType, ensembles, 1)
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindKernelAvgMisorientations::preflight()
{
  dataCheck(true, 1,1,1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindKernelAvgMisorientations::execute()
{
  setErrorCondition(0);

  VolumeDataContainer* m = getVolumeDataContainer();
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The DataContainer Object was NULL", -999);
    return;
  }

  //int64_t totalPoints = m->getTotalPoints();

  dataCheck(false, m->getTotalPoints(), m->getNumFieldTuples(), m->getNumEnsembleTuples());
  if (getErrorCondition() < 0)
  {
    return;
  }

  QuatF q1;
  QuatF q2;
  QuatF* quats = reinterpret_cast<QuatF*>(m_Quats);

  int numVoxel; // number of voxels in the grain...
  int good = 0;

  float w, totalmisorientation;
  float n1, n2, n3;
  unsigned int phase1 = Ebsd::CrystalStructure::UnknownCrystalStructure;
  unsigned int phase2 = Ebsd::CrystalStructure::UnknownCrystalStructure;
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
        if (m_GrainIds[point] > 0 && m_CellPhases[point] > 0)
        {
          totalmisorientation = 0.0;
          numVoxel = 0;
          QuaternionMathF::Copy(quats[point], q1);
          phase1 = m_CrystalStructures[m_CellPhases[point]];
          for (int j = -m_KernelSize.z; j < m_KernelSize.z + 1; j++)
          {
            jStride = j * xPoints * yPoints;
            for (int k = -m_KernelSize.y; k < m_KernelSize.y + 1; k++)
            {
              kStride = k * xPoints;
              for (int l = -m_KernelSize.x; l < m_KernelSize.z + 1; l++)
              {
                good = 1;
                neighbor = point + (jStride) + (kStride) + (l);
                if(plane + j < 0) good = 0;
                else if(plane + j > zPoints - 1) good = 0;
                else if(row + k < 0) good = 0;
                else if(row + k > yPoints - 1) good = 0;
                else if(col + l < 0) good = 0;
                else if(col + l > xPoints - 1) good = 0;
                if(good == 1 && m_GrainIds[point] == m_GrainIds[neighbor])
                {
                  w = 10000.0;
                  QuaternionMathF::Copy(quats[neighbor], q2);
                  phase2 = m_CrystalStructures[m_CellPhases[neighbor]];
                  w = m_OrientationOps[phase1]->getMisoQuat( q1, q2, n1, n2, n3);
                  w = w *(180.0f/DREAM3D::Constants::k_Pi);
                  totalmisorientation = totalmisorientation + w;
                  numVoxel++;
                }
              }
            }
          }
          m_KernelAverageMisorientations[point] = totalmisorientation / (float)numVoxel;
          if(numVoxel == 0)
          {
            m_KernelAverageMisorientations[point] = 0;
          }
        }
        if (m_GrainIds[point] == 0 || m_CellPhases[point] == 0)
        {
          m_KernelAverageMisorientations[point] = 0;
        }
      }
    }
  }

 notifyStatusMessage("FindKernelAvgMisorientations Completed");
}
