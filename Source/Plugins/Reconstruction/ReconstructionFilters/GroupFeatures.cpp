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

#include <memory>

#include "GroupFeatures.h"

#include <QtCore/QTextStream>

#include "SIMPLib/Common/Constants.h"

#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedBooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"

#include "Reconstruction/ReconstructionVersion.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GroupFeatures::GroupFeatures()
: m_ContiguousNeighborListArrayPath("", "", "")
, m_NonContiguousNeighborListArrayPath("", "", "")
, m_UseNonContiguousNeighbors(false)
, m_PatchGrouping(false)
{
  m_ContiguousNeighborList = NeighborList<int32_t>::NullPointer();
  m_NonContiguousNeighborList = NeighborList<int32_t>::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GroupFeatures::~GroupFeatures() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GroupFeatures::setupFilterParameters()
{
  FilterParameterVectorType parameters;
  QStringList linkedProps("NonContiguousNeighborListArrayPath");
  parameters.push_back(SIMPL_NEW_LINKED_BOOL_FP("Use Non-Contiguous Neighbors", UseNonContiguousNeighbors, FilterParameter::Parameter, GroupFeatures, linkedProps));
  parameters.push_back(SeparatorFilterParameter::New("Feature Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateCategoryRequirement(SIMPL::TypeNames::NeighborList, 1, AttributeMatrix::Category::Feature);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Non-Contiguous Neighbor List", NonContiguousNeighborListArrayPath, FilterParameter::RequiredArray, GroupFeatures, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateCategoryRequirement(SIMPL::TypeNames::NeighborList, 1, AttributeMatrix::Category::Feature);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Contiguous Neighbor List", ContiguousNeighborListArrayPath, FilterParameter::RequiredArray, GroupFeatures, req));
  }
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GroupFeatures::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setUseNonContiguousNeighbors(reader->readValue("UseNonContiguousNeighbors", getUseNonContiguousNeighbors()));
  setContiguousNeighborListArrayPath(reader->readDataArrayPath("ContiguousNeighborListArrayPath", getContiguousNeighborListArrayPath()));
  setNonContiguousNeighborListArrayPath(reader->readDataArrayPath("NonContiguousNeighborListArrayPath", getNonContiguousNeighborListArrayPath()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GroupFeatures::initialize()
{
  m_ContiguousNeighborList = NeighborList<int32_t>::NullPointer();
  m_NonContiguousNeighborList = NeighborList<int32_t>::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GroupFeatures::dataCheck()
{
  clearErrorCode();
  clearWarningCode();
  initialize();

  std::vector<size_t> cDims(1, 1);
  m_ContiguousNeighborList = getDataContainerArray()->getPrereqArrayFromPath<NeighborList<int32_t>, AbstractFilter>(this, getContiguousNeighborListArrayPath(), cDims);

  if(m_UseNonContiguousNeighbors)
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
  clearErrorCode();
  clearWarningCode();
  dataCheck();
  if(getErrorCode() < 0)
  {
    return;
  }

  NeighborList<int32_t>& neighborlist = *(m_ContiguousNeighborList.lock());
  NeighborList<int32_t>* nonContigNeighList = m_NonContiguousNeighborList.lock().get();

  std::vector<int32_t> grouplist;

  int32_t parentcount = 0;
  int32_t seed = 0;
  int32_t list1size = 0, list2size = 0, listsize = 0;
  int32_t neigh = 0;

  while(seed >= 0)
  {
    parentcount++;
    seed = getSeed(parentcount);
    if(seed >= 0)
    {
      grouplist.push_back(seed);
      for(std::vector<int32_t>::size_type j = 0; j < grouplist.size(); j++)
      {
        int32_t firstfeature = grouplist[j];
        list1size = int32_t(neighborlist[firstfeature].size());
        if(m_UseNonContiguousNeighbors)
        {
          list2size = nonContigNeighList->getListSize(firstfeature);
        }
        for(int32_t k = 0; k < 2; k++)
        {
          if(m_PatchGrouping)
          {
            k = 1;
          }
          if(k == 0)
          {
            listsize = list1size;
          }
          else if(k == 1)
          {
            listsize = list2size;
          }
          for(int32_t l = 0; l < listsize; l++)
          {
            if(k == 0)
            {
              neigh = neighborlist[firstfeature][l];
            }
            else if(k == 1)
            {
              neigh = nonContigNeighList->getListReference(firstfeature)[l];
            }
            if(neigh != firstfeature)
            {
              if(determineGrouping(firstfeature, neigh, parentcount))
              {
                if(!m_PatchGrouping)
                {
                  grouplist.push_back(neigh);
                }
              }
            }
          }
        }
      }
      if(m_PatchGrouping)
      {
        if(growPatch(parentcount))
        {
          for(std::vector<int32_t>::size_type j = 0; j < grouplist.size(); j++)
          {
            int32_t firstfeature = grouplist[j];
            listsize = int32_t(neighborlist[firstfeature].size());
            for(int32_t l = 0; l < listsize; l++)
            {
              neigh = neighborlist[firstfeature][l];
              if(neigh != firstfeature)
              {
                if(growGrouping(firstfeature, neigh, parentcount))
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


}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer GroupFeatures::newFilterInstance(bool copyFilterParameters) const
{
  GroupFeatures::Pointer filter = GroupFeatures::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString GroupFeatures::getCompiledLibraryName() const
{
  return ReconstructionConstants::ReconstructionBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString GroupFeatures::getBrandingString() const
{
  return "Reconstruction";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString GroupFeatures::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << Reconstruction::Version::Major() << "." << Reconstruction::Version::Minor() << "." << Reconstruction::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString GroupFeatures::getGroupName() const
{
  return SIMPL::FilterGroups::ReconstructionFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid GroupFeatures::getUuid() const
{
  return QUuid("{18d3e50e-050f-57f8-a357-103f00ab7b7b}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString GroupFeatures::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::SegmentationFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString GroupFeatures::getHumanLabel() const
{
  return "Group Features";
}

// -----------------------------------------------------------------------------
GroupFeatures::Pointer GroupFeatures::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<GroupFeatures> GroupFeatures::New()
{
  struct make_shared_enabler : public GroupFeatures
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString GroupFeatures::getNameOfClass() const
{
  return QString("GroupFeatures");
}

// -----------------------------------------------------------------------------
QString GroupFeatures::ClassName()
{
  return QString("GroupFeatures");
}

// -----------------------------------------------------------------------------
void GroupFeatures::setContiguousNeighborListArrayPath(const DataArrayPath& value)
{
  m_ContiguousNeighborListArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath GroupFeatures::getContiguousNeighborListArrayPath() const
{
  return m_ContiguousNeighborListArrayPath;
}

// -----------------------------------------------------------------------------
void GroupFeatures::setNonContiguousNeighborListArrayPath(const DataArrayPath& value)
{
  m_NonContiguousNeighborListArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath GroupFeatures::getNonContiguousNeighborListArrayPath() const
{
  return m_NonContiguousNeighborListArrayPath;
}

// -----------------------------------------------------------------------------
void GroupFeatures::setUseNonContiguousNeighbors(bool value)
{
  m_UseNonContiguousNeighbors = value;
}

// -----------------------------------------------------------------------------
bool GroupFeatures::getUseNonContiguousNeighbors() const
{
  return m_UseNonContiguousNeighbors;
}

// -----------------------------------------------------------------------------
void GroupFeatures::setPatchGrouping(bool value)
{
  m_PatchGrouping = value;
}

// -----------------------------------------------------------------------------
bool GroupFeatures::getPatchGrouping() const
{
  return m_PatchGrouping;
}
