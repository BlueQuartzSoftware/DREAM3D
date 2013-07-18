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

#include "ScalarSegmentGrains.h"

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/variate_generator.hpp>

#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Common/DREAM3DMath.h"
#include "DREAM3DLib/OrientationOps/OrientationOps.h"
#include "DREAM3DLib/Common/DREAM3DRandom.h"

#include "DREAM3DLib/OrientationOps/CubicOps.h"
#include "DREAM3DLib/OrientationOps/HexagonalOps.h"
#include "DREAM3DLib/OrientationOps/OrthoRhombicOps.h"

#include "DREAM3DLib/GenericFilters/FindCellQuats.h"

#define ERROR_TXT_OUT 1
#define ERROR_TXT_OUT1 1

const static float m_pi = static_cast<float>(M_PI);


#define NEW_SHARED_ARRAY(var, m_msgType, size)\
  boost::shared_array<m_msgType> var##Array(new m_msgType[size]);\
  m_msgType* var = var##Array.get();



/* from http://www.newty.de/fpt/functor.html */
// base class
class CompareFunctor
{
public:
  virtual ~CompareFunctor(){}

  virtual bool operator()(size_t index, size_t neighIndex, size_t gnum)  // call using operator
  {
    return false;
  }
};


template<class T>
class TSpecificCompareFunctor : public CompareFunctor
{
  public:
    TSpecificCompareFunctor(void* data, size_t length, T tolerance, int32_t* grainIds) :
      m_Length(length),
      m_Tolerance(tolerance),
      m_GrainIds(grainIds)
      {
        m_Data = reinterpret_cast<T*>(data);
      }
    virtual ~TSpecificCompareFunctor(){};

    virtual bool operator()(size_t referencepoint, size_t neighborpoint, size_t gnum)
    {
      // Sanity check the indices that are being passed in.
      if (referencepoint >= m_Length || neighborpoint >= m_Length) { return false; }
      
      if(m_Data[referencepoint] >= m_Data[neighborpoint])
      {
        if ((m_Data[referencepoint]-m_Data[neighborpoint]) <= m_Tolerance) { 
          m_GrainIds[neighborpoint] = gnum;
          return true; 
        }
      }
      else
      {
        if ((m_Data[neighborpoint]-m_Data[referencepoint]) <= m_Tolerance) { 
          m_GrainIds[neighborpoint] = gnum;
          return true; 
        }
      }
      return false;
    }
  
protected:
   TSpecificCompareFunctor(){}

private:
   T* m_Data;       // The data that is being compared
   size_t m_Length; // Length of the Data Array
   T      m_Tolerance; // The tolerance of the comparison
   int32_t* m_GrainIds; // The grain Ids
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ScalarSegmentGrains::ScalarSegmentGrains() :
SegmentGrains(),
m_ScalarArrayName(""),
m_GrainIdsArrayName(DREAM3D::CellData::GrainIds),
m_ActiveArrayName(DREAM3D::FieldData::Active),
m_ScalarTolerance(5.0f),
m_RandomizeGrainIds(true),
m_GrainIds(NULL),
m_Active(NULL),
m_Compare(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ScalarSegmentGrains::~ScalarSegmentGrains()
{
  if (m_Compare != NULL) {
    delete m_Compare;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ScalarSegmentGrains::setupFilterParameters()
{
  std::vector<FilterParameter::Pointer> parameters;
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Input Cell Array Name");
    option->setPropertyName("ScalarArrayName");
    option->setWidgetType(FilterParameter::VoxelCellArrayNameSelectionWidget);
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
    option->setHumanLabel("Randomly Reorder Generated Grain Ids");
    option->setPropertyName("RandomizeGrainIds");
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
void ScalarSegmentGrains::readFilterParameters(AbstractFilterParametersReader* reader)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ScalarSegmentGrains::writeFilterParameters(AbstractFilterParametersWriter* writer)

{
  writer->writeValue("ScalarArrayName", getScalarArrayName() );
  writer->writeValue("ScalarTolerance", getScalarTolerance() );
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ScalarSegmentGrains::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;
  VoxelDataContainer* m = getVoxelDataContainer();
  //int err = 0;

  if(m_ScalarArrayName.empty() == true)
  {
    setErrorCondition(-11000);
    addErrorMessage(getHumanLabel(), "An array from the Voxel Data Container must be selected.", getErrorCondition());
  }

  CREATE_NON_PREREQ_DATA(m, DREAM3D, CellData, GrainIds, ss, int32_t, Int32ArrayType, 0, voxels, 1)
  CREATE_NON_PREREQ_DATA(m, DREAM3D, FieldData, Active, ss, bool, BoolArrayType, true, fields, 1)

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ScalarSegmentGrains::preflight()
{
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ScalarSegmentGrains::execute()
{
  setErrorCondition(0);
  VoxelDataContainer* m = getVoxelDataContainer();
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The DataContainer Object was NULL", -999);
    return;
  }

  int64_t totalPoints = m->getTotalPoints();
  m->resizeFieldDataArrays(1);
  // This runs a subfilter
  dataCheck(false, totalPoints, m->getNumFieldTuples(), m->getNumEnsembleTuples());
  if (getErrorCondition() < 0)
  {
    return;
  }

  std::stringstream ss;

  m_InputData = m->getCellData(m_ScalarArrayName);
  if (NULL == m_InputData.get())
  {
    ss.str("");
    ss << "Selected array '" << m_ScalarArrayName << "' does not exist in the Voxel Data Container. Was it spelled correctly?";
    setErrorCondition(-11001);
    notifyErrorMessage(ss.str(), getErrorCondition());
    return;
  }

  // Tell the user we are starting the filter
  notifyStatusMessage("Starting");

  for(int64_t i=0;i<totalPoints;i++)
  {
    m_GrainIds[i] = 0;
  }

  std::string dType = m_InputData->getTypeAsString();
  if(m_InputData->GetNumberOfComponents() != 1)
  {
    m_Compare = new CompareFunctor(); // The default CompareFunctor which ALWAYS returns false for the comparison
  }
  else if (dType.compare("int8_t") == 0)
  {
    m_Compare = new TSpecificCompareFunctor<int8_t>(m_InputData->GetVoidPointer(0), m_InputData->GetNumberOfTuples(), m_ScalarTolerance, m_GrainIds);
  }
  else if (dType.compare("uint8_t") == 0)
  {
    m_Compare =  new TSpecificCompareFunctor<uint8_t>(m_InputData->GetVoidPointer(0), m_InputData->GetNumberOfTuples(), m_ScalarTolerance, m_GrainIds);
  }
  else if (dType.compare("bool") == 0)
  {
    m_Compare =  new TSpecificCompareFunctor<bool>(m_InputData->GetVoidPointer(0), m_InputData->GetNumberOfTuples(), m_ScalarTolerance, m_GrainIds);
  }
  else if (dType.compare("int16_t") == 0)
  {
    m_Compare =  new TSpecificCompareFunctor<int16_t>(m_InputData->GetVoidPointer(0), m_InputData->GetNumberOfTuples(), m_ScalarTolerance, m_GrainIds);
  }
  else if (dType.compare("uint16_t") == 0)
  {
    m_Compare =  new TSpecificCompareFunctor<uint16_t>(m_InputData->GetVoidPointer(0), m_InputData->GetNumberOfTuples(), m_ScalarTolerance, m_GrainIds);
  }
  else if (dType.compare("int32_t") == 0)
  {
    m_Compare =  new TSpecificCompareFunctor<int32_t>(m_InputData->GetVoidPointer(0), m_InputData->GetNumberOfTuples(), m_ScalarTolerance, m_GrainIds);
  }
  else if (dType.compare("uint32_t") == 0)
  {
    m_Compare =  new TSpecificCompareFunctor<uint32_t>(m_InputData->GetVoidPointer(0), m_InputData->GetNumberOfTuples(), m_ScalarTolerance, m_GrainIds);
  }
  else if (dType.compare("int64_t") == 0)
  {
    m_Compare =  new TSpecificCompareFunctor<int64_t>(m_InputData->GetVoidPointer(0), m_InputData->GetNumberOfTuples(), m_ScalarTolerance, m_GrainIds);
  }
  else if (dType.compare("uint64_t") == 0)
  {
    m_Compare =  new TSpecificCompareFunctor<uint64_t>(m_InputData->GetVoidPointer(0), m_InputData->GetNumberOfTuples(), m_ScalarTolerance, m_GrainIds);
  }
  else if (dType.compare("float") == 0)
  {
    m_Compare =  new TSpecificCompareFunctor<float>(m_InputData->GetVoidPointer(0), m_InputData->GetNumberOfTuples(), m_ScalarTolerance, m_GrainIds);
  }
  else if (dType.compare("double") == 0)
  {
    m_Compare =  new TSpecificCompareFunctor<double>(m_InputData->GetVoidPointer(0), m_InputData->GetNumberOfTuples(), m_ScalarTolerance, m_GrainIds);
  }

  SegmentGrains::execute();

  if (true == m_RandomizeGrainIds)
  {
    totalPoints = m->getTotalPoints();
    size_t totalFields = m->getNumFieldTuples();

    // Generate all the numbers up front
    const int rangeMin = 1;
    const int rangeMax = totalFields - 1;
    typedef boost::uniform_int<int> NumberDistribution;
    typedef boost::mt19937 RandomNumberGenerator;
    typedef boost::variate_generator<RandomNumberGenerator&,
                                     NumberDistribution> Generator;

    NumberDistribution distribution(rangeMin, rangeMax);
    RandomNumberGenerator generator;
    Generator numberGenerator(generator, distribution);
    generator.seed(static_cast<boost::uint32_t>( MXA::getMilliSeconds() )); // seed with the current time

    DataArray<int32_t>::Pointer rndNumbers = DataArray<int32_t>::CreateArray(totalFields, "New GrainIds");
    int32_t* gid = rndNumbers->GetPointer(0);
    gid[0] = 0;
    std::set<int32_t> grainIdSet;
    grainIdSet.insert(0);
    for(size_t i = 1; i < totalFields; ++i)
    {
      gid[i] = i; //numberGenerator();
      grainIdSet.insert(gid[i]);
    }

    size_t r;
    size_t temp;
    //--- Shuffle elements by randomly exchanging each with one other.
    for (size_t i=1; i< totalFields; i++) {
        r = numberGenerator(); // Random remaining position.
        if (r >= totalFields) {
          continue;
        }
        temp = gid[i];
        gid[i] = gid[r];
        gid[r] = temp;
    }

    // Now adjust all the Grain Id values for each Voxel
    for(int64_t i = 0; i < totalPoints; ++i)
    {
       m_GrainIds[i] = gid[ m_GrainIds[i] ];
    }
  }

  // If there is an error set this to something negative and also set a message
 notifyStatusMessage("Completed");
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int ScalarSegmentGrains::getSeed(size_t gnum)
{
  setErrorCondition(0);
  VoxelDataContainer* m = getVoxelDataContainer();
  if (NULL == m)
  {
    setErrorCondition(-1);
    std::stringstream ss;
    ss << " DataContainer was NULL";
    addErrorMessage(getHumanLabel(), ss.str(), -1);
    return -1;
  }

  int64_t totalPoints = m->getTotalPoints();

  DREAM3D_RANDOMNG_NEW()
  int seed = -1;
  int randpoint = 0;

  // Pre-calculate some constants
  int64_t totalPMinus1 = totalPoints - 1;

  int counter = 0;
  randpoint = int(float(rg.genrand_res53()) * float(totalPMinus1));
  while (seed == -1 && counter < totalPoints)
  {
      if (randpoint > totalPMinus1) randpoint = static_cast<int>( randpoint - totalPoints );
      if (m_GrainIds[randpoint] == 0) seed = randpoint;
      randpoint++;
      counter++;
  }
  if (seed >= 0)
  {
    m_GrainIds[seed] = gnum;
    m->resizeFieldDataArrays(gnum+1);
    dataCheck(false, totalPoints, m->getNumFieldTuples(), m->getNumEnsembleTuples());
  }
  return seed;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool ScalarSegmentGrains::determineGrouping(int referencepoint, int neighborpoint, size_t gnum)
{
  if(m_GrainIds[neighborpoint] > 0) { return false; }
  return (*m_Compare)( (size_t)(referencepoint), (size_t)(neighborpoint), gnum );
  //     | Functor  ||calling the operator() method of the CompareFunctor Class |
}
