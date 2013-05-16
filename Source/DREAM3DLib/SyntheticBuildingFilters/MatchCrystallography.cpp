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

#include "MatchCrystallography.h"

#include "DREAM3DLib/Common/DREAM3DMath.h"
#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Common/DataContainerMacros.h"
#include "DREAM3DLib/Common/DREAM3DRandom.h"
#include "DREAM3DLib/Common/OrientationMath.h"

#include "DREAM3DLib/OrientationOps/CubicOps.h"
#include "DREAM3DLib/OrientationOps/HexagonalOps.h"
#include "DREAM3DLib/OrientationOps/OrthoRhombicOps.h"

#include "DREAM3DLib/StatisticsFilters/FindNeighbors.h"
#include "DREAM3DLib/GenericFilters/FindSurfaceGrains.h"
#include "DREAM3DLib/GenericFilters/FindGrainPhases.h"
#include "DREAM3DLib/StatisticsFilters/FindNumFields.h"

#include "DREAM3DLib/Common/PrecipitateStatsData.h"
#include "DREAM3DLib/Common/PrimaryStatsData.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------

MatchCrystallography::MatchCrystallography() :
AbstractFilter(),
m_GrainIdsArrayName(DREAM3D::CellData::GrainIds),
m_CellEulerAnglesArrayName(DREAM3D::CellData::EulerAngles),
m_FieldPhasesArrayName(DREAM3D::FieldData::Phases),
m_SurfaceFieldsArrayName(DREAM3D::FieldData::SurfaceFields),
m_AvgQuatsArrayName(DREAM3D::FieldData::AvgQuats),
m_FieldEulerAnglesArrayName(DREAM3D::FieldData::EulerAngles),
m_VolumesArrayName(DREAM3D::FieldData::Volumes),
m_CrystalStructuresArrayName(DREAM3D::EnsembleData::CrystalStructures),
m_PhaseTypesArrayName(DREAM3D::EnsembleData::PhaseTypes),
m_NumFieldsArrayName(DREAM3D::EnsembleData::NumFields),
m_NeighborListArrayName(DREAM3D::FieldData::NeighborList),
m_SharedSurfaceAreaListArrayName(DREAM3D::FieldData::SharedSurfaceAreaList),
m_StatsDataArrayName(DREAM3D::EnsembleData::Statistics),
m_MaxIterations(1),
m_GrainIds(NULL),
m_CellEulerAngles(NULL),
m_SurfaceFields(NULL),
m_FieldPhases(NULL),
m_Volumes(NULL),
m_FieldEulerAngles(NULL),
m_AvgQuats(NULL),
m_NeighborList(NULL),
m_SharedSurfaceAreaList(NULL),
m_CrystalStructures(NULL),
m_PhaseTypes(NULL),
m_NumFields(NULL)
{
  m_OrientationOps = OrientationMath::getOrientationOpsVector();
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MatchCrystallography::~MatchCrystallography()
{
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MatchCrystallography::setupFilterParameters()
{
  std::vector<FilterParameter::Pointer> parameters;
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Maximum Number of Iterations (Swaps)");
    option->setPropertyName("MaxIterations");
    option->setWidgetType(FilterParameter::IntWidget);
    option->setValueType("int");
    option->setUnits("");
    parameters.push_back(option);
  }
  setFilterParameters(parameters);
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MatchCrystallography::writeFilterParameters(AbstractFilterParametersWriter* writer)
{
  writer->writeValue("MaxIterations", getMaxIterations() );
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MatchCrystallography::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;
  VoxelDataContainer* m = getVoxelDataContainer();
  //int err = 0;
  // Cell Data
  GET_PREREQ_DATA( m, DREAM3D, CellData, GrainIds, ss, -301, int32_t, Int32ArrayType, voxels, 1)
  CREATE_NON_PREREQ_DATA( m, DREAM3D, CellData, CellEulerAngles, ss, float, FloatArrayType, 0, voxels, 3)

  // Field Data
  GET_PREREQ_DATA(m, DREAM3D, FieldData, SurfaceFields, ss, -302, bool, BoolArrayType, fields, 1)

  GET_PREREQ_DATA(m, DREAM3D, FieldData, FieldPhases, ss, -303, int32_t, Int32ArrayType, fields, 1)


  CREATE_NON_PREREQ_DATA(m, DREAM3D, FieldData, Volumes, ss, float, FloatArrayType, 0, fields, 1)
  CREATE_NON_PREREQ_DATA(m, DREAM3D, FieldData, FieldEulerAngles, ss, float, FloatArrayType, 0, fields, 3)
  CREATE_NON_PREREQ_DATA(m, DREAM3D, FieldData, AvgQuats, ss, float, FloatArrayType, 0, fields, 5)



  // Now we are going to get a "Pointer" to the NeighborList object out of the DataContainer
  if (NULL == m->getFieldData(m_NeighborListArrayName).get())
  {
      ss.str("");
      ss << "'NeighborLists' are not available and are required for this filter to run. A filter that generates NeighborLists needs to be placed before this filter in the pipeline." << std::endl;
      setErrorCondition(-305);
      addErrorMessage(getHumanLabel(), ss.str(), getErrorCondition());
  }
  else
  {
    m_NeighborList = NeighborList<int>::SafeObjectDownCast<IDataArray*, NeighborList<int>*>(m->getFieldData(DREAM3D::FieldData::NeighborList).get());
  }

  if(NULL == m->getFieldData(m_SharedSurfaceAreaListArrayName).get())
  {
      ss.str("");
      ss << "'SharedSurfaceAreaLists' are not available and are required for this filter to run. A filter that generates 'Shared SurfaceArea Lists' needs to be placed before this filter in the pipeline." << std::endl;
      setErrorCondition(-306);
      addErrorMessage(getHumanLabel(), ss.str(), getErrorCondition());
  }
  else
  {
    m_SharedSurfaceAreaList = NeighborList<float>::SafeObjectDownCast<IDataArray*, NeighborList<float>*>(m->getFieldData(DREAM3D::FieldData::SharedSurfaceAreaList).get());
  }

  if(NULL == m->getEnsembleData(m_StatsDataArrayName).get())
  {
    ss.str("");
    ss << "'Ensemble Statistics' are not available and are required for this filter to run. A filter that generates 'Shared SurfaceArea Lists' needs to be placed before this filter in the pipeline." << std::endl;
    setErrorCondition(-310);
    addErrorMessage(getHumanLabel(), ss.str(), getErrorCondition());
  }
  else
  {
    m_StatsDataArray = StatsDataArray::SafeObjectDownCast<IDataArray*, StatsDataArray*>(m->getEnsembleData(m_StatsDataArrayName).get());
  }
  // Ensemble Data
  typedef DataArray<unsigned int> XTalStructArrayType;
  typedef DataArray<unsigned int> PhaseTypeArrayType;
  GET_PREREQ_DATA(m, DREAM3D, EnsembleData, CrystalStructures, ss, -307, unsigned int, XTalStructArrayType, ensembles, 1)
  GET_PREREQ_DATA(m, DREAM3D, EnsembleData, PhaseTypes, ss, -307, unsigned int, PhaseTypeArrayType, ensembles, 1)
  GET_PREREQ_DATA(m, DREAM3D, EnsembleData, NumFields, ss, -308, int32_t, Int32ArrayType, ensembles, 1)



}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MatchCrystallography::preflight()
{
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MatchCrystallography::execute()
{
  int err = 0;
  setErrorCondition(err);
  VoxelDataContainer* m = getVoxelDataContainer();
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The DataContainer Object was NULL", -999);
    return;
  }

  int64_t totalPoints = m->getTotalPoints();
  int totalFields = m->getNumFieldTuples();
  int numEnsembleTuples = m->getNumEnsembleTuples();
  dataCheck(false, totalPoints, totalFields, numEnsembleTuples);
  if (getErrorCondition() < 0)
  {
    return;
  }


  std::stringstream ss;
  ss << "Determining Volumes";
  notifyStatusMessage(ss.str());
  determine_volumes();
  ss.str("");

  ss << "Determining Boundary Areas";
  notifyStatusMessage(ss.str());
  determine_boundary_areas();
  ss.str("");

  size_t size = m->getNumEnsembleTuples();
  for (size_t i = 1; i < size; ++i)
  {
    if(m_PhaseTypes[i] == DREAM3D::PhaseType::PrimaryPhase ||  m_PhaseTypes[i] == DREAM3D::PhaseType::PrecipitatePhase)
    {
      ss << "Initializing Arrays of Phase " << i;
      notifyStatusMessage("Initializing Arrays");
      initializeArrays(i);
      ss.str("");

      ss << "Assigning Eulers to Phase " << i;
      notifyStatusMessage(ss.str());
      assign_eulers(i);
      ss.str("");

      ss << "Measuring Misorientations of Phase " << i;
      notifyStatusMessage(ss.str());
      measure_misorientations(i);
      ss.str("");

      ss << "Matching Crystallography of Phase " << i;
      notifyStatusMessage(ss.str());
      matchCrystallography(i);
      ss.str("");
    }
  }

  // If there is an error set this to something negative and also set a message
  notifyStatusMessage("Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MatchCrystallography::initializeArrays(int ensem)
{
 // VoxelDataContainer* m = getVoxelDataContainer();

  StatsDataArray& statsDataArray = *m_StatsDataArray;

 // size_t size = m->getNumEnsembleTuples();

  if(m_PhaseTypes[ensem] == DREAM3D::PhaseType::PrecipitatePhase)
  {
      PrecipitateStatsData* pp = PrecipitateStatsData::SafePointerDownCast(statsDataArray[ensem].get());
      actualodf = pp->getODF();
      actualmdf = pp->getMisorientationBins();
  }
  else if(m_PhaseTypes[ensem] == DREAM3D::PhaseType::PrimaryPhase)
  {
      PrimaryStatsData* pp = PrimaryStatsData::SafePointerDownCast(statsDataArray[ensem].get());
      actualodf = pp->getODF();
      actualmdf = pp->getMisorientationBins();
  }
  else
  {
      setErrorCondition(-55000);
      std::stringstream ss;
      ss << "Improper PhaseType for MatchCrystallography";
      addErrorMessage(getHumanLabel(), ss.str(), getErrorCondition());
      return;
  }

  simodf = FloatArrayType::CreateArray(actualodf->GetSize(), DREAM3D::HDF5::ODF);
  simmdf = FloatArrayType::CreateArray(actualmdf->GetSize(), DREAM3D::HDF5::MisorientationBins);
  for (size_t j = 0; j < simodf->GetSize(); j++)
  {
      simodf->SetValue(j, 0.0);
  }
  for (size_t j = 0; j < simmdf->GetSize(); j++)
  {
      simmdf->SetValue(j, 0.0);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MatchCrystallography::determine_volumes()
{
  VoxelDataContainer* m = getVoxelDataContainer();

  size_t totalPoints = m->getNumCellTuples();
  size_t totalFields = m->getNumFieldTuples();
  size_t totalEnsembles = m->getNumEnsembleTuples();

  unbiasedvol.resize(totalEnsembles);
  for (size_t i = 1; i < totalFields; i++)
  {
    m_Volumes[i] = 0.0;
  }
  for (size_t i = 0; i < totalPoints; i++)
  {
    m_Volumes[m_GrainIds[i]]++;
  }
  float res_scalar = m->getXRes() * m->getYRes() * m->getZRes();
  for (size_t i = 1; i < totalFields; i++)
  {
    m_Volumes[i] = m_Volumes[i] * res_scalar;
    if(m_SurfaceFields[i] == false)
  {
    unbiasedvol[m_FieldPhases[i]] = unbiasedvol[m_FieldPhases[i]] + m_Volumes[i];
  }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MatchCrystallography::determine_boundary_areas()
{
  VoxelDataContainer* m = getVoxelDataContainer();

  NeighborList<int>& neighborlist = *m_NeighborList;
  NeighborList<float>& neighborsurfacearealist = *m_SharedSurfaceAreaList;

  size_t totalFields = m->getNumFieldTuples();
  size_t totalEnsembles = m->getNumEnsembleTuples();

  totalSurfaceArea.resize(totalEnsembles,0.0);

  int phase1, phase2;
  for (size_t i = 1; i < totalFields; i++)
  {
    phase1 = m_FieldPhases[i];
    size_t size = 0;
    if(neighborlist[i].size() != 0 && neighborsurfacearealist[i].size() == neighborlist[i].size())
    {
      size = neighborlist[i].size();
    }

    for (size_t j = 0; j < size; j++)
    {
      int nname = neighborlist[i][j];
      float neighsurfarea = neighborsurfacearealist[i][j];
      phase2 = m_FieldPhases[nname];
      if(phase1 == phase2)
      {
    totalSurfaceArea[phase1] = totalSurfaceArea[phase1] + neighsurfarea;
    }
  }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MatchCrystallography::assign_eulers(int ensem)
{
  DREAM3D_RANDOMNG_NEW()
  VoxelDataContainer* m = getVoxelDataContainer();
  int numbins = 0;
  float totaldensity = 0;
  float synea1 = 0, synea2 = 0, synea3 = 0;
  float q[5];
  float random;
  int choose, phase;

  int totalFields = m->getNumFieldTuples();
  //size_t xtalCount = m->getNumEnsembleTuples();

  //float threshold = 0.0f;

  std::stringstream ss;
  for (int i = 1; i < totalFields; i++)
  {
  phase = m_FieldPhases[i];
  if(phase == ensem)
  {
    random = static_cast<float>( rg.genrand_res53() );
    choose = 0;
    totaldensity = 0;

    if(m_CrystalStructures[phase] == Ebsd::CrystalStructure::Cubic) numbins = 5832;
    if(m_CrystalStructures[phase] == Ebsd::CrystalStructure::Hexagonal) numbins = 15552;

    for (int j = 0; j < numbins; j++)
    {
      float density = actualodf->GetValue(j);
      totaldensity = totaldensity + density;
      if(random >= totaldensity) choose = j;
    }

    m_OrientationOps[m_CrystalStructures[ensem]]->determineEulerAngles(choose, synea1, synea2, synea3);
    m_FieldEulerAngles[3 * i] = synea1;
    m_FieldEulerAngles[3 * i + 1] = synea2;
    m_FieldEulerAngles[3 * i + 2] = synea3;
    OrientationMath::eulertoQuat(q, synea1, synea2, synea3);
    m_AvgQuats[5 * i] = q[0];
    m_AvgQuats[5 * i + 1] = q[1];
    m_AvgQuats[5 * i + 2] = q[2];
    m_AvgQuats[5 * i + 3] = q[3];
    m_AvgQuats[5 * i + 4] = q[4];
    if(m_SurfaceFields[i] == false)
    {
      simodf->SetValue(choose, (simodf->GetValue(choose) + m_Volumes[i]/unbiasedvol[ensem]));
    }
  }
  }
}

void MatchCrystallography::MC_LoopBody1(int grain, int ensem, int j, float neighsurfarea, unsigned int sym, float q1[5], float q2[5])
{
  float w;
  float n1, n2, n3;
  float r1, r2, r3;
  float curmiso1, curmiso2, curmiso3;
  size_t curmisobin, newmisobin;

  curmiso1 = misorientationlists[grain][3 * j];
  curmiso2 = misorientationlists[grain][3 * j + 1];
  curmiso3 = misorientationlists[grain][3 * j + 2];
  curmisobin = m_OrientationOps[sym]->getMisoBin(curmiso1, curmiso2, curmiso3);
  w = m_OrientationOps[sym]->getMisoQuat(q1, q2, n1, n2, n3);
  OrientationMath::axisAngletoRod(w, n1, n2, n3, r1, r2, r3);
  newmisobin = m_OrientationOps[sym]->getMisoBin(n1, n2, n3);
  mdfchange = mdfchange
      + (((actualmdf->GetValue(curmisobin) - simmdf->GetValue(curmisobin)) * (actualmdf->GetValue(curmisobin) - simmdf->GetValue(curmisobin)))
          - ((actualmdf->GetValue(curmisobin) - (simmdf->GetValue(curmisobin) - (neighsurfarea / totalSurfaceArea[ensem])))
              * (actualmdf->GetValue(curmisobin) - (simmdf->GetValue(curmisobin) - (neighsurfarea / totalSurfaceArea[ensem])))));
  mdfchange = mdfchange
      + (((actualmdf->GetValue(newmisobin) - simmdf->GetValue(newmisobin)) * (actualmdf->GetValue(newmisobin) - simmdf->GetValue(newmisobin)))
          - ((actualmdf->GetValue(newmisobin) - (simmdf->GetValue(newmisobin) + (neighsurfarea / totalSurfaceArea[ensem])))
              * (actualmdf->GetValue(newmisobin) - (simmdf->GetValue(newmisobin) + (neighsurfarea / totalSurfaceArea[ensem])))));
}

void MatchCrystallography::MC_LoopBody2(int grain, int ensem, int j, float neighsurfarea, unsigned int sym, float q1[5], float q2[5])
{
  float w;
  float n1, n2, n3;
  float r1, r2, r3;
  float curmiso1, curmiso2, curmiso3;
  size_t curmisobin, newmisobin;
  float miso1 = 0.0f, miso2 = 0.0f, miso3 = 0.0f;

  curmiso1 = misorientationlists[grain][3 * j];
  curmiso2 = misorientationlists[grain][3 * j + 1];
  curmiso3 = misorientationlists[grain][3 * j + 2];
  curmisobin = m_OrientationOps[sym]->getMisoBin(curmiso1, curmiso2, curmiso3);
  w = m_OrientationOps[sym]->getMisoQuat(q1, q2, n1, n2, n3);
  OrientationMath::axisAngletoRod(w, n1, n2, n3, r1, r2, r3);
  newmisobin = m_OrientationOps[sym]->getMisoBin(n1, n2, n3);
  misorientationlists[grain][3 * j] = miso1;
  misorientationlists[grain][3 * j + 1] = miso2;
  misorientationlists[grain][3 * j + 2] = miso3;
  simmdf->SetValue(curmisobin, (simmdf->GetValue(curmisobin) - (neighsurfarea / totalSurfaceArea[ensem])));
  simmdf->SetValue(newmisobin, (simmdf->GetValue(newmisobin) + (neighsurfarea / totalSurfaceArea[ensem])));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MatchCrystallography::matchCrystallography(int ensem)
{
  VoxelDataContainer* m = getVoxelDataContainer();
  // But since a pointer is difficult to use operators with we will now create a
  // reference variable to the pointer with the correct variable name that allows
  // us to use the same syntax as the "vector of vectors"
  NeighborList<int>& neighborlist = *m_NeighborList;
  NeighborList<float>& neighborsurfacearealist = *m_SharedSurfaceAreaList;

  int64_t totalPoints = m->getTotalPoints();
  size_t totalFields = m->getNumFieldTuples();

//  float xRes = m->getXRes();
//  float yRes = m->getYRes();
//  float zRes = m->getZRes();
//  float volResConst = xRes * yRes * zRes;
  DREAM3D_RANDOMNG_NEW()
  int numbins = 0;
  int iterations = 0, badtrycount = 0;
  float random = 0;
  size_t counter = 0;
  float q1[5], q2[5];
  float ea1 = 0, ea2 = 0, ea3 = 0;
  float r1 = 0, r2 = 0, r3 = 0;
  float g1ea1 = 0, g1ea2 = 0, g1ea3 = 0, g2ea1 = 0, g2ea2 = 0, g2ea3 = 0;
  int g1odfbin = 0, g2odfbin = 0;
  float totaldensity = 0, deltaerror = 0;
  float currentodferror = 0, currentmdferror = 0;
  size_t selectedgrain1 = 0, selectedgrain2 = 0;
  //size_t numensembles = m->getNumEnsembleTuples();
  size_t numfields = m->getNumFieldTuples();
  iterations = 0;
  badtrycount = 0;
  if(m_CrystalStructures[ensem] == Ebsd::CrystalStructure::Cubic) numbins = 18 * 18 * 18;
  if(m_CrystalStructures[ensem] == Ebsd::CrystalStructure::Hexagonal) numbins = 36 * 36 * 12;
  while (badtrycount < (m_MaxIterations/10) && iterations < m_MaxIterations)
  {
      std::stringstream ss;
      ss << "Matching Crystallography - Swapping/Switching Orientations - " << ((float)iterations/float(1000*totalFields))*100 << "% Complete";
//      notifyStatusMessage(ss.str());
      currentodferror = 0;
      currentmdferror = 0;
      for (int i = 0; i < numbins; i++)
      {
    currentodferror = currentodferror + ((actualodf->GetValue(i) - simodf->GetValue(i)) * (actualodf->GetValue(i) - simodf->GetValue(i)));
      }
      for (int i = 0; i < (numbins); i++)
      {
        currentmdferror = currentmdferror + ((actualmdf->GetValue(i) - simmdf->GetValue(i)) * (actualmdf->GetValue(i) - simmdf->GetValue(i)));
      }
      iterations++;
      badtrycount++;
      random = static_cast<float>( rg.genrand_res53() );

      if(random < 0.5) // SwapOutOrientation
      {
        counter = 0;
        selectedgrain1 = int(rg.genrand_res53() * numfields);
        while ((m_SurfaceFields[selectedgrain1] == true || m_FieldPhases[selectedgrain1] != static_cast<int32_t>(ensem)) && counter < numfields)
        {
          if(selectedgrain1 >= numfields) selectedgrain1 = selectedgrain1 - numfields;
          selectedgrain1++;
          counter++;
        }
        if(counter == numfields)
        {
          badtrycount = 10*m_NumFields[ensem];
        }
    else
    {
      ea1 = m_FieldEulerAngles[3 * selectedgrain1];
      ea2 = m_FieldEulerAngles[3 * selectedgrain1 + 1];
      ea3 = m_FieldEulerAngles[3 * selectedgrain1 + 2];
      OrientationMath::eulertoRod(r1, r2, r3, ea1, ea2, ea3);
      g1odfbin = m_OrientationOps[m_CrystalStructures[ensem]]->getOdfBin(r1, r2, r3);
      random = static_cast<float>( rg.genrand_res53() );
      int choose = 0;
      totaldensity = 0;
      for (int i = 0; i < numbins; i++)
      {
      float density = actualodf->GetValue(i);
      totaldensity = totaldensity + density;
      if(random >= totaldensity) choose = i;
      }

      m_OrientationOps[m_CrystalStructures[ensem]]->determineEulerAngles(choose, g1ea1, g1ea2, g1ea3);
      OrientationMath::eulertoQuat(q1, g1ea1, g1ea2, g1ea3);

      odfchange = ((actualodf->GetValue(choose) - simodf->GetValue(choose)) * (actualodf->GetValue(choose) - simodf->GetValue(choose)))
      - ((actualodf->GetValue(choose) - (simodf->GetValue(choose) + (m_Volumes[selectedgrain1] / unbiasedvol[ensem])))
        * (actualodf->GetValue(choose) - (simodf->GetValue(choose) + (m_Volumes[selectedgrain1] / unbiasedvol[ensem]))));
      odfchange = odfchange
      + (((actualodf->GetValue(g1odfbin) - simodf->GetValue(g1odfbin)) * (actualodf->GetValue(g1odfbin) - simodf->GetValue(g1odfbin)))
        - ((actualodf->GetValue(g1odfbin) - (simodf->GetValue(g1odfbin) - (m_Volumes[selectedgrain1] / unbiasedvol[ensem])))
        * (actualodf->GetValue(g1odfbin) - (simodf->GetValue(g1odfbin) - (m_Volumes[selectedgrain1] / unbiasedvol[ensem])))));

      mdfchange = 0;
      size_t size = 0;
      if(neighborlist[selectedgrain1].size() != 0) size = neighborlist[selectedgrain1].size();
      for (size_t j = 0; j < size; j++)
      {
      int neighbor = neighborlist[selectedgrain1][j];
      ea1 = m_FieldEulerAngles[3 * neighbor];
      ea2 = m_FieldEulerAngles[3 * neighbor + 1];
      ea3 = m_FieldEulerAngles[3 * neighbor + 2];
      OrientationMath::eulertoQuat(q2, ea1, ea2, ea3);
      float neighsurfarea = neighborsurfacearealist[selectedgrain1][j];
      MC_LoopBody1(selectedgrain1, ensem, j, neighsurfarea, m_CrystalStructures[ensem], q1, q2);
      }

      deltaerror = (odfchange / currentodferror) + (mdfchange / currentmdferror);
      if(deltaerror > 0)
      {
      badtrycount = 0;
      m_FieldEulerAngles[3 * selectedgrain1] = g1ea1;
      m_FieldEulerAngles[3 * selectedgrain1 + 1] = g1ea2;
      m_FieldEulerAngles[3 * selectedgrain1 + 2] = g1ea3;
      m_AvgQuats[5 * selectedgrain1 + 1] = q1[1];
      m_AvgQuats[5 * selectedgrain1 + 2] = q1[2];
      m_AvgQuats[5 * selectedgrain1 + 3] = q1[3];
      m_AvgQuats[5 * selectedgrain1 + 4] = q1[4];
      simodf->SetValue(choose, (simodf->GetValue(choose) + (m_Volumes[selectedgrain1] / unbiasedvol[ensem])));
      simodf->SetValue(g1odfbin, (simodf->GetValue(g1odfbin) - (m_Volumes[selectedgrain1] / unbiasedvol[ensem])));
      size_t size = 0;
      if(neighborlist[selectedgrain1].size() != 0) size = neighborlist[selectedgrain1].size();
      for (size_t j = 0; j < size; j++)
      {
        int neighbor = neighborlist[selectedgrain1][j];
        ea1 = m_FieldEulerAngles[3 * neighbor];
        ea2 = m_FieldEulerAngles[3 * neighbor + 1];
        ea3 = m_FieldEulerAngles[3 * neighbor + 2];
        OrientationMath::eulertoQuat(q2, ea1, ea2, ea3);
        float neighsurfarea = neighborsurfacearealist[selectedgrain1][j];
        MC_LoopBody2(selectedgrain1, ensem, j, neighsurfarea, m_CrystalStructures[ensem], q1, q2);
      }
      }
    }
      }
      else // SwitchOrientation
      {
        counter = 0;
        selectedgrain1 = int(rg.genrand_res53() * numfields);
        while ((m_SurfaceFields[selectedgrain1] == true || m_FieldPhases[selectedgrain1] != static_cast<int32_t>(ensem)) && counter < numfields)
        {
          if(selectedgrain1 >= numfields) selectedgrain1 = selectedgrain1 - numfields;
          selectedgrain1++;
          counter++;
        }
        if(counter == numfields)
    {
      badtrycount = 10*m_NumFields[ensem];
    }
    else
    {
        counter = 0;
        selectedgrain2 = int(rg.genrand_res53() * numfields);
        while ((m_SurfaceFields[selectedgrain2] == true || m_FieldPhases[selectedgrain2] != static_cast<int32_t>(ensem) || selectedgrain2 == selectedgrain1) && counter < numfields)
          {
            if(selectedgrain2 >= numfields) selectedgrain2 = selectedgrain2 - numfields;
            selectedgrain2++;
            counter++;
          }
          if(counter == numfields)
      {
      badtrycount = 10*m_NumFields[ensem];
      }
      else
      {
      g1ea1 = m_FieldEulerAngles[3 * selectedgrain1];
      g1ea2 = m_FieldEulerAngles[3 * selectedgrain1 + 1];
      g1ea3 = m_FieldEulerAngles[3 * selectedgrain1 + 2];
      g2ea1 = m_FieldEulerAngles[3 * selectedgrain2];
      g2ea2 = m_FieldEulerAngles[3 * selectedgrain2 + 1];
      g2ea3 = m_FieldEulerAngles[3 * selectedgrain2 + 2];
      q1[1] = m_AvgQuats[5 * selectedgrain1 + 1];
      q1[2] = m_AvgQuats[5 * selectedgrain1 + 2];
      q1[3] = m_AvgQuats[5 * selectedgrain1 + 3];
      q1[4] = m_AvgQuats[5 * selectedgrain1 + 4];
      OrientationMath::eulertoRod(r1, r2, r3, g1ea1, g1ea2, g1ea3);
      g1odfbin = m_OrientationOps[m_CrystalStructures[ensem]]->getOdfBin(r1, r2, r3);
      q1[1] = m_AvgQuats[5 * selectedgrain2 + 1];
      q1[2] = m_AvgQuats[5 * selectedgrain2 + 2];
      q1[3] = m_AvgQuats[5 * selectedgrain2 + 3];
      q1[4] = m_AvgQuats[5 * selectedgrain2 + 4];
      OrientationMath::eulertoRod(r1, r2, r3, g2ea1, g2ea2, g2ea3);
      g2odfbin = m_OrientationOps[m_CrystalStructures[ensem]]->getOdfBin(r1, r2, r3);

      odfchange = ((actualodf->GetValue(g1odfbin) - simodf->GetValue(g1odfbin)) * (actualodf->GetValue(g1odfbin) - simodf->GetValue(g1odfbin)))
        - ((actualodf->GetValue(g1odfbin)
        - (simodf->GetValue(g1odfbin) - (m_Volumes[selectedgrain1] / unbiasedvol[ensem])
          + (m_Volumes[selectedgrain2] / unbiasedvol[ensem])))
        * (actualodf->GetValue(g1odfbin)
          - (simodf->GetValue(g1odfbin) - (m_Volumes[selectedgrain1] / unbiasedvol[ensem])
          + (m_Volumes[selectedgrain2] / unbiasedvol[ensem]))));
      odfchange = odfchange
        + (((actualodf->GetValue(g2odfbin) - simodf->GetValue(g2odfbin)) * (actualodf->GetValue(g2odfbin) - simodf->GetValue(g2odfbin)))
        - ((actualodf->GetValue(g2odfbin)
          - (simodf->GetValue(g2odfbin) - (m_Volumes[selectedgrain2] / unbiasedvol[ensem])
          + (m_Volumes[selectedgrain1] / unbiasedvol[ensem])))
          * (actualodf->GetValue(g2odfbin)
          - (simodf->GetValue(g2odfbin) - (m_Volumes[selectedgrain2] / unbiasedvol[ensem])
            + (m_Volumes[selectedgrain1] / unbiasedvol[ensem])))));

      mdfchange = 0;
      OrientationMath::eulertoQuat(q1, g2ea1, g2ea2, g2ea3);
      size_t size = 0;
      if(neighborlist[selectedgrain1].size() != 0) size = neighborlist[selectedgrain1].size();
      for (size_t j = 0; j < size; j++)
      {
        int neighbor = neighborlist[selectedgrain1][j];
        ea1 = m_FieldEulerAngles[3 * neighbor];
        ea2 = m_FieldEulerAngles[3 * neighbor + 1];
        ea3 = m_FieldEulerAngles[3 * neighbor + 2];
        OrientationMath::eulertoQuat(q2, ea1, ea2, ea3);
        float neighsurfarea = neighborsurfacearealist[selectedgrain1][j];
        if(neighbor != static_cast<int>(selectedgrain2) )
        {
        MC_LoopBody1(selectedgrain1, ensem, j, neighsurfarea, m_CrystalStructures[ensem], q1, q2);
        }
      }

      OrientationMath::eulertoQuat(q1, g1ea1, g1ea2, g1ea3);
      size = 0;
      if(neighborlist[selectedgrain2].size() != 0) size = neighborlist[selectedgrain2].size();
      for (size_t j = 0; j < size; j++)
      {
        size_t neighbor = neighborlist[selectedgrain2][j];
        ea1 = m_FieldEulerAngles[3 * neighbor];
        ea2 = m_FieldEulerAngles[3 * neighbor + 1];
        ea3 = m_FieldEulerAngles[3 * neighbor + 2];
        OrientationMath::eulertoQuat(q2, ea1, ea2, ea3);
        float neighsurfarea = neighborsurfacearealist[selectedgrain2][j];
        if(neighbor != selectedgrain1)
        {
        MC_LoopBody1(selectedgrain2, ensem, j, neighsurfarea, m_CrystalStructures[ensem], q1, q2);
        }
      }

      deltaerror = (odfchange / currentodferror) + (mdfchange / currentmdferror);
      if(deltaerror > 0)
      {
        badtrycount = 0;
        m_FieldEulerAngles[3 * selectedgrain1] = g2ea1;
        m_FieldEulerAngles[3 * selectedgrain1 + 1] = g2ea2;
        m_FieldEulerAngles[3 * selectedgrain1 + 2] = g2ea3;
        m_FieldEulerAngles[3 * selectedgrain2] = g1ea1;
        m_FieldEulerAngles[3 * selectedgrain2 + 1] = g1ea2;
        m_FieldEulerAngles[3 * selectedgrain2 + 2] = g1ea3;
        simodf->SetValue(g1odfbin, (simodf->GetValue(g1odfbin) + (m_Volumes[selectedgrain2] / unbiasedvol[ensem])
        - (m_Volumes[selectedgrain1] / unbiasedvol[ensem])));
        simodf->SetValue(g2odfbin, (simodf->GetValue(g2odfbin) + (m_Volumes[selectedgrain1] / unbiasedvol[ensem])
        - (m_Volumes[selectedgrain2] / unbiasedvol[ensem])));

        OrientationMath::eulertoQuat(q1, g2ea1, g2ea2, g2ea3);
        m_AvgQuats[5 * selectedgrain1 + 1] = q1[1];
        m_AvgQuats[5 * selectedgrain1 + 2] = q1[2];
        m_AvgQuats[5 * selectedgrain1 + 3] = q1[3];
        m_AvgQuats[5 * selectedgrain1 + 4] = q1[4];
        size = 0;
        if(neighborlist[selectedgrain1].size() != 0) size = neighborlist[selectedgrain1].size();
        for (size_t j = 0; j < size; j++)
        {
          size_t neighbor = neighborlist[selectedgrain1][j];
          ea1 = m_FieldEulerAngles[3 * neighbor];
          ea2 = m_FieldEulerAngles[3 * neighbor + 1];
          ea3 = m_FieldEulerAngles[3 * neighbor + 2];
          OrientationMath::eulertoQuat(q2, ea1, ea2, ea3);
          float neighsurfarea = neighborsurfacearealist[selectedgrain1][j];
          if(neighbor != selectedgrain2)
          {
          MC_LoopBody2(selectedgrain1, ensem, j, neighsurfarea, m_CrystalStructures[ensem], q1, q2);
          }
        }

        OrientationMath::eulertoQuat(q1, g1ea1, g1ea2, g1ea3);
        m_AvgQuats[5 * selectedgrain2 + 1] = q1[1];
        m_AvgQuats[5 * selectedgrain2 + 2] = q1[2];
        m_AvgQuats[5 * selectedgrain2 + 3] = q1[3];
        m_AvgQuats[5 * selectedgrain2 + 4] = q1[4];
        size = 0;
        if(neighborlist[selectedgrain2].size() != 0) size = neighborlist[selectedgrain2].size();
        for (size_t j = 0; j < size; j++)
        {
          size_t neighbor = neighborlist[selectedgrain2][j];
          ea1 = m_FieldEulerAngles[3 * neighbor];
          ea2 = m_FieldEulerAngles[3 * neighbor + 1];
          ea3 = m_FieldEulerAngles[3 * neighbor + 2];
          OrientationMath::eulertoQuat(q2, ea1, ea2, ea3);
          float neighsurfarea = neighborsurfacearealist[selectedgrain2][j];
          if(neighbor != selectedgrain1)
          {
          MC_LoopBody2(selectedgrain2, ensem, j, neighsurfarea, m_CrystalStructures[ensem], q1, q2);
          }
        }
      }
      }
      }
    }
  }
  for (int i = 0; i < totalPoints; i++)
  {
    m_CellEulerAngles[3 * i] = m_FieldEulerAngles[3 * m_GrainIds[i]];
    m_CellEulerAngles[3 * i + 1] = m_FieldEulerAngles[3 * m_GrainIds[i] + 1];
    m_CellEulerAngles[3 * i + 2] = m_FieldEulerAngles[3 * m_GrainIds[i] + 2];
  }
}

void MatchCrystallography::measure_misorientations(int ensem)
{
  VoxelDataContainer* m = getVoxelDataContainer();
  // But since a pointer is difficult to use operators with we will now create a
  // reference variable to the pointer with the correct variable name that allows
  // us to use the same syntax as the "vector of vectors"
  NeighborList<int>& neighborlist = *m_NeighborList;
  NeighborList<float>& neighborsurfacearealist = *m_SharedSurfaceAreaList;

  float w;
  float n1 = 0.0f, n2 = 0.0f, n3 = 0.0f;
  float r1 = 0.0f, r2 = 0.0f, r3 = 0.0f;
  float q1[5];
  float q2[5];
  unsigned int crys1;
  int mbin = 0;
  size_t totalFields = m->getNumFieldTuples();
  //float threshold = 0.0f;

  misorientationlists.resize(totalFields);
  std::stringstream ss;
  for (size_t i = 1; i < totalFields; i++)
  {
  if(m_FieldPhases[i] == ensem)
  {
    if(misorientationlists[i].size() != 0)
    {
      misorientationlists[i].clear();
    }
    if(neighborlist[i].size() != 0)
    {
      misorientationlists[i].resize(neighborlist[i].size() * 3, 0.0);
    }

    q1[1] = m_AvgQuats[5 * i + 1];
    q1[2] = m_AvgQuats[5 * i + 2];
    q1[3] = m_AvgQuats[5 * i + 3];
    q1[4] = m_AvgQuats[5 * i + 4];
    crys1 = m_CrystalStructures[ensem];
    size_t size = 0;
    if(neighborlist[i].size() != 0 && neighborsurfacearealist[i].size() == neighborlist[i].size())
    {
      size = neighborlist[i].size();
    }

    for (size_t j = 0; j < size; j++)
    {
      int nname = neighborlist[i][j];
      if(m_FieldPhases[nname] == ensem)
      {
        w = 10000.0;
        float neighsurfarea = neighborsurfacearealist[i][j];
        q2[1] = m_AvgQuats[5 * nname + 1];
        q2[2] = m_AvgQuats[5 * nname + 2];
        q2[3] = m_AvgQuats[5 * nname + 3];
        q2[4] = m_AvgQuats[5 * nname + 4];
        w = m_OrientationOps[crys1]->getMisoQuat(q1, q2, n1, n2, n3);
        OrientationMath::axisAngletoRod(w, n1, n2, n3, r1, r2, r3);
        misorientationlists[i][3 * j] = r1;
        misorientationlists[i][3 * j + 1] = r2;
        misorientationlists[i][3 * j + 2] = r3;
        mbin = m_OrientationOps[crys1]->getMisoBin(misorientationlists[i][3 * j], misorientationlists[i][3 * j + 1], misorientationlists[i][3 * j + 2]);
        if(m_SurfaceFields[i] == false && (nname > static_cast<int>(i) || m_SurfaceFields[nname] == true))
        {
        simmdf->SetValue(mbin, (simmdf->GetValue(mbin)+(neighsurfarea/totalSurfaceArea[m_FieldPhases[i]])));
        }
      }
      else
      {
        misorientationlists[i][3 * j] = -100;
        misorientationlists[i][3 * j + 1] = -100;
        misorientationlists[i][3 * j + 2] = -100;
      }
    }
  }
  }
}
