/* ============================================================================
 * Copyright (c) 2011 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2011 Dr. Michael A. Groeber (US Air Force Research Laboratories)
 * Copyright (c) 2013 Dr. Joseph C. Tucker (UES, Inc.)
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
 * Neither the name of Joseph C. Tucker, Michael A. Groeber, Michael A. Jackson,
 * UES, Inc., the US Air Force, BlueQuartz Software nor the names of its contributors  
 * may be used to endorse or promote products derived from this software without  
 * specific prior written permission.
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
 *                   FA8650-07-D-5800 and FA8650-10-D-5226
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "MergeColonies.h"

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/variate_generator.hpp>

#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Math/DREAM3DMath.h"
#include "DREAM3DLib/Utilities/DREAM3DRandom.h"
#include "DREAM3DLib/Math/MatrixMath.h"
#include "DREAM3DLib/Math/OrientationMath.h"
#include "DREAM3DLib/OrientationOps/OrientationOps.h"

#include "DREAM3DLib/StatisticsFilters/FindNeighbors.h"
#include "DREAM3DLib/GenericFilters/FindFeaturePhases.h"
#include "DREAM3DLib/StatisticsFilters/FindAvgOrientations.h"

#define ERROR_TXT_OUT 1
#define ERROR_TXT_OUT1 1



static const float unit110 = 1.0 / sqrtf(2.0);
static const float unit111 = 1.0 / sqrtf(3.0);
static const float unit112_1 = 1.0 / sqrtf(6.0);
static const float unit112_2 = 2.0 / sqrtf(6.0);

float crystalDirections[12][3][3] = {{{unit111, unit112_1, unit110},
    { -unit111, -unit112_1, unit110},
    {unit111, -unit112_2, 0}
  },

  { { -unit111, unit112_1, unit110},
    {unit111, -unit112_1, unit110},
    {unit111, unit112_2, 0}
  },

  { {unit111, -unit112_1, unit110},
    {unit111, -unit112_1, -unit110},
    {unit111, unit112_2, 0}
  },

  { {unit111, unit112_1, unit110},
    {unit111, unit112_1, -unit110},
    { -unit111, unit112_2, 0}
  },

  { {unit111, unit112_1, unit110},
    {unit111, -unit112_2, 0},
    {unit111, unit112_1, -unit110}
  },

  { {unit111, -unit112_1, unit110},
    { -unit111, -unit112_2, 0},
    {unit111, -unit112_1, -unit110}
  },

  { {unit111, -unit112_1, unit110},
    {unit111, unit112_2, 0},
    { -unit111, unit112_1, unit110}
  },

  { { -unit111, -unit112_1, unit110},
    {unit111, -unit112_2, 0},
    {unit111, unit112_1, unit110}
  },

  { {unit111, -unit112_2, 0},
    {unit111, unit112_1, unit110},
    { -unit111, -unit112_1, unit110}
  },

  { {unit111, unit112_2, 0},
    { -unit111, unit112_1, unit110},
    {unit111, -unit112_1, unit110}
  },

  { {unit111, unit112_2, 0},
    {unit111, -unit112_1, unit110},
    {unit111, -unit112_1, -unit110}
  },

  { { -unit111, unit112_2, 0},
    {unit111, unit112_1, unit110},
    {unit111, unit112_1, -unit110}
  }
};


#define NEW_SHARED_ARRAY(var, m_msgType, size)\
  boost::shared_array<m_msgType> var##Array(new m_msgType[size]);\
  m_msgType* var = var##Array.get();

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MergeColonies::MergeColonies() :
  AbstractFilter(),
  m_DataContainerName(DREAM3D::Defaults::VolumeDataContainerName),
  m_CellFeatureAttributeMatrixName(DREAM3D::Defaults::CellFeatureAttributeMatrixName),
  m_CellEnsembleAttributeMatrixName(DREAM3D::Defaults::CellEnsembleAttributeMatrixName),
  m_CellAttributeMatrixName(DREAM3D::Defaults::CellAttributeMatrixName),
  m_AxisTolerance(1.0f),
  m_AngleTolerance(1.0f),
  m_RandomizeParentIds(true),
  m_IdentifyGlobAlpha(false),
  m_FeatureIdsArrayName(DREAM3D::CellData::FeatureIds),
  m_FeatureIds(NULL),
  m_CellPhasesArrayName(DREAM3D::CellData::Phases),
  m_CellPhases(NULL),
  m_CellParentIdsArrayName(DREAM3D::CellData::ParentIds),
  m_CellParentIds(NULL),
  m_FeatureParentIdsArrayName(DREAM3D::FeatureData::ParentIds),
  m_FeatureParentIds(NULL),
  m_GlobAlphaArrayName(DREAM3D::CellData::GlobAlpha),
  m_GlobAlpha(NULL),
  m_AvgQuatsArrayName(DREAM3D::FeatureData::AvgQuats),
  m_AvgQuats(NULL),
  m_ActiveArrayName(DREAM3D::FeatureData::Active),
  m_Active(NULL),
  m_FeaturePhasesArrayName(DREAM3D::FeatureData::Phases),
  m_FeaturePhases(NULL),
  m_CrystalStructuresArrayName(DREAM3D::EnsembleData::CrystalStructures),
  m_CrystalStructures(NULL)
{
  m_OrientationOps = OrientationOps::getOrientationOpsVector();

  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MergeColonies::~MergeColonies()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MergeColonies::setupFilterParameters()
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
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Identify Glob Alpha");
    option->setPropertyName("IdentifyGlobAlpha");
    option->setWidgetType(FilterParameter::BooleanWidget);
    option->setValueType("bool");
    parameters.push_back(option);
  }
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MergeColonies::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  /* Code to read the values goes between these statements */
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE BEGIN*/
  setAxisTolerance( reader->readValue("AxisTolerance", getAxisTolerance()) );
  setAngleTolerance( reader->readValue("AngleTolerance", getAngleTolerance()) );
  setIdentifyGlobAlpha( reader->readValue("IdentifyGlobAlpha", getIdentifyGlobAlpha()) );
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE END*/
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int MergeColonies::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("AxisTolerance", getAxisTolerance() );
  writer->writeValue("AngleTolerance", getAngleTolerance() );
  writer->writeValue("IdentifyGlobAlpha", getIdentifyGlobAlpha() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MergeColonies::dataCheck()
{
  setErrorCondition(0);

  VolumeDataContainer* m = getDataContainerArray()->getPrereqDataContainer<VolumeDataContainer, AbstractFilter>(this, getDataContainerName(), false);
  if(getErrorCondition() < 0) { return; }
  AttributeMatrix* cellEnsembleAttrMat = m->getPrereqAttributeMatrix<AbstractFilter>(this, getCellEnsembleAttributeMatrixName(), -301);
  if(getErrorCondition() < 0) { return; }
  AttributeMatrix* cellFeatureAttrMat = m->getPrereqAttributeMatrix<AbstractFilter>(this, getCellFeatureAttributeMatrixName(), -301);
  if(getErrorCondition() < 0) { return; }
  AttributeMatrix* cellAttrMat = m->getPrereqAttributeMatrix<AbstractFilter>(this, getCellAttributeMatrixName(), -301);
  if(getErrorCondition() < 0) { return; }

  QVector<int> dims(1, 1);
  // Cell Data
  m_FeatureIdsPtr = cellAttrMat->getPrereqArray<DataArray<int32_t>, AbstractFilter>(this, m_FeatureIdsArrayName, -301, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeatureIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  m_CellPhasesPtr = cellAttrMat->getPrereqArray<DataArray<int32_t>, AbstractFilter>(this, m_CellPhasesArrayName, -300, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CellPhasesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_CellPhases = m_CellPhasesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  m_CellParentIdsPtr = cellAttrMat->createNonPrereqArray<DataArray<int32_t>, AbstractFilter, int32_t>(this, m_CellParentIdsArrayName, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CellParentIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_CellParentIds = m_CellParentIdsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(m_IdentifyGlobAlpha == true)
  {
    m_GlobAlphaPtr = cellAttrMat->createNonPrereqArray<DataArray<int32_t>, AbstractFilter, int32_t>(this, m_GlobAlphaArrayName, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if( NULL != m_GlobAlphaPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    { m_GlobAlpha = m_GlobAlphaPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  }
  // Feature Data
  m_FeaturePhasesPtr = cellFeatureAttrMat->getPrereqArray<DataArray<int32_t>, AbstractFilter>(this, m_FeaturePhasesArrayName, -303, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeaturePhasesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeaturePhases = m_FeaturePhasesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  m_ActivePtr = cellFeatureAttrMat->createNonPrereqArray<DataArray<bool>, AbstractFilter, bool>(this, m_ActiveArrayName, true, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_ActivePtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_Active = m_ActivePtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  m_FeatureParentIdsPtr = cellFeatureAttrMat->createNonPrereqArray<DataArray<int32_t>, AbstractFilter, int32_t>(this, m_FeatureParentIdsArrayName, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeatureParentIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeatureParentIds = m_FeatureParentIdsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  dims[0] = 4;
  m_AvgQuatsPtr = cellFeatureAttrMat->getPrereqArray<DataArray<float>, AbstractFilter>(this, m_AvgQuatsArrayName, -301, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_AvgQuatsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_AvgQuats = m_AvgQuatsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  // Now we are going to get a "Pointer" to the NeighborList object out of the DataContainer
  m_NeighborList = NeighborList<int>::SafeObjectDownCast<IDataArray*, NeighborList<int>*>(m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->getAttributeArray(DREAM3D::FeatureData::NeighborList).get());
  if(m_NeighborList == NULL)
  {
    QString ss = QObject::tr("NeighborLists Array Not Initialized correctly");
    setErrorCondition(-304);
    PipelineMessage em (getHumanLabel(), ss, -1, PipelineMessage::Error);
    emit filterGeneratedMessage(em);
  }

  // Ensemble Data
  dims[0] = 1;
  typedef DataArray<unsigned int> XTalStructArrayType;
  m_CrystalStructuresPtr = cellEnsembleAttrMat->getPrereqArray<DataArray<unsigned int>, AbstractFilter>(this, m_CrystalStructuresArrayName, -305, dims)
; /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CrystalStructuresPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_CrystalStructures = m_CrystalStructuresPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MergeColonies::preflight()
{
  dataCheck();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MergeColonies::execute()
{
  setErrorCondition(0);

  dataCheck();
  if(getErrorCondition() < 0) { return; }

  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

  emit filterGeneratedMessage(PipelineMessage::CreateStatusMessage(getHumanLabel(), "Merging Colonies") );
  merge_colonies();

  emit filterGeneratedMessage(PipelineMessage::CreateStatusMessage(getHumanLabel(), "Characterizing Colonies") );
  characterize_colonies();

  //m_RandomizeParentIds = false;
  if (true == m_RandomizeParentIds)
  {
    int64_t totalPoints = m->getAttributeMatrix(getCellAttributeMatrixName())->getNumTuples();

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

  if(m_IdentifyGlobAlpha == true)
  {
    identify_globAlpha();
  }

  emit filterGeneratedMessage(PipelineMessage::CreateStatusMessage(getHumanLabel(), "Completed") );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MergeColonies::merge_colonies()
{
  // Since this method is called from the 'execute' and the DataContainer validity
  // was checked there we are just going to get the Shared Pointer to the DataContainer
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

  NeighborList<int>& neighborlist = *m_NeighborList;

  //Converting the user defined tolerance to radians.
  m_AxisTolerance = m_AxisTolerance * DREAM3D::Constants::k_Pi / 180.0f;

  QVector<int> colonylist;
  float w;
  float n1 = 0.0f, n2 = 0.0f, n3 = 0.0f;
  float r1 = 0.0f, r2 = 0.0f, r3 = 0.0f;
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
      colonylist.push_back(i);
      for (QVector<int>::size_type j = 0; j < colonylist.size(); j++)
      {
        int firstfeature = colonylist[j];
        int size = int(neighborlist[firstfeature].size());
        for (int l = 0; l < size; l++)
        {
          int colony = 0;
          size_t neigh = neighborlist[firstfeature][l];
          if (neigh != i && m_ParentNumbers[neigh] == -1 && m_FeaturePhases[neigh] > 0)
          {
            w = 10000.0f;
            QuaternionMathF::Copy(avgQuats[firstfeature], q1);
            phase1 = m_CrystalStructures[m_FeaturePhases[firstfeature]];
            QuaternionMathF::Copy(avgQuats[neigh], q2);
            phase2 = m_CrystalStructures[m_FeaturePhases[neigh]];

            if (phase1 == phase2 &&
                (phase1 == Ebsd::CrystalStructure::Hexagonal_High) )
            {
              w = m_OrientationOps[phase1]->getMisoQuat( q1, q2, n1, n2, n3);
              OrientationMath::AxisAngletoRod(w, n1, n2, n3, r1, r2, r3);
              m_OrientationOps[phase1]->getMDFFZRod(r1, r2, r3);
              OrientationMath::RodtoAxisAngle(r1, r2, r3, w, n1, n2, n3);
              w = w * (180.0f / DREAM3D::Constants::k_Pi);
              float angdiff1 = fabs(w - 10.53f);
              float axisdiff1 = acosf(fabs(n1) * 0.0000f + fabs(n2) * 0.0000f + fabs(n3) * 1.0000f);
              if(angdiff1 < m_AngleTolerance && axisdiff1 < m_AxisTolerance) { colony = 1; }
              float angdiff2 = fabs(w - 90.00f);
              float axisdiff2 = acosf(fabs(n1) * 0.9958f + fabs(n2) * 0.0917f + fabs(n3) * 0.0000f);
              if(angdiff2 < m_AngleTolerance && axisdiff2 < m_AxisTolerance) { colony = 1; }
              float angdiff3 = fabs(w - 60.00f);
              float axisdiff3 = acosf(fabs(n1) * 1.0000f + fabs(n2) * 0.0000f + fabs(n3) * 0.0000f);
              if(angdiff3 < m_AngleTolerance && axisdiff3 < m_AxisTolerance) { colony = 1; }
              float angdiff4 = fabs(w - 60.83f);
              float axisdiff4 = acosf(fabs(n1) * 0.9834f + fabs(n2) * 0.0905f + fabs(n3) * 0.1570f);
              if(angdiff4 < m_AngleTolerance && axisdiff4 < m_AxisTolerance) { colony = 1; }
              float angdiff5 = fabs(w - 63.26f);
              float axisdiff5 = acosf(fabs(n1) * 0.9549f + fabs(n2) * 0.0000f + fabs(n3) * 0.2969f);
              if(angdiff5 < m_AngleTolerance && axisdiff5 < m_AxisTolerance) { colony = 1; }
              if (colony == 1)
              {
                m_ParentNumbers[neigh] = parentcount;
                colonylist.push_back(neigh);
              }
            }
            else if (Ebsd::CrystalStructure::Cubic_High == phase2
                     && Ebsd::CrystalStructure::Hexagonal_High == phase1)
            {
              colony = check_for_burgers(q2, q1);
              if (colony == 1)
              {
                m_ParentNumbers[neigh] = parentcount;
                colonylist.push_back(neigh);
              }
            }
            else if ( Ebsd::CrystalStructure::Cubic_High == phase1
                      && Ebsd::CrystalStructure::Hexagonal_High == phase2)
            {
              colony = check_for_burgers(q1, q2);
              if (colony == 1)
              {
                m_ParentNumbers[neigh] = parentcount;
                colonylist.push_back(neigh);
              }
            }
          }
        }
      }
    }
    colonylist.clear();
  }
  size_t totalPoints = static_cast<size_t>(m->getTotalPoints());
  for (size_t k = 0; k < totalPoints; k++)
  {
    int featurename = m_FeatureIds[k];
    if(featurename > 0) { m_CellParentIds[k] = m_ParentNumbers[featurename]; }
    else { m_CellParentIds[k] = 0; }
  }
  numParents = parentcount + 1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MergeColonies::characterize_colonies()
{
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

  size_t numfeatures = m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->getNumTuples();
  for (size_t i = 0; i < numfeatures; i++)
  {

  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int MergeColonies::check_for_burgers(QuatF betaQuat, QuatF alphaQuat)
{
  float dP = 0.0;
  float angle = 0.0;
  float radToDeg = 180.0f / DREAM3D::Constants::k_Pi;

  float gBeta[3][3];
  float gBetaT[3][3];
  OrientationMath::QuattoMat(betaQuat, gBeta);
  //transpose gBeta so the sample direction is the output when
  //gBeta is multiplied by the crystal directions below
  MatrixMath::Transpose3x3(gBeta, gBetaT);

  float gAlpha[3][3];
  float gAlphaT[3][3];
  OrientationMath::QuattoMat(alphaQuat, gAlpha);
  //transpose gBeta so the sample direction is the output when
  //gBeta is multiplied by the crystal directions below
  MatrixMath::Transpose3x3(gAlpha, gAlphaT);

  float mat[3][3];
  float a[3];
  float b[3];
  for(int i = 0; i < 12; i++)
  {
    MatrixMath::Multiply3x3with3x3(gBetaT, crystalDirections[i], mat);
    a[0] = mat[0][2];
    a[1] = mat[1][2];
    a[2] = mat[2][2];
    b[0] = gAlphaT[0][2];
    b[1] = gAlphaT[1][2];
    b[2] = gAlphaT[2][2];
    dP = GeometryMath::CosThetaBetweenVectors(a, b);
    angle = acos(dP);
    if((angle * radToDeg) < m_AngleTolerance || (180.0 - (angle * radToDeg)) < m_AngleTolerance)
    {
      a[0] = mat[0][0];
      a[1] = mat[1][0];
      a[2] = mat[2][0];
      b[0] = gAlphaT[0][0];
      b[1] = gAlphaT[1][0];
      b[2] = gAlphaT[2][0];
      dP = GeometryMath::CosThetaBetweenVectors(a, b);
      angle = acos(dP);
      if((angle * radToDeg) < m_AngleTolerance) { return 1; }
      if((180.0 - (angle * radToDeg)) < m_AngleTolerance) { return 1; }
      b[0] = -0.5 * gAlphaT[0][0] + 0.866025 * gAlphaT[0][1];
      b[1] = -0.5 * gAlphaT[1][0] + 0.866025 * gAlphaT[1][1];
      b[2] = -0.5 * gAlphaT[2][0] + 0.866025 * gAlphaT[2][1];
      dP = GeometryMath::CosThetaBetweenVectors(a, b);
      angle = acos(dP);
      if((angle * radToDeg) < m_AngleTolerance) { return 1; }
      if((180.0 - (angle * radToDeg)) < m_AngleTolerance) { return 1; }
      b[0] = -0.5 * gAlphaT[0][0] - 0.866025 * gAlphaT[0][1];
      b[1] = -0.5 * gAlphaT[1][0] - 0.866025 * gAlphaT[1][1];
      b[2] = -0.5 * gAlphaT[2][0] - 0.866025 * gAlphaT[2][1];
      dP = GeometryMath::CosThetaBetweenVectors(a, b);
      angle = acos(dP);
      if((angle * radToDeg) < m_AngleTolerance) { return 1; }
      if((180.0 - (angle * radToDeg)) < m_AngleTolerance) { return 1; }
    }
  }

  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MergeColonies::identify_globAlpha()
{
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

  int64_t totalPoints = m->getAttributeMatrix(getCellAttributeMatrixName())->getNumTuples();
  QVector<int> betaSize(numParents, 0);
  QVector<int> totalSize(numParents, 0);
  for (int64_t i = 0; i < totalPoints; i++)
  {
    int pnum = m_CellParentIds[i];
    totalSize[pnum]++;
    if(Ebsd::CrystalStructure::Cubic_High == m_CrystalStructures[m_CellPhases[i]] )
    { betaSize[pnum]++; }
  }
  for (int64_t i = 0; i < totalPoints; i++)
  {
    int pnum = m_CellParentIds[i];
    float ratio = float(betaSize[pnum]) / float(totalSize[pnum]);
    if(ratio > 0.0) { m_GlobAlpha[i] = 0; }
    else { m_GlobAlpha[i] = 1; }
  }
}

