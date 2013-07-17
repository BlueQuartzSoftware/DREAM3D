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

#include "DREAM3DLib/Math/MatrixMath.h"
#include "DREAM3DLib/Common/DREAM3DMath.h"
#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/GenericFilters/FindGrainPhases.h"
#include "DREAM3DLib/StatisticsFilters/FindAvgOrientations.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindSchmids::FindSchmids() :
  AbstractFilter(),
  m_AvgQuatsArrayName(DREAM3D::FieldData::AvgQuats),
  m_SchmidsArrayName(DREAM3D::FieldData::Schmids),
  m_PolesArrayName(DREAM3D::FieldData::Poles),
  m_FieldPhasesArrayName(DREAM3D::FieldData::Phases),
  m_SlipSystemsArrayName(DREAM3D::FieldData::SlipSystems),
  m_CrystalStructuresArrayName(DREAM3D::EnsembleData::CrystalStructures),
  m_Schmids(NULL),
  m_Poles(NULL),
  m_FieldPhases(NULL),
  m_AvgQuats(NULL),
  m_SlipSystems(NULL)
{
  m_LoadingDir.x = 1.0f;
  m_LoadingDir.y = 1.0f;
  m_LoadingDir.z = 1.0f;
  m_OrientationOps = OrientationOps::getOrientationOpsVector();

  setupFilterParameters();
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
void FindSchmids::setupFilterParameters()
{
  std::vector<FilterParameter::Pointer> parameters;
  {
    FilterParameter::Pointer option = FilterParameter::New();

    option->setHumanLabel("Loading Direction");
    option->setPropertyName("LoadingDir");
    option->setWidgetType(FilterParameter::FloatVec3Widget);
    option->setValueType("FloatVec3Widget_t");
    option->setUnits("");
    parameters.push_back(option);
  }
  setFilterParameters(parameters);
}
// -----------------------------------------------------------------------------
void FindSchmids::readFilterParameters(AbstractFilterParametersReader* reader)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSchmids::writeFilterParameters(AbstractFilterParametersWriter* writer)

{
  writer->writeValue("LoadingDirection", getLoadingDir() );
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSchmids::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;
  VoxelDataContainer* m = getVoxelDataContainer();

  GET_PREREQ_DATA(m, DREAM3D, FieldData, AvgQuats, ss, -301, float, FloatArrayType, fields, 4)

  CREATE_NON_PREREQ_DATA(m, DREAM3D, FieldData, Schmids, ss, float, FloatArrayType, 0, fields, 1)
  CREATE_NON_PREREQ_DATA(m, DREAM3D, FieldData, Poles, ss, int32_t, Int32ArrayType, 0, fields, 3)

  GET_PREREQ_DATA(m, DREAM3D, FieldData, FieldPhases, ss, -302, int32_t, Int32ArrayType, fields, 1)


  CREATE_NON_PREREQ_DATA(m, DREAM3D, FieldData, SlipSystems, ss, int32_t, Int32ArrayType, 0, fields, 1)

  typedef DataArray<unsigned int> XTalStructArrayType;
  GET_PREREQ_DATA(m, DREAM3D, EnsembleData, CrystalStructures, ss, -305, unsigned int, XTalStructArrayType, ensembles, 1)
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

  int ss = 0;
  QuatF q1;
  QuatF* avgQuats = reinterpret_cast<QuatF*>(m_AvgQuats);

  float g[3][3];
  float sampleLoading[3];
  float crystalLoading[3];
  float schmid = 0;

  size_t numgrains = m->getNumFieldTuples();
  for (size_t i = 1; i < numgrains; i++)
  {
    QuaternionMathF::Copy(avgQuats[i], q1);
    #warning Fix This
//    q1[0] = 1;
//      q1[1] = m_AvgQuats[5*i+1];
//      q1[2] = m_AvgQuats[5*i+2];
//      q1[3] = m_AvgQuats[5*i+3];
//      q1[4] = m_AvgQuats[5*i+4];
    if(m_AvgQuats[5*i] == 0)
    {
      QuaternionMathF::Identity(q1);
     //q1[1] = 0, q1[2] = 0, q1[3] = 0, q1[4] = 1;
    }
    OrientationMath::QuattoMat(q1, g);

    sampleLoading[0] = m_LoadingDir.x;
    sampleLoading[1] = m_LoadingDir.y;
    sampleLoading[2] = m_LoadingDir.z;

    MatrixMath::Multiply3x3with3x1(g, sampleLoading, crystalLoading);

    m_OrientationOps[m_CrystalStructures[m_FieldPhases[i]]]->getSchmidFactorAndSS(crystalLoading[0], crystalLoading[1], crystalLoading[2], schmid, ss);

      m_Schmids[i] = schmid;
    m_Poles[3*i] = int32_t(crystalLoading[0]*100);
    m_Poles[3*i+1] = int32_t(crystalLoading[1]*100);
    m_Poles[3*i+2] = int32_t(crystalLoading[2]*100);
    m_SlipSystems[i] = ss;
  }

 notifyStatusMessage("FindSchmids Completed");
}
