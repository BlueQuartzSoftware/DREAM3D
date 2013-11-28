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

#include "OpenCloseBadData.h"


#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Math/DREAM3DMath.h"
#include "DREAM3DLib/Utilities/DREAM3DRandom.h"

#include "DREAM3DLib/GenericFilters/FindFeaturePhases.h"



#define NEW_SHARED_ARRAY(var, m_msgType, size)\
  boost::shared_array<m_msgType> var##Array(new m_msgType[size]);\
  m_msgType* var = var##Array.get();

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
OpenCloseBadData::OpenCloseBadData() :
  AbstractFilter(),
  m_DataContainerName(DREAM3D::HDF5::VolumeDataContainerName),
  m_CellAttributeMatrixName(DREAM3D::HDF5::CellAttributeMatrixName),
  m_FeatureIdsArrayName(DREAM3D::CellData::FeatureIds),
  m_Direction(0),
  m_NumIterations(1),
  m_XDirOn(true),
  m_YDirOn(true),
  m_ZDirOn(true),
  m_Neighbors(NULL),
  m_FeatureIds(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
OpenCloseBadData::~OpenCloseBadData()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OpenCloseBadData::setupFilterParameters()
{
  FilterParameterVector parameters;
  {
    ChoiceFilterParameter::Pointer option = ChoiceFilterParameter::New();
    option->setHumanLabel("Operation");
    option->setPropertyName("Direction");
    option->setWidgetType(FilterParameter::ChoiceWidget);
    option->setValueType("unsigned int");
    QVector<QString> choices;
    choices.push_back("Dilate");
    choices.push_back("Erode");
    option->setChoices(choices);
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Number of Iterations");
    option->setPropertyName("NumIterations");
    option->setWidgetType(FilterParameter::IntWidget);
    option->setValueType("int");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("X Direction");
    option->setPropertyName("XDirOn");
    option->setWidgetType(FilterParameter::BooleanWidget);
    option->setValueType("bool");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Y Direction");
    option->setPropertyName("YDirOn");
    option->setWidgetType(FilterParameter::BooleanWidget);
    option->setValueType("bool");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Z Direction");
    option->setPropertyName("ZDirOn");
    option->setWidgetType(FilterParameter::BooleanWidget);
    option->setValueType("bool");
    parameters.push_back(option);
  }
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OpenCloseBadData::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  /* Code to read the values goes between these statements */
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE BEGIN*/
  setDirection( reader->readValue("Direction", getDirection()) );
  setNumIterations( reader->readValue("NumIterations", getNumIterations()) );
  setXDirOn(reader->readValue("X Direction", getXDirOn()) );
  setYDirOn(reader->readValue("Y Direction", getYDirOn()) );
  setZDirOn(reader->readValue("Z Direction", getZDirOn()) );
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE END*/
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int OpenCloseBadData::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("Direction", getDirection() );
  writer->writeValue("NumIterations", getNumIterations() );
  writer->writeValue("X Direction", getXDirOn() );
  writer->writeValue("Y Direction", getYDirOn() );
  writer->writeValue("Z Direction", getZDirOn() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OpenCloseBadData::dataCheck(bool preflight, size_t voxels, size_t features, size_t ensembles)
{
  setErrorCondition(0);

  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

  QVector<int> dims(1, 1);
  m_FeatureIdsPtr = m->getPrereqArray<int32_t, AbstractFilter>(this, m_CellAttributeMatrixName,  m_FeatureIdsArrayName, -301, voxels, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeatureIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OpenCloseBadData::preflight()
{
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());
  if(NULL == m)
  {
    setErrorCondition(-999);
    addErrorMessage(getHumanLabel(), "The VolumeDataContainer Object with the specific name " + getDataContainerName() + " was not available.", getErrorCondition());
    return;
  }

  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OpenCloseBadData::execute()
{
  setErrorCondition(0);
// int err = 0;
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The DataContainer Object was NULL", -999);
    return;
  }


  int64_t totalPoints = m->getAttributeMatrix(getCellAttributeMatrixName())->getNumTuples();
  dataCheck(false, totalPoints, 0, 0);
  if (getErrorCondition() < 0 && getErrorCondition() != -305)
  {
    return;
  }
  setErrorCondition(0);

  Int32ArrayType::Pointer neighborsPtr = Int32ArrayType::CreateArray(totalPoints, "Neighbors");
  m_Neighbors = neighborsPtr->getPointer(0);
  neighborsPtr->initializeWithValues(-1);

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
  size_t count = 0;
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
    featurename = m_FeatureIds[point];
    if(featurename > numfeatures) { numfeatures = featurename; }
  }

  int neighpoints[6];
  neighpoints[0] = static_cast<int>(-dims[0] * dims[1]);
  neighpoints[1] = static_cast<int>(-dims[0]);
  neighpoints[2] = static_cast<int>(-1);
  neighpoints[3] = static_cast<int>(1);
  neighpoints[4] = static_cast<int>(dims[0]);
  neighpoints[5] = static_cast<int>(dims[0] * dims[1]);
  QVector<int> currentvlist;

  QVector<int > n(numfeatures + 1, 0);
  for (int iteration = 0; iteration < m_NumIterations; iteration++)
  {
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
          if (featurename == 0)
          {
            current = 0;
            most = 0;
            for (int l = 0; l < 6; l++)
            {
              good = 1;
              neighpoint = static_cast<int>( count + neighpoints[l] );
              if (l == 0 && (k == 0 || m_ZDirOn == false)) { good = 0; }
              else if (l == 5 && (k == (dims[2] - 1) || m_ZDirOn == false)) { good = 0; }
              else if (l == 1 && (j == 0 || m_YDirOn == false)) { good = 0; }
              else if (l == 4 && (j == (dims[1] - 1) || m_YDirOn == false)) { good = 0; }
              else if (l == 2 && (i == 0 || m_XDirOn == false)) { good = 0; }
              else if (l == 3 && (i == (dims[0] - 1) || m_XDirOn == false)) { good = 0; }
              if (good == 1)
              {
                feature = m_FeatureIds[neighpoint];
                if (m_Direction == 0 && feature > 0)
                {
                  m_Neighbors[neighpoint] = count;
                }
                if ((feature > 0 && m_Direction == 1))
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
            if (m_Direction == 1)
            {
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
                  n[feature] = 0;
                }
              }
            }
          }
        }
      }
    }
    QList<QString> voxelArrayNames = m->getAttributeMatrix(getCellAttributeMatrixName())->getAttributeArrayNameList();
    for (size_t j = 0; j < totalPoints; j++)
    {
      featurename = m_FeatureIds[j];
      int neighbor = m_Neighbors[j];
      if (neighbor >= 0)
      {
        if ( (featurename == 0 && m_FeatureIds[neighbor] > 0 && m_Direction == 1)
             || (featurename > 0 && m_FeatureIds[neighbor] == 0 && m_Direction == 0))
        {
          for(QList<QString>::iterator iter = voxelArrayNames.begin(); iter != voxelArrayNames.end(); ++iter)
          {
            QString name = *iter;
            IDataArray::Pointer p = m->getAttributeMatrix(getCellAttributeMatrixName())->getAttributeArray(*iter);
            p->CopyTuple(neighbor, j);
          }
        }
      }
    }
  }

  // If there is an error set this to something negative and also set a message
  notifyStatusMessage("Complete");
}
