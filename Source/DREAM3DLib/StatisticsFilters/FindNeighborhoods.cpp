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

#include "FindNeighborhoods.h"

#include "DREAM3DLib/Math/DREAM3DMath.h"
#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/StatisticsFilters/FindSizes.h"
#include "DREAM3DLib/GenericFilters/FindGrainPhases.h"
#include "DREAM3DLib/GenericFilters/FindGrainCentroids.h"



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindNeighborhoods::FindNeighborhoods() :
AbstractFilter(),
m_CentroidsArrayName(DREAM3D::FieldData::Centroids),
m_EquivalentDiametersArrayName(DREAM3D::FieldData::EquivalentDiameters),
m_FieldPhasesArrayName(DREAM3D::FieldData::Phases),
m_NeighborhoodsArrayName(DREAM3D::FieldData::Neighborhoods),
m_FieldPhases(NULL),
m_Centroids(NULL),
m_EquivalentDiameters(NULL),
m_Neighborhoods(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindNeighborhoods::~FindNeighborhoods()
{
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindNeighborhoods::setupFilterParameters()
{

}
// -----------------------------------------------------------------------------
void FindNeighborhoods::readFilterParameters(AbstractFilterParametersReader* reader, int index)
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
int FindNeighborhoods::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindNeighborhoods::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;
  VoxelDataContainer* m = getVoxelDataContainer();

  GET_PREREQ_DATA(m, DREAM3D, FieldData, EquivalentDiameters, ss, -302, float, FloatArrayType, fields, 1)

  GET_PREREQ_DATA(m, DREAM3D, FieldData, FieldPhases, ss, -304, int32_t, Int32ArrayType, fields, 1)

  GET_PREREQ_DATA(m, DREAM3D, FieldData, Centroids, ss, -305, float, FloatArrayType, fields, 3)

  CREATE_NON_PREREQ_DATA(m, DREAM3D, FieldData, Neighborhoods, ss, int32_t, Int32ArrayType, 0, fields, 1)
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindNeighborhoods::preflight()
{
  dataCheck(true, 1, 1, 1);
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindNeighborhoods::execute()
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

  find_neighborhoods();
 notifyStatusMessage("FindNeighborhoods Completed");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindNeighborhoods::find_neighborhoods()
{
  VoxelDataContainer* m = getVoxelDataContainer();

  float x, y, z;
  float xn, yn, zn;
  float dx, dy, dz;
  size_t numgrains = m->getNumFieldTuples();

  for (size_t i = 1; i < numgrains; i++)
  {
    m_Neighborhoods[i] = 0;
  }
  for (size_t i = 1; i < numgrains; i++)
  {
      x = m_Centroids[3*i];
      y = m_Centroids[3*i+1];
      z = m_Centroids[3*i+2];
    if(m->getXPoints() == 1) x = 0;
    if(m->getYPoints() == 1) y = 0;
    if(m->getZPoints() == 1) z = 0;
      for (size_t j = i; j < numgrains; j++)
      {
      xn = m_Centroids[3*j];
      yn = m_Centroids[3*j+1];
      zn = m_Centroids[3*j+2];
      if(m->getXPoints() == 1) xn = 0;
      if(m->getYPoints() == 1) yn = 0;
      if(m->getZPoints() == 1) zn = 0;
        dx = fabs(x - xn);
        dy = fabs(y - yn);
        dz = fabs(z - zn);
        if (dx < m_EquivalentDiameters[i] && dy < m_EquivalentDiameters[i] && dz < m_EquivalentDiameters[i])
        {
            m_Neighborhoods[i]++;
        }
        if (dx < m_EquivalentDiameters[j] && dy < m_EquivalentDiameters[j] && dz < m_EquivalentDiameters[j])
        {
            m_Neighborhoods[j]++;
        }
      }
  }
}
