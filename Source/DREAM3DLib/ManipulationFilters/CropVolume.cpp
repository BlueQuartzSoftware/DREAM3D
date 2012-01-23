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
  if (getErrorCondition() < 0)
  {
    return;
  }

  size_t udims[3] =
   { 0, 0, 0 };
   m->getDimensions(udims);
 #if (CMP_SIZEOF_SIZE_T == 4)
   typedef int32_t DimType;
 #else
   typedef int64_t DimType;
 #endif
   DimType dims[3] =
   {
       static_cast<DimType>(udims[0]),
       static_cast<DimType>(udims[1]),
       static_cast<DimType>(udims[2]), };

   // updateProgressAndMessage(("Operating on Volume"), 50);
   if(dims[0] == (m_XMax-m_XMin)
       && dims[1] == (m_YMax-m_YMin)
       && dims[2] == (m_ZMax-m_ZMin))
   {
     return;
   }

   float m_XP = (m_XMax-m_XMin);
   float m_YP = (m_YMax-m_YMin);
   float m_ZP = (m_ZMax-m_ZMin);
   float m_XStart = m_XMin*m->getXRes();
   float m_YStart = m_YMin*m->getYRes();
   float m_ZStart = m_ZMin*m->getZRes();


  float x, y, z;
  int col, row, plane;
  int index;
  int index_old;
  std::list<std::string> voxelArrayNames = m->getVoxelArrayNameList();
  for (int i = 0; i < m_ZP; i++)
  {
    std::stringstream ss;
    ss << "Cropping Volume - " << ((float)i/m->getZPoints())*100 << " Percent Complete";
    notify(ss.str(), 0, Observable::UpdateProgressMessage);
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
        for(std::list<std::string>::iterator iter = voxelArrayNames.begin(); iter != voxelArrayNames.end(); ++iter)
        {
          std::string name = *iter;
          IDataArray::Pointer p = m->getVoxelData(*iter);
          p->CopyTuple(index_old, index);
        }
      }
    }
  }
  m->setDimensions(m_XP, m_YP, m_ZP);
  totalPoints = m_XP*m_YP*m_ZP;
  for(std::list<std::string>::iterator iter = voxelArrayNames.begin(); iter != voxelArrayNames.end(); ++iter)
  {
     std::string name = *iter;
     IDataArray::Pointer p = m->getVoxelData(*iter);
	 err = p->Resize(totalPoints);
  }
  notify("Cropping Volume Complete", 0, Observable::UpdateProgressValueAndMessage);
}
