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

#include "BadDataNeighborOrientationCheck.h"


#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Common/DREAM3DMath.h"
#include "DREAM3DLib/Common/DREAM3DRandom.h"

#include "DREAM3DLib/GenericFilters/FindGrainPhases.h"



#define NEW_SHARED_ARRAY(var, m_msgType, size)\
  boost::shared_array<m_msgType> var##Array(new m_msgType[size]);\
  m_msgType* var = var##Array.get();

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
BadDataNeighborOrientationCheck::BadDataNeighborOrientationCheck() :
AbstractFilter(),
m_QuatsArrayName(DREAM3D::CellData::Quats),
m_GoodVoxelsArrayName(DREAM3D::CellData::GoodVoxels),
m_CellPhasesArrayName(DREAM3D::CellData::Phases),
m_CrystalStructuresArrayName(DREAM3D::EnsembleData::CrystalStructures),
m_MisorientationTolerance(5.0f),
m_NumberOfNeighbors(6),
m_Quats(NULL),
m_GoodVoxels(NULL),
m_CellPhases(NULL),
m_CrystalStructures(NULL)
{
  m_OrientationOps = OrientationOps::getOrientationOpsVector();
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
BadDataNeighborOrientationCheck::~BadDataNeighborOrientationCheck()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BadDataNeighborOrientationCheck::setupFilterParameters()
{
  QVector<FilterParameter::Pointer> parameters;
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setPropertyName("MisorientationTolerance");
    option->setHumanLabel("Misorientation Tolerance");
    option->setWidgetType(FilterParameter::DoubleWidget);
    option->setValueType("float");
    option->setCastableValueType("double");
  option->setUnits("Degrees");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Required Number of Neighbors");
    option->setPropertyName("NumberOfNeighbors");
    option->setWidgetType(FilterParameter::IntWidget);
    option->setValueType("int");
  option->setUnits("");
    parameters.push_back(option);
  }

  setFilterParameters(parameters);
}
// -----------------------------------------------------------------------------
void BadDataNeighborOrientationCheck::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  /* Code to read the values goes between these statements */
/* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE BEGIN*/
  setMisorientationTolerance( reader->readValue("MisorientationTolerance", getMisorientationTolerance()) );
  setNumberOfNeighbors( reader->readValue("NumberOfNeighbors", getNumberOfNeighbors()) );
/* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE END*/
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int BadDataNeighborOrientationCheck::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("MisorientationTolerance", getMisorientationTolerance() );
  writer->writeValue("NumberofNeighbors", getNumberOfNeighbors() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BadDataNeighborOrientationCheck::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  
  VolumeDataContainer* m = getVolumeDataContainer();

  GET_PREREQ_DATA(m, DREAM3D, CellData, GoodVoxels, -301, bool, BoolArrayType, voxels, 1)
  GET_PREREQ_DATA(m, DREAM3D, CellData, Quats, -301, float, FloatArrayType, voxels, 4)
  GET_PREREQ_DATA(m, DREAM3D, CellData, CellPhases, -302, int32_t, Int32ArrayType,  voxels, 1)
  typedef DataArray<unsigned int> XTalStructArrayType;
  GET_PREREQ_DATA(m, DREAM3D, CellEnsembleData, CrystalStructures, -304, unsigned int, XTalStructArrayType, ensembles, 1)
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BadDataNeighborOrientationCheck::preflight()
{
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BadDataNeighborOrientationCheck::execute()
{
  setErrorCondition(0);
 // int err = 0;
  VolumeDataContainer* m = getVolumeDataContainer();
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The DataContainer Object was NULL", -999);
    return;
  }


  int64_t totalPoints = m->getTotalPoints();
  dataCheck(false, totalPoints, m->getNumCellFieldTuples(), m->getNumCellEnsembleTuples());
  if (getErrorCondition() < 0 && getErrorCondition() != -305)
  {
    return;
  }
  setErrorCondition(0);

  m_MisorientationTolerance = m_MisorientationTolerance*DREAM3D::Constants::k_Pi/180.0;

  size_t udims[3] = {0,0,0};
  m->getDimensions(udims);
#if (CMP_SIZEOF_SIZE_T == 4)
  typedef int32_t DimType;
#else
  typedef int64_t DimType;
#endif
  DimType dims[3] = {
    static_cast<DimType>(udims[0]),
    static_cast<DimType>(udims[1]),
    static_cast<DimType>(udims[2]),
  };

  //size_t count = 1;
  int good = 1;
  int neighbor;
  //float x, y, z;
  DimType column, row, plane;
 // int neighpoint;
//  size_t numgrains = m->getNumCellFieldTuples();

  int neighpoints[6];
  neighpoints[0] = static_cast<int>(-dims[0] * dims[1]);
  neighpoints[1] = static_cast<int>(-dims[0]);
  neighpoints[2] = static_cast<int>(-1);
  neighpoints[3] = static_cast<int>(1);
  neighpoints[4] = static_cast<int>(dims[0]);
  neighpoints[5] = static_cast<int>(dims[0] * dims[1]);

  float w = 10000.0;
  QuatF q1;
  QuatF q2;
  QuatF* quats = reinterpret_cast<QuatF*>(m_Quats);
  float n1, n2, n3;
  unsigned int phase1, phase2;

  QVector<int> neighborCount(totalPoints,0);

  for (int64_t i = 0; i < totalPoints; i++)
  {
    if(m_GoodVoxels[i] == false)
    {
    column = i % dims[0];
    row = (i / dims[0]) % dims[1];
    plane = i / (dims[0] * dims[1]);
    for (DimType j = 0; j < 6; j++)
    {
      good = 1;
      neighbor = i + neighpoints[j];
      if (j == 0 && plane == 0) good = 0;
      if (j == 5 && plane == (dims[2] - 1)) good = 0;
      if (j == 1 && row == 0) good = 0;
      if (j == 4 && row == (dims[1] - 1)) good = 0;
      if (j == 2 && column == 0) good = 0;
      if (j == 3 && column == (dims[0] - 1)) good = 0;
      if (good == 1 && m_GoodVoxels[neighbor] == true)
      {
      phase1 = m_CrystalStructures[m_CellPhases[i]];
      QuaternionMathF::Copy(quats[i], q1);

//      q1[0] = 1;
//      q1[1] = m_Quats[i * 5 + 1];
//      q1[2] = m_Quats[i * 5 + 2];
//      q1[3] = m_Quats[i * 5 + 3];
//      q1[4] = m_Quats[i * 5 + 4];

      phase2 = m_CrystalStructures[m_CellPhases[neighbor]];
      QuaternionMathF::Copy(quats[neighbor], q2);
//      q2[0] = 1;
//      q2[1] = m_Quats[neighbor*5 + 1];
//      q2[2] = m_Quats[neighbor*5 + 2];
//      q2[3] = m_Quats[neighbor*5 + 3];
//      q2[4] = m_Quats[neighbor*5 + 4];

      if (m_CellPhases[i] == m_CellPhases[neighbor] && m_CellPhases[i] > 0) w = m_OrientationOps[phase1]->getMisoQuat( q1, q2, n1, n2, n3);
      if (w < m_MisorientationTolerance)
      {
        neighborCount[i]++;
      }
      }
    }
    }
  }

  int currentLevel = 6;
  int counter;
  // int index;
  while(currentLevel > m_NumberOfNeighbors)
  {
    counter = 1;
    while(counter > 0)
    {
      counter = 0;
      for (size_t i = 0; i < totalPoints; i++)
      {
      if(neighborCount[i] >= currentLevel && m_GoodVoxels[i] == false)
      {
      m_GoodVoxels[i] = true;
      counter++;
      column = i % dims[0];
      row = (i / dims[0]) % dims[1];
      plane = i / (dims[0] * dims[1]);
      for (DimType j = 0; j < 6; j++)
      {
        good = 1;
        neighbor = i + neighpoints[j];
        if (j == 0 && plane == 0) good = 0;
        if (j == 5 && plane == (dims[2] - 1)) good = 0;
        if (j == 1 && row == 0) good = 0;
        if (j == 4 && row == (dims[1] - 1)) good = 0;
        if (j == 2 && column == 0) good = 0;
        if (j == 3 && column == (dims[0] - 1)) good = 0;
        if (good == 1 && m_GoodVoxels[neighbor] == false)
        {
        phase1 = m_CrystalStructures[m_CellPhases[i]];
        QuaternionMathF::Copy(quats[i], q1);
//        q1[0] = 1;
//        q1[1] = m_Quats[i * 5 + 1];
//        q1[2] = m_Quats[i * 5 + 2];
//        q1[3] = m_Quats[i * 5 + 3];
//        q1[4] = m_Quats[i * 5 + 4];

        phase2 = m_CrystalStructures[m_CellPhases[neighbor]];
        QuaternionMathF::Copy(quats[neighbor], q1);
//        q2[0] = 1;
//        q2[1] = m_Quats[neighbor*5 + 1];
//        q2[2] = m_Quats[neighbor*5 + 2];
//        q2[3] = m_Quats[neighbor*5 + 3];
//        q2[4] = m_Quats[neighbor*5 + 4];

        if (m_CellPhases[i] == m_CellPhases[neighbor] && m_CellPhases[i] > 0) w = m_OrientationOps[phase1]->getMisoQuat( q1, q2, n1, n2, n3);
        if (w < m_MisorientationTolerance)
        {
          neighborCount[neighbor]++;
        }
        }
      }
      }
    }
    }
    currentLevel = currentLevel-1;
  }

// If there is an error set this to something negative and also set a message
 notifyStatusMessage("Filling Bad Data Complete");
}
