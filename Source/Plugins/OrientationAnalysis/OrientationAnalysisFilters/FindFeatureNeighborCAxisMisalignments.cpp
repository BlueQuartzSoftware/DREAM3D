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

#include "FindFeatureNeighborCAxisMisalignments.h"

#include <cmath>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedBooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/Math/GeometryMath.h"

#include "OrientationLib/OrientationMath/OrientationTransforms.hpp"

#include "OrientationAnalysis/OrientationAnalysisConstants.h"
#include "OrientationAnalysis/OrientationAnalysisVersion.h"

#include "EbsdLib/EbsdConstants.h"

// Include the MOC generated file for this class
#include "moc_FindFeatureNeighborCAxisMisalignments.cpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindFeatureNeighborCAxisMisalignments::FindFeatureNeighborCAxisMisalignments()
: AbstractFilter()
, m_FindAvgMisals(false)
, m_NeighborListArrayPath("", "", "")
, m_CAxisMisalignmentListArrayName(SIMPL::FeatureData::CAxisMisalignmentList)
, m_AvgQuatsArrayPath("", "", "")
, m_FeaturePhasesArrayPath("", "", "")
, m_CrystalStructuresArrayPath("", "", "")
, m_AvgCAxisMisalignmentsArrayName(SIMPL::FeatureData::AvgCAxisMisalignments)
, m_AvgQuats(nullptr)
, m_FeaturePhases(nullptr)
, m_CrystalStructures(nullptr)
, m_AvgCAxisMisalignments(nullptr)
{
  m_OrientationOps = LaueOps::getOrientationOpsQVector();

  m_NeighborList = NeighborList<int32_t>::NullPointer();
  m_CAxisMisalignmentList = NeighborList<float>::NullPointer();

  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindFeatureNeighborCAxisMisalignments::~FindFeatureNeighborCAxisMisalignments()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindFeatureNeighborCAxisMisalignments::setupFilterParameters()
{
  FilterParameterVector parameters;
  QStringList linkedProps("AvgCAxisMisalignmentsArrayName");
  parameters.push_back(SIMPL_NEW_LINKED_BOOL_FP("Find Average Misalignment Per Feature", FindAvgMisals, FilterParameter::Parameter, FindFeatureNeighborCAxisMisalignments, linkedProps));
  parameters.push_back(SeparatorFilterParameter::New("Feature Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateCategoryRequirement(SIMPL::TypeNames::NeighborList, 1, AttributeMatrix::Category::Feature);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Neighbor List", NeighborListArrayPath, FilterParameter::RequiredArray, FindFeatureNeighborCAxisMisalignments, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateCategoryRequirement(SIMPL::TypeNames::Float, 4, AttributeMatrix::Category::Feature);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Average Quaternions", AvgQuatsArrayPath, FilterParameter::RequiredArray, FindFeatureNeighborCAxisMisalignments, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateCategoryRequirement(SIMPL::TypeNames::Int32, 1, AttributeMatrix::Category::Feature);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Phases", FeaturePhasesArrayPath, FilterParameter::RequiredArray, FindFeatureNeighborCAxisMisalignments, req));
  }
  parameters.push_back(SeparatorFilterParameter::New("Ensemble Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateCategoryRequirement(SIMPL::TypeNames::UInt32, 1, AttributeMatrix::Category::Ensemble);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Crystal Structures", CrystalStructuresArrayPath, FilterParameter::RequiredArray, FindFeatureNeighborCAxisMisalignments, req));
  }
  parameters.push_back(SeparatorFilterParameter::New("Feature Data", FilterParameter::CreatedArray));
  parameters.push_back(SIMPL_NEW_STRING_FP("C-Axis Misalignment List", CAxisMisalignmentListArrayName, FilterParameter::CreatedArray, FindFeatureNeighborCAxisMisalignments));
  parameters.push_back(SIMPL_NEW_STRING_FP("Avgerage C-Axis Misalignments", AvgCAxisMisalignmentsArrayName, FilterParameter::CreatedArray, FindFeatureNeighborCAxisMisalignments));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindFeatureNeighborCAxisMisalignments::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setNeighborListArrayPath(reader->readDataArrayPath("NeighborListArrayPath", getNeighborListArrayPath()));
  setCAxisMisalignmentListArrayName(reader->readString("CAxisMisalignmentListArrayName", getCAxisMisalignmentListArrayName()));
  setAvgCAxisMisalignmentsArrayName(reader->readString("AvgCAxisMisalignmentsArrayName", getAvgCAxisMisalignmentsArrayName()));
  setCrystalStructuresArrayPath(reader->readDataArrayPath("CrystalStructuresArrayPath", getCrystalStructuresArrayPath()));
  setFeaturePhasesArrayPath(reader->readDataArrayPath("FeaturePhasesArrayPath", getFeaturePhasesArrayPath()));
  setAvgQuatsArrayPath(reader->readDataArrayPath("AvgQuatsArrayPath", getAvgQuatsArrayPath()));
  setFindAvgMisals(reader->readValue("FindAvgMisals", getFindAvgMisals()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindFeatureNeighborCAxisMisalignments::initialize()
{
  m_NeighborList = NeighborList<int32_t>::NullPointer();
  m_CAxisMisalignmentList = NeighborList<float>::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindFeatureNeighborCAxisMisalignments::dataCheck()
{
  setErrorCondition(0);
  setWarningCondition(0);
  initialize();

  DataArrayPath tempPath;

  QVector<DataArrayPath> dataArrayPaths;

  QVector<size_t> cDims(1, 4);
  // Feature Data
  m_AvgQuatsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getAvgQuatsArrayPath(),
                                                                                                    cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_AvgQuatsPtr.lock().get())                                                                 /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_AvgQuats = m_AvgQuatsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCondition() >= 0)
  {
    dataArrayPaths.push_back(getAvgQuatsArrayPath());
  }

  cDims[0] = 1;
  m_FeaturePhasesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getFeaturePhasesArrayPath(),
                                                                                                           cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_FeaturePhasesPtr.lock().get())                                                                   /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_FeaturePhases = m_FeaturePhasesPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCondition() >= 0)
  {
    dataArrayPaths.push_back(getFeaturePhasesArrayPath());
  }

  // New Feature Data
  if(m_FindAvgMisals == true)
  {
    tempPath.update(m_FeaturePhasesArrayPath.getDataContainerName(), getFeaturePhasesArrayPath().getAttributeMatrixName(), getAvgCAxisMisalignmentsArrayName());
    m_AvgCAxisMisalignmentsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(
        this, tempPath, 0, cDims);                         /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if(nullptr != m_AvgCAxisMisalignmentsPtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
    {
      m_AvgCAxisMisalignments = m_AvgCAxisMisalignmentsPtr.lock()->getPointer(0);
    } /* Now assign the raw pointer to data from the DataArray<T> object */
  }

  // Ensemble Data
  m_CrystalStructuresPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<uint32_t>, AbstractFilter>(this, getCrystalStructuresArrayPath(),
                                                                                                                cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_CrystalStructuresPtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_CrystalStructures = m_CrystalStructuresPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  // Now we are going to get a "Pointer" to the NeighborList object out of the DataContainer
  m_NeighborList = getDataContainerArray()->getPrereqArrayFromPath<NeighborList<int32_t>, AbstractFilter>(this, getNeighborListArrayPath(), cDims);
  if(getErrorCondition() >= 0)
  {
    dataArrayPaths.push_back(getNeighborListArrayPath());
  }

  tempPath.update(m_NeighborListArrayPath.getDataContainerName(), getNeighborListArrayPath().getAttributeMatrixName(), getCAxisMisalignmentListArrayName());
  m_CAxisMisalignmentList = getDataContainerArray()->createNonPrereqArrayFromPath<NeighborList<float>, AbstractFilter, float>(
      this, tempPath, 0, cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */

  getDataContainerArray()->validateNumberOfTuples<AbstractFilter>(this, dataArrayPaths);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindFeatureNeighborCAxisMisalignments::preflight()
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
void FindFeatureNeighborCAxisMisalignments::execute()
{
  setErrorCondition(0);
  setWarningCondition(0);
  dataCheck();
  if(getErrorCondition() < 0)
  {
    return;
  }

  size_t totalFeatures = m_FeaturePhasesPtr.lock()->getNumberOfTuples();

  // But since a pointer is difficult to use operators with we will now create a
  // reference variable to the pointer with the correct variable name that allows
  // us to use the same syntax as the "vector of vectors"
  NeighborList<int32_t>& neighborlist = *(m_NeighborList.lock());

  std::vector<std::vector<float>> misalignmentlists;

  float w = 0.0f;
  float g1[3][3] = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};
  float g2[3][3] = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};
  float g1t[3][3] = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};
  float g2t[3][3] = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};
  float c1[3] = {0.0f, 0.0f, 0.0f};
  float c2[3] = {0.0f, 0.0f, 0.0f};
  float caxis[3] = {0.0f, 0.0f, 1.0f};
  size_t hexneighborlistsize = 0;
  QuatF q1 = QuaternionMathF::New();
  QuatF q2 = QuaternionMathF::New();
  QuatF* avgQuats = reinterpret_cast<QuatF*>(m_AvgQuats);

  uint32_t phase1 = 0, phase2 = 0;
  size_t nname = 0;
  misalignmentlists.resize(totalFeatures);

  for(size_t i = 1; i < totalFeatures; i++)
  {
    QuaternionMathF::Copy(avgQuats[i], q1);
    phase1 = m_CrystalStructures[m_FeaturePhases[i]];
    FOrientArrayType om(9);
    FOrientTransformsType::qu2om(FOrientArrayType(q1), om);
    om.toGMatrix(g1);
    // transpose the g matrix so when caxis is multiplied by it
    // it will give the sample direction that the caxis is along
    MatrixMath::Transpose3x3(g1, g1t);
    MatrixMath::Multiply3x3with3x1(g1t, caxis, c1);
    // normalize so that the dot product can be taken below without
    // dividing by the magnitudes (they would be 1)
    MatrixMath::Normalize3x1(c1);
    misalignmentlists[i].resize(neighborlist[i].size(), -1.0f);
    for(size_t j = 0; j < neighborlist[i].size(); j++)
    {
      w = std::numeric_limits<float>::max();
      nname = neighborlist[i][j];
      phase2 = m_CrystalStructures[m_FeaturePhases[nname]];
      hexneighborlistsize = neighborlist[i].size();
      if(phase1 == phase2 && (phase1 == Ebsd::CrystalStructure::Hexagonal_High))
      {
        QuaternionMathF::Copy(avgQuats[nname], q2);
        FOrientTransformsType::qu2om(FOrientArrayType(q2), om);
        om.toGMatrix(g2);
        // transpose the g matrix so when caxis is multiplied by it
        // it will give the sample direction that the caxis is along
        MatrixMath::Transpose3x3(g2, g2t);
        MatrixMath::Multiply3x3with3x1(g2t, caxis, c2);
        // normalize so that the dot product can be taken below without
        // dividing by the magnitudes (they would be 1)
        MatrixMath::Normalize3x1(c2);

        w = GeometryMath::CosThetaBetweenVectors(c1, c2);
        SIMPLibMath::boundF(w, -1, 1);
        w = acosf(w);
        if(w > (SIMPLib::Constants::k_Pi / 2))
        {
          w = SIMPLib::Constants::k_Pi - w;
        }

        misalignmentlists[i][j] = w * SIMPLib::Constants::k_180OverPi;
        if(m_FindAvgMisals == true)
        {
          m_AvgCAxisMisalignments[i] += misalignmentlists[i][j];
        }
      }
      else
      {
        if(m_FindAvgMisals == true)
        {
          hexneighborlistsize--;
        }
        misalignmentlists[i][j] = NAN;
      }
    }
    if(m_FindAvgMisals == true)
    {
      if(hexneighborlistsize > 0)
      {
        m_AvgCAxisMisalignments[i] /= hexneighborlistsize;
      }
      else
      {
        m_AvgCAxisMisalignments[i] = NAN;
      }
      hexneighborlistsize = 0;
    }
  }

  for(size_t i = 1; i < totalFeatures; i++)
  {
    // Set the vector for each list into the NeighborList Object
    NeighborList<float>::SharedVectorType misaL(new std::vector<float>);
    misaL->assign(misalignmentlists[i].begin(), misalignmentlists[i].end());
    m_CAxisMisalignmentList.lock()->setList(static_cast<int32_t>(i), misaL);
  }

  notifyStatusMessage(getHumanLabel(), "Complete");
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer FindFeatureNeighborCAxisMisalignments::newFilterInstance(bool copyFilterParameters)
{
  FindFeatureNeighborCAxisMisalignments::Pointer filter = FindFeatureNeighborCAxisMisalignments::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindFeatureNeighborCAxisMisalignments::getCompiledLibraryName()
{
  return OrientationAnalysisConstants::OrientationAnalysisBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindFeatureNeighborCAxisMisalignments::getBrandingString()
{
  return "OrientationAnalysis";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindFeatureNeighborCAxisMisalignments::getFilterVersion()
{
  QString version;
  QTextStream vStream(&version);
  vStream << OrientationAnalysis::Version::Major() << "." << OrientationAnalysis::Version::Minor() << "." << OrientationAnalysis::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindFeatureNeighborCAxisMisalignments::getGroupName()
{
  return SIMPL::FilterGroups::StatisticsFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindFeatureNeighborCAxisMisalignments::getSubGroupName()
{
  return SIMPL::FilterSubGroups::CrystallographicFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindFeatureNeighborCAxisMisalignments::getHumanLabel()
{
  return "Find Feature Neighbor C-Axis Misalignments";
}
