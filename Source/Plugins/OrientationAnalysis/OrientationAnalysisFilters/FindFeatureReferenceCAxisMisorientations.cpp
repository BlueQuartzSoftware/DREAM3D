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

#include "FindFeatureReferenceCAxisMisorientations.h"

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/Math/GeometryMath.h"
#include "SIMPLib/Geometry/ImageGeom.h"

#include "OrientationLib/OrientationMath/OrientationTransforms.hpp"

#include "OrientationAnalysis/OrientationAnalysisConstants.h"
#include "OrientationAnalysis/OrientationAnalysisVersion.h"

// Include the MOC generated file for this class
#include "moc_FindFeatureReferenceCAxisMisorientations.cpp"



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindFeatureReferenceCAxisMisorientations::FindFeatureReferenceCAxisMisorientations() :
  AbstractFilter(),
  m_FeatureIdsArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellAttributeMatrixName, SIMPL::CellData::FeatureIds),
  m_CellPhasesArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellAttributeMatrixName, SIMPL::CellData::Phases),
  m_AvgCAxesArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellFeatureAttributeMatrixName, SIMPL::FeatureData::AvgCAxes),
  m_QuatsArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellAttributeMatrixName, SIMPL::CellData::Quats),
  m_FeatureAvgCAxisMisorientationsArrayName(SIMPL::FeatureData::FeatureAvgCAxisMisorientations),
  m_FeatureStdevCAxisMisorientationsArrayName(SIMPL::FeatureData::FeatureStdevCAxisMisorientations),
  m_FeatureReferenceCAxisMisorientationsArrayName(SIMPL::CellData::FeatureReferenceCAxisMisorientations),
  m_FeatureIds(NULL),
  m_CellPhases(NULL),
  m_Quats(NULL),
  m_AvgCAxes(NULL),
  m_FeatureReferenceCAxisMisorientations(NULL),
  m_FeatureAvgCAxisMisorientations(NULL),
  m_FeatureStdevCAxisMisorientations(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindFeatureReferenceCAxisMisorientations::~FindFeatureReferenceCAxisMisorientations()
{
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindFeatureReferenceCAxisMisorientations::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(SeparatorFilterParameter::New("Cell Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Int32, 1, SIMPL::AttributeMatrixType::Cell, SIMPL::GeometryType::ImageGeometry);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Feature Ids", FeatureIdsArrayPath, FilterParameter::RequiredArray, FindFeatureReferenceCAxisMisorientations, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Int32, 1, SIMPL::AttributeMatrixType::Cell, SIMPL::GeometryType::ImageGeometry);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Phases", CellPhasesArrayPath, FilterParameter::RequiredArray, FindFeatureReferenceCAxisMisorientations, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Float, 4, SIMPL::AttributeMatrixType::Cell, SIMPL::GeometryType::ImageGeometry);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Quaternions", QuatsArrayPath, FilterParameter::RequiredArray, FindFeatureReferenceCAxisMisorientations, req));
  }
  parameters.push_back(SeparatorFilterParameter::New("Cell Feature Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Float, 3, SIMPL::AttributeMatrixType::CellFeature, SIMPL::GeometryType::ImageGeometry);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Average C-Axes", AvgCAxesArrayPath, FilterParameter::RequiredArray, FindFeatureReferenceCAxisMisorientations, req));
  }
  parameters.push_back(SeparatorFilterParameter::New("Cell Data", FilterParameter::CreatedArray));
  parameters.push_back(SIMPL_NEW_STRING_FP("Average C-Axis Misorientations", FeatureAvgCAxisMisorientationsArrayName, FilterParameter::CreatedArray, FindFeatureReferenceCAxisMisorientations));
  parameters.push_back(SeparatorFilterParameter::New("Cell Feature Data", FilterParameter::CreatedArray));
  parameters.push_back(SIMPL_NEW_STRING_FP("Feature Stdev C-Axis Misorientations", FeatureStdevCAxisMisorientationsArrayName, FilterParameter::CreatedArray, FindFeatureReferenceCAxisMisorientations));
  parameters.push_back(SIMPL_NEW_STRING_FP("Feature Reference C-Axis Misorientations", FeatureReferenceCAxisMisorientationsArrayName, FilterParameter::CreatedArray, FindFeatureReferenceCAxisMisorientations));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
void FindFeatureReferenceCAxisMisorientations::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setFeatureReferenceCAxisMisorientationsArrayName(reader->readString("FeatureReferenceCAxisMisorientationsArrayName", getFeatureReferenceCAxisMisorientationsArrayName() ) );
  setFeatureStdevCAxisMisorientationsArrayName(reader->readString("FeatureStdevCAxisMisorientationsArrayName", getFeatureStdevCAxisMisorientationsArrayName() ) );
  setFeatureAvgCAxisMisorientationsArrayName(reader->readString("FeatureAvgCAxisMisorientationsArrayName", getFeatureAvgCAxisMisorientationsArrayName() ) );
  setQuatsArrayPath(reader->readDataArrayPath("QuatsArrayPath", getQuatsArrayPath() ) );
  setAvgCAxesArrayPath(reader->readDataArrayPath("AvgCAxesArrayPath", getAvgCAxesArrayPath() ) );
  setCellPhasesArrayPath(reader->readDataArrayPath("CellPhasesArrayPath", getCellPhasesArrayPath() ) );
  setFeatureIdsArrayPath(reader->readDataArrayPath("FeatureIdsArrayPath", getFeatureIdsArrayPath() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindFeatureReferenceCAxisMisorientations::initialize()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindFeatureReferenceCAxisMisorientations::dataCheck()
{
  setErrorCondition(0);
  DataArrayPath tempPath;

  getDataContainerArray()->getPrereqGeometryFromDataContainer<ImageGeom, AbstractFilter>(this, getFeatureIdsArrayPath().getDataContainerName());

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

  tempPath.update(m_AvgCAxesArrayPath.getDataContainerName(), getAvgCAxesArrayPath().getAttributeMatrixName(), getFeatureAvgCAxisMisorientationsArrayName() );
  m_FeatureAvgCAxisMisorientationsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this, tempPath, 0, cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeatureAvgCAxisMisorientationsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeatureAvgCAxisMisorientations = m_FeatureAvgCAxisMisorientationsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  tempPath.update(m_AvgCAxesArrayPath.getDataContainerName(), getAvgCAxesArrayPath().getAttributeMatrixName(), getFeatureStdevCAxisMisorientationsArrayName() );
  m_FeatureStdevCAxisMisorientationsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this, tempPath, 0, cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeatureStdevCAxisMisorientationsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeatureStdevCAxisMisorientations = m_FeatureStdevCAxisMisorientationsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  tempPath.update(m_FeatureIdsArrayPath.getDataContainerName(), getFeatureIdsArrayPath().getAttributeMatrixName(), getFeatureReferenceCAxisMisorientationsArrayName() );
  m_FeatureReferenceCAxisMisorientationsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this, tempPath, 0, cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeatureReferenceCAxisMisorientationsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeatureReferenceCAxisMisorientations = m_FeatureReferenceCAxisMisorientationsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  cDims[0] = 3;
  m_AvgCAxesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getAvgCAxesArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_AvgCAxesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_AvgCAxes = m_AvgCAxesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  cDims[0] = 4;
  m_QuatsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getQuatsArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_QuatsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_Quats = m_QuatsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCondition() >= 0) { dataArrayPaths.push_back(getQuatsArrayPath()); }

  getDataContainerArray()->validateNumberOfTuples<AbstractFilter>(this, dataArrayPaths);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindFeatureReferenceCAxisMisorientations::preflight()
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
void FindFeatureReferenceCAxisMisorientations::execute()
{
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(m_FeatureIdsArrayPath.getDataContainerName());
  size_t totalPoints = m_FeatureIdsPtr.lock()->getNumberOfTuples();
  size_t totalFeatures = m_AvgCAxesPtr.lock()->getNumberOfTuples();

  int32_t avgMisoComps = 3;
  QVector<size_t> dims(1, avgMisoComps);
  FloatArrayType::Pointer avgmisoPtr = FloatArrayType::CreateArray(totalFeatures, dims, "_INTERNAL_USE_ONLY_AvgMiso_Temp");
  avgmisoPtr->initializeWithZeros();
  float* avgmiso = avgmisoPtr->getPointer(0);

  QuatF q1 = QuaternionMathF::New();
  QuatF* quats = reinterpret_cast<QuatF*>(m_Quats);

  float w = 0.0f;
  size_t udims[3] = { 0, 0, 0 };
  m->getGeometryAs<ImageGeom>()->getDimensions(udims);

  uint32_t maxUInt32 = std::numeric_limits<uint32_t>::max();
  // We have more points than can be allocated on a 32 bit machine. Assert Now.
  if(totalPoints > maxUInt32)
  {
    QString ss = QObject::tr("The volume is too large for a 32 bit machine. Try reducing the input volume size. Total Voxels: %1").arg(totalPoints);
    notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);
    return;
  }

  int64_t xPoints = static_cast<int64_t>(udims[0]);
  int64_t yPoints = static_cast<int64_t>(udims[1]);
  int64_t zPoints = static_cast<int64_t>(udims[2]);
  int64_t point = 0;

  float g1[3][3] = { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } };
  float g1t[3][3] = { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } };
  float caxis[3] = {0.0f, 0.0f, 1.0f};
  float c1[3] = {0.0f, 0.0f, 0.0f};
  float AvgCAxis[3] = {0.0f, 0.0f, 0.0f};
  size_t index = 0;

  for (int64_t col = 0; col < xPoints; col++)
  {
    for (int64_t row = 0; row < yPoints; row++)
    {
      for (int64_t plane = 0; plane < zPoints; plane++)
      {
        point = (plane * xPoints * yPoints) + (row * xPoints) + col;
        if (m_FeatureIds[point] > 0 && m_CellPhases[point] > 0)
        {
          QuaternionMathF::Copy(quats[point], q1);
          FOrientArrayType om(9);
          FOrientTransformsType::qu2om(FOrientArrayType(q1), om);
          om.toGMatrix(g1);
          // transpose the g matricies so when caxis is multiplied by it
          // it will give the sample direction that the caxis is along
          MatrixMath::Transpose3x3(g1, g1t);
          MatrixMath::Multiply3x3with3x1(g1t, caxis, c1);
          // normalize so that the magnitude is 1
          MatrixMath::Normalize3x1(c1);

          AvgCAxis[0] = m_AvgCAxes[3 * m_FeatureIds[point]];
          AvgCAxis[1] = m_AvgCAxes[3 * m_FeatureIds[point] + 1];
          AvgCAxis[2] = m_AvgCAxes[3 * m_FeatureIds[point] + 2];
          // normalize so that the magnitude is 1
          MatrixMath::Normalize3x1(AvgCAxis);
          w = GeometryMath::CosThetaBetweenVectors(c1, AvgCAxis);
          SIMPLibMath::boundF(w, -1, 1);
          w = acosf(w);
          w = w * SIMPLib::Constants::k_180OverPi;
          if (w > 90.0) { w = 180.0 - w; }

          m_FeatureReferenceCAxisMisorientations[point] = w;
          index = m_FeatureIds[point] * avgMisoComps;
          avgmiso[index]++;
          avgmiso[index + 1] += w;
        }
        if (m_FeatureIds[point] == 0 || m_CellPhases[point] == 0)
        {
          m_FeatureReferenceCAxisMisorientations[point] = 0;
        }
      }
    }
  }

  for (size_t i = 1; i < totalFeatures; i++)
  {
    if (i % 1000 == 0)
    {
      QString ss = QObject::tr("Working On Feature %1 of %2").arg(i).arg(totalFeatures);
      notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);
    }
    index = i * avgMisoComps;
    m_FeatureAvgCAxisMisorientations[i] = avgmiso[index + 1] / avgmiso[index];
    if (avgmiso[index] == 0) { m_FeatureAvgCAxisMisorientations[i] = 0.0; }
  }


  int32_t gNum = 0;
  for (size_t j = 0; j < totalPoints; j++)
  {
    gNum = m_FeatureIds[j];
    avgmiso[(gNum * avgMisoComps) + 2] += ((m_FeatureReferenceCAxisMisorientations[j] - m_FeatureAvgCAxisMisorientations[gNum]) * (m_FeatureReferenceCAxisMisorientations[j] - m_FeatureAvgCAxisMisorientations[gNum]));
  }

  for (size_t i = 1; i < totalFeatures; i++)
  {
    index = i * avgMisoComps;
    m_FeatureStdevCAxisMisorientations[i] = sqrtf((1 / avgmiso[index]) * avgmiso[index + 2]);
  }

  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer FindFeatureReferenceCAxisMisorientations::newFilterInstance(bool copyFilterParameters)
{
  FindFeatureReferenceCAxisMisorientations::Pointer filter = FindFeatureReferenceCAxisMisorientations::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindFeatureReferenceCAxisMisorientations::getCompiledLibraryName()
{
  return OrientationAnalysisConstants::OrientationAnalysisBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindFeatureReferenceCAxisMisorientations::getBrandingString()
{
  return "OrientationAnalysis";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindFeatureReferenceCAxisMisorientations::getFilterVersion()
{
  QString version;
  QTextStream vStream(&version);
  vStream <<  OrientationAnalysis::Version::Major() << "." << OrientationAnalysis::Version::Minor() << "." << OrientationAnalysis::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindFeatureReferenceCAxisMisorientations::getGroupName()
{ return SIMPL::FilterGroups::StatisticsFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindFeatureReferenceCAxisMisorientations::getSubGroupName()
{ return SIMPL::FilterSubGroups::CrystallographicFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindFeatureReferenceCAxisMisorientations::getHumanLabel()
{ return "Find Feature Reference C-Axis Misalignments"; }
