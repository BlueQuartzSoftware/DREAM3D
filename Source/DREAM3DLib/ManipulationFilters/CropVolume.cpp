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

#include "CropVolume.h"

#include <map>


#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Common/DREAM3DMath.h"
#include "DREAM3DLib/Common/DREAM3DRandom.h"

#include "DREAM3DLib/HDF5/H5VoxelReader.h"
#include "DREAM3DLib/HDF5/H5VoxelWriter.h"





// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CropVolume::CropVolume() :
AbstractFilter(),
m_GrainIds(NULL),
m_EulerAngles(NULL),
m_PhasesC(NULL)
{


}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CropVolume::~CropVolume()
{
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CropVolume::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;
  DataContainer* m = getDataContainer();

  CREATE_NON_PREREQ_DATA(m, DREAM3D, VoxelData, GrainIds, ss, int32_t, Int32ArrayType, voxels, 1);

  GET_PREREQ_DATA_SUFFIX(m, DREAM3D, VoxelData, Phases, C, ss, -300, int32_t, Int32ArrayType,  voxels, 1);
  GET_PREREQ_DATA(m, DREAM3D, VoxelData, EulerAngles, ss, -300, float, FloatArrayType,  voxels, 3);

  setErrorMessage(ss.str());
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CropVolume::preflight()
{
  dataCheck(true, 1,1,1);
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CropVolume::execute()
{
  int err = 0;
  setErrorCondition(err);
  DataContainer* m = getDataContainer();
  if (NULL == m)
  {
    setErrorCondition(-1);
    std::stringstream ss;
    ss << getNameOfClass() << " DataContainer was NULL";
    setErrorMessage(ss.str());
    return;
  }
  setErrorCondition(0);

  int64_t totalPoints = m->totalPoints();
  dataCheck(false, totalPoints, m->getTotalFields(), m->crystruct.size());
  if (getErrorCondition() < 0)
  {
    return;
  }


  notify("Starting Crop Volume", 0, Observable::UpdateProgressValueAndMessage);
  float x, y, z;
  int col, row, plane;
  int index;
  int index_old;
  for (int i = 0; i < m_ZP; i++)
  {
    notify("Cropping Percent", ((float)i/m->getZPoints())*100, Observable::UpdateProgressValue);
    for (int j = 0; j < m_YP; j++)
    {
      for (int k = 0; k < m_XP; k++)
      {
        x = (k * m->getXRes()) + m_XStart;
        y = (j * m->getYRes()) + m_YStart;
        z = (i * m->getZRes()) + m_ZStart;
        col = int(x / m->getXRes());
        row = int(y / m->getYRes());
        plane = int(z / m->getZRes());
		    index_old = (plane * m->getXPoints() * m->getYPoints()) + (row * m->getXPoints()) + col;
        index = (i * m_XP * m_YP) + (j * m_XP) + k;
        m_GrainIds[index] = m_GrainIds[index_old];
        m_PhasesC[index] = m_PhasesC[index_old];
        m_EulerAngles[3*index] = m_EulerAngles[3*index_old];
        m_EulerAngles[3*index + 1] = m_EulerAngles[3*index_old + 1];
        m_EulerAngles[3*index + 2] = m_EulerAngles[3*index_old + 2];
      }
    }
  }
  m->setDimensions(m_XP, m_YP, m_ZP);
  totalPoints = m_XP*m_YP*m_ZP;
  err = m->getVoxelData(DREAM3D::VoxelData::GrainIds)->Resize(totalPoints);
  err = m->getVoxelData(DREAM3D::VoxelData::Phases)->Resize(totalPoints);
  err = m->getVoxelData(DREAM3D::VoxelData::EulerAngles)->Resize(3*totalPoints);
  notify("Crop Volume Complete", 100, Observable::UpdateProgressValueAndMessage);
}
