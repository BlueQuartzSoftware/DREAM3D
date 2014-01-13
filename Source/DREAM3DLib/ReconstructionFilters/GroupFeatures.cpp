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
  m_DataContainerName(DREAM3D::Defaults::VolumeDataContainerName),
  m_CellFeatureAttributeMatrixName(DREAM3D::Defaults::CellFeatureAttributeMatrixName),
  m_ContiguousNeighborListArrayName(DREAM3D::FeatureData::NeighborList),
  m_NonContiguousNeighborListArrayName(DREAM3D::FeatureData::NeighborhoodList),
  m_UseNonContiguousNeighbors(false)
{

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
void GroupFeatures::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  /* Code to read the values goes between these statements */
  ////!!##
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int GroupFeatures::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GroupFeatures::dataCheck()
{
  setErrorCondition(0);

  VolumeDataContainer* m = getDataContainerArray()->getPrereqDataContainer<VolumeDataContainer, AbstractFilter>(this, getDataContainerName(), false);
  if(getErrorCondition() < 0) { return; }
  AttributeMatrix::Pointer cellFeatureAttrMat = m->getPrereqAttributeMatrix<AbstractFilter>(this, getCellFeatureAttributeMatrixName(), -301);
  if(getErrorCondition() < 0) { return; }

  // Now we are going to get a "Pointer" to the NeighborList object out of the DataContainer
  m_ContiguousNeighborList = NeighborList<int>::SafeObjectDownCast<IDataArray*, NeighborList<int>*>(m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->getAttributeArray(DREAM3D::FeatureData::NeighborList).get());
  if(m_ContiguousNeighborList == NULL)
  {
    QString ss = QObject::tr("NeighborLists Array Not Initialized correctly");
    setErrorCondition(-304);
    notifyErrorMessage(getHumanLabel(), ss, -1);
  }
  if(m_UseNonContiguousNeighbors == true)
  {
    // Now we are going to get a "Pointer" to the NeighborList object out of the DataContainer
    m_NonContiguousNeighborList = NeighborList<int>::SafeObjectDownCast<IDataArray*, NeighborList<int>*>(m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->getAttributeArray(DREAM3D::FeatureData::NeighborhoodList).get());
    if(m_NonContiguousNeighborList == NULL)
    {
      QString ss = QObject::tr("NeighborhoodLists Array Not Initialized correctly");
      setErrorCondition(-305);
      notifyErrorMessage(getHumanLabel(), ss, -1);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GroupFeatures::preflight()
{
  dataCheck();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GroupFeatures::execute()
{
  // Since this method is called from the 'execute' and the DataContainer validity
  // was checked there we are just going to get the Shared Pointer to the DataContainer
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

  // Now we are going to get a "Pointer" to the NeighborList object out of the DataContainer
  m_ContiguousNeighborList = NeighborList<int>::SafeObjectDownCast<IDataArray*, NeighborList<int>* >(m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->getAttributeArray(DREAM3D::FeatureData::NeighborList).get());
  if(m_NonContiguousNeighborList == NULL) m_NonContiguousNeighborList = NeighborList<int>::SafeObjectDownCast<IDataArray*, NeighborList<int>* >(m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->getAttributeArray(DREAM3D::FeatureData::NeighborhoodList).get());
  // But since a pointer is difficult to use operators with we will now create a
  // reference variable to the pointer with the correct variable name that allows
  // us to use the same syntax as the "vector of vectors"
  NeighborList<int>& neighborlist = *m_ContiguousNeighborList;
  NeighborList<int>& neighborlist2 = *m_NonContiguousNeighborList;

  QVector<int> grouplist;

  size_t numfeatures = m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->getNumTuples();
  int parentcount = 0;
  size_t seed = 0;
  size_t list1size=0, list2size=0, listsize=0;
  size_t neigh;

  while (seed >= 0)
  {
    seed = getSeed(parentcount);
    if(seed >= 0)
    {
      parentcount++;
      grouplist.push_back(seed);
      for (size_t j = 0; j < grouplist.size(); j++)
      {
        int firstfeature = grouplist[j];
        list1size = int(neighborlist[firstfeature].size());
        if (m_UseNonContiguousNeighbors == true) { list2size = int(neighborlist2[firstfeature].size()); }
        for (int k = 0; k < 2; k++)
        {
          if (k == 0) { listsize = list1size; }
          else if (k == 1) { listsize = list2size; }
          for (int l = 0; l < listsize; l++)
          {
            int twin = 0;
            if (k == 0) { neigh = neighborlist[firstfeature][l]; }
            else if (k == 1) { neigh = neighborlist2[firstfeature][l]; }
            if (neigh != seed)
            {
              if(determineGrouping(seed, neigh, parentcount) == true);
              {
                grouplist.push_back(neigh);
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
size_t GroupFeatures::getSeed(size_t newFid)
{
  return -1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool GroupFeatures::determineGrouping(int64_t referenceFeature, int64_t neighborFeature, size_t newFid)
{
  return false;
}

