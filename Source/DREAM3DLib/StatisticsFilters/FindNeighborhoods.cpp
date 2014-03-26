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

#include "FindNeighborhoods.h"

#include "DREAM3DLib/Math/DREAM3DMath.h"
#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/StatisticsFilters/FindSizes.h"
#include "DREAM3DLib/GenericFilters/FindGrainPhases.h"
#include "DREAM3DLib/GenericFilters/FindGrainCentroids.h"



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindNeighborhoods::FindNeighborhoods() :
  AbstractFilter(),
  m_CentroidsArrayName(DREAM3D::FieldData::Centroids),
  m_EquivalentDiametersArrayName(DREAM3D::FieldData::EquivalentDiameters),
  m_FieldPhasesArrayName(DREAM3D::FieldData::Phases),
  m_NeighborhoodsArrayName(DREAM3D::FieldData::Neighborhoods),
  m_NeighborhoodListArrayName(DREAM3D::FieldData::NeighborhoodList),
  m_NumNeighborsArrayName(DREAM3D::FieldData::NumNeighbors),
  m_MultiplesOfAverage(1.0f),
  m_FieldPhases(NULL),
  m_Centroids(NULL),
  m_EquivalentDiameters(NULL),
  m_Neighborhoods(NULL),
  m_NeighborhoodList(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindNeighborhoods::~FindNeighborhoods()
{
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindNeighborhoods::setupFilterParameters()
{
  std::vector<FilterParameter::Pointer> parameters;
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setPropertyName("MultiplesOfAverage");
    option->setHumanLabel("Multiples Of Average Diameter");
    option->setWidgetType(FilterParameter::DoubleWidget);
    option->setValueType("float");
    option->setCastableValueType("double");
    option->setUnits("");
    parameters.push_back(option);
  }

  setFilterParameters(parameters);
}
// -----------------------------------------------------------------------------
void FindNeighborhoods::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  /* Code to read the values goes between these statements */
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE BEGIN*/
  setMultiplesOfAverage( reader->readValue("MultiplesOfAverage", getMultiplesOfAverage()) );
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE END*/
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int FindNeighborhoods::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("MultiplesOfAverage", getMultiplesOfAverage() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindNeighborhoods::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;
  VoxelDataContainer* m = getVoxelDataContainer();

  // Field Data
  // Do this whole block FIRST otherwise the side effect is that a call to m->getNumFieldTuples will = 0
  // because we are just creating an empty NeighborList object.
  // Now we are going to get a "Pointer" to the NeighborList object out of the DataContainer
  m_NeighborhoodList = NeighborList<int>::SafeObjectDownCast<IDataArray*, NeighborList<int>* >
                       (m->getFieldData(m_NeighborhoodListArrayName).get());
  if(m_NeighborhoodList == NULL)
  {
    NeighborList<int>::Pointer neighborhoodlistPtr = NeighborList<int>::New();
    neighborhoodlistPtr->SetName(m_NeighborhoodListArrayName);
    neighborhoodlistPtr->Resize(fields);
    neighborhoodlistPtr->setNumNeighborsArrayName(m_NeighborhoodsArrayName);
    m->addFieldData(m_NeighborhoodListArrayName, neighborhoodlistPtr);
    if (neighborhoodlistPtr.get() == NULL)
    {
      ss << "NeighborhoodLists Array Not Initialized at Beginning of FindNeighbors Filter" << std::endl;
      setErrorCondition(-308);
    }
    m_NeighborhoodList = NeighborList<int>::SafeObjectDownCast<IDataArray*, NeighborList<int>* >
                         (m->getFieldData(m_NeighborhoodListArrayName).get());

    CreatedArrayHelpIndexEntry::Pointer e = CreatedArrayHelpIndexEntry::New();
    e->setFilterName(this->getNameOfClass());
    e->setFilterHumanLabel(this->getHumanLabel());
    e->setFilterGroup(this->getGroupName());
    e->setFilterSubGroup(this->getSubGroupName());
    e->setArrayDefaultName(m_NeighborhoodListArrayName);
    e->setArrayGroup("Field");
    e->setArrayNumComponents(0);
    e->setArrayType("NeighborList");
    addCreatedArrayHelpIndexEntry(e);
  }

  GET_PREREQ_DATA(m, DREAM3D, FieldData, EquivalentDiameters, ss, -302, float, FloatArrayType, fields, 1)

  GET_PREREQ_DATA(m, DREAM3D, FieldData, FieldPhases, ss, -304, int32_t, Int32ArrayType, fields, 1)

  GET_PREREQ_DATA(m, DREAM3D, FieldData, Centroids, ss, -305, float, FloatArrayType, fields, 3)

  CREATE_NON_PREREQ_DATA(m, DREAM3D, FieldData, Neighborhoods, ss, int32_t, Int32ArrayType, 0, fields, 1)
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindNeighborhoods::preflight()
{
  dataCheck(true, 1, 1, 1);
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindNeighborhoods::execute()
{
  VoxelDataContainer* m = getVoxelDataContainer();
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The DataContainer Object was NULL", -999);
    return;
  }
  setErrorCondition(0);

  dataCheck(false, m->getTotalPoints(), m->getNumFieldTuples(), m->getNumEnsembleTuples());
  if (getErrorCondition() < 0)
  {
    return;
  }

  find_neighborhoods();
  notifyStatusMessage("FindNeighborhoods Completed");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindNeighborhoods::find_neighborhoods()
{
  std::stringstream ss;
  VoxelDataContainer* m = getVoxelDataContainer();

  float x, y, z;

  std::vector<std::vector<int> > neighborhoodlist;
  std::vector<float> criticalDistance;

  int totalFields = int(m->getNumFieldTuples());

  neighborhoodlist.resize(totalFields);
  criticalDistance.resize(totalFields);

  float aveDiam = 0.0f;
  for (size_t i = 1; i < totalFields; i++)
  {
    m_Neighborhoods[i] = 0;
    aveDiam += m_EquivalentDiameters[i];
    criticalDistance[i] = m_EquivalentDiameters[i] * m_MultiplesOfAverage;
  }
  aveDiam /= totalFields;
  for (size_t i = 1; i < totalFields; i++)
  {
    criticalDistance[i] /= aveDiam;
  }

  float m_OriginX, m_OriginY, m_OriginZ;
  m->getOrigin(m_OriginX, m_OriginY, m_OriginZ);

  int xbin, ybin, zbin; //, bin, bin1, bin2;
  std::vector<size_t> bins(3*totalFields, 0);
  for (size_t i = 1; i < totalFields; i++)
  {
    x = m_Centroids[3 * i];
    y = m_Centroids[3 * i + 1];
    z = m_Centroids[3 * i + 2];
    xbin = int((x - m_OriginX) / aveDiam);
    ybin = int((y - m_OriginY) / aveDiam);
    zbin = int((z - m_OriginZ) / aveDiam);
    bins[3*i] = xbin;
    bins[3*i+1] = ybin;
    bins[3*i+2] = zbin;
  }

  int bin1x, bin2x, bin1y, bin2y, bin1z, bin2z;
  int dBinX, dBinY, dBinZ;
  int criticalDistance1, criticalDistance2;
  for (size_t i = 1; i < totalFields; i++)
  {
    if (i % 1000 == 0)
    {
      ss.str("");
      ss << "Working On Grain " << i << " of " << totalFields;
      notifyStatusMessage(ss.str());
    }
    bin1x = bins[3*i];
    bin1y = bins[3*i+1];
    bin1z = bins[3*i+2];
    criticalDistance1 = criticalDistance[i];

    for (size_t j = i + 1; j < totalFields; j++)
    {
      bin2x = bins[3*j];
      bin2y = bins[3*j+1];
      bin2z = bins[3*j+2];
      criticalDistance2 = criticalDistance[j];

      dBinX = abs(bin2x - bin1x);
      dBinY = abs(bin2y - bin1y);
      dBinZ = abs(bin2z - bin1z);

      if (dBinX < criticalDistance1 && dBinY < criticalDistance1 && dBinZ < criticalDistance1)
      {
        m_Neighborhoods[i]++;
        neighborhoodlist[i].push_back(j);
      }

      if (dBinX < criticalDistance2 && dBinY < criticalDistance2 && dBinZ < criticalDistance2)
      {
        m_Neighborhoods[j]++;
        neighborhoodlist[j].push_back(i);
      }
    }
  }
  for (size_t i = 1; i < totalFields; i++)
  {
    // Set the vector for each list into the NeighborhoodList Object
    NeighborList<int>::SharedVectorType sharedNeiLst(new std::vector<int>);
    sharedNeiLst->assign(neighborhoodlist[i].begin(), neighborhoodlist[i].end());
    m_NeighborhoodList->setList(static_cast<int>(i), sharedNeiLst);
  }
}
