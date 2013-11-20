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

#include "FindSurfaceCells.h"

#include <sstream>

#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/DataArrays/IDataArray.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindSurfaceCells::FindSurfaceCells() :
  AbstractFilter(),
  m_DataContainerName(DREAM3D::HDF5::VolumeDataContainerName),
  m_FeatureIdsArrayName(DREAM3D::CellData::FeatureIds),
  m_SurfaceVoxelsArrayName(DREAM3D::CellData::SurfaceVoxels),
  m_FeatureIds(NULL),
  m_SurfaceVoxels(NULL)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindSurfaceCells::~FindSurfaceCells()
{
}
// -----------------------------------------------------------------------------
void FindSurfaceCells::readFilterParameters(AbstractFilterParametersReader* reader, int index)
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
int FindSurfaceCells::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSurfaceCells::dataCheck(bool preflight, size_t voxels, size_t features, size_t ensembles)
{

  setErrorCondition(0);

  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

  QVector<int> dims(1, 1);
  m_FeatureIds = m->getPrereqArray<int32_t, AbstractFilter>(this, m_CellAttributeMatrixName,  m_FeatureIdsArrayName, -300, voxels, dims);
  CREATE_NON_PREREQ_DATA(m, DREAM3D, CellData, SurfaceVoxels, int8_t, Int8ArrayType, 0, voxels, dims)
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSurfaceCells::preflight()
{
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());
  if(NULL == m)
  {
    setErrorCondition(-999);
    addErrorMessage(getHumanLabel(), "The VolumeDataContainer Object with the specific name " + getDataContainerName() + " was not available.", getErrorCondition());
    return;
  }

  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSurfaceCells::execute()
{
  setErrorCondition(0);
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The DataContainer Object was NULL", -999);
    return;
  }
  setErrorCondition(0);

  int64_t totalPoints = m->getTotalPoints();
  size_t totalFeatures = m->getNumCellFeatureTuples();
  size_t totalEnsembles = m->getNumCellEnsembleTuples();
  dataCheck(false, totalPoints, totalFeatures, totalEnsembles);
  if (getErrorCondition() < 0)
  {
    return;
  }

  int xPoints = static_cast<int>(m->getXPoints());
  int yPoints = static_cast<int>(m->getYPoints());
  int zPoints = static_cast<int>(m->getZPoints());

  int neighpoints[6];
  neighpoints[0] = -xPoints * yPoints;
  neighpoints[1] = -xPoints;
  neighpoints[2] = -1;
  neighpoints[3] = 1;
  neighpoints[4] = xPoints;
  neighpoints[5] = xPoints * yPoints;

  //float column, row, plane;
  int feature;
  int onsurf = 0;
  int good = 0;
  int neighbor = 0;

  int zStride, yStride;
  for(int i = 0; i < zPoints; i++)
  {
    zStride = i * xPoints * yPoints;
    for (int j = 0; j < yPoints; j++)
    {
      yStride = j * xPoints;
      for(int k = 0; k < xPoints; k++)
      {
        onsurf = 0;
        feature = m_FeatureIds[zStride + yStride + k];
        if(feature > 0)
        {
          for (int l = 0; l < 6; l++)
          {
            good = 1;
            neighbor = static_cast<int>( zStride + yStride + k + neighpoints[l] );
            if(l == 0 && i == 0) { good = 0; }
            if(l == 5 && i == (zPoints - 1)) { good = 0; }
            if(l == 1 && j == 0) { good = 0; }
            if(l == 4 && j == (yPoints - 1)) { good = 0; }
            if(l == 2 && k == 0) { good = 0; }
            if(l == 3 && k == (xPoints - 1)) { good = 0; }
            if(good == 1 && m_FeatureIds[neighbor] != feature && m_FeatureIds[neighbor] > 0)
            {
              onsurf++;
            }
          }
        }
        m_SurfaceVoxels[zStride + yStride + k] = onsurf;
      }
    }
  }

  notifyStatusMessage("Complete");
}


