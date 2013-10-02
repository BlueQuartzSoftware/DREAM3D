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

#include "FieldDataCSVWriter.h"


#include "MXA/Utilities/MXAFileInfo.h"
#include "MXA/Utilities/MXADir.h"

#include "DREAM3DLib/Math/DREAM3DMath.h"
#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Common/DREAM3DRandom.h"
#include "DREAM3DLib/DataArrays/NeighborList.hpp"






// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FieldDataCSVWriter::FieldDataCSVWriter() :
  AbstractFilter(),
  m_FieldDataFile(""),
  m_WriteNeighborListData(false)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FieldDataCSVWriter::~FieldDataCSVWriter()
{
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FieldDataCSVWriter::setupFilterParameters()
{
  std::vector<FilterParameter::Pointer> parameters;
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Output File");
    option->setPropertyName("FieldDataFile");
    option->setWidgetType(FilterParameter::OutputFileWidget);
    option->setFileExtension("*.csv");
    option->setFileType("Comma Separated Data");
    option->setValueType("string");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Write Neighbor Data");
    option->setPropertyName("WriteNeighborListData");
    option->setWidgetType(FilterParameter::BooleanWidget);
    option->setValueType("bool");
    parameters.push_back(option);
  }
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FieldDataCSVWriter::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  /* Code to read the values goes between these statements */
/* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE BEGIN*/
  setFieldDataFile( reader->readValue( "FieldDataFile", getFieldDataFile() ) );
  setWriteNeighborListData( reader->readValue("WriteNeighborListData", getWriteNeighborListData()) );
/* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE END*/
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int FieldDataCSVWriter::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("FieldDataFile", getFieldDataFile() );
  writer->writeValue("WriteNeighborData", getWriteNeighborListData() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FieldDataCSVWriter::preflight()
{
  setErrorCondition(0);
  std::stringstream ss;

  if (getFieldDataFile().empty() == true)
  {
    ss <<  ": The output file must be set before executing this filter.";
    addErrorMessage(getHumanLabel(), ss.str(), -1);
    setErrorCondition(-1);
  }

  std::string parentPath = MXAFileInfo::parentPath(getFieldDataFile());
  if (MXADir::exists(parentPath) == false)
  {
    ss.str("");
    ss <<  "The directory path for the output file does not exist.";
    addWarningMessage(getHumanLabel(), ss.str(), -1);
  }

  if (MXAFileInfo::extension(getFieldDataFile()).compare("") == 0)
  {
    setFieldDataFile(getFieldDataFile().append(".dx"));
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FieldDataCSVWriter::execute()
{
  int err = 0;
  setErrorCondition(err);
  VolumeDataContainer* m = getVolumeDataContainer();
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The DataContainer Object was NULL", -999);
    return;
  }

  // Make sure any directory path is also available as the user may have just typed
  // in a path without actually creating the full path
  std::string parentPath = MXAFileInfo::parentPath(m_FieldDataFile);
  if(!MXADir::mkdir(parentPath, true))
  {
    std::stringstream ss;
    ss << "Error creating parent path '" << parentPath << "'";
    notifyErrorMessage(ss.str(), -1);
    setErrorCondition(-1);
    return;
  }



  std::string filename = getFieldDataFile();

  std::ofstream outFile;
  outFile.open(filename.c_str(), std::ios_base::binary);
  char space = DREAM3D::GrainData::Delimiter;
  // Write the total number of grains
  outFile << m->getNumFieldTuples()-1 << std::endl;
  // Get all the names of the arrays from the Data Container
  std::list<std::string> headers = m->getFieldArrayNameList();

  std::vector<IDataArray::Pointer> data;

  //For checking if an array is a neighborlist
  NeighborList<int>::Pointer neighborlistPtr = NeighborList<int>::New();

  // Print the GrainIds Header before the rest of the headers
  outFile << DREAM3D::GrainData::GrainID;
  // Loop throught the list and print the rest of the headers, ignoring those we don't want
  for(std::list<std::string>::iterator iter = headers.begin(); iter != headers.end(); ++iter)
  {
    // Only get the array if the name does NOT match those listed
    IDataArray::Pointer p = m->getFieldData(*iter);
    if(p->getNameOfClass().compare(neighborlistPtr->getNameOfClass()) != 0)
    {
      if (p->GetNumberOfComponents() == 1) {
        outFile << space << (*iter);
      }
      else // There are more than a single component so we need to add multiple header values
      {
        for(int k = 0; k < p->GetNumberOfComponents(); ++k)
        {
          outFile << space << (*iter) << "_" << k;
        }
      }
      // Get the IDataArray from the DataContainer
      data.push_back(p);
    }
  }
  outFile << std::endl;

  // Get the number of tuples in the arrays
  size_t numTuples = data[0]->GetNumberOfTuples();
  std::stringstream ss;
  float threshold = 0.0f;

  // Skip the first grain
  for(size_t i = 1; i < numTuples; ++i)
  {
    if (((float)i / numTuples) * 100.0f > threshold) {
      ss.str("");
      ss << "Writing Field Data - " << ((float)i / numTuples) * 100 << "% Complete";
      notifyStatusMessage(ss.str());
      threshold = threshold + 5.0f;
      if (threshold < ((float)i / numTuples) * 100.0f) {
        threshold = ((float)i / numTuples) * 100.0f;
      }
    }

    // Print the grain id
    outFile << i;
    // Print a row of data
    for( std::vector<IDataArray::Pointer>::iterator p = data.begin(); p != data.end(); ++p)
    {
      outFile << space;
      (*p)->printTuple(outFile, i, space);
    }
    outFile << std::endl;
  }

  if(m_WriteNeighborListData == true)
  {
    // Print the GrainIds Header before the rest of the headers
    // Loop throught the list and print the rest of the headers, ignoring those we don't want
    for(std::list<std::string>::iterator iter = headers.begin(); iter != headers.end(); ++iter)
    {
      // Only get the array if the name does NOT match those listed
      IDataArray::Pointer p = m->getFieldData(*iter);
      if(p->getNameOfClass().compare(neighborlistPtr->getNameOfClass()) == 0)
      {
        outFile << DREAM3D::GrainData::GrainID << space << DREAM3D::GrainData::NumNeighbors << space << (*iter) << std::endl;
        size_t numTuples = p->GetNumberOfTuples();
        //	  float threshold = 0.0f;

        // Skip the first grain
        for(size_t i = 1; i < numTuples; ++i)
        {
          // Print the grain id
          outFile << i;
          // Print a row of data
          outFile << space;
          p->printTuple(outFile, i, space);
          outFile << std::endl;
        }
      }
    }
  }
  outFile.close();

  // If there is an error set this to something negative and also set a message
  notifyStatusMessage("FieldDataCSVWriter Completed");

}







