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
  m_PhaseTypesArrayName(DREAM3D::EnsembleData::PhaseTypes),
  m_PhaseTypes(NULL),
  m_VolumesArrayName(DREAM3D::FeatureData::Volumes),
  m_Volumes(NULL),
  m_FeatureEulerAnglesArrayName(DREAM3D::FeatureData::EulerAngles),
  m_FeatureEulerAngles(NULL),
  m_FeaturePhasesArrayName(DREAM3D::FeatureData::Phases),
  m_FeaturePhases(NULL),
  m_SurfaceFeaturesArrayName(DREAM3D::FeatureData::SurfaceFeatures),
  m_SurfaceFeatures(NULL),
  m_CrystalStructuresArrayName(DREAM3D::EnsembleData::CrystalStructures),
  m_CrystalStructures(NULL)
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
void FindODF::dataCheck()
{
  setErrorCondition(0);

  VoxelDataContainer* m = getVoxelDataContainer();
  int err = 0;

  TEST_PREREQ_DATA(m, DREAM3D, FeatureData, FeaturePhases, err, -301, int32_t, Int32ArrayType, features, 1)
  if(err == -301)
  {
    setErrorCondition(0);
    FindGrainPhases::Pointer find_grainphases = FindGrainPhases::New();
    find_grainphases->setObservers(this->getObservers());
    find_grainphases->setVoxelDataContainer(getVoxelDataContainer());
    if(preflight == true) { find_grainphases->preflight(); }
    if(preflight == false) { find_grainphases->execute(); }
  }
  m_FeaturePhasesPtr = cellFeatureAttrMat->getPrereqArray<DataArray<int32_t>, AbstractFilter>(this, m_FeaturePhasesArrayName, -301, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeaturePhasesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeaturePhases = m_FeaturePhasesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  m_FeatureEulerAnglesPtr = cellFeatureAttrMat->getPrereqArray<DataArray<float>, AbstractFilter>(this, m_FeatureEulerAnglesArrayName, -302, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeatureEulerAnglesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeatureEulerAngles = m_FeatureEulerAnglesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  TEST_PREREQ_DATA(m, DREAM3D, FeatureData, SurfaceFeatures, err, -303, bool, BoolArrayType, features, 1)
  if(err == -303)
  {
    setErrorCondition(0);
    FindSurfaceGrains::Pointer find_surfacefeatures = FindSurfaceGrains::New();
    find_surfacefeatures->setObservers(this->getObservers());
    find_surfacefeatures->setVoxelDataContainer(getVoxelDataContainer());
    if(preflight == true) { find_surfacefeatures->preflight(); }
    if(preflight == false) { find_surfacefeatures->execute(); }
  }
  m_SurfaceFeaturesPtr = cellFeatureAttrMat->getPrereqArray<DataArray<bool>, AbstractFilter>(this, m_SurfaceFeaturesArrayName, -303, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_SurfaceFeaturesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_SurfaceFeatures = m_SurfaceFeaturesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */


  m_VolumesPtr = cellFeatureAttrMat->getPrereqArray<DataArray<float>, AbstractFilter>(this, m_VolumesArrayName, -304, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_VolumesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_Volumes = m_VolumesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  typedef DataArray<unsigned int> XTalStructArrayType;
  m_CrystalStructuresPtr = attrMat->getPrereqArray<DataArray<unsigned int>, AbstractFilter>(this, m_EnsembleAttributeMatrixName,  m_CrystalStructuresArrayName, -305, ensembles, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CrystalStructuresPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_CrystalStructures = m_CrystalStructuresPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  typedef DataArray<unsigned int> PhaseTypeArrayType;
  m_PhaseTypesPtr = attrMat->getPrereqArray<DataArray<unsigned int>, AbstractFilter>(this, m_EnsembleAttributeMatrixName,  m_PhaseTypesArrayName, -307, ensembles, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_PhaseTypesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_PhaseTypes = m_PhaseTypesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
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
  dataCheck();
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
  dataCheck(false, m->getTotalPoints(), m->getNumFeatureTuples(), m->getNumEnsembleTuples());
  if (getErrorCondition() < 0)
  {
    return;
  }

  StatsDataArray& statsDataArray = *m_StatsDataArray;

  size_t bin;
  size_t numgrains = m->getNumFeatureTuples();
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
      eulerodf[i] = FloatArrayType::CreateArray(dims, DREAM3D::StringConstants::ODF);
      for (unsigned long long j = 0; j < dims; j++)
      {
        eulerodf[i]->SetValue(j, 0.0);
      }
    }
    else if (m_CrystalStructures[i] == Ebsd::CrystalStructure::Cubic)
    {
      dims = 18 * 18 * 18;
      eulerodf[i] = FloatArrayType::CreateArray(dims, DREAM3D::StringConstants::ODF);
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
    if (m_SurfaceFeatures[i] == false)
    {
      totalvol[m_FeaturePhases[i]] = totalvol[m_FeaturePhases[i]] + m_Volumes[i];
    }
  }
  for (size_t i = 1; i < numgrains; i++)
  {
    if (m_SurfaceFeatures[i] == false)
    {
      ea1 = m_FeatureEulerAngles[3 * i];
      ea2 = m_FeatureEulerAngles[3 * i + 1];
      ea3 = m_FeatureEulerAngles[3 * i + 2];
      phase = m_CrystalStructures[m_FeaturePhases[i]];
      OrientationMath::eulertoRod(r1, r2, r3, ea1, ea2, ea3);
      bin = m_OrientationOps[phase]->getOdfBin(r1, r2, r3);
      eulerodf[m_FeaturePhases[i]]->SetValue(bin, (eulerodf[m_FeaturePhases[i]]->GetValue(bin) + (m_Volumes[i] / totalvol[m_FeaturePhases[i]])));
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

