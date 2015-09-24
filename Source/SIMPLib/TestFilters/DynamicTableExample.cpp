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
*                           FA8650-10-D-5210
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include "DynamicTableExample.h"

#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersWriter.h"
#include "SIMPLib/FilterParameters/DynamicTableFilterParameter.h"



// Include the MOC generated file for this class
#include "moc_DynamicTableExample.cpp"



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DynamicTableExample::DynamicTableExample() :
  AbstractFilter()
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DynamicTableExample::~DynamicTableExample()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DynamicTableExample::setupFilterParameters()
{
  QVector<FilterParameter::Pointer> parameters;
  /* Place all your option initialization code here */

  // Table 1 - Fixed rows and columns, no default data passed in
  {
    QStringList rHeaders, cHeaders;
    std::vector<std::vector<double> > defaultTable;
    parameters.push_back(DynamicTableFilterParameter::New("Dynamic Table 1", "DynamicData1", rHeaders, cHeaders, defaultTable, FilterParameter::Parameter, false, false, 0));
  }

  // Table 2 - Fixed rows and columns, default data passed in
  {
    QStringList rHeaders, cHeaders;
    rHeaders << "Passed RowName 1" << "Passed RowName 2";
    cHeaders << "Passed ColName 1" << "Passed ColName 2";
    std::vector<std::vector<double> > defaultTable(4, std::vector<double>(1, 3.87));
    parameters.push_back(DynamicTableFilterParameter::New("Dynamic Table 2", "DynamicData2", rHeaders, cHeaders, defaultTable, FilterParameter::Parameter, false, false, 0));
  }

  // Table 3 - Dynamic rows and fixed columns, default data passed in
  {
    QStringList rHeaders, cHeaders;
    rHeaders << "Passed RowName 1" << "Passed RowName 2";
    cHeaders << "Passed ColName 1" << "Passed ColName 2";
    std::vector<std::vector<double> > defaultTable(3, std::vector<double>(5, 1.34));
    parameters.push_back(DynamicTableFilterParameter::New("Dynamic Table 3", "DynamicData3", rHeaders, cHeaders, defaultTable, FilterParameter::Parameter, true, false, 0));
  }

  // Table 4 - Fixed rows and dynamic columns, default data passed in
  {
    QStringList rHeaders, cHeaders;
    rHeaders << "Passed RowName 1" << "Passed RowName 2";
    cHeaders << "Passed ColName 1" << "Passed ColName 2";
    std::vector<std::vector<double> > defaultTable(2, std::vector<double>(2, 3));
    parameters.push_back(DynamicTableFilterParameter::New("Dynamic Table 4", "DynamicData4", rHeaders, cHeaders, defaultTable, FilterParameter::Parameter, false, true, 0));
  }

  // Table 5 - Dynamic rows and dynamic columns, default data passed in
  {
    QStringList rHeaders, cHeaders;
    rHeaders << "Passed RowName 1" << "Passed RowName 2";
    cHeaders << "Passed ColName 1" << "Passed ColName 2";
    std::vector<std::vector<double> > defaultTable(2, std::vector<double>(2, 3));
    parameters.push_back(DynamicTableFilterParameter::New("Dynamic Table 5", "DynamicData5", rHeaders, cHeaders, defaultTable, FilterParameter::Parameter, true, true, 0));
  }




  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DynamicTableExample::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{

  reader->openFilterGroup(this, index);
  setDynamicData1(reader->readDynamicTableData("DynamicData", getDynamicData1()));
  setDynamicData2(reader->readDynamicTableData("DynamicData", getDynamicData2()));
  setDynamicData3(reader->readDynamicTableData("DynamicData", getDynamicData3()));
  setDynamicData4(reader->readDynamicTableData("DynamicData", getDynamicData4()));
  setDynamicData5(reader->readDynamicTableData("DynamicData", getDynamicData5()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int DynamicTableExample::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  SIMPL_FILTER_WRITE_PARAMETER(FilterVersion)
  SIMPL_FILTER_WRITE_PARAMETER(DynamicData1)
  SIMPL_FILTER_WRITE_PARAMETER(DynamicData2)
  SIMPL_FILTER_WRITE_PARAMETER(DynamicData3)
  SIMPL_FILTER_WRITE_PARAMETER(DynamicData4)
  SIMPL_FILTER_WRITE_PARAMETER(DynamicData5)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DynamicTableExample::dataCheck()
{
  // This filter does nothing during the dataCheck!
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DynamicTableExample::preflight()
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
void DynamicTableExample::execute()
{
  // This filter does nothing during execute!

  /* Let the GUI know we are done with this filter */
  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer DynamicTableExample::newFilterInstance(bool copyFilterParameters)
{
  DynamicTableExample::Pointer filter = DynamicTableExample::New();
  if (true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString DynamicTableExample::getCompiledLibraryName()
{
  return Test::TestBaseName;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString DynamicTableExample::getGroupName()
{
  return DREAM3D::FilterGroups::TestFilters;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString DynamicTableExample::getSubGroupName()
{
  return "Misc";
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString DynamicTableExample::getHumanLabel()
{
  return "Dynamic Table Example";
}

