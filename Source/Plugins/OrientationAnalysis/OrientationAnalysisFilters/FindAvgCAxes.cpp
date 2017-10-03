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

#include "FindAvgCAxes.h"

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/DataArrayCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/Math/GeometryMath.h"

#include "OrientationLib/OrientationMath/OrientationTransforms.hpp"

#include "OrientationAnalysis/OrientationAnalysisConstants.h"
#include "OrientationAnalysis/OrientationAnalysisVersion.h"

// Include the MOC generated file for this class
#include "moc_FindAvgCAxes.cpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindAvgCAxes::FindAvgCAxes()
: AbstractFilter()
, m_QuatsArrayPath("", "", "")
, m_FeatureIdsArrayPath("", "", "")
, m_AvgCAxesArrayPath("", "", "")
, m_FeatureIds(nullptr)
, m_Quats(nullptr)
, m_AvgCAxes(nullptr)
{
  m_OrientationOps = LaueOps::getOrientationOpsQVector();

  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindAvgCAxes::~FindAvgCAxes()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindAvgCAxes::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(SeparatorFilterParameter::New("Element Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateCategoryRequirement(SIMPL::TypeNames::Float, 4, AttributeMatrix::Category::Element);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Quaternions", QuatsArrayPath, FilterParameter::RequiredArray, FindAvgCAxes, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateCategoryRequirement(SIMPL::TypeNames::Int32, 1, AttributeMatrix::Category::Element);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Feature Ids", FeatureIdsArrayPath, FilterParameter::RequiredArray, FindAvgCAxes, req));
  }
  parameters.push_back(SeparatorFilterParameter::New("Feature Data", FilterParameter::CreatedArray));
  {
    DataArrayCreationFilterParameter::RequirementType req = DataArrayCreationFilterParameter::CreateRequirement(AttributeMatrix::Category::Feature);
    req.dcGeometryTypes = IGeometry::Types(1, IGeometry::Type::Image);
    parameters.push_back(SIMPL_NEW_DA_CREATION_FP("Average C-Axes", AvgCAxesArrayPath, FilterParameter::CreatedArray, FindAvgCAxes, req));
  }
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindAvgCAxes::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setAvgCAxesArrayPath(reader->readDataArrayPath("AvgCAxesArrayPath", getAvgCAxesArrayPath()));
  setFeatureIdsArrayPath(reader->readDataArrayPath("FeatureIdsArrayPath", getFeatureIdsArrayPath()));
  setQuatsArrayPath(reader->readDataArrayPath("QuatsArrayPath", getQuatsArrayPath()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindAvgCAxes::initialize()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindAvgCAxes::dataCheck()
{
  setErrorCondition(0);
  setWarningCondition(0);

  QVector<DataArrayPath> dataArrayPaths;

  QVector<size_t> cDims(1, 4);
  m_QuatsPtr =
      getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getQuatsArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_QuatsPtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_Quats = m_QuatsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCondition() >= 0)
  {
    dataArrayPaths.push_back(getQuatsArrayPath());
  }

  cDims[0] = 1;
  m_FeatureIdsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getFeatureIdsArrayPath(),
                                                                                                        cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_FeatureIdsPtr.lock().get())                                                                   /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCondition() >= 0)
  {
    dataArrayPaths.push_back(getFeatureIdsArrayPath());
  }

  cDims[0] = 3;
  m_AvgCAxesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this, getAvgCAxesArrayPath(), 0,
                                                                                                                 cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_AvgCAxesPtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_AvgCAxes = m_AvgCAxesPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  getDataContainerArray()->validateNumberOfTuples<AbstractFilter>(this, dataArrayPaths);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindAvgCAxes::preflight()
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
void FindAvgCAxes::execute()
{
  setErrorCondition(0);
  setWarningCondition(0);
  dataCheck();
  if(getErrorCondition() < 0)
  {
    return;
  }

  size_t totalPoints = m_FeatureIdsPtr.lock()->getNumberOfTuples();
  size_t totalFeatures = m_AvgCAxesPtr.lock()->getNumberOfTuples();

  QuatF q1 = QuaternionMathF::New();
  QuatF* quats = reinterpret_cast<QuatF*>(m_Quats);
  float g1[3][3] = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};
  float g1t[3][3] = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};
  float caxis[3] = {0.0f, 0.0f, 1.0f};
  float c1[3] = {0.0f, 0.0f, 0.0f};

  std::vector<int32_t> counter(totalFeatures, 0);

  float curCAxis[3] = {0.0f, 0.0f, 0.0f};
  size_t index = 0;
  float w = 0.0f;
  for(size_t i = 0; i < totalPoints; i++)
  {
    if(m_FeatureIds[i] > 0)
    {
      index = 3 * m_FeatureIds[i];
      QuaternionMathF::Copy(quats[i], q1);

      FOrientArrayType om(9);
      FOrientTransformsType::qu2om(FOrientArrayType(q1), om);
      om.toGMatrix(g1);
      // transpose the g matricies so when caxis is multiplied by it
      // it will give the sample direction that the caxis is along
      MatrixMath::Transpose3x3(g1, g1t);
      MatrixMath::Multiply3x3with3x1(g1t, caxis, c1);
      // normalize so that the magnitude is 1
      MatrixMath::Normalize3x1(c1);
      curCAxis[0] = m_AvgCAxes[index] / counter[m_FeatureIds[i]];
      curCAxis[1] = m_AvgCAxes[index + 1] / counter[m_FeatureIds[i]];
      curCAxis[2] = m_AvgCAxes[index + 2] / counter[m_FeatureIds[i]];
      MatrixMath::Normalize3x1(curCAxis);
      w = GeometryMath::CosThetaBetweenVectors(c1, curCAxis);
      if(w < 0)
      {
        MatrixMath::Multiply3x1withConstant(c1, -1);
      }
      counter[m_FeatureIds[i]]++;
      m_AvgCAxes[index] += c1[0];
      m_AvgCAxes[index + 1] += c1[1];
      m_AvgCAxes[index + 2] += c1[2];
    }
  }

  for(size_t i = 1; i < totalFeatures; i++)
  {
    if(counter[i] == 0)
    {
      m_AvgCAxes[3 * i] = 0;
      m_AvgCAxes[3 * i + 1] = 0;
      m_AvgCAxes[3 * i + 2] = 1;
    }
    else
    {
      m_AvgCAxes[3 * i] /= counter[i];
      m_AvgCAxes[3 * i + 1] /= counter[i];
      m_AvgCAxes[3 * i + 2] /= counter[i];
    }
  }

  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer FindAvgCAxes::newFilterInstance(bool copyFilterParameters)
{
  FindAvgCAxes::Pointer filter = FindAvgCAxes::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindAvgCAxes::getCompiledLibraryName()
{
  return OrientationAnalysisConstants::OrientationAnalysisBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindAvgCAxes::getBrandingString()
{
  return "OrientationAnalysis";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindAvgCAxes::getFilterVersion()
{
  QString version;
  QTextStream vStream(&version);
  vStream << OrientationAnalysis::Version::Major() << "." << OrientationAnalysis::Version::Minor() << "." << OrientationAnalysis::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindAvgCAxes::getGroupName()
{
  return SIMPL::FilterGroups::StatisticsFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindAvgCAxes::getSubGroupName()
{
  return SIMPL::FilterSubGroups::CrystallographyFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindAvgCAxes::getHumanLabel()
{
  return "Find Average C-Axis Orientations";
}
