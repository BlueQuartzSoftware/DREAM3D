/* ============================================================================
 * Copyright (c) 2011 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2011 Dr. Michael A. Groeber (US Air Force Research Laboratories)
 * Copyright (c) 2014 Dr. Joseph C. Tucker (UES, Inc.)
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

#include "GroupFeatures.h"

#include "DREAM3DLib/Common/Constants.h"

#include "DREAM3DLib/Utilities/DREAM3DRandom.h"

#define ERROR_TXT_OUT 1
#define ERROR_TXT_OUT1 1




#define NEW_SHARED_ARRAY(var, m_msgType, size)\
  boost::shared_array<m_msgType> var##Array(new m_msgType[size]);\
  m_msgType* var = var##Array.get();

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GroupFeatures::GroupFeatures() :
  AbstractFilter(),
  m_ContiguousNeighborListArrayPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellFeatureAttributeMatrixName, DREAM3D::FeatureData::NeighborList),
  m_NonContiguousNeighborListArrayPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellFeatureAttributeMatrixName, DREAM3D::FeatureData::NeighborhoodList),
  m_UseNonContiguousNeighbors(false),
  m_PatchGrouping(false)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GroupFeatures::~GroupFeatures()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GroupFeatures::setupFilterParameters()
{
  FilterParameterVector parameters;

  QStringList linkedProps("NonContiguousNeighborListArrayPath");
  parameters.push_back(FilterParameter::NewConditional("Use Non-Contiguous Neighbors", "UseNonContiguousNeighbors", FilterParameterWidgetType::LinkedBooleanWidget, getUseNonContiguousNeighbors(), true, linkedProps));
  parameters.push_back(FilterParameter::New("NonContiguous NeighborList Array", "NonContiguousNeighborListArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getNonContiguousNeighborListArrayPath(), false, ""));

  parameters.push_back(FilterParameter::New("Required Information", "", FilterParameterWidgetType::SeparatorWidget, "QString", true));
  parameters.push_back(FilterParameter::New("Contiguous NeighborList Array", "ContiguousNeighborListArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getContiguousNeighborListArrayPath(), true, ""));

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GroupFeatures::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setUseNonContiguousNeighbors( reader->readValue("UseNonContiguousNeighbors", getUseNonContiguousNeighbors()) );
  setContiguousNeighborListArrayPath( reader->readDataArrayPath("ContiguousNeighborListArrayPath", getContiguousNeighborListArrayPath()));
  setNonContiguousNeighborListArrayPath( reader->readDataArrayPath("NonContiguousNeighborListArrayPath", getNonContiguousNeighborListArrayPath()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int GroupFeatures::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("UseNonContiguousNeighbors", getUseNonContiguousNeighbors() );
  writer->writeValue("ContiguousNeighborListArrayPath", getContiguousNeighborListArrayPath());
  writer->writeValue("NonContiguousNeighborListArrayPath", getNonContiguousNeighborListArrayPath());
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GroupFeatures::dataCheck()
{
  setErrorCondition(0);

  QVector<size_t> dims(1, 1);
  // Now we are going to get a "Pointer" to the NeighborList object out of the DataContainer
  m_ContiguousNeighborList = getDataContainerArray()->getPrereqArrayFromPath<NeighborList<int>, AbstractFilter>(this, getContiguousNeighborListArrayPath(), dims);

  if(m_UseNonContiguousNeighbors == true)
  {
    // Now we are going to get a "Pointer" to the NeighborList object out of the DataContainer
    m_NonContiguousNeighborList = getDataContainerArray()->getPrereqArrayFromPath<NeighborList<int>, AbstractFilter>(this, getNonContiguousNeighborListArrayPath(), dims);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GroupFeatures::preflight()
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
void GroupFeatures::execute()
{
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  // But since a pointer is difficult to use operators with we will now create a
  // reference variable to the pointer with the correct variable name that allows
  // us to use the same syntax as the "vector of vectors"
  NeighborList<int>& neighborlist = *(m_ContiguousNeighborList.lock());
  NeighborList<int>* nonContigNeighList = m_NonContiguousNeighborList.lock().get();

  QVector<int> grouplist;

  int parentcount = 0;
  int seed = 0;
  int list1size = 0, list2size = 0, listsize = 0;
  int neigh;

  while (seed >= 0)
  {
    parentcount++;
    seed = getSeed(parentcount);
    if(seed >= 0)
    {
      grouplist.push_back(seed);
      for (size_t j = 0; j < grouplist.size(); j++)
      {
        int firstfeature = grouplist[j];
        list1size = int(neighborlist[firstfeature].size());
        if (m_UseNonContiguousNeighbors == true) { list2size = nonContigNeighList->getListSize(firstfeature); }
        for (int k = 0; k < 2; k++)
        {
          if (m_PatchGrouping == true) { k = 1; }
          if (k == 0) { listsize = list1size; }
          else if (k == 1) { listsize = list2size; }
          for (int l = 0; l < listsize; l++)
          {
            //int twin = 0;
            if (k == 0) { neigh = neighborlist[firstfeature][l]; }
            else if (k == 1) { neigh = nonContigNeighList->getListReference(firstfeature)[l]; }
            if (neigh != firstfeature)
            {
              if(determineGrouping(firstfeature, neigh, parentcount) == true)
              {
                if (m_PatchGrouping == false) { grouplist.push_back(neigh); }
              }
            }
          }
        }
      }
      if (m_PatchGrouping == true)
      {
        if (growPatch(parentcount) == true)
        {
          for (size_t j = 0; j < grouplist.size(); j++)
          {
            int firstfeature = grouplist[j];
            listsize = int(neighborlist[firstfeature].size());
            for (int l = 0; l < listsize; l++)
            {
              neigh = neighborlist[firstfeature][l];
              if (neigh != firstfeature)
              {
                if(growGrouping(firstfeature, neigh, parentcount) == true)
                {
                  grouplist.push_back(neigh);
                }
              }
            }
          }
        }
      }
    }
    grouplist.clear();
  }

  // If there is an error set this to something negative and also set a message
  notifyStatusMessage(getHumanLabel(), "Completed");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int GroupFeatures::getSeed(int newFid)
{
  return -1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool GroupFeatures::determineGrouping(int referenceFeature, int neighborFeature, int newFid)
{
  return false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool GroupFeatures::growPatch(int currentPatch)
{
  return false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool GroupFeatures::growGrouping(int referenceFeature, int neighborFeature, int newFid)
{
  return false;
}


