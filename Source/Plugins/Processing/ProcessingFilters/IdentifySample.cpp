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

#include "IdentifySample.h"


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
IdentifySample::IdentifySample() :
  AbstractFilter(),
  m_FillHoles(false),
  m_GoodVoxelsArrayPath(DREAM3D::Defaults::DataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, DREAM3D::CellData::GoodVoxels),
  m_GoodVoxelsArrayName(DREAM3D::CellData::GoodVoxels),
  m_GoodVoxels(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IdentifySample::~IdentifySample()
{
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void IdentifySample::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(FilterParameter::New("Fill Holes in Largest Feature", "FillHoles", FilterParameterWidgetType::BooleanWidget, getFillHoles(), false));
  parameters.push_back(FilterParameter::New("Required Information", "", FilterParameterWidgetType::SeparatorWidget, "", true));
  parameters.push_back(FilterParameter::New("GoodVoxels", "GoodVoxelsArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getGoodVoxelsArrayPath(), true, ""));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
void IdentifySample::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setFillHoles(reader->readValue("FillHoles", getFillHoles() ) );
  setGoodVoxelsArrayPath(reader->readDataArrayPath("GoodVoxelsArrayPath", getGoodVoxelsArrayPath() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int IdentifySample::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  DREAM3D_FILTER_WRITE_PARAMETER(FilterVersion)
  DREAM3D_FILTER_WRITE_PARAMETER(FillHoles)
  DREAM3D_FILTER_WRITE_PARAMETER(GoodVoxelsArrayPath)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void IdentifySample::dataCheck()
{
  setErrorCondition(0);

  QVector<size_t> dims(1, 1);
  m_GoodVoxelsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<bool>, AbstractFilter>(this, getGoodVoxelsArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_GoodVoxelsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_GoodVoxels = m_GoodVoxelsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCondition() < 0) { return; }

  ImageGeom::Pointer image = getDataContainerArray()->getDataContainer(getGoodVoxelsArrayPath().getDataContainerName())->getPrereqGeometry<ImageGeom, AbstractFilter>(this);
  if(getErrorCondition() < 0 || NULL == image.get()) { return; }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void IdentifySample::preflight()
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
void IdentifySample::execute()
{
  setErrorCondition(0);

  dataCheck();
  if(getErrorCondition() < 0) { return; }

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(m_GoodVoxelsArrayPath.getDataContainerName());
  int64_t totalPoints = m_GoodVoxelsPtr.lock()->getNumberOfTuples();

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

  DimType neighpoints[6];
  DimType xp = dims[0];
  DimType yp = dims[1];
  DimType zp = dims[2];

  neighpoints[0] = -(xp * yp);
  neighpoints[1] = -xp;
  neighpoints[2] = -1;
  neighpoints[3] = 1;
  neighpoints[4] = xp;
  neighpoints[5] = (xp * yp);
  std::vector<int> currentvlist;
  std::vector<bool> checked(totalPoints, false);
  std::vector<bool> sample(totalPoints, false);
  int biggestBlock = 0;
  size_t count;
  int good;
  int neighbor;
  DimType column, row, plane;
  int index;

  // In this loop over the data we are finding the biggest contiguous set of GoodVoxels and calling that the 'sample'  All GoodVoxels that do not touch the 'sample'
  // are flipped to be called 'bad' voxels or 'not sample'
  for (int i = 0; i < totalPoints; i++)
  {
    if(checked[i] == false && m_GoodVoxels[i] == true)
    {
      currentvlist.push_back(i);
      count = 0;
      while (count < currentvlist.size())
      {
        index = currentvlist[count];
        column = index % xp;
        row = (index / xp) % yp;
        plane = index / (xp * yp);
        for (int j = 0; j < 6; j++)
        {
          good = 1;
          neighbor = static_cast<int>( index + neighpoints[j] );
          if(j == 0 && plane == 0) { good = 0; }
          if(j == 5 && plane == (zp - 1)) { good = 0; }
          if(j == 1 && row == 0) { good = 0; }
          if(j == 4 && row == (yp - 1)) { good = 0; }
          if(j == 2 && column == 0) { good = 0; }
          if(j == 3 && column == (xp - 1)) { good = 0; }
          if(good == 1 && checked[neighbor] == false && m_GoodVoxels[neighbor] == true)
          {
            currentvlist.push_back(neighbor);
            checked[neighbor] = true;
          }
        }
        count++;
      }
      if(static_cast<int>(currentvlist.size()) >= biggestBlock)
      {
        biggestBlock = currentvlist.size();
        sample.assign(totalPoints, false);
        for(size_t j = 0; j < biggestBlock; j++)
        {
          sample[currentvlist[j]] = true;
        }
      }
      currentvlist.clear();
    }
  }
  for (int i = 0; i < totalPoints; i++)
  {
    if (sample[i] == false && m_GoodVoxels[i] == true) { m_GoodVoxels[i] = false; }
  }
  sample.clear();
  checked.assign(totalPoints, false);


  // In this loop we are going to 'close' all of the 'holes' inside of the region already identified as the 'sample' if the user chose to do so.
  // This is done by flipping all 'bad' voxel features that do not touch the outside of the sample (i.e. they are fully contained inside of the 'sample'.
  if(m_FillHoles == true)
  {
    bool touchesBoundary = false;
    for (int i = 0; i < totalPoints; i++)
    {
      if(checked[i] == false && m_GoodVoxels[i] == false)
      {
        currentvlist.push_back(i);
        count = 0;
        touchesBoundary = false;
        while (count < currentvlist.size())
        {
          index = currentvlist[count];
          column = index % xp;
          row = (index / xp) % yp;
          plane = index / (xp * yp);
          if(column == 0 || column == (xp - 1) || row == 0 || row == (yp - 1) || plane == 0 || plane == (zp - 1)) { touchesBoundary = true; }
          for (int j = 0; j < 6; j++)
          {
            good = 1;
            neighbor = static_cast<int>( index + neighpoints[j] );
            if(j == 0 && plane == 0) { good = 0; }
            if(j == 5 && plane == (zp - 1)) { good = 0; }
            if(j == 1 && row == 0) { good = 0; }
            if(j == 4 && row == (yp - 1)) { good = 0; }
            if(j == 2 && column == 0) { good = 0; }
            if(j == 3 && column == (xp - 1)) { good = 0; }
            if(good == 1 && checked[neighbor] == false && m_GoodVoxels[neighbor] == false)
            {
              currentvlist.push_back(neighbor);
              checked[neighbor] = true;
            }
          }
          count++;
        }
        if(touchesBoundary == false)
        {
          for(size_t j = 0; j < currentvlist.size(); j++)
          {
            m_GoodVoxels[currentvlist[j]] = true;
          }
        }
        currentvlist.clear();
      }
    }
  }
  checked.clear();

  // If there is an error set this to something negative and also set a message
  notifyStatusMessage(getHumanLabel(), "Identifying Sample Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer IdentifySample::newFilterInstance(bool copyFilterParameters)
{
  IdentifySample::Pointer filter = IdentifySample::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString IdentifySample::getCompiledLibraryName()
{ return Processing::ProcessingBaseName; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString IdentifySample::getGroupName()
{ return DREAM3D::FilterGroups::ProcessingFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString IdentifySample::getHumanLabel()
{ return "Isolate Largest Feature (Identify Sample)"; }

