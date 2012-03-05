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

#include "FindSchmids.h"

#include "DREAM3DLib/Common/DREAM3DMath.h"
#include "DREAM3DLib/Common/Constants.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindSchmids::FindSchmids() :
AbstractFilter(),
m_XLoading(1.0f),
m_YLoading(1.0f),
m_ZLoading(1.0f),
m_Schmids(NULL),
m_AvgQuats(NULL),
m_SlipSystems(NULL)
{
  m_HexOps = HexagonalOps::New();
  m_OrientationOps.push_back(dynamic_cast<OrientationMath*> (m_HexOps.get()));

  m_CubicOps = CubicOps::New();
  m_OrientationOps.push_back(dynamic_cast<OrientationMath*> (m_CubicOps.get()));

  m_OrthoOps = OrthoRhombicOps::New();
  m_OrientationOps.push_back(dynamic_cast<OrientationMath*> (m_OrthoOps.get()));

  setupFilterOptions();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindSchmids::~FindSchmids()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSchmids::setupFilterOptions()
{
  std::vector<FilterOption::Pointer> options;
  {
    FilterOption::Pointer option = FilterOption::New();
    option->setHumanLabel("Loading X:");
    option->setPropertyName("XLoading");
    option->setWidgetType(FilterOption::DoubleWidget);
    option->setValueType("float");
    option->setCastableValueType("double");
    options.push_back(option);
  }
  {
    FilterOption::Pointer option = FilterOption::New();
    option->setHumanLabel("Loading Y:");
    option->setPropertyName("YLoading");
    option->setWidgetType(FilterOption::DoubleWidget);
    option->setValueType("float");
    option->setCastableValueType("double");
    options.push_back(option);
  }
  {
    FilterOption::Pointer option = FilterOption::New();
    option->setHumanLabel("Loading Z:");
    option->setPropertyName("ZLoading");
    option->setWidgetType(FilterOption::DoubleWidget);
    option->setValueType("float");
    option->setCastableValueType("double");
    options.push_back(option);
  }
  setFilterOptions(options);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSchmids::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;
  DataContainer* m = getDataContainer();

  GET_PREREQ_DATA(m, DREAM3D, FieldData, AvgQuats, ss, -301, float, FloatArrayType, fields, 5);

  CREATE_NON_PREREQ_DATA(m, DREAM3D, FieldData, Schmids, ss, float, FloatArrayType, fields, 1);
  CREATE_NON_PREREQ_DATA(m, DREAM3D, FieldData, SlipSystems, ss, int32_t, Int32ArrayType, fields, 1);

  typedef DataArray<unsigned int> XTalStructArrayType;
  GET_PREREQ_DATA(m, DREAM3D, EnsembleData, CrystalStructures, ss, -305, unsigned int, XTalStructArrayType, ensembles, 1);

  setErrorMessage(ss.str());
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSchmids::preflight()
{
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSchmids::execute()
{
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

  dataCheck(false, m->getTotalPoints(), m->getNumFieldTuples(), m->getNumEnsembleTuples());
  if (getErrorCondition() < 0)
  {
    return;
  }

  int ss = 0;
  float q1[5];
  float schmid = 0;
  float loadx, loady, loadz;

  size_t numgrains = m->getNumFieldTuples();
  for (size_t i = 1; i < numgrains; i++)
  {
      q1[1] = m_AvgQuats[5*i+1]/m_AvgQuats[5*i];
      q1[2] = m_AvgQuats[5*i+2]/m_AvgQuats[5*i];
      q1[3] = m_AvgQuats[5*i+3]/m_AvgQuats[5*i];
      q1[4] = m_AvgQuats[5*i+4]/m_AvgQuats[5*i];
	  if(m_AvgQuats[5*i] == 0) q1[1] = 0, q1[2] = 0, q1[3] = 0, q1[4] = 1;

      loadx = ((1 - 2*q1[2]*q1[2] - 2*q1[3]*q1[3]) * m_XLoading) + ((2*q1[1]*q1[2] + 2*q1[3]*q1[4]) * m_YLoading) + ((2*q1[1]*q1[3] - 2*q1[2]*q1[4]) * m_ZLoading);
      loady = ((2*q1[1]*q1[2] - 2*q1[3]*q1[4]) * m_XLoading) + ((1 - 2*q1[1]*q1[1] - 2*q1[3]*q1[3]) * m_YLoading) + ((2*q1[2]*q1[3] + 2*q1[1]*q1[4]) * m_ZLoading);
      loadz = ((2*q1[1]*q1[3] + 2*q1[2]*q1[4]) * m_XLoading) + ((2*q1[2]*q1[3] - 2*q1[1]*q1[4]) * m_YLoading) + ((1 - 2*q1[1]*q1[1] - 2*q1[2]*q1[2]) * m_ZLoading);

	  m_OrientationOps[m_CrystalStructures[i]]->getSchmidFactorAndSS(loadx, loady, loadz, schmid, ss);

      m_Schmids[i] = schmid;
	  m_SlipSystems[i] = ss;
  }

  notify("FindSchmids Completed", 0, Observable::UpdateProgressMessage);
}
