/* ============================================================================
 * Copyright (c) 2011, Michael A. Jackson (BlueQuartz Software)
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
 * Neither the name of Michael A. Jackson nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
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
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "AbstractFilter.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::AbstractFilter() :
  Observable(),
  m_ErrorCondition(0),
  m_Cancel(false)
{
  m_DataContainerArray = DataContainerArray::New();
  setupFilterParameters();
  m_PreviousFilter = NullPointer();
  m_NextFilter = NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::~AbstractFilter()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AbstractFilter::setCancel(bool value)
{
  m_Cancel = value;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AbstractFilter::setupFilterParameters()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AbstractFilter::execute()
{
  setErrorCondition(-999);
  notifyErrorMessage(getNameOfClass(), "QAbstractFilter does not implement an execute method. Please use a subclass instead.", -999);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AbstractFilter::preflight()
{
  setErrorCondition(-999);
  notifyErrorMessage(getNameOfClass(), "AbstractFilter does not implement a preflight method. Please use a subclass instead.", -999);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool AbstractFilter::doesPipelineContainFilterBeforeThis(const QString& name)
{
  bool contains = false;
  // Check the previous filter
  AbstractFilter::Pointer prev = getPreviousFilter();
  while(prev.get() != NULL)
  {
    if (prev->getNameOfClass().compare(name) == 0)
    {
      contains = true;
      break;
    }
    prev = prev->getPreviousFilter();
  }
  return contains;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool AbstractFilter::doesPipelineContainFilterAfterThis(const QString& name)
{
  bool contains = false;
  // Check the previous filter
  AbstractFilter::Pointer next = getNextFilter();
  while(next.get() != NULL)
  {
    if (next->getNameOfClass().compare(name) == 0)
    {
      contains = true;
      break;
    }
    next = next->getNextFilter();
  }
  return contains;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AbstractFilter::addCreatedArrayHelpIndexEntry(CreatedArrayHelpIndexEntry::Pointer entry)
{
  m_CreatedArrayHelpIndexEntries.push_back(entry);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AbstractFilter::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);

  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int AbstractFilter::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  BOOST_ASSERT(writer != NULL);
  qDebug() << "AbstractFilter::writeFilterParameters() -> Writing Filter Options" << "\n";
  return -1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool AbstractFilter::getCancel()
{
  return m_Cancel;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer AbstractFilter::newFilterInstance(bool copyFilterParameters)
{
  return NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AbstractFilter::copyFilterParameterInstanceVariables(AbstractFilter *filter)
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
