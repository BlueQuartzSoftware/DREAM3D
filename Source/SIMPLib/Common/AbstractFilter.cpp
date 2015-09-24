/* ============================================================================
* Copyright (c) 2009-2015 BlueQuartz Software, LLC
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
* Neither the name of BlueQuartz Software, the US Air Force, nor the names of its
* contributors may be used to endorse or promote products derived from this software
* without specific prior written permission.
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
* The code contained herein was partially funded by the followig contracts:
*    United States Air Force Prime Contract FA8650-07-D-5800
*    United States Air Force Prime Contract FA8650-10-D-5210
*    United States Prime Contract Navy N00173-07-C-2068
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */


#include "AbstractFilter.h"

#include "SIMPLib/SIMPLibVersion.h"
#include "SIMPLib/Plugin/PluginManager.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::AbstractFilter() :
  Observable(),
  m_ErrorCondition(0),
  m_InPreflight(false),
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
  //  if(NULL != m_PreviousFilter.get())
  //  std::cout << "~PipelineFilterWidget() m_PreviousFilter " << this  << "  " << m_PreviousFilter->getNameOfClass().toStdString()
  //            << "  " << m_PreviousFilter.use_count() << std::endl;
  // m_PreviousFilter = AbstractFilter::NullPointer();

  //  if(NULL != m_NextFilter.get())
  //  std::cout << "~PipelineFilterWidget() m_NextFilter " << this  << "  " << m_NextFilter->getNameOfClass().toStdString()
  //            << "  " << m_NextFilter.use_count() << std::endl;
  //  m_NextFilter = AbstractFilter::NullPointer();
  //  std::cout << "~AbstractFilter" << std::endl;
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
  setErrorCondition(-3015);
  notifyErrorMessage(getNameOfClass(), "QAbstractFilter does not implement an execute method. Please use a subclass instead.", getErrorCondition());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AbstractFilter::preflight()
{
  setInPreflight(true);
  setErrorCondition(-3016);
  notifyErrorMessage(getNameOfClass(), "AbstractFilter does not implement a preflight method. Please use a subclass instead.", getErrorCondition());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool AbstractFilter::doesPipelineContainFilterBeforeThis(const QString& name)
{
  bool contains = false;
  // Check the previous filter
  AbstractFilter::Pointer prev = getPreviousFilter().lock();
  while(prev.get() != NULL)
  {
    if (prev->getNameOfClass().compare(name) == 0)
    {
      contains = true;
      break;
    }
    prev = prev->getPreviousFilter().lock();
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
  AbstractFilter::Pointer next = getNextFilter().lock();
  while(next.get() != NULL)
  {
    if (next->getNameOfClass().compare(name) == 0)
    {
      contains = true;
      break;
    }
    next = next->getNextFilter().lock();
  }
  return contains;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AbstractFilter::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  BOOST_ASSERT(reader != NULL);
  qDebug() << "AbstractFilter::readFilterParameters() -> Writing Filter Options" << "\n";
  return;
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
ISIMPLibPlugin* AbstractFilter::getPluginInstance()
{
  PluginManager* manager = PluginManager::Instance();
  QString pluginName = manager->getPluginName(getNameOfClass());
  ISIMPLibPlugin* plugin = manager->findPlugin(pluginName);
  return plugin;
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
void AbstractFilter::copyFilterParameterInstanceVariables(AbstractFilter* filter)
{
  filter->setFilterParameters(getFilterParameters() );

  //Loop over each Filter Parameter that is registered to the filter either through this class or a parent class
  // and copy the value from the current instance of the object into the "new" instance that was just created
  QVector<FilterParameter::Pointer> options = getFilterParameters(); // Get the current set of filter parameters
  for (QVector<FilterParameter::Pointer>::iterator iter = options.begin(); iter != options.end(); ++iter )
  {
    FilterParameter* parameter = (*iter).get();
    if (parameter->getReadOnly() == true )
    {
      continue; // Skip this type of filter parameter as it has nothing to do with anything in the filter.
    }
    // Get the property from the current instance of the filter
    QVariant var = property(parameter->getPropertyName().toLatin1().constData());
    if(parameter->getReadOnly() == false)
    {
      bool ok = filter->setProperty(parameter->getPropertyName().toLatin1().constData(), var);
      if(false == ok)
      {
        QString ss = QString("Error occurred transferring the Filter Parameter '%1' in Filter '%2' to the filter instance. The pipeline may run but the underlying filter will NOT be using the values from the GUI."
                             " Please report this issue to the developers of this filter.").arg(parameter->getPropertyName()).arg(filter->getHumanLabel());
        Q_ASSERT_X(ok, __FILE__, ss.toLatin1().constData());
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString AbstractFilter::getGroupName()
{
  return "YOUR CLASS SHOULD IMPLEMENT THIS";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString AbstractFilter::getSubGroupName()
{
  return "YOUR CLASS SHOULD IMPLEMENT THIS";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString AbstractFilter::getHumanLabel()
{
  return "YOUR CLASS SHOULD IMPLEMENT THIS";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString AbstractFilter::getBrandingString()
{
  return "DREAM3D Core Filter";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString AbstractFilter::getCompiledLibraryName()
{
  return "DREAM3DLib";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString AbstractFilter::getFilterVersion()
{
  QString version;
  QTextStream vStream(&version);
  vStream <<  SIMPLib::Version::Major() << "." << SIMPLib::Version::Minor() << "." << SIMPLib::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString AbstractFilter::generateHtmlSummary()
{
  QString html;
  QTextStream ss(&html);
  ss << "<html><head></head>\n";
  ss << "<body>\n";

  // A table for the summary items
  ss << "<table cellpadding=\"2\" cellspacing=\"0\" border=\"0\">\n";
  ss << "<tbody>\n";
  ss << "	<tr><th align=\"right\">Filter Name:</th><td>" << getHumanLabel() << "</td></tr>\n";
  ss << "	<tr><th align=\"right\">Filter Class Name:</th><td>" << getNameOfClass() << "</td></tr>\n";
  ss << "	<tr><th align=\"right\">Filter Group Name:</th><td>" << getGroupName() << "</td></tr>\n";
  ss << "	<tr><th align=\"right\">Filter Subroup Name:</th><td>" << getSubGroupName() << "</td></tr>\n";
  ss << "	<tr><th align=\"right\">Branding String:</th><td>" << getBrandingString() << "</td></tr>\n";
  ss << "	<tr><th align=\"right\">Compiled Plugin Name:</th><td>" << getCompiledLibraryName() << "</td></tr>\n";
  ss << "</tbody></table>\n";
  ss << "</body></html>";
  return html;
}
