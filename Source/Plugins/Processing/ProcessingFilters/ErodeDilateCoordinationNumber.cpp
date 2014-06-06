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

#include "ErodeDilateCoordinationNumber.h"


#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Math/DREAM3DMath.h"
#include "DREAM3DLib/Utilities/DREAM3DRandom.h"


#define NEW_SHARED_ARRAY(var, m_msgType, size)\
  boost::shared_array<m_msgType> var##Array(new m_msgType[size]);\
  m_msgType* var = var##Array.get();

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ErodeDilateCoordinationNumber::ErodeDilateCoordinationNumber() :
  AbstractFilter(),
  m_Loop(false),
  m_CoordinationNumber(6),
  m_FeatureIdsArrayPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, DREAM3D::CellData::FeatureIds),
  m_FeatureIdsArrayName(DREAM3D::CellData::FeatureIds),
  m_FeatureIds(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ErodeDilateCoordinationNumber::~ErodeDilateCoordinationNumber()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ErodeDilateCoordinationNumber::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(FilterParameter::New("Coordination Number to Consider", "CoordinationNumber", FilterParameterWidgetType::IntWidget, getCoordinationNumber(), false));
  parameters.push_back(FilterParameter::New("Loop Until Gone", "Loop", FilterParameterWidgetType::BooleanWidget, getLoop(), false));
  parameters.push_back(FilterParameter::New("Required Information", "", FilterParameterWidgetType::SeparatorWidget, "", true));
  parameters.push_back(FilterParameter::New("FeatureIds", "FeatureIdsArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getFeatureIdsArrayPath(), true, ""));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ErodeDilateCoordinationNumber::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setFeatureIdsArrayPath(reader->readDataArrayPath("FeatureIdsArrayPath", getFeatureIdsArrayPath() ) );
  setCoordinationNumber( reader->readValue("CoordinationNumber", getCoordinationNumber()) );
  setLoop( reader->readValue("Loop", false) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int ErodeDilateCoordinationNumber::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("FeatureIdsArrayPath", getFeatureIdsArrayPath() );
  writer->writeValue("CoordinationNumber", getCoordinationNumber() );
  writer->writeValue("Loop", getLoop() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ErodeDilateCoordinationNumber::dataCheck()
{
  setErrorCondition(0);

  QVector<size_t> dims(1, 1);
  m_FeatureIdsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getFeatureIdsArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeatureIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ErodeDilateCoordinationNumber::preflight()
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
void ErodeDilateCoordinationNumber::execute()
{
  setErrorCondition(0);

  dataCheck();
  if(getErrorCondition() < 0) { return; }

  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getFeatureIdsArrayPath().getDataContainerName());
  int64_t totalPoints = m_FeatureIdsPtr.lock()->getNumberOfTuples();

  Int32ArrayType::Pointer neighborsPtr = Int32ArrayType::CreateArray(totalPoints, "Neighbors");
  m_Neighbors = neighborsPtr->getPointer(0);
  neighborsPtr->initializeWithValue(-1);

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

  int good = 1;
  size_t point = 0;
  int kstride, jstride;
  int featurename, feature;
  int coordination = 0;
  int current = 0;
  int most = 0;
  int neighpoint;
  size_t numfeatures = 0;
  for(int64_t i = 0; i < totalPoints; i++)
  {
    featurename = m_FeatureIds[i];
    if(featurename > numfeatures) { numfeatures = featurename; }
  }
  if (numfeatures == 0)
  {
    setErrorCondition(-90001);
    notifyErrorMessage(getHumanLabel(), "No features have been defined in the Feature map. A filter needs to be executed before this filter that defines the number of features.", getErrorCondition());
    notifyStatusMessage(getHumanLabel(), "Completed with Errors");
    return;
  }

  int neighpoints[6];
  neighpoints[0] = static_cast<int>(-dims[0] * dims[1]);
  neighpoints[1] = static_cast<int>(-dims[0]);
  neighpoints[2] = static_cast<int>(-1);
  neighpoints[3] = static_cast<int>(1);
  neighpoints[4] = static_cast<int>(dims[0]);
  neighpoints[5] = static_cast<int>(dims[0] * dims[1]);

  QString attrMatName = m_FeatureIdsArrayPath.getAttributeMatrixName();
  QList<QString> voxelArrayNames = m->getAttributeMatrix(attrMatName)->getAttributeArrayNameList();

  QVector<int > n(numfeatures + 1, 0);

  QVector<int > coordinationNumber(totalPoints, 0);
  bool keepgoing = true;
  int counter = 1;
  while (counter > 0 && keepgoing == true)
  {
    counter = 0;
    if(m_Loop == false) { keepgoing = false; }

    for (int k = 0; k < dims[2]; k++)
    {
      kstride = static_cast<int>( dims[0] * dims[1] * k );
      for (int j = 0; j < dims[1]; j++)
      {
        jstride = static_cast<int>( dims[0] * j );
        for (int i = 0; i < dims[0]; i++)
        {
          point = kstride + jstride + i;
          featurename = m_FeatureIds[point];
          coordination = 0;
          current = 0;
          most = 0;
          for (int l = 0; l < 6; l++)
          {
            good = 1;
            neighpoint = static_cast<int>( point + neighpoints[l] );
            if (l == 0 && k == 0) { good = 0; }
            if (l == 5 && k == (dims[2] - 1)) { good = 0; }
            if (l == 1 && j == 0) { good = 0; }
            if (l == 4 && j == (dims[1] - 1)) { good = 0; }
            if (l == 2 && i == 0) { good = 0; }
            if (l == 3 && i == (dims[0] - 1)) { good = 0; }
            if (good == 1)
            {
              feature = m_FeatureIds[neighpoint];
              if((featurename > 0 && feature == 0) || (featurename == 0 && feature > 0))
              {
                coordination = coordination + 1;
                n[feature]++;
                current = n[feature];
                if (current > most)
                {
                  most = current;
                  m_Neighbors[point] = neighpoint;
                }
              }
            }
          }
          coordinationNumber[point] = coordination;
          int neighbor = m_Neighbors[point];
          if (coordinationNumber[point] >= m_CoordinationNumber && coordinationNumber[point] > 0)
          {
            for(QList<QString>::iterator iter = voxelArrayNames.begin(); iter != voxelArrayNames.end(); ++iter)
            {
              QString name = *iter;
              IDataArray::Pointer p = m->getAttributeMatrix(attrMatName)->getAttributeArray(*iter);
              p->copyTuple(neighbor, point);
            }
          }
          for (int l = 0; l < 6; l++)
          {
            good = 1;
            neighpoint = static_cast<int>( point + neighpoints[l] );
            if (l == 0 && k == 0) { good = 0; }
            if (l == 5 && k == (dims[2] - 1)) { good = 0; }
            if (l == 1 && j == 0) { good = 0; }
            if (l == 4 && j == (dims[1] - 1)) { good = 0; }
            if (l == 2 && i == 0) { good = 0; }
            if (l == 3 && i == (dims[0] - 1)) { good = 0; }
            if (good == 1)
            {
              feature = m_FeatureIds[neighpoint];
              if(feature > 0) { n[feature] = 0; }
            }
          }
        }
      }
    }
    for (int k = 0; k < dims[2]; k++)
    {
      kstride = static_cast<int>( dims[0] * dims[1] * k );
      for (int j = 0; j < dims[1]; j++)
      {
        jstride = static_cast<int>( dims[0] * j );
        for (int i = 0; i < dims[0]; i++)
        {
          point = kstride + jstride + i;
          if(coordinationNumber[point] >= m_CoordinationNumber)
          {
            counter++;
          }
        }
      }
    }
  }

  // If there is an error set this to something negative and also set a message
  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer ErodeDilateCoordinationNumber::newFilterInstance(bool copyFilterParameters)
{
  ErodeDilateCoordinationNumber::Pointer filter = ErodeDilateCoordinationNumber::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}
