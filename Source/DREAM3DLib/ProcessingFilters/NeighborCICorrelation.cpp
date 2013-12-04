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

#include "NeighborCICorrelation.h"


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
NeighborCICorrelation::NeighborCICorrelation() :
  AbstractFilter(),
  m_DataContainerName(DREAM3D::Defaults::VolumeDataContainerName),
  m_CellAttributeMatrixName(DREAM3D::Defaults::CellAttributeMatrixName),
  m_MinConfidence(0.1),
  m_Loop(false),
  m_ConfidenceIndexArrayName(DREAM3D::CellData::ConfidenceIndex),
  m_ConfidenceIndex(NULL)
{
  m_OrientationOps = OrientationOps::getOrientationOpsVector();
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
NeighborCICorrelation::~NeighborCICorrelation()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void NeighborCICorrelation::setupFilterParameters()
{
  FilterParameterVector parameters;
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setPropertyName("MinConfidence");
    option->setHumanLabel("Minimum Confidence Index");
    option->setWidgetType(FilterParameter::DoubleWidget);
    option->setValueType("float");
    option->setCastableValueType("double");
    option->setUnits("");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Loop Until Gone");
    option->setPropertyName("Loop");
    option->setWidgetType(FilterParameter::BooleanWidget);
    option->setValueType("bool");
    parameters.push_back(option);
  }

  setFilterParameters(parameters);
}
// -----------------------------------------------------------------------------
void NeighborCICorrelation::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  /* Code to read the values goes between these statements */
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE BEGIN*/
  setMinConfidence( reader->readValue("MinConfidence", getMinConfidence()) );
  setLoop( reader->readValue("Loop", false) );
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE END*/
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int NeighborCICorrelation::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("MinConfidence", getMinConfidence() );
  writer->writeValue("Loop", getLoop() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void NeighborCICorrelation::dataCheck()
{
  setErrorCondition(0);

  VolumeDataContainer* m = getDataContainerArray()->getPrereqDataContainer<VolumeDataContainer, NeighborCICorrelation>(this, getDataContainerName(), false);
  if(getErrorCondition() < 0) { return; }

  QVector<int> dims(1, 1);
  m_ConfidenceIndexPtr = cellAttrMat->getPrereqArray<DataArray<float>, AbstractFilter>(this, m_ConfidenceIndexArrayName, -301, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_ConfidenceIndexPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_ConfidenceIndex = m_ConfidenceIndexPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void NeighborCICorrelation::preflight()
{

  dataCheck();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void NeighborCICorrelation::execute()
{
  setErrorCondition(0);
// int err = 0;
  VolumeDataContainer* m = getDataContainerArray()->getPrereqDataContainer<VolumeDataContainer, AbstractFilter>(this, getDataContainerName(), false);
  if(getErrorCondition() < 0) { return; }


  int64_t totalPoints = m->getAttributeMatrix(getCellAttributeMatrixName())->getNumTuples();
  dataCheck(false, totalPoints, 0, 0);
  if (getErrorCondition() < 0 && getErrorCondition() != -305)
  {
    return;
  }
  setErrorCondition(0);

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
  int neighbor;
  DimType column, row, plane;

  int neighpoints[6];
  neighpoints[0] = static_cast<int>(-dims[0] * dims[1]);
  neighpoints[1] = static_cast<int>(-dims[0]);
  neighpoints[2] = static_cast<int>(-1);
  neighpoints[3] = static_cast<int>(1);
  neighpoints[4] = static_cast<int>(dims[0]);
  neighpoints[5] = static_cast<int>(dims[0] * dims[1]);

  QVector<int> bestNeighbor(totalPoints, -1);

  int count = 0;
  float best;

  bool keepGoing = true;
  while(keepGoing == true)
  {
    keepGoing = false;
    count = 0;
    for (int64_t i = 0; i < totalPoints; i++)
    {
      if(m_ConfidenceIndex[i] < m_MinConfidence)
      {
        column = i % dims[0];
        row = (i / dims[0]) % dims[1];
        plane = i / (dims[0] * dims[1]);
        count++;
        best = m_ConfidenceIndex[i];
        for (DimType j = 0; j < 6; j++)
        {
          good = 1;
          neighbor = i + neighpoints[j];
          if (j == 0 && plane == 0) { good = 0; }
          if (j == 5 && plane == (dims[2] - 1)) { good = 0; }
          if (j == 1 && row == 0) { good = 0; }
          if (j == 4 && row == (dims[1] - 1)) { good = 0; }
          if (j == 2 && column == 0) { good = 0; }
          if (j == 3 && column == (dims[0] - 1)) { good = 0; }
          if (good == 1)
          {
            if(m_ConfidenceIndex[neighbor] >= m_MinConfidence && m_ConfidenceIndex[neighbor] > best)
            {
              best = m_ConfidenceIndex[neighbor];
              bestNeighbor[i] = neighbor;
            }
          }
        }
      }
    }
    QList<QString> voxelArrayNames = m->getAttributeMatrix(getCellAttributeMatrixName())->getAttributeArrayNameList();
    for (size_t j = 0; j < totalPoints; j++)
    {
      neighbor = bestNeighbor[j];
      if (neighbor != -1)
      {
        for(QList<QString>::iterator iter = voxelArrayNames.begin(); iter != voxelArrayNames.end(); ++iter)
        {
          QString name = *iter;
          IDataArray::Pointer p = m->getAttributeMatrix(getCellAttributeMatrixName())->getAttributeArray(*iter);
          p->CopyTuple(neighbor, j);
        }
      }
    }
    if(m_Loop == true && count > 0) { keepGoing = true; }
  }

// If there is an error set this to something negative and also set a message
  notifyStatusMessage("Filling Bad Data Complete");
}

