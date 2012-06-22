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

#include "FindSizes.h"

#include "DREAM3DLib/Common/DREAM3DMath.h"
#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/DistributionAnalysisOps/BetaOps.h"
#include "DREAM3DLib/DistributionAnalysisOps/PowerLawOps.h"
#include "DREAM3DLib/DistributionAnalysisOps/LogNormalOps.h"
#include "DREAM3DLib/GenericFilters/FindBoundingBoxGrains.h"
#include "DREAM3DLib/GenericFilters/FindGrainPhases.h"

const static float m_pi = M_PI;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindSizes::FindSizes() :
AbstractFilter(),
m_GrainIdsArrayName(DREAM3D::CellData::GrainIds),
m_BiasedFieldsArrayName(DREAM3D::FieldData::BiasedFields),
m_FieldPhasesArrayName(DREAM3D::FieldData::Phases),
m_EquivalentDiametersArrayName(DREAM3D::FieldData::EquivalentDiameters),
m_NumCellsArrayName(DREAM3D::FieldData::NumCells),
m_PhaseTypesArrayName(DREAM3D::EnsembleData::PhaseTypes),
m_VolumesArrayName(DREAM3D::FieldData::Volumes),
m_GrainIds(NULL),
m_FieldPhases(NULL),
m_Volumes(NULL),
m_EquivalentDiameters(NULL),
m_PhaseTypes(NULL),
m_NumCells(NULL)
{
  m_DistributionAnalysis.push_back(BetaOps::New());
  m_DistributionAnalysis.push_back(LogNormalOps::New());
  m_DistributionAnalysis.push_back(PowerLawOps::New());
  setupFilterOptions();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindSizes::~FindSizes()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSizes::setupFilterOptions()
{
  std::vector<FilterOption::Pointer> options;
  {
    ChoiceFilterOption::Pointer option = ChoiceFilterOption::New();
    option->setHumanLabel("Distribution Type");
    option->setPropertyName("DistributionType");
    option->setWidgetType(FilterOption::ChoiceWidget);
	option->setValueType("unsigned int");
    std::vector<std::string> choices;
    choices.push_back("Beta");
    choices.push_back("LogNormal");
    choices.push_back("Power");
    option->setChoices(choices);
    options.push_back(option);
  }
  setFilterOptions(options);
}
// -----------------------------------------------------------------------------
void FindSizes::writeFilterOptions(AbstractFilterOptionsWriter* writer)
{
  writer->writeValue("DistributionType", getDistributionType() );
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSizes::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;
  DataContainer* m = getDataContainer();
  GET_PREREQ_DATA(m, DREAM3D, CellData, GrainIds, ss, -300, int32_t, Int32ArrayType, voxels, 1);

  GET_PREREQ_DATA(m, DREAM3D, FieldData, BiasedFields, ss, -301, bool, BoolArrayType, fields, 1);
  if(getErrorCondition() == -301)
  {
	setErrorCondition(0);
	FindBoundingBoxGrains::Pointer find_boundingboxfields = FindBoundingBoxGrains::New();
	find_boundingboxfields->setObservers(this->getObservers());
	find_boundingboxfields->setDataContainer(getDataContainer());
	if(preflight == true) find_boundingboxfields->preflight();
	if(preflight == false) find_boundingboxfields->execute();
	GET_PREREQ_DATA(m, DREAM3D, FieldData, BiasedFields, ss, -301, bool, BoolArrayType, fields, 1);
  }
  GET_PREREQ_DATA(m, DREAM3D, FieldData, FieldPhases, ss, -302, int32_t, Int32ArrayType, fields, 1);
  if(getErrorCondition() == -302)
  {
	setErrorCondition(0);
	FindGrainPhases::Pointer find_grainphases = FindGrainPhases::New();
	find_grainphases->setObservers(this->getObservers());
	find_grainphases->setDataContainer(getDataContainer());
	if(preflight == true) find_grainphases->preflight();
	if(preflight == false) find_grainphases->execute();
	GET_PREREQ_DATA(m, DREAM3D, FieldData, FieldPhases, ss, -302, int32_t, Int32ArrayType, fields, 1);
  }
  CREATE_NON_PREREQ_DATA(m, DREAM3D, FieldData, Volumes, ss, float, FloatArrayType, 0, fields, 1);
  CREATE_NON_PREREQ_DATA(m, DREAM3D, FieldData, EquivalentDiameters, ss, float,FloatArrayType, 0, fields, 1);
  CREATE_NON_PREREQ_DATA(m, DREAM3D, FieldData, NumCells, ss, int32_t, Int32ArrayType, 0, fields, 1);

  typedef DataArray<unsigned int> PhaseTypeArrayType;
  GET_PREREQ_DATA(m, DREAM3D, EnsembleData, PhaseTypes, ss, -307, unsigned int, PhaseTypeArrayType, ensembles, 1);
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
void FindSizes::preflight()
{
  dataCheck(true, 1, 1, 1);
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSizes::execute()
{
  DataContainer* m = getDataContainer();
  if(NULL == m)
  {
    setErrorCondition(-1);
    std::stringstream ss;
    ss << " DataContainer was NULL";
    addErrorMessage(getNameOfClass(), ss.str(), -1);
    return;
  }
  setErrorCondition(0);

  dataCheck(false, m->getTotalPoints(), m->getNumFieldTuples(), m->getNumEnsembleTuples());
  if (getErrorCondition() < 0)
  {
    return;
  }


  if(m->getZPoints() > 1) find_sizes();
  if(m->getZPoints() == 1) find_sizes2D();
 notifyProgress("FindSizes Completed", 0, Observable::UpdateProgressMessage);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSizes::find_sizes()
{
  DataContainer* m = getDataContainer();
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
	  sizedist[i] = statsDataArray[i]->CreateCorrelatedDistributionArrays(getDistributionType(), 1);
	  values[i].resize(1);
  }

  DataArray<float>::Pointer m_GrainCounts = DataArray<float>::CreateArray(numgrains, "GrainCounts");
  float* graincounts = m_GrainCounts->GetPointer(0);


  // Initialize every element to 0.0
  for (size_t i = 0; i < numgrains * 1; i++)
  {
    graincounts[i] = 0.0f;
  }
  for (int j = 0; j < totalPoints; j++)
  {
    int gnum = m_GrainIds[j];
    graincounts[gnum]++;
  }
  float res_scalar = m->getXRes() * m->getYRes() * m->getZRes();
  float vol_term = (4.0/3.0)*m_pi;
  for (size_t i = 1; i < numgrains; i++)
  {
    m_NumCells[i] = graincounts[i];
    m_Volumes[i] = (graincounts[i] * res_scalar);
    radcubed = m_Volumes[i] / vol_term;
    diameter = 2.0f * powf(radcubed, 0.3333333333f);
    m_EquivalentDiameters[i] = diameter;
    if(m_BiasedFields[i] == false)
    {
      values[m_FieldPhases[i]][0].push_back(m_EquivalentDiameters[i]);
      fractions[m_FieldPhases[i]] = fractions[m_FieldPhases[i]] + m_Volumes[i];
      totalUnbiasedVolume = totalUnbiasedVolume + m_Volumes[i];
    }
  }
  for (size_t i = 1; i < numensembles; i++)
  {
	  if(m_PhaseTypes[i] == DREAM3D::PhaseType::PrimaryPhase)
	  {
		  PrimaryStatsData* pp = PrimaryStatsData::SafePointerDownCast(statsDataArray[i].get());
		  pp->setPhaseFraction((fractions[i]/totalUnbiasedVolume));
		  m_DistributionAnalysis[getDistributionType()]->calculateCorrelatedParameters(values[i], sizedist[i]);
		  pp->setGrainSizeDistribution(sizedist[i]);
		  DistributionAnalysisOps::determinemaxandminvalues(values[i][0], maxdiam, mindiam);
		  float stepsize = (1.01*(maxdiam-mindiam))/10.0;
		  pp->setGrainDiameterInfo(stepsize, maxdiam, mindiam);
		  binnumbers = FloatArrayType::CreateArray(10, DREAM3D::HDF5::BinNumber);
		  DistributionAnalysisOps::determinebinnumbers(maxdiam, mindiam, stepsize, binnumbers);
		  pp->setBinNumbers(binnumbers);
	  }
	  if(m_PhaseTypes[i] == DREAM3D::PhaseType::PrecipitatePhase)
	  {
		  PrecipitateStatsData* pp = PrecipitateStatsData::SafePointerDownCast(statsDataArray[i].get());
		  pp->setPhaseFraction((fractions[i]/totalUnbiasedVolume));
		  m_DistributionAnalysis[getDistributionType()]->calculateCorrelatedParameters(values[i], sizedist[i]);
		  pp->setGrainSizeDistribution(sizedist[i]);
		  DistributionAnalysisOps::determinemaxandminvalues(values[i][0], maxdiam, mindiam);
		  float stepsize = (1.01*(maxdiam-mindiam))/10.0;
		  pp->setGrainDiameterInfo(stepsize, maxdiam, mindiam);
		  binnumbers = FloatArrayType::CreateArray(10, DREAM3D::HDF5::BinNumber);
		  DistributionAnalysisOps::determinebinnumbers(maxdiam, mindiam, stepsize, binnumbers);
		  pp->setBinNumbers(binnumbers);
	  }
	  if(m_PhaseTypes[i] == DREAM3D::PhaseType::TransformationPhase)
	  {
		  TransformationStatsData* tp = TransformationStatsData::SafePointerDownCast(statsDataArray[i].get());
		  tp->setPhaseFraction((fractions[i]/totalUnbiasedVolume));
		  m_DistributionAnalysis[getDistributionType()]->calculateCorrelatedParameters(values[i], sizedist[i]);
		  tp->setGrainSizeDistribution(sizedist[i]);
		  DistributionAnalysisOps::determinemaxandminvalues(values[i][0], maxdiam, mindiam);
		  float stepsize = (1.01*(maxdiam-mindiam))/10.0;
		  tp->setGrainDiameterInfo(stepsize, maxdiam, mindiam);
		  binnumbers = FloatArrayType::CreateArray(10, DREAM3D::HDF5::BinNumber);
		  DistributionAnalysisOps::determinebinnumbers(maxdiam, mindiam, stepsize, binnumbers);
		  tp->setBinNumbers(binnumbers);
	  }
  }
}
void FindSizes::find_sizes2D()
{
  DataContainer* m = getDataContainer();
  int64_t totalPoints = m->getTotalPoints();
  StatsData::Pointer stats_data = StatsData::New();

  StatsDataArray& statsDataArray = *m_StatsDataArray;

  float radsquared;
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
	  sizedist[i] = stats_data->CreateCorrelatedDistributionArrays(getDistributionType(), 1);
	  values[i].resize(1);
  }

  DataArray<float>::Pointer m_GrainCounts = DataArray<float>::CreateArray(numgrains, "GrainCounts");
  float* graincounts = m_GrainCounts->GetPointer(0);


  for (size_t i = 0; i < numgrains; i++)
  {
      graincounts[i] = 0.0f;
  }
  for (int j = 0; j < totalPoints; j++)
  {
    int gnum = m_GrainIds[j];
    graincounts[gnum]++;
  }
  for (size_t i = 1; i < numgrains; i++)
  {
    m_NumCells[i] = graincounts[i];
    m_Volumes[i] = (graincounts[i] * m->getXRes() * m->getYRes());
    radsquared = m_Volumes[i] / m_pi;
    diameter = (2 * sqrt(radsquared));
    m_EquivalentDiameters[i] = diameter;
    if(m_BiasedFields[i] == false)
    {
      values[m_FieldPhases[i]][0].push_back(m_EquivalentDiameters[i]);
      fractions[m_FieldPhases[i]] = fractions[m_FieldPhases[i]] + m_Volumes[i];
      totalUnbiasedVolume = totalUnbiasedVolume + m_Volumes[i];
    }
  }
  for (size_t i = 1; i < numensembles; i++)
  {
	  if(m_PhaseTypes[i] == DREAM3D::PhaseType::PrimaryPhase)
	  {
		  PrimaryStatsData* pp = PrimaryStatsData::SafePointerDownCast(statsDataArray[i].get());
		  pp->setPhaseFraction((fractions[i]/totalUnbiasedVolume));
		  m_DistributionAnalysis[getDistributionType()]->calculateCorrelatedParameters(values[i], sizedist[i]);
		  pp->setGrainSizeDistribution(sizedist[i]);
		  DistributionAnalysisOps::determinemaxandminvalues(values[i][0], maxdiam, mindiam);
		  float stepsize = (1.01*(maxdiam-mindiam))/10.0;
		  pp->setGrainDiameterInfo(stepsize, maxdiam, mindiam);
		  binnumbers = FloatArrayType::CreateArray(10, DREAM3D::HDF5::BinNumber);
		  DistributionAnalysisOps::determinebinnumbers(maxdiam, mindiam, stepsize, binnumbers);
		  pp->setBinNumbers(binnumbers);
	  }
	  if(m_PhaseTypes[i] == DREAM3D::PhaseType::PrecipitatePhase)
	  {
		  PrecipitateStatsData* pp = PrecipitateStatsData::SafePointerDownCast(statsDataArray[i].get());
		  pp->setPhaseFraction((fractions[i]/totalUnbiasedVolume));
		  m_DistributionAnalysis[getDistributionType()]->calculateCorrelatedParameters(values[i], sizedist[i]);
		  pp->setGrainSizeDistribution(sizedist[i]);
		  DistributionAnalysisOps::determinemaxandminvalues(values[i][0], maxdiam, mindiam);
		  float stepsize = (1.01*(maxdiam-mindiam))/10.0;
		  pp->setGrainDiameterInfo(stepsize, maxdiam, mindiam);
		  binnumbers = FloatArrayType::CreateArray(10, DREAM3D::HDF5::BinNumber);
		  DistributionAnalysisOps::determinebinnumbers(maxdiam, mindiam, stepsize, binnumbers);
		  pp->setBinNumbers(binnumbers);
	  }
	  if(m_PhaseTypes[i] == DREAM3D::PhaseType::TransformationPhase)
	  {
		  TransformationStatsData* tp = TransformationStatsData::SafePointerDownCast(statsDataArray[i].get());
		  tp->setPhaseFraction((fractions[i]/totalUnbiasedVolume));
		  m_DistributionAnalysis[getDistributionType()]->calculateCorrelatedParameters(values[i], sizedist[i]);
		  tp->setGrainSizeDistribution(sizedist[i]);
		  DistributionAnalysisOps::determinemaxandminvalues(values[i][0], maxdiam, mindiam);
		  float stepsize = (1.01*(maxdiam-mindiam))/10.0;
		  tp->setGrainDiameterInfo(stepsize, maxdiam, mindiam);
		  binnumbers = FloatArrayType::CreateArray(10, DREAM3D::HDF5::BinNumber);
		  DistributionAnalysisOps::determinebinnumbers(maxdiam, mindiam, stepsize, binnumbers);
		  tp->setBinNumbers(binnumbers);
	  }
  }
}

