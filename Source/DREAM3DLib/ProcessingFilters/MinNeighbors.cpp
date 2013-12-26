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

#include "MinNeighbors.h"


#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Math/DREAM3DMath.h"
#include "DREAM3DLib/Utilities/DREAM3DRandom.h"

#include "DREAM3DLib/StatisticsFilters/FindNeighbors.h"
#include "DREAM3DLib/GenericFilters/FindFeaturePhases.h"
#include "DREAM3DLib/GenericFilters/RenumberFeatures.h"



#define NEW_SHARED_ARRAY(var, m_msgType, size)\
  boost::shared_array<m_msgType> var##Array(new m_msgType[size]);\
  m_msgType* var = var##Array.get();

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MinNeighbors::MinNeighbors() :
  AbstractFilter(),
  m_DataContainerName(DREAM3D::Defaults::VolumeDataContainerName),
  m_CellFeatureAttributeMatrixName(DREAM3D::Defaults::CellFeatureAttributeMatrixName),
  m_CellAttributeMatrixName(DREAM3D::Defaults::CellAttributeMatrixName),
  m_MinNumNeighbors(1),
  m_FeatureIdsArrayName(DREAM3D::CellData::FeatureIds),
  m_FeatureIds(NULL),
  m_NumNeighborsArrayName(DREAM3D::FeatureData::NumNeighbors),
  m_NumNeighbors(NULL),
  m_ActiveArrayName(DREAM3D::FeatureData::Active),
  m_Active(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MinNeighbors::~MinNeighbors()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MinNeighbors::setupFilterParameters()
{
  FilterParameterVector parameters;
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Minimum Number Neighbors");
    option->setPropertyName("MinNumNeighbors");
    option->setWidgetType(FilterParameter::IntWidget);
    option->setValueType("int");
    parameters.push_back(option);
  }

  setFilterParameters(parameters);
}
// -----------------------------------------------------------------------------
void MinNeighbors::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  /* Code to read the values goes between these statements */
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE BEGIN*/
  setMinNumNeighbors( reader->readValue("MinNumNeighbors", getMinNumNeighbors()) );
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE END*/
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int MinNeighbors::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("MinNumNeighbors", getMinNumNeighbors() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MinNeighbors::dataCheck()
{
  setErrorCondition(0);

  VolumeDataContainer* m = getDataContainerArray()->getPrereqDataContainer<VolumeDataContainer, MinNeighbors>(this, getDataContainerName(), false);
  if(getErrorCondition() < 0) { return; }
  AttributeMatrix* cellFeatureAttrMat = m->getPrereqAttributeMatrix<AbstractFilter>(this, getCellFeatureAttributeMatrixName(), -301);
  if(getErrorCondition() < 0) { return; }
  AttributeMatrix* cellAttrMat = m->getPrereqAttributeMatrix<AbstractFilter>(this, getCellAttributeMatrixName(), -301);
  if(getErrorCondition() < 0) { return; }

  QVector<size_t> dims(1, 1);
  m_FeatureIdsPtr = cellAttrMat->getPrereqArray<DataArray<int32_t>, AbstractFilter>(this, m_FeatureIdsArrayName, -301, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeatureIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  m_ActivePtr = cellFeatureAttrMat->createNonPrereqArray<DataArray<bool>, AbstractFilter, bool>(this, m_ActiveArrayName, true, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_ActivePtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_Active = m_ActivePtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  m_NumNeighborsPtr = cellFeatureAttrMat->getPrereqArray<DataArray<int32_t>, AbstractFilter>(this, m_NumNeighborsArrayName, -304, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_NumNeighborsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_NumNeighbors = m_NumNeighborsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MinNeighbors::preflight()
{
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  RenumberFeatures::Pointer renumber_features = RenumberFeatures::New();
  connect(renumber_features.get(), SIGNAL(filterGeneratedMessage(const PipelineMessage&)),
          this, SLOT(broadcastPipelineMessage(const PipelineMessage&)));
  renumber_features->setDataContainerArray(getDataContainerArray());
  renumber_features->setMessagePrefix(getMessagePrefix());
  renumber_features->preflight();
  int err = renumber_features->getErrorCondition();
  if (err < 0)
  {
    setErrorCondition(renumber_features->getErrorCondition());

    return;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MinNeighbors::execute()
{
  setErrorCondition(0);

  dataCheck();
  if(getErrorCondition() < 0) { return; }

  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());
  int64_t totalPoints = m->getAttributeMatrix(getCellAttributeMatrixName())->getNumTuples();

  Int32ArrayType::Pointer neighborsPtr = Int32ArrayType::CreateArray(totalPoints, "Neighbors");
  m_Neighbors = neighborsPtr->getPointer(0);
  neighborsPtr->initializeWithZeros();

  BoolArrayType::Pointer alreadCheckedPtr = BoolArrayType::CreateArray(totalPoints, "AlreadyChecked");
  m_AlreadyChecked = alreadCheckedPtr->getPointer(0);
  alreadCheckedPtr->initializeWithZeros();

  merge_containedfeatures();
  assign_badpoints();

  RenumberFeatures::Pointer renumber_features = RenumberFeatures::New();
  connect(renumber_features.get(), SIGNAL(filterGeneratedMessage(const PipelineMessage&)),
          this, SLOT(broadcastPipelineMessage(const PipelineMessage&)));
  renumber_features->setDataContainerArray(getDataContainerArray());
  renumber_features->setMessagePrefix(getMessagePrefix());
  renumber_features->execute();
  int err = renumber_features->getErrorCondition();
  if (err < 0)
  {
    setErrorCondition(renumber_features->getErrorCondition());

    return;
  }

  // If there is an error set this to something negative and also set a message
  notifyStatusMessage(getHumanLabel(), "Minimum Number of Neighbors Filter Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MinNeighbors::assign_badpoints()
{
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

  int64_t totalPoints = m->getTotalPoints();
  size_t udims[3] = {0, 0, 0};
  m->getDimensions(udims);
#if (CMP_SIZEOF_SIZE_T == 4)
  typedef int32_t DimType;
#else
  typedef int64_t DimType;
#endif
  DimType dims[3] =
  {
    static_cast<DimType>(udims[0]),
    static_cast<DimType>(udims[1]),
    static_cast<DimType>(udims[2]),
  };

  Int32ArrayType::Pointer neighborsPtr = Int32ArrayType::CreateArray(totalPoints, "Neighbors");
  m_Neighbors = neighborsPtr->getPointer(0);
  neighborsPtr->initializeWithValue(-1);

  QVector<int > remove;
  int good = 1;
//  int neighbor;
//  int index = 0;
//  float x, y, z;
  int current = 0;
  int most = 0;
//  int curfeature = 0;
// DimType row, plane;
  int neighpoint;
  size_t numfeatures = m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->getNumTuples();

  int neighpoints[6];
  neighpoints[0] = static_cast<int>(-dims[0] * dims[1]);
  neighpoints[1] = static_cast<int>(-dims[0]);
  neighpoints[2] = static_cast<int>(-1);
  neighpoints[3] = static_cast<int>(1);
  neighpoints[4] = static_cast<int>(dims[0]);
  neighpoints[5] = static_cast<int>(dims[0] * dims[1]);
  QVector<int> currentvlist;

  size_t counter = 1;
  size_t count = 0;
  int kstride, jstride;
  int featurename, feature;
  int neighbor;
  QVector<int > n(numfeatures + 1, 0);
  while (counter != 0)
  {
    counter = 0;
    for (int k = 0; k < dims[2]; k++)
    {
      kstride = static_cast<int>( dims[0] * dims[1] * k );
      for (int j = 0; j < dims[1]; j++)
      {
        jstride = static_cast<int>( dims[0] * j );
        for (int i = 0; i < dims[0]; i++)
        {
          count = kstride + jstride + i;

          featurename = m_FeatureIds[count];
          if (featurename < 0)
          {
            counter++;
            current = 0;
            most = 0;
            for (int l = 0; l < 6; l++)
            {
              good = 1;
              neighpoint = static_cast<int>( count + neighpoints[l] );
              if (l == 0 && k == 0) { good = 0; }
              if (l == 5 && k == (dims[2] - 1)) { good = 0; }
              if (l == 1 && j == 0) { good = 0; }
              if (l == 4 && j == (dims[1] - 1)) { good = 0; }
              if (l == 2 && i == 0) { good = 0; }
              if (l == 3 && i == (dims[0] - 1)) { good = 0; }
              if (good == 1)
              {
                feature = m_FeatureIds[neighpoint];
                if (feature >= 0)
                {
                  n[feature]++;
                  current = n[feature];
                  if (current > most)
                  {
                    most = current;
                    m_Neighbors[count] = neighpoint;
                  }
                }
              }
            }
            for (int l = 0; l < 6; l++)
            {
              good = 1;
              neighpoint = static_cast<int>( count + neighpoints[l] );
              if (l == 0 && k == 0) { good = 0; }
              if (l == 5 && k == (dims[2] - 1)) { good = 0; }
              if (l == 1 && j == 0) { good = 0; }
              if (l == 4 && j == (dims[1] - 1)) { good = 0; }
              if (l == 2 && i == 0) { good = 0; }
              if (l == 3 && i == (dims[0] - 1)) { good = 0; }
              if (good == 1)
              {
                feature = m_FeatureIds[neighpoint];
                if(feature >= 0) { n[feature] = 0; }
              }
            }
          }
        }
      }
    }
    QList<QString> voxelArrayNames = m->getAttributeMatrix(getCellAttributeMatrixName())->getAttributeArrayNameList();
    for (int64_t j = 0; j < totalPoints; j++)
    {
      featurename = m_FeatureIds[j];
      neighbor = m_Neighbors[j];
      if (featurename < 0 && neighbor >= 0 && m_FeatureIds[neighbor] >= 0)
      {
        for(QList<QString>::iterator iter = voxelArrayNames.begin(); iter != voxelArrayNames.end(); ++iter)
        {
          QString name = *iter;
          IDataArray::Pointer p = m->getAttributeMatrix(getCellAttributeMatrixName())->getAttributeArray(*iter);
          p->copyTuple(neighbor, j);
        }
      }
    }
  }
}


void MinNeighbors::merge_containedfeatures()
{
  // Since this method is called from the 'execute' and the DataContainer validity
  // was checked there we are just going to get the Shared Pointer to the DataContainer
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

  bool good = false;

  size_t totalPoints = static_cast<size_t>(m->getTotalPoints());
  size_t totalFeatures = static_cast<size_t>(m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->getNumTuples());
  for (size_t i = 0; i < totalFeatures; i++)
  {
    m_Active[i] = true;
    if(m_NumNeighbors[i] >= m_MinNumNeighbors) { good = true; }
  }
  if(good == false)
  {
    setErrorCondition(-1);
    notifyErrorMessage(getHumanLabel(), "The minimum number of neighbors is larger than the Feature with the most neighbors.  All Features would be removed.  The filter has quit.", -1);
    return;
  }
  for (size_t i = 0; i < totalPoints; i++)
  {

    int featurename = m_FeatureIds[i];
    if(m_NumNeighbors[featurename] < m_MinNumNeighbors && featurename > 0)
    {
      m_Active[featurename] = false;
      m_FeatureIds[i] = -1;
    }
  }
}

