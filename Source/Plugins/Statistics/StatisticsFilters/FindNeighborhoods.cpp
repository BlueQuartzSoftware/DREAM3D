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

#include "FindNeighborhoods.h"

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/DoubleFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/Math/SIMPLibMath.h"
#include "SIMPLib/Geometry/ImageGeom.h"

#include "Statistics/StatisticsConstants.h"
#include "Statistics/StatisticsVersion.h"

// Include the MOC generated file for this class
#include "moc_FindNeighborhoods.cpp"



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindNeighborhoods::FindNeighborhoods() :
  AbstractFilter(),
  m_NeighborhoodListArrayName(SIMPL::FeatureData::NeighborhoodList),
  m_MultiplesOfAverage(1.0f),
  m_EquivalentDiametersArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellFeatureAttributeMatrixName, SIMPL::FeatureData::EquivalentDiameters),
  m_FeaturePhasesArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellFeatureAttributeMatrixName, SIMPL::FeatureData::Phases),
  m_CentroidsArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellFeatureAttributeMatrixName, SIMPL::FeatureData::Centroids),
  m_NeighborhoodsArrayName(SIMPL::FeatureData::Neighborhoods),
  m_FeaturePhases(NULL),
  m_Centroids(NULL),
  m_EquivalentDiameters(NULL),
  m_Neighborhoods(NULL)
{
  m_NeighborhoodList = NeighborList<int32_t>::NullPointer();

  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindNeighborhoods::~FindNeighborhoods()
{
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindNeighborhoods::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(DoubleFilterParameter::New("Multiples of Average Diameter", "MultiplesOfAverage", getMultiplesOfAverage(), FilterParameter::Parameter, SIMPL_BIND_SETTER(FindNeighborhoods, this, MultiplesOfAverage), SIMPL_BIND_GETTER(FindNeighborhoods, this, MultiplesOfAverage)));
  parameters.push_back(SeparatorFilterParameter::New("Cell Feature Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Float, 1, SIMPL::AttributeMatrixType::CellFeature, SIMPL::GeometryType::ImageGeometry);
    parameters.push_back(DataArraySelectionFilterParameter::New("Equivalent Diameters", "EquivalentDiametersArrayPath", getEquivalentDiametersArrayPath(), FilterParameter::RequiredArray, req, SIMPL_BIND_SETTER(FindNeighborhoods, this, EquivalentDiametersArrayPath), SIMPL_BIND_GETTER(FindNeighborhoods, this, EquivalentDiametersArrayPath)));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Int32, 1, SIMPL::AttributeMatrixType::CellFeature, SIMPL::GeometryType::ImageGeometry);
    parameters.push_back(DataArraySelectionFilterParameter::New("Phases", "FeaturePhasesArrayPath", getFeaturePhasesArrayPath(), FilterParameter::RequiredArray, req, SIMPL_BIND_SETTER(FindNeighborhoods, this, FeaturePhasesArrayPath), SIMPL_BIND_GETTER(FindNeighborhoods, this, FeaturePhasesArrayPath)));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Float, 3, SIMPL::AttributeMatrixType::CellFeature, SIMPL::GeometryType::ImageGeometry);
    parameters.push_back(DataArraySelectionFilterParameter::New("Centroids", "CentroidsArrayPath", getCentroidsArrayPath(), FilterParameter::RequiredArray, req, SIMPL_BIND_SETTER(FindNeighborhoods, this, CentroidsArrayPath), SIMPL_BIND_GETTER(FindNeighborhoods, this, CentroidsArrayPath)));
  }
  parameters.push_back(SeparatorFilterParameter::New("Cell Feature Data", FilterParameter::CreatedArray));
  parameters.push_back(StringFilterParameter::New("Neighborhoods", "NeighborhoodsArrayName", getNeighborhoodsArrayName(), FilterParameter::CreatedArray, SIMPL_BIND_SETTER(FindNeighborhoods, this, NeighborhoodsArrayName), SIMPL_BIND_GETTER(FindNeighborhoods, this, NeighborhoodsArrayName)));
  parameters.push_back(StringFilterParameter::New("Neighborhood List", "NeighborhoodListArrayName", getNeighborhoodListArrayName(), FilterParameter::CreatedArray, SIMPL_BIND_SETTER(FindNeighborhoods, this, NeighborhoodListArrayName), SIMPL_BIND_GETTER(FindNeighborhoods, this, NeighborhoodListArrayName)));
  setFilterParameters(parameters);
}
// -----------------------------------------------------------------------------
void FindNeighborhoods::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setNeighborhoodsArrayName(reader->readString("NeighborhoodsArrayName", getNeighborhoodsArrayName() ) );
  setNeighborhoodListArrayName(reader->readString("NeighborhoodListArrayName", getNeighborhoodListArrayName() ) );
  setCentroidsArrayPath(reader->readDataArrayPath("CentroidsArrayPath", getCentroidsArrayPath() ) );
  setFeaturePhasesArrayPath(reader->readDataArrayPath("FeaturePhasesArrayPath", getFeaturePhasesArrayPath() ) );
  setEquivalentDiametersArrayPath(reader->readDataArrayPath("EquivalentDiametersArrayPath", getEquivalentDiametersArrayPath() ) );
  setMultiplesOfAverage( reader->readValue("MultiplesOfAverage", getMultiplesOfAverage()) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindNeighborhoods::initialize()
{
  m_NeighborhoodList = NeighborList<int32_t>::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindNeighborhoods::dataCheck()
{
  setErrorCondition(0);
  initialize();
  DataArrayPath tempPath;

  QVector<DataArrayPath> dataArrayPaths;

  getDataContainerArray()->getPrereqGeometryFromDataContainer<ImageGeom, AbstractFilter>(this, getEquivalentDiametersArrayPath().getDataContainerName());

  // Feature Data
  // Do this whole block FIRST otherwise the side effect is that a call to m->getNumCellFeatureTuples will = 0
  // because we are just creating an empty NeighborList object.
  // Now we are going to get a "Pointer" to the NeighborList object out of the DataContainer
  QVector<size_t> cDims(1, 1);
  tempPath.update(m_EquivalentDiametersArrayPath.getDataContainerName(), m_EquivalentDiametersArrayPath.getAttributeMatrixName(), getNeighborhoodListArrayName() );
  m_NeighborhoodList = getDataContainerArray()->createNonPrereqArrayFromPath<NeighborList<int32_t>, AbstractFilter, int32_t>(this, tempPath, 0, cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */

  m_EquivalentDiametersPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getEquivalentDiametersArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_EquivalentDiametersPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_EquivalentDiameters = m_EquivalentDiametersPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCondition() >= 0) { dataArrayPaths.push_back(getEquivalentDiametersArrayPath()); }

  m_FeaturePhasesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getFeaturePhasesArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeaturePhasesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeaturePhases = m_FeaturePhasesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCondition() >= 0) { dataArrayPaths.push_back(getFeaturePhasesArrayPath()); }

  tempPath.update(m_EquivalentDiametersArrayPath.getDataContainerName(), m_EquivalentDiametersArrayPath.getAttributeMatrixName(), getNeighborhoodsArrayName() );
  m_NeighborhoodsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter, int32_t>(this, tempPath, 0, cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_NeighborhoodsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_Neighborhoods = m_NeighborhoodsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  cDims[0] = 3;
  m_CentroidsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getCentroidsArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CentroidsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_Centroids = m_CentroidsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCondition() >= 0) { dataArrayPaths.push_back(getCentroidsArrayPath()); }

  getDataContainerArray()->validateNumberOfTuples(this, dataArrayPaths);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindNeighborhoods::preflight()
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
void FindNeighborhoods::find_neighborhoods()
{
  float x = 0.0f, y = 0.0f, z = 0.0f;

  std::vector<std::vector<int32_t> > neighborhoodlist;
  std::vector<float> criticalDistance;

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(m_EquivalentDiametersArrayPath.getDataContainerName());
  size_t totalFeatures = m_EquivalentDiametersPtr.lock()->getNumberOfTuples();

  neighborhoodlist.resize(totalFeatures);
  criticalDistance.resize(totalFeatures);

  float aveDiam = 0.0f;
  for (size_t i = 1; i < totalFeatures; i++)
  {
    m_Neighborhoods[i] = 0;
    aveDiam += m_EquivalentDiameters[i];
    criticalDistance[i] = m_EquivalentDiameters[i] * m_MultiplesOfAverage;
  }
  aveDiam /= totalFeatures;
  for (size_t i = 1; i < totalFeatures; i++)
  {
    criticalDistance[i] /= aveDiam;
  }

  float m_OriginX = 0.0f, m_OriginY = 0.0f, m_OriginZ = 0.0f;
  m->getGeometryAs<ImageGeom>()->getOrigin(m_OriginX, m_OriginY, m_OriginZ);
  size_t udims[3] = { 0, 0, 0 };
  m->getGeometryAs<ImageGeom>()->getDimensions(udims);

  size_t xbin = 0, ybin = 0, zbin = 0;
  std::vector<int64_t> bins(3 * totalFeatures, 0);
  for (size_t i = 1; i < totalFeatures; i++)
  {
    x = m_Centroids[3 * i];
    y = m_Centroids[3 * i + 1];
    z = m_Centroids[3 * i + 2];
    xbin = int32_t((x - m_OriginX) / aveDiam);
    ybin = int32_t((y - m_OriginY) / aveDiam);
    zbin = int32_t((z - m_OriginZ) / aveDiam);
    bins[3 * i] = xbin;
    bins[3 * i + 1] = ybin;
    bins[3 * i + 2] = zbin;
  }

  int64_t bin1x = 0, bin2x = 0, bin1y = 0, bin2y = 0, bin1z = 0, bin2z = 0;
  float dBinX = 0, dBinY = 0, dBinZ = 0;
  float criticalDistance1 = 0, criticalDistance2 = 0;

  for (size_t i = 1; i < totalFeatures; i++)
  {
    if (i % 1000 == 0)
    {

      QString ss = QObject::tr("Working on Feature %1 of %2").arg(i).arg(totalFeatures);
      notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);
    }
    x = m_Centroids[3 * i];
    y = m_Centroids[3 * i + 1];
    z = m_Centroids[3 * i + 2];
    bin1x = bins[3 * i];
    bin1y = bins[3 * i + 1];
    bin1z = bins[3 * i + 2];
    criticalDistance1 = criticalDistance[i];

    for (size_t j = i + 1; j < totalFeatures; j++)
    {
      bin2x = bins[3 * j];
      bin2y = bins[3 * j + 1];
      bin2z = bins[3 * j + 2];
      criticalDistance2 = criticalDistance[j];
      // Use the llabs version of the "C" abs function because we are using int64_t
      // do NOT try to use the std::abs() function as this is C++11 ONLY
      dBinX = llabs(bin2x - bin1x);
      dBinY = llabs(bin2y - bin1y);
      dBinZ = llabs(bin2z - bin1z);

      if (dBinX < criticalDistance1 && dBinY < criticalDistance1 && dBinZ < criticalDistance1)
      {
        m_Neighborhoods[i]++;
        neighborhoodlist[i].push_back(j);
      }

      if (dBinX < criticalDistance2 && dBinY < criticalDistance2 && dBinZ < criticalDistance2)
      {
        m_Neighborhoods[j]++;
        neighborhoodlist[j].push_back(i);
      }
    }
  }
  for (size_t i = 1; i < totalFeatures; i++)
  {
    // Set the vector for each list into the NeighborhoodList Object
    NeighborList<int32_t>::SharedVectorType sharedNeiLst(new std::vector<int32_t>);
    sharedNeiLst->assign(neighborhoodlist[i].begin(), neighborhoodlist[i].end());
    m_NeighborhoodList.lock()->setList(static_cast<int32_t>(i), sharedNeiLst);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindNeighborhoods::execute()
{
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  find_neighborhoods();

  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer FindNeighborhoods::newFilterInstance(bool copyFilterParameters)
{
  FindNeighborhoods::Pointer filter = FindNeighborhoods::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindNeighborhoods::getCompiledLibraryName()
{
  return StatisticsConstants::StatisticsBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindNeighborhoods::getBrandingString()
{
  return "Statistics";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindNeighborhoods::getFilterVersion()
{
  QString version;
  QTextStream vStream(&version);
  vStream <<  Statistics::Version::Major() << "." << Statistics::Version::Minor() << "." << Statistics::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindNeighborhoods::getGroupName()
{ return SIMPL::FilterGroups::StatisticsFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindNeighborhoods::getSubGroupName()
{ return SIMPL::FilterSubGroups::MorphologicalFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindNeighborhoods::getHumanLabel()
{ return "Find Feature Neighborhoods"; }
