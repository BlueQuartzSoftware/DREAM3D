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

#include "PerPhaseMinSize.h"


#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Common/DREAM3DMath.h"
#include "DREAM3DLib/Common/DREAM3DRandom.h"

#include "DREAM3DLib/GenericFilters/FindGrainPhases.h"
#include "DREAM3DLib/GenericFilters/RenumberGrains.h"

const static float m_pi = static_cast<float>(M_PI);

#define NEW_SHARED_ARRAY(var, m_msgType, size)\
  boost::shared_array<m_msgType> var##Array(new m_msgType[size]);\
  m_msgType* var = var##Array.get();

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PerPhaseMinSize::PerPhaseMinSize() :
  MinSize(),
  m_GrainIdsArrayName(DREAM3D::CellData::GrainIds),
  m_FieldPhasesArrayName(DREAM3D::FieldData::Phases),
  m_ActiveArrayName(DREAM3D::FieldData::Active),
  m_MinAllowedGrainSize(1),
  m_GrainIds(NULL),
  m_FieldPhases(NULL),
  m_Active(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PerPhaseMinSize::~PerPhaseMinSize()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PerPhaseMinSize::setupFilterParameters()
{
  std::vector<FilterParameter::Pointer> parameters;
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Minimum Allowed Grain Size");
    option->setPropertyName("MinAllowedGrainSize");
    option->setWidgetType(FilterParameter::IntWidget);
    option->setValueType("int");
    option->setUnits("Pixels");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Phase Number to Run Min Size Filter on");
    option->setPropertyName("PhaseNumber");
    option->setWidgetType(FilterParameter::IntWidget);
    option->setValueType("int");
    parameters.push_back(option);
  }
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PerPhaseMinSize::readFilterParameters(AbstractFilterParametersReader* reader)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int PerPhaseMinSize::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("MinAllowedGrainSize", getMinAllowedGrainSize() );
  writer->writeValue("PhaseNumber", getPhaseNumber() );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PerPhaseMinSize::remove_smallgrains()
{
  VoxelDataContainer* m = getVoxelDataContainer();
  int64_t totalPoints = m->getTotalPoints();

  bool good = false;

  int gnum;

  int numgrains = m->getNumFieldTuples();

  std::vector<int> voxcounts;
  voxcounts.resize(numgrains,0);
  for (int64_t i = 0; i < totalPoints; i++)
  {
    gnum = m_GrainIds[i];
    if(gnum >= 0) voxcounts[gnum]++;
  }
  for (size_t i = 1; i <  static_cast<size_t>(numgrains); i++)
  {
	m_Active[i] = true;
    if(voxcounts[i] >= m_MinAllowedGrainSize || m_FieldPhases[i] != m_PhaseNumber) good = true;
  }
  if(good == false)
  {
    setErrorCondition(-1);
    notifyErrorMessage("The minimum size is larger than the largest Field.  All Fields would be removed.  The filter has quit.", -1);
    return;
  }
  for (int64_t i = 0; i < totalPoints; i++)
  {
    gnum = m_GrainIds[i];
    if(voxcounts[gnum] < m_MinAllowedGrainSize && m_FieldPhases[i] == m_PhaseNumber && gnum > 0)
	{
		m_GrainIds[i] = -1;
		m_Active[gnum] = false;
	}
  }
}
