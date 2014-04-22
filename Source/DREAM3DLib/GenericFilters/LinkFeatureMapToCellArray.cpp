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

#include "LinkFeatureMapToCellArray.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
LinkFeatureMapToCellArray::LinkFeatureMapToCellArray() :
  AbstractFilter(),
  m_CellFeatureAttributeMatrixName(DREAM3D::Defaults::CellFeatureAttributeMatrixName),
  m_SelectedCellArrayPath("", "", ""),
  m_ActiveArrayName(""),
  m_SelectedCellDataArrayName(""),
  m_SelectedCellData(NULL),
  m_Active(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
LinkFeatureMapToCellArray::~LinkFeatureMapToCellArray()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void LinkFeatureMapToCellArray::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(FilterParameter::New("Cell Array", "SelectedCellArrayPath", FilterParameterWidgetType::DataArraySelectionWidget,"DataArrayPath", false));
  parameters.push_back(FilterParameter::New("Created Information", "", FilterParameterWidgetType::SeparatorWidget, "QString", true));
  parameters.push_back(FilterParameter::New("Cell Feature Attribute Matrix Name", "CellFeatureAttributeMatrixName", FilterParameterWidgetType::StringWidget, "QString", true, ""));
  parameters.push_back(FilterParameter::New("Active", "ActiveArrayName", FilterParameterWidgetType::StringWidget, "QString", true, ""));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void LinkFeatureMapToCellArray::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setCellFeatureAttributeMatrixName(reader->readString("CellFeatureAttributeMatrixName", getCellFeatureAttributeMatrixName()));
  setActiveArrayName(reader->readString("ActiveArrayName", getActiveArrayName()));
  setSelectedCellArrayPath( reader->readDataArrayPath( "SelectedCellArrayPath", getSelectedCellArrayPath() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int LinkFeatureMapToCellArray::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("ActiveArrayName", getActiveArrayName());
  writer->writeValue("CellFeatureAttributeMatrixName", getCellFeatureAttributeMatrixName());
  writer->writeValue("SelectedCellArrayPath", getSelectedCellArrayPath() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void LinkFeatureMapToCellArray::updateFeatureInstancePointers()
{
  setErrorCondition(0);

  if( NULL != m_ActivePtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_Active = m_ActivePtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void LinkFeatureMapToCellArray::dataCheck()
{
  DataArrayPath tempPath;
  setErrorCondition(0);

  VolumeDataContainer* m = getDataContainerArray()->getPrereqDataContainer<VolumeDataContainer, AbstractFilter>(this, getSelectedCellArrayPath().getDataContainerName(), false);
  if(getErrorCondition() < 0 || NULL == m) { return; }
  QVector<size_t> tDims(1, 0);
  AttributeMatrix::Pointer cellFeatureAttrMat = m->createNonPrereqAttributeMatrix<AbstractFilter>(this, getCellFeatureAttributeMatrixName(), tDims, DREAM3D::AttributeMatrixType::CellFeature);
  if(getErrorCondition() < 0) { return; }

  QVector<size_t> dims(1, 1);
  m_SelectedCellDataPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getSelectedCellArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_SelectedCellDataPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_SelectedCellData = m_SelectedCellDataPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  tempPath.update(getSelectedCellArrayPath().getDataContainerName(), getCellFeatureAttributeMatrixName(), getActiveArrayName() );
  m_ActivePtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<bool>, AbstractFilter, bool>(this, tempPath, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_ActivePtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_Active = m_ActivePtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void LinkFeatureMapToCellArray::preflight()
{
  emit preflightAboutToExecute();
  emit updateFilterParameters(this);
  dataCheck();
  emit preflightExecuted();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void LinkFeatureMapToCellArray::execute()
{
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getSelectedCellArrayPath().getDataContainerName());
  int64_t totalPoints = m_SelectedCellDataPtr.lock()->getNumberOfTuples();

  int maxIndex = 0;
  std::vector<bool> active;
  for(int64_t i = 0; i < totalPoints; i++)
  {
    int index = m_SelectedCellData[i];
    if((index + 1) > maxIndex)
    {
      active.resize(index + 1);
      active[index] = true;
      maxIndex = index + 1;
    }
  }

  QVector<size_t> tDims(1, maxIndex);
  m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->resizeAttributeArrays(tDims);
  updateFeatureInstancePointers();

  for(int i = 0; i < maxIndex; i++)
  {
    m_Active[i] = active[i];
  }

  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer LinkFeatureMapToCellArray::newFilterInstance(bool copyFilterParameters)
{
  /*
  * SelectedCellDataArrayName
  */
  LinkFeatureMapToCellArray::Pointer filter = LinkFeatureMapToCellArray::New();
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
