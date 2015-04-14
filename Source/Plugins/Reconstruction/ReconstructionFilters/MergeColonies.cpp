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
#include "DREAM3DLib/FilterParameters/AbstractFilterParametersReader.h"
#include "DREAM3DLib/FilterParameters/AbstractFilterParametersWriter.h"
#include "DREAM3DLib/Math/DREAM3DMath.h"
#include "DREAM3DLib/Math/GeometryMath.h"
#include "DREAM3DLib/Math/MatrixMath.h"
#include "DREAM3DLib/Utilities/DREAM3DRandom.h"
#include "OrientationLib/Math/OrientationMath.h"
#include "OrientationLib/OrientationOps/OrientationOps.h"

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
  GroupFeatures(),
  m_NewCellFeatureAttributeMatrixName(DREAM3D::Defaults::NewCellFeatureAttributeMatrixName),
  m_FeatureIdsArrayPath(DREAM3D::Defaults::DataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, DREAM3D::CellData::FeatureIds),
  m_CellPhasesArrayPath(DREAM3D::Defaults::DataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, DREAM3D::CellData::Phases),
  m_FeaturePhasesArrayPath(DREAM3D::Defaults::DataContainerName, DREAM3D::Defaults::CellFeatureAttributeMatrixName, DREAM3D::FeatureData::Phases),
  m_AvgQuatsArrayPath(DREAM3D::Defaults::DataContainerName, DREAM3D::Defaults::CellFeatureAttributeMatrixName, DREAM3D::FeatureData::AvgQuats),
  m_CrystalStructuresArrayPath(DREAM3D::Defaults::DataContainerName, DREAM3D::Defaults::CellEnsembleAttributeMatrixName, DREAM3D::EnsembleData::CrystalStructures),
  m_CellParentIdsArrayName(DREAM3D::CellData::ParentIds),
  m_GlobAlphaArrayName(DREAM3D::CellData::GlobAlpha),
  m_FeatureParentIdsArrayName(DREAM3D::FeatureData::ParentIds),
  m_ActiveArrayName(DREAM3D::FeatureData::Active),
  m_AxisTolerance(1.0f),
  m_AngleTolerance(1.0f),
  m_RandomizeParentIds(true),
  m_IdentifyGlobAlpha(false),
  m_FeatureIdsArrayName(DREAM3D::CellData::FeatureIds),
  m_FeatureIds(NULL),
  m_CellPhasesArrayName(DREAM3D::CellData::Phases),
  m_CellPhases(NULL),
  m_CellParentIds(NULL),
  m_FeatureParentIds(NULL),
  m_GlobAlpha(NULL),
  m_AvgQuatsArrayName(DREAM3D::FeatureData::AvgQuats),
  m_AvgQuats(NULL),
  m_Active(NULL),
  m_FeaturePhasesArrayName(DREAM3D::FeatureData::Phases),
  m_FeaturePhases(NULL),
  m_CrystalStructuresArrayName(DREAM3D::EnsembleData::CrystalStructures),
  m_CrystalStructures(NULL)
{
  m_OrientationOps = OrientationOps::getOrientationOpsQVector();

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

  //These are parameters that the super class needs
  parameters.push_back(FilterParameter::New("Required Information", "", FilterParameterWidgetType::SeparatorWidget, "QString", true));
  parameters.push_back(FilterParameter::New("Contiguous NeighborList Array", "ContiguousNeighborListArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getContiguousNeighborListArrayPath(), true, ""));

  QStringList linkedProps("GlobAlphaArrayName");
  parameters.push_back(LinkedBooleanFilterParameter::New("Identify Glob Alpha", "IdentifyGlobAlpha", getIdentifyGlobAlpha(), linkedProps, false));
  parameters.push_front(FilterParameter::New("Axis Tolerance", "AxisTolerance", FilterParameterWidgetType::DoubleWidget, getAxisTolerance(), false, "Degrees"));
  parameters.push_front(FilterParameter::New("Angle Tolerance", "AngleTolerance", FilterParameterWidgetType::DoubleWidget, getAngleTolerance(), false, "Degrees"));
  parameters.push_back(FilterParameter::New("FeatureIds", "FeatureIdsArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getFeatureIdsArrayPath(), true, ""));
  parameters.push_back(FilterParameter::New("Cell Phases", "CellPhasesArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getCellPhasesArrayPath(), true, ""));
  parameters.push_back(FilterParameter::New("FeaturePhases", "FeaturePhasesArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getFeaturePhasesArrayPath(), true, ""));
  parameters.push_back(FilterParameter::New("AvgQuats", "AvgQuatsArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getAvgQuatsArrayPath(), true, ""));
  parameters.push_back(FilterParameter::New("Crystal Structures", "CrystalStructuresArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getCrystalStructuresArrayPath(), true, ""));
  parameters.push_back(FilterParameter::New("Created Information", "", FilterParameterWidgetType::SeparatorWidget, "", true));
  parameters.push_back(FilterParameter::New("CellParentIds", "CellParentIdsArrayName", FilterParameterWidgetType::StringWidget, getCellParentIdsArrayName(), true, ""));
  parameters.push_back(FilterParameter::New("New Cell Feature Attribute Matrix Name", "NewCellFeatureAttributeMatrixName", FilterParameterWidgetType::StringWidget, getNewCellFeatureAttributeMatrixName(), true, ""));
  parameters.push_back(FilterParameter::New("GlobAlpha", "GlobAlphaArrayName", FilterParameterWidgetType::StringWidget, getGlobAlphaArrayName(), true, ""));
  parameters.push_back(FilterParameter::New("FeatureParentIds", "FeatureParentIdsArrayName", FilterParameterWidgetType::StringWidget, getFeatureParentIdsArrayName(), true, ""));
  parameters.push_back(FilterParameter::New("Active", "ActiveArrayName", FilterParameterWidgetType::StringWidget, getActiveArrayName(), true, ""));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MergeColonies::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  GroupFeatures::readFilterParameters(reader, index);
  reader->openFilterGroup(this, index);
  setNewCellFeatureAttributeMatrixName(reader->readString("NewCellFeatureAttributeMatrixName", getNewCellFeatureAttributeMatrixName() ) );
  setActiveArrayName(reader->readString("ActiveArrayName", getActiveArrayName() ) );
  setFeatureParentIdsArrayName(reader->readString("FeatureParentIdsArrayName", getFeatureParentIdsArrayName() ) );
  setGlobAlphaArrayName(reader->readString("GlobAlphaArrayName", getGlobAlphaArrayName() ) );
  setCellParentIdsArrayName(reader->readString("CellParentIdsArrayName", getCellParentIdsArrayName() ) );
  setCrystalStructuresArrayPath(reader->readDataArrayPath("CrystalStructuresArrayPath", getCrystalStructuresArrayPath() ) );
  setAvgQuatsArrayPath(reader->readDataArrayPath("AvgQuatsArrayPath", getAvgQuatsArrayPath() ) );
  setFeaturePhasesArrayPath(reader->readDataArrayPath("FeaturePhasesArrayPath", getFeaturePhasesArrayPath() ) );
  setCellPhasesArrayPath(reader->readDataArrayPath("CellPhasesArrayPath", getCellPhasesArrayPath() ) );
  setFeatureIdsArrayPath(reader->readDataArrayPath("FeatureIdsArrayPath", getFeatureIdsArrayPath() ) );
  setAxisTolerance( reader->readValue("AxisTolerance", getAxisTolerance()) );
  setAngleTolerance( reader->readValue("AngleTolerance", getAngleTolerance()) );
  setIdentifyGlobAlpha( reader->readValue("IdentifyGlobAlpha", getIdentifyGlobAlpha()) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int MergeColonies::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  GroupFeatures::writeFilterParameters(writer, index);
  writer->openFilterGroup(this, index);
  DREAM3D_FILTER_WRITE_PARAMETER(ActiveArrayName)
  DREAM3D_FILTER_WRITE_PARAMETER(NewCellFeatureAttributeMatrixName)
  DREAM3D_FILTER_WRITE_PARAMETER(FeatureParentIdsArrayName)
  DREAM3D_FILTER_WRITE_PARAMETER(GlobAlphaArrayName)
  DREAM3D_FILTER_WRITE_PARAMETER(CellParentIdsArrayName)
  DREAM3D_FILTER_WRITE_PARAMETER(CrystalStructuresArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(AvgQuatsArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(FeaturePhasesArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(CellPhasesArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(FeatureIdsArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(AxisTolerance)
  DREAM3D_FILTER_WRITE_PARAMETER(AngleTolerance)
  DREAM3D_FILTER_WRITE_PARAMETER(IdentifyGlobAlpha)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MergeColonies::updateFeatureInstancePointers()
{
  setErrorCondition(0);

  if( NULL != m_ActivePtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_Active = m_ActivePtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MergeColonies::dataCheck()
{
  DataArrayPath tempPath;
  setErrorCondition(0);

  DataContainer::Pointer m = getDataContainerArray()->getPrereqDataContainer<AbstractFilter>(this, m_FeatureIdsArrayPath.getDataContainerName(), false);
  if(getErrorCondition() < 0 || NULL == m) { return; }
  QVector<size_t> tDims(1, 0);
  AttributeMatrix::Pointer newCellFeatureAttrMat = m->createNonPrereqAttributeMatrix<AbstractFilter>(this, getNewCellFeatureAttributeMatrixName(), tDims, DREAM3D::AttributeMatrixType::CellFeature);
  if(getErrorCondition() < 0) { return; }

  QVector<size_t> dims(1, 1);
  // Cell Data
  m_FeatureIdsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getFeatureIdsArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeatureIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  m_CellPhasesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getCellPhasesArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CellPhasesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_CellPhases = m_CellPhasesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  tempPath.update(m_FeatureIdsArrayPath.getDataContainerName(), m_FeatureIdsArrayPath.getAttributeMatrixName(), getCellParentIdsArrayName() );
  m_CellParentIdsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter, int32_t>(this, tempPath, -1, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CellParentIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_CellParentIds = m_CellParentIdsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  if(m_IdentifyGlobAlpha == true)
  {
    tempPath.update(m_FeatureIdsArrayPath.getDataContainerName(), m_FeatureIdsArrayPath.getAttributeMatrixName(), getGlobAlphaArrayName() );
    m_GlobAlphaPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter, int32_t>(this, tempPath, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if( NULL != m_GlobAlphaPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    { m_GlobAlpha = m_GlobAlphaPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  }
  // Feature Data
  m_FeaturePhasesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getFeaturePhasesArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeaturePhasesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeaturePhases = m_FeaturePhasesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  tempPath.update(m_FeaturePhasesArrayPath.getDataContainerName(), m_FeaturePhasesArrayPath.getAttributeMatrixName(), getFeatureParentIdsArrayName() );
  m_FeatureParentIdsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter, int32_t>(this, tempPath, -1, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeatureParentIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeatureParentIds = m_FeatureParentIdsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  dims[0] = 4;
  m_AvgQuatsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getAvgQuatsArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_AvgQuatsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_AvgQuats = m_AvgQuatsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  // NewFeature Data
  dims[0] = 1;
  tempPath.update(m_FeatureIdsArrayPath.getDataContainerName(), getNewCellFeatureAttributeMatrixName(), getActiveArrayName() );
  m_ActivePtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<bool>, AbstractFilter, bool>(this, tempPath, true, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_ActivePtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_Active = m_ActivePtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  // Ensemble Data
//typedef DataArray<unsigned int> XTalStructArrayType;
  m_CrystalStructuresPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<unsigned int>, AbstractFilter>(this, getCrystalStructuresArrayPath(), dims)
                           ; /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CrystalStructuresPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_CrystalStructures = m_CrystalStructuresPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MergeColonies::preflight()
{
  setInPreflight(true);
  emit preflightAboutToExecute();
  emit updateFilterParameters(this);
  dataCheck();
  emit preflightExecuted();
  setInPreflight(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MergeColonies::execute()
{
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  axisTolerance = m_AxisTolerance * DREAM3D::Constants::k_Pi / 180.0f;

  // Tell the user we are starting the filter
  notifyStatusMessage(getMessagePrefix(), getHumanLabel(), "Starting");
  GroupFeatures::execute();

  size_t totalFeatures = m_ActivePtr.lock()->getNumberOfTuples();
  if (totalFeatures < 2)
  {
    setErrorCondition(-87000);
    notifyErrorMessage(getHumanLabel(), "The number of Grouped Features was 0 or 1 which means no grouped features were detected. Is a grouping value set to high?", getErrorCondition());
    return;
  }

  qint32 numParents = 0;
  size_t totalPoints = static_cast<size_t>(m_FeatureIdsPtr.lock()->getNumberOfTuples());
  for (size_t k = 0; k < totalPoints; k++)
  {
    int featurename = m_FeatureIds[k];
    m_CellParentIds[k] = m_FeatureParentIds[featurename];
    if(m_FeatureParentIds[featurename] > numParents) { numParents = m_FeatureParentIds[featurename]; }
  }
  numParents += 1;

  notifyStatusMessage(getHumanLabel(), "Characterizing Colonies");
  characterize_colonies();

  //m_RandomizeParentIds = false;
  if (true == m_RandomizeParentIds)
  {
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

    for(size_t i = 0; i < totalPoints; ++i)
    {
      m_CellParentIds[i] = pid[ m_CellParentIds[i] ];
      m_FeatureParentIds[m_FeatureIds[i]] = m_CellParentIds[i];
    }
  }

  if(m_IdentifyGlobAlpha == true)
  {
    identify_globAlpha();
  }

  notifyStatusMessage(getHumanLabel(), "Completed");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int MergeColonies::getSeed(int newFid)
{
  setErrorCondition(0);

  size_t numfeatures = m_FeaturePhasesPtr.lock()->getNumberOfTuples();

  DREAM3D_RANDOMNG_NEW()
  int seed = -1;
  int randfeature = 0;

  // Precalculate some constants
  size_t totalFMinus1 = numfeatures - 1;

  size_t counter = 0;
  randfeature = int(float(rg.genrand_res53()) * float(totalFMinus1));
  while (seed == -1 && counter < numfeatures)
  {
    if (randfeature > totalFMinus1) { randfeature = static_cast<int>( randfeature - numfeatures ); }
    if (m_FeatureParentIds[randfeature] == -1) { seed = randfeature; }
    randfeature++;
    counter++;
  }
  if (seed >= 0)
  {
    m_FeatureParentIds[seed] = newFid;
    QVector<size_t> tDims(1, newFid + 1);
    getDataContainerArray()->getDataContainer(m_FeatureIdsArrayPath.getDataContainerName())->getAttributeMatrix(getNewCellFeatureAttributeMatrixName())->resizeAttributeArrays(tDims);
    updateFeatureInstancePointers();
  }
  return seed;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool MergeColonies::determineGrouping(int referenceFeature, int neighborFeature, int newFid)
{
  float w;
  float n1 = 0.0f, n2 = 0.0f, n3 = 0.0f;
  float r1 = 0.0f, r2 = 0.0f, r3 = 0.0f;
  bool colony = false;
  QuatF q1;
  QuatF q2;
  QuatF* avgQuats = reinterpret_cast<QuatF*>(m_AvgQuats);

  if (m_FeatureParentIds[neighborFeature] == -1 && m_FeaturePhases[referenceFeature] > 0 && m_FeaturePhases[neighborFeature] > 0)
  {
    w = 10000.0f;
    QuaternionMathF::Copy(avgQuats[referenceFeature], q1);
    unsigned int phase1 = m_CrystalStructures[m_FeaturePhases[referenceFeature]];
    QuaternionMathF::Copy(avgQuats[neighborFeature], q2);
    unsigned int phase2 = m_CrystalStructures[m_FeaturePhases[neighborFeature]];
    if (phase1 == phase2 && (phase1 == Ebsd::CrystalStructure::Hexagonal_High) )
    {
      w = m_OrientationOps[phase1]->getMisoQuat( q1, q2, n1, n2, n3);
      OrientationMath::AxisAngletoRod(w, n1, n2, n3, r1, r2, r3);
      m_OrientationOps[phase1]->getMDFFZRod(r1, r2, r3);
      OrientationMath::RodtoAxisAngle(r1, r2, r3, w, n1, n2, n3);
      w = w * (180.0f / DREAM3D::Constants::k_Pi);
      float angdiff1 = fabs(w - 10.53f);
      float axisdiff1 = acosf(fabs(n1) * 0.0000f + fabs(n2) * 0.0000f + fabs(n3) * 1.0000f);
      if(angdiff1 < m_AngleTolerance && axisdiff1 < axisTolerance) { colony = true; }
      float angdiff2 = fabs(w - 90.00f);
      float axisdiff2 = acosf(fabs(n1) * 0.9958f + fabs(n2) * 0.0917f + fabs(n3) * 0.0000f);
      if(angdiff2 < m_AngleTolerance && axisdiff2 < axisTolerance) { colony = true; }
      float angdiff3 = fabs(w - 60.00f);
      float axisdiff3 = acosf(fabs(n1) * 1.0000f + fabs(n2) * 0.0000f + fabs(n3) * 0.0000f);
      if(angdiff3 < m_AngleTolerance && axisdiff3 < axisTolerance) { colony = true; }
      float angdiff4 = fabs(w - 60.83f);
      float axisdiff4 = acosf(fabs(n1) * 0.9834f + fabs(n2) * 0.0905f + fabs(n3) * 0.1570f);
      if(angdiff4 < m_AngleTolerance && axisdiff4 < axisTolerance) { colony = true; }
      float angdiff5 = fabs(w - 63.26f);
      float axisdiff5 = acosf(fabs(n1) * 0.9549f + fabs(n2) * 0.0000f + fabs(n3) * 0.2969f);
      if(angdiff5 < m_AngleTolerance && axisdiff5 < axisTolerance) { colony = true; }
      if (colony == true)
      {
        m_FeatureParentIds[neighborFeature] = newFid;
        return true;
      }
    }
    else if (Ebsd::CrystalStructure::Cubic_High == phase2 && Ebsd::CrystalStructure::Hexagonal_High == phase1)
    {
      colony = check_for_burgers(q2, q1);
      if (colony == true)
      {
        m_FeatureParentIds[neighborFeature] = newFid;
        return true;
      }
    }
    else if ( Ebsd::CrystalStructure::Cubic_High == phase1 && Ebsd::CrystalStructure::Hexagonal_High == phase2)
    {
      colony = check_for_burgers(q1, q2);
      if (colony == true)
      {
        m_FeatureParentIds[neighborFeature] = newFid;
        return true;
      }
    }
  }
  return false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MergeColonies::characterize_colonies()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool MergeColonies::check_for_burgers(QuatF betaQuat, QuatF alphaQuat)
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
      if((angle * radToDeg) < m_AngleTolerance) { return true; }
      if((180.0 - (angle * radToDeg)) < m_AngleTolerance) { return true; }
      b[0] = -0.5 * gAlphaT[0][0] + 0.866025 * gAlphaT[0][1];
      b[1] = -0.5 * gAlphaT[1][0] + 0.866025 * gAlphaT[1][1];
      b[2] = -0.5 * gAlphaT[2][0] + 0.866025 * gAlphaT[2][1];
      dP = GeometryMath::CosThetaBetweenVectors(a, b);
      angle = acos(dP);
      if((angle * radToDeg) < m_AngleTolerance) { return true; }
      if((180.0 - (angle * radToDeg)) < m_AngleTolerance) { return true; }
      b[0] = -0.5 * gAlphaT[0][0] - 0.866025 * gAlphaT[0][1];
      b[1] = -0.5 * gAlphaT[1][0] - 0.866025 * gAlphaT[1][1];
      b[2] = -0.5 * gAlphaT[2][0] - 0.866025 * gAlphaT[2][1];
      dP = GeometryMath::CosThetaBetweenVectors(a, b);
      angle = acos(dP);
      if((angle * radToDeg) < m_AngleTolerance) { return true; }
      if((180.0 - (angle * radToDeg)) < m_AngleTolerance) { return true; }
    }
  }

  return false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MergeColonies::identify_globAlpha()
{
  //DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getDataContainerName());

  //int64_t totalPoints = m->getAttributeMatrix(getCellAttributeMatrixName())->getNumTuples();
  //QVector<int> betaSize(numParents, 0);
  //QVector<int> totalSize(numParents, 0);
  //for (int64_t i = 0; i < totalPoints; i++)
  //{
  //  int pnum = m_CellParentIds[i];
  //  totalSize[pnum]++;
  //  if(Ebsd::CrystalStructure::Cubic_High == m_CrystalStructures[m_CellPhases[i]] )
  //  { betaSize[pnum]++; }
  //}
  //for (int64_t i = 0; i < totalPoints; i++)
  //{
  //  int pnum = m_CellParentIds[i];
  //  float ratio = float(betaSize[pnum]) / float(totalSize[pnum]);
  //  if(ratio > 0.0) { m_GlobAlpha[i] = 0; }
  //  else { m_GlobAlpha[i] = 1; }
  //}
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer MergeColonies::newFilterInstance(bool copyFilterParameters)
{
  MergeColonies::Pointer filter = MergeColonies::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString MergeColonies::getCompiledLibraryName()
{ return Reconstruction::ReconstructionBaseName; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString MergeColonies::getGroupName()
{ return DREAM3D::FilterGroups::ReconstructionFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString MergeColonies::getSubGroupName()
{return DREAM3D::FilterSubGroups::GroupingFilters;}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString MergeColonies::getHumanLabel()
{ return "Reconstruct Prior Beta Features"; }

