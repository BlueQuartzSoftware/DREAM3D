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

#include "FindSurfaceVoxelFractions.h"

#include "DREAM3DLib/Common/DREAM3DMath.h"
#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/GenericFilters/FindGrainPhases.h"

const static float m_pi = static_cast<float>(M_PI);

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindSurfaceVoxelFractions::FindSurfaceVoxelFractions() :
AbstractFilter(),
m_GrainIdsArrayName(DREAM3D::CellData::GrainIds),
m_SurfaceVoxelsArrayName(DREAM3D::CellData::SurfaceVoxels),
m_SurfaceVoxelFractionsArrayName(DREAM3D::FieldData::SurfaceVoxelFractions),
m_GrainIds(NULL),
m_SurfaceVoxels(NULL),
m_SurfaceVoxelFractions(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindSurfaceVoxelFractions::~FindSurfaceVoxelFractions()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSurfaceVoxelFractions::setupFilterParameters()
{
}
// -----------------------------------------------------------------------------
void FindSurfaceVoxelFractions::readFilterParameters(AbstractFilterParametersReader* reader)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSurfaceVoxelFractions::writeFilterParameters(AbstractFilterParametersWriter* writer)

{
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSurfaceVoxelFractions::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;
  VoxelDataContainer* m = getVoxelDataContainer();
  //int err = 0;

  GET_PREREQ_DATA(m, DREAM3D, CellData, GrainIds, ss, -300, int32_t, Int32ArrayType, voxels, 1)
  GET_PREREQ_DATA(m, DREAM3D, CellData, SurfaceVoxels, ss, -300, int8_t, Int32ArrayType, voxels, 1)

  CREATE_NON_PREREQ_DATA(m, DREAM3D, FieldData, SurfaceVoxelFractions, ss, float, FloatArrayType, 0, fields, 1)

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSurfaceVoxelFractions::preflight()
{
  dataCheck(true, 1, 1, 1);
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSurfaceVoxelFractions::execute()
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

  find_surface_voxel_fractions();

  notifyStatusMessage("FindSurfaceVoxelFractions Completed");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSurfaceVoxelFractions::find_surface_voxel_fractions()
{
  VoxelDataContainer* m = getVoxelDataContainer();
  int64_t totalPoints = m->getTotalPoints();

  size_t numgrains = m->getNumFieldTuples();

  DataArray<float>::Pointer m_SurfVoxCounts = DataArray<float>::CreateArray(numgrains, "SurfVoxCounts");
  float* surfvoxcounts = m_SurfVoxCounts->GetPointer(0);
  DataArray<float>::Pointer m_VoxCounts = DataArray<float>::CreateArray(numgrains, "VoxCounts");
  float* voxcounts = m_VoxCounts->GetPointer(0);

  // Initialize every element to 0.0
  for (size_t i = 0; i < numgrains * 1; i++)
  {
    surfvoxcounts[i] = 0.0f;
    voxcounts[i] = 0.0f;
  }
  for (int j = 0; j < totalPoints; j++)
  {
    int gnum = m_GrainIds[j];
    voxcounts[gnum]++;
    if(m_SurfaceVoxels[j] > 0) surfvoxcounts[gnum]++;
  }
  for (size_t i = 1; i < numgrains; i++)
  {
    m_SurfaceVoxelFractions[i] = surfvoxcounts[i]/voxcounts[i];
  }
}
