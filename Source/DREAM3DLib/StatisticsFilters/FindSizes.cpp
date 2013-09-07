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

#include "FindSizes.h"


#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Common/DREAM3DMath.h"
#include "DREAM3DLib/GenericFilters/FindGrainPhases.h"



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindSizes::FindSizes() :
AbstractFilter(),
m_GrainIdsArrayName(DREAM3D::CellData::GrainIds),
m_EquivalentDiametersArrayName(DREAM3D::FieldData::EquivalentDiameters),
m_NumCellsArrayName(DREAM3D::FieldData::NumCells),
m_VolumesArrayName(DREAM3D::FieldData::Volumes),
m_GrainIds(NULL),
m_Volumes(NULL),
m_EquivalentDiameters(NULL),
m_NumCells(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindSizes::~FindSizes()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSizes::setupFilterParameters()
{
}
// -----------------------------------------------------------------------------
void FindSizes::readFilterParameters(AbstractFilterParametersReader* reader, int index)
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
int FindSizes::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSizes::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  VolumeDataContainer* m = getVolumeDataContainer();

  //int err = 0;

  GET_PREREQ_DATA(m, DREAM3D, CellData, GrainIds, -300, int32_t, Int32ArrayType, voxels, 1)

  CREATE_NON_PREREQ_DATA(m, DREAM3D, FieldData, Volumes, float, FloatArrayType, 0, fields, 1)
  CREATE_NON_PREREQ_DATA(m, DREAM3D, FieldData, EquivalentDiameters, float,FloatArrayType, 0, fields, 1)
  CREATE_NON_PREREQ_DATA(m, DREAM3D, FieldData, NumCells, int32_t, Int32ArrayType, 0, fields, 1)
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSizes::preflight()
{
  dataCheck(true, 1, 1, 1);
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSizes::execute()
{
  VolumeDataContainer* m = getVolumeDataContainer();
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
  if(m->getXPoints() == 1 || m->getYPoints() == 1 || m->getZPoints() == 1) find_sizes2D();
 notifyStatusMessage("FindSizes Completed");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSizes::find_sizes()
{
  VolumeDataContainer* m = getVolumeDataContainer();
  int64_t totalPoints = m->getTotalPoints();

  float radcubed;
  float diameter;

  size_t numgrains = m->getNumFieldTuples();

  DataArray<double>::Pointer m_GrainCounts = DataArray<double>::CreateArray(numgrains, "GrainCounts");
  double* graincounts = m_GrainCounts->GetPointer(0);

  // Initialize every element to 0.0
  for (size_t i = 0; i < numgrains * 1; i++)
  {
    graincounts[i] = 0.0f;
  }

  for (int64_t j = 0; j < totalPoints; j++)
  {
    int gnum = m_GrainIds[j];
    graincounts[gnum]++;
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

void FindSizes::find_sizes2D()
{
  VolumeDataContainer* m = getVolumeDataContainer();
  int64_t totalPoints = m->getTotalPoints();

  float radsquared;
  float diameter;
  size_t numgrains = m->getNumFieldTuples();

  DataArray<float>::Pointer m_GrainCounts = DataArray<float>::CreateArray(numgrains, "GrainCounts");
  float* graincounts = m_GrainCounts->GetPointer(0);

  for (size_t i = 0; i < numgrains; i++)
  {
      graincounts[i] = 0.0f;
  }
  for (int64_t j = 0; j < totalPoints; j++)
  {
    int gnum = m_GrainIds[j];
    graincounts[gnum]++;
  }
  float res_scalar = 0;
  if(m->getXPoints() == 1) res_scalar = m->getYRes() * m->getZRes();
  else if(m->getYPoints() == 1) res_scalar = m->getXRes() * m->getZRes();
  else if(m->getZPoints() == 1) res_scalar = m->getXRes() * m->getYRes();
  for (size_t i = 1; i < numgrains; i++)
  {
    m_NumCells[i] = static_cast<int32_t>( graincounts[i] );
    m_Volumes[i] = (graincounts[i] * res_scalar);
    radsquared = m_Volumes[i] / DREAM3D::Constants::k_Pi;
    diameter = (2 * sqrt(radsquared));
    m_EquivalentDiameters[i] = diameter;
  }
}

