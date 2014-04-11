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
  m_GoodVoxelsArrayPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, DREAM3D::Defaults::SomePath),
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
  parameters.push_back(FilterParameter::New("Required Information", "", FilterParameterWidgetType::SeparatorWidget, "QString", true));
/*[]*/parameters.push_back(FilterParameter::New("GoodVoxels", "GoodVoxelsArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, "DataArrayPath", true, ""));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
void IdentifySample::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setGoodVoxelsArrayPath(reader->readDataArrayPath("GoodVoxelsArrayPath", getGoodVoxelsArrayPath() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int IdentifySample::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("GoodVoxelsArrayPath", getGoodVoxelsArrayPath() );
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
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void IdentifySample::preflight()
{
  emit preflightAboutToExecute();
  emit updateFilterParameters(this);
  dataCheck();
  emit preflightExecuted();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void IdentifySample::execute()
{
  setErrorCondition(0);

  dataCheck();
  if(getErrorCondition() < 0) { return; }

  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(m_GoodVoxelsArrayPath.getDataContainerName());
  int64_t totalPoints = m_GoodVoxelsPtr.lock()->getNumberOfTuples();

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
  QVector<int> currentvlist;
  QVector<bool> checked(totalPoints, false);
  QVector<bool> Sample(totalPoints, false);
  QVector<bool> notSample(totalPoints, false);
  int biggestBlock = 0;
  size_t count;
  int good;
  int neighbor;
  DimType column, row, plane;
  int index;

  for (int i = 0; i < totalPoints; i++)
  {
    if(checked[i] == false && m_GoodVoxels[i] == false)
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
          if(good == 1 && checked[neighbor] == false && m_GoodVoxels[neighbor] == false)
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
        for(int j = 0; j < totalPoints; j++)
        {
          notSample[j] = false;
        }
        for(size_t j = 0; j < currentvlist.size(); j++)
        {
          notSample[currentvlist[j]] = true;
        }
      }
      currentvlist.clear();
    }
  }
  for (int i = 0; i < totalPoints; i++)
  {
    if (notSample[i] == false && m_GoodVoxels[i] == false) { m_GoodVoxels[i] = true; }
    else if (notSample[i] == true && m_GoodVoxels[i] == true) { m_GoodVoxels[i] = false; }
  }
  notSample.clear();
  checked.fill(false, totalPoints);

  biggestBlock = 0;
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
        for(int j = 0; j < totalPoints; j++)
        {
          Sample[j] = false;
        }
        for(size_t j = 0; j < currentvlist.size(); j++)
        {
          Sample[currentvlist[j]] = true;
        }
      }
      currentvlist.clear();
    }
  }
  for (int i = 0; i < totalPoints; i++)
  {
    if (Sample[i] == false && m_GoodVoxels[i] == true) { m_GoodVoxels[i] = false; }
    else if (Sample[i] == true && m_GoodVoxels[i] == false) { m_GoodVoxels[i] = true; }
  }
  Sample.clear();
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
    filter->setFilterParameters(getFilterParameters() );
    
    //Loop over each Filter Parameter that is registered to the filter either through this class or a parent class
    // and copy the value from the current instance of the object into the "new" instance that was just created
    QVector<FilterParameter::Pointer> options = getFilterParameters(); // Get the current set of filter parameters
    for (QVector<FilterParameter::Pointer>::iterator iter = options.begin(); iter != options.end(); ++iter )
    {
      FilterParameter* parameter = (*iter).get();
      if (parameter->getWidgetType().compare(FilterParameterWidgetType::SeparatorWidget) == 0 )
      {
        continue; // Skip this type of filter parameter as it has nothing to do with anything in the filter.
      }
      // Get the property from the current instance of the filter
      QVariant var = property(parameter->getPropertyName().toLatin1().constData());
      bool ok = filter->setProperty(parameter->getPropertyName().toLatin1().constData(), var);
      if(false == ok)
      {
        QString ss = QString("Error occurred transferring the Filter Parameter '%1' in Filter '%2' to the filter instance. The pipeline may run but the underlying filter will NOT be using the values from the GUI."
                             " Please report this issue to the developers of this filter.").arg(parameter->getPropertyName()).arg(filter->getHumanLabel());
        Q_ASSERT_X(ok, __FILE__, ss.toLatin1().constData());
      }

      if(parameter->isConditional() == true)
      {
        QVariant cond = property(parameter->getConditionalProperty().toLatin1().constData() );
        ok = filter->setProperty(parameter->getConditionalProperty().toLatin1().constData(), cond);
        if(false == ok)
        {
          QString ss = QString("%1::newFilterInstance()\nError occurred transferring the Filter Parameter '%2' in Filter '%3' to the filter instance. "
                              " The filter parameter has a conditional property '%4'. The transfer of this property from the old filter to the new filter failed."
                              " Please report this issue to the developers of this filter.").arg(filter->getNameOfClass())
                              .arg(parameter->getPropertyName())
                              .arg(filter->getHumanLabel())
                              .arg(parameter->getConditionalProperty());
          Q_ASSERT_X(ok, __FILE__, ss.toLatin1().constData());
        }
      }
    }
  }
  return filter;
}
