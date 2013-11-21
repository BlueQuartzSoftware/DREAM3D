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
#include "DREAM3DLib/Math/DREAM3DMath.h"
#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/GenericFilters/FindFeaturePhases.h"
#include "DREAM3DLib/StatisticsFilters/FindAvgOrientations.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindSchmids::FindSchmids() :
  AbstractFilter(),
  m_DataContainerName(DREAM3D::HDF5::VolumeDataContainerName),
  m_AvgQuatsArrayName(DREAM3D::FeatureData::AvgQuats),
  m_SchmidsArrayName(DREAM3D::FeatureData::Schmids),
  m_PolesArrayName(DREAM3D::FeatureData::Poles),
  m_FeaturePhasesArrayName(DREAM3D::FeatureData::Phases),
  m_SlipSystemsArrayName(DREAM3D::FeatureData::SlipSystems),
  m_CrystalStructuresArrayName(DREAM3D::EnsembleData::CrystalStructures),
  m_Schmids(NULL),
  m_Poles(NULL),
  m_FeaturePhases(NULL),
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
  FilterParameterVector parameters;
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
void FindSchmids::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  /* Code to read the values goes between these statements */
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE BEGIN*/
  setLoadingDir( reader->readFloatVec3("LoadingDir", getLoadingDir() ) );
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE END*/
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int FindSchmids::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("LoadingDirection", getLoadingDir() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSchmids::dataCheck(bool preflight, size_t voxels, size_t features, size_t ensembles)
{
  setErrorCondition(0);

  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

  QVector<int> dims(1, 1);
  m_SchmidsPtr = m->createNonPrereqArray<float, AbstractFilter>(this, m_CellFeatureAttributeMatrixName,  m_SchmidsArrayName, 0, features, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  m_Schmids = m_SchmidsPtr.lock()->getPointer(0); /* Assigns the actual data pointer to our instance variable m_Schmids */
  m_FeaturePhasesPtr = m->getPrereqArray<int32_t, AbstractFilter>(this, m_CellFeatureAttributeMatrixName,  m_FeaturePhasesArrayName, -302, features, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  m_FeaturePhases = m_FeaturePhasesPtr.lock()->getPointer(0); /* Assigns the actual data pointer to our instance variable m_FeaturePhases */
  m_SlipSystemsPtr = m->createNonPrereqArray<int32_t, AbstractFilter>(this, m_CellFeatureAttributeMatrixName,  m_SlipSystemsArrayName, 0, features, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  m_SlipSystems = m_SlipSystemsPtr.lock()->getPointer(0); /* Assigns the actual data pointer to our instance variable m_SlipSystems */
  typedef DataArray<unsigned int> XTalStructArrayType;
  m_CrystalStructuresPtr = m->getPrereqArray<unsigned int, AbstractFilter>(this, m_CellEnsembleAttributeMatrixName,  m_CrystalStructuresArrayName, -305, ensembles, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  m_CrystalStructures = m_CrystalStructuresPtr.lock()->getPointer(0); /* Assigns the actual data pointer to our instance variable m_CrystalStructures */
  dims[0] = 3;
  m_PolesPtr = m->createNonPrereqArray<int32_t, AbstractFilter>(this, m_CellFeatureAttributeMatrixName,  m_PolesArrayName, 0, features, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  m_Poles = m_PolesPtr.lock()->getPointer(0); /* Assigns the actual data pointer to our instance variable m_Poles */
  dims[0] = 4;
  m_AvgQuatsPtr = m->getPrereqArray<float, AbstractFilter>(this, m_CellFeatureAttributeMatrixName,  m_AvgQuatsArrayName, -301, features, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  m_AvgQuats = m_AvgQuatsPtr.lock()->getPointer(0); /* Assigns the actual data pointer to our instance variable m_AvgQuats */
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSchmids::preflight()
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
void FindSchmids::execute()
{
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The DataContainer Object was NULL", -999);
    return;
  }
  setErrorCondition(0);

  dataCheck(false, totalPoints, totalFeatures, totalEnsembles);
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

  sampleLoading[0] = m_LoadingDir.x;
  sampleLoading[1] = m_LoadingDir.y;
  sampleLoading[2] = m_LoadingDir.z;
  MatrixMath::Normalize3x1(sampleLoading);

  size_t numfeatures = m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->getNumTuples();
  for (size_t i = 1; i < numfeatures; i++)
  {
    QuaternionMathF::Copy(avgQuats[i], q1);
    OrientationMath::QuattoMat(q1, g);

    MatrixMath::Multiply3x3with3x1(g, sampleLoading, crystalLoading);

    m_OrientationOps[m_CrystalStructures[m_FeaturePhases[i]]]->getSchmidFactorAndSS(crystalLoading[0], crystalLoading[1], crystalLoading[2], schmid, ss);

    m_Schmids[i] = schmid;
    m_Poles[3 * i] = int32_t(crystalLoading[0] * 100);
    m_Poles[3 * i + 1] = int32_t(crystalLoading[1] * 100);
    m_Poles[3 * i + 2] = int32_t(crystalLoading[2] * 100);
    m_SlipSystems[i] = ss;
  }

  notifyStatusMessage("FindSchmids Completed");
}
