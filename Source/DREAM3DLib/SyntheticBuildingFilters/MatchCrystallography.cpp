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


#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Utilities/DREAM3DRandom.h"
#include "DREAM3DLib/Common/Texture.hpp"
#include "DREAM3DLib/StatsData/PrimaryStatsData.h"
#include "DREAM3DLib/StatsData/PrecipitateStatsData.h"

#include "DREAM3DLib/OrientationOps/OrientationOps.h"
#include "DREAM3DLib/OrientationOps/CubicOps.h"
#include "DREAM3DLib/OrientationOps/HexagonalOps.h"
#include "DREAM3DLib/OrientationOps/OrthoRhombicOps.h"

#include "DREAM3DLib/StatisticsFilters/FindNeighbors.h"
#include "DREAM3DLib/StatisticsFilters/FindNumFeatures.h"

#include "DREAM3DLib/GenericFilters/FindSurfaceFeatures.h"
#include "DREAM3DLib/GenericFilters/FindFeaturePhases.h"



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------

MatchCrystallography::MatchCrystallography() :
  AbstractFilter(),
  m_DataContainerName(DREAM3D::HDF5::VolumeDataContainerName),
  m_CellAttributeMatrixName(DREAM3D::HDF5::CellAttributeMatrixName),
  m_CellFeatureAttributeMatrixName(DREAM3D::HDF5::CellFeatureAttributeMatrixName),
  m_CellEnsembleAttributeMatrixName(DREAM3D::HDF5::CellEnsembleAttributeMatrixName),
  m_FeatureIdsArrayName(DREAM3D::CellData::FeatureIds),
  m_CellEulerAnglesArrayName(DREAM3D::CellData::EulerAngles),
  m_FeaturePhasesArrayName(DREAM3D::FeatureData::Phases),
  m_SurfaceFeaturesArrayName(DREAM3D::FeatureData::SurfaceFeatures),
  m_AvgQuatsArrayName(DREAM3D::FeatureData::AvgQuats),
  m_FeatureEulerAnglesArrayName(DREAM3D::FeatureData::EulerAngles),
  m_VolumesArrayName(DREAM3D::FeatureData::Volumes),
  m_CrystalStructuresArrayName(DREAM3D::EnsembleData::CrystalStructures),
  m_PhaseTypesArrayName(DREAM3D::EnsembleData::PhaseTypes),
  m_NumFeaturesArrayName(DREAM3D::EnsembleData::NumFeatures),
  m_NeighborListArrayName(DREAM3D::FeatureData::NeighborList),
  m_SharedSurfaceAreaListArrayName(DREAM3D::FeatureData::SharedSurfaceAreaList),
  m_StatsDataArrayName(DREAM3D::EnsembleData::Statistics),
  m_MaxIterations(1),
  m_FeatureIds(NULL),
  m_CellEulerAngles(NULL),
  m_SurfaceFeatures(NULL),
  m_FeaturePhases(NULL),
  m_Volumes(NULL),
  m_FeatureEulerAngles(NULL),
  m_AvgQuats(NULL),
  m_NeighborList(NULL),
  m_SharedSurfaceAreaList(NULL),
  m_CrystalStructures(NULL),
  m_PhaseTypes(NULL),
  m_NumFeatures(NULL)
{
  m_OrientationOps = OrientationOps::getOrientationOpsVector();
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
  FilterParameterVector parameters;
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
void MatchCrystallography::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  /* Code to read the values goes between these statements */
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE BEGIN*/
  setMaxIterations( reader->readValue("MaxIterations", getMaxIterations()) );
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE END*/
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int MatchCrystallography::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("MaxIterations", getMaxIterations() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MatchCrystallography::dataCheck(bool preflight, size_t voxels, size_t features, size_t ensembles)
{
  setErrorCondition(0);
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

  QVector<int> dims(1, 1);
  // Cell Data
  m_FeatureIdsPtr = m->getPrereqArray<int32_t, Int32ArrayType, AbstractFilter>(this, m_CellAttributeMatrixName,  m_FeatureIdsArrayName, -301, voxels, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0); /* Assigns the actual data pointer to our instance variable m_FeatureIds */
  dims[0] = 3;
  m_CellEulerAnglesPtr = m->createNonPrereqArray<float, FloatArrayType, AbstractFilter>(this, m_CellAttributeMatrixName,  m_CellEulerAnglesArrayName, 0, voxels, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  m_CellEulerAngles = m_CellEulerAnglesPtr.lock()->getPointer(0); /* Assigns the actual data pointer to our instance variable m_CellEulerAngles */

  // Feature Data
  dims[0] = 1;
  m_FeaturePhasesPtr = m->getPrereqArray<int32_t, Int32ArrayType, AbstractFilter>(this, m_CellFeatureAttributeMatrixName,  m_FeaturePhasesArrayName, -301, features, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  m_FeaturePhases = m_FeaturePhasesPtr.lock()->getPointer(0); /* Assigns the actual data pointer to our instance variable m_FeaturePhases */
  m_SurfaceFeaturesPtr = m->getPrereqArray<bool, BoolArrayType, AbstractFilter>(this, m_CellFeatureAttributeMatrixName,  m_SurfaceFeaturesArrayName, -301, features, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  m_SurfaceFeatures = m_SurfaceFeaturesPtr.lock()->getPointer(0); /* Assigns the actual data pointer to our instance variable m_SurfaceFeatures */
  m_VolumesPtr = m->createNonPrereqArray<float, FloatArrayType, AbstractFilter>(this, m_CellFeatureAttributeMatrixName,  m_VolumesArrayName, 0, features, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  m_Volumes = m_VolumesPtr.lock()->getPointer(0); /* Assigns the actual data pointer to our instance variable m_Volumes */
  dims[0] = 3;
  m_FeatureEulerAnglesPtr = m->createNonPrereqArray<float, FloatArrayType, AbstractFilter>(this, m_CellFeatureAttributeMatrixName,  m_FeatureEulerAnglesArrayName, 0, features, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  m_FeatureEulerAngles = m_FeatureEulerAnglesPtr.lock()->getPointer(0); /* Assigns the actual data pointer to our instance variable m_FeatureEulerAngles */
  dims[0] = 4;
  m_AvgQuatsPtr = m->createNonPrereqArray<float, FloatArrayType, AbstractFilter>(this, m_CellFeatureAttributeMatrixName,  m_AvgQuatsArrayName, 0, features, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  m_AvgQuats = m_AvgQuatsPtr.lock()->getPointer(0); /* Assigns the actual data pointer to our instance variable m_AvgQuats */

  // Now we are going to get a "Pointer" to the NeighborList object out of the DataContainer
  if (NULL == m->getAttributeMatrix(m_CellFeatureAttributeMatrixName)->getAttributeArray(m_NeighborListArrayName).get())
  {

    QString ss = QObject::tr("'NeighborLists' are not available and are required for this filter to run. A filter that generates NeighborLists needs to be placed before this filter in the pipeline.");
    setErrorCondition(-305);
    addErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }
  else
  {
    m_NeighborList = NeighborList<int>::SafeObjectDownCast<IDataArray*, NeighborList<int>*>(m->getAttributeMatrix(m_CellFeatureAttributeMatrixName)->getAttributeArray(DREAM3D::FeatureData::NeighborList).get());
  }

  if(NULL == m->getAttributeMatrix(m_CellFeatureAttributeMatrixName)->getAttributeArray(m_SharedSurfaceAreaListArrayName).get())
  {

    QString ss = QObject::tr("'SharedSurfaceAreaLists' are not available and are required for this filter to run. A filter that generates 'Shared SurfaceArea Lists' needs to be placed before this filter in the pipeline.");
    setErrorCondition(-306);
    addErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }
  else
  {
    m_SharedSurfaceAreaList = NeighborList<float>::SafeObjectDownCast<IDataArray*, NeighborList<float>*>(m->getAttributeMatrix(m_CellFeatureAttributeMatrixName)->getAttributeArray(DREAM3D::FeatureData::SharedSurfaceAreaList).get());
  }

  if(NULL == m->getAttributeMatrix(m_CellEnsembleAttributeMatrixName)->getAttributeArray(m_StatsDataArrayName).get())
  {

    QString ss = QObject::tr("'Ensemble Statistics' are not available and are required for this filter to run. A filter that generates 'Shared SurfaceArea Lists' needs to be placed before this filter in the pipeline.");
    setErrorCondition(-310);
    addErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }
  else
  {
    m_StatsDataArray = StatsDataArray::SafeObjectDownCast<IDataArray*, StatsDataArray*>(m->getAttributeMatrix(m_CellEnsembleAttributeMatrixName)->getAttributeArray(m_StatsDataArrayName).get());
  }
  // Ensemble Data
  typedef DataArray<unsigned int> XTalStructArrayType;
  typedef DataArray<unsigned int> PhaseTypeArrayType;
  dims[0] = 1;
  m->getPrereqArray<unsigned int, XTalStructArrayType, AbstractFilter>(this, m_CellAttributeMatrixName,  m_CrystalStructuresArrayName, m_CrystalStructures, -301, ensembles, dims);
  m->getPrereqArray<unsigned int, PhaseTypeArrayType, AbstractFilter>(this, m_CellAttributeMatrixName,  m_PhaseTypesArrayName, m_PhaseTypes, -301, ensembles, dims);
  m->getPrereqArray<int32_t, Int32ArrayType, AbstractFilter>(this, m_CellAttributeMatrixName,  m_NumFeaturesArrayName, m_NumFeatures, -301, ensembles, dims);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MatchCrystallography::preflight()
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
void MatchCrystallography::execute()
{
  int err = 0;
  setErrorCondition(err);
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The DataContainer Object was NULL", -999);
    return;
  }

  int64_t totalPoints = m->getAttributeMatrix(m_CellAttributeMatrixName)->getNumTuples();
  int64_t totalFeatures = m->getAttributeMatrix(m_CellFeatureAttributeMatrixName)->getNumTuples();
  int64_t totalEnsembles = m->getAttributeMatrix(m_CellEnsembleAttributeMatrixName)->getNumTuples();
  dataCheck(false, totalPoints, totalFeatures, totalEnsembles);
  if (getErrorCondition() < 0)
  {
    return;
  }


  QString ss;
  ss = QObject::tr("Determining Volumes");
  notifyStatusMessage(ss);
  determine_volumes();


  ss = QObject::tr("Determining Boundary Areas");
  notifyStatusMessage(ss);
  determine_boundary_areas();

  for (int64_t i = 1; i < totalEnsembles; ++i)
  {
    if(m_PhaseTypes[i] == DREAM3D::PhaseType::PrimaryPhase ||  m_PhaseTypes[i] == DREAM3D::PhaseType::PrecipitatePhase)
    {
      ss = QObject::tr("Initializing Arrays of Phase ").arg(i);
      notifyStatusMessage("Initializing Arrays");
      initializeArrays(i);


      ss = QObject::tr("Assigning Eulers to Phase ").arg(i);
      notifyStatusMessage(ss);
      assign_eulers(i);


      ss = QObject::tr("Measuring Misorientations of Phase ").arg(i);
      notifyStatusMessage(ss);
      measure_misorientations(i);


      ss = QObject::tr("Matching Crystallography of Phase ").arg(i);
      notifyStatusMessage(ss);
      matchCrystallography(i);

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
  // VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

  StatsDataArray& statsDataArray = *m_StatsDataArray;

  // size_t size = m->getNumCellEnsembleTuples();

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
    QString ss;
    ss = QObject::tr("Improper PhaseType for MatchCrystallography");
    addErrorMessage(getHumanLabel(), ss, getErrorCondition());
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
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

  int64_t totalPoints = m->getAttributeMatrix(m_CellAttributeMatrixName)->getNumTuples();
  int64_t totalFeatures = m->getAttributeMatrix(m_CellFeatureAttributeMatrixName)->getNumTuples();
  int64_t totalEnsembles = m->getAttributeMatrix(m_CellEnsembleAttributeMatrixName)->getNumTuples();

  unbiasedvol.resize(totalEnsembles);
  for (int64_t i = 1; i < totalFeatures; i++)
  {
    m_Volumes[i] = 0.0;
  }
  for (int64_t i = 0; i < totalPoints; i++)
  {
    m_Volumes[m_FeatureIds[i]]++;
  }
  float res_scalar = m->getXRes() * m->getYRes() * m->getZRes();
  for (size_t i = 1; i < totalFeatures; i++)
  {
    m_Volumes[i] = m_Volumes[i] * res_scalar;
    if(m_SurfaceFeatures[i] == false)
    {
      unbiasedvol[m_FeaturePhases[i]] = unbiasedvol[m_FeaturePhases[i]] + m_Volumes[i];
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MatchCrystallography::determine_boundary_areas()
{
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

  NeighborList<int>& neighborlist = *m_NeighborList;
  NeighborList<float>& neighborsurfacearealist = *m_SharedSurfaceAreaList;

  int64_t totalFeatures = m->getAttributeMatrix(m_CellFeatureAttributeMatrixName)->getNumTuples();
  int64_t totalEnsembles = m->getAttributeMatrix(m_CellEnsembleAttributeMatrixName)->getNumTuples();

  m_TotalSurfaceArea.fill(0.0, totalEnsembles);

  int phase1, phase2;
  for (size_t i = 1; i < totalFeatures; i++)
  {
    phase1 = m_FeaturePhases[i];
    size_t size = 0;
    if(neighborlist[i].size() != 0 && neighborsurfacearealist[i].size() == neighborlist[i].size())
    {
      size = neighborlist[i].size();
    }

    for (size_t j = 0; j < size; j++)
    {
      int nname = neighborlist[i][j];
      float neighsurfarea = neighborsurfacearealist[i][j];
      phase2 = m_FeaturePhases[nname];
      if(phase1 == phase2)
      {
        m_TotalSurfaceArea[phase1] = m_TotalSurfaceArea[phase1] + neighsurfarea;
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
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

  int numbins = 0;
  float synea1 = 0, synea2 = 0, synea3 = 0;
  QuatF q;
  QuatF* avgQuats = reinterpret_cast<QuatF*>(m_AvgQuats);
  float random;
  int choose, phase;

  int64_t totalFeatures = m->getAttributeMatrix(m_CellFeatureAttributeMatrixName)->getNumTuples();

  for (int64_t i = 1; i < totalFeatures; i++)
  {
    phase = m_FeaturePhases[i];
    if(phase == ensem)
    {
      random = static_cast<float>( rg.genrand_res53() );

      if( Ebsd::CrystalStructure::Cubic_High == m_CrystalStructures[phase] ) { numbins = CubicOps::k_OdfSize; };
      if( Ebsd::CrystalStructure::Hexagonal_High == m_CrystalStructures[phase] ) { numbins = HexagonalOps::k_OdfSize; }

      choose = pick_euler(random, numbins);

      m_OrientationOps[m_CrystalStructures[ensem]]->determineEulerAngles(choose, synea1, synea2, synea3);
      m_FeatureEulerAngles[3 * i] = synea1;
      m_FeatureEulerAngles[3 * i + 1] = synea2;
      m_FeatureEulerAngles[3 * i + 2] = synea3;
      OrientationMath::EulertoQuat(q, synea1, synea2, synea3);
      QuaternionMathF::Copy(q, avgQuats[i]);
      if(m_SurfaceFeatures[i] == false)
      {
        simodf->SetValue(choose, (simodf->GetValue(choose) + m_Volumes[i] / unbiasedvol[ensem]));
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int MatchCrystallography::pick_euler(float random, int numbins)
{
  int choose = 0;
  float totaldensity = 0;

  for (int j = 0; j < numbins; j++)
  {
    float density = actualodf->GetValue(j);
    float td1 = totaldensity;
    totaldensity = totaldensity + density;
    if (random < totaldensity && random >= td1) { choose = static_cast<int> (j); break; }
  }
  return choose;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MatchCrystallography::MC_LoopBody1(int feature, int ensem, int j, float neighsurfarea, unsigned int sym, QuatF& q1, QuatF& q2)
{
  float w;
  float n1, n2, n3;
  float r1, r2, r3;
  float curmiso1, curmiso2, curmiso3;
  size_t curmisobin, newmisobin;

  curmiso1 = m_MisorientationLists[feature][3 * j];
  curmiso2 = m_MisorientationLists[feature][3 * j + 1];
  curmiso3 = m_MisorientationLists[feature][3 * j + 2];
  curmisobin = m_OrientationOps[sym]->getMisoBin(curmiso1, curmiso2, curmiso3);
  w = m_OrientationOps[sym]->getMisoQuat(q1, q2, n1, n2, n3);
  OrientationMath::AxisAngletoRod(w, n1, n2, n3, r1, r2, r3);
  newmisobin = m_OrientationOps[sym]->getMisoBin(n1, n2, n3);
  mdfchange = mdfchange
              + (((actualmdf->GetValue(curmisobin) - simmdf->GetValue(curmisobin)) * (actualmdf->GetValue(curmisobin) - simmdf->GetValue(curmisobin)))
                 - ((actualmdf->GetValue(curmisobin) - (simmdf->GetValue(curmisobin) - (neighsurfarea / m_TotalSurfaceArea[ensem])))
                    * (actualmdf->GetValue(curmisobin) - (simmdf->GetValue(curmisobin) - (neighsurfarea / m_TotalSurfaceArea[ensem])))));
  mdfchange = mdfchange
              + (((actualmdf->GetValue(newmisobin) - simmdf->GetValue(newmisobin)) * (actualmdf->GetValue(newmisobin) - simmdf->GetValue(newmisobin)))
                 - ((actualmdf->GetValue(newmisobin) - (simmdf->GetValue(newmisobin) + (neighsurfarea / m_TotalSurfaceArea[ensem])))
                    * (actualmdf->GetValue(newmisobin) - (simmdf->GetValue(newmisobin) + (neighsurfarea / m_TotalSurfaceArea[ensem])))));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MatchCrystallography::MC_LoopBody2(int feature, int ensem, int j, float neighsurfarea, unsigned int sym, QuatF& q1, QuatF& q2)
{
  float w;
  float n1, n2, n3;
  float r1, r2, r3;
  float curmiso1, curmiso2, curmiso3;
  size_t curmisobin, newmisobin;
  float miso1 = 0.0f, miso2 = 0.0f, miso3 = 0.0f;

  curmiso1 = m_MisorientationLists[feature][3 * j];
  curmiso2 = m_MisorientationLists[feature][3 * j + 1];
  curmiso3 = m_MisorientationLists[feature][3 * j + 2];
  curmisobin = m_OrientationOps[sym]->getMisoBin(curmiso1, curmiso2, curmiso3);
  w = m_OrientationOps[sym]->getMisoQuat(q1, q2, n1, n2, n3);
  OrientationMath::AxisAngletoRod(w, n1, n2, n3, r1, r2, r3);
  newmisobin = m_OrientationOps[sym]->getMisoBin(n1, n2, n3);
  m_MisorientationLists[feature][3 * j] = miso1;
  m_MisorientationLists[feature][3 * j + 1] = miso2;
  m_MisorientationLists[feature][3 * j + 2] = miso3;
  simmdf->SetValue(curmisobin, (simmdf->GetValue(curmisobin) - (neighsurfarea / m_TotalSurfaceArea[ensem])));
  simmdf->SetValue(newmisobin, (simmdf->GetValue(newmisobin) + (neighsurfarea / m_TotalSurfaceArea[ensem])));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MatchCrystallography::matchCrystallography(int ensem)
{
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

  // But since a pointer is difficult to use operators with we will now create a
  // reference variable to the pointer with the correct variable name that allows
  // us to use the same syntax as the "vector of vectors"
  NeighborList<int>& neighborlist = *m_NeighborList;
  NeighborList<float>& neighborsurfacearealist = *m_SharedSurfaceAreaList;

  int64_t totalPoints = m->getAttributeMatrix(m_CellAttributeMatrixName)->getNumTuples();
  int64_t totalFeatures = m->getAttributeMatrix(m_CellFeatureAttributeMatrixName)->getNumTuples();

  DREAM3D_RANDOMNG_NEW()
  int numbins = 0;
  int iterations = 0, badtrycount = 0;
  float random = 0;
  size_t counter = 0;

  QuatF q1;
  QuatF q2;
  QuatF* avgQuats = reinterpret_cast<QuatF*>(m_AvgQuats);

  float ea1 = 0, ea2 = 0, ea3 = 0;
  float r1 = 0, r2 = 0, r3 = 0;
  float g1ea1 = 0, g1ea2 = 0, g1ea3 = 0, g2ea1 = 0, g2ea2 = 0, g2ea3 = 0;
  int g1odfbin = 0, g2odfbin = 0;
  float totaldensity = 0, deltaerror = 0;
  float currentodferror = 0, currentmdferror = 0;
  size_t selectedfeature1 = 0, selectedfeature2 = 0;
  iterations = 0;
  badtrycount = 0;
  if( Ebsd::CrystalStructure::Cubic_High == m_CrystalStructures[ensem]) { numbins = 18 * 18 * 18; }
  if( Ebsd::CrystalStructure::Hexagonal_High == m_CrystalStructures[ensem]) { numbins = 36 * 36 * 12; }
  while (badtrycount < (m_MaxIterations / 10) && iterations < m_MaxIterations)
  {
    QString ss = QObject::tr("Matching Crystallography - Swapping/Switching Orientations - %1% Complete").arg(((float)iterations / float(1000 * totalFeatures)) * 100);
    //      notifyStatusMessage(ss);
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
      selectedfeature1 = int(rg.genrand_res53() * totalFeatures);
      while ((m_SurfaceFeatures[selectedfeature1] == true || m_FeaturePhases[selectedfeature1] != static_cast<int32_t>(ensem)) && counter < totalFeatures)
      {
        if(selectedfeature1 >= totalFeatures) { selectedfeature1 = selectedfeature1 - totalFeatures; }
        selectedfeature1++;
        counter++;
      }
      if(counter == totalFeatures)
      {
        badtrycount = 10 * m_NumFeatures[ensem];
      }
      else
      {
        ea1 = m_FeatureEulerAngles[3 * selectedfeature1];
        ea2 = m_FeatureEulerAngles[3 * selectedfeature1 + 1];
        ea3 = m_FeatureEulerAngles[3 * selectedfeature1 + 2];
        OrientationMath::EulertoRod(r1, r2, r3, ea1, ea2, ea3);
        g1odfbin = m_OrientationOps[m_CrystalStructures[ensem]]->getOdfBin(r1, r2, r3);
        random = static_cast<float>( rg.genrand_res53() );
        int choose = 0;

        choose = pick_euler(random, numbins);

        m_OrientationOps[m_CrystalStructures[ensem]]->determineEulerAngles(choose, g1ea1, g1ea2, g1ea3);
        OrientationMath::EulertoQuat(q1, g1ea1, g1ea2, g1ea3);

        odfchange = ((actualodf->GetValue(choose) - simodf->GetValue(choose)) * (actualodf->GetValue(choose) - simodf->GetValue(choose)))
                    - ((actualodf->GetValue(choose) - (simodf->GetValue(choose) + (m_Volumes[selectedfeature1] / unbiasedvol[ensem])))
                       * (actualodf->GetValue(choose) - (simodf->GetValue(choose) + (m_Volumes[selectedfeature1] / unbiasedvol[ensem]))));
        odfchange = odfchange
                    + (((actualodf->GetValue(g1odfbin) - simodf->GetValue(g1odfbin)) * (actualodf->GetValue(g1odfbin) - simodf->GetValue(g1odfbin)))
                       - ((actualodf->GetValue(g1odfbin) - (simodf->GetValue(g1odfbin) - (m_Volumes[selectedfeature1] / unbiasedvol[ensem])))
                          * (actualodf->GetValue(g1odfbin) - (simodf->GetValue(g1odfbin) - (m_Volumes[selectedfeature1] / unbiasedvol[ensem])))));

        mdfchange = 0;
        size_t size = 0;
        if(neighborlist[selectedfeature1].size() != 0) { size = neighborlist[selectedfeature1].size(); }
        for (size_t j = 0; j < size; j++)
        {
          int neighbor = neighborlist[selectedfeature1][j];
          ea1 = m_FeatureEulerAngles[3 * neighbor];
          ea2 = m_FeatureEulerAngles[3 * neighbor + 1];
          ea3 = m_FeatureEulerAngles[3 * neighbor + 2];
          OrientationMath::EulertoQuat(q2, ea1, ea2, ea3);
          float neighsurfarea = neighborsurfacearealist[selectedfeature1][j];
          MC_LoopBody1(selectedfeature1, ensem, j, neighsurfarea, m_CrystalStructures[ensem], q1, q2);
        }

        deltaerror = (odfchange / currentodferror) + (mdfchange / currentmdferror);
        if(deltaerror > 0)
        {
          badtrycount = 0;
          m_FeatureEulerAngles[3 * selectedfeature1] = g1ea1;
          m_FeatureEulerAngles[3 * selectedfeature1 + 1] = g1ea2;
          m_FeatureEulerAngles[3 * selectedfeature1 + 2] = g1ea3;
          QuaternionMathF::Copy(q1, avgQuats[selectedfeature1]);
          simodf->SetValue(choose, (simodf->GetValue(choose) + (m_Volumes[selectedfeature1] / unbiasedvol[ensem])));
          simodf->SetValue(g1odfbin, (simodf->GetValue(g1odfbin) - (m_Volumes[selectedfeature1] / unbiasedvol[ensem])));
          size = 0;
          if(neighborlist[selectedfeature1].size() != 0) { size = neighborlist[selectedfeature1].size(); }
          for (size_t j = 0; j < size; j++)
          {
            int neighbor = neighborlist[selectedfeature1][j];
            ea1 = m_FeatureEulerAngles[3 * neighbor];
            ea2 = m_FeatureEulerAngles[3 * neighbor + 1];
            ea3 = m_FeatureEulerAngles[3 * neighbor + 2];
            OrientationMath::EulertoQuat(q2, ea1, ea2, ea3);
            float neighsurfarea = neighborsurfacearealist[selectedfeature1][j];
            MC_LoopBody2(selectedfeature1, ensem, j, neighsurfarea, m_CrystalStructures[ensem], q1, q2);
          }
        }
      }
    }
    else // SwitchOrientation
    {
      counter = 0;
      selectedfeature1 = int(rg.genrand_res53() * totalFeatures);
      while ((m_SurfaceFeatures[selectedfeature1] == true || m_FeaturePhases[selectedfeature1] != static_cast<int32_t>(ensem)) && counter < totalFeatures)
      {
        if(selectedfeature1 >= totalFeatures) { selectedfeature1 = selectedfeature1 - totalFeatures; }
        selectedfeature1++;
        counter++;
      }
      if(counter == totalFeatures)
      {
        badtrycount = 10 * m_NumFeatures[ensem];
      }
      else
      {
        counter = 0;
        selectedfeature2 = int(rg.genrand_res53() * totalFeatures);
        while ((m_SurfaceFeatures[selectedfeature2] == true || m_FeaturePhases[selectedfeature2] != static_cast<int32_t>(ensem) || selectedfeature2 == selectedfeature1) && counter < totalFeatures)
        {
          if(selectedfeature2 >= totalFeatures) { selectedfeature2 = selectedfeature2 - totalFeatures; }
          selectedfeature2++;
          counter++;
        }
        if(counter == totalFeatures)
        {
          badtrycount = 10 * m_NumFeatures[ensem];
        }
        else
        {
          g1ea1 = m_FeatureEulerAngles[3 * selectedfeature1];
          g1ea2 = m_FeatureEulerAngles[3 * selectedfeature1 + 1];
          g1ea3 = m_FeatureEulerAngles[3 * selectedfeature1 + 2];
          g2ea1 = m_FeatureEulerAngles[3 * selectedfeature2];
          g2ea2 = m_FeatureEulerAngles[3 * selectedfeature2 + 1];
          g2ea3 = m_FeatureEulerAngles[3 * selectedfeature2 + 2];
          QuaternionMathF::Copy(q1, avgQuats[selectedfeature1]);
          OrientationMath::EulertoRod(r1, r2, r3, g1ea1, g1ea2, g1ea3);
          g1odfbin = m_OrientationOps[m_CrystalStructures[ensem]]->getOdfBin(r1, r2, r3);
          QuaternionMathF::Copy(q1, avgQuats[selectedfeature2]);
          OrientationMath::EulertoRod(r1, r2, r3, g2ea1, g2ea2, g2ea3);
          g2odfbin = m_OrientationOps[m_CrystalStructures[ensem]]->getOdfBin(r1, r2, r3);

          odfchange = ((actualodf->GetValue(g1odfbin) - simodf->GetValue(g1odfbin)) * (actualodf->GetValue(g1odfbin) - simodf->GetValue(g1odfbin)))
                      - ((actualodf->GetValue(g1odfbin)
                          - (simodf->GetValue(g1odfbin) - (m_Volumes[selectedfeature1] / unbiasedvol[ensem])
                             + (m_Volumes[selectedfeature2] / unbiasedvol[ensem])))
                         * (actualodf->GetValue(g1odfbin)
                            - (simodf->GetValue(g1odfbin) - (m_Volumes[selectedfeature1] / unbiasedvol[ensem])
                               + (m_Volumes[selectedfeature2] / unbiasedvol[ensem]))));
          odfchange = odfchange
                      + (((actualodf->GetValue(g2odfbin) - simodf->GetValue(g2odfbin)) * (actualodf->GetValue(g2odfbin) - simodf->GetValue(g2odfbin)))
                         - ((actualodf->GetValue(g2odfbin)
                             - (simodf->GetValue(g2odfbin) - (m_Volumes[selectedfeature2] / unbiasedvol[ensem])
                                + (m_Volumes[selectedfeature1] / unbiasedvol[ensem])))
                            * (actualodf->GetValue(g2odfbin)
                               - (simodf->GetValue(g2odfbin) - (m_Volumes[selectedfeature2] / unbiasedvol[ensem])
                                  + (m_Volumes[selectedfeature1] / unbiasedvol[ensem])))));

          mdfchange = 0;
          OrientationMath::EulertoQuat(q1, g2ea1, g2ea2, g2ea3);
          size_t size = 0;
          if(neighborlist[selectedfeature1].size() != 0) { size = neighborlist[selectedfeature1].size(); }
          for (size_t j = 0; j < size; j++)
          {
            int neighbor = neighborlist[selectedfeature1][j];
            ea1 = m_FeatureEulerAngles[3 * neighbor];
            ea2 = m_FeatureEulerAngles[3 * neighbor + 1];
            ea3 = m_FeatureEulerAngles[3 * neighbor + 2];
            OrientationMath::EulertoQuat(q2, ea1, ea2, ea3);
            float neighsurfarea = neighborsurfacearealist[selectedfeature1][j];
            if(neighbor != static_cast<int>(selectedfeature2) )
            {
              MC_LoopBody1(selectedfeature1, ensem, j, neighsurfarea, m_CrystalStructures[ensem], q1, q2);
            }
          }

          OrientationMath::EulertoQuat(q1, g1ea1, g1ea2, g1ea3);
          size = 0;
          if(neighborlist[selectedfeature2].size() != 0) { size = neighborlist[selectedfeature2].size(); }
          for (size_t j = 0; j < size; j++)
          {
            size_t neighbor = neighborlist[selectedfeature2][j];
            ea1 = m_FeatureEulerAngles[3 * neighbor];
            ea2 = m_FeatureEulerAngles[3 * neighbor + 1];
            ea3 = m_FeatureEulerAngles[3 * neighbor + 2];
            OrientationMath::EulertoQuat(q2, ea1, ea2, ea3);
            float neighsurfarea = neighborsurfacearealist[selectedfeature2][j];
            if(neighbor != selectedfeature1)
            {
              MC_LoopBody1(selectedfeature2, ensem, j, neighsurfarea, m_CrystalStructures[ensem], q1, q2);
            }
          }

          deltaerror = (odfchange / currentodferror) + (mdfchange / currentmdferror);
          if(deltaerror > 0)
          {

            badtrycount = 0;
            m_FeatureEulerAngles[3 * selectedfeature1] = g2ea1;
            m_FeatureEulerAngles[3 * selectedfeature1 + 1] = g2ea2;
            m_FeatureEulerAngles[3 * selectedfeature1 + 2] = g2ea3;
            m_FeatureEulerAngles[3 * selectedfeature2] = g1ea1;
            m_FeatureEulerAngles[3 * selectedfeature2 + 1] = g1ea2;
            m_FeatureEulerAngles[3 * selectedfeature2 + 2] = g1ea3;
            simodf->SetValue(g1odfbin, (simodf->GetValue(g1odfbin) + (m_Volumes[selectedfeature2] / unbiasedvol[ensem])
                                        - (m_Volumes[selectedfeature1] / unbiasedvol[ensem])));
            simodf->SetValue(g2odfbin, (simodf->GetValue(g2odfbin) + (m_Volumes[selectedfeature1] / unbiasedvol[ensem])
                                        - (m_Volumes[selectedfeature2] / unbiasedvol[ensem])));

            OrientationMath::EulertoQuat(q1, g2ea1, g2ea2, g2ea3);
            QuaternionMathF::Copy(avgQuats[selectedfeature1], q1);
            size = 0;
            if(neighborlist[selectedfeature1].size() != 0) { size = neighborlist[selectedfeature1].size(); }
            for (size_t j = 0; j < size; j++)
            {
              size_t neighbor = neighborlist[selectedfeature1][j];
              ea1 = m_FeatureEulerAngles[3 * neighbor];
              ea2 = m_FeatureEulerAngles[3 * neighbor + 1];
              ea3 = m_FeatureEulerAngles[3 * neighbor + 2];
              OrientationMath::EulertoQuat(q2, ea1, ea2, ea3);
              float neighsurfarea = neighborsurfacearealist[selectedfeature1][j];
              if(neighbor != selectedfeature2)
              {
                MC_LoopBody2(selectedfeature1, ensem, j, neighsurfarea, m_CrystalStructures[ensem], q1, q2);
              }
            }

            OrientationMath::EulertoQuat(q1, g1ea1, g1ea2, g1ea3);
            QuaternionMathF::Copy(q1, avgQuats[selectedfeature2]);
            size = 0;
            if(neighborlist[selectedfeature2].size() != 0) { size = neighborlist[selectedfeature2].size(); }
            for (size_t j = 0; j < size; j++)
            {
              size_t neighbor = neighborlist[selectedfeature2][j];
              ea1 = m_FeatureEulerAngles[3 * neighbor];
              ea2 = m_FeatureEulerAngles[3 * neighbor + 1];
              ea3 = m_FeatureEulerAngles[3 * neighbor + 2];
              OrientationMath::EulertoQuat(q2, ea1, ea2, ea3);
              float neighsurfarea = neighborsurfacearealist[selectedfeature2][j];
              if(neighbor != selectedfeature1)
              {
                MC_LoopBody2(selectedfeature2, ensem, j, neighsurfarea, m_CrystalStructures[ensem], q1, q2);
              }
            }
          }
        }
      }
    }
  }
  for (int i = 0; i < totalPoints; i++)
  {
    m_CellEulerAngles[3 * i] = m_FeatureEulerAngles[3 * m_FeatureIds[i]];
    m_CellEulerAngles[3 * i + 1] = m_FeatureEulerAngles[3 * m_FeatureIds[i] + 1];
    m_CellEulerAngles[3 * i + 2] = m_FeatureEulerAngles[3 * m_FeatureIds[i] + 2];
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MatchCrystallography::measure_misorientations(int ensem)
{
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

  // But since a pointer is difficult to use operators with we will now create a
  // reference variable to the pointer with the correct variable name that allows
  // us to use the same syntax as the "vector of vectors"
  NeighborList<int>& neighborlist = *m_NeighborList;
  NeighborList<float>& neighborsurfacearealist = *m_SharedSurfaceAreaList;

  float w;
  float n1 = 0.0f, n2 = 0.0f, n3 = 0.0f;
  float r1 = 0.0f, r2 = 0.0f, r3 = 0.0f;
  QuatF q1;
  QuatF q2;
  QuatF* avgQuats = reinterpret_cast<QuatF*>(m_AvgQuats);

  unsigned int crys1;
  int mbin = 0;

  int64_t totalFeatures = m->getAttributeMatrix(m_CellFeatureAttributeMatrixName)->getNumTuples();

  //float threshold = 0.0f;

  m_MisorientationLists.resize(totalFeatures);
  QString ss;
  for (size_t i = 1; i < totalFeatures; i++)
  {
    if(m_FeaturePhases[i] == ensem)
    {
      if(m_MisorientationLists[i].size() != 0)
      {
        m_MisorientationLists[i].clear();
      }
      if(neighborlist[i].size() != 0)
      {
        m_MisorientationLists[i].fill(0.0f, neighborlist[i].size() * 3);
      }
      QuaternionMathF::Copy(avgQuats[i], q1);
      crys1 = m_CrystalStructures[ensem];
      size_t size = 0;
      if(neighborlist[i].size() != 0 && neighborsurfacearealist[i].size() == neighborlist[i].size())
      {
        size = neighborlist[i].size();
      }

      for (size_t j = 0; j < size; j++)
      {
        int nname = neighborlist[i][j];
        if(m_FeaturePhases[nname] == ensem)
        {
          w = 10000.0;
          float neighsurfarea = neighborsurfacearealist[i][j];
          QuaternionMathF::Copy(avgQuats[nname], q2);
          w = m_OrientationOps[crys1]->getMisoQuat(q1, q2, n1, n2, n3);
          OrientationMath::AxisAngletoRod(w, n1, n2, n3, r1, r2, r3);
          m_MisorientationLists[i][3 * j] = r1;
          m_MisorientationLists[i][3 * j + 1] = r2;
          m_MisorientationLists[i][3 * j + 2] = r3;
          mbin = m_OrientationOps[crys1]->getMisoBin(m_MisorientationLists[i][3 * j], m_MisorientationLists[i][3 * j + 1], m_MisorientationLists[i][3 * j + 2]);
          if(m_SurfaceFeatures[i] == false && (nname > static_cast<int>(i) || m_SurfaceFeatures[nname] == true))
          {
            simmdf->SetValue(mbin, (simmdf->GetValue(mbin) + (neighsurfarea / m_TotalSurfaceArea[m_FeaturePhases[i]])));
          }
        }
        else
        {
          m_MisorientationLists[i][3 * j] = -100;
          m_MisorientationLists[i][3 * j + 1] = -100;
          m_MisorientationLists[i][3 * j + 2] = -100;
        }
      }
    }
  }
}
