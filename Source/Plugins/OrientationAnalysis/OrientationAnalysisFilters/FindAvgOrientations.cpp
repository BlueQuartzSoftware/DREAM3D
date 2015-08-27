/* ============================================================================
* Copyright (c) 2009-2015 BlueQuartz Software, LLC
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
* Neither the name of BlueQuartz Software, the US Air Force, nor the names of its
* contributors may be used to endorse or promote products derived from this software
* without specific prior written permission.
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
* The code contained herein was partially funded by the followig contracts:
*    United States Air Force Prime Contract FA8650-07-D-5800
*    United States Air Force Prime Contract FA8650-10-D-5210
*    United States Prime Contract Navy N00173-07-C-2068
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */


#include "FindAvgOrientations.h"

#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/FilterParameters/AbstractFilterParametersReader.h"
#include "DREAM3DLib/FilterParameters/AbstractFilterParametersWriter.h"

#include "DREAM3DLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "DREAM3DLib/FilterParameters/DataArrayCreationFilterParameter.h"
#include "DREAM3DLib/FilterParameters/SeparatorFilterParameter.h"

#include "OrientationLib/OrientationMath/OrientationMath.h"

#include "OrientationAnalysis/OrientationAnalysisConstants.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindAvgOrientations::FindAvgOrientations() :
  AbstractFilter(),
  m_FeatureIdsArrayPath("", "", ""),
  m_CellPhasesArrayPath("", "", ""),
  m_QuatsArrayPath("", "", ""),
  m_CrystalStructuresArrayPath("", "", ""),
  m_AvgQuatsArrayPath("", "", ""),
  m_AvgEulerAnglesArrayPath("", "", ""),
  m_FeatureIds(NULL),
  m_CellPhases(NULL),
  m_Quats(NULL),
  m_CrystalStructures(NULL),
  m_FeatureEulerAngles(NULL),
  m_AvgQuats(NULL)
{
  m_OrientationOps = SpaceGroupOps::getOrientationOpsQVector();

  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindAvgOrientations::~FindAvgOrientations()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindAvgOrientations::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(SeparatorFilterParameter::New("Element Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::DataStructureRequirements req = DataArraySelectionFilterParameter::CreateCategoryRequirement(DREAM3D::TypeNames::Int32, 1, DREAM3D::AttributeMatrixObjectType::Element);
    parameters.push_back(DataArraySelectionFilterParameter::New("Feature Ids", "FeatureIdsArrayPath", getFeatureIdsArrayPath(), FilterParameter::RequiredArray, req));
  }
  {
    DataArraySelectionFilterParameter::DataStructureRequirements req = DataArraySelectionFilterParameter::CreateCategoryRequirement(DREAM3D::TypeNames::Int32, 1, DREAM3D::AttributeMatrixObjectType::Element);
    parameters.push_back(DataArraySelectionFilterParameter::New("Phases", "CellPhasesArrayPath", getCellPhasesArrayPath(), FilterParameter::RequiredArray, req));
  }
  {
    DataArraySelectionFilterParameter::DataStructureRequirements req = DataArraySelectionFilterParameter::CreateCategoryRequirement(DREAM3D::TypeNames::Float, 4, DREAM3D::AttributeMatrixObjectType::Element);
    parameters.push_back(DataArraySelectionFilterParameter::New("Quaternions", "QuatsArrayPath", getQuatsArrayPath(), FilterParameter::RequiredArray, req));
  }
  parameters.push_back(SeparatorFilterParameter::New("Ensemble Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::DataStructureRequirements req = DataArraySelectionFilterParameter::CreateCategoryRequirement(DREAM3D::TypeNames::UInt32, 1, DREAM3D::AttributeMatrixObjectType::Ensemble);
    parameters.push_back(DataArraySelectionFilterParameter::New("Crystal Structures", "CrystalStructuresArrayPath", getCrystalStructuresArrayPath(), FilterParameter::RequiredArray, req));
  }
  parameters.push_back(SeparatorFilterParameter::New("Feature Data", FilterParameter::CreatedArray));

  {
    DataArrayCreationFilterParameter::DataStructureRequirements req = DataArrayCreationFilterParameter::CreateRequirement(DREAM3D::AttributeMatrixObjectType::Feature);
    parameters.push_back(DataArrayCreationFilterParameter::New("Average Quaternions", "AvgQuatsArrayPath", getAvgQuatsArrayPath(), FilterParameter::CreatedArray, req));
  }
  {
    DataArrayCreationFilterParameter::DataStructureRequirements req = DataArrayCreationFilterParameter::CreateRequirement(DREAM3D::AttributeMatrixObjectType::Feature);
    parameters.push_back(DataArrayCreationFilterParameter::New("Average Euler Angles", "AvgEulerAnglesArrayPath", getAvgEulerAnglesArrayPath(), FilterParameter::CreatedArray, req));
  }
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindAvgOrientations::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setAvgEulerAnglesArrayPath(reader->readDataArrayPath("AvgEulerAnglesArrayPath", getAvgEulerAnglesArrayPath() ) );
  setAvgQuatsArrayPath(reader->readDataArrayPath("AvgQuatsArrayPath", getAvgQuatsArrayPath() ) );
  setCrystalStructuresArrayPath(reader->readDataArrayPath("CrystalStructuresArrayPath", getCrystalStructuresArrayPath() ) );
  setQuatsArrayPath(reader->readDataArrayPath("QuatsArrayPath", getQuatsArrayPath() ) );
  setCellPhasesArrayPath(reader->readDataArrayPath("CellPhasesArrayPath", getCellPhasesArrayPath() ) );
  setFeatureIdsArrayPath(reader->readDataArrayPath("FeatureIdsArrayPath", getFeatureIdsArrayPath() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int FindAvgOrientations::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  DREAM3D_FILTER_WRITE_PARAMETER(FilterVersion)
  DREAM3D_FILTER_WRITE_PARAMETER(AvgQuatsArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(AvgEulerAnglesArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(CrystalStructuresArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(QuatsArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(CellPhasesArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(FeatureIdsArrayPath)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindAvgOrientations::dataCheck()
{
  setErrorCondition(0);

  QVector<DataArrayPath> dataArrayPaths;

  QVector<size_t> cDims(1, 1);
  m_FeatureIdsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getFeatureIdsArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeatureIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCondition() >= 0) { dataArrayPaths.push_back(getFeatureIdsArrayPath()); }

  m_CellPhasesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getCellPhasesArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CellPhasesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_CellPhases = m_CellPhasesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCondition() >= 0) { dataArrayPaths.push_back(getCellPhasesArrayPath()); }

  cDims[0] = 4;
  m_QuatsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getQuatsArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_QuatsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_Quats = m_QuatsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCondition() >= 0) { dataArrayPaths.push_back(getQuatsArrayPath()); }

  m_AvgQuatsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this, getAvgQuatsArrayPath(), 0, cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_AvgQuatsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_AvgQuats = m_AvgQuatsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  cDims[0] = 3;
  m_FeatureEulerAnglesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this, getAvgEulerAnglesArrayPath(), 0, cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeatureEulerAnglesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeatureEulerAngles = m_FeatureEulerAnglesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  cDims[0] = 1;
  m_CrystalStructuresPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<uint32_t>, AbstractFilter>(this, getCrystalStructuresArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CrystalStructuresPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_CrystalStructures = m_CrystalStructuresPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  getDataContainerArray()->validateNumberOfTuples<AbstractFilter>(this, dataArrayPaths);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindAvgOrientations::preflight()
{
  setInPreflight(true);
  emit preflightAboutToExecute();
  emit updateFilterParameters(this);
  dataCheck();
  emit preflightExecuted();
  setInPreflight(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindAvgOrientations::execute()
{
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  size_t totalPoints = m_FeatureIdsPtr.lock()->getNumberOfTuples();
  size_t totalFeatures = m_AvgQuatsPtr.lock()->getNumberOfTuples();

  std::vector<float> counts(totalFeatures, 0.0f);

  int32_t phase = 0;
  QuatF voxquat = QuaternionMathF::New();
  QuatF curavgquat = QuaternionMathF::New();
  QuatF* avgQuats = reinterpret_cast<QuatF*>(m_AvgQuats);
  QuatF* quats = reinterpret_cast<QuatF*>(m_Quats);

  for (size_t i = 1; i < totalFeatures; i++)
  {
    QuaternionMathF::ElementWiseAssign(avgQuats[i], 0.0);
  }

  for (size_t i = 0; i < totalPoints; i++)
  {
    if (m_FeatureIds[i] > 0 && m_CellPhases[i] > 0)
    {
      counts[m_FeatureIds[i]] += 1;
      phase = m_CellPhases[i];
      QuaternionMathF::Copy(quats[i], voxquat);
      QuaternionMathF::Copy(avgQuats[m_FeatureIds[i]], curavgquat);
      QuaternionMathF::ScalarDivide(curavgquat, counts[m_FeatureIds[i]]);

      if (counts[m_FeatureIds[i]] == 1)
      {
        QuaternionMathF::Identity(curavgquat);
      }
      m_OrientationOps[m_CrystalStructures[phase]]->getNearestQuat(curavgquat, voxquat);
      QuaternionMathF::Add(avgQuats[m_FeatureIds[i]], voxquat, avgQuats[m_FeatureIds[i]]);
    }
  }

  for (size_t i = 1; i < totalFeatures; i++)
  {
    if (counts[i] == 0)
    {
      QuaternionMathF::Identity(avgQuats[i]);
    }
    QuaternionMathF::ScalarDivide(avgQuats[i], counts[i]);
    QuaternionMathF::UnitQuaternion(avgQuats[i]);

    FOrientArrayType eu(m_FeatureEulerAngles + (3 * i), 3);
    FOrientTransformsType::qu2eu(FOrientArrayType(avgQuats[i]), eu);
  }
  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer FindAvgOrientations::newFilterInstance(bool copyFilterParameters)
{
  FindAvgOrientations::Pointer filter = FindAvgOrientations::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindAvgOrientations::getCompiledLibraryName()
{ return OrientationAnalysisConstants::OrientationAnalysisBaseName; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindAvgOrientations::getGroupName()
{ return DREAM3D::FilterGroups::StatisticsFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindAvgOrientations::getSubGroupName()
{ return DREAM3D::FilterSubGroups::CrystallographicFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindAvgOrientations::getHumanLabel()
{ return "Find Feature Average Orientations"; }
