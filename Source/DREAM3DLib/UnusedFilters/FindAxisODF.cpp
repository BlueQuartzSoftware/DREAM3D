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

#include "FindAxisODF.h"

#include "DREAM3DLib/Math/DREAM3DMath.h"
#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/StatisticsFilters/FindShapes.h"
#include "DREAM3DLib/GenericFilters/FindSurfaceGrains.h"
#include "DREAM3DLib/GenericFilters/FindGrainPhases.h"

const static float m_pi = static_cast<float>(M_PI);

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindAxisODF::FindAxisODF() :
  AbstractFilter(),
  m_AxisEulerAnglesArrayName(DREAM3D::FeatureData::AxisEulerAngles),
  m_FeaturePhasesArrayName(DREAM3D::FeatureData::Phases),
  m_SurfaceFeaturesArrayName(DREAM3D::FeatureData::SurfaceFeatures),
  m_PhaseTypesArrayName(DREAM3D::EnsembleData::PhaseTypes),
  m_SurfaceFeatures(NULL),
  m_FeaturePhases(NULL),
  m_AxisEulerAngles(NULL),
  m_PhaseTypes(NULL),
  m_SurfaceFeaturesArrayName(),
  m_SurfaceFeatures(NULL),
  m_FeaturePhasesArrayName(DREAM3D::FeatureData::FeaturePhases),
  m_FeaturePhases(NULL),
  m_AxisEulerAnglesArrayName(DREAM3D::FeatureData::EulerAngles),
  m_AxisEulerAngles(NULL),
  m_PhaseTypesArrayName(DREAM3D::FeatureData::Phases),
  m_PhaseTypes(NULL)
{
  m_HexOps = HexagonalOps::New();
  m_OrientationOps.push_back(dynamic_cast<OrientationMath*>(m_HexOps.get()));

  m_CubicOps = CubicOps::New();
  m_OrientationOps.push_back(dynamic_cast<OrientationMath*>(m_CubicOps.get()));

  m_OrthoOps = OrthoRhombicOps::New();
  m_OrientationOps.push_back(dynamic_cast<OrientationMath*>(m_OrthoOps.get()));
  setupFilterParameters();
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindAxisODF::~FindAxisODF()
{
}
// -----------------------------------------------------------------------------
void FindAxisODF::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  /* Code to read the values goes between these statements */
////!!##
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int FindAxisODF::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->closeFilterGroup();
  return index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindAxisODF::dataCheck()
{
  setErrorCondition(0);

  VoxelDataContainer* m = getVoxelDataContainer();
  int err = 0;

  m_AxisEulerAnglesPtr = cellFeatureAttrMat->getPrereqArray<DataArray<float>, AbstractFilter>(this, m_AxisEulerAnglesArrayName, -301, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_AxisEulerAnglesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_AxisEulerAngles = m_AxisEulerAnglesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  TEST_PREREQ_DATA(m, DREAM3D, FeatureData, SurfaceFeatures, err, -302, bool, BoolArrayType, features, 1)
  if(err == -302)
  {
    setErrorCondition(0);
    FindSurfaceGrains::Pointer find_surfacefeatures = FindSurfaceGrains::New();
    connect(find_surfacefeatures, SIGNAL(filterGeneratedMessage(const PipelineMessage&)),
            this, SLOT(emitFilterGeneratedMessage(const PipelineMessage&)));
    find_surfacefeatures->setVoxelDataContainer(getVoxelDataContainer());
    if(preflight == true) { find_surfacefeatures->preflight(); }
    if(preflight == false) { find_surfacefeatures->execute(); }
  }
  m_SurfaceFeaturesPtr = cellFeatureAttrMat->getPrereqArray<DataArray<bool>, AbstractFilter>(this, m_SurfaceFeaturesArrayName, -302, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_SurfaceFeaturesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_SurfaceFeatures = m_SurfaceFeaturesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  err = 0;
  TEST_PREREQ_DATA(m, DREAM3D, FeatureData, FeaturePhases, err, -303, int32_t, Int32ArrayType, features, 1)
  if(err == -303)
  {
    setErrorCondition(0);
    FindGrainPhases::Pointer find_grainphases = FindGrainPhases::New();
    connect(find_grainphases, SIGNAL(filterGeneratedMessage(const PipelineMessage&)),
            this, SLOT(emitFilterGeneratedMessage(const PipelineMessage&)));
    find_grainphases->setVoxelDataContainer(getVoxelDataContainer());
    if(preflight == true) { find_grainphases->preflight(); }
    if(preflight == false) { find_grainphases->execute(); }
  }
  m_FeaturePhasesPtr = cellFeatureAttrMat->getPrereqArray<DataArray<int32_t>, AbstractFilter>(this, m_FeaturePhasesArrayName, -303, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeaturePhasesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeaturePhases = m_FeaturePhasesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

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
void FindAxisODF::preflight()
void FindAxisODF::preflight()
{
  dataCheck();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindAxisODF::execute()
{
  VoxelDataContainer* m = getVoxelDataContainer();
  if(NULL == m)
  {
    setErrorCondition(-999);
    PipelineMessage em(getHumanLabel(), "The DataContainer Object was NULL", getErrorCondition(), PipelineMessage::Error);
    emit filterGeneratedMessage(em);
    return;
  }
  setErrorCondition(0);

  dataCheck();
  if(getErrorCondition() < 0)
  {
    return;
  }

  StatsDataArray& statsDataArray = *m_StatsDataArray;

  typedef DataArray<unsigned int> XTalType;
  XTalType* crystruct = XTalType::SafeObjectDownCast<IDataArray*, XTalType*>(m->getEnsembleData(DREAM3D::EnsembleData::CrystalStructures).get());

  float r1, r2, r3;
  int bin;
  QVector<FloatArrayType::Pointer> axisodf;
  QVector<float> totalaxes;
  size_t numXTals = crystruct->GetNumberOfTuples();
  axisodf.resize(numXTals);
  totalaxes.resize(numXTals);
  for (size_t i = 1; i < numXTals; i++)
  {
    totalaxes[i] = 0.0;
    axisodf[i] = FloatArrayType::CreateArray((36 * 36 * 36), DREAM3D::StringConstants::AxisOrientation);
    for (int j = 0; j < (36 * 36 * 36); j++)
    {
      axisodf[i]->SetValue(j, 0.0);
    }
  }
  size_t numgrains = m->getNumFeatureTuples();
  for (size_t i = 0; i < numgrains; i++)
  {
    if(m_SurfaceFeatures[i] == false)
    {
      totalaxes[m_FeaturePhases[i]]++;
    }
  }
  for (size_t i = 1; i < numgrains; i++)
  {
    float ea1 = m_AxisEulerAngles[3 * i];
    float ea2 = m_AxisEulerAngles[3 * i + 1];
    float ea3 = m_AxisEulerAngles[3 * i + 2];
    if(m_SurfaceFeatures[i] == 0)
    {
      OrientationMath::eulertoRod(r1, r2, r3, ea1, ea2, ea3);
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

  emit filterGeneratedMessage(PipelineMessage::CreateStatusMessage(getHumanLabel(), "Completed") );
}

