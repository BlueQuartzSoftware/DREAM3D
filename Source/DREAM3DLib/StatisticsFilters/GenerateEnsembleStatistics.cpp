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

#include "DREAM3DLib/GenericFilters/FindNeighbors.h"
#include "DREAM3DLib/GenericFilters/FindSurfaceGrains.h"
#include "DREAM3DLib/GenericFilters/FindGrainPhases.h"

const static float m_pi = static_cast<float>(M_PI);

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GenerateEnsembleStatistics::GenerateEnsembleStatistics()  :
AbstractFilter(),
m_AvgQuatsArrayName(DREAM3D::FieldData::AvgQuats),
m_FieldPhasesArrayName(DREAM3D::FieldData::Phases),
m_BiasedFieldsArrayName(DREAM3D::FieldData::BiasedFields),
m_AxisEulerAnglesArrayName(DREAM3D::FieldData::AxisEulerAngles),
m_EquivalentDiametersArrayName(DREAM3D::FieldData::EquivalentDiameters),
m_AspectRatiosArrayName(DREAM3D::FieldData::AspectRatios),
m_Omega3sArrayName(DREAM3D::FieldData::Omega3s),
m_NeighborhoodsArrayName(DREAM3D::FieldData::Neighborhoods),
m_SurfaceFieldsArrayName(DREAM3D::FieldData::SurfaceFields),
m_CrystalStructuresArrayName(DREAM3D::EnsembleData::CrystalStructures),
m_TotalSurfaceAreasArrayName(DREAM3D::EnsembleData::TotalSurfaceAreas),
m_PhaseTypesArrayName(DREAM3D::EnsembleData::PhaseTypes),
m_SizeDistribution(false),
m_SizeDistributionFitType(DREAM3D::DistributionType::LogNormal),
m_AvgQuats(NULL),
m_SurfaceFields(NULL),
m_FieldPhases(NULL),
m_EquivalentDiameters(NULL),
m_AxisEulerAngles(NULL),
m_Omega3s(NULL),
m_AspectRatios(NULL),
m_Neighborhoods(NULL),
m_CrystalStructures(NULL),
m_PhaseTypes(NULL)
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
  int err = 0;
  //GET_PREREQ_DATA(m, DREAM3D, FieldData, AvgQuats, ss, -301, float, FloatArrayType, fields, 5)
  //TEST_PREREQ_DATA(m, DREAM3D, FieldData, SurfaceFields, err, -302, bool, BoolArrayType, fields, 1)
  //if(err == -302)
  //{
  //  setErrorCondition(0);
  //  FindSurfaceGrains::Pointer find_surfacefields = FindSurfaceGrains::New();
  //  find_surfacefields->setObservers(this->getObservers());
  //  find_surfacefields->setVoxelDataContainer(getVoxelDataContainer());
  //  if(preflight == true) find_surfacefields->preflight();
  //  if(preflight == false) find_surfacefields->execute();
  //}
  //GET_PREREQ_DATA(m, DREAM3D, FieldData, SurfaceFields, ss, -302, bool, BoolArrayType, fields, 1)


  TEST_PREREQ_DATA(m, DREAM3D, FieldData, FieldPhases, err, -303,  int32_t, Int32ArrayType, fields, 1)
  if(err == -303)
  {
    setErrorCondition(0);
    FindGrainPhases::Pointer find_grainphases = FindGrainPhases::New();
    find_grainphases->setObservers(this->getObservers());
    find_grainphases->setVoxelDataContainer(getVoxelDataContainer());
    if(preflight == true) find_grainphases->preflight();
    if(preflight == false) find_grainphases->execute();
  }
  GET_PREREQ_DATA(m, DREAM3D, FieldData, FieldPhases, ss, -303, int32_t, Int32ArrayType, fields, 1)
  
  //GET_PREREQ_DATA(m, DREAM3D, FieldData, Neighborhoods, ss, -304, int32_t, Int32ArrayType, fields, 1)
  //GET_PREREQ_DATA(m, DREAM3D, FieldData, AxisEulerAngles, ss, -305, float, FloatArrayType, fields, 3)
  //GET_PREREQ_DATA(m, DREAM3D, FieldData, Omega3s, ss, -306, float, FloatArrayType, fields, 1)
  //GET_PREREQ_DATA(m, DREAM3D, FieldData, AspectRatios, ss, -307, float, FloatArrayType, fields, 2)

  if(m_SizeDistribution == true)
  {
	GET_PREREQ_DATA(m, DREAM3D, FieldData, BiasedFields, ss, -302, bool, BoolArrayType, fields, 1)
	GET_PREREQ_DATA(m, DREAM3D, FieldData, EquivalentDiameters, ss, -302, float, FloatArrayType, fields, 1)
  }

  typedef DataArray<unsigned int> XTalStructArrayType;
  GET_PREREQ_DATA(m, DREAM3D, EnsembleData, CrystalStructures, ss, -305, unsigned int, XTalStructArrayType, ensembles, 1)
  typedef DataArray<unsigned int> PhaseTypeArrayType;
  GET_PREREQ_DATA(m, DREAM3D, EnsembleData, PhaseTypes, ss, -307, unsigned int, PhaseTypeArrayType, ensembles, 1)
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

  dataCheck(false, m->getTotalPoints(), m->getNumFieldTuples(), m->getNumEnsembleTuples());
  if (getErrorCondition() < 0)
  {
    return;
  }

  if(m_SizeDistribution == true)
  {
	  gatherSizeStats();
  }

 notifyStatusMessage("GenerateEnsembleStatistics Completed");
}

void GenerateEnsembleStatistics::gatherSizeStats()
{
  VoxelDataContainer* m = getVoxelDataContainer();
  int64_t totalPoints = m->getTotalPoints();

  StatsDataArray& statsDataArray = *m_StatsDataArray;

  float radcubed;
  float diameter;
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
		  float stepsize = (1.01f*(maxdiam-mindiam))/10.0f;
		  pp->setGrainDiameterInfo(stepsize, maxdiam, mindiam);
		  binnumbers = FloatArrayType::CreateArray(10, DREAM3D::HDF5::BinNumber);
		  DistributionAnalysisOps::determinebinnumbers(maxdiam, mindiam, stepsize, binnumbers);
		  pp->setBinNumbers(binnumbers);
	  }
	  if(m_PhaseTypes[i] == DREAM3D::PhaseType::PrecipitatePhase)
	  {
		  PrecipitateStatsData* pp = PrecipitateStatsData::SafePointerDownCast(statsDataArray[i].get());
		  pp->setPhaseFraction((fractions[i]/totalUnbiasedVolume));
		  m_DistributionAnalysis[m_SizeDistributionFitType]->calculateCorrelatedParameters(values[i], sizedist[i]);
		  pp->setGrainSizeDistribution(sizedist[i]);
		  DistributionAnalysisOps::determinemaxandminvalues(values[i][0], maxdiam, mindiam);
		  float stepsize = (1.01f*(maxdiam-mindiam))/10.0f;
		  pp->setGrainDiameterInfo(stepsize, maxdiam, mindiam);
		  binnumbers = FloatArrayType::CreateArray(10, DREAM3D::HDF5::BinNumber);
		  DistributionAnalysisOps::determinebinnumbers(maxdiam, mindiam, stepsize, binnumbers);
		  pp->setBinNumbers(binnumbers);
	  }
	  if(m_PhaseTypes[i] == DREAM3D::PhaseType::TransformationPhase)
	  {
		  TransformationStatsData* tp = TransformationStatsData::SafePointerDownCast(statsDataArray[i].get());
		  tp->setPhaseFraction((fractions[i]/totalUnbiasedVolume));
		  m_DistributionAnalysis[m_SizeDistributionFitType]->calculateCorrelatedParameters(values[i], sizedist[i]);
		  tp->setGrainSizeDistribution(sizedist[i]);
		  DistributionAnalysisOps::determinemaxandminvalues(values[i][0], maxdiam, mindiam);
		  float stepsize = (1.01f*(maxdiam-mindiam))/10.0f;
		  tp->setGrainDiameterInfo(stepsize, maxdiam, mindiam);
		  binnumbers = FloatArrayType::CreateArray(10, DREAM3D::HDF5::BinNumber);
		  DistributionAnalysisOps::determinebinnumbers(maxdiam, mindiam, stepsize, binnumbers);
		  tp->setBinNumbers(binnumbers);
	  }
  }
}