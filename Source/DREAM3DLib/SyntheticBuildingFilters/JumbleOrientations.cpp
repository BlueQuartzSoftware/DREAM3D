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

#include "JumbleOrientations.h"

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/variate_generator.hpp>

#include "DREAM3DLib/Math/DREAM3DMath.h"
#include "DREAM3DLib/OrientationOps/OrientationOps.h"
#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/DataContainers/DataContainerMacros.h"
#include "DREAM3DLib/Utilities/DREAM3DRandom.h"

#include "DREAM3DLib/GenericFilters/FindGrainPhases.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------

JumbleOrientations::JumbleOrientations() :
  AbstractFilter(),
  m_GrainIdsArrayName(DREAM3D::CellData::GrainIds),
  m_CellEulerAnglesArrayName(DREAM3D::CellData::EulerAngles),
  m_FieldPhasesArrayName(DREAM3D::FieldData::Phases),
  m_AvgQuatsArrayName(DREAM3D::FieldData::AvgQuats),
  m_FieldEulerAnglesArrayName(DREAM3D::FieldData::EulerAngles),
  m_GrainIds(NULL),
  m_CellEulerAngles(NULL),
  m_FieldPhases(NULL),
  m_FieldEulerAngles(NULL),
  m_AvgQuats(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
JumbleOrientations::~JumbleOrientations()
{
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void JumbleOrientations::setupFilterParameters()
{

}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void JumbleOrientations::readFilterParameters(AbstractFilterParametersReader* reader, int index)
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
int JumbleOrientations::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void JumbleOrientations::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  
  VolumeDataContainer* m = getVolumeDataContainer();
  // Cell Data
  GET_PREREQ_DATA( m, DREAM3D, CellData, GrainIds, -301, int32_t, Int32ArrayType, voxels, 1)
      CREATE_NON_PREREQ_DATA( m, DREAM3D, CellData, CellEulerAngles, float, FloatArrayType, 0, voxels, 3)

      GET_PREREQ_DATA(m, DREAM3D, CellFieldData, FieldPhases, -303, int32_t, Int32ArrayType, fields, 1)

      CREATE_NON_PREREQ_DATA(m, DREAM3D, CellFieldData, FieldEulerAngles, float, FloatArrayType, 0, fields, 3)
      CREATE_NON_PREREQ_DATA(m, DREAM3D, CellFieldData, AvgQuats, float, FloatArrayType, 0, fields, 4)
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void JumbleOrientations::preflight()
{
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void JumbleOrientations::execute()
{
  int err = 0;
  setErrorCondition(err);
  VolumeDataContainer* m = getVolumeDataContainer();
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The DataContainer Object was NULL", -999);
    return;
  }

  int64_t totalPoints = m->getTotalPoints();
  int totalFields = m->getNumCellFieldTuples();
  int numEnsembleTuples = m->getNumCellEnsembleTuples();
  dataCheck(false, totalPoints, totalFields, numEnsembleTuples);
  if (getErrorCondition() < 0)
  {
    return;
  }

  // Generate all the numbers up front
  const int rangeMin = 1;
  const int rangeMax = totalFields - 1;
  typedef boost::uniform_int<int> NumberDistribution;
  typedef boost::mt19937 RandomNumberGenerator;
  typedef boost::variate_generator<RandomNumberGenerator&,
      NumberDistribution> Generator;

  NumberDistribution distribution(rangeMin, rangeMax);
  RandomNumberGenerator generator;
  Generator numberGenerator(generator, distribution);
  generator.seed(static_cast<boost::uint32_t>( QDateTime::currentMSecsSinceEpoch() )); // seed with the current time


  int r;
  float temp1, temp2, temp3;
  //--- Shuffle elements by randomly exchanging each with one other.
  for (int i=1; i< totalFields; i++)
  {
    bool good = false;
    while(good == false)
    {
      good = true;
      r = numberGenerator(); // Random remaining position.
      if (r >= totalFields) good = false;
      if (m_FieldPhases[i] != m_FieldPhases[r]) good = false;
    }
    temp1 = m_FieldEulerAngles[3*i];
    temp2 = m_FieldEulerAngles[3*i+1];
    temp3 = m_FieldEulerAngles[3*i+2];
    m_FieldEulerAngles[3*i] = m_FieldEulerAngles[3*r];
    m_FieldEulerAngles[3*i+1] = m_FieldEulerAngles[3*r+1];
    m_FieldEulerAngles[3*i+2] = m_FieldEulerAngles[3*r+2];
    m_FieldEulerAngles[3*r] = temp1;
    m_FieldEulerAngles[3*r+1] = temp2;
    m_FieldEulerAngles[3*r+2] = temp3;
  }

  // Now adjust all the Euler angle values for each Voxel
  for(int64_t i = 0; i < totalPoints; ++i)
  {
    m_CellEulerAngles[3*i] = m_FieldEulerAngles[3*(m_GrainIds[i])];
    m_CellEulerAngles[3*i+1] = m_FieldEulerAngles[3*(m_GrainIds[i])+1];
    m_CellEulerAngles[3*i+2] = m_FieldEulerAngles[3*(m_GrainIds[i])+2];
  }
  QuatF q;
  QuatF* avgQuats = reinterpret_cast<QuatF*>(m_AvgQuats);
  for (int i=1; i< totalFields; i++)
  {
    OrientationMath::EulertoQuat(q, m_FieldEulerAngles[3*i], m_FieldEulerAngles[3*i+1], m_FieldEulerAngles[3*i+2]);
    QuaternionMathF::Copy(q, avgQuats[i]);
//    m_AvgQuats[5*i] = q[0];
//    m_AvgQuats[5*i+1] = q[1];
//    m_AvgQuats[5*i+2] = q[2];
//    m_AvgQuats[5*i+3] = q[3];
//    m_AvgQuats[5*i+4] = q[4];
  }

  // If there is an error set this to something negative and also set a message
  notifyStatusMessage("Jumbling Orientations Complete");
}
