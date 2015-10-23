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

#include "EmptyFilter.h"

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersWriter.h"
#include "SIMPLib/FilterParameters/UnknownFilterParameter.h"

// Include the MOC generated file for this class
#include "moc_EmptyFilter.cpp"



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EmptyFilter::EmptyFilter() :
  AbstractFilter(),
  m_HumanLabel("Unknown Filter")
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EmptyFilter::~EmptyFilter()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EmptyFilter::setupFilterParameters()
{
  FilterParameterVector parameters;

  QString str;
  QTextStream ss(&str);
  ss << "The original filter '<b><i>" << getOriginalFilterName() << "</i></b>' was not loaded. ";
  ss << "Either a plugin with the original filter was not loaded or the original ";
  ss << "filter has been renamed.";
  parameters.push_back(UnknownFilterParameter::New(str, "OriginalFilterName", getOriginalFilterName(), FilterParameter::Parameter));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EmptyFilter::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setOriginalFilterName(reader->readString("OriginalFilterName", getOriginalFilterName()));
  reader->closeFilterGroup();

  /* Specifically for this filter we need to reset the filter parameters so that
   * it picks up the Original Filter name. This is NOT normal operation for any
   * other filters. Your filter should not be doing this. If you are copy/pasting
   * from this filter. STOP. Pick another filter.
   */
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int EmptyFilter::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  SIMPL_FILTER_WRITE_PARAMETER(FilterVersion)
  SIMPL_FILTER_WRITE_PARAMETER(OriginalFilterName)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EmptyFilter::dataCheck()
{
  QString ss = QObject::tr("This filter does nothing and was inserted as a place holder for filter '%1' that does not exist anymore.").arg(getOriginalFilterName());
  setErrorCondition(-9999);
  notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EmptyFilter::preflight()
{
  setInPreflight(true);
  dataCheck();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EmptyFilter::execute()
{
  dataCheck();

  /* Let the GUI know we are done with this filter */
  notifyStatusMessage(getHumanLabel(), "Complete");
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer EmptyFilter::newFilterInstance(bool copyFilterParameters)
{
  EmptyFilter::Pointer filter = EmptyFilter::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString EmptyFilter::getCompiledLibraryName()
{
  return Core::CoreBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString EmptyFilter::getGroupName()
{
  return DREAM3D::FilterGroups::CoreFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString EmptyFilter::getSubGroupName()
{
  return DREAM3D::FilterSubGroups::MiscFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString EmptyFilter::getHumanLabel()
{
  if(m_HumanLabel.isEmpty())
  {
    return QString::fromLatin1("Unknown Filter");
  }

  return QString("Unknown Filter: %1").arg(m_OriginalFilterName);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EmptyFilter::setHumanLabel(const QString& humanLabel)
{
  m_HumanLabel = humanLabel;
}
