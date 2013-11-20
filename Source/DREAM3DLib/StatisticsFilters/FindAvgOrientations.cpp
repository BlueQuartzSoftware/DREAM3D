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

#include "FindAvgOrientations.h"

#include "DREAM3DLib/Math/DREAM3DMath.h"
#include "DREAM3DLib/Common/Constants.h"

#include "DREAM3DLib/Math/QuaternionMath.hpp"
#include "DREAM3DLib/GenericFilters/FindCellQuats.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindAvgOrientations::FindAvgOrientations() :
  AbstractFilter(),
  m_DataContainerName(DREAM3D::HDF5::VolumeDataContainerName),
  m_FeatureIdsArrayName(DREAM3D::CellData::FeatureIds),
  m_CellPhasesArrayName(DREAM3D::CellData::Phases),
  m_QuatsArrayName(DREAM3D::CellData::Quats),
  m_AvgQuatsArrayName(DREAM3D::FeatureData::AvgQuats),
  m_FeatureEulerAnglesArrayName(DREAM3D::FeatureData::EulerAngles),
  m_CrystalStructuresArrayName(DREAM3D::EnsembleData::CrystalStructures),
  m_FeatureIds(NULL),
  m_CellPhases(NULL),
  m_FeatureEulerAngles(NULL),
  m_Quats(NULL),
  m_AvgQuats(NULL)
{
  m_OrientationOps = OrientationOps::getOrientationOpsVector();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindAvgOrientations::~FindAvgOrientations()
{
}
// -----------------------------------------------------------------------------
void FindAvgOrientations::readFilterParameters(AbstractFilterParametersReader* reader, int index)
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
int FindAvgOrientations::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindAvgOrientations::dataCheck(bool preflight, size_t voxels, size_t features, size_t ensembles)
{
  setErrorCondition(0);

  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

  QVector<int> dims(1, 1);
  m_FeatureIdsPtr = m->getPrereqArray<int32_t, AbstractFilter>(this, m_CellAttributeMatrixName,  m_FeatureIdsArrayName, -300,  voxels, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0); /* Assigns the actual data pointer to our instance variable m_FeatureIds */
  m_CellPhasesPtr = m->getPrereqArray<int32_t, AbstractFilter>(this, m_CellAttributeMatrixName,  m_CellPhasesArrayName, -300,  voxels, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  m_CellPhases = m_CellPhasesPtr.lock()->getPointer(0); /* Assigns the actual data pointer to our instance variable m_CellPhases */

  dims[0] = 4;
  m_QuatsPtr = m->getPrereqArray<float, AbstractFilter>(this, m_CellAttributeMatrixName,  m_QuatsArrayName, -303, voxels, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  m_Quats = m_QuatsPtr.lock()->getPointer(0); /* Assigns the actual data pointer to our instance variable m_Quats */

  m_AvgQuatsPtr = m->createNonPrereqArray<float, AbstractFilter>(this, m_CellFeatureAttributeMatrixName,  m_AvgQuatsArrayName, 0, features, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  m_AvgQuats = m_AvgQuatsPtr.lock()->getPointer(0); /* Assigns the actual data pointer to our instance variable m_AvgQuats */
  dims[0] = 3;
  m_FeatureEulerAnglesPtr = m->createNonPrereqArray<float, AbstractFilter>(this, m_CellFeatureAttributeMatrixName,  m_FeatureEulerAnglesArrayName, 0, features, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  m_FeatureEulerAngles = m_FeatureEulerAnglesPtr.lock()->getPointer(0); /* Assigns the actual data pointer to our instance variable m_FeatureEulerAngles */

  typedef DataArray<unsigned int> XTalStructArrayType;
  dims[0] = 1;
  m_CrystalStructuresPtr = m->getPrereqArray<unsigned int, AbstractFilter>(this, m_CellEnsembleAttributeMatrixName,  m_CrystalStructuresArrayName, -305, ensembles, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  m_CrystalStructures = m_CrystalStructuresPtr.lock()->getPointer(0); /* Assigns the actual data pointer to our instance variable m_CrystalStructures */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindAvgOrientations::preflight()
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
void FindAvgOrientations::execute()
{
  setErrorCondition(0);

  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The DataContainer Object was NULL", -999);
    return;
  }
  int64_t totalPoints = m->getTotalPoints();
  dataCheck(false, m->getTotalPoints(), m->getNumCellFeatureTuples(), m->getNumCellEnsembleTuples());
  if (getErrorCondition() < 0)
  {
    return;
  }

  size_t numfeatures = m->getNumCellFeatureTuples();
  QVector<float> counts(numfeatures, 0.0);

  int phase;
  QuatF voxquat;
  QuatF curavgquat;
  QuatF* avgQuats = reinterpret_cast<QuatF*>(m_AvgQuats);
  QuatF* quats = reinterpret_cast<QuatF*>(m_Quats);

  for (size_t i = 1; i < numfeatures; i++)
  {
    QuaternionMathF::ElementWiseAssign(avgQuats[i], 0.0);
  }
  for(int i = 0; i < totalPoints; i++)
  {
    if(m_FeatureIds[i] > 0 && m_CellPhases[i] > 0)
    {
      counts[m_FeatureIds[i]] += 1;
      phase = m_CellPhases[i];
      QuaternionMathF::Copy(quats[i], voxquat);
      QuaternionMathF::Copy(avgQuats[m_FeatureIds[i]], curavgquat);
      QuaternionMathF::ScalarDivide(curavgquat, counts[m_FeatureIds[i]]);

      if(counts[m_FeatureIds[i]] == 1)
      {
        QuaternionMathF::Identity(curavgquat);
      }
      m_OrientationOps[m_CrystalStructures[phase]]->getNearestQuat(curavgquat, voxquat);
      QuaternionMathF::Add(avgQuats[m_FeatureIds[i]], voxquat, avgQuats[m_FeatureIds[i]]);
    }
  }
  float ea1, ea2, ea3;
  for (size_t i = 1; i < numfeatures; i++)
  {
    if(counts[i] == 0)
    {
      QuaternionMathF::Identity(avgQuats[i]);
    }
    QuaternionMathF::ScalarDivide(avgQuats[i], counts[i]);
    QuaternionMathF::UnitQuaternion(avgQuats[i]);
    OrientationMath::QuattoEuler(avgQuats[i], ea1, ea2, ea3);
    m_FeatureEulerAngles[3 * i] = ea1;
    m_FeatureEulerAngles[3 * i + 1] = ea2;
    m_FeatureEulerAngles[3 * i + 2] = ea3;
  }
  notifyStatusMessage("Completed");
}


