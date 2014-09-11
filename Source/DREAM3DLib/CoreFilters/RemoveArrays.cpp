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

#include "RemoveArrays.h"




// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RemoveArrays::RemoveArrays() :
  AbstractFilter()
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RemoveArrays::~RemoveArrays()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RemoveArrays::setupFilterParameters()
{
  FilterParameterVector parameters;

  DataContainerArrayProxyFilterParameter::Pointer parameter = DataContainerArrayProxyFilterParameter::New();
  parameter->setHumanLabel("Arrays to Delete");
  parameter->setPropertyName("DataArraysToRemove");
  parameter->setWidgetType(FilterParameterWidgetType::DataContainerArrayProxyWidget);
  parameter->setDefaultFlagValue(Qt::Unchecked);
  parameters.push_back(parameter);

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RemoveArrays::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  DataContainerArrayProxy proxy = reader->readDataContainerArrayProxy("DataArraysToRemove", getDataArraysToRemove() );
  setDataArraysToRemove(proxy);
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int RemoveArrays::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  DataContainerArrayProxy dcaProxy = getDataArraysToRemove(); // This line makes a COPY of the DataContainerArrayProxy that is stored in the current instance
  writer->writeValue("DataArraysToRemove", dcaProxy );
  writer->closeFilterGroup();
  return ++index;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RemoveArrays::dataCheck()
{
  setErrorCondition(0);
  // Remove all the arrays, but only if they are CHECKED. This is opposite of the default
  m_DataArraysToRemove.removeSelectionsFromDataContainerArray(getDataContainerArray().get(), Qt::Checked);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RemoveArrays::preflight()
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
void RemoveArrays::execute()
{
  int err = 0;
  QString ss;
  setErrorCondition(err);

  // Simply running the preflight will do what we need it to.
  dataCheck();
  if(getErrorCondition() < 0) { return; }
  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer RemoveArrays::newFilterInstance(bool copyFilterParameters)
{
  RemoveArrays::Pointer filter = RemoveArrays::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString RemoveArrays::getCompiledLibraryName()
{ return Core::CoreBaseName; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString RemoveArrays::getGroupName()
{ return DREAM3D::FilterGroups::CoreFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString RemoveArrays::getSubGroupName()
{ return DREAM3D::FilterSubGroups::MemoryManagementFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString RemoveArrays::getHumanLabel()
{ return "Delete Arrays"; }

