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

#include "FindMDF.h"

#include "DREAM3DLib/Math/DREAM3DMath.h"
#include "DREAM3DLib/Common/Constants.h"

#include "DREAM3DLib/GenericFilters/FindNeighbors.h"
#include "DREAM3DLib/GenericFilters/FindSurfaceGrains.h"
#include "DREAM3DLib/GenericFilters/FindGrainPhases.h"
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindMDF::FindMDF()  :
  AbstractFilter(),
  m_AvgQuatsArrayName(DREAM3D::FieldData::AvgQuats),
  m_FieldPhasesArrayName(DREAM3D::FieldData::Phases),
  m_SurfaceFieldsArrayName(DREAM3D::FieldData::SurfaceFields),
  m_CrystalStructuresArrayName(DREAM3D::EnsembleData::CrystalStructures),
  m_TotalSurfaceAreasArrayName(DREAM3D::EnsembleData::TotalSurfaceAreas),
  m_PhaseTypesArrayName(DREAM3D::EnsembleData::PhaseTypes),
  m_AvgQuats(NULL),
  m_SurfaceFields(NULL),
  m_FieldPhases(NULL),
  m_TotalSurfaceAreas(NULL),
  m_NeighborList(NULL),
  m_SharedSurfaceAreaList(NULL),
  m_CrystalStructures(NULL),
  m_PhaseTypes(NULL)
{
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
FindMDF::~FindMDF()
{
}
// -----------------------------------------------------------------------------
void FindMDF::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  /* Code to read the values goes between these statements */
////!!##
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int FindMDF::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->closeFilterGroup();
  return index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindMDF::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;
  VoxelDataContainer* m = getVoxelDataContainer();
  int err = 0;
  GET_PREREQ_DATA(m, DREAM3D, FieldData, AvgQuats, -301, float, FloatArrayType, fields, 4)
  TEST_PREREQ_DATA(m, DREAM3D, FieldData, SurfaceFields, err, -302, bool, BoolArrayType, fields, 1)
  if(err == -302)
  {
    setErrorCondition(0);
    FindSurfaceGrains::Pointer find_surfacefields = FindSurfaceGrains::New();
    find_surfacefields->setObservers(this->getObservers());
    find_surfacefields->setVoxelDataContainer(getVoxelDataContainer());
    if(preflight == true) { find_surfacefields->preflight(); }
    if(preflight == false) { find_surfacefields->execute(); }
  }
  GET_PREREQ_DATA(m, DREAM3D, FieldData, SurfaceFields, -302, bool, BoolArrayType, fields, 1)


  TEST_PREREQ_DATA(m, DREAM3D, FieldData, FieldPhases, err, -303,  int32_t, Int32ArrayType, fields, 1)
  if(err == -303)
  {
    setErrorCondition(0);
    FindGrainPhases::Pointer find_grainphases = FindGrainPhases::New();
    find_grainphases->setObservers(this->getObservers());
    find_grainphases->setVoxelDataContainer(getVoxelDataContainer());
    if(preflight == true) { find_grainphases->preflight(); }
    if(preflight == false) { find_grainphases->execute(); }
  }
  GET_PREREQ_DATA(m, DREAM3D, FieldData, FieldPhases, -303, int32_t, Int32ArrayType, fields, 1)


  // Now we are going to get a "Pointer" to the NeighborList object out of the DataContainer
  m_NeighborList = NeighborList<int>::SafeObjectDownCast<IDataArray*, NeighborList<int>*>(m->getFieldData(DREAM3D::FieldData::NeighborList).get());
  if(m_NeighborList == NULL)
  {
    setErrorCondition(0);
    FindNeighbors::Pointer find_neighbors = FindNeighbors::New();
    find_neighbors->setObservers(this->getObservers());
    find_neighbors->setVoxelDataContainer(getVoxelDataContainer());
    if(preflight == true) { find_neighbors->preflight(); }
    if(preflight == false) { find_neighbors->execute(); }
    m_NeighborList = NeighborList<int>::SafeObjectDownCast<IDataArray*, NeighborList<int>*>(m->getFieldData(DREAM3D::FieldData::NeighborList).get());
    if(m_NeighborList == NULL)
    {
      ss.str("");
      ss << "NeighborLists Array Not Initialized correctly" << std::endl;
      setErrorCondition(-305);
      addErrorMessage(getHumanLabel(), ss.str(), -305);
    }
  }
  m_SharedSurfaceAreaList = NeighborList<float>::SafeObjectDownCast<IDataArray*, NeighborList<float>*>(m->getFieldData(DREAM3D::FieldData::SharedSurfaceAreaList).get());
  if(m_SharedSurfaceAreaList == NULL)
  {
    setErrorCondition(0);
    FindNeighbors::Pointer find_neighbors = FindNeighbors::New();
    find_neighbors->setObservers(this->getObservers());
    find_neighbors->setVoxelDataContainer(getVoxelDataContainer());
    if(preflight == true) { find_neighbors->preflight(); }
    if(preflight == false) { find_neighbors->execute(); }
    m_SharedSurfaceAreaList = NeighborList<float>::SafeObjectDownCast<IDataArray*, NeighborList<float>*>(m->getFieldData(DREAM3D::FieldData::SharedSurfaceAreaList).get());
    if(m_SharedSurfaceAreaList == NULL)
    {
      ss.str("");
      ss << "SurfaceAreaLists Array Not Initialized correctly" << std::endl;
      setErrorCondition(-306);
      addErrorMessage(getHumanLabel(), ss.str(), -306);
    }
  }

  typedef DataArray<unsigned int> XTalStructArrayType;
  GET_PREREQ_DATA(m, DREAM3D, EnsembleData, CrystalStructures, -305, unsigned int, XTalStructArrayType, ensembles, 1)
  GET_PREREQ_DATA(m, DREAM3D, EnsembleData, TotalSurfaceAreas, -303,  float, FloatArrayType, ensembles, 1)
  typedef DataArray<unsigned int> PhaseTypeArrayType;
  GET_PREREQ_DATA(m, DREAM3D, EnsembleData, PhaseTypes, -307, unsigned int, PhaseTypeArrayType, ensembles, 1)
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
void FindMDF::preflight()
{
  dataCheck(true, 1, 1, 1);
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindMDF::execute()
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
  float q1[5];
  float q2[5];
  size_t numgrains = m->getNumFieldTuples();
  unsigned int phase1, phase2;
  std::vector<FloatArrayType::Pointer> misobin;
  int numbins = 0;

  size_t numensembles = m->getNumEnsembleTuples();

  misobin.resize(numensembles);
  for(size_t i = 1; i < numensembles; ++i)
  {
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
    q1[0] = m_AvgQuats[5 * i];
    q1[1] = m_AvgQuats[5 * i + 1];
    q1[2] = m_AvgQuats[5 * i + 2];
    q1[3] = m_AvgQuats[5 * i + 3];
    q1[4] = m_AvgQuats[5 * i + 4];
    phase1 = m_CrystalStructures[m_FieldPhases[i]];
    for (size_t j = 0; j < neighborlist[i].size(); j++)
    {
      w = 10000.0;
      nname = neighborlist[i][j];
      q2[0] = m_AvgQuats[5 * nname];
      q2[1] = m_AvgQuats[5 * nname + 1];
      q2[2] = m_AvgQuats[5 * nname + 2];
      q2[3] = m_AvgQuats[5 * nname + 3];
      q2[4] = m_AvgQuats[5 * nname + 4];
      phase2 = m_CrystalStructures[m_FieldPhases[nname]];
      if (phase1 == phase2) { w = m_OrientationOps[phase1]->getMisoQuat( q1, q2, n1, n2, n3); }
      if (phase1 == phase2)
      {
        OrientationMath::AxisAngletoRod(w, n1, n2, n3, r1, r2, r3);
        if ((nname > i || m_SurfaceFields[nname] == true))
        {
          mbin = m_OrientationOps[phase1]->getMisoBin(r1, r2, r3);
          nsa = neighborsurfacearealist[i][j];
          misobin[m_FieldPhases[i]]->SetValue(mbin, (misobin[m_FieldPhases[i]]->GetValue(mbin) + (nsa / m_TotalSurfaceAreas[m_FieldPhases[i]])));
        }
      }
    }
  }

  // We do this to create new set of MisorientationList objects
  dataCheck(false, m->getNumCellTuples(), m->getNumFieldTuples(), m->getNumEnsembleTuples());

// unsigned long long int dims = static_cast<unsigned long long int>(numbins);
  for (size_t i = 1; i < numensembles; i++)
  {
    if(m_PhaseTypes[i] == DREAM3D::PhaseType::PrimaryPhase)
    {
      PrimaryStatsData* pp = PrimaryStatsData::SafePointerDownCast(statsDataArray[i].get());
      pp->setMisorientationBins(misobin[i]);
    }
    if(m_PhaseTypes[i] == DREAM3D::PhaseType::PrecipitatePhase)
    {
      PrecipitateStatsData* pp = PrecipitateStatsData::SafePointerDownCast(statsDataArray[i].get());
      pp->setMisorientationBins(misobin[i]);
    }
    if(m_PhaseTypes[i] == DREAM3D::PhaseType::TransformationPhase)
    {
      TransformationStatsData* tp = TransformationStatsData::SafePointerDownCast(statsDataArray[i].get());
      tp->setMisorientationBins(misobin[i]);
    }
  }

  notifyStatusMessage("FindMDF Completed");
}
