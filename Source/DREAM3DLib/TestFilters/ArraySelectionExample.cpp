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
#include "ArraySelectionExample.h"




// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ArraySelectionExample::ArraySelectionExample() :
  AbstractFilter()
  //  m_SelectedArrayName(""),
  //  m_DataContainerName(DREAM3D::Defaults::VolumeDataContainerName)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ArraySelectionExample::~ArraySelectionExample()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ArraySelectionExample::setupFilterParameters()
{
  QVector<FilterParameter::Pointer> parameters;
  /* To select arrays */
  parameters.push_back(FilterParameter::New("Array to Select", "DataContainerArrayProxy", FilterParameterWidgetType::DataContainerArrayProxyWidget, "", false));


  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ArraySelectionExample::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{

  reader->openFilterGroup(this, index);
  //  setDataContainerName( reader->readString("DataContainerName", getDataContainerName()) );
  //  setAttributeMatrixName( reader->readString("AttributeMatrixName", getAttributeMatrixName()) );
  //  setSelectedArrayName( reader->readString("SelectedArrayName", getSelectedArrayName()) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int ArraySelectionExample::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  //  writer->writeValue("DataContainerName", getDataContainerName());
  //  writer->writeValue("AttributeMatrixName", getAttributeMatrixName());
  //  writer->writeValue("SelectedArrayName", getSelectedArrayName());
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ArraySelectionExample::dataCheck()
{
  //std::cout << " ArraySelectionExample   Preflighting " << std::endl;
  setErrorCondition(0);

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ArraySelectionExample::preflight()
{
  setInPreflight(true);
  // Read up the structure from the file
  m_DataContainerArrayProxy = DataContainerArrayProxy(getDataContainerArray().get());
  // Annouce we are about to preflight
  emit preflightAboutToExecute();
  emit updateFilterParameters(this);
  dataCheck();
  emit preflightExecuted();
  setInPreflight(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ArraySelectionExample::execute()
{
  int err = 0;
  setErrorCondition(err);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  /* Place all your code to execute your filter here. */

  /* Let the GUI know we are done with this filter */
  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer ArraySelectionExample::newFilterInstance(bool copyFilterParameters)
{
  /*
  * DataContainerArrayProxy
  */
  ArraySelectionExample::Pointer filter = ArraySelectionExample::New();
  if(true == copyFilterParameters)
  {
    filter->setDataContainerArrayProxy(getDataContainerArrayProxy());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ArraySelectionExample::getCompiledLibraryName()
{ return Test::TestBaseName; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ArraySelectionExample::getGroupName()
{ return DREAM3D::FilterGroups::TestFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ArraySelectionExample::getSubGroupName()
{ return "Misc"; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ArraySelectionExample::getHumanLabel()
{ return "DataContainerArrayProxy Example"; }

