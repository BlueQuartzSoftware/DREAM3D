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

#include "FindFeatureClustering.h"

#include <fstream>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/SIMPLibVersion.h"
#include "SIMPLib/Math/SIMPLibMath.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersWriter.h"
#include "SIMPLib/FilterParameters/IntFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/AttributeMatrixSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedBooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/Math/RadialDistributionFunction.h"
#include "SIMPLib/Geometry/ImageGeom.h"

#include "Statistics/StatisticsConstants.h"

// Include the MOC generated file for this class
#include "moc_FindFeatureClustering.cpp"



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindFeatureClustering::FindFeatureClustering() :
  AbstractFilter(),
  m_ErrorOutputFile(""),
  m_NumberOfBins(1),
  m_PhaseNumber(1),
  m_CellEnsembleAttributeMatrixName(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellEnsembleAttributeMatrixName, ""),
  m_RemoveBiasedFeatures(false),
  m_EquivalentDiametersArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellFeatureAttributeMatrixName, SIMPL::FeatureData::EquivalentDiameters),
  m_FeaturePhasesArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellFeatureAttributeMatrixName, SIMPL::FeatureData::Phases),
  m_CentroidsArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellFeatureAttributeMatrixName, SIMPL::FeatureData::Centroids),
  m_ClusteringListArrayName(SIMPL::FeatureData::ClusteringList),
  m_NewEnsembleArrayArrayName("RDF"),
  m_MaxMinArrayName("RDFMaxMinDistances"),
  m_FeaturePhases(NULL),
  m_Centroids(NULL),
  m_EquivalentDiameters(NULL),
  m_NewEnsembleArray(NULL),
  m_MaxMinArray(NULL),
  m_BiasedFeatures(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindFeatureClustering::~FindFeatureClustering()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindFeatureClustering::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(IntFilterParameter::New("Number of Bins for RDF", "NumberOfBins", getNumberOfBins(), FilterParameter::Parameter));
  parameters.push_back(IntFilterParameter::New("Phase Index", "PhaseNumber", getPhaseNumber(), FilterParameter::Parameter));
  QStringList linkedProps("BiasedFeaturesArrayPath");
  parameters.push_back(LinkedBooleanFilterParameter::New("Remove Biased Features", "RemoveBiasedFeatures", getRemoveBiasedFeatures(), linkedProps, FilterParameter::Parameter));
  parameters.push_back(SeparatorFilterParameter::New("Cell Feature Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Float, 1, SIMPL::AttributeMatrixType::CellFeature, SIMPL::GeometryType::ImageGeometry);
    parameters.push_back(DataArraySelectionFilterParameter::New("Equivalent Diameters", "EquivalentDiametersArrayPath", getEquivalentDiametersArrayPath(), FilterParameter::RequiredArray, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Int32, 1, SIMPL::AttributeMatrixType::CellFeature, SIMPL::GeometryType::ImageGeometry);
    parameters.push_back(DataArraySelectionFilterParameter::New("Phases", "FeaturePhasesArrayPath", getFeaturePhasesArrayPath(), FilterParameter::RequiredArray, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Float, 3, SIMPL::AttributeMatrixType::CellFeature, SIMPL::GeometryType::ImageGeometry);
    parameters.push_back(DataArraySelectionFilterParameter::New("Centroids", "CentroidsArrayPath", getCentroidsArrayPath(), FilterParameter::RequiredArray, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Bool, 1, SIMPL::AttributeMatrixType::CellFeature, SIMPL::GeometryType::ImageGeometry);
    parameters.push_back(DataArraySelectionFilterParameter::New("Biased Features", "BiasedFeaturesArrayPath", getBiasedFeaturesArrayPath(), FilterParameter::RequiredArray, req));
  }
  parameters.push_back(SeparatorFilterParameter::New("Cell Ensemble Data", FilterParameter::RequiredArray));
  {
    AttributeMatrixSelectionFilterParameter::RequirementType amReq = AttributeMatrixSelectionFilterParameter::CreateRequirement(SIMPL::AttributeMatrixType::CellEnsemble, SIMPL::GeometryType::ImageGeometry);
    parameters.push_back(AttributeMatrixSelectionFilterParameter::New("Cell Ensemble Attribute Matrix", "CellEnsembleAttributeMatrixName", getCellEnsembleAttributeMatrixName(), FilterParameter::RequiredArray, amReq));
  }
  parameters.push_back(SeparatorFilterParameter::New("Cell Feature Data", FilterParameter::CreatedArray));
  parameters.push_back(StringFilterParameter::New("Clustering List", "ClusteringListArrayName", getClusteringListArrayName(), FilterParameter::CreatedArray));
  parameters.push_back(SeparatorFilterParameter::New("Cell Ensemble Data", FilterParameter::CreatedArray));
  parameters.push_back(StringFilterParameter::New("Radial Distribution Function", "NewEnsembleArrayArrayName", getNewEnsembleArrayArrayName(), FilterParameter::CreatedArray));
  parameters.push_back(StringFilterParameter::New("Max and Min Separation Distances", "MaxMinArrayName", getMaxMinArrayName(), FilterParameter::CreatedArray));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindFeatureClustering::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setCellEnsembleAttributeMatrixName(reader->readDataArrayPath("CellEnsembleAttributeMatrixName", getCellEnsembleAttributeMatrixName() ) );
  setNumberOfBins( reader->readValue( "NumberOfBins", getNumberOfBins() ) );
  setNewEnsembleArrayArrayName(reader->readString("NewEnsembleArrayArrayName", getNewEnsembleArrayArrayName() ) );
  setMaxMinArrayName(reader->readString("MaxMinArrayName", getMaxMinArrayName() ) );
  setClusteringListArrayName(reader->readString("ClusteringListArrayName", getClusteringListArrayName() ) );
  setCentroidsArrayPath(reader->readDataArrayPath("CentroidsArrayPath", getCentroidsArrayPath() ) );
  setFeaturePhasesArrayPath(reader->readDataArrayPath("FeaturePhasesArrayPath", getFeaturePhasesArrayPath() ) );
  setEquivalentDiametersArrayPath(reader->readDataArrayPath("EquivalentDiametersArrayPath", getEquivalentDiametersArrayPath() ) );
  setPhaseNumber( reader->readValue("PhaseNumber", getPhaseNumber() ) );
  setBiasedFeaturesArrayPath(reader->readDataArrayPath("BiasedFeaturesArrayPath", getBiasedFeaturesArrayPath() ) );
  setRemoveBiasedFeatures( reader->readValue( "RemoveBiasedFeatures", getRemoveBiasedFeatures() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int FindFeatureClustering::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  SIMPL_FILTER_WRITE_PARAMETER(FilterVersion)
  SIMPL_FILTER_WRITE_PARAMETER(CellEnsembleAttributeMatrixName)
  SIMPL_FILTER_WRITE_PARAMETER(NumberOfBins)
  SIMPL_FILTER_WRITE_PARAMETER(ClusteringListArrayName)
  SIMPL_FILTER_WRITE_PARAMETER(CentroidsArrayPath)
  SIMPL_FILTER_WRITE_PARAMETER(FeaturePhasesArrayPath)
  SIMPL_FILTER_WRITE_PARAMETER(EquivalentDiametersArrayPath)
  SIMPL_FILTER_WRITE_PARAMETER(NewEnsembleArrayArrayName)
  SIMPL_FILTER_WRITE_PARAMETER(MaxMinArrayName)
  SIMPL_FILTER_WRITE_PARAMETER(PhaseNumber)
  SIMPL_FILTER_WRITE_PARAMETER(RemoveBiasedFeatures)
  SIMPL_FILTER_WRITE_PARAMETER(BiasedFeaturesArrayPath)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindFeatureClustering::initialize()
{
  m_ClusteringList = NeighborList<float>::NullPointer();
  m_RandomCentroids.clear();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindFeatureClustering::dataCheck()
{
  setErrorCondition(0);
  initialize();
  getDataContainerArray()->getPrereqGeometryFromDataContainer<ImageGeom, AbstractFilter>(this, getEquivalentDiametersArrayPath().getDataContainerName());

  DataArrayPath tempPath;
  QVector<size_t> cDims(1, 1);

  m_EquivalentDiametersPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getEquivalentDiametersArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_EquivalentDiametersPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_EquivalentDiameters = m_EquivalentDiametersPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  m_FeaturePhasesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getFeaturePhasesArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeaturePhasesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeaturePhases = m_FeaturePhasesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  cDims[0] = 3;
  m_CentroidsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getCentroidsArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CentroidsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_Centroids = m_CentroidsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  if (m_RemoveBiasedFeatures == true)
  {
    cDims[0] = 1;
    m_BiasedFeaturesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<bool>, AbstractFilter>(this, getBiasedFeaturesArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if( NULL != m_BiasedFeaturesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    { m_BiasedFeatures = m_BiasedFeaturesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  }

  cDims[0] = m_NumberOfBins;

  tempPath.update(getCellEnsembleAttributeMatrixName().getDataContainerName(), getCellEnsembleAttributeMatrixName().getAttributeMatrixName(), getNewEnsembleArrayArrayName() );
  m_NewEnsembleArrayPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, tempPath, 0, cDims); /* Assigns the shared_ptr<>(this, tempPath, 0, dims); Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_NewEnsembleArrayPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_NewEnsembleArray = m_NewEnsembleArrayPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  cDims[0] = 2;
  tempPath.update(getCellEnsembleAttributeMatrixName().getDataContainerName(), getCellEnsembleAttributeMatrixName().getAttributeMatrixName(), getMaxMinArrayName() );
  m_MaxMinArrayPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, tempPath, 0, cDims);
  if( NULL != m_MaxMinArrayPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_MaxMinArray = m_MaxMinArrayPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  cDims[0] = 1;
  tempPath.update(getFeaturePhasesArrayPath().getDataContainerName(), getFeaturePhasesArrayPath().getAttributeMatrixName(), getClusteringListArrayName() );
  m_ClusteringList = getDataContainerArray()->createNonPrereqArrayFromPath<NeighborList<float>, AbstractFilter, float>(this, tempPath, 0, cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindFeatureClustering::preflight()
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
void FindFeatureClustering::find_clustering()
{
  bool writeErrorFile = true;
  std::ofstream outFile;

  if (m_ErrorOutputFile.isEmpty() == false)
  {
    outFile.open(m_ErrorOutputFile.toLatin1().data(), std::ios_base::binary);
    writeErrorFile = true;
  }

  float x = 0.0f, y = 0.0f, z = 0.0f;
  float xn = 0.0f, yn = 0.0f, zn = 0.0f;
  float r = 0.0f;

  int32_t bin = 0;
  int32_t ensemble = 0;
  int32_t totalPPTfeatures = 0;
  float min = std::numeric_limits<float>::max();
  float max = 0.0f;
  float value = 0.0f;
  float sizex = 0.0f, sizey = 0.0f, sizez = 0.0f, totalvol = 0.0f, totalpoints = 0.0f;
  float normFactor = 0.0f;

  std::vector<std::vector<float> > clusteringlist;
  std::vector<float> oldcount(m_NumberOfBins);
  std::vector<float> randomRDF;

  size_t totalFeatures = m_FeaturePhasesPtr.lock()->getNumberOfTuples();
  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(m_EquivalentDiametersArrayPath.getDataContainerName());

  size_t dims[3] = { 0, 0, 0 };
  m->getGeometryAs<ImageGeom>()->getDimensions(dims);

  sizex = float(dims[0]) * m->getGeometryAs<ImageGeom>()->getXRes();
  sizey = float(dims[1]) * m->getGeometryAs<ImageGeom>()->getYRes();
  sizez = float(dims[2]) * m->getGeometryAs<ImageGeom>()->getZRes();
  totalvol = sizex * sizey * sizez;
  totalpoints = static_cast<float>(dims[0] * dims[1] * dims[2]);

  // initialize boxdims and boxres vectors
  std::vector<float> boxdims(3);
  boxdims[0] = sizex;
  boxdims[1] = sizey;
  boxdims[2] = sizez;

  std::vector<float> boxres(3);
  boxres[0] = m->getGeometryAs<ImageGeom>()->getXRes();
  boxres[1] = m->getGeometryAs<ImageGeom>()->getYRes();
  boxres[2] = m->getGeometryAs<ImageGeom>()->getZRes();

  for (size_t i = 1; i < totalFeatures; i++)
  {
    if (m_FeaturePhases[i] == m_PhaseNumber) {totalPPTfeatures++;}
  }

  clusteringlist.resize(totalFeatures);

  for (size_t i = 1; i < totalFeatures; i++)
  {
    if (m_FeaturePhases[i] == m_PhaseNumber)
    {
      if (i % 1000 == 0)
      {
        QString ss = QObject::tr("Working on Feature %1 of %2").arg(i).arg(totalPPTfeatures);
        notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);
      }

      x = m_Centroids[3 * i];
      y = m_Centroids[3 * i + 1];
      z = m_Centroids[3 * i + 2];

      for (size_t j = i + 1; j < totalFeatures; j++)
      {
        if (m_FeaturePhases[i] == m_FeaturePhases[j])
        {
          xn = m_Centroids[3 * j];
          yn = m_Centroids[3 * j + 1];
          zn = m_Centroids[3 * j + 2];

          r = sqrtf((x - xn) * (x - xn) + (y - yn) * (y - yn) + (z - zn) * (z - zn));

          clusteringlist[i].push_back(r);
          clusteringlist[j].push_back(r);

          if (writeErrorFile == true && m_FeaturePhases[j] == 2)
          {
            outFile << r << "\n" << r << "\n";
          }
        }
      }
    }
  }

  for (size_t i = 1; i < totalFeatures; i++)
  {
    for (size_t j = 0; j < clusteringlist[i].size(); j++)
    {
      if (m_FeaturePhases[i] == m_PhaseNumber)
      {
        value = clusteringlist[i][j];
        if (value > max) { max = value; }
        if (value < min) { min = value; }
      }
    }
  }

  float stepsize = (max - min) / m_NumberOfBins;

  m_MaxMinArray[(m_PhaseNumber * 2)] = max;
  m_MaxMinArray[(m_PhaseNumber * 2) + 1] = min;

  for (size_t i = 1; i < totalFeatures; i++)
  {
    for (size_t j = 0; j < clusteringlist[i].size(); j++)
    {
      if (m_FeaturePhases[i] == m_PhaseNumber)
      {
        if (m_RemoveBiasedFeatures == false || m_BiasedFeatures[i] == false)
        {
          ensemble = m_FeaturePhases[i];
          bin = (clusteringlist[i][j] - min) / stepsize;
          if(bin >= m_NumberOfBins) { bin = m_NumberOfBins - 1; }
          m_NewEnsembleArray[(m_NumberOfBins * ensemble) + bin]++;
        }
      }
    }
  }

  // Generate random distribution based on same box size and same stepsize
  float max_box_distance = sqrtf((sizex * sizex) + (sizey * sizey) + (sizez * sizez));
  int32_t current_num_bins = ceilf((max_box_distance - min) / (stepsize));

  randomRDF.resize(current_num_bins + 1);
  // Call this function to generate the random distribution, which is normalized by the total number of distances
  randomRDF = RadialDistributionFunction::GenerateRandomDistribution(min, max, m_NumberOfBins, boxdims, boxres);

  // Scale the random distribution by the number of distances in this particular instance
  normFactor = totalPPTfeatures * (totalPPTfeatures - 1);
  for (size_t i = 0; i < randomRDF.size(); i++)
  {
    randomRDF[i] = randomRDF[i] * normFactor;
  }

  for (size_t i = 0; i < m_NumberOfBins; i++)
  {
    oldcount[i] = m_NewEnsembleArray[(m_NumberOfBins * m_PhaseNumber) + i];
    m_NewEnsembleArray[(m_NumberOfBins * m_PhaseNumber) + i] = oldcount[i] / randomRDF[i + 1];
  }

//    std::ofstream testFile3;
//    testFile3.open("/Users/Shared/Data/PW_Work/OUTFILE/normalized_target.txt");
//    for (size_t i = 0; i < m_NumberOfBins; i++)
//    {
//    testFile3 << "\n" << m_NewEnsembleArray[(m_NumberOfBins*m_PhaseNumber) + i];
//    }
//    testFile3.close();

//    std::ofstream testFile4;
//    testFile4.open("/Users/Shared/Data/PW_Work/OUTFILE/randomrdf_target.txt");
//    for (size_t i = 0; i < randomRDF.size(); i++)
//    {
//    testFile4 << "\n" << randomRDF[i];
//    }
//    testFile4.close();

//    std::ofstream testFile7;
//    testFile7.open("/Users/Shared/Data/PW_Work/OUTFILE/targetRaw.txt");
//    for (size_t i = 0; i < oldcount.size(); i++)
//    {
//    testFile7 << "\n" << oldcount[i];
//    }
//    testFile7.close();

  for (size_t i = 1; i < totalFeatures; i++)
  {
    // Set the vector for each list into the Clustering Object
    NeighborList<float>::SharedVectorType sharedClustLst(new std::vector<float>);
    sharedClustLst->assign(clusteringlist[i].begin(), clusteringlist[i].end());
    m_ClusteringList.lock()->setList(static_cast<int>(i), sharedClustLst);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindFeatureClustering::execute()
{
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  find_clustering();

  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer FindFeatureClustering::newFilterInstance(bool copyFilterParameters)
{
  FindFeatureClustering::Pointer filter = FindFeatureClustering::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindFeatureClustering::getCompiledLibraryName()
{
  return StatisticsConstants::StatisticsBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindFeatureClustering::getBrandingString()
{
  return "Statistics";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindFeatureClustering::getFilterVersion()
{
  QString version;
  QTextStream vStream(&version);
  vStream <<  SIMPLib::Version::Major() << "." << SIMPLib::Version::Minor() << "." << SIMPLib::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindFeatureClustering::getGroupName()
{ return SIMPL::FilterGroups::StatisticsFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindFeatureClustering::getSubGroupName()
{ return SIMPL::FilterSubGroups::MorphologicalFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindFeatureClustering::getHumanLabel()
{ return "Find Feature Clustering"; }
