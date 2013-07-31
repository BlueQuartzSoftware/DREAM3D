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

#include "FindLargestCrossSections.h"

#include "DREAM3DLib/Common/DREAM3DMath.h"
#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/GenericFilters/FindGrainPhases.h"



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindLargestCrossSections::FindLargestCrossSections() :
AbstractFilter(),
m_GrainIdsArrayName(DREAM3D::CellData::GrainIds),
m_LargestCrossSectionsArrayName(DREAM3D::FieldData::LargestCrossSections),
m_GrainIds(NULL),
m_LargestCrossSections(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindLargestCrossSections::~FindLargestCrossSections()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindLargestCrossSections::setupFilterParameters()
{
}
// -----------------------------------------------------------------------------
void FindLargestCrossSections::readFilterParameters(AbstractFilterParametersReader* reader)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindLargestCrossSections::writeFilterParameters(AbstractFilterParametersWriter* writer)

{
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindLargestCrossSections::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;
  VoxelDataContainer* m = getVoxelDataContainer();
  //int err = 0;

  GET_PREREQ_DATA(m, DREAM3D, CellData, GrainIds, ss, -300, int32_t, Int32ArrayType, voxels, 1)

  CREATE_NON_PREREQ_DATA(m, DREAM3D, FieldData, LargestCrossSections, ss, float, FloatArrayType, 0, fields, 1)

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindLargestCrossSections::preflight()
{
  dataCheck(true, 1, 1, 1);
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindLargestCrossSections::execute()
{
  VoxelDataContainer* m = getVoxelDataContainer();
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The DataContainer Object was NULL", -999);
    return;
  }
  setErrorCondition(0);

  dataCheck(false, m->getTotalPoints(), m->getNumFieldTuples(), m->getNumEnsembleTuples());
  if (getErrorCondition() < 0)
  {
    return;
  }


  if(m->getXPoints() > 1 && m->getYPoints() > 1 && m->getZPoints() > 1) find_sizes();
  else
  {
    setErrorCondition(-999);
    notifyErrorMessage("The volume is not 3D and cannot be run through this filter", -999);
    return;
  }
 notifyStatusMessage("FindLargestCrossSections Completed");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindLargestCrossSections::find_sizes()
{
  VoxelDataContainer* m = getVoxelDataContainer();
  int64_t totalPoints = m->getTotalPoints();

  float radcubed;
  float diameter;

  size_t numgrains = m->getNumFieldTuples();

  DataArray<double>::Pointer m_GrainCounts = DataArray<double>::CreateArray(numgrains, "GrainCounts");
  double* graincounts = m_GrainCounts->GetPointer(0);

  int xpoints = m->getXPoints();
  int ypoints = m->getYPoints();
  int zpoints = m->getZPoints();
  for(int i = 0; i < zpoints; i++)
  {
    for (size_t i = 0; i < numgrains * 1; i++)
    {
      graincounts[i] = 0.0f;
    }
    for (int j = 0; j < xpoints; j++)
    {
      for(int k = 0; k < ypoints; k++)
      {
        int gnum = m_GrainIds[j];
        graincounts[gnum]++;
      }
    }
    float res_scalar = m->getXRes() * m->getYRes() * m->getZRes();
    float vol_term = static_cast<double>( (4.0/3.0)*DREAM3D::Constants::k_Pi );
    for (size_t i = 1; i < numgrains; i++)
    {
      m_NumCells[i] = static_cast<int32_t>( graincounts[i] );
      m_Volumes[i] = (graincounts[i] * res_scalar);
      radcubed = m_Volumes[i] / vol_term;
      diameter = 2.0f * powf(radcubed, 0.3333333333f);
      m_EquivalentDiameters[i] = diameter;
    }
  }
}


