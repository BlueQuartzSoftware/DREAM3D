/* ============================================================================
* Copyright (c) 2009-2016 BlueQuartz Software, LLC
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

#include "FindLocalAverageCAxisMisalignments.h"

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/AttributeMatrixSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedBooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/Math/SIMPLibMath.h"
#include "SIMPLib/Math/MatrixMath.h"
#include "SIMPLib/Utilities/SIMPLibRandom.h"
#include "OrientationLib/SpaceGroupOps/SpaceGroupOps.h"

#include "OrientationAnalysis/OrientationAnalysisConstants.h"
#include "OrientationAnalysis/OrientationAnalysisVersion.h"

#define ERROR_TXT_OUT 1
#define ERROR_TXT_OUT1 1

#define NEW_SHARED_ARRAY(var, m_msgType, size)\
  boost::shared_array<m_msgType> var##Array(new m_msgType[size]);\
  m_msgType* var = var##Array.get();

// Include the MOC generated file for this class
#include "moc_FindLocalAverageCAxisMisalignments.cpp"



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindLocalAverageCAxisMisalignments::FindLocalAverageCAxisMisalignments() :
  AbstractFilter(),
  m_NewCellFeatureAttributeMatrixName("", "", ""),
  m_CalcUnbiasedAvg(false),
  m_CalcBiasedAvg(false),
  m_NeighborListArrayPath("", "", ""),
  m_CAxisMisalignmentListArrayPath("", "", ""),
  m_FeatureIdsArrayPath("", "", ""),
  m_CellParentIdsArrayPath("", "", ""),
  m_AvgCAxisMisalignmentsArrayPath("", "", ""),
  m_FeatureParentIdsArrayPath("", "", ""),
  m_CrystalStructuresArrayPath("", "", ""),
  m_UnbiasedLocalCAxisMisalignmentsArrayName(SIMPL::FeatureData::UnbiasedLocalCAxisMisalignments),
  m_NumFeaturesPerParentArrayName(SIMPL::FeatureData::NumFeaturesPerParent),
  m_FeatureIds(NULL),
  m_CellParentIds(NULL),
  m_FeatureParentIds(NULL),
  m_NumFeaturesPerParent(NULL),
  m_AvgCAxisMisalignments(NULL),
  m_LocalCAxisMisalignments(NULL),
  m_UnbiasedLocalCAxisMisalignments(NULL),
  m_CrystalStructures(NULL)
{
  m_OrientationOps = SpaceGroupOps::getOrientationOpsQVector();

  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindLocalAverageCAxisMisalignments::~FindLocalAverageCAxisMisalignments()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindLocalAverageCAxisMisalignments::setupFilterParameters()
{
  FilterParameterVector parameters;

  QStringList linkedProps("LocalCAxisMisalignmentsArrayName");
  parameters.push_back(SIMPL_NEW_LINKED_BOOL_FP("Calculate Local C-Axis Misalignments", CalcBiasedAvg, FilterParameter::Parameter, FindLocalAverageCAxisMisalignments, linkedProps));
  linkedProps.clear();
  linkedProps << "UnbiasedLocalCAxisMisalignmentsArrayName";
  parameters.push_back(SIMPL_NEW_LINKED_BOOL_FP("Calculate Unbiased Local C-Axis Misalignments", CalcUnbiasedAvg, FilterParameter::Parameter, FindLocalAverageCAxisMisalignments, linkedProps));

  {
    DataArraySelectionFilterParameter::RequirementType req;
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Neighbor List Array Name", NeighborListArrayPath, FilterParameter::RequiredArray, FindLocalAverageCAxisMisalignments, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req;
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("CAxis Misalignment List Array Name", CAxisMisalignmentListArrayPath, FilterParameter::RequiredArray, FindLocalAverageCAxisMisalignments, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req;
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("FeatureIds", FeatureIdsArrayPath, FilterParameter::RequiredArray, FindLocalAverageCAxisMisalignments, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req;
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("CellParentIds", CellParentIdsArrayPath, FilterParameter::RequiredArray, FindLocalAverageCAxisMisalignments, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req;
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("AvgCAxisMisalignments", AvgCAxisMisalignmentsArrayPath, FilterParameter::RequiredArray, FindLocalAverageCAxisMisalignments, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req;
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("FeatureParentIds", FeatureParentIdsArrayPath, FilterParameter::RequiredArray, FindLocalAverageCAxisMisalignments, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req;
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Crystal Structures", CrystalStructuresArrayPath, FilterParameter::RequiredArray, FindLocalAverageCAxisMisalignments, req));
  }
  {
    AttributeMatrixSelectionFilterParameter::RequirementType req = AttributeMatrixSelectionFilterParameter::CreateRequirement(SIMPL::AttributeMatrixType::CellFeature, SIMPL::GeometryType::UnknownGeometry);
    parameters.push_back(SIMPL_NEW_AM_SELECTION_FP("New Cell Feature Attribute Matrix Name", NewCellFeatureAttributeMatrixName, FilterParameter::RequiredArray, FindLocalAverageCAxisMisalignments, req));
  }

  parameters.push_back(SIMPL_NEW_STRING_FP("NumFeaturesPerParent", NumFeaturesPerParentArrayName, FilterParameter::CreatedArray, FindLocalAverageCAxisMisalignments));
  parameters.push_back(SIMPL_NEW_STRING_FP("LocalCAxisMisalignments", LocalCAxisMisalignmentsArrayName, FilterParameter::CreatedArray, FindLocalAverageCAxisMisalignments));
  parameters.push_back(SIMPL_NEW_STRING_FP("UnbiasedLocalCAxisMisalignments", UnbiasedLocalCAxisMisalignmentsArrayName, FilterParameter::CreatedArray, FindLocalAverageCAxisMisalignments));

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindLocalAverageCAxisMisalignments::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setNewCellFeatureAttributeMatrixName(reader->readDataArrayPath("NewCellFeatureAttributeMatrixName", getNewCellFeatureAttributeMatrixName()));
  setNeighborListArrayPath(reader->readDataArrayPath("NeighborListArrayPath", getNeighborListArrayPath() ) );
  setCAxisMisalignmentListArrayPath(reader->readDataArrayPath("CAxisMisalignmentListArrayPath", getCAxisMisalignmentListArrayPath() ) );
  setNumFeaturesPerParentArrayName(reader->readString("NumFeaturesPerParentArrayName", getNumFeaturesPerParentArrayName() ) );
  setLocalCAxisMisalignmentsArrayName(reader->readString("LocalCAxisMisalignmentsArrayName", getLocalCAxisMisalignmentsArrayName() ) );
  setUnbiasedLocalCAxisMisalignmentsArrayName(reader->readString("UnbiasedLocalCAxisMisalignmentsArrayName", getUnbiasedLocalCAxisMisalignmentsArrayName() ) );
  setCrystalStructuresArrayPath(reader->readDataArrayPath("CrystalStructuresArrayPath", getCrystalStructuresArrayPath() ) );
  setFeatureParentIdsArrayPath(reader->readDataArrayPath("FeatureParentIdsArrayPath", getFeatureParentIdsArrayPath() ) );
  setAvgCAxisMisalignmentsArrayPath(reader->readDataArrayPath("AvgCAxisMisalignmentsArrayPath", getAvgCAxisMisalignmentsArrayPath() ) );
  setCellParentIdsArrayPath(reader->readDataArrayPath("CellParentIdsArrayPath", getCellParentIdsArrayPath() ) );
  setFeatureIdsArrayPath(reader->readDataArrayPath("FeatureIdsArrayPath", getFeatureIdsArrayPath() ) );
  setCalcUnbiasedAvg( reader->readValue("CalcUnbiasedAvg", getCalcUnbiasedAvg()) );
  setCalcBiasedAvg( reader->readValue("CalcBiasedAvg", getCalcBiasedAvg()) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindLocalAverageCAxisMisalignments::initialize()
{
  m_NeighborList = NeighborList<int32_t>::NullPointer();
  m_CAxisMisalignmentList = NeighborList<float>::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindLocalAverageCAxisMisalignments::dataCheck()
{
  DataArrayPath tempPath;
  setErrorCondition(0);

  // Cell Data
  QVector<size_t> dims(1, 1);
  m_FeatureIdsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getFeatureIdsArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeatureIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  m_CellParentIdsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getCellParentIdsArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CellParentIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_CellParentIds = m_CellParentIdsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  // Feature Data
  if ( m_CalcUnbiasedAvg == true)
  {
    tempPath.update(getNewCellFeatureAttributeMatrixName().getDataContainerName(), getNewCellFeatureAttributeMatrixName().getAttributeMatrixName(), getUnbiasedLocalCAxisMisalignmentsArrayName() );
    m_UnbiasedLocalCAxisMisalignmentsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this, tempPath, 0.0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if( NULL != m_UnbiasedLocalCAxisMisalignmentsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    { m_UnbiasedLocalCAxisMisalignments = m_UnbiasedLocalCAxisMisalignmentsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
    // Now we are going to get a "Pointer" to the NeighborList object out of the DataContainer
    m_NeighborList = getDataContainerArray()->getPrereqArrayFromPath<NeighborList<int>, AbstractFilter>(this, getNeighborListArrayPath(), dims);
    m_CAxisMisalignmentList = getDataContainerArray()->getPrereqArrayFromPath<NeighborList<float>, AbstractFilter>(this, getCAxisMisalignmentListArrayPath(), dims);
  }

  if (m_CalcBiasedAvg == true)
  {
    m_AvgCAxisMisalignmentsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getAvgCAxisMisalignmentsArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if( NULL != m_AvgCAxisMisalignmentsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    { m_AvgCAxisMisalignments = m_AvgCAxisMisalignmentsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
    tempPath.update(getNewCellFeatureAttributeMatrixName().getDataContainerName(), getNewCellFeatureAttributeMatrixName().getAttributeMatrixName(), getLocalCAxisMisalignmentsArrayName() );
    m_LocalCAxisMisalignmentsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this, tempPath, 0.0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if( NULL != m_LocalCAxisMisalignmentsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    { m_LocalCAxisMisalignments = m_LocalCAxisMisalignmentsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  }

  m_FeatureParentIdsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getFeatureParentIdsArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeatureParentIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeatureParentIds = m_FeatureParentIdsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  // New Feature Data
  tempPath.update(getNewCellFeatureAttributeMatrixName().getDataContainerName(), getNewCellFeatureAttributeMatrixName().getAttributeMatrixName(), getNumFeaturesPerParentArrayName() );
  m_NumFeaturesPerParentPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter, float>(this, tempPath, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_NumFeaturesPerParentPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_NumFeaturesPerParent = m_NumFeaturesPerParentPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  // Ensemble Data
//typedef DataArray<unsigned int> XTalStructArrayType;
  m_CrystalStructuresPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<unsigned int>, AbstractFilter>(this,  getCrystalStructuresArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CrystalStructuresPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_CrystalStructures = m_CrystalStructuresPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindLocalAverageCAxisMisalignments::preflight()
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
void FindLocalAverageCAxisMisalignments::execute()
{
  setErrorCondition(0);

  dataCheck();
  if(getErrorCondition() < 0) { return; }

  size_t numFeatures = m_FeatureParentIdsPtr.lock()->getNumberOfTuples();
  size_t newNumFeatures = m_NumFeaturesPerParentPtr.lock()->getNumberOfTuples();

  std::vector<int32_t> NumUnbiasedFeaturesPerParent(numFeatures, 0);

  if(m_CalcUnbiasedAvg == true)
  {
    NeighborList<int>& neighborlist = *(m_NeighborList.lock());
    NeighborList<float>& caxismisalignmentList = *(m_CAxisMisalignmentList.lock());
    for(size_t i = 1; i < numFeatures; i++)
    {
      int parentid = m_FeatureParentIds[i];
      for (size_t j = 0; j < neighborlist[i].size(); j++)
      {
        if (m_FeatureParentIds[neighborlist[i][j]] == m_FeatureParentIds[i])
        {
          m_UnbiasedLocalCAxisMisalignments[parentid] += caxismisalignmentList[i][j];
          NumUnbiasedFeaturesPerParent[parentid]++;
        }
      }
    }
  }

  if (m_CalcBiasedAvg == true)
  {
    for(size_t i = 1; i < numFeatures; i++)
    {
      int parentid = m_FeatureParentIds[i];
      m_NumFeaturesPerParent[parentid]++;
      m_LocalCAxisMisalignments[parentid] += m_AvgCAxisMisalignments[i];
    }
  }

  for(size_t i = 1; i < newNumFeatures; i++)
  {
    if (m_CalcBiasedAvg == true) { m_LocalCAxisMisalignments[i] /= m_NumFeaturesPerParent[i]; }
    if (m_CalcUnbiasedAvg == true)
    {
      if(NumUnbiasedFeaturesPerParent[i] > 0) { m_UnbiasedLocalCAxisMisalignments[i] /= NumUnbiasedFeaturesPerParent[i]; }
      else { m_UnbiasedLocalCAxisMisalignments[i] = 0.0f; }
    }
  }

  // If there is an error set this to something negative and also set a message
  notifyStatusMessage(getHumanLabel(), "Complete");
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer FindLocalAverageCAxisMisalignments::newFilterInstance(bool copyFilterParameters)
{
  FindLocalAverageCAxisMisalignments::Pointer filter = FindLocalAverageCAxisMisalignments::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindLocalAverageCAxisMisalignments::getCompiledLibraryName()
{
  return OrientationAnalysisConstants::OrientationAnalysisBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindLocalAverageCAxisMisalignments::getBrandingString()
{
  return "OrientationAnalysis";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindLocalAverageCAxisMisalignments::getFilterVersion()
{
  QString version;
  QTextStream vStream(&version);
  vStream <<  OrientationAnalysis::Version::Major() << "." << OrientationAnalysis::Version::Minor() << "." << OrientationAnalysis::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindLocalAverageCAxisMisalignments::getGroupName()
{ return SIMPL::FilterGroups::StatisticsFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindLocalAverageCAxisMisalignments::getSubGroupName()
{return SIMPL::FilterSubGroups::CrystallographicFilters;}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindLocalAverageCAxisMisalignments::getHumanLabel()
{ return "Find Local Average C-Axis Misalignments"; }

