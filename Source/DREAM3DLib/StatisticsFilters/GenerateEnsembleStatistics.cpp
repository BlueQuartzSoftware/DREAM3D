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

#include "GenerateEnsembleStatistics.h"

#include "DREAM3DLib/Math/DREAM3DMath.h"
#include "DREAM3DLib/Common/Constants.h"

#include "DREAM3DLib/DistributionAnalysisOps/BetaOps.h"
#include "DREAM3DLib/DistributionAnalysisOps/PowerLawOps.h"
#include "DREAM3DLib/DistributionAnalysisOps/LogNormalOps.h"

#include "DREAM3DLib/StatisticsFilters/FindNeighbors.h"
#include "DREAM3DLib/GenericFilters/FindSurfaceFeatures.h"
#include "DREAM3DLib/GenericFilters/FindFeaturePhases.h"



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GenerateEnsembleStatistics::GenerateEnsembleStatistics()  :
  AbstractFilter(),
  m_DataContainerName(DREAM3D::HDF5::VolumeDataContainerName),
  m_AvgQuatsArrayName(DREAM3D::FeatureData::AvgQuats),
  m_BiasedFeaturesArrayName(DREAM3D::FeatureData::BiasedFeatures),
  m_VolumesArrayName(DREAM3D::FeatureData::Volumes),
  m_FeatureEulerAnglesArrayName(DREAM3D::FeatureData::EulerAngles),
  m_FeaturePhasesArrayName(DREAM3D::FeatureData::Phases),
  m_SurfaceFeaturesArrayName(DREAM3D::FeatureData::SurfaceFeatures),
  m_EquivalentDiametersArrayName(DREAM3D::FeatureData::EquivalentDiameters),
  m_AspectRatiosArrayName(DREAM3D::FeatureData::AspectRatios),
  m_Omega3sArrayName(DREAM3D::FeatureData::Omega3s),
  m_NeighborhoodsArrayName(DREAM3D::FeatureData::Neighborhoods),
  m_AxisEulerAnglesArrayName(DREAM3D::FeatureData::AxisEulerAngles),
  m_CrystalStructuresArrayName(DREAM3D::EnsembleData::CrystalStructures),
  m_TotalSurfaceAreasArrayName(DREAM3D::EnsembleData::TotalSurfaceAreas),
  m_PhaseTypesArrayName(DREAM3D::EnsembleData::PhaseTypes),
  m_SizeCorrelationResolution(1),
  m_SizeDistribution(false),
  m_SizeDistributionFitType(DREAM3D::DistributionType::LogNormal),
  m_AspectRatioDistribution(false),
  m_AspectRatioDistributionFitType(DREAM3D::DistributionType::Beta),
  m_Omega3Distribution(false),
  m_Omega3DistributionFitType(DREAM3D::DistributionType::Beta),
  m_NeighborhoodDistribution(false),
  m_NeighborhoodDistributionFitType(DREAM3D::DistributionType::LogNormal),
  m_CalculateODF(false),
  m_CalculateMDF(false),
  m_CalculateAxisODF(false),
  m_AvgQuats(NULL),
  m_FeatureEulerAngles(NULL),
  m_Volumes(NULL),
  m_BiasedFeatures(NULL),
  m_SurfaceFeatures(NULL),
  m_FeaturePhases(NULL),
  m_AxisEulerAngles(NULL),
  m_Omega3s(NULL),
  m_AspectRatios(NULL),
  m_EquivalentDiameters(NULL),
  m_Neighborhoods(NULL),
  m_CrystalStructures(NULL),
  m_PhaseTypes(NULL),
  m_NeighborList(NULL),
  m_SharedSurfaceAreaList(NULL),
  m_StatsDataArray(NULL)
{
  m_DistributionAnalysis.push_back(BetaOps::New());
  m_DistributionAnalysis.push_back(LogNormalOps::New());
  m_DistributionAnalysis.push_back(PowerLawOps::New());

  m_OrientationOps = OrientationOps::getOrientationOpsVector();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GenerateEnsembleStatistics::~GenerateEnsembleStatistics()
{
}
// -----------------------------------------------------------------------------
void GenerateEnsembleStatistics::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setSizeDistribution( reader->readValue("SizeDistribution", getSizeDistribution() ) );
  setSizeDistributionFitType( reader->readValue("SizeDistributionFitType", getSizeDistributionFitType() ) );
  setAspectRatioDistribution( reader->readValue("AspectRatioDistribution", getAspectRatioDistribution() ) );
  setAspectRatioDistributionFitType( reader->readValue("AspectRatioDistributionFitType", getAspectRatioDistributionFitType() ) );
  setOmega3Distribution( reader->readValue("Omega3Distribution", getOmega3Distribution() ) );
  setOmega3DistributionFitType( reader->readValue("Omega3DistributionFitType", getOmega3DistributionFitType() ) );
  setNeighborhoodDistribution( reader->readValue("NeighborhoodDistribution", getNeighborhoodDistribution() ) );
  setNeighborhoodDistributionFitType( reader->readValue("NeighborhoodDistributionFitType", getNeighborhoodDistributionFitType() ) );
  setCalculateODF( reader->readValue("CalculateODF", getCalculateODF() ) );
  setCalculateMDF( reader->readValue("CalculateMDF", getCalculateMDF() ) );
  setCalculateAxisODF( reader->readValue("CalculateAxisODF", getCalculateAxisODF() ) );
  setSizeCorrelationResolution( reader->readValue("SizeCorrelationResolution", getSizeCorrelationResolution() ) );
  setPhaseTypeArray( reader->readArray("PhaseTypeArray", getPhaseTypeArray() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int GenerateEnsembleStatistics::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("SizeDistribution", getSizeDistribution() );
  writer->writeValue("SizeDistributionFitType", getSizeDistributionFitType() );
  writer->writeValue("AspectRatioDistribution", getAspectRatioDistribution() );
  writer->writeValue("AspectRatioDistributionFitType", getAspectRatioDistributionFitType() );
  writer->writeValue("Omega3Distribution", getOmega3Distribution() );
  writer->writeValue("Omega3DistributionFitType", getOmega3DistributionFitType() );
  writer->writeValue("NeighborhoodDistribution", getNeighborhoodDistribution() );
  writer->writeValue("NeighborhoodDistributionFitType", getNeighborhoodDistributionFitType() );
  writer->writeValue("CalculateODF", getCalculateODF() );
  writer->writeValue("CalculateMDF", getCalculateMDF() );
  writer->writeValue("CalculateAxisODF", getCalculateAxisODF() );
  writer->writeValue("SizeCorrelationResolution", getSizeCorrelationResolution() );
  writer->writeValue("PhaseTypeArray", getPhaseTypeArray() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateEnsembleStatistics::dataCheck(bool preflight, size_t voxels, size_t features, size_t ensembles)
{
  setErrorCondition(0);

  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

  QVector<int> dims(1, 1);
  m_FeaturePhasesPtr = m->getPrereqArray<int32_t, AbstractFilter>(this, m_CellFeatureAttributeMatrixName,  m_FeaturePhasesArrayName, -303, features, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeaturePhasesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
{ m_FeaturePhases = m_FeaturePhasesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  if(m_SizeDistribution == true || m_Omega3Distribution == true
      || m_AspectRatioDistribution == true || m_NeighborhoodDistribution == true || m_CalculateAxisODF == true)
  {
    m_BiasedFeaturesPtr = m->getPrereqArray<bool, AbstractFilter>(this, m_CellFeatureAttributeMatrixName,  m_BiasedFeaturesArrayName, -302, features, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_BiasedFeaturesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
{ m_BiasedFeatures = m_BiasedFeaturesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
    m_EquivalentDiametersPtr = m->getPrereqArray<float, AbstractFilter>(this, m_CellFeatureAttributeMatrixName,  m_EquivalentDiametersArrayName, -302, features, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_EquivalentDiametersPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
{ m_EquivalentDiameters = m_EquivalentDiametersPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  }
  if(m_NeighborhoodDistribution == true)
  {
    m_NeighborhoodsPtr = m->getPrereqArray<int32_t, AbstractFilter>(this, m_CellFeatureAttributeMatrixName,  m_NeighborhoodsArrayName, -304, features, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_NeighborhoodsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
{ m_Neighborhoods = m_NeighborhoodsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  }
  if(m_AspectRatioDistribution == true)
  {
    dims[0] = 2;
    m_AspectRatiosPtr = m->getPrereqArray<float, AbstractFilter>(this, m_CellFeatureAttributeMatrixName,  m_AspectRatiosArrayName, -307, features, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_AspectRatiosPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
{ m_AspectRatios = m_AspectRatiosPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  }
  if(m_Omega3Distribution == true)
  {
    dims[0] = 1;
    m_Omega3sPtr = m->getPrereqArray<float, AbstractFilter>(this, m_CellFeatureAttributeMatrixName,  m_Omega3sArrayName, -306, features, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_Omega3sPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
{ m_Omega3s = m_Omega3sPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  }
  if(m_CalculateAxisODF == true)
  {
    dims[0] = 3;
    m_AxisEulerAnglesPtr = m->getPrereqArray<float, AbstractFilter>(this, m_CellFeatureAttributeMatrixName,  m_AxisEulerAnglesArrayName, -305, features, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_AxisEulerAnglesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
{ m_AxisEulerAngles = m_AxisEulerAnglesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  }


  if(m_CalculateODF == true || m_CalculateMDF == true)
  {
    dims[0] = 1;
    typedef DataArray<unsigned int> XTalStructArrayType;
    m_CrystalStructuresPtr = m->getPrereqArray<unsigned int, AbstractFilter>(this, m_CellEnsembleAttributeMatrixName,  m_CrystalStructuresArrayName, -305, ensembles, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CrystalStructuresPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
{ m_CrystalStructures = m_CrystalStructuresPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
    m_SurfaceFeaturesPtr = m->getPrereqArray<bool, AbstractFilter>(this, m_CellFeatureAttributeMatrixName,  m_SurfaceFeaturesArrayName, -302, features, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_SurfaceFeaturesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
{ m_SurfaceFeatures = m_SurfaceFeaturesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  }
  if(m_CalculateODF == true)
  {
    dims[0] = 1;
    m_VolumesPtr = m->getPrereqArray<float, AbstractFilter>(this, m_CellFeatureAttributeMatrixName,  m_VolumesArrayName, -304, features, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_VolumesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
{ m_Volumes = m_VolumesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
    dims[0] = 3;
    m_FeatureEulerAnglesPtr = m->getPrereqArray<float, AbstractFilter>(this, m_CellFeatureAttributeMatrixName,  m_FeatureEulerAnglesArrayName, -302, features, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeatureEulerAnglesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
{ m_FeatureEulerAngles = m_FeatureEulerAnglesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  }
  if(m_CalculateMDF == true)
  {
    dims[0] = 4;
    m_AvgQuatsPtr = m->getPrereqArray<float, AbstractFilter>(this, m_CellFeatureAttributeMatrixName,  m_AvgQuatsArrayName, -301, features, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_AvgQuatsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
{ m_AvgQuats = m_AvgQuatsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
    m_SharedSurfaceAreaList = NeighborList<float>::SafeObjectDownCast<IDataArray*, NeighborList<float>*>(m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->addAttributeArray(DREAM3D::FeatureData::SharedSurfaceAreaList).get());
    if(m_SharedSurfaceAreaList == NULL)
    {

      QString ss = QObject::tr("SurfaceAreaLists Array Not Initialized correctly");
      setErrorCondition(-306);
      addErrorMessage(getHumanLabel(), ss, -306);
    }
    // Now we are going to get a "Pointer" to the NeighborList object out of the DataContainer
    m_NeighborList = NeighborList<int>::SafeObjectDownCast<IDataArray*, NeighborList<int>*>(m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->addAttributeArray(DREAM3D::FeatureData::NeighborList).get());
    if(m_NeighborList == NULL)
    {

      QString ss = QObject::tr("NeighborLists Array Not Initialized correctly");
      setErrorCondition(-305);
      addErrorMessage(getHumanLabel(), ss, -305);
    }
  }


  if (m_PhaseTypeArray.size() <= 1)
  {
    addErrorMessage(getHumanLabel(), "The Phase Type Array must contain at least one member.  Add a Phase Type on the GUI.", -1000);
  }
  else
  {
    dims[0] = 1;
    typedef DataArray<unsigned int> PhaseTypeArrayType;
    m_PhaseTypesPtr = m->createNonPrereqArray<unsigned int, AbstractFilter>(this, m_CellEnsembleAttributeMatrixName,  m_PhaseTypesArrayName, DREAM3D::PhaseType::UnknownPhaseType, ensembles, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_PhaseTypesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
{ m_PhaseTypes = m_PhaseTypesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  }
  m_StatsDataArray = StatsDataArray::SafeObjectDownCast<IDataArray*, StatsDataArray*>(m->getCellEnsembleData(DREAM3D::EnsembleData::Statistics).get());
  if(m_StatsDataArray == NULL)
  {
    StatsDataArray::Pointer p = StatsDataArray::New();
    m_StatsDataArray = p.get();
    m_StatsDataArray->fillArrayWithNewStatsData(ensembles, m_PhaseTypes);
    m->getAttributeMatrix(getCellEnsembleAttributeMatrixName())->addAttributeArray(DREAM3D::EnsembleData::Statistics, p);
  }

  if (m_SizeDistributionFitType != DREAM3D::DistributionType::LogNormal)
  {
    addWarningMessage(getHumanLabel(), "The Size Distribution needs to be a Log Normal Distribution otherwise unpredictable results may occur.", -1000);
  }
  if (m_AspectRatioDistributionFitType != DREAM3D::DistributionType::Beta)
  {
    addWarningMessage(getHumanLabel(), "The Aspect Ratio needs to be a Beta Distribution otherwise unpredictable results may occur.", -1000);
  }
  if (m_Omega3DistributionFitType != DREAM3D::DistributionType::Beta)
  {
    addWarningMessage(getHumanLabel(), "The Omega 3 needs to be a Beta Distribution otherwise unpredictable results may occur.", -1000);
  }
  if (m_NeighborhoodDistributionFitType != DREAM3D::DistributionType::LogNormal)
  {
    addWarningMessage(getHumanLabel(), "The Neighborhood type needs to be a Log Normal Distribution otherwise unpredictable results may occur.", -1000);
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateEnsembleStatistics::preflight()
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
void GenerateEnsembleStatistics::execute()
{
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The DataContainer Object was NULL", -999);
    return;
  }
  setErrorCondition(0);


  //  int totalPoints = m->getTotalPoints();
  //  int totalFeatures = m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->getNumTuples();
  int totalEnsembles = m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->getNumTuples();
  dataCheck(false, totalPoints, totalFeatures, totalEnsembles);
  if (getErrorCondition() < 0)
  {
    return;
  }

  // Check to see if the user has over ridden the phase types for this filter
  if (m_PhaseTypeArray.size() > 0)
  {
    typedef DataArray<unsigned int> PhaseTypeArrayType;

    if(static_cast<int>(m_PhaseTypeArray.size()) < totalEnsembles)
    {
      setErrorCondition(-999);
      notifyErrorMessage("The number of PhaseTypes entered is less than the number of Ensembles", -999);
      return;
    }
    if(static_cast<int>(m_PhaseTypeArray.size()) > totalEnsembles)
    {

      QString ss = QObject::tr("The number of PhaseTypes entered is more than the number of Ensembles, only the first %1 will be used").arg(totalEnsembles - 1);
      notifyWarningMessage(ss, -999);
    }
    PhaseTypeArrayType::Pointer phaseTypes = PhaseTypeArrayType::CreateArray(totalEnsembles, m_PhaseTypesArrayName);
    for(int r = 0; r < totalEnsembles; ++r)
    {
      phaseTypes->SetValue(r, m_PhaseTypeArray[r]);
    }
    m->getAttributeMatrix(getCellEnsembleAttributeMatrixName())->addAttributeArray(phaseTypes->GetName(), phaseTypes);
    m_PhaseTypes = phaseTypes->getPointer(0);
  }

  StatsDataArray::Pointer p = StatsDataArray::New();
  m_StatsDataArray = p.get();
  m_StatsDataArray->fillArrayWithNewStatsData(totalEnsembles, m_PhaseTypes);
  m->getAttributeMatrix(getCellEnsembleAttributeMatrixName())->addAttributeArray(DREAM3D::EnsembleData::Statistics, p);

  dataCheck(false, totalPoints, totalFeatures, totalEnsembles);

  if(m_SizeDistribution == true)
  {
    gatherSizeStats();
  }
  if(m_AspectRatioDistribution == true)
  {
    gatherAspectRatioStats();
  }
  if(m_Omega3Distribution == true)
  {
    gatherOmega3Stats();
  }
  if(m_NeighborhoodDistribution == true)
  {
    gatherNeighborhoodStats();
  }
  if(m_CalculateODF == true)
  {
    gatherODFStats();
  }
  if(m_CalculateMDF == true)
  {
    gatherMDFStats();
  }
  if(m_CalculateAxisODF == true)
  {
    gatherAxisODFStats();
  }

  notifyStatusMessage("GenerateEnsembleStatistics Completed");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateEnsembleStatistics::gatherSizeStats()
{
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

  StatsDataArray& statsDataArray = *m_StatsDataArray;

  float maxdiam;
  float mindiam;
  float totalUnbiasedVolume = 0.0;
  QVector<VectorOfFloatArray> sizedist;
  QVector<QVector<QVector<float > > > values;

  FloatArrayType::Pointer binnumbers;
  size_t numfeatures = m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->getNumTuples();
  size_t numensembles = m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->getNumTuples();

  QVector<float> fractions(numensembles, 0.0);
  sizedist.resize(numensembles);
  values.resize(numensembles);

  for(size_t i = 1; i < numensembles; i++)
  {
    sizedist[i] = statsDataArray[i]->CreateCorrelatedDistributionArrays(m_SizeDistributionFitType, 1);
    values[i].resize(1);
  }

  float vol;
  for (size_t i = 1; i < numfeatures; i++)
  {
    if(m_BiasedFeatures[i] == false)
    {
      values[m_FeaturePhases[i]][0].push_back(m_EquivalentDiameters[i]);
      vol = (1.0 / 6.0) * DREAM3D::Constants::k_Pi * m_EquivalentDiameters[i] * m_EquivalentDiameters[i] * m_EquivalentDiameters[i];
      fractions[m_FeaturePhases[i]] = fractions[m_FeaturePhases[i]] + vol;
      totalUnbiasedVolume = totalUnbiasedVolume + vol;
    }
  }
  for (size_t i = 1; i < numensembles; i++)
  {
    if(m_PhaseTypes[i] == DREAM3D::PhaseType::PrimaryPhase)
    {
      PrimaryStatsData* pp = PrimaryStatsData::SafePointerDownCast(statsDataArray[i].get());
      pp->setPhaseFraction((fractions[i] / totalUnbiasedVolume));
      m_DistributionAnalysis[m_SizeDistributionFitType]->calculateCorrelatedParameters(values[i], sizedist[i]);
      pp->setFeatureSizeDistribution(sizedist[i]);
      DistributionAnalysisOps::determinemaxandminvalues(values[i][0], maxdiam, mindiam);
      int numbins = int(maxdiam / m_SizeCorrelationResolution) + 1;
      pp->setFeatureDiameterInfo(m_SizeCorrelationResolution, maxdiam, mindiam);
      binnumbers = FloatArrayType::CreateArray(numbins, DREAM3D::HDF5::BinNumber);
      DistributionAnalysisOps::determinebinnumbers(maxdiam, mindiam, m_SizeCorrelationResolution, binnumbers);
      pp->setBinNumbers(binnumbers);
    }
    if(m_PhaseTypes[i] == DREAM3D::PhaseType::PrecipitatePhase)
    {
      PrecipitateStatsData* pp = PrecipitateStatsData::SafePointerDownCast(statsDataArray[i].get());
      pp->setPhaseFraction((fractions[i] / totalUnbiasedVolume));
      m_DistributionAnalysis[m_SizeDistributionFitType]->calculateCorrelatedParameters(values[i], sizedist[i]);
      pp->setFeatureSizeDistribution(sizedist[i]);
      DistributionAnalysisOps::determinemaxandminvalues(values[i][0], maxdiam, mindiam);
      int numbins = int(maxdiam / m_SizeCorrelationResolution) + 1;
      pp->setFeatureDiameterInfo(m_SizeCorrelationResolution, maxdiam, mindiam);
      binnumbers = FloatArrayType::CreateArray(numbins, DREAM3D::HDF5::BinNumber);
      DistributionAnalysisOps::determinebinnumbers(maxdiam, mindiam, m_SizeCorrelationResolution, binnumbers);

      pp->setBinNumbers(binnumbers);
    }
    if(m_PhaseTypes[i] == DREAM3D::PhaseType::TransformationPhase)
    {
      TransformationStatsData* tp = TransformationStatsData::SafePointerDownCast(statsDataArray[i].get());
      tp->setPhaseFraction((fractions[i] / totalUnbiasedVolume));
      m_DistributionAnalysis[m_SizeDistributionFitType]->calculateCorrelatedParameters(values[i], sizedist[i]);
      tp->setFeatureSizeDistribution(sizedist[i]);
      DistributionAnalysisOps::determinemaxandminvalues(values[i][0], maxdiam, mindiam);
      int numbins = int(maxdiam / m_SizeCorrelationResolution) + 1;
      tp->setFeatureDiameterInfo(m_SizeCorrelationResolution, maxdiam, mindiam);
      binnumbers = FloatArrayType::CreateArray(numbins, DREAM3D::HDF5::BinNumber);
      DistributionAnalysisOps::determinebinnumbers(maxdiam, mindiam, m_SizeCorrelationResolution, binnumbers);

      tp->setBinNumbers(binnumbers);
    }
  }
}
void GenerateEnsembleStatistics::gatherAspectRatioStats()
{
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

  StatsData::Pointer stats_data = StatsData::New();

  StatsDataArray& statsDataArray = *m_StatsDataArray;

  size_t bin;

  QVector<VectorOfFloatArray> boveras;
  QVector<VectorOfFloatArray> coveras;
  QVector<QVector<QVector<float> > > bvalues;
  QVector<QVector<QVector<float> > > cvalues;
  QVector<float> mindiams;
  QVector<float> binsteps;
  size_t numfeatures = m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->getNumTuples();
  size_t numensembles = m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->getNumTuples();

  boveras.resize(numensembles);
  coveras.resize(numensembles);
  bvalues.resize(numensembles);
  cvalues.resize(numensembles);
  mindiams.resize(numensembles);
  binsteps.resize(numensembles);
  for(size_t i = 1; i < numensembles; i++)
  {
    if(m_PhaseTypes[i] == DREAM3D::PhaseType::PrimaryPhase)
    {
      PrimaryStatsData* pp = PrimaryStatsData::SafePointerDownCast(statsDataArray[i].get());
      boveras[i] = pp->CreateCorrelatedDistributionArrays(m_AspectRatioDistributionFitType, pp->getBinNumbers()->GetSize());
      coveras[i] = pp->CreateCorrelatedDistributionArrays(m_AspectRatioDistributionFitType, pp->getBinNumbers()->GetSize());
      bvalues[i].resize(pp->getBinNumbers()->GetSize());
      cvalues[i].resize(pp->getBinNumbers()->GetSize());
      mindiams[i] = pp->getMinFeatureDiameter();
      binsteps[i] = pp->getBinStepSize();
    }
    if(m_PhaseTypes[i] == DREAM3D::PhaseType::PrecipitatePhase)
    {
      PrecipitateStatsData* pp = PrecipitateStatsData::SafePointerDownCast(statsDataArray[i].get());
      boveras[i] = pp->CreateCorrelatedDistributionArrays(m_AspectRatioDistributionFitType, pp->getBinNumbers()->GetSize());
      coveras[i] = pp->CreateCorrelatedDistributionArrays(m_AspectRatioDistributionFitType, pp->getBinNumbers()->GetSize());
      bvalues[i].resize(pp->getBinNumbers()->GetSize());
      cvalues[i].resize(pp->getBinNumbers()->GetSize());
      mindiams[i] = pp->getMinFeatureDiameter();
      binsteps[i] = pp->getBinStepSize();
    }
    if(m_PhaseTypes[i] == DREAM3D::PhaseType::TransformationPhase)
    {
      TransformationStatsData* tp = TransformationStatsData::SafePointerDownCast(statsDataArray[i].get());
      boveras[i] = tp->CreateCorrelatedDistributionArrays(m_AspectRatioDistributionFitType, tp->getBinNumbers()->GetSize());
      coveras[i] = tp->CreateCorrelatedDistributionArrays(m_AspectRatioDistributionFitType, tp->getBinNumbers()->GetSize());
      bvalues[i].resize(tp->getBinNumbers()->GetSize());
      cvalues[i].resize(tp->getBinNumbers()->GetSize());
      mindiams[i] = tp->getMinFeatureDiameter();
      binsteps[i] = tp->getBinStepSize();
    }
  }
  for (size_t i = 1; i < numfeatures; i++)
  {
    if(m_BiasedFeatures[i] == false)
    {
      bin = size_t((m_EquivalentDiameters[i] - mindiams[m_FeaturePhases[i]]) / binsteps[m_FeaturePhases[i]]);
      bvalues[m_FeaturePhases[i]][bin].push_back(m_AspectRatios[2 * i]);
      cvalues[m_FeaturePhases[i]][bin].push_back(m_AspectRatios[2 * i + 1]);
    }
  }
  for (size_t i = 1; i < numensembles; i++)
  {
    if(m_PhaseTypes[i] == DREAM3D::PhaseType::PrimaryPhase)
    {
      PrimaryStatsData* pp = PrimaryStatsData::SafePointerDownCast(statsDataArray[i].get());
      m_DistributionAnalysis[m_AspectRatioDistributionFitType]->calculateCorrelatedParameters(bvalues[i], boveras[i]);
      m_DistributionAnalysis[m_AspectRatioDistributionFitType]->calculateCorrelatedParameters(cvalues[i], coveras[i]);
      pp->setFeatureSize_BOverA(boveras[i]);
      pp->setFeatureSize_COverA(coveras[i]);
    }
    if(m_PhaseTypes[i] == DREAM3D::PhaseType::PrecipitatePhase)
    {
      PrecipitateStatsData* pp = PrecipitateStatsData::SafePointerDownCast(statsDataArray[i].get());
      m_DistributionAnalysis[m_AspectRatioDistributionFitType]->calculateCorrelatedParameters(bvalues[i], boveras[i]);
      m_DistributionAnalysis[m_AspectRatioDistributionFitType]->calculateCorrelatedParameters(cvalues[i], coveras[i]);
      pp->setFeatureSize_BOverA(boveras[i]);
      pp->setFeatureSize_COverA(coveras[i]);
    }
    if(m_PhaseTypes[i] == DREAM3D::PhaseType::TransformationPhase)
    {
      TransformationStatsData* tp = TransformationStatsData::SafePointerDownCast(statsDataArray[i].get());
      m_DistributionAnalysis[m_AspectRatioDistributionFitType]->calculateCorrelatedParameters(bvalues[i], boveras[i]);
      m_DistributionAnalysis[m_AspectRatioDistributionFitType]->calculateCorrelatedParameters(cvalues[i], coveras[i]);
      tp->setFeatureSize_BOverA(boveras[i]);
      tp->setFeatureSize_COverA(coveras[i]);
    }
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateEnsembleStatistics::gatherOmega3Stats()
{
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

  StatsDataArray& statsDataArray = *m_StatsDataArray;

  size_t bin;

  QVector<VectorOfFloatArray> omega3s;
  QVector<QVector<QVector<float> > > values;
  QVector<float> mindiams;
  QVector<float> binsteps;
  size_t numfeatures = m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->getNumTuples();
  size_t numensembles = m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->getNumTuples();

  omega3s.resize(numensembles);
  values.resize(numensembles);
  mindiams.resize(numensembles);
  binsteps.resize(numensembles);
  for(size_t i = 1; i < numensembles; i++)
  {
    if(m_PhaseTypes[i] == DREAM3D::PhaseType::PrimaryPhase)
    {
      PrimaryStatsData* pp = PrimaryStatsData::SafePointerDownCast(statsDataArray[i].get());
      omega3s[i] = pp->CreateCorrelatedDistributionArrays(m_Omega3DistributionFitType, pp->getBinNumbers()->GetSize());
      values[i].resize(pp->getBinNumbers()->GetSize());
      mindiams[i] = pp->getMinFeatureDiameter();
      binsteps[i] = pp->getBinStepSize();
    }
    if(m_PhaseTypes[i] == DREAM3D::PhaseType::PrecipitatePhase)
    {
      PrecipitateStatsData* pp = PrecipitateStatsData::SafePointerDownCast(statsDataArray[i].get());
      omega3s[i] = pp->CreateCorrelatedDistributionArrays(m_Omega3DistributionFitType, pp->getBinNumbers()->GetSize());
      values[i].resize(pp->getBinNumbers()->GetSize());
      mindiams[i] = pp->getMinFeatureDiameter();
      binsteps[i] = pp->getBinStepSize();
    }
    if(m_PhaseTypes[i] == DREAM3D::PhaseType::TransformationPhase)
    {
      TransformationStatsData* tp = TransformationStatsData::SafePointerDownCast(statsDataArray[i].get());
      omega3s[i] = tp->CreateCorrelatedDistributionArrays(m_Omega3DistributionFitType, tp->getBinNumbers()->GetSize());
      values[i].resize(tp->getBinNumbers()->GetSize());
      mindiams[i] = tp->getMinFeatureDiameter();
      binsteps[i] = tp->getBinStepSize();
    }
  }
  for (size_t i = 1; i < numfeatures; i++)
  {
    if(m_BiasedFeatures[i] == false)
    {
      bin = size_t((m_EquivalentDiameters[i] - mindiams[m_FeaturePhases[i]]) / binsteps[m_FeaturePhases[i]]);
      values[m_FeaturePhases[i]][bin].push_back(m_Omega3s[i]);
    }
  }
  for (size_t i = 1; i < numensembles; i++)
  {
    if(m_PhaseTypes[i] == DREAM3D::PhaseType::PrimaryPhase)
    {
      PrimaryStatsData* pp = PrimaryStatsData::SafePointerDownCast(statsDataArray[i].get());
      m_DistributionAnalysis[m_Omega3DistributionFitType]->calculateCorrelatedParameters(values[i], omega3s[i]);
      pp->setFeatureSize_Omegas(omega3s[i]);
    }
    if(m_PhaseTypes[i] == DREAM3D::PhaseType::PrecipitatePhase)
    {
      PrecipitateStatsData* pp = PrecipitateStatsData::SafePointerDownCast(statsDataArray[i].get());
      m_DistributionAnalysis[m_Omega3DistributionFitType]->calculateCorrelatedParameters(values[i], omega3s[i]);
      pp->setFeatureSize_Omegas(omega3s[i]);
    }
    if(m_PhaseTypes[i] == DREAM3D::PhaseType::TransformationPhase)
    {
      TransformationStatsData* tp = TransformationStatsData::SafePointerDownCast(statsDataArray[i].get());
      m_DistributionAnalysis[m_Omega3DistributionFitType]->calculateCorrelatedParameters(values[i], omega3s[i]);
      tp->setFeatureSize_Omegas(omega3s[i]);
    }
  }
}
void GenerateEnsembleStatistics::gatherNeighborhoodStats()
{
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

  StatsDataArray& statsDataArray = *m_StatsDataArray;

  size_t bin;
  QVector<VectorOfFloatArray> neighborhoods;
  QVector<QVector<QVector<float > > > values;
  QVector<float> mindiams;
  QVector<float> binsteps;
  size_t numfeatures = m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->getNumTuples();
  size_t numensembles = m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->getNumTuples();

  neighborhoods.resize(numensembles);
  values.resize(numensembles);
  mindiams.resize(numensembles);
  binsteps.resize(numensembles);
  for(size_t i = 1; i < numensembles; i++)
  {
    if(m_PhaseTypes[i] == DREAM3D::PhaseType::PrimaryPhase)
    {
      PrimaryStatsData* pp = PrimaryStatsData::SafePointerDownCast(statsDataArray[i].get());
      neighborhoods[i] = pp->CreateCorrelatedDistributionArrays(m_NeighborhoodDistributionFitType, pp->getBinNumbers()->GetSize());
      values[i].resize(pp->getBinNumbers()->GetSize());
      mindiams[i] = pp->getMinFeatureDiameter();
      binsteps[i] = pp->getBinStepSize();
    }
    if(m_PhaseTypes[i] == DREAM3D::PhaseType::PrecipitatePhase)
    {
      PrecipitateStatsData* pp = PrecipitateStatsData::SafePointerDownCast(statsDataArray[i].get());
      neighborhoods[i] = pp->CreateCorrelatedDistributionArrays(m_NeighborhoodDistributionFitType, pp->getBinNumbers()->GetSize());
      values[i].resize(pp->getBinNumbers()->GetSize());
      mindiams[i] = pp->getMinFeatureDiameter();
      binsteps[i] = pp->getBinStepSize();
    }
    if(m_PhaseTypes[i] == DREAM3D::PhaseType::TransformationPhase)
    {
      TransformationStatsData* tp = TransformationStatsData::SafePointerDownCast(statsDataArray[i].get());
      neighborhoods[i] = tp->CreateCorrelatedDistributionArrays(m_NeighborhoodDistributionFitType, tp->getBinNumbers()->GetSize());
      values[i].resize(tp->getBinNumbers()->GetSize());
      mindiams[i] = tp->getMinFeatureDiameter();
      binsteps[i] = tp->getBinStepSize();
    }
  }

  for (size_t i = 1; i < numfeatures; i++)
  {
    if(m_BiasedFeatures[i] == false)
    {
      bin = size_t((m_EquivalentDiameters[i] - mindiams[m_FeaturePhases[i]]) / binsteps[m_FeaturePhases[i]]);
      values[m_FeaturePhases[i]][bin].push_back(static_cast<float>( m_Neighborhoods[i] ));
    }
  }
  for (size_t i = 1; i < numensembles; i++)
  {
    if(m_PhaseTypes[i] == DREAM3D::PhaseType::PrimaryPhase)
    {
      PrimaryStatsData* pp = PrimaryStatsData::SafePointerDownCast(statsDataArray[i].get());
      m_DistributionAnalysis[m_NeighborhoodDistributionFitType]->calculateCorrelatedParameters(values[i], neighborhoods[i]);
      pp->setFeatureSize_Neighbors(neighborhoods[i]);
    }
    if(m_PhaseTypes[i] == DREAM3D::PhaseType::PrecipitatePhase)
    {
      PrecipitateStatsData* pp = PrecipitateStatsData::SafePointerDownCast(statsDataArray[i].get());
      m_DistributionAnalysis[m_NeighborhoodDistributionFitType]->calculateCorrelatedParameters(values[i], neighborhoods[i]);
      pp->setFeatureSize_Clustering(neighborhoods[i]);
    }
    if(m_PhaseTypes[i] == DREAM3D::PhaseType::TransformationPhase)
    {
      TransformationStatsData* tp = TransformationStatsData::SafePointerDownCast(statsDataArray[i].get());
      m_DistributionAnalysis[m_NeighborhoodDistributionFitType]->calculateCorrelatedParameters(values[i], neighborhoods[i]);
      tp->setFeatureSize_Neighbors(neighborhoods[i]);
    }
  }

}
void GenerateEnsembleStatistics::gatherODFStats()
{
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

  setErrorCondition(0);

  StatsDataArray& statsDataArray = *m_StatsDataArray;

  size_t bin;
  size_t numfeatures = m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->getNumTuples();
  int phase;
  QVector<float> totalvol;
  QVector<FloatArrayType::Pointer> eulerodf;

  size_t numensembles = m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->getNumTuples();

  totalvol.resize(numensembles);
  eulerodf.resize(numensembles);
  unsigned long long dims = 0;
  for(unsigned long long i = 1; i < numensembles; i++)
  {
    totalvol[i] = 0;
    if (m_CrystalStructures[i] == Ebsd::CrystalStructure::Hexagonal_High)
    {
      dims = 36 * 36 * 12;
      eulerodf[i] = FloatArrayType::CreateArray(dims, DREAM3D::HDF5::ODF);
      for (unsigned long long j = 0; j < dims; j++)
      {
        eulerodf[i]->SetValue(j, 0.0);
      }
    }
    else if (m_CrystalStructures[i] == Ebsd::CrystalStructure::Cubic_High)
    {
      dims = 18 * 18 * 18;
      eulerodf[i] = FloatArrayType::CreateArray(dims, DREAM3D::HDF5::ODF);
      for (unsigned long long j = 0; j < dims; j++)
      {
        eulerodf[i]->SetValue(j, 0.0);
      }
    }
  }
  float ea1, ea2, ea3;
  float r1, r2, r3;
  for (size_t i = 1; i < numfeatures; i++)
  {
    if (m_SurfaceFeatures[i] == false)
    {
      totalvol[m_FeaturePhases[i]] = totalvol[m_FeaturePhases[i]] + m_Volumes[i];
    }
  }
  for (size_t i = 1; i < numfeatures; i++)
  {
    if (m_SurfaceFeatures[i] == false)
    {
      ea1 = m_FeatureEulerAngles[3 * i];
      ea2 = m_FeatureEulerAngles[3 * i + 1];
      ea3 = m_FeatureEulerAngles[3 * i + 2];
      phase = m_CrystalStructures[m_FeaturePhases[i]];
      OrientationMath::EulertoRod(r1, r2, r3, ea1, ea2, ea3);
      bin = m_OrientationOps[phase]->getOdfBin(r1, r2, r3);
      eulerodf[m_FeaturePhases[i]]->SetValue(bin, (eulerodf[m_FeaturePhases[i]]->GetValue(bin) + (m_Volumes[i] / totalvol[m_FeaturePhases[i]])));
    }
  }

  for(size_t i = 1; i < numensembles; i++)
  {
    if(m_PhaseTypes[i] == DREAM3D::PhaseType::PrimaryPhase)
    {
      PrimaryStatsData* pp = PrimaryStatsData::SafePointerDownCast(statsDataArray[i].get());
      pp->setODF(eulerodf[i]);
    }
    if(m_PhaseTypes[i] == DREAM3D::PhaseType::PrecipitatePhase)
    {
      PrecipitateStatsData* pp = PrecipitateStatsData::SafePointerDownCast(statsDataArray[i].get());
      pp->setODF(eulerodf[i]);
    }
    if(m_PhaseTypes[i] == DREAM3D::PhaseType::TransformationPhase)
    {
      TransformationStatsData* tp = TransformationStatsData::SafePointerDownCast(statsDataArray[i].get());
      tp->setODF(eulerodf[i]);
    }
  }

}
void GenerateEnsembleStatistics::gatherMDFStats()
{
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

  setErrorCondition(0);

  StatsDataArray& statsDataArray = *m_StatsDataArray;

  // But since a pointer is difficult to use operators with we will now create a
  // reference variable to the pointer with the correct variable name that allows
  // us to use the same syntax as the "vector of vectors"
  NeighborList<int>& neighborlist = *m_NeighborList;
  // And we do the same for the SharedSurfaceArea list
  NeighborList<float>& neighborsurfacearealist = *m_SharedSurfaceAreaList;

  float n1 = 0.0f, n2 = 0.0f, n3 = 0.0f;
  float r1 = 0.0f, r2 = 0.0f, r3 = 0.0f;
  int mbin = 0;
  float w;
  QuatF q1;
  QuatF q2;
  QuatF* avgQuats = reinterpret_cast<QuatF*>(m_AvgQuats);

  size_t numfeatures = m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->getNumTuples();
  unsigned int phase1, phase2;
  QVector<float> totalSurfaceArea;
  QVector<FloatArrayType::Pointer> misobin;
  int numbins = 0;

  size_t numensembles = m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->getNumTuples();

  misobin.resize(numensembles);
  totalSurfaceArea.resize(numensembles);
  for(size_t i = 1; i < numensembles; ++i)
  {
    totalSurfaceArea[i] = 0;
    if (Ebsd::CrystalStructure::Hexagonal_High == m_CrystalStructures[i] )
    {
      numbins = 36 * 36 * 12;
      misobin[i] = FloatArrayType::CreateArray(numbins, DREAM3D::HDF5::MisorientationBins);
    }
    else if (Ebsd::CrystalStructure::Cubic_High == m_CrystalStructures[i] )
    {
      numbins = 18 * 18 * 18;
      misobin[i] = FloatArrayType::CreateArray(numbins, DREAM3D::HDF5::MisorientationBins);
    }
    // Now initialize all bins to 0.0
    for (int j = 0; j < numbins; j++)
    {
      misobin[i]->SetValue(j, 0.0);
    }
  }
  size_t nname;
  float nsa;
  for (size_t i = 1; i < numfeatures; i++)
  {
    QuaternionMathF::Copy(avgQuats[i], q1);
    phase1 = m_CrystalStructures[m_FeaturePhases[i]];
    for (size_t j = 0; j < neighborlist[i].size(); j++)
    {
      w = 10000.0;
      nname = neighborlist[i][j];
      QuaternionMathF::Copy(avgQuats[nname], q2);
      phase2 = m_CrystalStructures[m_FeaturePhases[nname]];
      if (phase1 == phase2) { w = m_OrientationOps[phase1]->getMisoQuat( q1, q2, n1, n2, n3); }
      if (phase1 == phase2)
      {
        OrientationMath::AxisAngletoRod(w, n1, n2, n3, r1, r2, r3);
        if ((nname > i || m_SurfaceFeatures[nname] == true))
        {
          mbin = m_OrientationOps[phase1]->getMisoBin(r1, r2, r3);
          nsa = neighborsurfacearealist[i][j];
          misobin[m_FeaturePhases[i]]->SetValue(mbin, (misobin[m_FeaturePhases[i]]->GetValue(mbin) + nsa));
          totalSurfaceArea[m_FeaturePhases[i]] = totalSurfaceArea[m_FeaturePhases[i]] + nsa;
        }
      }
    }
  }

  // unsigned long long int dims = static_cast<unsigned long long int>(numbins);
  for (size_t i = 1; i < numensembles; i++)
  {
    for(size_t j = 0; j < misobin[i]->GetSize(); j++)
    {
      misobin[i]->SetValue(j, (misobin[i]->GetValue(j) / totalSurfaceArea[i]));
    }
    if(m_PhaseTypes[i] == DREAM3D::PhaseType::PrimaryPhase)
    {
      PrimaryStatsData* pp = PrimaryStatsData::SafePointerDownCast(statsDataArray[i].get());
      pp->setMisorientationBins(misobin[i]);
      pp->setBoundaryArea(totalSurfaceArea[i]);
    }
    if(m_PhaseTypes[i] == DREAM3D::PhaseType::PrecipitatePhase)
    {
      PrecipitateStatsData* pp = PrecipitateStatsData::SafePointerDownCast(statsDataArray[i].get());
      pp->setMisorientationBins(misobin[i]);
      pp->setBoundaryArea(totalSurfaceArea[i]);
    }
    if(m_PhaseTypes[i] == DREAM3D::PhaseType::TransformationPhase)
    {
      TransformationStatsData* tp = TransformationStatsData::SafePointerDownCast(statsDataArray[i].get());
      tp->setMisorientationBins(misobin[i]);
      tp->setBoundaryArea(totalSurfaceArea[i]);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateEnsembleStatistics::gatherAxisODFStats()
{
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

  setErrorCondition(0);

  StatsDataArray& statsDataArray = *m_StatsDataArray;

  float r1, r2, r3;
  int bin;
  QVector<FloatArrayType::Pointer> axisodf;
  QVector<float> totalaxes;
  size_t numXTals = m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->getNumTuples();
  axisodf.resize(numXTals);
  totalaxes.resize(numXTals);
  for (size_t i = 1; i < numXTals; i++)
  {
    totalaxes[i] = 0.0;
    axisodf[i] = FloatArrayType::CreateArray((36 * 36 * 36), DREAM3D::HDF5::AxisOrientation);
    for (int j = 0; j < (36 * 36 * 36); j++)
    {
      axisodf[i]->SetValue(j, 0.0);
    }
  }
  size_t numfeatures = m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->getNumTuples();
  for (size_t i = 1; i < numfeatures; i++)
  {
    if(m_SurfaceFeatures[i] == false)
    {
      totalaxes[m_FeaturePhases[i]]++;
    }
  }
  for (size_t i = 1; i < numfeatures; i++)
  {
    float ea1 = m_AxisEulerAngles[3 * i];
    float ea2 = m_AxisEulerAngles[3 * i + 1];
    float ea3 = m_AxisEulerAngles[3 * i + 2];
    if(m_SurfaceFeatures[i] == 0)
    {
      OrientationMath::EulertoRod(r1, r2, r3, ea1, ea2, ea3);
      m_OrientationOps[Ebsd::CrystalStructure::OrthoRhombic]->getODFFZRod(r1, r2, r3);
      bin = m_OrientationOps[Ebsd::CrystalStructure::OrthoRhombic]->getOdfBin(r1, r2, r3);
      axisodf[m_FeaturePhases[i]]->SetValue(bin, (axisodf[m_FeaturePhases[i]]->GetValue(bin) + static_cast<float>((1.0 / totalaxes[m_FeaturePhases[i]]))));
    }
  }
  // int err;
  for (size_t i = 1; i < numXTals; i++)
  {
    if(m_PhaseTypes[i] == DREAM3D::PhaseType::PrimaryPhase)
    {
      PrimaryStatsData* pp = PrimaryStatsData::SafePointerDownCast(statsDataArray[i].get());
      pp->setAxisOrientation(axisodf[i]);
    }
    if(m_PhaseTypes[i] == DREAM3D::PhaseType::PrecipitatePhase)
    {
      PrecipitateStatsData* pp = PrecipitateStatsData::SafePointerDownCast(statsDataArray[i].get());
      pp->setAxisOrientation(axisodf[i]);
    }
    if(m_PhaseTypes[i] == DREAM3D::PhaseType::TransformationPhase)
    {
      TransformationStatsData* tp = TransformationStatsData::SafePointerDownCast(statsDataArray[i].get());
      tp->setAxisOrientation(axisodf[i]);
    }
  }
}
