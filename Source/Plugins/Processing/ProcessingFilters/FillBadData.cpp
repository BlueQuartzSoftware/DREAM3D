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

#include "FillBadData.h"


#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Math/DREAM3DMath.h"
#include "DREAM3DLib/Utilities/DREAM3DRandom.h"


#define NEW_SHARED_ARRAY(var, m_msgType, size)\
  boost::shared_array<m_msgType> var##Array(new m_msgType[size]);\
  m_msgType* var = var##Array.get();

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FillBadData::FillBadData() :
  AbstractFilter(),
  m_StoreAsNewPhase(false),
  m_ReplaceBadData(true),
  m_MinAllowedDefectSize(1),
  m_FeatureIdsArrayPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, DREAM3D::CellData::FeatureIds),
  m_CellPhasesArrayPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, DREAM3D::CellData::Phases),
  m_FeatureIdsArrayName(DREAM3D::CellData::FeatureIds),
  m_FeatureIds(NULL),
  m_CellPhasesArrayName(DREAM3D::CellData::Phases),
  m_CellPhases(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FillBadData::~FillBadData()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FillBadData::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(FilterParameter::New("Minimum Allowed Defect Size", "MinAllowedDefectSize", FilterParameterWidgetType::IntWidget, getMinAllowedDefectSize(), false, "Pixels"));
  QStringList linkedProps;
  linkedProps << "CellPhasesArrayPath";
  parameters.push_back(LinkedBooleanFilterParameter::New("Store Defects As New Phase", "StoreAsNewPhase", getStoreAsNewPhase(), linkedProps, false));
  parameters.push_back(FilterParameter::New("Replace Bad Data", "ReplaceBadData", FilterParameterWidgetType::BooleanWidget, getReplaceBadData(), false, ""));

  parameters.push_back(FilterParameter::New("Required Information", "", FilterParameterWidgetType::SeparatorWidget, "", true));
  parameters.push_back(FilterParameter::New("FeatureIds", "FeatureIdsArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getFeatureIdsArrayPath(), true, ""));
  parameters.push_back(FilterParameter::New("Cell Phases", "CellPhasesArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getCellPhasesArrayPath(), true, ""));
  setFilterParameters(parameters);
}
// -----------------------------------------------------------------------------
void FillBadData::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setFeatureIdsArrayPath(reader->readDataArrayPath("FeatureIdsArrayPath", getFeatureIdsArrayPath() ) );
  setCellPhasesArrayPath(reader->readDataArrayPath("CellPhasesArrayPath", getCellPhasesArrayPath() ) );
  setMinAllowedDefectSize( reader->readValue("MinAllowedDefectSize", getMinAllowedDefectSize()) );
  setStoreAsNewPhase( reader->readValue("StoreAsNewPhase", getStoreAsNewPhase()) );
  setReplaceBadData( reader->readValue("ReplaceBadData", getReplaceBadData()) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int FillBadData::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  DREAM3D_FILTER_WRITE_PARAMETER(FeatureIdsArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(CellPhasesArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(MinAllowedDefectSize)
  DREAM3D_FILTER_WRITE_PARAMETER(StoreAsNewPhase)
  DREAM3D_FILTER_WRITE_PARAMETER(ReplaceBadData)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FillBadData::dataCheck()
{
  setErrorCondition(0);

  QVector<size_t> dims(1, 1);
  m_FeatureIdsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getFeatureIdsArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeatureIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  if(m_StoreAsNewPhase == true)
  {
    m_CellPhasesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getCellPhasesArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if( NULL != m_CellPhasesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    { m_CellPhases = m_CellPhasesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FillBadData::preflight()
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
void FillBadData::execute()
{
  setErrorCondition(0);

  dataCheck();
  if(getErrorCondition() < 0) { return; }

  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(m_FeatureIdsArrayPath.getDataContainerName());
  int64_t totalPoints = m_FeatureIdsPtr.lock()->getNumberOfTuples();

  Int32ArrayType::Pointer neighborsPtr = Int32ArrayType::CreateArray(totalPoints, "Neighbors");
  m_Neighbors = neighborsPtr->getPointer(0);
  neighborsPtr->initializeWithValue(-1);

  BoolArrayType::Pointer alreadCheckedPtr = BoolArrayType::CreateArray(totalPoints, "AlreadyChecked");
  m_AlreadyChecked = alreadCheckedPtr->getPointer(0);
  alreadCheckedPtr->initializeWithZeros();

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

  std::vector<int> neighs;
  std::vector<int> remove;
  size_t count = 1;
  //size_t point = 0;
  int good = 1;
  int neighbor;
  int index = 0;
  float x, y, z;
  DimType column, row, plane;
  int neighpoint;
  int featurename, feature;
  size_t numfeatures = 0;
  size_t maxPhase = 0;
  for(int64_t i = 0; i < totalPoints; i++)
  {
    featurename = m_FeatureIds[i];
    if(featurename > numfeatures) { numfeatures = featurename; }
  }
  if(m_StoreAsNewPhase == true)
  {
    for(int64_t i = 0; i < totalPoints; i++)
    {
      if(m_CellPhases[i] > maxPhase) { maxPhase = m_CellPhases[i]; }
    }
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
  std::vector<int> currentvlist;

  for (int64_t iter = 0; iter < totalPoints; iter++)
  {
    m_AlreadyChecked[iter] = false;
    if (m_FeatureIds[iter] != 0) { m_AlreadyChecked[iter] = true; }
  }
  for (int64_t i = 0; i < totalPoints; i++)
  {
    if(m_AlreadyChecked[i] == false && m_FeatureIds[i] == 0)
    {
      currentvlist.push_back( static_cast<int>(i) );
      count = 0;
      while(count < currentvlist.size())
      {
        index = currentvlist[count];
        column = index % dims[0];
        row = (index / dims[0]) % dims[1];
        plane = index / (dims[0] * dims[1]);
        for (DimType j = 0; j < 6; j++)
        {
          good = 1;
          neighbor = index + neighpoints[j];
          if (j == 0 && plane == 0) { good = 0; }
          if (j == 5 && plane == (dims[2] - 1)) { good = 0; }
          if (j == 1 && row == 0) { good = 0; }
          if (j == 4 && row == (dims[1] - 1)) { good = 0; }
          if (j == 2 && column == 0) { good = 0; }
          if (j == 3 && column == (dims[0] - 1)) { good = 0; }
          if (good == 1 && m_FeatureIds[neighbor] == 0 && m_AlreadyChecked[neighbor] == false)
          {
            currentvlist.push_back(neighbor);
            m_AlreadyChecked[neighbor] = true;
          }
        }
        count++;
      }
      if((int)currentvlist.size() >= m_MinAllowedDefectSize)
      {
        for (size_t k = 0; k < currentvlist.size(); k++)
        {
          m_FeatureIds[currentvlist[k]] = 0;
          if(m_StoreAsNewPhase == true) { m_CellPhases[currentvlist[k]] = maxPhase + 1; }
        }
      }
      if((int)currentvlist.size() < m_MinAllowedDefectSize)
      {
        for (qint32 k = 0; k < currentvlist.size(); k++)
        {
          m_FeatureIds[currentvlist[k]] = -1;
        }
      }
      currentvlist.clear();
    }
  }

  int current = 0;
  int most = 0;

  std::vector<int> n(numfeatures + 1, 0);
  while (count != 0)
  {
    count = 0;
    for (int i = 0; i < totalPoints; i++)
    {
      featurename = m_FeatureIds[i];
      if (featurename < 0)
      {
        count++;
        current = 0;
        most = 0;
        x = static_cast<float>(i % dims[0]);
        y = static_cast<float>((i / dims[0]) % dims[1]);
        z = static_cast<float>(i / (dims[0] * dims[1]));
        for (int j = 0; j < 6; j++)
        {
          good = 1;
          neighpoint = i + neighpoints[j];
          if (j == 0 && z == 0) { good = 0; }
          if (j == 5 && z == (dims[2] - 1)) { good = 0; }
          if (j == 1 && y == 0) { good = 0; }
          if (j == 4 && y == (dims[1] - 1)) { good = 0; }
          if (j == 2 && x == 0) { good = 0; }
          if (j == 3 && x == (dims[0] - 1)) { good = 0; }
          if (good == 1)
          {
            feature = m_FeatureIds[neighpoint];
            if (feature > 0)
            {
              n[feature]++;
              current = n[feature];
              if (current > most)
              {
                most = current;
                m_Neighbors[i] = neighpoint;
              }
            }
          }
        }
        for (int l = 0; l < 6; l++)
        {
          good = 1;
          neighpoint = i + neighpoints[l];
          if (l == 0 && z == 0) { good = 0; }
          if (l == 5 && z == (dims[2] - 1)) { good = 0; }
          if (l == 1 && y == 0) { good = 0; }
          if (l == 4 && y == (dims[1] - 1)) { good = 0; }
          if (l == 2 && x == 0) { good = 0; }
          if (l == 3 && x == (dims[0] - 1)) { good = 0; }
          if (good == 1)
          {
            feature = m_FeatureIds[neighpoint];
            if(feature > 0) { n[feature] = 0; }
          }
        }
      }
    }
    QString attrMatName = m_FeatureIdsArrayPath.getAttributeMatrixName();
    QList<QString> voxelArrayNames = m->getAttributeMatrix(attrMatName)->getAttributeArrayNameList();
    for (int64_t j = 0; j < totalPoints; j++)
    {
      featurename = m_FeatureIds[j];
      neighbor = m_Neighbors[j];
      if (featurename < 0 && neighbor != -1 && m_FeatureIds[neighbor] > 0)
      {
        if(getReplaceBadData())
        {
          for(QList<QString>::iterator iter = voxelArrayNames.begin(); iter != voxelArrayNames.end(); ++iter)
          {
            QString name = *iter;
            IDataArray::Pointer p = m->getAttributeMatrix(attrMatName)->getAttributeArray(*iter);
            p->copyTuple(neighbor, j);
          }
        }
        else
        {
          m_FeatureIds[j] = m_FeatureIds[neighbor];
        }
      }
    }
  }

  // If there is an error set this to something negative and also set a message
  notifyStatusMessage(getHumanLabel(), "Filling Bad Data Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer FillBadData::newFilterInstance(bool copyFilterParameters)
{
  FillBadData::Pointer filter = FillBadData::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FillBadData::getCompiledLibraryName()
{ return Processing::ProcessingBaseName; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FillBadData::getGroupName()
{ return DREAM3D::FilterGroups::ProcessingFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FillBadData::getHumanLabel()
{ return "Fill Bad Data"; }

