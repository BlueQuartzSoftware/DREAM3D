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
#include "DREAM3DLib/Utilities/DREAM3DRandom.h"

#include "DREAM3DLib/GenericFilters/FindFeaturePhases.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------

JumbleOrientations::JumbleOrientations() :
  AbstractFilter(),
  m_DataContainerName(DREAM3D::HDF5::VolumeDataContainerName),
  m_CellAttributeMatrixName(DREAM3D::HDF5::CellAttributeMatrixName),
  m_CellFeatureAttributeMatrixName(DREAM3D::HDF5::CellFeatureAttributeMatrixName),
  m_FeatureIdsArrayName(DREAM3D::CellData::FeatureIds),
  m_CellEulerAnglesArrayName(DREAM3D::CellData::EulerAngles),
  m_FeaturePhasesArrayName(DREAM3D::FeatureData::Phases),
  m_AvgQuatsArrayName(DREAM3D::FeatureData::AvgQuats),
  m_FeatureEulerAnglesArrayName(DREAM3D::FeatureData::EulerAngles),
  m_FeatureIds(NULL),
  m_CellEulerAngles(NULL),
  m_FeaturePhases(NULL),
  m_FeatureEulerAngles(NULL),
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
void JumbleOrientations::dataCheck(bool preflight, size_t voxels, size_t features, size_t ensembles)
{
  setErrorCondition(0);

  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

  QVector<int> dims(1, 1);
  // Cell Data
  m_FeatureIdsPtr = m->getPrereqArray<int32_t, Int32ArrayType, AbstractFilter>(this, m_CellAttributeMatrixName,  m_FeatureIdsArrayName, -301, voxels, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0); /* Assigns the actual data pointer to our instance variable m_FeatureIds */
  dims[0] = 3;
  m_CellEulerAnglesPtr = m->getPrereqArray<float, FloatArrayType, AbstractFilter>(this, m_CellAttributeMatrixName,  m_CellEulerAnglesArrayName, -301, voxels, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  m_CellEulerAngles = m_CellEulerAnglesPtr.lock()->getPointer(0); /* Assigns the actual data pointer to our instance variable m_CellEulerAngles */

  // Feature Data
  dims[0] = 1;
  m_FeaturePhasesPtr = m->getPrereqArray<int32_t, Int32ArrayType, AbstractFilter>(this, m_CellFeatureAttributeMatrixName,  m_FeaturePhasesArrayName, -301, features, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  m_FeaturePhases = m_FeaturePhasesPtr.lock()->getPointer(0); /* Assigns the actual data pointer to our instance variable m_FeaturePhases */
  dims[0] = 3;
  m_FeatureEulerAnglesPtr = m->createNonPrereqArray<float, FloatArrayType, AbstractFilter>(this, m_CellFeatureAttributeMatrixName,  m_FeatureEulerAnglesArrayName, 0, features, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  m_FeatureEulerAngles = m_FeatureEulerAnglesPtr.lock()->getPointer(0); /* Assigns the actual data pointer to our instance variable m_FeatureEulerAngles */
  dims[0] = 4;
  m_AvgQuatsPtr = m->createNonPrereqArray<float, FloatArrayType, AbstractFilter>(this, m_CellFeatureAttributeMatrixName,  m_AvgQuatsArrayName, 0, features, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  m_AvgQuats = m_AvgQuatsPtr.lock()->getPointer(0); /* Assigns the actual data pointer to our instance variable m_AvgQuats */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void JumbleOrientations::preflight()
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
void JumbleOrientations::execute()
{
  int err = 0;
  setErrorCondition(err);
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The DataContainer Object was NULL", -999);
    return;
  }

  int64_t totalPoints = m->getAttributeMatrix(m_CellAttributeMatrixName)->getNumTuples();
  int64_t totalFeatures = m->getAttributeMatrix(m_CellFeatureAttributeMatrixName)->getNumTuples();

  dataCheck(false, totalPoints, totalFeatures, 0);
  if (getErrorCondition() < 0)
  {
    return;
  }

  // Generate all the numbers up front
  const int rangeMin = 1;
  const int rangeMax = totalFeatures - 1;
  typedef boost::uniform_int<int> NumberDistribution;
  typedef boost::mt19937 RandomNumberGenerator;
  typedef boost::variate_generator < RandomNumberGenerator&,
          NumberDistribution > Generator;

  NumberDistribution distribution(rangeMin, rangeMax);
  RandomNumberGenerator generator;
  Generator numberGenerator(generator, distribution);
  generator.seed(static_cast<boost::uint32_t>( QDateTime::currentMSecsSinceEpoch() )); // seed with the current time


  int r;
  float temp1, temp2, temp3;
  //--- Shuffle elements by randomly exchanging each with one other.
  for (int i = 1; i < totalFeatures; i++)
  {
    bool good = false;
    while(good == false)
    {
      good = true;
      r = numberGenerator(); // Random remaining position.
      if (r >= totalFeatures) { good = false; }
      if (m_FeaturePhases[i] != m_FeaturePhases[r]) { good = false; }
    }
    temp1 = m_FeatureEulerAngles[3 * i];
    temp2 = m_FeatureEulerAngles[3 * i + 1];
    temp3 = m_FeatureEulerAngles[3 * i + 2];
    m_FeatureEulerAngles[3 * i] = m_FeatureEulerAngles[3 * r];
    m_FeatureEulerAngles[3 * i + 1] = m_FeatureEulerAngles[3 * r + 1];
    m_FeatureEulerAngles[3 * i + 2] = m_FeatureEulerAngles[3 * r + 2];
    m_FeatureEulerAngles[3 * r] = temp1;
    m_FeatureEulerAngles[3 * r + 1] = temp2;
    m_FeatureEulerAngles[3 * r + 2] = temp3;
  }

  // Now adjust all the Euler angle values for each Voxel
  for(int64_t i = 0; i < totalPoints; ++i)
  {
    m_CellEulerAngles[3 * i] = m_FeatureEulerAngles[3 * (m_FeatureIds[i])];
    m_CellEulerAngles[3 * i + 1] = m_FeatureEulerAngles[3 * (m_FeatureIds[i]) + 1];
    m_CellEulerAngles[3 * i + 2] = m_FeatureEulerAngles[3 * (m_FeatureIds[i]) + 2];
  }
  QuatF q;
  QuatF* avgQuats = reinterpret_cast<QuatF*>(m_AvgQuats);
  for (int i = 1; i < totalFeatures; i++)
  {
    OrientationMath::EulertoQuat(q, m_FeatureEulerAngles[3 * i], m_FeatureEulerAngles[3 * i + 1], m_FeatureEulerAngles[3 * i + 2]);
    QuaternionMathF::Copy(q, avgQuats[i]);
  }

  // If there is an error set this to something negative and also set a message
  notifyStatusMessage("Jumbling Orientations Complete");
}
