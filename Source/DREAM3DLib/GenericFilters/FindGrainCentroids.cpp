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

#include "FindGrainCentroids.h"

#include <sstream>

#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/DataArrays/IDataArray.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindGrainCentroids::FindGrainCentroids() :
AbstractFilter(),
m_GrainIdsArrayName(DREAM3D::CellData::GrainIds),
m_CentroidsArrayName(DREAM3D::FieldData::Centroids),
m_GrainIds(NULL),
m_Centroids(NULL)
{
  graincenters = NULL;

  INIT_DataArray(m_GrainCenters,float);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindGrainCentroids::~FindGrainCentroids()
{
}
// -----------------------------------------------------------------------------
void FindGrainCentroids::readFilterParameters(AbstractFilterParametersReader* reader, int index)
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
int FindGrainCentroids::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindGrainCentroids::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{

  setErrorCondition(0);
  
  VolumeDataContainer* m = getVolumeDataContainer();

  GET_PREREQ_DATA(m, DREAM3D, CellData, GrainIds, -300, int32_t, Int32ArrayType, voxels, 1)

  CREATE_NON_PREREQ_DATA(m, DREAM3D, CellFieldData, Centroids, float, FloatArrayType, 0, fields, 3)

}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindGrainCentroids::preflight()
{
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindGrainCentroids::execute()
{
  setErrorCondition(0);
  VolumeDataContainer* m = getVolumeDataContainer();
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The DataContainer Object was NULL", -999);
    return;
  }
  setErrorCondition(0);

  int64_t totalPoints = m->getTotalPoints();
  size_t totalFields = m->getNumCellFieldTuples();
  size_t totalEnsembles = m->getNumCellEnsembleTuples();
  dataCheck(false, totalPoints, totalFields, totalEnsembles);
  if (getErrorCondition() < 0)
  {
    return;
  }

  find_centroids();


  notifyStatusMessage("Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindGrainCentroids::find_centroids()
{
  VolumeDataContainer* m = getVolumeDataContainer();
  float x, y, z;
  size_t numgrains = m->getNumCellFieldTuples();
  if (numgrains == 0) { return; }
  m_GrainCenters->SetNumberOfComponents(5);
  m_GrainCenters->Resize(numgrains);
  
  graincenters = m_GrainCenters->getPointer(0);

  int xPoints = static_cast<int>(m->getXPoints());
  int yPoints = static_cast<int>(m->getYPoints());
  int zPoints = static_cast<int>(m->getZPoints());

  float xRes = m->getXRes();
  float yRes = m->getYRes();
  float zRes = m->getZRes();

  // Initialize every element to 0.0
  for (size_t i = 0; i < numgrains * 5; i++)
  {
    graincenters[i] = 0.0f;
  }
  size_t zStride, yStride;
  for(size_t i=0;i<zPoints;i++)
  {
  zStride = i*xPoints*yPoints;
  for (size_t j=0;j<yPoints;j++)
  {
    yStride = j*xPoints;
    for(size_t k=0;k<xPoints;k++)
    {
      int gnum = m_GrainIds[zStride+yStride+k];
      graincenters[gnum * 5 + 0]++;
      x = float(k) * xRes;
      y = float(j) * yRes;
      z = float(i) * zRes;
      graincenters[gnum * 5 + 1] = graincenters[gnum * 5 + 1] + x;
      graincenters[gnum * 5 + 2] = graincenters[gnum * 5 + 2] + y;
      graincenters[gnum * 5 + 3] = graincenters[gnum * 5 + 3] + z;
    }
  }
  }
  for (size_t i = 1; i < numgrains; i++)
  {
  graincenters[i * 5 + 1] = graincenters[i * 5 + 1] / graincenters[i * 5 + 0];
  graincenters[i * 5 + 2] = graincenters[i * 5 + 2] / graincenters[i * 5 + 0];
  graincenters[i * 5 + 3] = graincenters[i * 5 + 3] / graincenters[i * 5 + 0];
  m_Centroids[3 * i] = graincenters[i * 5 + 1];
  m_Centroids[3 * i + 1] = graincenters[i * 5 + 2];
  m_Centroids[3 * i + 2] = graincenters[i * 5 + 3];
  }
}
