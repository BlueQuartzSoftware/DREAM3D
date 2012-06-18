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

#include "InsertPrecipitatePhases.h"

#include <map>


#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Common/DREAM3DMath.h"
#include "DREAM3DLib/Common/DREAM3DRandom.h"
#include "DREAM3DLib/Common/DataContainerMacros.h"

#include "DREAM3DLib/ShapeOps/CubeOctohedronOps.h"
#include "DREAM3DLib/ShapeOps/CylinderOps.h"
#include "DREAM3DLib/ShapeOps/EllipsoidOps.h"
#include "DREAM3DLib/ShapeOps/SuperEllipsoidOps.h"

#include "DREAM3DLib/GenericFilters/FindSurfaceCells.h"
#include "DREAM3DLib/GenericFilters/FindNeighbors.h"
#include "DREAM3DLib/GenericFilters/RenumberGrains.h"

#include "DREAM3DLib/Common/PrecipitateStatsData.h"

const static float m_pi = static_cast<float>(M_PI);

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
InsertPrecipitatePhases::InsertPrecipitatePhases() :
AbstractFilter(),
m_GrainIdsArrayName(DREAM3D::CellData::GrainIds),
m_CellPhasesArrayName(DREAM3D::CellData::Phases),
m_SurfaceVoxelsArrayName(DREAM3D::CellData::SurfaceVoxels),
m_ActiveArrayName(DREAM3D::FieldData::Active),
m_AxisEulerAnglesArrayName(DREAM3D::FieldData::AxisEulerAngles),
m_AxisLengthsArrayName(DREAM3D::FieldData::AxisLengths),
m_CentroidsArrayName(DREAM3D::FieldData::Centroids),
m_EquivalentDiametersArrayName(DREAM3D::FieldData::EquivalentDiameters),
m_NeighborhoodsArrayName(DREAM3D::FieldData::Neighborhoods),
m_NumCellsArrayName(DREAM3D::FieldData::NumCells),
m_Omega3sArrayName(DREAM3D::FieldData::Omega3s),
m_FieldPhasesArrayName(DREAM3D::FieldData::Phases),
m_VolumesArrayName(DREAM3D::FieldData::Volumes),
m_PhaseTypesArrayName(DREAM3D::EnsembleData::PhaseTypes),
m_ShapeTypesArrayName(DREAM3D::EnsembleData::ShapeTypes),
m_NumFieldsArrayName(DREAM3D::EnsembleData::NumFields),
m_PeriodicBoundaries(false),
m_GrainIds(NULL),
m_CellPhases(NULL),
m_SurfaceVoxels(NULL),
m_AxisEulerAngles(NULL),
m_Centroids(NULL),
m_AxisLengths(NULL),
m_Volumes(NULL),
m_Omega3s(NULL),
m_EquivalentDiameters(NULL),
m_Active(NULL),
m_FieldPhases(NULL),
m_NumCells(NULL),
m_PhaseTypes(NULL),
m_ShapeTypes(NULL),
m_NumFields(NULL)
{
  m_EllipsoidOps = EllipsoidOps::New();
  m_ShapeOps[DREAM3D::ShapeType::EllipsoidShape] = m_EllipsoidOps.get();
  m_SuperEllipsoidOps = SuperEllipsoidOps::New();
  m_ShapeOps[DREAM3D::ShapeType::SuperEllipsoidShape] = m_SuperEllipsoidOps.get();
  m_CubicOctohedronOps = CubeOctohedronOps::New();
  m_ShapeOps[DREAM3D::ShapeType::CubeOctahedronShape] = m_CubicOctohedronOps.get();
  m_CylinderOps = CylinderOps::New();
  m_ShapeOps[DREAM3D::ShapeType::CylinderShape] = m_CylinderOps.get();
  m_UnknownShapeOps = ShapeOps::New();
  m_ShapeOps[DREAM3D::ShapeType::UnknownShapeType] = m_UnknownShapeOps.get();

  m_HexOps = HexagonalOps::New();
  m_OrientationOps.push_back(m_HexOps.get());
  m_CubicOps = CubicOps::New();
  m_OrientationOps.push_back(m_CubicOps.get());
  m_OrthoOps = OrthoRhombicOps::New();
  m_OrientationOps.push_back(m_OrthoOps.get());

  setupFilterOptions();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
InsertPrecipitatePhases::~InsertPrecipitatePhases()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------

void InsertPrecipitatePhases::setupFilterOptions()
{
  std::vector<FilterOption::Pointer> options;
  {
    FilterOption::Pointer option = FilterOption::New();
    option->setHumanLabel("Periodic Boundary");
    option->setPropertyName("PeriodicBoundaries");
    option->setWidgetType(FilterOption::BooleanWidget);
    option->setValueType("bool");
    options.push_back(option);
  }

  setFilterOptions(options);
}
// -----------------------------------------------------------------------------
void InsertPrecipitatePhases::writeFilterOptions(AbstractFilterOptionsWriter* writer)
{
  writer->writeValue("PeriodicBoundaries", getPeriodicBoundaries() );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InsertPrecipitatePhases::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;
  DataContainer* m = getDataContainer();

  // Cell Data
  GET_PREREQ_DATA(m, DREAM3D, CellData, GrainIds, ss, -300, int32_t, Int32ArrayType, voxels, 1);
  GET_PREREQ_DATA(m, DREAM3D, CellData, SurfaceVoxels, ss, -301, int8_t, Int8ArrayType, voxels, 1);
  if(getErrorCondition() == -301)
  {
	setErrorCondition(0);
	FindSurfaceCells::Pointer find_surfacecells = FindSurfaceCells::New();
	find_surfacecells->setObservers(this->getObservers());
	find_surfacecells->setDataContainer(getDataContainer());
	if(preflight == true) find_surfacecells->preflight();
	if(preflight == false) find_surfacecells->execute();
    GET_PREREQ_DATA(m, DREAM3D, CellData, SurfaceVoxels, ss, -301, int8_t, Int8ArrayType, voxels, 1);
  }
  GET_PREREQ_DATA(m, DREAM3D, CellData, CellPhases, ss, -302, int32_t, Int32ArrayType, voxels, 1);

  // Field Data
  CREATE_NON_PREREQ_DATA(m, DREAM3D, FieldData, FieldPhases, ss, int32_t, Int32ArrayType, 0, fields, 1);
  CREATE_NON_PREREQ_DATA(m, DREAM3D, FieldData, EquivalentDiameters, ss, float, FloatArrayType, 0, fields, 1);
  CREATE_NON_PREREQ_DATA(m, DREAM3D, FieldData, Omega3s, ss, float, FloatArrayType, 0, fields, 1);
  CREATE_NON_PREREQ_DATA(m, DREAM3D, FieldData, AxisEulerAngles, ss, float, FloatArrayType, 0, fields, 3);
  CREATE_NON_PREREQ_DATA(m, DREAM3D, FieldData, AxisLengths, ss, float, FloatArrayType, 0, fields, 3);
  CREATE_NON_PREREQ_DATA(m, DREAM3D, FieldData, Volumes, ss, float, FloatArrayType, 0, fields, 1);
  CREATE_NON_PREREQ_DATA(m, DREAM3D, FieldData, Centroids, ss, float, FloatArrayType, 0, fields, 3);
  CREATE_NON_PREREQ_DATA(m, DREAM3D, FieldData, Active, ss, bool, BoolArrayType, false, fields, 1);
  CREATE_NON_PREREQ_DATA(m, DREAM3D, FieldData, NumCells, ss, int32_t, Int32ArrayType, 0, fields, 1);
  CREATE_NON_PREREQ_DATA(m, DREAM3D, FieldData, Neighborhoods, ss, int32_t, Int32ArrayType, 0, fields, 1);

  //Ensemble Data
  typedef DataArray<unsigned int> PhaseTypeArrayType;
  typedef DataArray<unsigned int> ShapeTypeArrayType;
  GET_PREREQ_DATA(m, DREAM3D, EnsembleData, PhaseTypes, ss, -301, unsigned int, PhaseTypeArrayType, ensembles, 1);
  GET_PREREQ_DATA(m, DREAM3D, EnsembleData, ShapeTypes, ss, -304, unsigned int, ShapeTypeArrayType, ensembles, 1);
  CREATE_NON_PREREQ_DATA(m, DREAM3D, EnsembleData, NumFields, ss, int32_t, Int32ArrayType, 0, ensembles, 1);
  m_StatsDataArray = StatsDataArray::SafeObjectDownCast<IDataArray*, StatsDataArray*>(m->getEnsembleData(DREAM3D::EnsembleData::Statistics).get());
  if(m_StatsDataArray == NULL)
  {
    ss << "Stats Array Not Initialized At Beginning of '" << getNameOfClass() << "' Filter" << std::endl;
    setErrorCondition(-308);
  }

  setErrorMessage(ss.str());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InsertPrecipitatePhases::preflight()
{
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InsertPrecipitatePhases::execute()
{
  int err = 0;
  setErrorCondition(err);
  DREAM3D_RANDOMNG_NEW()
  DataContainer* m = getDataContainer();

  if(NULL == m)
  {
    setErrorCondition(-1);
    std::stringstream ss;
    ss << getNameOfClass() << " DataContainer was NULL";
    setErrorMessage(ss.str());
    return;
  }

  int64_t totalPoints = m->getTotalPoints();
  size_t totalFields = m->getNumFieldTuples();

  dataCheck(false, totalPoints, totalFields, m->getNumEnsembleTuples());
  if (getErrorCondition() < 0)
  {
    return;
  }

  sizex = m->getXPoints() * m->getXRes();
  sizey = m->getYPoints() * m->getYRes();
  sizez = m->getZPoints() * m->getZRes();

  totalvol = sizex * sizey * sizez;

  notify("Packing Precipitates - Generating and Placing Precipitates", 0, Observable::UpdateProgressMessage);
  initialize_packinggrid();
  place_precipitates();

  notify("Packing Precipitates - Assigning Voxels", 0, Observable::UpdateProgressMessage);
  assign_voxels();

  notify("Packing Precipitates - Filling Gaps", 0, Observable::UpdateProgressMessage);
  assign_gaps();

  notify("Packing Precipitates - Cleaning Up Volume", 0, Observable::UpdateProgressMessage);
  cleanup_grains();

  notify("Packing Precipitates - Renumbering Grains", 0, Observable::UpdateProgressMessage);
  RenumberGrains::Pointer renumber_grains = RenumberGrains::New();
  renumber_grains->setObservers(this->getObservers());
  renumber_grains->setDataContainer(m);
  renumber_grains->execute();
  err = renumber_grains->getErrorCondition();
  if (err < 0)
  {
    setErrorCondition(renumber_grains->getErrorCondition());
    setErrorMessage(renumber_grains->getErrorMessage());
    return;
  }

  // If there is an error set this to something negative and also set a message
  notify("InsertPrecipitatePhases Completed", 0, Observable::UpdateProgressMessage);
}

void  InsertPrecipitatePhases::place_precipitates()
{
  notify("Placing Precipitates", 0, Observable::UpdateProgressMessage);
  DREAM3D_RANDOMNG_NEW()

  DataContainer* m = getDataContainer();

  StatsDataArray& statsDataArray = *m_StatsDataArray;

  size_t udims[3] =
  { 0, 0, 0 };
  m->getDimensions(udims);
#if (CMP_SIZEOF_SIZE_T == 4)
  typedef int32_t DimType;
#else
  typedef int64_t DimType;
#endif
  DimType dims[3] =
  { static_cast<DimType>(udims[0]),
      static_cast<DimType>(udims[1]),
      static_cast<DimType>(udims[2]), };

  sizex = dims[0] * m->getXRes();
  sizey = dims[1] * m->getYRes();
  sizez = dims[2] * m->getZRes();
  totalvol = sizex*sizey*sizez;

  int64_t totalPoints = m->getTotalPoints();
  size_t currentnumgrains = m->getNumFieldTuples();
  if(currentnumgrains == 0)
  {
	  m->resizeFieldDataArrays(1);
	  dataCheck(false, totalPoints, 1, m->getNumEnsembleTuples());
	  currentnumgrains = 1;
  }
  firstPrecipitateField = currentnumgrains;
  int phase;
  float precipboundaryfraction = 0.0;
  float random;
  int random2;
  float xc, yc, zc;
  float oldxc, oldyc, oldzc;
  int randomgrain;
  int acceptedmoves = 0;
  double totalprecipitatefractions = 0.0;

  size_t numensembles = m->getNumEnsembleTuples();

  for (size_t i = 1; i < numensembles; ++i)
  {
	PrecipitateStatsData* pp = PrecipitateStatsData::SafePointerDownCast(statsDataArray[i].get());
    if(m_PhaseTypes[i] == DREAM3D::PhaseType::PrecipitatePhase && pp != NULL)
    {
	  m_NumFields[i] = 0;
      precipitatephases.push_back(i);
      precipitatephasefractions.push_back(pp->getPhaseFraction());
      totalprecipitatefractions = totalprecipitatefractions + pp->getPhaseFraction();
    }
  }
  for (size_t i = 0; i < precipitatephases.size(); i++)
  {
    precipitatephasefractions[i] = precipitatephasefractions[i] / totalprecipitatefractions;
    if(i > 0) precipitatephasefractions[i] = precipitatephasefractions[i] + precipitatephasefractions[i - 1];
  }

  // initialize the sim and goal size distributions for the precipitate phases
  grainsizedist.resize(precipitatephases.size());
  simgrainsizedist.resize(precipitatephases.size());
  grainsizediststep.resize(precipitatephases.size());
  for (size_t i = 0; i < precipitatephases.size(); i++)
  {
    phase = precipitatephases[i];
	PrecipitateStatsData* pp = PrecipitateStatsData::SafePointerDownCast(statsDataArray[phase].get());
    grainsizedist[i].resize(40);
    simgrainsizedist[i].resize(40);
    grainsizediststep[i] = ((2 * pp->getMaxGrainDiameter()) - (pp->getMinGrainDiameter() / 2.0))
        / grainsizedist[i].size();
    float input = 0;
    float previoustotal = 0;
    VectorOfFloatArray GSdist = pp->getGrainSizeDistribution();
    float avg = GSdist[0]->GetValue(0);
    float stdev = GSdist[1]->GetValue(0);
    float denominatorConst = sqrtf(2.0f * stdev * stdev); // Calculate it here rather than calculating the same thing multiple times below
    for (size_t j = 0; j < grainsizedist[i].size(); j++)
    {
      input = (float(j + 1) * grainsizediststep[i]) + (pp->getMinGrainDiameter() / 2.0f);
      float logInput = logf(input);
      if(logInput <= avg)
      {
        grainsizedist[i][j] = 0.5f - 0.5f * (DREAM3DMath::erf((avg - logInput) / denominatorConst)) - previoustotal;
      }
      if(logInput > avg)
      {
        grainsizedist[i][j] = 0.5f + 0.5f * (DREAM3DMath::erf((logInput - avg) / denominatorConst)) - previoustotal;
      }
      previoustotal = previoustotal + grainsizedist[i][j];
    }
  }

  Precip precip;
  std::vector<float> curphasevol;
  curphasevol.resize(precipitatephases.size());
  float change = 0.0f;
  float factor = 1.0;
  float iter = 0;
  for (size_t j = 0; j < precipitatephases.size(); ++j)
  {
	  curphasevol[j] = 0;
	  float curphasetotalvol = totalvol*totalprecipitatefractions*precipitatephasefractions[j];
	  while (curphasevol[j] < (factor * curphasetotalvol))
	  {
	    iter++;
	    Seed++;
	    phase = precipitatephases[j];
		generate_precipitate(phase, static_cast<int>(Seed), &precip, m_ShapeTypes[phase], m_OrthoOps);
		currentsizedisterror = check_sizedisterror(&precip);
		change = (currentsizedisterror) - (oldsizedisterror);
		if(change > 0 || currentsizedisterror > (1.0 - (iter * 0.001)) || curphasevol[j] < (0.75* factor * curphasetotalvol))
		{
	      std::stringstream ss;
	      ss << "Packing Precipitates - Generating Grain #" << currentnumgrains;
	      notify(ss.str(), 0, Observable::UpdateProgressMessage);

	      m->resizeFieldDataArrays(currentnumgrains + 1);
	      dataCheck(false, totalPoints, currentnumgrains + 1, m->getNumEnsembleTuples());
	      m_Active[currentnumgrains] = true;
	      transfer_attributes(currentnumgrains, &precip);
	      oldsizedisterror = currentsizedisterror;
	      curphasevol[j] = curphasevol[j] + m_Volumes[currentnumgrains];
	      //FIXME: Initialize the Grain with some sort of default data
	      iter = 0;
		  currentnumgrains++;
		}
	  }
  }

  // initialize the sim and goal neighbor distribution for the primary phases
  neighbordist.resize(precipitatephases.size());
  simneighbordist.resize(precipitatephases.size());
  for (size_t i = 0; i < precipitatephases.size(); i++)
  {
    phase = precipitatephases[i];
	PrecipitateStatsData* pp = PrecipitateStatsData::SafePointerDownCast(statsDataArray[phase].get());
	neighbordist[i].resize(pp->getBinNumbers()->GetSize());
    simneighbordist[i].resize(pp->getBinNumbers()->GetSize());
	VectorOfFloatArray Neighdist = pp->getGrainSize_Neighbors();
    for (size_t j = 0; j < neighbordist[i].size(); j++)
    {
		neighbordist[i][j].resize(40);
		float input = 0;
		float previoustotal = 0;
		float avg = Neighdist[0]->GetValue(j);
		float stdev = Neighdist[1]->GetValue(j);
		float denominatorConst = sqrtf(2.0f * stdev * stdev); // Calculate it here rather than calculating the same thing multiple times below
		for (size_t k = 0; k < neighbordist[i][j].size(); k++)
		{
		  input = (float(k + 1) * grainsizediststep[i]) + (pp->getMinGrainDiameter() / 2.0f);
		  float logInput = logf(input);
		  if(logInput <= avg)
		  {
			neighbordist[i][j][k] = 0.5f - 0.5f * (DREAM3DMath::erf((avg - logInput) / denominatorConst )) - previoustotal;
		  }
		  if(logInput > avg)
		  {
			neighbordist[i][j][k] = 0.5f + 0.5f * (DREAM3DMath::erf((logInput - avg) / denominatorConst)) - previoustotal;
		  }
		  previoustotal = previoustotal + neighbordist[i][j][k];
		}
    }
  }

  //  for each grain : select centroid, determine voxels in grain, monitor filling error and decide of the 10 placements which
  // is the most beneficial, then the grain is added and its neighbors are determined

  size_t numgrains = m->getNumFieldTuples();

  columnlist.resize(numgrains);
  rowlist.resize(numgrains);
  planelist.resize(numgrains);
  packqualities.resize(numgrains);
  fillingerror = 1;
  for (size_t i = firstPrecipitateField; i < numgrains; i++)
  {
    std::stringstream ss;
    ss << "Packing Grains - Placing Grain #" << i;
    notify(ss.str(), 0, Observable::UpdateProgressMessage);

	PrecipitateStatsData* pp = PrecipitateStatsData::SafePointerDownCast(statsDataArray[m_FieldPhases[i]].get());
	precipboundaryfraction = pp->getPrecipBoundaryFraction();
    random = rg.genrand_res53();
    if(random <= precipboundaryfraction)
    {
      random2 = int(rg.genrand_res53() * double(totalPoints - 1));
      while (m_SurfaceVoxels[random2] == 0 || m_GrainIds[random2] >= firstPrecipitateField)
      {
        random2++;
        if(random2 >= totalPoints) random2 = random2 - totalPoints;
      }
    }
    else if(random > precipboundaryfraction)
    {
      random2 = rg.genrand_res53() * (totalPoints - 1);
      while (m_SurfaceVoxels[random2] != 0 || m_GrainIds[random2] >= firstPrecipitateField)
      {
        random2++;
        if(random2 >= totalPoints) random2 = random2 - totalPoints;
      }
    }
    xc = find_xcoord(random2);
    yc = find_ycoord(random2);
    zc = find_zcoord(random2);
    m_Centroids[3 * i] = xc;
    m_Centroids[3 * i + 1] = yc;
    m_Centroids[3 * i + 2] = zc;
    insert_precipitate(i);
    fillingerror = check_fillingerror(i, -1000);
    for (int iter = 0; iter < 10; iter++)
    {
      random = rg.genrand_res53();
      if(random <= precipboundaryfraction)
      {
        random2 = int(rg.genrand_res53() * double(totalPoints - 1));
        while (m_SurfaceVoxels[random2] == 0 || m_GrainIds[random2] >= firstPrecipitateField)
        {
          random2++;
          if(random2 >= totalPoints) random2 = random2 - totalPoints;
        }
      }
      else if(random > precipboundaryfraction)
      {
        random2 = rg.genrand_res53() * (totalPoints - 1);
        while (m_SurfaceVoxels[random2] != 0 || m_GrainIds[random2] >= firstPrecipitateField)
        {
          random2++;
          if(random2 >= totalPoints) random2 = random2 - totalPoints;
        }
      }
      xc = find_xcoord(random2);
      yc = find_ycoord(random2);
      zc = find_zcoord(random2);
	  oldxc = m_Centroids[3 * i];
      oldyc = m_Centroids[3 * i + 1];
      oldzc = m_Centroids[3 * i + 2];
      oldfillingerror = fillingerror;
      fillingerror = check_fillingerror(-1000, i);
      move_precipitate(i, xc, yc, zc);
      fillingerror = check_fillingerror(i, -1000);
      if(fillingerror > oldfillingerror)
      {
        fillingerror = check_fillingerror(-1000, i);
        move_precipitate(i, oldxc, oldyc, oldzc);
        fillingerror = check_fillingerror(i, -1000);
      }
    }
  }

  notify("Packing Grains - Initial Grain Placement Complete", 0, Observable::UpdateProgressMessage);

  // determine initial filling and neighbor distribution errors
  oldneighborhooderror = check_neighborhooderror(-1000, -1000);
  // begin swaping/moving/adding/removing grains to try to improve packing
  int totalAdjustments = static_cast<int>(10 * (numgrains-1));
  for (int iteration = 0; iteration < totalAdjustments; ++iteration)
  {
    std::stringstream ss;
    ss << "Packing Grains - Swapping/Moving/Adding/Removing Grains Iteration " << iteration << "/" << totalAdjustments;
    if(iteration%100 == 0) notify(ss.str(), 0, Observable::UpdateProgressMessage);

//    change1 = 0;
//    change2 = 0;
    int option = iteration % 2;

    // JUMP - this option moves one grain to a random spot in the volume
    if(option == 0)
    {
      randomgrain = firstPrecipitateField + int(rg.genrand_res53() * (numgrains-firstPrecipitateField));
      if(randomgrain < firstPrecipitateField) randomgrain = firstPrecipitateField;
      if(randomgrain >= numgrains) randomgrain = numgrains - 1;
      Seed++;

	  PrecipitateStatsData* pp = PrecipitateStatsData::SafePointerDownCast(statsDataArray[m_FieldPhases[randomgrain]].get());
      precipboundaryfraction = pp->getPrecipBoundaryFraction();
	  random = rg.genrand_res53();
      if(random <= precipboundaryfraction)
      {
        random2 = int(rg.genrand_res53() * double(totalPoints - 1));
        while (m_SurfaceVoxels[random2] == 0 || m_GrainIds[random2] >= firstPrecipitateField)
        {
          random2++;
          if(random2 >= totalPoints) random2 = random2 - totalPoints;
        }
      }
      else if(random > precipboundaryfraction)
      {
        random2 = rg.genrand_res53() * (totalPoints - 1);
        while (m_SurfaceVoxels[random2] != 0 || m_GrainIds[random2] >= firstPrecipitateField)
        {
          random2++;
          if(random2 >= totalPoints) random2 = random2 - totalPoints;
        }
      }
      xc = find_xcoord(random2);
      yc = find_ycoord(random2);
      zc = find_zcoord(random2);
      oldxc = m_Centroids[3 * randomgrain];
      oldyc = m_Centroids[3 * randomgrain + 1];
      oldzc = m_Centroids[3 * randomgrain + 2];
      oldfillingerror = fillingerror;
      fillingerror = check_fillingerror(-1000, static_cast<int>(randomgrain)  );
      move_precipitate(randomgrain, xc, yc, zc);
      fillingerror = check_fillingerror(static_cast<int>(randomgrain), -1000);
      currentneighborhooderror = check_neighborhooderror(-1000, randomgrain);
//      change2 = (currentneighborhooderror * currentneighborhooderror) - (oldneighborhooderror * oldneighborhooderror);
      if(fillingerror <= oldfillingerror)
      {
//        oldneighborhooderror = currentneighborhooderror;
        acceptedmoves++;
      }
      else if(fillingerror > oldfillingerror)
      {
        fillingerror = check_fillingerror(-1000, static_cast<int>(randomgrain));
        move_precipitate(randomgrain, oldxc, oldyc, oldzc);
        fillingerror = check_fillingerror(static_cast<int>(randomgrain), -1000);
      }
    }
    // NUDGE - this option moves one grain to a spot close to its current centroid
    if(option == 1)
    {
      randomgrain = firstPrecipitateField + int(rg.genrand_res53() * (numgrains-firstPrecipitateField));
      if(randomgrain < firstPrecipitateField) randomgrain = firstPrecipitateField;
      if(randomgrain >= numgrains) randomgrain = numgrains - 1;
      Seed++;
      oldxc = m_Centroids[3 * randomgrain];
      oldyc = m_Centroids[3 * randomgrain + 1];
      oldzc = m_Centroids[3 * randomgrain + 2];
      xc = oldxc + ((2.0f * (rg.genrand_res53() - 0.5f)) * (2.0f * packingresx));
      yc = oldyc + ((2.0f * (rg.genrand_res53() - 0.5f)) * (2.0f * packingresy));
      zc = oldzc + ((2.0f * (rg.genrand_res53() - 0.5f)) * (2.0f * packingresz));
      oldfillingerror = fillingerror;
      fillingerror = check_fillingerror(-1000, static_cast<int>(randomgrain));
      move_precipitate(randomgrain, xc, yc, zc);
      fillingerror = check_fillingerror(static_cast<int>(randomgrain), -1000);
      currentneighborhooderror = check_neighborhooderror(-1000, randomgrain);
//      change2 = (currentneighborhooderror * currentneighborhooderror) - (oldneighborhooderror * oldneighborhooderror);
      if(fillingerror <= oldfillingerror)
      {
//        oldneighborhooderror = currentneighborhooderror;
        acceptedmoves++;
      }
      else if(fillingerror > oldfillingerror)
      {
        fillingerror = check_fillingerror(-1000, static_cast<int>(randomgrain));
        move_precipitate(randomgrain, oldxc, oldyc, oldzc);
        fillingerror = check_fillingerror(static_cast<int>(randomgrain), -1000);
      }
    }
  }

}

void InsertPrecipitatePhases::generate_precipitate(int phase, int Seed, Precip* precip, unsigned int shapeclass, OrientationMath::Pointer OrthoOps)
{
  DREAM3D_RANDOMNG_NEW_SEEDED(Seed)

  DataContainer* m = getDataContainer();

  StatsDataArray& statsDataArray = *m_StatsDataArray;

  float r1 = 1;
  float a2 = 0, a3 = 0;
  float b2 = 0, b3 = 0;
  float diam = 0;
  float vol = 0;
  int volgood = 0;
  float phi1, PHI, phi2;
  float fourThirdsPi =  static_cast<float>((4.0f / 3.0f) * (m_pi));
  PrecipitateStatsData* pp = PrecipitateStatsData::SafePointerDownCast(statsDataArray[phase].get());
  VectorOfFloatArray GSdist = pp->getGrainSizeDistribution();
  float avg = GSdist[0]->GetValue(0);
  float stdev = GSdist[1]->GetValue(0);
  while (volgood == 0)
  {
    volgood = 1;
    diam = static_cast<float>(rg.genrand_norm(avg, stdev));
    diam = exp(diam);
    if(diam >= pp->getMaxGrainDiameter()) volgood = 0;
    if(diam < pp->getMinGrainDiameter()) volgood = 0;
    vol = fourThirdsPi * ((diam / 2.0f) * (diam / 2.0f) * (diam / 2.0f));
  }
   int diameter = int((diam - pp->getMinGrainDiameter()) / pp->getBinStepSize());
  float r2 = 0, r3 = 1;
  VectorOfFloatArray bovera = pp->getGrainSize_BOverA();
  VectorOfFloatArray covera = pp->getGrainSize_COverA();
  while (r2 < r3)
  {
	  r2 = 0, r3 = 0;
	  a2 = bovera[0]->GetValue(diameter);
	  b2 = bovera[1]->GetValue(diameter);
	  if(a2 == 0)
	  {
		  a2 = bovera[0]->GetValue(diameter-1);
		  b2 = bovera[1]->GetValue(diameter-1);
	  }
	  r2 = static_cast<float>(rg.genrand_beta(a2, b2));
	  a3 = covera[0]->GetValue(diameter);
	  b3 = covera[1]->GetValue(diameter);
	  if(a3 == 0)
	  {
		  a3 = covera[0]->GetValue(diameter-1);
		  b3 = covera[1]->GetValue(diameter-1);
	  }
	  r3 = rg.genrand_beta(a3, b3);
  }
  float random = rg.genrand_res53();
  float totaldensity = 0;
  int bin = 0;
  FloatArrayType::Pointer axisodf = pp->getAxisOrientation();
  while (random > totaldensity && bin < axisodf->GetSize())
  {
    totaldensity = totaldensity + axisodf->GetValue(bin);
    bin++;
  }
  OrthoOps->determineEulerAngles(bin, phi1, PHI, phi2);
  VectorOfFloatArray omega3 = pp->getGrainSize_Omegas();
  float mf = omega3[0]->GetValue(diameter);
  float s = omega3[1]->GetValue(diameter);
  float omega3f = static_cast<float>(rg.genrand_beta(mf, s));
  if(shapeclass == DREAM3D::ShapeType::EllipsoidShape) omega3f = 1;

  precip->m_Volumes = vol;
  precip->m_EquivalentDiameters = diam;
  precip->m_AxisLengths[0] = r1;
  precip->m_AxisLengths[1] = r2;
  precip->m_AxisLengths[2] = r3;
  precip->m_AxisEulerAngles[0] = phi1;
  precip->m_AxisEulerAngles[1] = PHI;
  precip->m_AxisEulerAngles[2] = phi2;
  precip->m_Omega3s = omega3f;
  precip->m_FieldPhases = phase;
  precip->m_Neighborhoods = 0;
}

void InsertPrecipitatePhases::transfer_attributes(int gnum, Precip* precip)
{
  m_Volumes[gnum] = precip->m_Volumes;
  m_EquivalentDiameters[gnum] = precip->m_EquivalentDiameters;
  m_AxisLengths[3*gnum+0] = precip->m_AxisLengths[0];
  m_AxisLengths[3*gnum+1] = precip->m_AxisLengths[1];
  m_AxisLengths[3*gnum+2] = precip->m_AxisLengths[2];
  m_AxisEulerAngles[3*gnum+0] = precip->m_AxisEulerAngles[0];
  m_AxisEulerAngles[3*gnum+1] = precip->m_AxisEulerAngles[1];
  m_AxisEulerAngles[3*gnum+2] = precip->m_AxisEulerAngles[2];
  m_Omega3s[gnum] = precip->m_Omega3s;
  m_FieldPhases[gnum] = precip->m_FieldPhases;
  m_Neighborhoods[gnum] = precip->m_Neighborhoods;
}

void InsertPrecipitatePhases::move_precipitate(size_t gnum, float xc, float yc, float zc)
{
  int occolumn, ocrow, ocplane;
  int nccolumn, ncrow, ncplane;
  int shiftcolumn, shiftrow, shiftplane;
  float oxc = m_Centroids[3*gnum];
  float oyc = m_Centroids[3*gnum+1];
  float ozc = m_Centroids[3*gnum+2];
  occolumn = (oxc - (packingresx / 2.0f)) / packingresx;
  ocrow = (oyc - (packingresy / 2.0f)) / packingresy;
  ocplane = (ozc - (packingresz / 2.0f)) / packingresz;
  nccolumn = (xc - (packingresx / 2.0f)) / packingresx;
  ncrow = (yc - (packingresy / 2.0f)) / packingresy;
  ncplane = (zc - (packingresz / 2.0f)) / packingresz;
  shiftcolumn = nccolumn - occolumn;
  shiftrow = ncrow - ocrow;
  shiftplane = ncplane - ocplane;
  m_Centroids[3*gnum] = xc;
  m_Centroids[3*gnum+1] = yc;
  m_Centroids[3*gnum+2] = zc;
  size_t size = columnlist[gnum].size();

  for (size_t i = 0; i < size; i++)
  {
    int& cl = columnlist[gnum][i];
    cl += shiftcolumn;
    int& rl = rowlist[gnum][i];
    rl += shiftrow;
    int& pl = planelist[gnum][i];
    pl += shiftplane;
  }
}


void InsertPrecipitatePhases::determine_neighbors(size_t gnum, int add)
{
  DataContainer* m = getDataContainer();
  float x, y, z;
  float xn, yn, zn;
  float dia, dia2;
  float dx, dy, dz;
//  int nnum = 0;
//  nnum = 0;
  x = m_Centroids[3*gnum];
  y = m_Centroids[3*gnum+1];
  z = m_Centroids[3*gnum+2];
  dia = m_EquivalentDiameters[gnum];
  for (size_t n = firstPrecipitateField; n < m->getNumFieldTuples(); n++)
  {
    xn = m_Centroids[3*n];
    yn = m_Centroids[3*n+1];
    zn = m_Centroids[3*n+2];
    dia2 = m_EquivalentDiameters[n];
    dx = fabs(x - xn);
    dy = fabs(y - yn);
    dz = fabs(z - zn);
    if(dx < dia && dy < dia && dz < dia)
    {
        if(add > 0) m_Neighborhoods[gnum]++;
        if(add < 0) m_Neighborhoods[gnum] = m_Neighborhoods[gnum] - 1;
    }
    if(dx < dia2 && dy < dia2 && dz < dia2)
    {
        if(add > 0) m_Neighborhoods[n]++;
        if(add < 0) m_Neighborhoods[n] = m_Neighborhoods[n] - 1;
    }
  }
}

float InsertPrecipitatePhases::check_neighborhooderror(int gadd, int gremove)
{
  DataContainer* m = getDataContainer();

  StatsDataArray& statsDataArray = *m_StatsDataArray;

  float neighborerror;
  float bhattdist;
  float dia;
  int nnum;
  size_t bin = 0;
  int index = 0;
  std::vector<int> count;
  int phase;
  for (size_t iter = 0; iter < simneighbordist.size(); ++iter)
  {
    phase = precipitatephases[iter];
	PrecipitateStatsData* pp = PrecipitateStatsData::SafePointerDownCast(statsDataArray[phase].get());
    count.resize(simneighbordist[iter].size(), 0);
    for (size_t i = 0; i < simneighbordist[iter].size(); i++)
    {
	  simneighbordist[iter][i].resize(40);
	  for (size_t j = 0; j < 40; j++)
	  {
	      simneighbordist[iter][i][j] = 0;
	  }
    }
    if(gadd > 0 && m_FieldPhases[gadd] == phase)
    {
      determine_neighbors(gadd, 1);
    }
    if(gremove > 0 && m_FieldPhases[gremove] == phase)
    {
      determine_neighbors(gremove, -1);
    }
    for (size_t i = firstPrecipitateField; i < m->getNumFieldTuples(); i++)
    {
      nnum = 0;
      index = i;
      if(index != gremove && m_FieldPhases[index] == phase)
      {
        dia = m_EquivalentDiameters[index];
        if(dia > pp->getMaxGrainDiameter()) dia = pp->getMaxGrainDiameter();
        if(dia < pp->getMinGrainDiameter()) dia = pp->getMinGrainDiameter();
		dia = int((dia - pp->getMinGrainDiameter()) / pp->getBinStepSize());
        nnum = m_Neighborhoods[index];
		bin = nnum/2;
		if(bin >= 40) bin = 39;
        simneighbordist[iter][dia][bin]++;
        count[dia]++;
      }
    }
    if(gadd > 0 && m_FieldPhases[gadd] == phase)
    {
      dia = m_EquivalentDiameters[index];
      if(dia > pp->getMaxGrainDiameter()) dia = pp->getMaxGrainDiameter();
      if(dia < pp->getMinGrainDiameter()) dia = pp->getMinGrainDiameter();
	  dia = int((dia - pp->getMinGrainDiameter()) / pp->getBinStepSize());
      nnum = m_Neighborhoods[index];
	  bin = nnum/2;
	  if(bin >= 40) bin = 39;
      simneighbordist[iter][dia][bin]++;
      count[dia]++;
    }
    for (size_t i = 0; i < simneighbordist[iter].size(); i++)
    {
      for (size_t j = 0; j < 40; j++)
      {
        simneighbordist[iter][i][j] = simneighbordist[iter][i][j] / double(count[i]);
        if(count[i] == 0) simneighbordist[iter][i][j] = 0.0;
      }
    }
    if(gadd > 0 && m_FieldPhases[gadd] == phase)
    {
      determine_neighbors(gadd, -1);
    }
    if(gremove > 0 && m_FieldPhases[gremove] == phase)
    {
      determine_neighbors(gremove, 1);
    }
  }
  compare_3Ddistributions(simneighbordist, neighbordist, bhattdist);
  neighborerror = bhattdist;
  return neighborerror;
}

void InsertPrecipitatePhases::compare_1Ddistributions(std::vector<float> array1, std::vector<float> array2, float &bhattdist)
{
  bhattdist = 0;
  for (size_t i = 0; i < array1.size(); i++)
  {
    bhattdist = bhattdist + sqrt((array1[i]*array2[i]));
  }
}
void InsertPrecipitatePhases::compare_2Ddistributions(std::vector<std::vector<float> > array1, std::vector<std::vector<float> > array2, float &bhattdist)
{
  bhattdist = 0;
  for (size_t i = 0; i < array1.size(); i++)
  {
    for (size_t j = 0; j < array1[i].size(); j++)
    {
      bhattdist = bhattdist + sqrt((array1[i][j] * array2[i][j]));
    }
  }
}

void InsertPrecipitatePhases::compare_3Ddistributions(std::vector<std::vector<std::vector<float> > > array1, std::vector<std::vector<std::vector<float> > > array2, float &bhattdist)
{
  bhattdist = 0;
  for (size_t i = 0; i < array1.size(); i++)
  {
    for (size_t j = 0; j < array1[i].size(); j++)
    {
      for (size_t k = 0; k < array1[i][j].size(); k++)
      {
        bhattdist = bhattdist + sqrt((array1[i][j][k]*array2[i][j][k]));
      }
    }
  }
}

float InsertPrecipitatePhases::check_sizedisterror(Precip* precip)
{
  DataContainer* m = getDataContainer();

  StatsDataArray& statsDataArray = *m_StatsDataArray;

  float dia;
  float sizedisterror = 0;
  float bhattdist;
  int index;
  int count = 0;
  int phase;
  for (size_t iter = 0; iter < grainsizedist.size(); ++iter)
  {
    phase = precipitatephases[iter];
	PrecipitateStatsData* pp = PrecipitateStatsData::SafePointerDownCast(statsDataArray[phase].get());
    count = 0;
    for (size_t i = 0; i < grainsizedist[iter].size(); i++)
    {
      simgrainsizedist[iter][i] = 0.0f;
    }
    for (size_t b = firstPrecipitateField; b < m->getNumFieldTuples(); b++)
    {
      index = b;
      if(m_FieldPhases[index] == phase)
      {
        dia = m_EquivalentDiameters[index];
        dia = (dia - (pp->getMinGrainDiameter() / 2.0f)) / grainsizediststep[iter];
        if(dia < 0) dia = 0;
        if(dia > grainsizedist[iter].size() - 1) dia = grainsizedist[iter].size() - 1;
        simgrainsizedist[iter][int(dia)]++;
        count++;
      }
    }
    if(precip->m_FieldPhases == phase)
    {
      dia = precip->m_EquivalentDiameters;
      dia = (dia - (pp->getMinGrainDiameter() / 2.0f)) / grainsizediststep[iter];
      if(dia < 0) dia = 0;
      if(dia > grainsizedist[iter].size() - 1) dia = grainsizedist[iter].size() - 1;
      simgrainsizedist[iter][int(dia)]++;
      count++;
    }
    for (size_t i = 0; i < grainsizedist[iter].size(); i++)
    {
      simgrainsizedist[iter][i] = simgrainsizedist[iter][i] / float(count);
      if(count == 0) simgrainsizedist[iter][i] = 0.0;
    }
  }
  compare_2Ddistributions(simgrainsizedist, grainsizedist, bhattdist);
  sizedisterror = bhattdist;
  return sizedisterror;
}

float InsertPrecipitatePhases::check_fillingerror(int gadd, int gremove)
{
  fillingerror = fillingerror * float(packingtotalpoints);
  int col, row, plane;
  if(gadd > 0)
  {
    size_t size = columnlist[gadd].size();
    std::vector<int>& cl_gadd = columnlist[gadd];
    std::vector<int>& rl_gadd = rowlist[gadd];
    std::vector<int>& pl_gadd = planelist[gadd];
    float packquality = 0;
    for (size_t i = 0; i < size; i++)
    {
      col = cl_gadd[i];
      row = rl_gadd[i];
      plane = pl_gadd[i];

      if(m_PeriodicBoundaries == true)
      {
        if(col < 0) col = col + packingxpoints;
        if(col > packingxpoints - 1) col = col - packingxpoints;
        if(row < 0) row = row + packingypoints;
        if(row > packingypoints - 1) row = row - packingypoints;
        if(plane < 0) plane = plane + packingzpoints;
        if(plane > packingzpoints - 1) plane = plane - packingzpoints;
        int& currentGrainOwner = grainowners[col][row][plane];
        fillingerror = fillingerror + (2 * currentGrainOwner - 1);
        packquality = packquality + ((currentGrainOwner) * (currentGrainOwner));
        ++currentGrainOwner;
      }
      else
      {
        if(col >= 0 && col <= packingxpoints - 1 && row >= 0 && row <= packingypoints - 1 && plane >= 0 && plane <= packingzpoints - 1)
        {
          int& currentGrainOwner = grainowners[col][row][plane];
          fillingerror = fillingerror + (2 * currentGrainOwner - 1);
          packquality = packquality + ((currentGrainOwner) * (currentGrainOwner));
          ++currentGrainOwner;
        }
      }
    }
    packqualities[gadd] = packquality / float(size);
  }
  if(gremove > 0)
  {
    size_t size = columnlist[gremove].size();
    std::vector<int>& cl_gremove = columnlist[gremove];
    std::vector<int>& rl_gremove = rowlist[gremove];
    std::vector<int>& pl_gremove = planelist[gremove];
    for (size_t i = 0; i < size; i++)
    {
      col = cl_gremove[i];
      row = rl_gremove[i];
      plane = pl_gremove[i];
      if(m_PeriodicBoundaries == true)
      {
        if(col < 0) col = col + packingxpoints;
        if(col > packingxpoints - 1) col = col - packingxpoints;
        if(row < 0) row = row + packingypoints;
        if(row > packingypoints - 1) row = row - packingypoints;
        if(plane < 0) plane = plane + packingzpoints;
        if(plane > packingzpoints - 1) plane = plane - packingzpoints;
        int& currentGrainOwner = grainowners[col][row][plane];
		fillingerror = fillingerror + (-2 * currentGrainOwner + 3);
        currentGrainOwner = currentGrainOwner - 1;
      }
      else
      {
        if(col >= 0 && col <= packingxpoints - 1 && row >= 0 && row <= packingypoints - 1 && plane >= 0 && plane <= packingzpoints - 1)
        {
          int& currentGrainOwner = grainowners[col][row][plane];
          fillingerror = fillingerror + (-2 * currentGrainOwner + 3);
          currentGrainOwner = currentGrainOwner - 1;
        }
      }
    }
  }
  fillingerror = fillingerror / float(packingtotalpoints);
  return fillingerror;
}

void InsertPrecipitatePhases::insert_precipitate(size_t gnum)
{
  DREAM3D_RANDOMNG_NEW()
 //   DataContainer* m = getDataContainer();
//  float dist;
  float inside = -1;
  int column, row, plane;
  int centercolumn, centerrow, centerplane;
  int xmin, xmax, ymin, ymax, zmin, zmax;
  float xc, yc, zc;
  float xp, yp, zp;
  float x, y, z;
  float volcur = m_Volumes[gnum];
  float bovera = m_AxisLengths[3*gnum+1];
  float covera = m_AxisLengths[3*gnum+2];
  float omega3 = m_Omega3s[gnum];
  float radcur1 = 1;
  unsigned int shapeclass = m_ShapeTypes[m_FieldPhases[gnum]];

  // init any values for each of the Shape Ops
  for (std::map<unsigned int, ShapeOps*>::iterator ops = m_ShapeOps.begin(); ops != m_ShapeOps.end(); ++ops)
  {
    (*ops).second->init();
  }
  // Create our Argument Map
  std::map<ShapeOps::ArgName, float> shapeArgMap;
  shapeArgMap[ShapeOps::Omega3] = omega3;
  shapeArgMap[ShapeOps::VolCur] = volcur;
  shapeArgMap[ShapeOps::B_OverA] = bovera;
  shapeArgMap[ShapeOps::C_OverA] = covera;

  radcur1 = m_ShapeOps[shapeclass]->radcur1(shapeArgMap);

  float radcur2 = (radcur1 * bovera);
  float radcur3 = (radcur1 * covera);
  float phi1 = m_AxisEulerAngles[3*gnum];
  float PHI = m_AxisEulerAngles[3*gnum+1];
  float phi2 = m_AxisEulerAngles[3*gnum+2];
  float ga[3][3];
  ga[0][0] = cosf(phi1) * cosf(phi2) - sinf(phi1) * sinf(phi2) * cosf(PHI);
  ga[0][1] = sinf(phi1) * cosf(phi2) + cosf(phi1) * sinf(phi2) * cosf(PHI);
  ga[0][2] = sinf(phi2) * sinf(PHI);
  ga[1][0] = -cosf(phi1) * sinf(phi2) - sinf(phi1) * cosf(phi2) * cosf(PHI);
  ga[1][1] = -sinf(phi1) * sinf(phi2) + cosf(phi1) * cosf(phi2) * cosf(PHI);
  ga[1][2] = cosf(phi2) * sinf(PHI);
  ga[2][0] = sinf(phi1) * sinf(PHI);
  ga[2][1] = -cosf(phi1) * sinf(PHI);
  ga[2][2] = cosf(PHI);
  xc = m_Centroids[3*gnum];
  yc = m_Centroids[3*gnum+1];
  zc = m_Centroids[3*gnum+2];
  centercolumn = (xc - (packingresx / 2)) / packingresx;
  centerrow = (yc - (packingresy / 2)) / packingresy;
  centerplane = (zc - (packingresz / 2)) / packingresz;
  xmin = int(centercolumn - ((radcur1 / packingresx) + 1));
  xmax = int(centercolumn + ((radcur1 / packingresx) + 1));
  ymin = int(centerrow - ((radcur1 / packingresy) + 1));
  ymax = int(centerrow + ((radcur1 / packingresy) + 1));
  zmin = int(centerplane - ((radcur1 / packingresz) + 1));
  zmax = int(centerplane + ((radcur1 / packingresz) + 1));
  if(xmin < -packingxpoints) xmin = -packingxpoints;
  if(xmax > 2 * packingxpoints - 1) xmax = (2 * packingxpoints - 1);
  if(ymin < -packingypoints) ymin = -packingypoints;
  if(ymax > 2 * packingypoints - 1) ymax = (2 * packingypoints - 1);
  if(zmin < -packingzpoints) zmin = -packingzpoints;
  if(zmax > 2 * packingzpoints - 1) zmax = (2 * packingzpoints - 1);
  for (int iter1 = xmin; iter1 < xmax + 1; iter1++)
  {
    for (int iter2 = ymin; iter2 < ymax + 1; iter2++)
    {
      for (int iter3 = zmin; iter3 < zmax + 1; iter3++)
      {
        column = iter1;
        row = iter2;
        plane = iter3;
        x = float(column) * packingresx;
        y = float(row) * packingresy;
        z = float(plane) * packingresz;
        inside = -1;
		x = x - xc;
		y = y - yc;
		z = z - zc;
		xp = (x*ga[0][0])+(y*ga[0][1])+(z*ga[0][2]);
		yp = (x*ga[1][0])+(y*ga[1][1])+(z*ga[1][2]);
		zp = (x*ga[2][0])+(y*ga[2][1])+(z*ga[2][2]);
		float axis1comp = xp / radcur1;
		float axis2comp = yp / radcur2;
		float axis3comp = zp / radcur3;
		inside = m_ShapeOps[shapeclass]->inside(axis1comp, axis2comp, axis3comp);
		if(inside >= 0)
		{
			columnlist[gnum].push_back(column);
			rowlist[gnum].push_back(row);
			planelist[gnum].push_back(plane);
		}
      }
    }
  }
}

void InsertPrecipitatePhases::assign_voxels()
{
  notify("Assigning Voxels", 0, Observable::UpdateProgressMessage);

  DataContainer* m = getDataContainer();
  int index;
  size_t udims[3] = {0,0,0};
  m->getDimensions(udims);
#if (CMP_SIZEOF_SIZE_T == 4)
  typedef int32_t DimType;
#else
  typedef int64_t DimType;
#endif
  DimType dims[3] = {
    static_cast<DimType>(udims[0]),
    static_cast<DimType>(udims[1]),
    static_cast<DimType>(udims[2]),
  };

  DimType neighpoints[6];
  neighpoints[0] = -dims[0]*dims[1];
  neighpoints[1] = -dims[0];
  neighpoints[2] = -1;
  neighpoints[3] = 1;
  neighpoints[4] = dims[0];
  neighpoints[5] = dims[0]*dims[1];

  float xRes = m->getXRes();
  float yRes = m->getYRes();
  float zRes = m->getZRes();

  int oldname;
  size_t column, row, plane;
  float inside;
  float xc, yc, zc;
  float xp, yp, zp;
  float dist;
  float x, y, z;
  DimType xmin, xmax, ymin, ymax, zmin, zmax;
 // int64_t totpoints = m->totalPoints();
  gsizes.resize(m->getNumFieldTuples());

  for (size_t i = firstPrecipitateField; i < m->getNumFieldTuples(); i++)
  {
    gsizes[i] = 0;
  }
  for (size_t i = firstPrecipitateField; i < m->getNumFieldTuples(); i++)
  {
    float volcur = m_Volumes[i];
    float bovera = m_AxisLengths[3*i+1];
    float covera = m_AxisLengths[3*i+2];
    float omega3 = m_Omega3s[i];
	xc = m_Centroids[3*i];
	yc = m_Centroids[3*i+1];
	zc = m_Centroids[3*i+2];
    float radcur1 = 0.0f;
    //Unbounded Check for the size of shapeTypes. We assume a 1:1 with phase
    unsigned int shapeclass = m_ShapeTypes[m_FieldPhases[i]];

    // init any values for each of the Shape Ops
    for (std::map<unsigned int, ShapeOps*>::iterator ops = m_ShapeOps.begin(); ops != m_ShapeOps.end(); ++ops )
    {
      (*ops).second->init();
    }
    // Create our Argument Map
    std::map<ShapeOps::ArgName, float> shapeArgMap;
    shapeArgMap[ShapeOps::Omega3] = omega3;
    shapeArgMap[ShapeOps::VolCur] = volcur;
    shapeArgMap[ShapeOps::B_OverA] = bovera;
    shapeArgMap[ShapeOps::C_OverA] = covera;

    radcur1 = m_ShapeOps[shapeclass]->radcur1(shapeArgMap);

    float radcur2 = (radcur1 * bovera);
    float radcur3 = (radcur1 * covera);
	float phi1 = m_AxisEulerAngles[3*i];
	float PHI = m_AxisEulerAngles[3*i+1];
	float phi2 = m_AxisEulerAngles[3*i+2];
    float ga[3][3];
    ga[0][0] = cosf(phi1) * cosf(phi2) - sinf(phi1) * sinf(phi2) * cosf(PHI);
    ga[0][1] = sinf(phi1) * cosf(phi2) + cosf(phi1) * sinf(phi2) * cosf(PHI);
    ga[0][2] = sinf(phi2) * sinf(PHI);
    ga[1][0] = -cosf(phi1) * sinf(phi2) - sinf(phi1) * cosf(phi2) * cosf(PHI);
    ga[1][1] = -sinf(phi1) * sinf(phi2) + cosf(phi1) * cosf(phi2) * cosf(PHI);
    ga[1][2] = cosf(phi2) * sinf(PHI);
    ga[2][0] = sinf(phi1) * sinf(PHI);
    ga[2][1] = -cosf(phi1) * sinf(PHI);
    ga[2][2] = cosf(PHI);
    column = (xc - (xRes / 2.0f)) / xRes;
    row = (yc - (yRes / 2.0f)) / yRes;
    plane = (zc - (zRes / 2.0f)) / zRes;
    xmin = int(column - ((radcur1 / xRes) + 1));
    xmax = int(column + ((radcur1 / xRes) + 1));
    ymin = int(row - ((radcur1 / yRes) + 1));
    ymax = int(row + ((radcur1 / yRes) + 1));
    zmin = int(plane - ((radcur1 / zRes) + 1));
    zmax = int(plane + ((radcur1 / zRes) + 1));
    if (m_PeriodicBoundaries == true)
    {
      if (xmin < -dims[0]) xmin = -dims[0];
      if (xmax > 2 * dims[0] - 1) xmax = (2 * dims[0] - 1);
      if (ymin < -dims[1]) ymin = -dims[1];
      if (ymax > 2 * dims[1] - 1) ymax = (2 * dims[1] - 1);
      if (zmin < -dims[2]) zmin = -dims[2];
      if (zmax > 2 * dims[2] - 1) zmax = (2 * dims[2] - 1);
    }
    if (m_PeriodicBoundaries == false)
    {
      if (xmin < 0) xmin = 0;
      if (xmax > dims[0] - 1) xmax = dims[0] - 1;
      if (ymin < 0) ymin = 0;
      if (ymax > dims[1] - 1) ymax = dims[1] - 1;
      if (zmin < 0) zmin = 0;
      if (zmax > dims[2] - 1) zmax = dims[2] - 1;
    }
    for (DimType iter1 = xmin; iter1 < xmax + 1; iter1++)
    {
      for (DimType iter2 = ymin; iter2 < ymax + 1; iter2++)
      {
        for (DimType iter3 = zmin; iter3 < zmax + 1; iter3++)
        {
          column = iter1;
          row = iter2;
          plane = iter3;
          if (iter1 < 0) column = iter1 + dims[0];
          if (iter1 > dims[0] - 1) column = iter1 - dims[0];
          if (iter2 < 0) row = iter2 + dims[1];
          if (iter2 > dims[1] - 1) row = iter2 - dims[1];
          if (iter3 < 0) plane = iter3 + dims[2];
          if (iter3 > dims[2] - 1) plane = iter3 - dims[2];
          index = (plane * dims[0] * dims[1]) + (row * dims[0]) + column;
          inside = -1;
          x = float(column) * xRes;
          y = float(row) * yRes;
          z = float(plane) * zRes;
          if (iter1 < 0) x = x - sizex;
          if (iter1 > dims[0] - 1) x = x + sizex;
          if (iter2 < 0) y = y - sizey;
          if (iter2 > dims[1] - 1) y = y + sizey;
          if (iter3 < 0) z = z - sizez;
          if (iter3 > dims[2] - 1) z = z + sizez;
          dist = ((x - xc) * (x - xc)) + ((y - yc) * (y - yc)) + ((z - zc) * (z - zc));
          dist = sqrtf(dist);
          if (dist < radcur1)
          {
            x = x - xc;
            y = y - yc;
            z = z - zc;
            xp = (x * ga[0][0]) + (y * ga[0][1]) + (z * ga[0][2]);
            yp = (x * ga[1][0]) + (y * ga[1][1]) + (z * ga[1][2]);
            zp = (x * ga[2][0]) + (y * ga[2][1]) + (z * ga[2][2]);
            float axis1comp = xp / radcur1;
            float axis2comp = yp / radcur2;
            float axis3comp = zp / radcur3;
            inside = m_ShapeOps[shapeclass]->inside(axis1comp, axis2comp, axis3comp);
            if (inside >= 0)
            {
              int currentpoint = index;
              if (m_GrainIds[currentpoint] > firstPrecipitateField)
              {
                oldname = m_GrainIds[currentpoint];
                m_GrainIds[currentpoint] = -2;
              }
              if (m_GrainIds[currentpoint] < firstPrecipitateField && m_GrainIds[currentpoint] != -2)
              {
                m_GrainIds[currentpoint] = static_cast<int32_t>(i);
              }
            }
          }
        }
      }
    }
  }
}

void InsertPrecipitatePhases::assign_gaps()
{
  notify("Assigning Gaps", 0, Observable::UpdateProgressMessage);

  DataContainer* m = getDataContainer();
  int64_t totpoints = m->getTotalPoints();

  size_t udims[3] = {0,0,0};
  m->getDimensions(udims);
#if (CMP_SIZEOF_SIZE_T == 4)
  typedef int32_t DimType;
#else
  typedef int64_t DimType;
#endif
  DimType dims[3] = {
    static_cast<DimType>(udims[0]),
    static_cast<DimType>(udims[1]),
    static_cast<DimType>(udims[2]),
  };


  int index;
  int timestep = 100;
  int unassignedcount = 1;
  DimType column, row, plane;
  float inside;
  float xc, yc, zc;
  float xp, yp, zp;
  float dist;
  float x, y, z;

  DimType xmin, xmax, ymin, ymax, zmin, zmax;

  float xRes = m->getXRes();
  float yRes = m->getYRes();
  float zRes = m->getZRes();

  Int32ArrayType::Pointer newownersPtr = Int32ArrayType::CreateArray(totpoints, "newowners");
  int32_t* newowners = newownersPtr->GetPointer(0);
  newownersPtr->initializeWithZeros();

  FloatArrayType::Pointer ellipfuncsPtr = FloatArrayType::CreateArray(totpoints, "ellipfuncs");
  float* ellipfuncs = ellipfuncsPtr->GetPointer(0);
  ellipfuncsPtr->initializeWithValues(-1);

  while (unassignedcount != 0)
  {
 	  unassignedcount = 0;
	  timestep = timestep + 50;
	  for (size_t i = firstPrecipitateField; i < m->getNumFieldTuples(); i++)
	  {
		float volcur = m_Volumes[i];
		float bovera = m_AxisLengths[3*i+1];
		float covera = m_AxisLengths[3*i+2];
		float omega3 = m_Omega3s[i];
		xc = m_Centroids[3*i];
		yc = m_Centroids[3*i+1];
		zc = m_Centroids[3*i+2];
		float radcur1 = 0.0f;
		//Unbounded Check for the size of shapeTypes. We assume a 1:1 with phase
		unsigned int shapeclass = m_ShapeTypes[m_FieldPhases[i]];

		// init any values for each of the Shape Ops
		for (std::map<unsigned int, ShapeOps*>::iterator ops = m_ShapeOps.begin(); ops != m_ShapeOps.end(); ++ops )
		{
		  (*ops).second->init();
		}
		// Create our Argument Map
		std::map<ShapeOps::ArgName, float> shapeArgMap;
		shapeArgMap[ShapeOps::Omega3] = omega3;
		shapeArgMap[ShapeOps::VolCur] = volcur;
		shapeArgMap[ShapeOps::B_OverA] = bovera;
		shapeArgMap[ShapeOps::C_OverA] = covera;

		radcur1 = m_ShapeOps[shapeclass]->radcur1(shapeArgMap);

		float radcur2 = (radcur1 * bovera);
		float radcur3 = (radcur1 * covera);
		radcur1 = (float(timestep)/100.0)*radcur1;
		radcur2 = (float(timestep)/100.0)*radcur2;
		radcur3 = (float(timestep)/100.0)*radcur3;
		float phi1 = m_AxisEulerAngles[3*i];
		float PHI = m_AxisEulerAngles[3*i+1];
		float phi2 = m_AxisEulerAngles[3*i+2];
		float ga[3][3];
		ga[0][0] = cosf(phi1) * cosf(phi2) - sinf(phi1) * sinf(phi2) * cosf(PHI);
		ga[0][1] = sinf(phi1) * cosf(phi2) + cosf(phi1) * sinf(phi2) * cosf(PHI);
		ga[0][2] = sinf(phi2) * sinf(PHI);
		ga[1][0] = -cosf(phi1) * sinf(phi2) - sinf(phi1) * cosf(phi2) * cosf(PHI);
		ga[1][1] = -sinf(phi1) * sinf(phi2) + cosf(phi1) * cosf(phi2) * cosf(PHI);
		ga[1][2] = cosf(phi2) * sinf(PHI);
		ga[2][0] = sinf(phi1) * sinf(PHI);
		ga[2][1] = -cosf(phi1) * sinf(PHI);
		ga[2][2] = cosf(PHI);
		column = (xc - (xRes / 2.0f)) / xRes;
		row = (yc - (yRes / 2.0f)) / yRes;
		plane = (zc - (zRes / 2.0f)) / zRes;
		xmin = int(column - ((radcur1 / xRes) + 1));
		xmax = int(column + ((radcur1 / xRes) + 1));
		ymin = int(row - ((radcur1 / yRes) + 1));
		ymax = int(row + ((radcur1 / yRes) + 1));
		zmin = int(plane - ((radcur1 / zRes) + 1));
		zmax = int(plane + ((radcur1 / zRes) + 1));
		if (m_PeriodicBoundaries == true)
		{
		  if (xmin < -dims[0]) xmin = -dims[0];
		  if (xmax > 2 * dims[0] - 1) xmax = (2 *dims[0] - 1);
		  if (ymin < -dims[1]) ymin = -dims[1];
		  if (ymax > 2 * dims[1] - 1) ymax = (2 * dims[1] - 1);
		  if (zmin < -dims[2]) zmin = -dims[2];
		  if (zmax > 2 * dims[2] - 1) zmax = (2 * dims[2] - 1);
		}
		if (m_PeriodicBoundaries == false)
		{
		  if (xmin < 0) xmin = 0;
		  if (xmax > dims[0] - 1) xmax = dims[0] - 1;
		  if (ymin < 0) ymin = 0;
		  if (ymax > dims[1] - 1) ymax = dims[1] - 1;
		  if (zmin < 0) zmin = 0;
		  if (zmax > dims[2] - 1) zmax = dims[2] - 1;
		}
		for (DimType iter1 = xmin; iter1 < xmax + 1; iter1++)
		{
		  for (DimType iter2 = ymin; iter2 < ymax + 1; iter2++)
		  {
			for (DimType iter3 = zmin; iter3 < zmax + 1; iter3++)
			{
			  column = iter1;
			  row = iter2;
			  plane = iter3;
			  if (iter1 < 0) column = iter1 + dims[0];
			  if (iter1 > dims[0] - 1) column = iter1 - dims[0];
			  if (iter2 < 0) row = iter2 + dims[1];
			  if (iter2 > dims[1] - 1) row = iter2 - dims[1];
			  if (iter3 < 0) plane = iter3 + dims[2];
			  if (iter3 > dims[2] - 1) plane = iter3 - dims[2];
			  index = (plane * dims[0] * dims[1]) + (row * dims[0]) + column;
			  if(m_GrainIds[index] <= 0)
			  {
				  inside = -1;
				  x = float(column) * xRes;
				  y = float(row) * yRes;
				  z = float(plane) * zRes;
				  if (iter1 < 0) x = x - sizex;
				  if (iter1 > dims[0] - 1) x = x + sizex;
				  if (iter2 < 0) y = y - sizey;
				  if (iter2 > dims[1] - 1) y = y + sizey;
				  if (iter3 < 0) z = z - sizez;
				  if (iter3 > dims[2] - 1) z = z + sizez;
				  dist = ((x - xc) * (x - xc)) + ((y - yc) * (y - yc)) + ((z - zc) * (z - zc));
				  dist = sqrtf(dist);
				  if (dist < radcur1)
				  {
					x = x - xc;
					y = y - yc;
					z = z - zc;
					xp = (x * ga[0][0]) + (y * ga[0][1]) + (z * ga[0][2]);
				    yp = (x * ga[1][0]) + (y * ga[1][1]) + (z * ga[1][2]);
			        zp = (x * ga[2][0]) + (y * ga[2][1]) + (z * ga[2][2]);
					float axis1comp = xp / radcur1;
					float axis2comp = yp / radcur2;
					float axis3comp = zp / radcur3;
					inside = m_ShapeOps[shapeclass]->inside(axis1comp, axis2comp, axis3comp);
					if (inside >= 0 && inside > ellipfuncs[index])
					{
						newowners[index] = i;
						ellipfuncs[index] = inside;
					}
				  }
			  }
			}
		  }
		}
	  }
	  for (int i = 0; i < totpoints; i++)
	  {
	    if (ellipfuncs[i] >= 0) m_GrainIds[i] = newowners[i];
		if (m_GrainIds[i] <= 0) unassignedcount++;
		newowners[i] = -1;
		ellipfuncs[i] = -1.0;
	  }
  }
  for (int i = 0; i < totpoints; i++)
  {
	  if(m_GrainIds[i] > 0) m_CellPhases[i] = m_FieldPhases[m_GrainIds[i]];
  }
}
void InsertPrecipitatePhases::cleanup_grains()
{
  notify("Cleaning Up Grains", 0, Observable::UpdateProgressMessage);

  DataContainer* m = getDataContainer();

  StatsDataArray& statsDataArray = *m_StatsDataArray;

  int64_t totpoints = m->getTotalPoints();
  size_t udims[3] = {0,0,0};
  m->getDimensions(udims);
#if (CMP_SIZEOF_SIZE_T == 4)
  typedef int32_t DimType;
#else
  typedef int64_t DimType;
#endif
  DimType dims[3] = {
    static_cast<DimType>(udims[0]),
    static_cast<DimType>(udims[1]),
    static_cast<DimType>(udims[2]),
  };

  DimType neighpoints[6];
  DimType xp = dims[0];
  DimType yp = dims[1];
  DimType zp = dims[2];

  neighpoints[0] = -(xp * yp);
  neighpoints[1] = -xp;
  neighpoints[2] = -1;
  neighpoints[3] = 1;
  neighpoints[4] = xp;
  neighpoints[5] = (xp * yp);
  std::vector<std::vector<int> > vlists;
  vlists.resize(m->getNumFieldTuples());
  std::vector<int> currentvlist;
  std::vector<bool> checked;
  checked.resize(totpoints,false);
  size_t count;
  int touchessurface = 0;
  int good;
  int neighbor;
  DimType column, row, plane;
  int index;
  float minsize = 0;
  gsizes.resize(m->getNumFieldTuples());
  for (size_t i = 1; i < m->getNumFieldTuples(); i++)
  {
    gsizes[i] = 0;
	m_Active[i] = true;
  }

  float resConst = m->getXRes() * m->getYRes() * m->getZRes();
  for (int i = 0; i < totpoints; i++)
  {
    touchessurface = 0;
    if(checked[i] == false && m_GrainIds[i] > 0)
    {
	  PrecipitateStatsData* pp = PrecipitateStatsData::SafePointerDownCast(statsDataArray[m_CellPhases[i]].get());
	  minsize = pp->getMinGrainDiameter() * pp->getMinGrainDiameter() * pp->getMinGrainDiameter() * M_PI / 6.0f;
      minsize = int(minsize / (resConst));
      currentvlist.push_back(i);
      count = 0;
      while (count < currentvlist.size())
      {
        index = currentvlist[count];
        column = index % xp;
        row = (index / xp) % yp;
        plane = index / (xp * yp);
        if(column == 0 || column == xp || row == 0 || row == yp || plane == 0 || plane == zp) touchessurface = 1;
        for (int j = 0; j < 6; j++)
        {
          good = 1;
          neighbor = index + neighpoints[j];
          if(m_PeriodicBoundaries == false)
          {
            if(j == 0 && plane == 0) good = 0;
            if(j == 5 && plane == (zp - 1)) good = 0;
            if(j == 1 && row == 0) good = 0;
            if(j == 4 && row == (yp - 1)) good = 0;
            if(j == 2 && column == 0) good = 0;
            if(j == 3 && column == (xp - 1)) good = 0;
            if(good == 1 && m_GrainIds[neighbor] == m_GrainIds[index] && checked[neighbor] == false)
            {
              currentvlist.push_back(neighbor);
              checked[neighbor] = true;
            }
          }
          else if(m_PeriodicBoundaries == true)
          {
            if(j == 0 && plane == 0) neighbor = neighbor + (xp * yp * zp);
            if(j == 5 && plane == (zp - 1)) neighbor = neighbor - (xp * yp * zp);
            if(j == 1 && row == 0) neighbor = neighbor + (xp * yp);
            if(j == 4 && row == (yp - 1)) neighbor = neighbor - (xp * yp);
            if(j == 2 && column == 0) neighbor = neighbor + (xp);
            if(j == 3 && column == (xp - 1)) neighbor = neighbor - (xp);
            if(m_GrainIds[neighbor] == m_GrainIds[index] && checked[neighbor] == false)
            {
              currentvlist.push_back(neighbor);
              checked[neighbor] = true;
            }
          }
        }
        count++;
      }
      size_t size = vlists[m_GrainIds[i]].size();
      if(size > 0)
      {
        if(size < currentvlist.size())
        {
          for (size_t k = 0; k < vlists[m_GrainIds[i]].size(); k++)
          {
            m_GrainIds[vlists[m_GrainIds[i]][k]] = -1;
          }
          vlists[m_GrainIds[i]].resize(currentvlist.size());
          vlists[m_GrainIds[i]].swap(currentvlist);
        }
        else if(size >= currentvlist.size())
        {
          for (size_t k = 0; k < currentvlist.size(); k++)
          {
            m_GrainIds[currentvlist[k]] = -1;
          }
        }
      }
      else if(size == 0)
      {
        if(currentvlist.size() >= minsize || touchessurface == 1)
        {
          vlists[m_GrainIds[i]].resize(currentvlist.size());
          vlists[m_GrainIds[i]].swap(currentvlist);
        }
        if(currentvlist.size() < minsize && touchessurface == 0)
        {
          for (size_t k = 0; k < currentvlist.size(); k++)
          {
            m_GrainIds[currentvlist[k]] = -1;
          }
        }
      }
      currentvlist.clear();
    }
  }
  assign_gaps();
  for (int i = 0; i < totpoints; i++)
  {
	if(m_GrainIds[i] > 0) gsizes[m_GrainIds[i]]++;
  }
  for (size_t i = firstPrecipitateField; i < m->getNumFieldTuples(); i++)
  {
     if(gsizes[i] == 0) m_Active[i] = false;
  }
  for (int i = 0; i < totpoints; i++)
  {
	  if(m_GrainIds[i] > 0) { m_CellPhases[i] = m_FieldPhases[m_GrainIds[i]]; }
  }
}

void InsertPrecipitatePhases::initialize_packinggrid()
{
  DataContainer* m = getDataContainer();

  packingresx = m->getXRes() * 2.0f;
  packingresy = m->getYRes() * 2.0f;
  packingresz = m->getZRes() * 2.0f;
  packingxpoints = int(sizex / packingresx);
  packingypoints = int(sizey / packingresy);
  packingzpoints = int(sizez / packingresz);
  packingtotalpoints = packingxpoints * packingypoints * packingzpoints;
  grainowners.resize(packingxpoints);
  for (int i = 0; i < packingxpoints; i++)
  {
    grainowners[i].resize(packingypoints);
    for (int j = 0; j < packingypoints; j++)
    {
      grainowners[i][j].resize(packingzpoints, 0);
    }
  }
}
float InsertPrecipitatePhases::find_xcoord(long long int index)
{
  DataContainer* m = getDataContainer();
  float x = m->getXRes()*float(index%m->getXPoints());
  return x;
}
float InsertPrecipitatePhases::find_ycoord(long long int index)
{
  DataContainer* m = getDataContainer();
  float y = m->getYRes()*float((index/m->getXPoints())%m->getYPoints());
  return y;
}
float InsertPrecipitatePhases::find_zcoord(long long int index)
{
  DataContainer* m = getDataContainer();
  float z = m->getZRes()*float(index/(m->getXPoints()*m->getYPoints()));
  return z;
}
