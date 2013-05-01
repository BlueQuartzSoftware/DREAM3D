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

#include "DREAM3DLib/Common/DREAM3DMath.h"
#include "DREAM3DLib/Common/Constants.h"

#include "DREAM3DLib/DistributionAnalysisOps/BetaOps.h"
#include "DREAM3DLib/DistributionAnalysisOps/PowerLawOps.h"
#include "DREAM3DLib/DistributionAnalysisOps/LogNormalOps.h"

#include "DREAM3DLib/StatisticsFilters/FindNeighbors.h"
#include "DREAM3DLib/GenericFilters/FindSurfaceGrains.h"
#include "DREAM3DLib/GenericFilters/FindGrainPhases.h"

const static float m_pi = static_cast<float>(M_PI);

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GenerateEnsembleStatistics::GenerateEnsembleStatistics()  :
  AbstractFilter(),
  m_AvgQuatsArrayName(DREAM3D::FieldData::AvgQuats),
  m_BiasedFieldsArrayName(DREAM3D::FieldData::BiasedFields),
  m_VolumesArrayName(DREAM3D::FieldData::Volumes),
  m_FieldEulerAnglesArrayName(DREAM3D::FieldData::FieldEulerAngles),
  m_FieldPhasesArrayName(DREAM3D::FieldData::Phases),
  m_SurfaceFieldsArrayName(DREAM3D::FieldData::SurfaceFields),
  m_EquivalentDiametersArrayName(DREAM3D::FieldData::EquivalentDiameters),
  m_AspectRatiosArrayName(DREAM3D::FieldData::AspectRatios),
  m_Omega3sArrayName(DREAM3D::FieldData::Omega3s),
  m_NeighborhoodsArrayName(DREAM3D::FieldData::Neighborhoods),
  m_AxisEulerAnglesArrayName(DREAM3D::FieldData::AxisEulerAngles),
  m_CrystalStructuresArrayName(DREAM3D::EnsembleData::CrystalStructures),
  m_TotalSurfaceAreasArrayName(DREAM3D::EnsembleData::TotalSurfaceAreas),
  m_PhaseTypesArrayName(DREAM3D::EnsembleData::PhaseTypes),
  m_SizeCorrelationResolution(1),
  m_SizeDistribution(false),
  m_SizeDistributionFitType(DREAM3D::DistributionType::LogNormal),
  m_AspectRatioDistribution(false),
  m_AspectRatioDistributionFitType(DREAM3D::DistributionType::LogNormal),
  m_Omega3Distribution(false),
  m_Omega3DistributionFitType(DREAM3D::DistributionType::LogNormal),
  m_NeighborhoodDistribution(false),
  m_NeighborhoodDistributionFitType(DREAM3D::DistributionType::LogNormal),
  m_CalculateODF(false),
  m_CalculateMDF(false),
  m_CalculateAxisODF(false),
  m_AvgQuats(NULL),
  m_FieldEulerAngles(NULL),
  m_Volumes(NULL),
  m_BiasedFields(NULL),
  m_SurfaceFields(NULL),
  m_FieldPhases(NULL),
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

  m_HexOps = HexagonalOps::New();
  m_OrientationOps.push_back(dynamic_cast<OrientationMath*> (m_HexOps.get()));

  m_CubicOps = CubicOps::New();
  m_OrientationOps.push_back(dynamic_cast<OrientationMath*> (m_CubicOps.get()));

  m_OrthoOps = OrthoRhombicOps::New();
  m_OrientationOps.push_back(dynamic_cast<OrientationMath*> (m_OrthoOps.get()));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GenerateEnsembleStatistics::~GenerateEnsembleStatistics()
{
}
// -----------------------------------------------------------------------------
void GenerateEnsembleStatistics::writeFilterParameters(AbstractFilterParametersWriter* writer)
{
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateEnsembleStatistics::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;
  VoxelDataContainer* m = getVoxelDataContainer();

  GET_PREREQ_DATA(m, DREAM3D, FieldData, FieldPhases, ss, -303, int32_t, Int32ArrayType, fields, 1)


      if(m_SizeDistribution == true || m_Omega3Distribution == true
         || m_AspectRatioDistribution == true || m_NeighborhoodDistribution == true || m_CalculateAxisODF == true)
  {
    GET_PREREQ_DATA(m, DREAM3D, FieldData, BiasedFields, ss, -302, bool, BoolArrayType, fields, 1)
        GET_PREREQ_DATA(m, DREAM3D, FieldData, EquivalentDiameters, ss, -302, float, FloatArrayType, fields, 1)
  }
  if(m_NeighborhoodDistribution == true)
  {
    GET_PREREQ_DATA(m, DREAM3D, FieldData, Neighborhoods, ss, -304, int32_t, Int32ArrayType, fields, 1)
  }
  if(m_AspectRatioDistribution == true)
  {
    GET_PREREQ_DATA(m, DREAM3D, FieldData, AspectRatios, ss, -307, float, FloatArrayType, fields, 2)
  }
  if(m_Omega3Distribution == true)
  {
    GET_PREREQ_DATA(m, DREAM3D, FieldData, Omega3s, ss, -306, float, FloatArrayType, fields, 1)
  }
  if(m_CalculateAxisODF == true)
  {
    GET_PREREQ_DATA(m, DREAM3D, FieldData, AxisEulerAngles, ss, -305, float, FloatArrayType, fields, 3)
  }


  if(m_CalculateODF == true || m_CalculateMDF == true)
  {
    typedef DataArray<unsigned int> XTalStructArrayType;
    GET_PREREQ_DATA(m, DREAM3D, EnsembleData, CrystalStructures, ss, -305, unsigned int, XTalStructArrayType, ensembles, 1)
        GET_PREREQ_DATA(m, DREAM3D, FieldData, SurfaceFields, ss, -302, bool, BoolArrayType, fields, 1)
  }
  if(m_CalculateODF == true)
  {
    GET_PREREQ_DATA(m, DREAM3D, FieldData, Volumes, ss, -304, float, FloatArrayType, fields, 1)
        GET_PREREQ_DATA(m, DREAM3D, FieldData, FieldEulerAngles, ss, -302, float, FloatArrayType, fields, 3)
  }
  if(m_CalculateMDF == true)
  {
    GET_PREREQ_DATA(m, DREAM3D, FieldData, AvgQuats, ss, -301, float, FloatArrayType, fields, 5)
        m_SharedSurfaceAreaList = NeighborList<float>::SafeObjectDownCast<IDataArray*, NeighborList<float>*>(m->getFieldData(DREAM3D::FieldData::SharedSurfaceAreaList).get());
    if(m_SharedSurfaceAreaList == NULL)
    {
      ss.str("");
      ss << "SurfaceAreaLists Array Not Initialized correctly" << std::endl;
      setErrorCondition(-306);
      addErrorMessage(getHumanLabel(), ss.str(), -306);
    }
    // Now we are going to get a "Pointer" to the NeighborList object out of the DataContainer
    m_NeighborList = NeighborList<int>::SafeObjectDownCast<IDataArray*, NeighborList<int>*>(m->getFieldData(DREAM3D::FieldData::NeighborList).get());
    if(m_NeighborList == NULL)
    {
      ss.str("");
      ss << "NeighborLists Array Not Initialized correctly" << std::endl;
      setErrorCondition(-305);
      addErrorMessage(getHumanLabel(), ss.str(), -305);
    }
  }


  typedef DataArray<unsigned int> PhaseTypeArrayType;
  CREATE_NON_PREREQ_DATA(m, DREAM3D, EnsembleData, PhaseTypes, ss, unsigned int, PhaseTypeArrayType, DREAM3D::PhaseType::UnknownPhaseType, ensembles, 1)

      m_StatsDataArray = StatsDataArray::SafeObjectDownCast<IDataArray*, StatsDataArray*>(m->getEnsembleData(DREAM3D::EnsembleData::Statistics).get());
  if(m_StatsDataArray == NULL)
  {
    StatsDataArray::Pointer p = StatsDataArray::New();
    m_StatsDataArray = p.get();
    m_StatsDataArray->fillArrayWithNewStatsData(ensembles, m_PhaseTypes);
    m->addEnsembleData(DREAM3D::EnsembleData::Statistics, p);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateEnsembleStatistics::preflight()
{
  dataCheck(true, 1, 1, 1);
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateEnsembleStatistics::execute()
{
  VoxelDataContainer* m = getVoxelDataContainer();
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The DataContainer Object was NULL", -999);
    return;
  }
  setErrorCondition(0);
  std::stringstream ss;

  //  int totalPoints = m->getTotalPoints();
  //  int totalFields = m->getNumFieldTuples();
  int totalEnsembles = m->getNumEnsembleTuples();
  dataCheck(false, m->getTotalPoints(), m->getNumFieldTuples(), m->getNumEnsembleTuples());
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
      ss.str("");
      ss << "The number of PhaseTypes entered is more than the number of Ensembles, only the first " << totalEnsembles-1 << " will be used";
      notifyWarningMessage(ss.str(), -999);
      return;
    }
    PhaseTypeArrayType::Pointer phaseTypes = PhaseTypeArrayType::CreateArray(totalEnsembles, m_PhaseTypesArrayName);
    for(int r = 0; r < totalEnsembles; ++r)
    {
      phaseTypes->SetValue(r, m_PhaseTypeArray[r]);
    }
    m->addEnsembleData(phaseTypes->GetName(), phaseTypes);
    m_PhaseTypes = phaseTypes->GetPointer(0);
  }

  StatsDataArray::Pointer p = StatsDataArray::New();
  m_StatsDataArray = p.get();
  m_StatsDataArray->fillArrayWithNewStatsData(totalEnsembles, m_PhaseTypes);
  m->addEnsembleData(DREAM3D::EnsembleData::Statistics, p);

  dataCheck(false, m->getTotalPoints(), m->getNumFieldTuples(), m->getNumEnsembleTuples());

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
  VoxelDataContainer* m = getVoxelDataContainer();
  // int64_t totalPoints = m->getTotalPoints();

  StatsDataArray& statsDataArray = *m_StatsDataArray;

  float maxdiam;
  float mindiam;
  float totalUnbiasedVolume = 0.0;
  std::vector<VectorOfFloatArray> sizedist;
  std::vector<std::vector<std::vector<float > > > values;
  std::vector<float> fractions;
  FloatArrayType::Pointer binnumbers;
  size_t numgrains = m->getNumFieldTuples();
  size_t numensembles = m->getNumEnsembleTuples();

  sizedist.resize(numensembles);
  values.resize(numensembles);
  fractions.resize(numensembles,0.0);
  for(size_t i = 1; i < numensembles; i++)
  {
    sizedist[i] = statsDataArray[i]->CreateCorrelatedDistributionArrays(m_SizeDistributionFitType, 1);
    values[i].resize(1);
  }

  float vol;
  for (size_t i = 1; i < numgrains; i++)
  {
    if(m_BiasedFields[i] == false)
    {
      values[m_FieldPhases[i]][0].push_back(m_EquivalentDiameters[i]);
      vol = (1.0/6.0)*m_pi*m_EquivalentDiameters[i]*m_EquivalentDiameters[i]*m_EquivalentDiameters[i];
      fractions[m_FieldPhases[i]] = fractions[m_FieldPhases[i]] + vol;
      totalUnbiasedVolume = totalUnbiasedVolume + vol;
    }
  }
  for (size_t i = 1; i < numensembles; i++)
  {
    if(m_PhaseTypes[i] == DREAM3D::PhaseType::PrimaryPhase)
    {
      PrimaryStatsData* pp = PrimaryStatsData::SafePointerDownCast(statsDataArray[i].get());
      pp->setPhaseFraction((fractions[i]/totalUnbiasedVolume));
      m_DistributionAnalysis[m_SizeDistributionFitType]->calculateCorrelatedParameters(values[i], sizedist[i]);
      pp->setGrainSizeDistribution(sizedist[i]);
      DistributionAnalysisOps::determinemaxandminvalues(values[i][0], maxdiam, mindiam);
      int numbins = int(maxdiam/m_SizeCorrelationResolution)+1;
      pp->setGrainDiameterInfo(m_SizeCorrelationResolution, maxdiam, mindiam);
      binnumbers = FloatArrayType::CreateArray(numbins, DREAM3D::HDF5::BinNumber);
      DistributionAnalysisOps::determinebinnumbers(maxdiam, mindiam, m_SizeCorrelationResolution, binnumbers);
      pp->setBinNumbers(binnumbers);
    }
    if(m_PhaseTypes[i] == DREAM3D::PhaseType::PrecipitatePhase)
    {
      PrecipitateStatsData* pp = PrecipitateStatsData::SafePointerDownCast(statsDataArray[i].get());
      pp->setPhaseFraction((fractions[i]/totalUnbiasedVolume));
      m_DistributionAnalysis[m_SizeDistributionFitType]->calculateCorrelatedParameters(values[i], sizedist[i]);
      pp->setGrainSizeDistribution(sizedist[i]);
      DistributionAnalysisOps::determinemaxandminvalues(values[i][0], maxdiam, mindiam);
      int numbins = int(maxdiam/m_SizeCorrelationResolution)+1;
      pp->setGrainDiameterInfo(m_SizeCorrelationResolution, maxdiam, mindiam);
      binnumbers = FloatArrayType::CreateArray(numbins, DREAM3D::HDF5::BinNumber);
      DistributionAnalysisOps::determinebinnumbers(maxdiam, mindiam, m_SizeCorrelationResolution, binnumbers);

      pp->setBinNumbers(binnumbers);
    }
    if(m_PhaseTypes[i] == DREAM3D::PhaseType::TransformationPhase)
    {
      TransformationStatsData* tp = TransformationStatsData::SafePointerDownCast(statsDataArray[i].get());
      tp->setPhaseFraction((fractions[i]/totalUnbiasedVolume));
      m_DistributionAnalysis[m_SizeDistributionFitType]->calculateCorrelatedParameters(values[i], sizedist[i]);
      tp->setGrainSizeDistribution(sizedist[i]);
      DistributionAnalysisOps::determinemaxandminvalues(values[i][0], maxdiam, mindiam);
      int numbins = int(maxdiam/m_SizeCorrelationResolution)+1;
      tp->setGrainDiameterInfo(m_SizeCorrelationResolution, maxdiam, mindiam);
      binnumbers = FloatArrayType::CreateArray(numbins, DREAM3D::HDF5::BinNumber);
      DistributionAnalysisOps::determinebinnumbers(maxdiam, mindiam, m_SizeCorrelationResolution, binnumbers);

      tp->setBinNumbers(binnumbers);
    }
  }
}
void GenerateEnsembleStatistics::gatherAspectRatioStats()
{
  VoxelDataContainer* m = getVoxelDataContainer();
  StatsData::Pointer stats_data = StatsData::New();

  StatsDataArray& statsDataArray = *m_StatsDataArray;

  size_t bin;

  std::vector<VectorOfFloatArray> boveras;
  std::vector<VectorOfFloatArray> coveras;
  std::vector<std::vector<std::vector<float> > > bvalues;
  std::vector<std::vector<std::vector<float> > > cvalues;
  std::vector<float> mindiams;
  std::vector<float> binsteps;
  size_t numgrains = m->getNumFieldTuples();
  size_t numensembles = m->getNumEnsembleTuples();

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
      mindiams[i] = pp->getMinGrainDiameter();
      binsteps[i] = pp->getBinStepSize();
    }
    if(m_PhaseTypes[i] == DREAM3D::PhaseType::PrecipitatePhase)
    {
      PrecipitateStatsData* pp = PrecipitateStatsData::SafePointerDownCast(statsDataArray[i].get());
      boveras[i] = pp->CreateCorrelatedDistributionArrays(m_AspectRatioDistributionFitType, pp->getBinNumbers()->GetSize());
      coveras[i] = pp->CreateCorrelatedDistributionArrays(m_AspectRatioDistributionFitType, pp->getBinNumbers()->GetSize());
      bvalues[i].resize(pp->getBinNumbers()->GetSize());
      cvalues[i].resize(pp->getBinNumbers()->GetSize());
      mindiams[i] = pp->getMinGrainDiameter();
      binsteps[i] = pp->getBinStepSize();
    }
    if(m_PhaseTypes[i] == DREAM3D::PhaseType::TransformationPhase)
    {
      TransformationStatsData* tp = TransformationStatsData::SafePointerDownCast(statsDataArray[i].get());
      boveras[i] = tp->CreateCorrelatedDistributionArrays(m_AspectRatioDistributionFitType, tp->getBinNumbers()->GetSize());
      coveras[i] = tp->CreateCorrelatedDistributionArrays(m_AspectRatioDistributionFitType, tp->getBinNumbers()->GetSize());
      bvalues[i].resize(tp->getBinNumbers()->GetSize());
      cvalues[i].resize(tp->getBinNumbers()->GetSize());
      mindiams[i] = tp->getMinGrainDiameter();
      binsteps[i] = tp->getBinStepSize();
    }
  }
  for (size_t i = 1; i < numgrains; i++)
  {
    if(m_BiasedFields[i] == false)
    {
      bin = size_t((m_EquivalentDiameters[i]-mindiams[m_FieldPhases[i]])/binsteps[m_FieldPhases[i]]);
      bvalues[m_FieldPhases[i]][bin].push_back(m_AspectRatios[2*i]);
      cvalues[m_FieldPhases[i]][bin].push_back(m_AspectRatios[2*i+1]);
    }
  }
  for (size_t i = 1; i < numensembles; i++)
  {
    if(m_PhaseTypes[i] == DREAM3D::PhaseType::PrimaryPhase)
    {
      PrimaryStatsData* pp = PrimaryStatsData::SafePointerDownCast(statsDataArray[i].get());
      m_DistributionAnalysis[m_AspectRatioDistributionFitType]->calculateCorrelatedParameters(bvalues[i], boveras[i]);
      m_DistributionAnalysis[m_AspectRatioDistributionFitType]->calculateCorrelatedParameters(cvalues[i], coveras[i]);
      pp->setGrainSize_BOverA(boveras[i]);
      pp->setGrainSize_COverA(coveras[i]);
    }
    if(m_PhaseTypes[i] == DREAM3D::PhaseType::PrecipitatePhase)
    {
      PrecipitateStatsData* pp = PrecipitateStatsData::SafePointerDownCast(statsDataArray[i].get());
      m_DistributionAnalysis[m_AspectRatioDistributionFitType]->calculateCorrelatedParameters(bvalues[i], boveras[i]);
      m_DistributionAnalysis[m_AspectRatioDistributionFitType]->calculateCorrelatedParameters(cvalues[i], coveras[i]);
      pp->setGrainSize_BOverA(boveras[i]);
      pp->setGrainSize_COverA(coveras[i]);
    }
    if(m_PhaseTypes[i] == DREAM3D::PhaseType::TransformationPhase)
    {
      TransformationStatsData* tp = TransformationStatsData::SafePointerDownCast(statsDataArray[i].get());
      m_DistributionAnalysis[m_AspectRatioDistributionFitType]->calculateCorrelatedParameters(bvalues[i], boveras[i]);
      m_DistributionAnalysis[m_AspectRatioDistributionFitType]->calculateCorrelatedParameters(cvalues[i], coveras[i]);
      tp->setGrainSize_BOverA(boveras[i]);
      tp->setGrainSize_COverA(coveras[i]);
    }
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateEnsembleStatistics::gatherOmega3Stats()
{
  VoxelDataContainer* m = getVoxelDataContainer();
  //int64_t totalPoints = m->getTotalPoints();

  StatsDataArray& statsDataArray = *m_StatsDataArray;

  size_t bin;

  std::vector<VectorOfFloatArray> omega3s;
  std::vector<std::vector<std::vector<float> > > values;
  std::vector<float> mindiams;
  std::vector<float> binsteps;
  size_t numgrains = m->getNumFieldTuples();
  size_t numensembles = m->getNumEnsembleTuples();

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
      mindiams[i] = pp->getMinGrainDiameter();
      binsteps[i] = pp->getBinStepSize();
    }
    if(m_PhaseTypes[i] == DREAM3D::PhaseType::PrecipitatePhase)
    {
      PrecipitateStatsData* pp = PrecipitateStatsData::SafePointerDownCast(statsDataArray[i].get());
      omega3s[i] = pp->CreateCorrelatedDistributionArrays(m_Omega3DistributionFitType, pp->getBinNumbers()->GetSize());
      values[i].resize(pp->getBinNumbers()->GetSize());
      mindiams[i] = pp->getMinGrainDiameter();
      binsteps[i] = pp->getBinStepSize();
    }
    if(m_PhaseTypes[i] == DREAM3D::PhaseType::TransformationPhase)
    {
      TransformationStatsData* tp = TransformationStatsData::SafePointerDownCast(statsDataArray[i].get());
      omega3s[i] = tp->CreateCorrelatedDistributionArrays(m_Omega3DistributionFitType, tp->getBinNumbers()->GetSize());
      values[i].resize(tp->getBinNumbers()->GetSize());
      mindiams[i] = tp->getMinGrainDiameter();
      binsteps[i] = tp->getBinStepSize();
    }
  }
  for (size_t i = 1; i < numgrains; i++)
  {
    if(m_BiasedFields[i] == false)
    {
      bin = size_t((m_EquivalentDiameters[i]-mindiams[m_FieldPhases[i]])/binsteps[m_FieldPhases[i]]);
      values[m_FieldPhases[i]][bin].push_back(m_Omega3s[i]);
    }
  }
  for (size_t i = 1; i < numensembles; i++)
  {
    if(m_PhaseTypes[i] == DREAM3D::PhaseType::PrimaryPhase)
    {
      PrimaryStatsData* pp = PrimaryStatsData::SafePointerDownCast(statsDataArray[i].get());
      m_DistributionAnalysis[m_Omega3DistributionFitType]->calculateCorrelatedParameters(values[i], omega3s[i]);
      pp->setGrainSize_Omegas(omega3s[i]);
    }
    if(m_PhaseTypes[i] == DREAM3D::PhaseType::PrecipitatePhase)
    {
      PrecipitateStatsData* pp = PrecipitateStatsData::SafePointerDownCast(statsDataArray[i].get());
      m_DistributionAnalysis[m_Omega3DistributionFitType]->calculateCorrelatedParameters(values[i], omega3s[i]);
      pp->setGrainSize_Omegas(omega3s[i]);
    }
    if(m_PhaseTypes[i] == DREAM3D::PhaseType::TransformationPhase)
    {
      TransformationStatsData* tp = TransformationStatsData::SafePointerDownCast(statsDataArray[i].get());
      m_DistributionAnalysis[m_Omega3DistributionFitType]->calculateCorrelatedParameters(values[i], omega3s[i]);
      tp->setGrainSize_Omegas(omega3s[i]);
    }
  }
}
void GenerateEnsembleStatistics::gatherNeighborhoodStats()
{
  VoxelDataContainer* m = getVoxelDataContainer();

  StatsDataArray& statsDataArray = *m_StatsDataArray;

  size_t bin;
  std::vector<VectorOfFloatArray> neighborhoods;
  std::vector<std::vector<std::vector<float > > > values;
  std::vector<float> mindiams;
  std::vector<float> binsteps;
  size_t numgrains = m->getNumFieldTuples();
  size_t numensembles = m->getNumEnsembleTuples();

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
      mindiams[i] = pp->getMinGrainDiameter();
      binsteps[i] = pp->getBinStepSize();
    }
    if(m_PhaseTypes[i] == DREAM3D::PhaseType::PrecipitatePhase)
    {
      PrecipitateStatsData* pp = PrecipitateStatsData::SafePointerDownCast(statsDataArray[i].get());
      neighborhoods[i] = pp->CreateCorrelatedDistributionArrays(m_NeighborhoodDistributionFitType, pp->getBinNumbers()->GetSize());
      values[i].resize(pp->getBinNumbers()->GetSize());
      mindiams[i] = pp->getMinGrainDiameter();
      binsteps[i] = pp->getBinStepSize();
    }
    if(m_PhaseTypes[i] == DREAM3D::PhaseType::TransformationPhase)
    {
      TransformationStatsData* tp = TransformationStatsData::SafePointerDownCast(statsDataArray[i].get());
      neighborhoods[i] = tp->CreateCorrelatedDistributionArrays(m_NeighborhoodDistributionFitType, tp->getBinNumbers()->GetSize());
      values[i].resize(tp->getBinNumbers()->GetSize());
      mindiams[i] = tp->getMinGrainDiameter();
      binsteps[i] = tp->getBinStepSize();
    }
  }

  for (size_t i = 1; i < numgrains; i++)
  {
    if(m_BiasedFields[i] == false)
    {
      bin = size_t((m_EquivalentDiameters[i]-mindiams[m_FieldPhases[i]])/binsteps[m_FieldPhases[i]]);
      values[m_FieldPhases[i]][bin].push_back(static_cast<float>( m_Neighborhoods[i] ));
    }
  }
  for (size_t i = 1; i < numensembles; i++)
  {
    if(m_PhaseTypes[i] == DREAM3D::PhaseType::PrimaryPhase)
    {
      PrimaryStatsData* pp = PrimaryStatsData::SafePointerDownCast(statsDataArray[i].get());
      m_DistributionAnalysis[m_NeighborhoodDistributionFitType]->calculateCorrelatedParameters(values[i], neighborhoods[i]);
      pp->setGrainSize_Neighbors(neighborhoods[i]);
    }
    if(m_PhaseTypes[i] == DREAM3D::PhaseType::PrecipitatePhase)
    {
      PrecipitateStatsData* pp = PrecipitateStatsData::SafePointerDownCast(statsDataArray[i].get());
      m_DistributionAnalysis[m_NeighborhoodDistributionFitType]->calculateCorrelatedParameters(values[i], neighborhoods[i]);
      pp->setGrainSize_Neighbors(neighborhoods[i]);
    }
    if(m_PhaseTypes[i] == DREAM3D::PhaseType::TransformationPhase)
    {
      TransformationStatsData* tp = TransformationStatsData::SafePointerDownCast(statsDataArray[i].get());
      m_DistributionAnalysis[m_NeighborhoodDistributionFitType]->calculateCorrelatedParameters(values[i], neighborhoods[i]);
      tp->setGrainSize_Neighbors(neighborhoods[i]);
    }
  }

}
void GenerateEnsembleStatistics::gatherODFStats()
{
  VoxelDataContainer* m = getVoxelDataContainer();
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The DataContainer Object was NULL", -999);
    return;
  }
  setErrorCondition(0);

  StatsDataArray& statsDataArray = *m_StatsDataArray;

  size_t bin;
  size_t numgrains = m->getNumFieldTuples();
  int phase;
  std::vector<float> totalvol;
  std::vector<FloatArrayType::Pointer> eulerodf;

  size_t numensembles = m->getNumEnsembleTuples();

  totalvol.resize(numensembles);
  eulerodf.resize(numensembles);
  unsigned long long dims = 0;
  for(unsigned long long i=1;i<numensembles;i++)
  {
    totalvol[i] = 0;
    if (m_CrystalStructures[i] == Ebsd::CrystalStructure::Hexagonal)
    {
      dims = 36 * 36 * 12;
      eulerodf[i] = FloatArrayType::CreateArray(dims, DREAM3D::HDF5::ODF);
      for (unsigned long long j = 0; j < dims; j++)
      {
        eulerodf[i]->SetValue(j, 0.0);
      }
    }
    else if (m_CrystalStructures[i] == Ebsd::CrystalStructure::Cubic)
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
  for (size_t i = 1; i < numgrains; i++)
  {
    if (m_SurfaceFields[i] == false)
    {
      totalvol[m_FieldPhases[i]] = totalvol[m_FieldPhases[i]] + m_Volumes[i];
    }
  }
  for (size_t i = 1; i < numgrains; i++)
  {
    if (m_SurfaceFields[i] == false)
    {
      ea1 = m_FieldEulerAngles[3*i];
      ea2 = m_FieldEulerAngles[3*i+1];
      ea3 = m_FieldEulerAngles[3*i+2];
      phase = m_CrystalStructures[m_FieldPhases[i]];
      OrientationMath::eulertoRod(r1, r2, r3, ea1, ea2, ea3);
      bin = m_OrientationOps[phase]->getOdfBin(r1, r2, r3);
      eulerodf[m_FieldPhases[i]]->SetValue(bin, (eulerodf[m_FieldPhases[i]]->GetValue(bin) + (m_Volumes[i] / totalvol[m_FieldPhases[i]])));
    }
  }

  for(size_t i = 1;i < numensembles;i++)
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
  VoxelDataContainer* m = getVoxelDataContainer();
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The DataContainer Object was NULL", -999);
    return;
  }
  setErrorCondition(0);

  StatsDataArray& statsDataArray = *m_StatsDataArray;

  // But since a pointer is difficult to use operators with we will now create a
  // reference variable to the pointer with the correct variable name that allows
  // us to use the same syntax as the "vector of vectors"
  NeighborList<int>& neighborlist = *m_NeighborList;
  // And we do the same for the SharedSurfaceArea list
  NeighborList<float>& neighborsurfacearealist = *m_SharedSurfaceAreaList;

  float n1 = 0.0f, n2 = 0.0f, n3= 0.0f;
  float r1= 0.0f, r2 = 0.0f, r3 = 0.0f;
  int mbin = 0;
  float w;
  float q1[5];
  float q2[5];
  size_t numgrains = m->getNumFieldTuples();
  unsigned int phase1, phase2;
  std::vector<float> totalSurfaceArea;
  std::vector<FloatArrayType::Pointer> misobin;
  int numbins = 0;

  size_t numensembles = m->getNumEnsembleTuples();

  misobin.resize(numensembles);
  totalSurfaceArea.resize(numensembles);
  for(size_t i=1;i<numensembles;++i)
  {
    totalSurfaceArea[i] = 0;
    if (m_CrystalStructures[i] == Ebsd::CrystalStructure::Hexagonal)
    {
      numbins = 36 * 36 * 12;
      misobin[i] = FloatArrayType::CreateArray(numbins, DREAM3D::HDF5::MisorientationBins);
    }
    else if (m_CrystalStructures[i] == Ebsd::CrystalStructure::Cubic)
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
  for (size_t i = 1; i < numgrains; i++)
  {
    q1[0] = m_AvgQuats[5*i];
    q1[1] = m_AvgQuats[5*i+1];
    q1[2] = m_AvgQuats[5*i+2];
    q1[3] = m_AvgQuats[5*i+3];
    q1[4] = m_AvgQuats[5*i+4];
    phase1 = m_CrystalStructures[m_FieldPhases[i]];
    for (size_t j = 0; j < neighborlist[i].size(); j++)
    {
      w = 10000.0;
      nname = neighborlist[i][j];
      q2[0] = m_AvgQuats[5*nname];
      q2[1] = m_AvgQuats[5*nname+1];
      q2[2] = m_AvgQuats[5*nname+2];
      q2[3] = m_AvgQuats[5*nname+3];
      q2[4] = m_AvgQuats[5*nname+4];
      phase2 = m_CrystalStructures[m_FieldPhases[nname]];
      if (phase1 == phase2) w = m_OrientationOps[phase1]->getMisoQuat( q1, q2, n1, n2, n3);
      if (phase1 == phase2)
      {
        OrientationMath::axisAngletoRod(w, n1, n2, n3, r1, r2, r3);
        if ((nname > i || m_SurfaceFields[nname] == true))
        {
          mbin = m_OrientationOps[phase1]->getMisoBin(r1, r2, r3);
          nsa = neighborsurfacearealist[i][j];
          misobin[m_FieldPhases[i]]->SetValue(mbin, (misobin[m_FieldPhases[i]]->GetValue(mbin) + nsa));
          totalSurfaceArea[m_FieldPhases[i]] = totalSurfaceArea[m_FieldPhases[i]] + nsa;
        }
      }
    }
  }

  // unsigned long long int dims = static_cast<unsigned long long int>(numbins);
  for (size_t i = 1; i < numensembles; i++)
  {
    for(size_t j=0;j<misobin[i]->GetSize();j++)
    {
      misobin[i]->SetValue(j, (misobin[i]->GetValue(j)/totalSurfaceArea[i]));
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
void GenerateEnsembleStatistics::gatherAxisODFStats()
{
  VoxelDataContainer* m = getVoxelDataContainer();
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The DataContainer Object was NULL", -999);
    return;
  }
  setErrorCondition(0);

  StatsDataArray& statsDataArray = *m_StatsDataArray;

  float r1, r2, r3;
  int bin;
  std::vector<FloatArrayType::Pointer> axisodf;
  std::vector<float> totalaxes;
  size_t numXTals = m->getNumEnsembleTuples();
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
  size_t numgrains = m->getNumFieldTuples();
  for (size_t i = 1; i < numgrains; i++)
  {
    if(m_SurfaceFields[i] == false)
    {
      totalaxes[m_FieldPhases[i]]++;
    }
  }
  for (size_t i = 1; i < numgrains; i++)
  {
    float ea1 = m_AxisEulerAngles[3 * i];
    float ea2 = m_AxisEulerAngles[3 * i + 1];
    float ea3 = m_AxisEulerAngles[3 * i + 2];
    if(m_SurfaceFields[i] == 0)
    {
      OrientationMath::eulertoRod(r1, r2, r3, ea1, ea2, ea3);
      m_OrientationOps[Ebsd::CrystalStructure::OrthoRhombic]->getODFFZRod(r1, r2, r3);
      bin = m_OrientationOps[Ebsd::CrystalStructure::OrthoRhombic]->getOdfBin(r1, r2, r3);
      axisodf[m_FieldPhases[i]]->SetValue(bin, (axisodf[m_FieldPhases[i]]->GetValue(bin) + static_cast<float>((1.0 / totalaxes[m_FieldPhases[i]]))));
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
