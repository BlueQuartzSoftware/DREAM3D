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

#include "FindODF.h"

#include "DREAM3DLib/Math/DREAM3DMath.h"
#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/GenericFilters/FindSurfaceGrains.h"
#include "DREAM3DLib/GenericFilters/FindGrainPhases.h"
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindODF::FindODF()  :
  AbstractFilter(),
  m_FieldEulerAnglesArrayName(DREAM3D::FieldData::EulerAngles),
  m_FieldPhasesArrayName(DREAM3D::FieldData::Phases),
  m_SurfaceFieldsArrayName(DREAM3D::FieldData::SurfaceFields),
  m_VolumesArrayName(DREAM3D::FieldData::Volumes),
  m_CrystalStructuresArrayName(DREAM3D::EnsembleData::CrystalStructures),
  m_PhaseTypesArrayName(DREAM3D::EnsembleData::PhaseTypes),
  m_PhaseTypes(NULL),
  m_Volumes(NULL),
  m_FieldEulerAngles(NULL),
  m_FieldPhases(NULL),
  m_SurfaceFields(NULL),
  m_CrystalStructures(NULL),
  m_StatsDataArray(NULL)
{
  m_HexOps = HexagonalOps::New();
  m_OrientationOps.push_back(dynamic_cast<OrientationMath*> (m_HexOps.get()));

  m_CubicOps = CubicOps::New();
  m_OrientationOps.push_back(dynamic_cast<OrientationMath*> (m_CubicOps.get()));

  m_OrthoOps = OrthoRhombicOps::New();
  m_OrientationOps.push_back(dynamic_cast<OrientationMath*> (m_OrthoOps.get()));
  setupFilterParameters();

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindODF::~FindODF()
{
}
// -----------------------------------------------------------------------------
void FindODF::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  /* Code to read the values goes between these statements */
////!!##
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int FindODF::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->closeFilterGroup();
  return index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindODF::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);

  VoxelDataContainer* m = getVoxelDataContainer();
  int err = 0;

  TEST_PREREQ_DATA(m, DREAM3D, FieldData, FieldPhases, err, -301, int32_t, Int32ArrayType, fields, 1)
  if(err == -301)
  {
    setErrorCondition(0);
    FindGrainPhases::Pointer find_grainphases = FindGrainPhases::New();
    find_grainphases->setObservers(this->getObservers());
    find_grainphases->setVoxelDataContainer(getVoxelDataContainer());
    if(preflight == true) { find_grainphases->preflight(); }
    if(preflight == false) { find_grainphases->execute(); }
  }
  GET_PREREQ_DATA(m, DREAM3D, FieldData, FieldPhases, -301, int32_t, Int32ArrayType, fields, 1)

  GET_PREREQ_DATA(m, DREAM3D, FieldData, FieldEulerAngles, -302, float, FloatArrayType, fields, 3)

  TEST_PREREQ_DATA(m, DREAM3D, FieldData, SurfaceFields, err, -303, bool, BoolArrayType, fields, 1)
  if(err == -303)
  {
    setErrorCondition(0);
    FindSurfaceGrains::Pointer find_surfacefields = FindSurfaceGrains::New();
    find_surfacefields->setObservers(this->getObservers());
    find_surfacefields->setVoxelDataContainer(getVoxelDataContainer());
    if(preflight == true) { find_surfacefields->preflight(); }
    if(preflight == false) { find_surfacefields->execute(); }
  }
  GET_PREREQ_DATA(m, DREAM3D, FieldData, SurfaceFields, -303, bool, BoolArrayType, fields, 1)


  GET_PREREQ_DATA(m, DREAM3D, FieldData, Volumes, -304, float, FloatArrayType, fields, 1)

  typedef DataArray<unsigned int> XTalStructArrayType;
  GET_PREREQ_DATA(m, DREAM3D, EnsembleData, CrystalStructures, -305, unsigned int, XTalStructArrayType, ensembles, 1)
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
void FindODF::preflight()
{
  dataCheck(true, 1, 1, 1);
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindODF::execute()
{
  VoxelDataContainer* m = getVoxelDataContainer();
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The DataContainer Object was NULL", -999);
    return;
  }
  setErrorCondition(0);

  //int64_t totalPoints = m->totalPoints();
  dataCheck(false, m->getTotalPoints(), m->getNumFieldTuples(), m->getNumEnsembleTuples());
  if (getErrorCondition() < 0)
  {
    return;
  }

  StatsDataArray& statsDataArray = *m_StatsDataArray;

  size_t bin;
  size_t numgrains = m->getNumFieldTuples();
  int phase;
  QVector<float> totalvol;
  QVector<FloatArrayType::Pointer> eulerodf;

  size_t numensembles = m->getNumEnsembleTuples();

  totalvol.resize(numensembles);
  eulerodf.resize(numensembles);
  unsigned long long dims = 0;
  for(unsigned long long i = 1; i < numensembles; i++)
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
      ea1 = m_FieldEulerAngles[3 * i];
      ea2 = m_FieldEulerAngles[3 * i + 1];
      ea3 = m_FieldEulerAngles[3 * i + 2];
      phase = m_CrystalStructures[m_FieldPhases[i]];
      OrientationMath::eulertoRod(r1, r2, r3, ea1, ea2, ea3);
      bin = m_OrientationOps[phase]->getOdfBin(r1, r2, r3);
      eulerodf[m_FieldPhases[i]]->SetValue(bin, (eulerodf[m_FieldPhases[i]]->GetValue(bin) + (m_Volumes[i] / totalvol[m_FieldPhases[i]])));
    }
  }
//  int err;

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

  notifyStatusMessage("FindODF Completed");
}
