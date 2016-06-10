/* ============================================================================
* Copyright (c) 2009-2016 BlueQuartz Software, LLC
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
* Neither the name of BlueQuartz Software, the US Air Force, nor the names of its
* contributors may be used to endorse or promote products derived from this software
* without specific prior written permission.
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
* The code contained herein was partially funded by the followig contracts:
*    United States Air Force Prime Contract FA8650-07-D-5800
*    United States Air Force Prime Contract FA8650-10-D-5210
*    United States Prime Contract Navy N00173-07-C-2068
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "GroupFeatures.h"

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/SIMPLibVersion.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersWriter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedBooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"


// Include the MOC generated file for this class
#include "moc_GroupFeatures.cpp"



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GroupFeatures::GroupFeatures() :
  AbstractFilter(),
  m_ContiguousNeighborListArrayPath("", "", ""),
  m_NonContiguousNeighborListArrayPath("", "", ""),
  m_UseNonContiguousNeighbors(false),
  m_PatchGrouping(false)
{
  m_ContiguousNeighborList = NeighborList<int32_t>::NullPointer();
  m_NonContiguousNeighborList = NeighborList<int32_t>::NullPointer();

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
  parameters.push_back(LinkedBooleanFilterParameter::New("Use Non-Contiguous Neighbors", "UseNonContiguousNeighbors", getUseNonContiguousNeighbors(), linkedProps, FilterParameter::Parameter));
  parameters.push_back(SeparatorFilterParameter::New("Feature Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateCategoryRequirement(SIMPL::TypeNames::NeighborList, 1, SIMPL::AttributeMatrixObjectType::Feature);
    parameters.push_back(DataArraySelectionFilterParameter::New("Non-Contiguous Neighbor List", "NonContiguousNeighborListArrayPath", getNonContiguousNeighborListArrayPath(), FilterParameter::RequiredArray, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateCategoryRequirement(SIMPL::TypeNames::NeighborList, 1, SIMPL::AttributeMatrixObjectType::Feature);
    parameters.push_back(DataArraySelectionFilterParameter::New("Contiguous Neighbor List", "ContiguousNeighborListArrayPath", getContiguousNeighborListArrayPath(), FilterParameter::RequiredArray, req));
  }
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
  SIMPL_FILTER_WRITE_PARAMETER(FilterVersion)
  SIMPL_FILTER_WRITE_PARAMETER(UseNonContiguousNeighbors)
  SIMPL_FILTER_WRITE_PARAMETER(ContiguousNeighborListArrayPath)
  SIMPL_FILTER_WRITE_PARAMETER(NonContiguousNeighborListArrayPath)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GroupFeatures::initialize()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GroupFeatures::dataCheck()
{
  setErrorCondition(0);

  QVector<size_t> cDims(1, 1);
  m_ContiguousNeighborList = getDataContainerArray()->getPrereqArrayFromPath<NeighborList<int32_t>, AbstractFilter>(this, getContiguousNeighborListArrayPath(), cDims);

  if(m_UseNonContiguousNeighbors == true)
  {
    m_NonContiguousNeighborList = getDataContainerArray()->getPrereqArrayFromPath<NeighborList<int32_t>, AbstractFilter>(this, getNonContiguousNeighborListArrayPath(), cDims);
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
int GroupFeatures::getSeed(int32_t newFid)
{
  return -1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool GroupFeatures::determineGrouping(int32_t referenceFeature, int32_t neighborFeature, int32_t newFid)
{
  return false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool GroupFeatures::growPatch(int32_t currentPatch)
{
  return false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool GroupFeatures::growGrouping(int32_t referenceFeature, int32_t neighborFeature, int32_t newFid)
{
  return false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GroupFeatures::execute()
{
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  NeighborList<int32_t>& neighborlist = *(m_ContiguousNeighborList.lock());
  NeighborList<int32_t>* nonContigNeighList = m_NonContiguousNeighborList.lock().get();

  std::vector<int32_t> grouplist;

  int32_t parentcount = 0;
  int32_t seed = 0;
  int32_t list1size = 0, list2size = 0, listsize = 0;
  int32_t neigh = 0;

  while (seed >= 0)
  {
    parentcount++;
    seed = getSeed(parentcount);
    if (seed >= 0)
    {
      grouplist.push_back(seed);
      for (std::vector<int32_t>::size_type j = 0; j < grouplist.size(); j++)
      {
        int32_t firstfeature = grouplist[j];
        list1size = int32_t(neighborlist[firstfeature].size());
        if (m_UseNonContiguousNeighbors == true) { list2size = nonContigNeighList->getListSize(firstfeature); }
        for (int32_t k = 0; k < 2; k++)
        {
          if (m_PatchGrouping == true) { k = 1; }
          if (k == 0) { listsize = list1size; }
          else if (k == 1) { listsize = list2size; }
          for (int32_t l = 0; l < listsize; l++)
          {
            if (k == 0) { neigh = neighborlist[firstfeature][l]; }
            else if (k == 1) { neigh = nonContigNeighList->getListReference(firstfeature)[l]; }
            if (neigh != firstfeature)
            {
              if (determineGrouping(firstfeature, neigh, parentcount) == true)
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
          for (std::vector<int32_t>::size_type j = 0; j < grouplist.size(); j++)
          {
            int32_t firstfeature = grouplist[j];
            listsize = int32_t(neighborlist[firstfeature].size());
            for (int32_t l = 0; l < listsize; l++)
            {
              neigh = neighborlist[firstfeature][l];
              if (neigh != firstfeature)
              {
                if (growGrouping(firstfeature, neigh, parentcount) == true)
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
  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer GroupFeatures::newFilterInstance(bool copyFilterParameters)
{
  GroupFeatures::Pointer filter = GroupFeatures::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString GroupFeatures::getCompiledLibraryName()
{
  return ReconstructionConstants::ReconstructionBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString GroupFeatures::getBrandingString()
{
  return "Reconstruction";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString GroupFeatures::getFilterVersion()
{
  QString version;
  QTextStream vStream(&version);
  vStream <<  SIMPLib::Version::Major() << "." << SIMPLib::Version::Minor() << "." << SIMPLib::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString GroupFeatures::getGroupName()
{ return SIMPL::FilterGroups::ReconstructionFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString GroupFeatures::getSubGroupName()
{ return SIMPL::FilterSubGroups::SegmentationFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString GroupFeatures::getHumanLabel()
{ return "Group Features"; }
