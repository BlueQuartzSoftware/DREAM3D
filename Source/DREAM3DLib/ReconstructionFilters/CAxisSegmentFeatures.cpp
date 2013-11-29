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

#include "CAxisSegmentFeatures.h"

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/variate_generator.hpp>

#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Math/DREAM3DMath.h"
#include "DREAM3DLib/Math/MatrixMath.h"
#include "DREAM3DLib/Math/OrientationMath.h"
#include "DREAM3DLib/OrientationOps/OrientationOps.h"
#include "DREAM3DLib/Utilities/DREAM3DRandom.h"

#include "DREAM3DLib/GenericFilters/FindCellQuats.h"

#define ERROR_TXT_OUT 1
#define ERROR_TXT_OUT1 1




#define NEW_SHARED_ARRAY(var, m_msgType, size)\
  boost::shared_array<m_msgType> var##Array(new m_msgType[size]);\
  m_msgType* var = var##Array.get();



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CAxisSegmentFeatures::CAxisSegmentFeatures() :
  SegmentFeatures(),
  m_DataContainerName(DREAM3D::Defaults::VolumeDataContainerName),
  m_CellAttributeMatrixName(DREAM3D::Defaults::CellAttributeMatrixName),
  m_CellFeatureAttributeMatrixName(DREAM3D::Defaults::CellFeatureAttributeMatrixName),
  m_CellEnsembleAttributeMatrixName(DREAM3D::Defaults::CellEnsembleAttributeMatrixName),
  m_GoodVoxelsArrayName(DREAM3D::CellData::GoodVoxels),
  m_CellPhasesArrayName(DREAM3D::CellData::Phases),
  m_QuatsArrayName(DREAM3D::CellData::Quats),
  m_FeatureIdsArrayName(DREAM3D::CellData::FeatureIds),
  m_ActiveArrayName(DREAM3D::FeatureData::Active),
  m_CrystalStructuresArrayName(DREAM3D::EnsembleData::CrystalStructures),
  m_MisorientationTolerance(5.0f),
  m_RandomizeFeatureIds(true),
  m_FeatureIds(NULL),
  m_Quats(NULL),
  m_CellPhases(NULL),
  m_GoodVoxels(NULL),
  m_Active(NULL),
  m_CrystalStructures(NULL)
{
  m_OrientationOps = OrientationOps::getOrientationOpsVector();

  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CAxisSegmentFeatures::~CAxisSegmentFeatures()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------

void CAxisSegmentFeatures::setupFilterParameters()
{
  FilterParameterVector parameters;
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setPropertyName("MisorientationTolerance");
    option->setHumanLabel("C-Axis Misorientation Tolerance");
    option->setWidgetType(FilterParameter::DoubleWidget);
    option->setValueType("float");
    option->setCastableValueType("double");
    option->setUnits("Degrees");
    parameters.push_back(option);
  }
#if 0
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Randomly Reorder Generated Feature Ids");
    option->setPropertyName("RandomizeFeatureIds");
    option->setWidgetType(FilterParameter::BooleanWidget);
    option->setValueType("bool");
    parameters.push_back(option);
  }
#endif
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CAxisSegmentFeatures::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  /* Code to read the values goes between these statements */
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE BEGIN*/
  setMisorientationTolerance( reader->readValue("MisorientationTolerance", getMisorientationTolerance()) );
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE END*/
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int CAxisSegmentFeatures::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("MisorientationTolerance", getMisorientationTolerance() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CAxisSegmentFeatures::dataCheck(bool preflight, size_t voxels, size_t features, size_t ensembles)
{
  setErrorCondition(0);
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

  QVector<int> dims(1 , 1);
  m_GoodVoxelsPtr = m->createNonPrereqArray<bool, AbstractFilter>(this, m_CellAttributeMatrixName,  m_GoodVoxelsArrayName,  true, voxels, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_GoodVoxelsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_GoodVoxels = m_GoodVoxelsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  m_CellPhasesPtr = m->getPrereqArray<int32_t, AbstractFilter>(this, m_CellAttributeMatrixName,  m_CellPhasesArrayName, -302,  voxels, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CellPhasesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_CellPhases = m_CellPhasesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  m_FeatureIdsPtr = m->createNonPrereqArray<int32_t, AbstractFilter>(this, m_CellAttributeMatrixName,  m_FeatureIdsArrayName, 0, voxels, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeatureIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  m_ActivePtr = m->createNonPrereqArray<bool, AbstractFilter>(this, m_CellFeatureAttributeMatrixName,  m_ActiveArrayName, true, features, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_ActivePtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_Active = m_ActivePtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  typedef DataArray<unsigned int> XTalStructArrayType;
  m_CrystalStructuresPtr = m->getPrereqArray<unsigned int, AbstractFilter>(this, m_CellEnsembleAttributeMatrixName,  m_CrystalStructuresArrayName, -304, ensembles, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CrystalStructuresPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_CrystalStructures = m_CrystalStructuresPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  dims[0] = 4;
  m_QuatsPtr = m->getPrereqArray<float, AbstractFilter>(this, m_CellAttributeMatrixName,  m_QuatsArrayName, -303, voxels, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_QuatsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_Quats = m_QuatsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CAxisSegmentFeatures::preflight()
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
void CAxisSegmentFeatures::execute()
{
  setErrorCondition(0);
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The DataContainer Object was NULL", -999);
    return;
  }

  m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->resizeAttributeArrays(1);
  int64_t totalPoints = m->getAttributeMatrix(getCellAttributeMatrixName())->getNumTuples();
  size_t totalFeatures = m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->getNumTuples();
  size_t totalEnsembles = m->getAttributeMatrix(getCellEnsembleAttributeMatrixName())->getNumTuples();
  dataCheck(false, totalPoints, totalFeatures, totalEnsembles);
  if (getErrorCondition() < 0)
  {
    return;
  }

  //Convert user defined tolerance to radians.
  m_MisorientationTolerance = m_MisorientationTolerance * DREAM3D::Constants::k_Pi / 180.0f;
  for(int64_t i = 0; i < totalPoints; i++)
  {
    m_FeatureIds[i] = 0;
  }

  SegmentFeatures::execute();

  if (true == m_RandomizeFeatureIds)
  {

    totalPoints = m->getAttributeMatrix(getCellAttributeMatrixName())->getNumTuples();
    totalFeatures = m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->getNumTuples();
    totalEnsembles = m->getAttributeMatrix(getCellEnsembleAttributeMatrixName())->getNumTuples();
    dataCheck(false, totalPoints, totalFeatures, totalEnsembles);
    if (getErrorCondition() < 0)
    {
      return;
    }

    // Generate all the numbers up front
    const int rangeMin = 1;
    const int rangeMax = totalFeatures - 1;
    typedef boost::uniform_int<int> NumberDistribution;
    typedef boost::mt19937 RandomNumberGenerator;
    typedef boost::variate_generator < RandomNumberGenerator&,
            NumberDistribution > Generator;

    NumberDistribution distribution(rangeMin, rangeMax);
    RandomNumberGenerator generator;
    Generator numberGenerator(generator, distribution);
    generator.seed(static_cast<boost::uint32_t>( QDateTime::currentMSecsSinceEpoch() )); // seed with the current time

    DataArray<int32_t>::Pointer rndNumbers = DataArray<int32_t>::CreateArray(totalFeatures, "New FeatureIds");
    int32_t* gid = rndNumbers->getPointer(0);
    gid[0] = 0;
    QSet<int32_t> featureIdSet;
    featureIdSet.insert(0);
    for(size_t i = 1; i < totalFeatures; ++i)
    {
      gid[i] = i; //numberGenerator();
      featureIdSet.insert(gid[i]);
    }

    size_t r;
    size_t temp;
    //--- Shuffle elements by randomly exchanging each with one other.
    for (size_t i = 1; i < totalFeatures; i++)
    {
      r = numberGenerator(); // Random remaining position.

      temp = gid[i];
      gid[i] = gid[r];
      gid[r] = temp;
    }

    // Now adjust all the Feature Id values for each Voxel
    for(int64_t i = 0; i < totalPoints; ++i)
    {
      m_FeatureIds[i] = gid[ m_FeatureIds[i] ];
    }
    for(size_t i = 0; i < totalFeatures; i++)
    {
      m_Active[i] = true;
    }
  }

  // If there is an error set this to something negative and also set a message
  notifyStatusMessage("Completed");
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int64_t CAxisSegmentFeatures::getSeed(size_t gnum)
{
  setErrorCondition(0);
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

  int64_t totalPoints = m->getTotalPoints();

  DREAM3D_RANDOMNG_NEW()
  int64_t seed = -1;
  int64_t randpoint = 0;

  // Precalculate some constants
  int64_t totalPMinus1 = totalPoints - 1;

  int64_t counter = 0;
  randpoint = int64_t(float(rg.genrand_res53()) * float(totalPMinus1));
  while (seed == -1 && counter < totalPoints)
  {
    if (randpoint > totalPMinus1) { randpoint = static_cast<int64_t>( randpoint - totalPoints ); }
    if (m_GoodVoxels[randpoint] == true && m_FeatureIds[randpoint] == 0 && m_CellPhases[randpoint] > 0) { seed = randpoint; }
    randpoint++;
    counter++;
  }
  if (seed >= 0)
  {
    m_FeatureIds[seed] = gnum;
    m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->resizeAttributeArrays(gnum + 1);
    int64_t totalPoints = m->getAttributeMatrix(getCellAttributeMatrixName())->getNumTuples();
    size_t totalFeatures = m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->getNumTuples();
    size_t totalEnsembles = m->getAttributeMatrix(getCellEnsembleAttributeMatrixName())->getNumTuples();
    dataCheck(false, totalPoints, totalFeatures, totalEnsembles);
  }
  return seed;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool CAxisSegmentFeatures::determineGrouping(int64_t referencepoint, int64_t neighborpoint, size_t gnum)
{
  bool group = false;
  float w = 10000.0;
  QuatF q1;
  QuatF q2;
  QuatF* quats = reinterpret_cast<QuatF*>(m_Quats);
  float g1[3][3];
  float g2[3][3];
  float g1t[3][3];
  float g2t[3][3];
// float n1, n2, n3;
  unsigned int phase1, phase2;
  float caxis[3] = {0, 0, 1};
  float c1[3];
  float c2[3];

  if(m_FeatureIds[neighborpoint] == 0 && m_GoodVoxels[neighborpoint] == true)
  {
    phase1 = m_CrystalStructures[m_CellPhases[referencepoint]];
    QuaternionMathF::Copy(quats[referencepoint], q1);

    phase2 = m_CrystalStructures[m_CellPhases[neighborpoint]];
    QuaternionMathF::Copy(quats[neighborpoint], q2);

    if (m_CellPhases[referencepoint] == m_CellPhases[neighborpoint])
    {
      OrientationMath::QuattoMat(q1, g1);
      OrientationMath::QuattoMat(q2, g2);

      //transpose the g matricies so when caxis is multiplied by it
      //it will give the sample direction that the caxis is along
      MatrixMath::Transpose3x3(g1, g1t);
      MatrixMath::Transpose3x3(g2, g2t);

      MatrixMath::Multiply3x3with3x1(g1t, caxis, c1);
      MatrixMath::Multiply3x3with3x1(g2t, caxis, c2);

      //normalize so that the dot product can be taken below without
      //dividing by the magnitudes (they would be 1)
      MatrixMath::Normalize3x1(c1);
      MatrixMath::Normalize3x1(c2);

      w = ((c1[0] * c2[0]) + (c1[1] * c2[1]) + (c1[2] * c2[2]));
      w = acosf(w);
      if (w <= m_MisorientationTolerance || (DREAM3D::Constants::k_Pi - w) <= m_MisorientationTolerance)
      {
        group = true;
        m_FeatureIds[neighborpoint] = gnum;
      }

    }
  }

  return group;
}
