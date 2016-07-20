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

#include "FindFeatureReferenceMisorientations.h"

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedChoicesFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/Geometry/ImageGeom.h"


#include "OrientationAnalysis/OrientationAnalysisConstants.h"
#include "OrientationAnalysis/OrientationAnalysisVersion.h"

#include "EbsdLib/EbsdConstants.h"

// Include the MOC generated file for this class
#include "moc_FindFeatureReferenceMisorientations.cpp"



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindFeatureReferenceMisorientations::FindFeatureReferenceMisorientations() :
  AbstractFilter(),
  m_FeatureIdsArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellAttributeMatrixName, SIMPL::CellData::FeatureIds),
  m_CellPhasesArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellAttributeMatrixName, SIMPL::CellData::Phases),
  m_CrystalStructuresArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellEnsembleAttributeMatrixName, SIMPL::EnsembleData::CrystalStructures),
  m_QuatsArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellAttributeMatrixName, SIMPL::CellData::Quats),
  m_AvgQuatsArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellFeatureAttributeMatrixName, SIMPL::FeatureData::AvgQuats),
  m_GBEuclideanDistancesArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellAttributeMatrixName, SIMPL::CellData::GBEuclideanDistances),
  m_FeatureAvgMisorientationsArrayName(SIMPL::FeatureData::FeatureAvgMisorientations),
  m_FeatureReferenceMisorientationsArrayName(SIMPL::CellData::FeatureReferenceMisorientations),
  m_ReferenceOrientation(0),
  m_FeatureIds(NULL),
  m_CellPhases(NULL),
  m_GBEuclideanDistances(NULL),
  m_Quats(NULL),
  m_CrystalStructures(NULL),
  m_FeatureReferenceMisorientations(NULL),
  m_FeatureAvgMisorientations(NULL)
{
  m_OrientationOps = SpaceGroupOps::getOrientationOpsQVector();

  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindFeatureReferenceMisorientations::~FindFeatureReferenceMisorientations()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindFeatureReferenceMisorientations::setupFilterParameters()
{
  FilterParameterVector parameters;

  {
    LinkedChoicesFilterParameter::Pointer parameter = LinkedChoicesFilterParameter::New();
    parameter->setHumanLabel("Reference Orientation");
    parameter->setPropertyName("ReferenceOrientation");
    parameter->setSetterCallback(SIMPL_BIND_SETTER(FindFeatureReferenceMisorientations, this, ReferenceOrientation));
    parameter->setGetterCallback(SIMPL_BIND_GETTER(FindFeatureReferenceMisorientations, this, ReferenceOrientation));

    QVector<QString> choices;
    choices.push_back("Average Orientation");
    choices.push_back("Orientation at Feature Centroid");
    parameter->setChoices(choices);
    QStringList linkedProps;
    linkedProps << "GBEuclideanDistancesArrayPath" << "AvgQuatsArrayPath";
    parameter->setLinkedProperties(linkedProps);
    parameter->setEditable(false);
    parameter->setCategory(FilterParameter::Parameter);
    parameters.push_back(parameter);
  }
  parameters.push_back(SeparatorFilterParameter::New("Cell Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Int32, 1, SIMPL::AttributeMatrixType::Cell, SIMPL::GeometryType::ImageGeometry);
    parameters.push_back(DataArraySelectionFilterParameter::New("Feature Ids", "FeatureIdsArrayPath", getFeatureIdsArrayPath(), FilterParameter::RequiredArray, req, SIMPL_BIND_SETTER(FindFeatureReferenceMisorientations, this, FeatureIdsArrayPath), SIMPL_BIND_GETTER(FindFeatureReferenceMisorientations, this, FeatureIdsArrayPath)));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Int32, 1, SIMPL::AttributeMatrixType::Cell, SIMPL::GeometryType::ImageGeometry);
    parameters.push_back(DataArraySelectionFilterParameter::New("Phases", "CellPhasesArrayPath", getCellPhasesArrayPath(), FilterParameter::RequiredArray, req, SIMPL_BIND_SETTER(FindFeatureReferenceMisorientations, this, CellPhasesArrayPath), SIMPL_BIND_GETTER(FindFeatureReferenceMisorientations, this, CellPhasesArrayPath)));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Float, 4, SIMPL::AttributeMatrixType::Cell, SIMPL::GeometryType::ImageGeometry);
    parameters.push_back(DataArraySelectionFilterParameter::New("Quaternions", "QuatsArrayPath", getQuatsArrayPath(), FilterParameter::RequiredArray, req, SIMPL_BIND_SETTER(FindFeatureReferenceMisorientations, this, QuatsArrayPath), SIMPL_BIND_GETTER(FindFeatureReferenceMisorientations, this, QuatsArrayPath)));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Float, 1, SIMPL::AttributeMatrixType::Cell, SIMPL::GeometryType::ImageGeometry);
    parameters.push_back(DataArraySelectionFilterParameter::New("Boundary Euclidean Distances", "GBEuclideanDistancesArrayPath", getGBEuclideanDistancesArrayPath(), FilterParameter::RequiredArray, req, SIMPL_BIND_SETTER(FindFeatureReferenceMisorientations, this, GBEuclideanDistancesArrayPath), SIMPL_BIND_GETTER(FindFeatureReferenceMisorientations, this, GBEuclideanDistancesArrayPath), 1));
  }
  parameters.push_back(SeparatorFilterParameter::New("Cell Feature Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Float, 4, SIMPL::AttributeMatrixType::CellFeature, SIMPL::GeometryType::ImageGeometry);
    parameters.push_back(DataArraySelectionFilterParameter::New("Average Quaternions", "AvgQuatsArrayPath", getAvgQuatsArrayPath(), FilterParameter::RequiredArray, req, SIMPL_BIND_SETTER(FindFeatureReferenceMisorientations, this, AvgQuatsArrayPath), SIMPL_BIND_GETTER(FindFeatureReferenceMisorientations, this, AvgQuatsArrayPath), 0));
  }
  parameters.push_back(SeparatorFilterParameter::New("Cell Ensemble Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::UInt32, 1, SIMPL::AttributeMatrixType::CellEnsemble, SIMPL::GeometryType::ImageGeometry);
    parameters.push_back(DataArraySelectionFilterParameter::New("Crystal Structures", "CrystalStructuresArrayPath", getCrystalStructuresArrayPath(), FilterParameter::RequiredArray, req, SIMPL_BIND_SETTER(FindFeatureReferenceMisorientations, this, CrystalStructuresArrayPath), SIMPL_BIND_GETTER(FindFeatureReferenceMisorientations, this, CrystalStructuresArrayPath)));
  }
  parameters.push_back(SeparatorFilterParameter::New("Cell Data", FilterParameter::CreatedArray));
  parameters.push_back(StringFilterParameter::New("Feature Reference Misorientations", "FeatureReferenceMisorientationsArrayName", getFeatureReferenceMisorientationsArrayName(), FilterParameter::CreatedArray, SIMPL_BIND_SETTER(FindFeatureReferenceMisorientations, this, FeatureReferenceMisorientationsArrayName), SIMPL_BIND_GETTER(FindFeatureReferenceMisorientations, this, FeatureReferenceMisorientationsArrayName)));
  parameters.push_back(SeparatorFilterParameter::New("Cell Feature Data", FilterParameter::CreatedArray));
  parameters.push_back(StringFilterParameter::New("Average Misorientations", "FeatureAvgMisorientationsArrayName", getFeatureAvgMisorientationsArrayName(), FilterParameter::CreatedArray, SIMPL_BIND_SETTER(FindFeatureReferenceMisorientations, this, FeatureAvgMisorientationsArrayName), SIMPL_BIND_GETTER(FindFeatureReferenceMisorientations, this, FeatureAvgMisorientationsArrayName)));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
void FindFeatureReferenceMisorientations::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setFeatureReferenceMisorientationsArrayName(reader->readString("FeatureReferenceMisorientationsArrayName", getFeatureReferenceMisorientationsArrayName() ) );
  setFeatureAvgMisorientationsArrayName(reader->readString("FeatureAvgMisorientationsArrayName", getFeatureAvgMisorientationsArrayName() ) );
  setGBEuclideanDistancesArrayPath(reader->readDataArrayPath("GBEuclideanDistancesArrayPath", getGBEuclideanDistancesArrayPath() ) );
  setAvgQuatsArrayPath(reader->readDataArrayPath("AvgQuatsArrayPath", getAvgQuatsArrayPath() ) );
  setQuatsArrayPath(reader->readDataArrayPath("QuatsArrayPath", getQuatsArrayPath() ) );
  setCrystalStructuresArrayPath(reader->readDataArrayPath("CrystalStructuresArrayPath", getCrystalStructuresArrayPath() ) );
  setCellPhasesArrayPath(reader->readDataArrayPath("CellPhasesArrayPath", getCellPhasesArrayPath() ) );
  setFeatureIdsArrayPath(reader->readDataArrayPath("FeatureIdsArrayPath", getFeatureIdsArrayPath() ) );
  setReferenceOrientation( reader->readValue("ReferenceOrientation", getReferenceOrientation()) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindFeatureReferenceMisorientations::initialize()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindFeatureReferenceMisorientations::dataCheck()
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

  tempPath.update(m_AvgQuatsArrayPath.getDataContainerName(), m_AvgQuatsArrayPath.getAttributeMatrixName(), getFeatureAvgMisorientationsArrayName() );
  m_FeatureAvgMisorientationsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this, tempPath, 0, cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeatureAvgMisorientationsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeatureAvgMisorientations = m_FeatureAvgMisorientationsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  tempPath.update(m_FeatureIdsArrayPath.getDataContainerName(), m_FeatureIdsArrayPath.getAttributeMatrixName(), getFeatureReferenceMisorientationsArrayName() );
  m_FeatureReferenceMisorientationsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this, tempPath, 0, cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeatureReferenceMisorientationsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeatureReferenceMisorientations = m_FeatureReferenceMisorientationsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  m_CrystalStructuresPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<uint32_t>, AbstractFilter>(this, getCrystalStructuresArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CrystalStructuresPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_CrystalStructures = m_CrystalStructuresPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  cDims[0] = 4;
  m_QuatsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getQuatsArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_QuatsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_Quats = m_QuatsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCondition() >= 0) { dataArrayPaths.push_back(getQuatsArrayPath()); }

  if (m_ReferenceOrientation == 0)
  {
    m_AvgQuatsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getAvgQuatsArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if( NULL != m_AvgQuatsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    { m_AvgQuats = m_AvgQuatsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  }
  else if (m_ReferenceOrientation == 1)
  {
    cDims[0] = 1;
    m_GBEuclideanDistancesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getGBEuclideanDistancesArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if( NULL != m_GBEuclideanDistancesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    { m_GBEuclideanDistances = m_GBEuclideanDistancesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
    if(getErrorCondition() >= 0) { dataArrayPaths.push_back(getGBEuclideanDistancesArrayPath()); }
  }

  getDataContainerArray()->validateNumberOfTuples<AbstractFilter>(this, dataArrayPaths);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindFeatureReferenceMisorientations::preflight()
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
void FindFeatureReferenceMisorientations::execute()
{
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(m_FeatureIdsArrayPath.getDataContainerName());
  size_t totalPoints = m_FeatureIdsPtr.lock()->getNumberOfTuples();
  size_t totalFeatures = m_AvgQuatsPtr.lock()->getNumberOfTuples();

  QuatF q1 = QuaternionMathF::New();
  QuatF q2 = QuaternionMathF::New();
  QuatF* quats = reinterpret_cast<QuatF*>(m_Quats);
  QuatF* avgQuats = reinterpret_cast<QuatF*>(m_AvgQuats);

  float n1 = 0.0f, n2 = 0.0f, n3 = 0.0f;
  uint32_t phase1 = Ebsd::CrystalStructure::UnknownCrystalStructure;
  uint32_t phase2 = Ebsd::CrystalStructure::UnknownCrystalStructure;
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

  int32_t gnum = 0;
  float dist = 0.0f;
  std::vector<size_t> m_Centers(totalFeatures, 0);
  std::vector<float> m_CenterDists(totalFeatures, 0.0f);
  if (m_ReferenceOrientation == 1)
  {
    for (size_t i = 0; i < totalPoints; i++)
    {
      gnum = m_FeatureIds[i];
      dist = m_GBEuclideanDistances[i];
      if (dist >= m_CenterDists[gnum])
      {
        m_CenterDists[gnum] = dist;
        m_Centers[gnum] = i;
      }
    }
  }

  FloatArrayType::Pointer avgMisoPtr = FloatArrayType::CreateArray(totalFeatures * 2, "_INTERNAL_USE_ONLY_AVERAGE_MISORIENTATION");
  avgMisoPtr->initializeWithZeros();
  float* avgMiso = avgMisoPtr->getPointer(0);

  int64_t xPoints = static_cast<int64_t>(udims[0]);
  int64_t yPoints = static_cast<int64_t>(udims[1]);
  int64_t zPoints = static_cast<int64_t>(udims[2]);
  int64_t point = 0;
  int32_t idx = 0;
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
          phase1 = m_CrystalStructures[m_CellPhases[point]];
          if (m_ReferenceOrientation == 0)
          {
            QuaternionMathF::Copy(avgQuats[m_FeatureIds[point]], q2);
          }
          else if (m_ReferenceOrientation == 1)
          {
            gnum = m_FeatureIds[point];
            QuaternionMathF::Copy(quats[m_Centers[gnum]], q2);
            phase2 = m_CrystalStructures[m_CellPhases[m_Centers[gnum]]];
          }
          m_FeatureReferenceMisorientations[point] = SIMPLib::Constants::k_180OverPi * m_OrientationOps[phase1]->getMisoQuat( q1, q2, n1, n2, n3);
          idx = m_FeatureIds[point] * 2;
          avgMiso[idx + 0]++;
          avgMiso[idx + 1] = avgMiso[idx + 1] + m_FeatureReferenceMisorientations[point];
        }
        if (m_FeatureIds[point] == 0 || m_CellPhases[point] == 0)
        {
          m_FeatureReferenceMisorientations[point] = 0.0f;
        }
      }
    }
  }

  for (size_t i = 1; i < totalFeatures; i++)
  {
    idx = i * 2;
    m_FeatureAvgMisorientations[i] = avgMiso[idx + 1] / avgMiso[idx];
    if (avgMiso[idx] == 0.0f) { m_FeatureAvgMisorientations[i] = 0.0f; }
  }

  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer FindFeatureReferenceMisorientations::newFilterInstance(bool copyFilterParameters)
{
  FindFeatureReferenceMisorientations::Pointer filter = FindFeatureReferenceMisorientations::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindFeatureReferenceMisorientations::getCompiledLibraryName()
{
  return OrientationAnalysisConstants::OrientationAnalysisBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindFeatureReferenceMisorientations::getBrandingString()
{
  return "OrientationAnalysis";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindFeatureReferenceMisorientations::getFilterVersion()
{
  QString version;
  QTextStream vStream(&version);
  vStream <<  OrientationAnalysis::Version::Major() << "." << OrientationAnalysis::Version::Minor() << "." << OrientationAnalysis::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindFeatureReferenceMisorientations::getGroupName()
{ return SIMPL::FilterGroups::StatisticsFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindFeatureReferenceMisorientations::getSubGroupName()
{ return SIMPL::FilterSubGroups::CrystallographicFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindFeatureReferenceMisorientations::getHumanLabel()
{ return "Find Feature Reference Misorientations"; }
