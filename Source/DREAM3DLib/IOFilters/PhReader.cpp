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


#include "PhReader.h"

#include <stdio.h>

#include <iostream>
#include <fstream>
#include <sstream>

#include "MXA/Utilities/MXAFileInfo.h"

#include "DREAM3DLib/DataArrays/DataArray.hpp"

#define BUF_SIZE 1024

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PhReader::PhReader() :
FileReader(),
m_InputFile(""),
m_GrainIdsArrayName(DREAM3D::CellData::GrainIds),
m_GrainIds(NULL),
m_InStream(NULL)

{
  m_Origin.x = 0.0;
  m_Origin.y = 0.0;
  m_Origin.z = 0.0;

  m_Resolution.x = 1.0;
  m_Resolution.y = 1.0;
  m_Resolution.z = 1.0;

  m_Dims[0] = 0; m_Dims[1] = 0; m_Dims[2] = 0;
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PhReader::~PhReader()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PhReader::setupFilterParameters()
{
  std::vector<FilterParameter::Pointer> parameters;
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Input File");
    option->setPropertyName("InputFile");
    option->setWidgetType(FilterParameter::InputFileWidget);
    option->setFileExtension("*.ph");
    option->setFileType("CMU Grain Growth");
    option->setValueType("string");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Origin");
    option->setPropertyName("Origin");
    option->setWidgetType(FilterParameter::FloatVec3Widget);
    option->setValueType("FloatVec3Widget_t");
    option->setUnits("XYZ");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Resolution");
    option->setPropertyName("Resolution");
    option->setWidgetType(FilterParameter::FloatVec3Widget);
    option->setValueType("FloatVec3Widget_t");
    option->setUnits("XYZ");
    parameters.push_back(option);
  }
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PhReader::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  /* Code to read the values goes between these statements */
/* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE BEGIN*/
  setInputFile( reader->readValue( "InputFile", getInputFile() ) );
  setOrigin( reader->readValue("Origin", getOrigin() ) );
  setResolution( reader->readValue("Resolution", getResolution() ) );
/* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE END*/
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int PhReader::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("InputFile", getInputFile() );
  writer->writeValue("Origin", getOrigin() );
  writer->writeValue("Resolution", getResolution() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PhReader::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{

  setErrorCondition(0);
  std::stringstream ss;
  VoxelDataContainer* m = getVoxelDataContainer();

  if (getInputFile().empty() == true)
  {
    ss << ClassName() << " needs the Input File Set and it was not.";
    setErrorCondition(-387);
    addErrorMessage(getHumanLabel(), ss.str(), getErrorCondition());
  }
  else if (MXAFileInfo::exists(getInputFile()) == false)
  {
    ss << "The input file does not exist.";
    setErrorCondition(-388);
    addErrorMessage(getHumanLabel(), ss.str(), getErrorCondition());
  }

  CREATE_NON_PREREQ_DATA(m, DREAM3D, CellData, GrainIds, ss, int32_t, Int32ArrayType, 0, voxels, 1)

  m->setResolution(m_Resolution.x, m_Resolution.y, m_Resolution.z);
  m->setOrigin(m_Origin.x, m_Origin.y, m_Origin.z);

// We need to read the header of the input file to get the dimensions
  m_InStream = fopen(getInputFile().c_str(), "r");
  if(m_InStream == NULL)
  {
    setErrorCondition(-48802);
    notifyErrorMessage("Error opening input file", getErrorCondition());
    return;
  }


  int error = readHeader();
  fclose(m_InStream);
  m_InStream = NULL;
  if (error < 0)
  {
    setErrorCondition(error);
    ss.clear();
    ss << "Error occurred trying to parse the dimensions from the input file. Is the input file a Ph file?";
    addErrorMessage(getHumanLabel(), ss.str(), -48010);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PhReader::preflight()
{
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PhReader::execute()
{
  /* DO NOT CALL THE DATACHECK() from this method. You will end up in an endless loop.
   * The array will get allocated down in the 'readFile()' method.
   */
  if (NULL == getVoxelDataContainer())
  {
    std::stringstream ss;
    ss << "DataContainer Pointer was NULL and Must be valid." << __FILE__ << "("<<__LINE__<<")";
    setErrorCondition(-48020);
    addErrorMessage(getHumanLabel(), ss.str(), getErrorCondition());
    return;
  }

  std::stringstream ss;
  int err = 0;

  m_InStream = fopen(getInputFile().c_str(), "r");
  if(m_InStream == NULL)
  {
    setErrorCondition(-48030);
    notifyErrorMessage("Error opening input file", getErrorCondition());
    return;
  }


  err = readHeader();
  if(err < 0)
  {
  fclose(m_InStream);
  m_InStream = NULL;
    return;
  }
  err = readFile();
  fclose(m_InStream);
  m_InStream = NULL;
  if(err < 0)
  {
    return;
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int PhReader::readHeader()
{
  VoxelDataContainer* m = getVoxelDataContainer();

  int nx = 0;
  int ny = 0;
  int nz = 0;


  // Read Line #1 which has the dimensions
  fscanf(m_InStream, "%d %d %d\n", &nx, &ny, &nz);
  m->setDimensions(nx, ny, nz);

  char buf[BUF_SIZE];
  // Read Line #2 and dump it
  ::memset(buf, 0, BUF_SIZE);
  fgets(buf, BUF_SIZE, m_InStream);
  // Read Line #3 and dump it
  ::memset(buf, 0, BUF_SIZE);
  fgets(buf, BUF_SIZE, m_InStream);
  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int  PhReader::readFile()
{

  VoxelDataContainer* m = getVoxelDataContainer();

  size_t totalPoints = m->getTotalPoints();
  Int32ArrayType::Pointer m_GrainIdData = Int32ArrayType::CreateArray(totalPoints, m_GrainIdsArrayName);
  m_GrainIdData->initializeWithValues(-1);
  int32_t* grainIds = m_GrainIdData->GetPointer(0);
  for(size_t n = 0; n < totalPoints; ++n)
  {
    if (fscanf(m_InStream, "%d", grainIds+n) == 0)
    {
      fclose(m_InStream);
      m_InStream = NULL;
      setErrorCondition(-48040);
      notifyErrorMessage("Error reading Ph data", getErrorCondition());
      return getErrorCondition();
    }
  }

  // Read the data and stick it in the data Container
  getVoxelDataContainer()->addCellData(DREAM3D::CellData::GrainIds, m_GrainIdData);

  // Now set the Resolution and Origin that the user provided on the GUI or as parameters
  m->setResolution(m_Resolution.x, m_Resolution.y, m_Resolution.z);
  m->setOrigin(m_Origin.x, m_Origin.y, m_Origin.z);

  notifyStatusMessage("Complete");
  return 0;
}
