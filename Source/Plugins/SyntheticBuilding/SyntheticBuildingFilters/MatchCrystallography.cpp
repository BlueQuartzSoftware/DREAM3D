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



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------

MatchCrystallography::MatchCrystallography() :
  AbstractFilter(),
  m_DataContainerName(DREAM3D::Defaults::VolumeDataContainerName),
  m_CellAttributeMatrixName(DREAM3D::Defaults::CellAttributeMatrixName),
  m_CellFeatureAttributeMatrixName(DREAM3D::Defaults::CellFeatureAttributeMatrixName),
  m_CellEnsembleAttributeMatrixName(DREAM3D::Defaults::CellEnsembleAttributeMatrixName),
  m_NeighborListArrayName(DREAM3D::FeatureData::NeighborList),
  m_SharedSurfaceAreaListArrayName(DREAM3D::FeatureData::SharedSurfaceAreaList),
  m_StatsDataArrayName(DREAM3D::EnsembleData::Statistics),
  m_MaxIterations(1),
  m_FeatureIdsArrayName(DREAM3D::CellData::FeatureIds),
  m_FeatureIds(NULL),
  m_CellEulerAnglesArrayName(DREAM3D::CellData::EulerAngles),
  m_CellEulerAngles(NULL),
  m_SurfaceFeaturesArrayName(DREAM3D::FeatureData::SurfaceFeatures),
  m_SurfaceFeatures(NULL),
  m_FeaturePhasesArrayName(DREAM3D::FeatureData::Phases),
  m_FeaturePhases(NULL),
  m_VolumesArrayName(DREAM3D::FeatureData::Volumes),
  m_Volumes(NULL),
  m_FeatureEulerAnglesArrayName(DREAM3D::FeatureData::EulerAngles),
  m_FeatureEulerAngles(NULL),
  m_AvgQuatsArrayName(DREAM3D::FeatureData::AvgQuats),
  m_AvgQuats(NULL),
  m_CrystalStructuresArrayName(DREAM3D::EnsembleData::CrystalStructures),
  m_CrystalStructures(NULL),
  m_PhaseTypesArrayName(DREAM3D::EnsembleData::PhaseTypes),
  m_PhaseTypes(NULL),
  m_NumFeaturesArrayName(DREAM3D::EnsembleData::NumFeatures),
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
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("Maximum Number of Iterations (Swaps)");
    parameter->setPropertyName("MaxIterations");
    parameter->setWidgetType(FilterParameterWidgetType::IntWidget);
    parameter->setValueType("int");
    parameter->setUnits("");
    parameters.push_back(parameter);
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
void MatchCrystallography::dataCheck()
{
  setErrorCondition(0);

  VolumeDataContainer* m = getDataContainerArray()->getPrereqDataContainer<VolumeDataContainer, AbstractFilter>(this, getDataContainerName(), false);
  if(getErrorCondition() < 0 || NULL == m) { return; }
  AttributeMatrix::Pointer cellAttrMat = m->getPrereqAttributeMatrix<AbstractFilter>(this, getCellAttributeMatrixName(), -301);
  if(getErrorCondition() < 0 || NULL == cellAttrMat.get() ) { return; }
  AttributeMatrix::Pointer cellFeatureAttrMat = m->getPrereqAttributeMatrix<AbstractFilter>(this, getCellFeatureAttributeMatrixName(), -302);
  if(getErrorCondition() < 0 || NULL == cellFeatureAttrMat.get()) { return; }

  AttributeMatrix::Pointer cellEnsembleAttrMat = m->getPrereqAttributeMatrix<AbstractFilter>(this, getCellEnsembleAttributeMatrixName(), -303);
  if(getErrorCondition() < 0 || NULL == cellEnsembleAttrMat.get()) { return; }


  QVector<size_t> dims(1, 1);
  // Cell Data
  m_FeatureIdsPtr = cellAttrMat->getPrereqArray<DataArray<int32_t>, AbstractFilter>(this,  m_FeatureIdsArrayName, -301, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeatureIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  dims[0] = 3;
  m_CellEulerAnglesPtr = cellAttrMat->createNonPrereqArray<DataArray<float>, AbstractFilter, float>(this,  m_CellEulerAnglesArrayName, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CellEulerAnglesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_CellEulerAngles = m_CellEulerAnglesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  // Feature Data
  dims[0] = 1;
  m_FeaturePhasesPtr = cellFeatureAttrMat->getPrereqArray<DataArray<int32_t>, AbstractFilter>(this,  m_FeaturePhasesArrayName, -301, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeaturePhasesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeaturePhases = m_FeaturePhasesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  m_SurfaceFeaturesPtr = cellFeatureAttrMat->getPrereqArray<DataArray<bool>, AbstractFilter>(this,  m_SurfaceFeaturesArrayName, -301, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_SurfaceFeaturesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_SurfaceFeatures = m_SurfaceFeaturesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  m_VolumesPtr = cellFeatureAttrMat->createNonPrereqArray<DataArray<float>, AbstractFilter, float>(this,  m_VolumesArrayName, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_VolumesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_Volumes = m_VolumesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  dims[0] = 3;
  m_FeatureEulerAnglesPtr = cellFeatureAttrMat->createNonPrereqArray<DataArray<float>, AbstractFilter, float>(this,  m_FeatureEulerAnglesArrayName, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeatureEulerAnglesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeatureEulerAngles = m_FeatureEulerAnglesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  dims[0] = 4;
  m_AvgQuatsPtr = cellFeatureAttrMat->createNonPrereqArray<DataArray<float>, AbstractFilter, float>(this,  m_AvgQuatsArrayName, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_AvgQuatsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_AvgQuats = m_AvgQuatsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  // Now we are going to get a "Pointer" to the NeighborList object out of the DataContainer
  if (NULL == m->getAttributeMatrix(m_CellFeatureAttributeMatrixName)->getAttributeArray(m_NeighborListArrayName).get())
  {

    QString ss = QObject::tr("'NeighborLists' are not available and are required for this filter to run. A filter that generates NeighborLists needs to be placed before this filter in the pipeline.");
    setErrorCondition(-305);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }
  else
  {
    m_NeighborList = NeighborList<int>::SafeObjectDownCast<IDataArray*, NeighborList<int>*>(m->getAttributeMatrix(m_CellFeatureAttributeMatrixName)->getAttributeArray(DREAM3D::FeatureData::NeighborList).get());
  }

  if(NULL == m->getAttributeMatrix(m_CellFeatureAttributeMatrixName)->getAttributeArray(m_SharedSurfaceAreaListArrayName).get())
  {

    QString ss = QObject::tr("'SharedSurfaceAreaLists' are not available and are required for this filter to run. A filter that generates 'Shared SurfaceArea Lists' needs to be placed before this filter in the pipeline.");
    setErrorCondition(-306);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }
  else
  {
    m_SharedSurfaceAreaList = NeighborList<float>::SafeObjectDownCast<IDataArray*, NeighborList<float>*>(m->getAttributeMatrix(m_CellFeatureAttributeMatrixName)->getAttributeArray(DREAM3D::FeatureData::SharedSurfaceAreaList).get());
  }

  if(NULL == m->getAttributeMatrix(m_CellEnsembleAttributeMatrixName)->getAttributeArray(m_StatsDataArrayName).get())
  {

    QString ss = QObject::tr("'Ensemble Statistics' are not available and are required for this filter to run. A filter that generates 'Shared SurfaceArea Lists' needs to be placed before this filter in the pipeline.");
    setErrorCondition(-310);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }
  else
  {
    m_StatsDataArray = StatsDataArray::SafeObjectDownCast<IDataArray*, StatsDataArray*>(m->getAttributeMatrix(m_CellEnsembleAttributeMatrixName)->getAttributeArray(m_StatsDataArrayName).get());
  }
  // Ensemble Data
  typedef DataArray<unsigned int> XTalStructArrayType;
  typedef DataArray<unsigned int> PhaseTypeArrayType;
  dims[0] = 1;
  QList<QString> check = cellEnsembleAttrMat->getAttributeArrayNameList();
  for(QList<QString>::iterator it = check.begin(); it != check.end(); ++it)
  {
    QString name = *(it);
    int stop = 0;
  }
  m_CrystalStructuresPtr = cellEnsembleAttrMat->getPrereqArray<DataArray<unsigned int>, AbstractFilter>(this,  m_CrystalStructuresArrayName, -301, dims);
  if( NULL != m_CrystalStructuresPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  {m_CrystalStructures = m_CrystalStructuresPtr.lock()->getPointer(0);}
  m_PhaseTypesPtr = cellEnsembleAttrMat->getPrereqArray<DataArray<unsigned int>, AbstractFilter>(this,  m_PhaseTypesArrayName, -301, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_PhaseTypesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_PhaseTypes = m_PhaseTypesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  m_NumFeaturesPtr = cellEnsembleAttrMat->createNonPrereqArray<DataArray<int32_t>, AbstractFilter, int32_t>(this,  m_NumFeaturesArrayName, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_NumFeaturesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_NumFeatures = m_NumFeaturesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MatchCrystallography::preflight()
{
  emit preflightAboutToExecute();
  emit updateFilterParameters(this);
  dataCheck();
  emit preflightExecuted();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MatchCrystallography::execute()
{
  int err = 0;
  setErrorCondition(err);

  dataCheck();
  if(getErrorCondition() < 0) { return; }

  VolumeDataContainer* m = getDataContainerArray()->getPrereqDataContainer<VolumeDataContainer, AbstractFilter>(this, getDataContainerName(), false);

  int64_t totalEnsembles = m->getAttributeMatrix(m_CellEnsembleAttributeMatrixName)->getNumTuples();

  QString ss;
  ss = QObject::tr("Determining Volumes");
  notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);
  determine_volumes();

  ss = QObject::tr("Determining Boundary Areas");
  notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);
  determine_boundary_areas();

  for (int64_t i = 1; i < totalEnsembles; ++i)
  {
    if(m_PhaseTypes[i] == DREAM3D::PhaseType::PrimaryPhase ||  m_PhaseTypes[i] == DREAM3D::PhaseType::PrecipitatePhase)
    {
      ss = QObject::tr("Initializing Arrays of Phase %1").arg(i);
      notifyStatusMessage(getHumanLabel(), "Initializing Arrays");
      initializeArrays(i);

      ss = QObject::tr("Assigning Eulers to Phase %1").arg(i);
      notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);
      assign_eulers(i);

      ss = QObject::tr("Measuring Misorientations of Phase %1").arg(i);
      notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);
      measure_misorientations(i);

      ss = QObject::tr("Matching Crystallography of Phase %1").arg(i);
      notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);
      matchCrystallography(i);
    }
  }

  // If there is an error set this to something negative and also set a message
  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MatchCrystallography::initializeArrays(int ensem)
{
  // VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

  StatsDataArray& statsDataArray = *m_StatsDataArray;

  // size_t size = m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->getNumTuples();

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
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  simodf = FloatArrayType::CreateArray(actualodf->getSize(), DREAM3D::StringConstants::ODF);
  simmdf = FloatArrayType::CreateArray(actualmdf->getSize(), DREAM3D::StringConstants::MisorientationBins);
  for (size_t j = 0; j < simodf->getSize(); j++)
  {
    simodf->setValue(j, 0.0);
  }
  for (size_t j = 0; j < simmdf->getSize(); j++)
  {
    simmdf->setValue(j, 0.0);
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
      OrientationMath::EulertoQuat(synea1, synea2, synea3, q);
      QuaternionMathF::Copy(q, avgQuats[i]);
      if(m_SurfaceFeatures[i] == false)
      {
        simodf->setValue(choose, (simodf->getValue(choose) + m_Volumes[i] / unbiasedvol[ensem]));
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
    float density = actualodf->getValue(j);
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
              + (((actualmdf->getValue(curmisobin) - simmdf->getValue(curmisobin)) * (actualmdf->getValue(curmisobin) - simmdf->getValue(curmisobin)))
                 - ((actualmdf->getValue(curmisobin) - (simmdf->getValue(curmisobin) - (neighsurfarea / m_TotalSurfaceArea[ensem])))
                    * (actualmdf->getValue(curmisobin) - (simmdf->getValue(curmisobin) - (neighsurfarea / m_TotalSurfaceArea[ensem])))));
  mdfchange = mdfchange
              + (((actualmdf->getValue(newmisobin) - simmdf->getValue(newmisobin)) * (actualmdf->getValue(newmisobin) - simmdf->getValue(newmisobin)))
                 - ((actualmdf->getValue(newmisobin) - (simmdf->getValue(newmisobin) + (neighsurfarea / m_TotalSurfaceArea[ensem])))
                    * (actualmdf->getValue(newmisobin) - (simmdf->getValue(newmisobin) + (neighsurfarea / m_TotalSurfaceArea[ensem])))));
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
  simmdf->setValue(curmisobin, (simmdf->getValue(curmisobin) - (neighsurfarea / m_TotalSurfaceArea[ensem])));
  simmdf->setValue(newmisobin, (simmdf->getValue(newmisobin) + (neighsurfarea / m_TotalSurfaceArea[ensem])));
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
    //      notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);
    currentodferror = 0;
    currentmdferror = 0;
    for (int i = 0; i < numbins; i++)
    {
      currentodferror = currentodferror + ((actualodf->getValue(i) - simodf->getValue(i)) * (actualodf->getValue(i) - simodf->getValue(i)));
    }
    for (int i = 0; i < (numbins); i++)
    {
      currentmdferror = currentmdferror + ((actualmdf->getValue(i) - simmdf->getValue(i)) * (actualmdf->getValue(i) - simmdf->getValue(i)));
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
        OrientationMath::EulertoRod( ea1, ea2, ea3, r1, r2, r3);
        g1odfbin = m_OrientationOps[m_CrystalStructures[ensem]]->getOdfBin(r1, r2, r3);
        random = static_cast<float>( rg.genrand_res53() );
        int choose = 0;

        choose = pick_euler(random, numbins);

        m_OrientationOps[m_CrystalStructures[ensem]]->determineEulerAngles(choose, g1ea1, g1ea2, g1ea3);
        OrientationMath::EulertoQuat(g1ea1, g1ea2, g1ea3, q1);

        odfchange = ((actualodf->getValue(choose) - simodf->getValue(choose)) * (actualodf->getValue(choose) - simodf->getValue(choose)))
                    - ((actualodf->getValue(choose) - (simodf->getValue(choose) + (m_Volumes[selectedfeature1] / unbiasedvol[ensem])))
                       * (actualodf->getValue(choose) - (simodf->getValue(choose) + (m_Volumes[selectedfeature1] / unbiasedvol[ensem]))));
        odfchange = odfchange
                    + (((actualodf->getValue(g1odfbin) - simodf->getValue(g1odfbin)) * (actualodf->getValue(g1odfbin) - simodf->getValue(g1odfbin)))
                       - ((actualodf->getValue(g1odfbin) - (simodf->getValue(g1odfbin) - (m_Volumes[selectedfeature1] / unbiasedvol[ensem])))
                          * (actualodf->getValue(g1odfbin) - (simodf->getValue(g1odfbin) - (m_Volumes[selectedfeature1] / unbiasedvol[ensem])))));

        mdfchange = 0;
        size_t size = 0;
        if(neighborlist[selectedfeature1].size() != 0) { size = neighborlist[selectedfeature1].size(); }
        for (size_t j = 0; j < size; j++)
        {
          int neighbor = neighborlist[selectedfeature1][j];
          ea1 = m_FeatureEulerAngles[3 * neighbor];
          ea2 = m_FeatureEulerAngles[3 * neighbor + 1];
          ea3 = m_FeatureEulerAngles[3 * neighbor + 2];
          OrientationMath::EulertoQuat(ea1, ea2, ea3, q2);
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
          simodf->setValue(choose, (simodf->getValue(choose) + (m_Volumes[selectedfeature1] / unbiasedvol[ensem])));
          simodf->setValue(g1odfbin, (simodf->getValue(g1odfbin) - (m_Volumes[selectedfeature1] / unbiasedvol[ensem])));
          size = 0;
          if(neighborlist[selectedfeature1].size() != 0) { size = neighborlist[selectedfeature1].size(); }
          for (size_t j = 0; j < size; j++)
          {
            int neighbor = neighborlist[selectedfeature1][j];
            ea1 = m_FeatureEulerAngles[3 * neighbor];
            ea2 = m_FeatureEulerAngles[3 * neighbor + 1];
            ea3 = m_FeatureEulerAngles[3 * neighbor + 2];
            OrientationMath::EulertoQuat(ea1, ea2, ea3, q2);
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
          OrientationMath::EulertoRod(g1ea1, g1ea2, g1ea3, r1, r2, r3);
          g1odfbin = m_OrientationOps[m_CrystalStructures[ensem]]->getOdfBin(r1, r2, r3);
          QuaternionMathF::Copy(q1, avgQuats[selectedfeature2]);
          OrientationMath::EulertoRod(g2ea1, g2ea2, g2ea3, r1, r2, r3);
          g2odfbin = m_OrientationOps[m_CrystalStructures[ensem]]->getOdfBin(r1, r2, r3);

          odfchange = ((actualodf->getValue(g1odfbin) - simodf->getValue(g1odfbin)) * (actualodf->getValue(g1odfbin) - simodf->getValue(g1odfbin)))
                      - ((actualodf->getValue(g1odfbin)
                          - (simodf->getValue(g1odfbin) - (m_Volumes[selectedfeature1] / unbiasedvol[ensem])
                             + (m_Volumes[selectedfeature2] / unbiasedvol[ensem])))
                         * (actualodf->getValue(g1odfbin)
                            - (simodf->getValue(g1odfbin) - (m_Volumes[selectedfeature1] / unbiasedvol[ensem])
                               + (m_Volumes[selectedfeature2] / unbiasedvol[ensem]))));
          odfchange = odfchange
                      + (((actualodf->getValue(g2odfbin) - simodf->getValue(g2odfbin)) * (actualodf->getValue(g2odfbin) - simodf->getValue(g2odfbin)))
                         - ((actualodf->getValue(g2odfbin)
                             - (simodf->getValue(g2odfbin) - (m_Volumes[selectedfeature2] / unbiasedvol[ensem])
                                + (m_Volumes[selectedfeature1] / unbiasedvol[ensem])))
                            * (actualodf->getValue(g2odfbin)
                               - (simodf->getValue(g2odfbin) - (m_Volumes[selectedfeature2] / unbiasedvol[ensem])
                                  + (m_Volumes[selectedfeature1] / unbiasedvol[ensem])))));

          mdfchange = 0;
          OrientationMath::EulertoQuat(g2ea1, g2ea2, g2ea3, q1);
          size_t size = 0;
          if(neighborlist[selectedfeature1].size() != 0) { size = neighborlist[selectedfeature1].size(); }
          for (size_t j = 0; j < size; j++)
          {
            int neighbor = neighborlist[selectedfeature1][j];
            ea1 = m_FeatureEulerAngles[3 * neighbor];
            ea2 = m_FeatureEulerAngles[3 * neighbor + 1];
            ea3 = m_FeatureEulerAngles[3 * neighbor + 2];
            OrientationMath::EulertoQuat(ea1, ea2, ea3, q2);
            float neighsurfarea = neighborsurfacearealist[selectedfeature1][j];
            if(neighbor != static_cast<int>(selectedfeature2) )
            {
              MC_LoopBody1(selectedfeature1, ensem, j, neighsurfarea, m_CrystalStructures[ensem], q1, q2);
            }
          }

          OrientationMath::EulertoQuat(g1ea1, g1ea2, g1ea3, q1);
          size = 0;
          if(neighborlist[selectedfeature2].size() != 0) { size = neighborlist[selectedfeature2].size(); }
          for (size_t j = 0; j < size; j++)
          {
            size_t neighbor = neighborlist[selectedfeature2][j];
            ea1 = m_FeatureEulerAngles[3 * neighbor];
            ea2 = m_FeatureEulerAngles[3 * neighbor + 1];
            ea3 = m_FeatureEulerAngles[3 * neighbor + 2];
            OrientationMath::EulertoQuat(ea1, ea2, ea3, q2);
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
            simodf->setValue(g1odfbin, (simodf->getValue(g1odfbin) + (m_Volumes[selectedfeature2] / unbiasedvol[ensem])
                                        - (m_Volumes[selectedfeature1] / unbiasedvol[ensem])));
            simodf->setValue(g2odfbin, (simodf->getValue(g2odfbin) + (m_Volumes[selectedfeature1] / unbiasedvol[ensem])
                                        - (m_Volumes[selectedfeature2] / unbiasedvol[ensem])));

            OrientationMath::EulertoQuat(g2ea1, g2ea2, g2ea3, q1);
            QuaternionMathF::Copy(avgQuats[selectedfeature1], q1);
            size = 0;
            if(neighborlist[selectedfeature1].size() != 0) { size = neighborlist[selectedfeature1].size(); }
            for (size_t j = 0; j < size; j++)
            {
              size_t neighbor = neighborlist[selectedfeature1][j];
              ea1 = m_FeatureEulerAngles[3 * neighbor];
              ea2 = m_FeatureEulerAngles[3 * neighbor + 1];
              ea3 = m_FeatureEulerAngles[3 * neighbor + 2];
              OrientationMath::EulertoQuat(ea1, ea2, ea3, q2);
              float neighsurfarea = neighborsurfacearealist[selectedfeature1][j];
              if(neighbor != selectedfeature2)
              {
                MC_LoopBody2(selectedfeature1, ensem, j, neighsurfarea, m_CrystalStructures[ensem], q1, q2);
              }
            }

            OrientationMath::EulertoQuat(g1ea1, g1ea2, g1ea3, q1);
            QuaternionMathF::Copy(q1, avgQuats[selectedfeature2]);
            size = 0;
            if(neighborlist[selectedfeature2].size() != 0) { size = neighborlist[selectedfeature2].size(); }
            for (size_t j = 0; j < size; j++)
            {
              size_t neighbor = neighborlist[selectedfeature2][j];
              ea1 = m_FeatureEulerAngles[3 * neighbor];
              ea2 = m_FeatureEulerAngles[3 * neighbor + 1];
              ea3 = m_FeatureEulerAngles[3 * neighbor + 2];
              OrientationMath::EulertoQuat(ea1, ea2, ea3, q2);
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
            simmdf->setValue(mbin, (simmdf->getValue(mbin) + (neighsurfarea / m_TotalSurfaceArea[m_FeaturePhases[i]])));
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

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer MatchCrystallography::newFilterInstance(bool copyFilterParameters)
{
  /*
  * MaxIterations
  */
  MatchCrystallography::Pointer filter = MatchCrystallography::New();
  if(true == copyFilterParameters)
  {
    filter->setMaxIterations( getMaxIterations() );
  }
  return filter;
}
