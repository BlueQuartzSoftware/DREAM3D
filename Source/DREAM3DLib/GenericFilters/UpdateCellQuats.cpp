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

#include "UpdateCellQuats.h"

#include <sstream>

#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/DataArrays/IDataArray.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
UpdateCellQuats::UpdateCellQuats() :
  AbstractFilter(),
  m_Quats5ArrayPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, DREAM3D::CellData::Quats),
  m_QuatsArrayName(DREAM3D::CellData::Quats),
  m_Quats(NULL)
{
  m_OrientationOps = OrientationOps::getOrientationOpsVector();

  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
UpdateCellQuats::~UpdateCellQuats()
{
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void UpdateCellQuats::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(FilterParameter::New("Required Information", "", FilterParameterWidgetType::SeparatorWidget, "QString", true));
  parameters.push_back(FilterParameter::New("Quats5", "Quats5ArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, "DataArrayPath", true, ""));
  parameters.push_back(FilterParameter::New("Created Information", "", FilterParameterWidgetType::SeparatorWidget, "QString", true));
  parameters.push_back(FilterParameter::New("Quats", "QuatsArrayName", FilterParameterWidgetType::StringWidget, "QString", true, ""));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
void UpdateCellQuats::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setQuatsArrayName(reader->readString("QuatsArrayName", getQuatsArrayName() ) );
  setQuats5ArrayPath(reader->readDataArrayPath("Quats5ArrayPath", getQuats5ArrayPath() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int UpdateCellQuats::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("QuatsArrayName", getQuatsArrayName() );
  writer->writeValue("Quats5ArrayPath", getQuats5ArrayPath() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void UpdateCellQuats::dataCheck()
{
  DataArrayPath tempPath;
  setErrorCondition(0);

  QVector<size_t> dims(1, 5);
  m_Quats5Ptr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getQuats5ArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_Quats5Ptr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_Quats5 = m_Quats5Ptr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  oldQuats = DataArray<float>::CreateArray(m_Quats5Ptr.lock()->getNumberOfTuples(), m_Quats5Ptr.lock()->getComponentDimensions(), m_QuatsPtr.lock()->getName());
  float* quats5 = oldQuats->getPointer(0);
  size_t totalElements = m_Quats5Ptr.lock()->getNumberOfTuples()*m_Quats5Ptr.lock()->getNumberOfComponents();
  for(int iter=0;iter<totalElements;iter++)
  {
    quats5[iter] = m_Quats5[iter];
  }
  dims[0] = 4;
  tempPath.update(getQuats5ArrayPath().getDataContainerName(), getQuats5ArrayPath().getAttributeMatrixName(), getQuatsArrayName() );
  m_QuatsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this, tempPath, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_QuatsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_Quats = m_QuatsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void UpdateCellQuats::preflight()
{
  emit preflightAboutToExecute();
  emit updateFilterParameters(this);
  dataCheck();
  emit preflightExecuted();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void UpdateCellQuats::execute()
{
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  int64_t totalPoints = oldQuats->getNumberOfTuples();
  float* quats5 = oldQuats->getPointer(0);

  //copying the 5 component quaternions into the new 4 component quaternions
  // This is a special case for dealing with this conversion. If you are dealing with
  // quaternions then DO NOT USE THIS CODE as an example. Use another filter instead.
  for (int i = 0; i < totalPoints; i++)
  {
    for(int j = 0; j < 4; j++)
    {
      m_Quats[4 * i + j] = quats5[5 * i + (j + 1)];
    }
  }

  notifyStatusMessage(getHumanLabel(), "Complete");
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer UpdateCellQuats::newFilterInstance(bool copyFilterParameters)
{
  UpdateCellQuats::Pointer filter = UpdateCellQuats::New();
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
