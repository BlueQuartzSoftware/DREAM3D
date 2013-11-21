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

#include "ScalarSegmentFeatures.h"

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/variate_generator.hpp>

#include "DREAM3DLib/Common/Constants.h"

#include "DREAM3DLib/OrientationOps/OrientationOps.h"
#include "DREAM3DLib/Utilities/DREAM3DRandom.h"

#include "DREAM3DLib/OrientationOps/CubicOps.h"
#include "DREAM3DLib/OrientationOps/HexagonalOps.h"
#include "DREAM3DLib/OrientationOps/OrthoRhombicOps.h"

#include "DREAM3DLib/GenericFilters/FindCellQuats.h"

#define ERROR_TXT_OUT 1
#define ERROR_TXT_OUT1 1




#define NEW_SHARED_ARRAY(var, m_msgType, size)\
  boost::shared_array<m_msgType> var##Array(new m_msgType[size]);\
  m_msgType* var = var##Array.get();



/* from http://www.newty.de/fpt/functor.html */
// base class
class CompareFunctor
{
  public:
    virtual ~CompareFunctor() {}

    virtual bool operator()(size_t index, size_t neighIndex, size_t gnum)  // call using operator
    {
      return false;
    }
};


template<class T>
class TSpecificCompareFunctor : public CompareFunctor
{
  public:
    TSpecificCompareFunctor(void* data, size_t length, T tolerance, int32_t* featureIds) :
      m_Length(length),
      m_Tolerance(tolerance),
      m_FeatureIds(featureIds)
    {
      m_Data = reinterpret_cast<T*>(data);
    }
    virtual ~TSpecificCompareFunctor() {};

    virtual bool operator()(size_t referencepoint, size_t neighborpoint, size_t gnum)
    {
      // Sanity check the indices that are being passed in.
      if (referencepoint >= m_Length || neighborpoint >= m_Length) { return false; }

      if(m_Data[referencepoint] >= m_Data[neighborpoint])
      {
        if ((m_Data[referencepoint] - m_Data[neighborpoint]) <= m_Tolerance)
        {
          m_FeatureIds[neighborpoint] = gnum;
          return true;
        }
      }
      else
      {
        if ((m_Data[neighborpoint] - m_Data[referencepoint]) <= m_Tolerance)
        {
          m_FeatureIds[neighborpoint] = gnum;
          return true;
        }
      }
      return false;
    }

  protected:
    TSpecificCompareFunctor() {}

  private:
    T* m_Data;       // The data that is being compared
    size_t m_Length; // Length of the Data Array
    T      m_Tolerance; // The tolerance of the comparison
    int32_t* m_FeatureIds; // The feature Ids
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ScalarSegmentFeatures::ScalarSegmentFeatures() :
  SegmentFeatures(),
  m_DataContainerName(DREAM3D::HDF5::VolumeDataContainerName),
  m_ScalarArrayName(""),
  m_FeatureIdsArrayName(DREAM3D::CellData::FeatureIds),
  m_ActiveArrayName(DREAM3D::FeatureData::Active),
  m_ScalarTolerance(5.0f),
  m_RandomizeFeatureIds(true),
  m_FeatureIds(NULL),
  m_Active(NULL),
  m_Compare(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ScalarSegmentFeatures::~ScalarSegmentFeatures()
{
  if (m_Compare != NULL)
  {
    delete m_Compare;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ScalarSegmentFeatures::setupFilterParameters()
{
  FilterParameterVector parameters;
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Input Cell Array Name");
    option->setPropertyName("ScalarArrayName");
    option->setWidgetType(FilterParameter::VolumeCellArrayNameSelectionWidget);
    option->setValueType("string");
    option->setUnits("");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setPropertyName("ScalarTolerance");
    option->setHumanLabel("Scalar Tolerance");
    option->setWidgetType(FilterParameter::DoubleWidget);
    option->setValueType("float");
    option->setCastableValueType("double");
    option->setUnits("");
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
void ScalarSegmentFeatures::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  /* Code to read the values goes between these statements */
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE BEGIN*/
  setScalarArrayName( reader->readString( "ScalarArrayName", getScalarArrayName() ) );
  setScalarTolerance( reader->readValue("ScalarTolerance", getScalarTolerance()) );
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE END*/
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int ScalarSegmentFeatures::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("ScalarArrayName", getScalarArrayName() );
  writer->writeValue("ScalarTolerance", getScalarTolerance() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ScalarSegmentFeatures::dataCheck(bool preflight, size_t voxels, size_t features, size_t ensembles)
{
  setErrorCondition(0);
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

  if(m_ScalarArrayName.isEmpty() == true)
  {
    setErrorCondition(-11000);
    addErrorMessage(getHumanLabel(), "An array from the Volume DataContainer must be selected.", getErrorCondition());
  }

  QVector<int> dims(1, 1);
  m_FeatureIdsPtr = m->createNonPrereqArray<int32_t, AbstractFilter>(this, m_CellAttributeMatrixName,  m_FeatureIdsArrayName, 0, voxels, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeatureIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
{ m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  m_ActivePtr = m->createNonPrereqArray<bool, AbstractFilter>(this, m_CellFeatureAttributeMatrixName,  m_ActiveArrayName, true, features, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_ActivePtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
{ m_Active = m_ActivePtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ScalarSegmentFeatures::preflight()
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
void ScalarSegmentFeatures::execute()
{
  setErrorCondition(0);
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The DataContainer Object was NULL", -999);
    return;
  }

  int64_t totalPoints = m->getTotalPoints();
  m->resizeCellFeatureDataArrays(1);
  // This runs a subfilter
  dataCheck(false, totalPoints, m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->getNumTuples();
  if (getErrorCondition() < 0)
  {
    return;
  }

  m_InputData = m->getAttributeMatrix(getCellAttributeMatrixName())->getAttributeArray(m_ScalarArrayName);
  if (NULL == m_InputData.get())
  {

    QString ss = QObject::tr("Selected array '%1' does not exist in the Voxel Data Container. Was it spelled correctly?").arg(m_ScalarArrayName);
    setErrorCondition(-11001);
    notifyErrorMessage(ss, getErrorCondition());
    return;
  }

  // Tell the user we are starting the filter
  notifyStatusMessage("Starting");

  for(int64_t i = 0; i < totalPoints; i++)
  {
    m_FeatureIds[i] = 0;
  }

  QString dType = m_InputData->getTypeAsString();
  if(m_InputData->GetNumberOfComponents() != 1)
  {
    m_Compare = new CompareFunctor(); // The default CompareFunctor which ALWAYS returns false for the comparison
  }
  else if (dType.compare("int8_t") == 0)
  {
    m_Compare = new TSpecificCompareFunctor<int8_t>(m_InputData->GetVoidPointer(0), m_InputData->getNumberOfTuples(), m_ScalarTolerance, m_FeatureIds);
  }
  else if (dType.compare("uint8_t") == 0)
  {
    m_Compare =  new TSpecificCompareFunctor<uint8_t>(m_InputData->GetVoidPointer(0), m_InputData->getNumberOfTuples(), m_ScalarTolerance, m_FeatureIds);
  }
  else if (dType.compare("bool") == 0)
  {
    m_Compare =  new TSpecificCompareFunctor<bool>(m_InputData->GetVoidPointer(0), m_InputData->getNumberOfTuples(), m_ScalarTolerance, m_FeatureIds);
  }
  else if (dType.compare("int16_t") == 0)
  {
    m_Compare =  new TSpecificCompareFunctor<int16_t>(m_InputData->GetVoidPointer(0), m_InputData->getNumberOfTuples(), m_ScalarTolerance, m_FeatureIds);
  }
  else if (dType.compare("uint16_t") == 0)
  {
    m_Compare =  new TSpecificCompareFunctor<uint16_t>(m_InputData->GetVoidPointer(0), m_InputData->getNumberOfTuples(), m_ScalarTolerance, m_FeatureIds);
  }
  else if (dType.compare("int32_t") == 0)
  {
    m_Compare =  new TSpecificCompareFunctor<int32_t>(m_InputData->GetVoidPointer(0), m_InputData->getNumberOfTuples(), m_ScalarTolerance, m_FeatureIds);
  }
  else if (dType.compare("uint32_t") == 0)
  {
    m_Compare =  new TSpecificCompareFunctor<uint32_t>(m_InputData->GetVoidPointer(0), m_InputData->getNumberOfTuples(), m_ScalarTolerance, m_FeatureIds);
  }
  else if (dType.compare("int64_t") == 0)
  {
    m_Compare =  new TSpecificCompareFunctor<int64_t>(m_InputData->GetVoidPointer(0), m_InputData->getNumberOfTuples(), m_ScalarTolerance, m_FeatureIds);
  }
  else if (dType.compare("uint64_t") == 0)
  {
    m_Compare =  new TSpecificCompareFunctor<uint64_t>(m_InputData->GetVoidPointer(0), m_InputData->getNumberOfTuples(), m_ScalarTolerance, m_FeatureIds);
  }
  else if (dType.compare("float") == 0)
  {
    m_Compare =  new TSpecificCompareFunctor<float>(m_InputData->GetVoidPointer(0), m_InputData->getNumberOfTuples(), m_ScalarTolerance, m_FeatureIds);
  }
  else if (dType.compare("double") == 0)
  {
    m_Compare =  new TSpecificCompareFunctor<double>(m_InputData->GetVoidPointer(0), m_InputData->getNumberOfTuples(), m_ScalarTolerance, m_FeatureIds);
  }

  SegmentFeatures::execute();

  if (true == m_RandomizeFeatureIds)
  {
    totalPoints = m->getTotalPoints();
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
  notifyStatusMessage("Completed");
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int64_t ScalarSegmentFeatures::getSeed(size_t gnum)
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
    if (m_FeatureIds[randpoint] == 0) { seed = randpoint; }
    randpoint++;
    counter++;
  }
  if (seed >= 0)
  {
    m_FeatureIds[seed] = gnum;
    m->resizeCellFeatureDataArrays(gnum + 1);
    dataCheck(false, totalPoints, m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->getNumTuples();
  }
  return seed;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool ScalarSegmentFeatures::determineGrouping(int64_t referencepoint, int64_t neighborpoint, size_t gnum)
{

  if(m_FeatureIds[neighborpoint] > 0) { return false; }
  return (*m_Compare)( (size_t)(referencepoint), (size_t)(neighborpoint), gnum );
  //     | Functor  ||calling the operator() method of the CompareFunctor Class |

}
