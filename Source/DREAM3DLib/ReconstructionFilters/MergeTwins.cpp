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

#include "MergeTwins.h"

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/variate_generator.hpp>

#include "DREAM3DLib/Common/Constants.h"

#include "DREAM3DLib/OrientationOps/OrientationOps.h"
#include "DREAM3DLib/Utilities/DREAM3DRandom.h"

#include "DREAM3DLib/StatisticsFilters/FindNeighbors.h"
#include "DREAM3DLib/GenericFilters/FindFeaturePhases.h"
#include "DREAM3DLib/StatisticsFilters/FindAvgOrientations.h"


#define ERROR_TXT_OUT 1
#define ERROR_TXT_OUT1 1




#define NEW_SHARED_ARRAY(var, m_msgType, size)\
  boost::shared_array<m_msgType> var##Array(new m_msgType[size]);\
  m_msgType* var = var##Array.get();

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MergeTwins::MergeTwins() :
  AbstractFilter(),
  m_DataContainerName(DREAM3D::Defaults::VolumeDataContainerName),
  m_CellAttributeMatrixName(DREAM3D::Defaults::CellAttributeMatrixName),
  m_CellFeatureAttributeMatrixName(DREAM3D::Defaults::CellFeatureAttributeMatrixName),
  m_CellEnsembleAttributeMatrixName(DREAM3D::Defaults::CellEnsembleAttributeMatrixName),
  m_FeatureIdsArrayName(DREAM3D::CellData::FeatureIds),
  m_CellParentIdsArrayName(DREAM3D::CellData::ParentIds),
  m_AvgQuatsArrayName(DREAM3D::FeatureData::AvgQuats),
  m_FeaturePhasesArrayName(DREAM3D::FeatureData::Phases),
  m_ActiveArrayName(DREAM3D::FeatureData::Active),
  m_FeatureParentIdsArrayName(DREAM3D::FeatureData::ParentIds),
  m_CrystalStructuresArrayName(DREAM3D::EnsembleData::CrystalStructures),
  m_AxisTolerance(1.0f),
  m_AngleTolerance(1.0f),
  m_RandomizeParentIds(true),
  m_FeatureIds(NULL),
  m_CellParentIds(NULL),
  m_FeatureParentIds(NULL),
  m_AvgQuats(NULL),
  m_Active(NULL),
  m_FeaturePhases(NULL),
  m_NeighborList(NULL),
  m_CrystalStructures(NULL)
{
  m_OrientationOps = OrientationOps::getOrientationOpsVector();

  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MergeTwins::~MergeTwins()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MergeTwins::setupFilterParameters()
{
  FilterParameterVector parameters;
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Axis Tolerance");
    option->setPropertyName("AxisTolerance");
    option->setWidgetType(FilterParameter::DoubleWidget);
    option->setValueType("float");
    option->setCastableValueType("double");
    option->setUnits("Degrees");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Angle Tolerance");
    option->setPropertyName("AngleTolerance");
    option->setWidgetType(FilterParameter::DoubleWidget);
    option->setValueType("float");
    option->setCastableValueType("double");
    option->setUnits("Degrees");
    parameters.push_back(option);
  }

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MergeTwins::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  /* Code to read the values goes between these statements */
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE BEGIN*/
  setAxisTolerance( reader->readValue("AxisTolerance", getAxisTolerance()) );
  setAngleTolerance( reader->readValue("AngleTolerance", getAngleTolerance()) );
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE END*/
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int MergeTwins::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("AxisTolerance", getAxisTolerance() );
  writer->writeValue("AngleTolerance", getAngleTolerance() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MergeTwins::dataCheck(bool preflight, size_t voxels, size_t features, size_t ensembles)
{
  setErrorCondition(0);

  VolumeDataContainer* m = getDataContainerArray()->getPrereqDataContainer<VolumeDataContainer, AbstractFilter>(getDataContainerName(), false, this);
  if(getErrorCondition() < 0) { return; }

  QVector<int> dims(1, 1);
  // Cell Data
  m_FeatureIdsPtr = m->getPrereqArray<int32_t, AbstractFilter>(this, m_CellAttributeMatrixName,  m_FeatureIdsArrayName, -301, voxels, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeatureIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  m_CellParentIdsPtr = m->createNonPrereqArray<int32_t, AbstractFilter>(this, m_CellAttributeMatrixName,  m_CellParentIdsArrayName, -1, voxels, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CellParentIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_CellParentIds = m_CellParentIdsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  // Feature Data
  m_FeaturePhasesPtr = m->getPrereqArray<int32_t, AbstractFilter>(this, m_CellFeatureAttributeMatrixName,  m_FeaturePhasesArrayName, -303, features, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeaturePhasesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeaturePhases = m_FeaturePhasesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  m_ActivePtr = m->createNonPrereqArray<bool, AbstractFilter>(this, m_CellFeatureAttributeMatrixName,  m_ActiveArrayName, true, features, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_ActivePtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_Active = m_ActivePtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  m_FeatureParentIdsPtr = m->createNonPrereqArray<int32_t, AbstractFilter>(this, m_CellFeatureAttributeMatrixName,  m_FeatureParentIdsArrayName, 0, features, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeatureParentIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeatureParentIds = m_FeatureParentIdsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  dims[0] = 4;
  m_AvgQuatsPtr = m->getPrereqArray<float, AbstractFilter>(this, m_CellFeatureAttributeMatrixName,  m_AvgQuatsArrayName, -301, features, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_AvgQuatsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_AvgQuats = m_AvgQuatsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  // Now we are going to get a "Pointer" to the NeighborList object out of the DataContainer
  m_NeighborList = NeighborList<int>::SafeObjectDownCast<IDataArray*, NeighborList<int>*>(m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->getAttributeArray(DREAM3D::FeatureData::NeighborList).get());
  if(m_NeighborList == NULL)
  {
    QString ss = QObject::tr("NeighborLists Array Not Initialized Correctly");
    setErrorCondition(-304);
    addErrorMessage(getHumanLabel(), ss, -304);
  }

  // Ensemble Data
  dims[0] = 1;
  typedef DataArray<unsigned int> XTalStructArrayType;
  m_CrystalStructuresPtr = m->getPrereqArray<unsigned int, AbstractFilter>(this, m_CellEnsembleAttributeMatrixName,  m_CrystalStructuresArrayName, -305, ensembles, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CrystalStructuresPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_CrystalStructures = m_CrystalStructuresPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MergeTwins::preflight()
{

  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MergeTwins::execute()
{
  VolumeDataContainer* m = getDataContainerArray()->getPrereqDataContainer<VolumeDataContainer, AbstractFilter>(getDataContainerName(), false, this);
  if(getErrorCondition() < 0) { return; }

  setErrorCondition(0);

  int64_t totalPoints = m->getAttributeMatrix(getCellAttributeMatrixName())->getNumTuples();
  size_t totalFeatures = m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->getNumTuples();
  size_t totalEnsembles = m->getAttributeMatrix(getCellEnsembleAttributeMatrixName())->getNumTuples();
  dataCheck(false, totalPoints, totalFeatures, totalEnsembles);
  if (getErrorCondition() < 0)
  {
    return;
  }

  notifyStatusMessage("Merging Twins");
  merge_twins();

  notifyStatusMessage("Characterizing Twins");
  characterize_twins();

  if (true == m_RandomizeParentIds)
  {
    int64_t totalPoints = m->getTotalPoints();

    // Generate all the numbers up front
    const int rangeMin = 1;
    const int rangeMax = numParents - 1;
    typedef boost::uniform_int<int> NumberDistribution;
    typedef boost::mt19937 RandomNumberGenerator;
    typedef boost::variate_generator < RandomNumberGenerator&,
            NumberDistribution > Generator;

    NumberDistribution distribution(rangeMin, rangeMax);
    RandomNumberGenerator generator;
    Generator numberGenerator(generator, distribution);

    generator.seed(static_cast<boost::uint32_t>( QDateTime::currentMSecsSinceEpoch() )); // seed with the current time


    DataArray<int32_t>::Pointer rndNumbers = DataArray<int32_t>::CreateArray(numParents, "New ParentIds");
    int32_t* pid = rndNumbers->getPointer(0);
    pid[0] = 0;
    QSet<int32_t> parentIdSet;
    parentIdSet.insert(0);
    for(int i = 1; i < numParents; ++i)
    {
      pid[i] = i; //numberGenerator();
      parentIdSet.insert(pid[i]);
    }

    int r;
    size_t temp;
    //--- Shuffle elements by randomly exchanging each with one other.
    for (int i = 1; i < numParents; i++)
    {
      r = numberGenerator(); // Random remaining position.
      if (r >= numParents)
      {
        continue;
      }
      temp = pid[i];
      pid[i] = pid[r];
      pid[r] = temp;
    }

    // Now adjust all the Feature Id values for each Voxel
    for(int64_t i = 0; i < totalPoints; ++i)
    {
      m_CellParentIds[i] = pid[ m_CellParentIds[i] ];
      m_FeatureParentIds[m_FeatureIds[i]] = m_CellParentIds[i];
    }
  }

  notifyStatusMessage("Completed");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MergeTwins::merge_twins()
{
  // Since this method is called from the 'execute' and the DataContainer validity
  // was checked there we are just going to get the Shared Pointer to the DataContainer
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

  // Now we are going to get a "Pointer" to the NeighborList object out of the DataContainer
  m_NeighborList = NeighborList<int>::SafeObjectDownCast<IDataArray*, NeighborList<int>* >(m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->getAttributeArray(DREAM3D::FeatureData::NeighborList).get());
  // But since a pointer is difficult to use operators with we will now create a
  // reference variable to the pointer with the correct variable name that allows
  // us to use the same syntax as the "vector of vectors"
  NeighborList<int>& neighborlist = *m_NeighborList;

  QVector<int> twinlist;
  float w;
  float n1, n2, n3;
  float angtol = m_AngleTolerance;
  float axistol = static_cast<float>( m_AxisTolerance * M_PI / 180.0f );
  QuatF q1;
  QuatF q2;
  QuatF* avgQuats = reinterpret_cast<QuatF*>(m_AvgQuats);

  size_t numfeatures = m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->getNumTuples();
  unsigned int phase1, phase2;
  int parentcount = 0;
  m_ParentNumbers.clear();
  m_ParentNumbers.fill(-1, numfeatures);

  m_ParentNumbers[0] = 0;
  for (size_t i = 1; i < numfeatures; i++)
  {
    if (m_ParentNumbers[i] == -1 && m_FeaturePhases[i] > 0)
    {
      parentcount++;
      m_ParentNumbers[i] = parentcount;
      m_Active[i] = true;
      twinlist.push_back(i);
      for (size_t j = 0; j < twinlist.size(); j++)
      {
        int firstfeature = twinlist[j];
        int size = int(neighborlist[firstfeature].size());
        for (int l = 0; l < size; l++)
        {
          int twin = 0;
          size_t neigh = neighborlist[firstfeature][l];
          if (neigh != i && m_ParentNumbers[neigh] == -1 && m_FeaturePhases[neigh] > 0)
          {
            //w = 10000.0f;
            QuaternionMathF::Copy(avgQuats[firstfeature], q1);
            phase1 = m_CrystalStructures[m_FeaturePhases[firstfeature]];
            QuaternionMathF::Copy(avgQuats[neigh], q2);
            phase2 = m_CrystalStructures[m_FeaturePhases[neigh]];
            if (phase1 == phase2 && (phase1 == Ebsd::CrystalStructure::Cubic_High))
            {
              w = m_OrientationOps[phase1]->getMisoQuat( q1, q2, n1, n2, n3);
              w = w * (180.0f / DREAM3D::Constants::k_Pi);
              float axisdiff111 = acosf(fabs(n1) * 0.57735f + fabs(n2) * 0.57735f + fabs(n3) * 0.57735f);
              float angdiff60 = fabs(w - 60.0f);
              if (axisdiff111 < axistol && angdiff60 < angtol) { twin = 1; }
              if (twin == 1)
              {
                m_ParentNumbers[neigh] = parentcount;
                twinlist.push_back(neigh);
              }
            }
          }
        }
      }
    }
    twinlist.clear();
  }
  size_t totalPoints = static_cast<size_t>(m->getTotalPoints());
  for (size_t k = 0; k < totalPoints; k++)
  {
    int featurename = m_FeatureIds[k];
    m_CellParentIds[k] = m_ParentNumbers[featurename];
  }
  numParents = parentcount + 1;
}

void MergeTwins::characterize_twins()
{
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

  size_t numfeatures = m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->getNumTuples();
  for (size_t i = 0; i < numfeatures; i++)
  {

  }
}

