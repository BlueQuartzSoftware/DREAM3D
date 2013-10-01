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
m_MultiplesOfAverage(1),
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
  QVector<FilterParameter::Pointer> parameters;
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setPropertyName("MultiplesOfAverage");
    option->setHumanLabel("Multiples Of Average Diameter");
    option->setWidgetType(FilterParameter::IntWidget);
    option->setValueType("int");
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
  VolumeDataContainer* m = getVolumeDataContainer();

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
    if (neighborhoodlistPtr.get() == NULL) {
      QString ss = QObject::tr("NeighborLists Array Not Initialized correctly");
      setErrorCondition(-304);
      addErrorMessage(getHumanLabel(), ss, -1);
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

  GET_PREREQ_DATA(m, DREAM3D, FieldData, EquivalentDiameters, -302, float, FloatArrayType, fields, 1)

  GET_PREREQ_DATA(m, DREAM3D, FieldData, FieldPhases, -304, int32_t, Int32ArrayType, fields, 1)

  GET_PREREQ_DATA(m, DREAM3D, FieldData, Centroids, -305, float, FloatArrayType, fields, 3)

  CREATE_NON_PREREQ_DATA(m, DREAM3D, FieldData, Neighborhoods, int32_t, Int32ArrayType, 0, fields, 1)
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
  VolumeDataContainer* m = getVolumeDataContainer();
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
  VolumeDataContainer* m = getVolumeDataContainer();

  float x, y, z;
  float xn, yn, zn;
  float dx, dy, dz;

  std::vector<std::vector<int> > neighborhoodlist;

  int totalFields = int(m->getNumFieldTuples());

  neighborhoodlist.resize(totalFields);

  float aveDiam = 0;
  for (size_t i = 1; i < totalFields; i++)
  {
    m_Neighborhoods[i] = 0;
    aveDiam += m_EquivalentDiameters[i];
  }
  aveDiam /= totalFields;
  float criticalDistance = aveDiam*m_MultiplesOfAverage;

  float m_OriginX, m_OriginY, m_OriginZ;
  m->getOrigin(m_OriginX, m_OriginY, m_OriginZ);
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

  size_t xP = dims[0];
  size_t yP = dims[1];
//  size_t zP = dims[2];
  float xRes = m->getXRes();
  float yRes = m->getYRes();
//  float zRes = m->getZRes();
  float sizeX = float(xP)*xRes;
  float sizeY = float(yP)*yRes;
//  float sizeZ = float(zP)*zRes;
  int numXBins = int(sizeX/criticalDistance);
  int numYBins = int(sizeY/criticalDistance);
//  int numZBins = int(sizeZ/criticalDistance);

  int xbin, ybin, zbin, bin, bin1, bin2;
  std::vector<size_t> bins(totalFields, 0);
  for (size_t i = 1; i < totalFields; i++)
  {
      x = m_Centroids[3*i];
      y = m_Centroids[3*i+1];
      z = m_Centroids[3*i+2];
      xbin = int((x-m_OriginX)/criticalDistance);
      ybin = int((y-m_OriginY)/criticalDistance);
      zbin = int((z-m_OriginZ)/criticalDistance);
      bin = (zbin*numXBins*numYBins)+(ybin*numXBins)+(xbin);
      bins[i] = bin;
  }
  for (size_t i = 1; i < totalFields; i++)
  {
    if (i%1000 == 0)
    {
      QString ss = QObject::tr("Working On Grain %1 of %2").arg(i).arg(totalFields);
      notifyStatusMessage(ss);
    }
    x = m_Centroids[3*i];
    y = m_Centroids[3*i+1];
    z = m_Centroids[3*i+2];
    bin1 = bins[i];
    for (size_t j = i+1; j < totalFields; j++)
    {
      bin2 = bins[j];
      if(bin1 == bin2)
      {
          m_Neighborhoods[i]++;
          neighborhoodlist[i].push_back(j);
          m_Neighborhoods[j]++;
          neighborhoodlist[j].push_back(i);
      }
      else if(abs(bin1-bin2) == 1 || abs(bin1-bin2) == numXBins || abs(bin1-bin2) == (numXBins*numYBins))
      {
        xn = m_Centroids[3*j];
        yn = m_Centroids[3*j+1];
        zn = m_Centroids[3*j+2];
        dx = fabs(x - xn);
        dy = fabs(y - yn);
        dz = fabs(z - zn);
        if (dx < criticalDistance && dy < criticalDistance && dz < criticalDistance)
        {
          m_Neighborhoods[i]++;
          neighborhoodlist[i].push_back(j);
          m_Neighborhoods[j]++;
          neighborhoodlist[j].push_back(i);
        }
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
