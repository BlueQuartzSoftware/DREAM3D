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

#include "VectorSegmentFeatures.h"

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/variate_generator.hpp>

#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Math/GeometryMath.h"

#include "DREAM3DLib/Utilities/DREAM3DRandom.h"


#define ERROR_TXT_OUT 1
#define ERROR_TXT_OUT1 1

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VectorSegmentFeatures::VectorSegmentFeatures() :
  SegmentFeatures(),
  m_DataContainerName(DREAM3D::Defaults::VolumeDataContainerName),
  m_CellFeatureAttributeMatrixName(DREAM3D::Defaults::CellFeatureAttributeMatrixName),
  m_CellAttributeMatrixName(DREAM3D::Defaults::CellAttributeMatrixName),
  m_SelectedVectorArrayName(""),
  m_AngleTolerance(5.0f),
  m_RandomizeFeatureIds(true),
  m_VectorsArrayName(""),
  m_Vectors(NULL),
  m_FeatureIdsArrayName(DREAM3D::CellData::FeatureIds),
  m_FeatureIds(NULL),
  m_GoodVoxelsArrayName(DREAM3D::CellData::GoodVoxels),
  m_GoodVoxels(NULL),
  m_ActiveArrayName(DREAM3D::FeatureData::Active),
  m_Active(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VectorSegmentFeatures::~VectorSegmentFeatures()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VectorSegmentFeatures::setupFilterParameters()
{
  FilterParameterVector parameters;
  {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("Vector Array Name");
    parameter->setPropertyName("SelectedVectorArrayName");
    parameter->setWidgetType(FilterParameterWidgetType::SingleArraySelectionWidget);
    parameter->setValueType("QString");
    parameter->setUnits("");
    parameters.push_back(parameter);
  }
  {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setPropertyName("AngleTolerance");
    parameter->setHumanLabel("Angle Tolerance");
    parameter->setWidgetType(FilterParameterWidgetType::DoubleWidget);
    parameter->setValueType("float");
    parameter->setCastableValueType("double");
    parameter->setUnits("");
    parameters.push_back(parameter);
  }
#if 0
  {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("Randomly Reorder Generated Feature Ids");
    parameter->setPropertyName("RandomizeFeatureIds");
    parameter->setWidgetType(FilterParameterWidgetType::BooleanWidget);
    parameter->setValueType("bool");
    parameters.push_back(parameter);
  }
#endif
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VectorSegmentFeatures::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  /* Code to read the values goes between these statements */
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE BEGIN*/
  setSelectedVectorArrayName( reader->readString( "SelectedVectorArrayName", getSelectedVectorArrayName() ) );
  setAngleTolerance( reader->readValue("AngleTolerance", getAngleTolerance()) );
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE END*/
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VectorSegmentFeatures::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("SelectedVectorArrayName", getSelectedVectorArrayName() );
  writer->writeValue("AngleTolerance", getAngleTolerance() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VectorSegmentFeatures::updateFeatureInstancePointers()
{
  setErrorCondition(0);

  if( NULL != m_ActivePtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_Active = m_ActivePtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VectorSegmentFeatures::dataCheck()
{
  setErrorCondition(0);

  VolumeDataContainer* m = getDataContainerArray()->getPrereqDataContainer<VolumeDataContainer, AbstractFilter>(this, getDataContainerName(), false);
  if(getErrorCondition() < 0 || NULL == m) { return; }
  QVector<size_t> tDims(1, 0);
  AttributeMatrix::Pointer cellFeatureAttrMat = m->createNonPrereqAttributeMatrix<AbstractFilter>(this, getCellFeatureAttributeMatrixName(), tDims, DREAM3D::AttributeMatrixType::CellFeature);
  if(getErrorCondition() < 0) { return; }
  AttributeMatrix::Pointer cellAttrMat = m->getPrereqAttributeMatrix<AbstractFilter>(this, getCellAttributeMatrixName(), -301);
  if(getErrorCondition() < 0 || NULL == cellAttrMat.get() ) { return; }

  if(m_SelectedVectorArrayName.isEmpty() == true)
  {
    setErrorCondition(-11000);
    notifyErrorMessage(getHumanLabel(), "An array from the Volume DataContainer must be selected.", getErrorCondition());
  }
  else
  {

    QString dcName;
    QString amName;
    QString daName;

    QStringList tokens = m_SelectedVectorArrayName.split(DREAM3D::PathSep);
    // We should end up with 3 Tokens
    if(tokens.size() != 3)
    {
      setErrorCondition(-11002);
      QString ss = QObject::tr("The path to the Attribute Array is malformed. Each part should be separated by a '|' character.");
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    }
    else
    {
      dcName = tokens.at(0);
      amName = tokens.at(1);
      daName = tokens.at(2);

      DataContainer::Pointer dc = getDataContainerArray()->getDataContainer(dcName);
      if(NULL == dc.get())
      {
        setErrorCondition(-11003);
        QString ss = QObject::tr("The DataContainer '%1' was not found in the DataContainerArray").arg(dcName);
        notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
        return;
      }

      AttributeMatrix::Pointer attrMat = dc->getAttributeMatrix(amName);
       if(NULL == attrMat.get())
      {
        setErrorCondition(-11004);
        QString ss = QObject::tr("The AttributeMatrix '%1' was not found in the DataContainer '%2'").arg(amName).arg(dcName);
        notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
        return;
      }

      m_VectorsArrayName = daName;

      QVector<size_t> dims(1, 3);
      m_VectorsPtr = cellAttrMat->getPrereqArray<DataArray<float>, AbstractFilter>(this, m_VectorsArrayName, -303, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
      if( NULL != m_VectorsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
      { m_Vectors = m_VectorsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
    }
  }

  QVector<size_t> dims(1, 1);
  m_FeatureIdsPtr = cellAttrMat->createNonPrereqArray<DataArray<int32_t>, AbstractFilter, int32_t>(this, m_FeatureIdsArrayName, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeatureIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  m_GoodVoxelsPtr = cellAttrMat->getPrereqArray<DataArray<bool>, AbstractFilter>(NULL, m_GoodVoxelsArrayName, -304, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_GoodVoxelsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_GoodVoxels = m_GoodVoxelsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  else {m_GoodVoxels = NULL;}
  m_ActivePtr = cellFeatureAttrMat->createNonPrereqArray<DataArray<bool>, AbstractFilter, bool>(this, m_ActiveArrayName, true, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_ActivePtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_Active = m_ActivePtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VectorSegmentFeatures::preflight()
{
  emit preflightAboutToExecute();
  emit updateFilterParameters(this);
  dataCheck();
  emit preflightExecuted();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VectorSegmentFeatures::execute()
{
  setErrorCondition(0);

  dataCheck();
  if(getErrorCondition() < 0) { return; }

  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

  QVector<size_t> tDims(1, 1);
  m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->resizeAttributeArrays(tDims);
  // This runs a subfilter
  int64_t totalPoints = m->getAttributeMatrix(getCellAttributeMatrixName())->getNumTuples();

  // Tell the user we are starting the filter
  notifyStatusMessage(getHumanLabel(), "Starting");

  //Convert user defined tolerance to radians.
  m_AngleTolerance = m_AngleTolerance * DREAM3D::Constants::k_Pi / 180.0f;
  for(int64_t i = 0; i < totalPoints; i++)
  {
    m_FeatureIds[i] = 0;
  }

  missingGoodVoxels = true;
  if (NULL != m_GoodVoxels)
  {
    missingGoodVoxels = false;
  }

  SegmentFeatures::execute();

  if (true == m_RandomizeFeatureIds)
  {
    totalPoints = m->getAttributeMatrix(getCellAttributeMatrixName())->getNumTuples();
    size_t totalFeatures = m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->getNumTuples();

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
      if (r >= totalFeatures)
      {
        continue;
      }
      temp = gid[i];
      gid[i] = gid[r];
      gid[r] = temp;
    }

    // Now adjust all the Feature Id values for each Voxel
    for(int64_t i = 0; i < totalPoints; ++i)
    {
      m_FeatureIds[i] = gid[ m_FeatureIds[i] ];
    }
  }

  // If there is an error set this to something negative and also set a message
  notifyStatusMessage(getHumanLabel(), "Completed");
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int64_t VectorSegmentFeatures::getSeed(size_t gnum)
{
  setErrorCondition(0);
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

  int64_t totalPoints = m->getTotalPoints();

  DREAM3D_RANDOMNG_NEW()
  int64_t seed = -1;
  int64_t randpoint = 0;

  // Pre-calculate some constants
  int64_t totalPMinus1 = totalPoints - 1;

  int64_t counter = 0;
  randpoint = int64_t(float(rg.genrand_res53()) * float(totalPMinus1));
  while (seed == -1 && counter < totalPoints)
  {
    if (randpoint > totalPMinus1) { randpoint = static_cast<int64_t>( randpoint - totalPoints ); }
    if ((missingGoodVoxels == true || m_GoodVoxels[randpoint] == true) && m_FeatureIds[randpoint] == 0) { seed = randpoint; }
    randpoint++;
    counter++;
  }
  if (seed >= 0)
  {
    m_FeatureIds[seed] = gnum;
    QVector<size_t> tDims(1, gnum+1);
    m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->resizeAttributeArrays(tDims);
    updateFeatureInstancePointers();
  }
  return seed;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VectorSegmentFeatures::determineGrouping(int64_t referencepoint, int64_t neighborpoint, size_t gnum)
{
  bool group = false;
  float v1[3];
  float v2[3];
  if(m_FeatureIds[neighborpoint] == 0 && (missingGoodVoxels == true || m_GoodVoxels[neighborpoint] == true))
  {
    v1[0] = m_Vectors[3*referencepoint+0];
    v1[1] = m_Vectors[3*referencepoint+1];
    v1[2] = m_Vectors[3*referencepoint+2];
    v2[0] = m_Vectors[3*neighborpoint+0];
    v2[1] = m_Vectors[3*neighborpoint+1];
    v2[2] = m_Vectors[3*neighborpoint+2];
    float w = GeometryMath::CosThetaBetweenVectors(v1, v2);
    w = acos(w);
    if(w < m_AngleTolerance || (DREAM3D::Constants::k_Pi-w) < m_AngleTolerance)
    {
      group = true;
      m_FeatureIds[neighborpoint] = gnum;
    }
  }

  return group;
}

