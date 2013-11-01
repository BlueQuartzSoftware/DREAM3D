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

#include "UpdateCellQuats.h"

#include <sstream>

#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/DataArrays/IDataArray.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
UpdateCellQuats::UpdateCellQuats() :
  AbstractFilter(),
  m_QuatsArrayName(DREAM3D::CellData::Quats),
  m_Quats(NULL)
{
  m_OrientationOps = OrientationOps::getOrientationOpsVector();

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
UpdateCellQuats::~UpdateCellQuats()
{
}

// -----------------------------------------------------------------------------
void UpdateCellQuats::readFilterParameters(AbstractFilterParametersReader* reader)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void UpdateCellQuats::writeFilterParameters(AbstractFilterParametersWriter* writer)

{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void UpdateCellQuats::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{

  setErrorCondition(0);
  std::stringstream ss;
  VoxelDataContainer* m = getVoxelDataContainer();

  GET_PREREQ_DATA(m, DREAM3D, CellData, Quats, ss, -301, float, FloatArrayType, voxels, 5)

  CREATE_NON_PREREQ_DATA(m, DREAM3D, CellData, Quats, ss, float, FloatArrayType, 0, voxels, 4)

}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void UpdateCellQuats::preflight()
{
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void UpdateCellQuats::execute()
{
  setErrorCondition(0);
  VoxelDataContainer* m = getVoxelDataContainer();
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The DataContainer Object was NULL", -999);
    return;
  }
  setErrorCondition(0);
  std::stringstream ss;

  int64_t totalPoints = m->getTotalPoints();

  //getting the 5 component quaternions from the data container and down-casting them
  IDataArray::Pointer Quats5 = m->getCellData(DREAM3D::CellData::Quats);
  if(Quats5->GetNumberOfComponents() != 5)
  {
    notifyErrorMessage("The Quats array did not contain 5 components", -999);
    return;
  }
  DataArray<float>* quats5 = DataArray<float>::SafePointerDownCast(Quats5.get());
  float* quats5ptr = quats5->GetPointer(0);
  //creating the 4 component quaternions in the data container
  CREATE_NON_PREREQ_DATA(m, DREAM3D, CellData, Quats, ss, float, FloatArrayType, 0, totalPoints, 4)

  if (getErrorCondition() < 0)
  {
    return;
  }

  //copying the 5 component quaternions into the new 4 component quaternions
  // This is a special case for dealing with this conversion. If you are dealing with
  // quaternions then DO NOT USE THIS CODE as an example. Use another filter instead.
  for (int i = 0; i < totalPoints; i++)
  {
    for(int j=0;j<4;j++)
    {
      m_Quats[4*i+j] = quats5ptr[5*i+(j+1)];
    }
  }

  notifyStatusMessage("Complete");
}


