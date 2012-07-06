/* ============================================================================
 * Copyright (c) 2012 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2012 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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
#include "DetermineGoodVoxels.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DetermineGoodVoxels::DetermineGoodVoxels():
AbstractFilter(),
m_CellPhasesArrayName(DREAM3D::CellData::Phases),
m_GoodVoxelsArrayName(DREAM3D::CellData::GoodVoxels),
m_GoodVoxels(NULL)
{
  setupFilterOptions();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DetermineGoodVoxels::~DetermineGoodVoxels()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DetermineGoodVoxels::setupFilterOptions()
{
  std::vector<FilterOption::Pointer> options;

  setFilterOptions(options);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DetermineGoodVoxels::writeFilterOptions(AbstractFilterOptionsWriter* writer)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DetermineGoodVoxels::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;
  DataContainer* m = getDataContainer();
  CREATE_NON_PREREQ_DATA(m, DREAM3D, CellData, GoodVoxels, ss, bool, BoolArrayType, true, voxels, 1);
  GET_PREREQ_DATA(m, DREAM3D, CellData, CellPhases, ss, -302, int32_t, Int32ArrayType,  voxels, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DetermineGoodVoxels::preflight()
{
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DetermineGoodVoxels::execute()
{
  DataContainer* m = getDataContainer();
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The DataContainer Object was NULL", -999);
    return;
  }
  setErrorCondition(0);
  dataCheck(false, m->getTotalPoints(), m->getNumFieldTuples(), m->getNumEnsembleTuples());
  //int err = 0;
  std::stringstream ss;

  // Create an Array of Void Pointers that will point to the data that is going to
  // serve as the filter data, such as Confidence Index or Image Quality
  std::vector<void*> dataPointers(m_QualityMetricFilters.size(), NULL);
  std::vector<Ebsd::NumType> dataTypes(m_QualityMetricFilters.size(), Ebsd::UnknownNumType);

  // Gather some information about the filters and types in order to run the QualityMetric Filter
  for (size_t i = 0; i < m_QualityMetricFilters.size(); ++i)
  {
    dataPointers[i] = m_EbsdVolumeReader->getPointerByName(m_QualityMetricFilters[i]->getFieldName());
    dataTypes[i] = m_EbsdVolumeReader->getPointerType(m_QualityMetricFilters[i]->getFieldName());
  }

  // Figure out which are good voxels
  int64_t nPoints = m->getTotalPoints();


  if(m_QualityMetricFilters.size() == 0)
  {
    ss.str("");
    ss << getHumanLabel() << " - No filters have been added to do the analysis.";
    setErrorCondition(-100);
    addErrorMessage(getNameOfClass(), ss.str(), -1);
    return;
  }


  size_t nFilters = m_QualityMetricFilters.size();
  for (size_t i = 0; i < nFilters; ++i)
  {
    m_QualityMetricFilters[i]->setInput(dataPointers[i]);
    m_QualityMetricFilters[i]->setDataType(dataTypes[i]);
    m_QualityMetricFilters[i]->setNumValues(nPoints);
    m_QualityMetricFilters[i]->setInputPhaseData(m_CellPhases);
    m_QualityMetricFilters[i]->filter();
    ss.str("");
    ss << getHumanLabel() << " - " << i << "/" << nFilters-1 << " Quality Metric Filters Complete";
    notifyStatusMessage(ss.str());
  }

  // Get the first bool array to use as a reference
  QualityMetricFilter::Pointer qmFilter = m_QualityMetricFilters[0];
  if(qmFilter.get() == NULL)
  {
    ss.str("");
    ss << getHumanLabel() << " - QualityMetricFilter[0] is NULL";
    setErrorCondition(-100);
    addErrorMessage(getNameOfClass(), ss.str(), -1);
    return;
  }
  BoolArrayType::Pointer baseArray = m_QualityMetricFilters[0]->getOutput();
  baseArray->SetName(DREAM3D::CellData::GoodVoxels);
  bool* baseArrayPtr = baseArray->GetPointer(0);
  if(NULL == baseArrayPtr)
  {
    ss.str("");
    ss << getHumanLabel() << " - baseArrayPtr is NULL";
    setErrorCondition(-101);
    addErrorMessage(getNameOfClass(), ss.str(), -1);
    return;
  }

  for (size_t i = 1; i < nFilters; ++i)
  {
    BoolArrayType::Pointer currentArray = m_QualityMetricFilters[i]->getOutput();
    if(currentArray.get() == NULL)
    {
      ss.str("");
      ss << getHumanLabel() << " - currentArray is NULL";
      setErrorCondition(-102);
      addErrorMessage(getNameOfClass(), ss.str(), -1);
      return;
    }

    bool* currentArrayPtr = currentArray->GetPointer(0);
    if(NULL == currentArrayPtr)
    {
      ss.str("");
      ss << getHumanLabel() << " - currentArray returned NULL";
      setErrorCondition(-103);
      addErrorMessage(getNameOfClass(), ss.str(), -1);
      return;
    }
    for (int64_t p = 0; p < nPoints; ++p)
    {
      if(baseArrayPtr[p] == false || currentArrayPtr[p] == false)
      {
        baseArrayPtr[p] = false;
      }
    }
  }

  m->addCellData(DREAM3D::CellData::GoodVoxels, baseArray);
 notifyStatusMessage("Determine Good Voxels Complete");
}

