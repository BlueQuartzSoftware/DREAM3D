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
AbstractFilter()
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
void CropVolume::preflight()
{
  int err = 0;
  std::stringstream ss;
  DataContainer::Pointer m = DataContainer::New();
  IDataArray::Pointer d = m->getVoxelData(DREAM3D::VoxelData::GrainIds);
  if(d.get() == NULL)
  {
	  PFInt32ArrayType::Pointer p = PFInt32ArrayType::CreateArray(1);
	  m->addVoxelData(DREAM3D::VoxelData::GrainIds, p);
  }
  d = m->getVoxelData(DREAM3D::VoxelData::Phases);
  if(d.get() == NULL)
  {
	  ss << "Phases (Cells) Array Not Initialized At Beginning of CropVolume Filter" << std::endl;
	  err = -300;
  }
  d = m->getVoxelData(DREAM3D::VoxelData::EulerAngles);
  if(d.get() == NULL)
  {
	  ss << "EulerAngles (Cells) Array Not Initialized At Beginning of CropVolume Filter" << std::endl;
	  err = -300;
  }

  setErrorCondition(err);
  setErrorMessage(ss.str());
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

  int64_t totalPoints = m->totalPoints();
    int32_t* grain_indicies = m->getVoxelDataSizeCheck<int32_t, Int32ArrayType, AbstractFilter>(DREAM3D::VoxelData::GrainIds, totalPoints, this);
  if (NULL == grain_indicies) { return; }
    int32_t* phases = m->getVoxelDataSizeCheck<int32_t, Int32ArrayType, AbstractFilter>(DREAM3D::VoxelData::Phases, totalPoints, this);
  if (NULL == phases) { return; }
  float* eulerangles = m->getVoxelDataSizeCheck<float, FloatArrayType, AbstractFilter>(DREAM3D::VoxelData::EulerAngles, 3*totalPoints, this);
  if (NULL == eulerangles) { return; }

  setErrorCondition(0);
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
        grain_indicies[index] = grain_indicies[index_old];
        phases[index] = phases[index_old];
        eulerangles[3*index] = eulerangles[3*index_old];
        eulerangles[3*index + 1] = eulerangles[3*index_old + 1];
        eulerangles[3*index + 2] = eulerangles[3*index_old + 2];
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
