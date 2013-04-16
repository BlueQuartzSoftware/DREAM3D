/* ============================================================================
 * Copyright (c) 2012 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2012 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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

#include "AdjustVolumeOriginResolution.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AdjustVolumeOriginResolution::AdjustVolumeOriginResolution() :
  AbstractFilter()
{

  m_Resolution.x = 1.0f;
  m_Resolution.y = 1.0f;
  m_Resolution.z = 1.0f;
  m_Origin.x = 0.0f;
  m_Origin.y = 0.0f;
  m_Origin.z = 0.0f;
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AdjustVolumeOriginResolution::~AdjustVolumeOriginResolution()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AdjustVolumeOriginResolution::setupFilterParameters()
{
  std::vector<FilterParameter::Pointer> parameters;
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Resolution");
    option->setPropertyName("Resolution");
    option->setWidgetType(FilterParameter::FloatVec3Widget);
    option->setValueType("float");
    option->setUnits("Microns");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Origin");
    option->setPropertyName("Origin");
    option->setWidgetType(FilterParameter::FloatVec3Widget);
    option->setValueType("float");
    option->setUnits("Microns");
    parameters.push_back(option);
  }
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AdjustVolumeOriginResolution::writeFilterParameters(AbstractFilterParametersWriter* writer)
{
  writer->writeValue("Origin", getOrigin() );
  writer->writeValue("Resolution", getResolution() );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AdjustVolumeOriginResolution::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AdjustVolumeOriginResolution::preflight()
{
  /* Place code here that sanity checks input arrays and input values. Look at some
  * of the other DREAM3DLib/Filters/.cpp files for sample codes */
  dataCheck(true, 1, 1, 1);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AdjustVolumeOriginResolution::execute()
{
  VoxelDataContainer* m = getVoxelDataContainer();
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The DataContainer Object was NULL", -999);
    return;
  }
  setErrorCondition(0);
  std::stringstream ss;

  m->setOrigin(m_Origin.x, m_Origin.y, m_Origin.z);
  m->setResolution(m_Resolution.x, m_Resolution.y, m_Resolution.z);

  notifyStatusMessage("Complete");
}

