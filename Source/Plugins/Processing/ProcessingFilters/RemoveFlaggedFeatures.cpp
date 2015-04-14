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

#include "RemoveFlaggedFeatures.h"


#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/FilterParameters/AbstractFilterParametersReader.h"
#include "DREAM3DLib/FilterParameters/AbstractFilterParametersWriter.h"
#include "DREAM3DLib/Math/DREAM3DMath.h"
#include "DREAM3DLib/Utilities/DREAM3DRandom.h"


#define NEW_SHARED_ARRAY(var, m_msgType, size)\
  boost::shared_array<m_msgType> var##Array(new m_msgType[size]);\
  m_msgType* var = var##Array.get();

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RemoveFlaggedFeatures::RemoveFlaggedFeatures() :
  AbstractFilter(),
  m_FeatureIdsArrayPath(DREAM3D::Defaults::DataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, DREAM3D::CellData::FeatureIds),
  m_FlaggedFeaturesArrayPath("", "" , ""),
  m_FeatureIdsArrayName(DREAM3D::CellData::FeatureIds),
  m_FeatureIds(NULL),
  m_FlaggedFeaturesArrayName(""),
  m_FlaggedFeatures(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RemoveFlaggedFeatures::~RemoveFlaggedFeatures()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RemoveFlaggedFeatures::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(FilterParameter::New("Required Information", "", FilterParameterWidgetType::SeparatorWidget, "", true));
  parameters.push_back(FilterParameter::New("FeatureIds", "FeatureIdsArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getFeatureIdsArrayPath(), true, ""));
  parameters.push_back(FilterParameter::New("Flagged Features", "FlaggedFeaturesArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getFlaggedFeaturesArrayPath(), true, ""));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RemoveFlaggedFeatures::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setFlaggedFeaturesArrayPath(reader->readDataArrayPath("FlaggedFeaturesArrayPath", getFlaggedFeaturesArrayPath() ) );
  setFeatureIdsArrayPath(reader->readDataArrayPath("FeatureIdsArrayPath", getFeatureIdsArrayPath() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int RemoveFlaggedFeatures::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  DREAM3D_FILTER_WRITE_PARAMETER(FilterVersion)
  DREAM3D_FILTER_WRITE_PARAMETER(FlaggedFeaturesArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(FeatureIdsArrayPath)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RemoveFlaggedFeatures::dataCheck()
{
  setErrorCondition(0);

  QVector<size_t> dims(1, 1);
  m_FeatureIdsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getFeatureIdsArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeatureIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCondition() < 0) { return; }

  ImageGeom::Pointer image = getDataContainerArray()->getDataContainer(getFeatureIdsArrayPath().getDataContainerName())->getPrereqGeometry<ImageGeom, AbstractFilter>(this);
  if(getErrorCondition() < 0 || NULL == image.get()) { return; }

  m_FlaggedFeaturesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<bool>, AbstractFilter>(this, getFlaggedFeaturesArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FlaggedFeaturesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FlaggedFeatures = m_FlaggedFeaturesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RemoveFlaggedFeatures::preflight()
{
  setInPreflight(true);
  emit preflightAboutToExecute();
  emit updateFilterParameters(this);
  dataCheck();
  emit preflightExecuted();
  if(getErrorCondition() < 0) { setInPreflight(false); return; }

  AttributeMatrix::Pointer cellFeatureAttrMat = getDataContainerArray()->getAttributeMatrix(getFlaggedFeaturesArrayPath());
  QVector<bool> activeObjects(cellFeatureAttrMat->getNumTuples(), true);
  cellFeatureAttrMat->removeInactiveObjects(activeObjects, m_FeatureIdsPtr.lock());
  setInPreflight(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RemoveFlaggedFeatures::execute()
{
  setErrorCondition(0);

  dataCheck();
  if(getErrorCondition() < 0) { return; }

  //int64_t totalPoints = m_FeatureIdsPtr.lock()->getNumberOfTuples();

  QVector<bool> activeObjects = remove_flaggedfeatures();
  assign_badpoints();

  AttributeMatrix::Pointer cellFeatureAttrMat = getDataContainerArray()->getAttributeMatrix(getFlaggedFeaturesArrayPath());
  cellFeatureAttrMat->removeInactiveObjects(activeObjects, m_FeatureIdsPtr.lock());

  // If there is an error set this to something negative and also set a message
  notifyStatusMessage(getHumanLabel(), "Remove Flagged Features Filter Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RemoveFlaggedFeatures::assign_badpoints()
{
  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(m_FeatureIdsArrayPath.getDataContainerName());

  int64_t totalPoints = m_FeatureIdsPtr.lock()->getNumberOfTuples();
  size_t udims[3] = {0, 0, 0};
  m->getGeometryAs<ImageGeom>()->getDimensions(udims);
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
  QVector<int > n(m_FlaggedFeaturesPtr.lock()->getNumberOfTuples(), 0);
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
    QString attrMatName = m_FeatureIdsArrayPath.getAttributeMatrixName();
    QList<QString> voxelArrayNames = m->getAttributeMatrix(attrMatName)->getAttributeArrayNames();
    for (int64_t j = 0; j < totalPoints; j++)
    {
      featurename = m_FeatureIds[j];
      neighbor = m_Neighbors[j];
      if (neighbor >= 0)
      {
        if (featurename < 0 && m_FeatureIds[neighbor] >= 0)
        {

          for(QList<QString>::iterator iter = voxelArrayNames.begin(); iter != voxelArrayNames.end(); ++iter)
          {
            QString name = *iter;
            IDataArray::Pointer p = m->getAttributeMatrix(attrMatName)->getAttributeArray(*iter);
            p->copyTuple(neighbor, j);
          }
        }
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<bool> RemoveFlaggedFeatures::remove_flaggedfeatures()
{
  int64_t totalPoints = m_FeatureIdsPtr.lock()->getNumberOfTuples();

  bool good = false;
  int gnum;

  size_t totalFeatures = m_FlaggedFeaturesPtr.lock()->getNumberOfTuples();
  QVector<bool> activeObjects(totalFeatures, true);

  for (size_t i = 1; i <  totalFeatures; i++)
  {
    if(m_FlaggedFeatures[i] == false) { good = true; }
    else { activeObjects[i] = false; }
  }
  if(good == false)
  {
    setErrorCondition(-1);
    notifyErrorMessage(getHumanLabel(), "All Features were flagged and would all be removed.  The filter has quit.", -1);
    return activeObjects;
  }
  for (int64_t i = 0; i < totalPoints; i++)
  {
    gnum = m_FeatureIds[i];
    if(activeObjects[gnum] == false)
    {
      m_FeatureIds[i] = -1;
    }
  }
  return activeObjects;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer RemoveFlaggedFeatures::newFilterInstance(bool copyFilterParameters)
{
  RemoveFlaggedFeatures::Pointer filter = RemoveFlaggedFeatures::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString RemoveFlaggedFeatures::getCompiledLibraryName()
{ return Processing::ProcessingBaseName; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString RemoveFlaggedFeatures::getGroupName()
{ return DREAM3D::FilterGroups::ProcessingFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString RemoveFlaggedFeatures::getHumanLabel()
{ return "Remove Flagged Features"; }

