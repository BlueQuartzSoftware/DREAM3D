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
#include "DxReader.h"

#include <iostream>
#include <fstream>

#include "MXA/Utilities/MXAFileInfo.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DxReader::DxReader() :
FileReader(),
m_InputFile(""),
m_GrainIdsArrayName(DREAM3D::CellData::GrainIds),
m_GrainIds(NULL)
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
DxReader::~DxReader()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DxReader::setupFilterParameters()
{
  std::vector<FilterParameter::Pointer> parameters;
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Input File");
    option->setPropertyName("InputFile");
    option->setWidgetType(FilterParameter::InputFileWidget);
    option->setFileExtension("*.dx");
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
void DxReader::readFilterParameters(AbstractFilterParametersReader* reader)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DxReader::writeFilterParameters(AbstractFilterParametersWriter* writer)
{
  writer->writeValue("InputFile", getInputFile() );
  writer->writeValue("Origin", getOrigin() );
  writer->writeValue("Resolution", getResolution() );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DxReader::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
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

  if (m_InStream.is_open() == true)
  {
    m_InStream.close();
  }
// We need to read the header of the input file to get the dimensions
  m_InStream.open(getInputFile().c_str(), std::ios_base::binary);
  if(!m_InStream)
  {
    ss.clear();
    ss << " Runtime Error. The input file '" << getInputFile() << "' could not be"
        << " opened for reading. Do you have access to this file?";
    setErrorCondition(-49800);
    addErrorMessage(getHumanLabel(), ss.str(), getErrorCondition());
    return;
  }

  int error = readHeader();
  m_InStream.close();
  if (error < 0)
  {
    setErrorCondition(error);
    ss.clear();
    ss << "Error occurred trying to parse the dimensions from the input file. Is the input file a Dx file?";
    addErrorMessage(getHumanLabel(), ss.str(), -11000);
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DxReader::preflight()
{
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DxReader::execute()
{
  std::stringstream ss;
  int err = 0;

  m_InStream.open(getInputFile().c_str(), std::ios_base::binary);
  if(!m_InStream)
  {
    ss.clear();
    ss << " Runtime Error. The input file '" << getInputFile() << "' could not be"
        << " opened for reading. Do you have access to this file?";
    setErrorCondition(-49801);
    addErrorMessage(getHumanLabel(), ss.str(), getErrorCondition());
    return;
  }

  err = readHeader();
  if(err < 0)
  {
    m_InStream.close();
    return;
  }
  err = readFile();
  m_InStream.close();
  if(err < 0)
  {
    return;
  }

}


//-----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int DxReader::readHeader()
{
  VoxelDataContainer* m = getVoxelDataContainer();
  std::stringstream ss;
  int error = 0;

  std::string line;
  std::string delimeters(", ;\t"); /* delimeters to split the data */
  std::vector<std::string> tokens; /* vector to store the split data */

  getline(m_InStream, line, '\n');
  tokenize(line, tokens, delimeters);

  // Process the header information and look for the std::string "counts"
  // Then read the data size after that
  size_t pos1 = 0;
  while (pos1 == 0)
  { // continue until we find the keyword
    for (size_t i = 0; i < tokens.size(); i++)
    {
      if(tokens[i] == "counts")
      {
        pos1 = i;
      }
    }
    // Read the next line of the header if we did not find the keyword
    // in the line
    if(pos1 == 0)
    {
      tokens.clear();
      getline(m_InStream, line, '\n');
      tokenize(line, tokens, delimeters);
      if(tokens.size() == 20)
      {
        ss.clear();
        ss << "ERROR: Unable to read data dimensions from the header" << std::endl;
        addErrorMessage(getHumanLabel(), ss.str(), -7);
        setErrorCondition(-499);
        m_InStream.close();
        return -499;
      }
    }
  }

  int nx = 0;
  int ny = 0;
  int nz = 0;

  if(pos1 != 0)
  {
    error = 0;
    error += sscanf(tokens[pos1 + 1].c_str(), "%d", &nz);
    error += sscanf(tokens[pos1 + 2].c_str(), "%d", &ny);
    error += sscanf(tokens[pos1 + 3].c_str(), "%d", &nx);
    tokens.clear();
    // The dimensions listed in the DX file are always one greater
    // than the actual dimensions
    nx--;
    ny--;
    nz--;
  }

//  std::cout << "INFO: DX data dimensions: " << std::endl;
//  std::cout << "nz= " << nz << std::endl;
//  std::cout << "ny= " << ny << std::endl;
//  std::cout << "nx= " << nx << std::endl;

  //The DX file has a unique format of 20 entries on each line. I have
  //no idea who initiated this insanity but I am about to perpetuate
  //it.
  //
  //The most simple thing to do is to read the entire dataset into one
  //long vector and then read that vector to assign values to the grid

  //  ADR:  6 Sep 08; time to make the input much more general!
  //  equivalent to list-direcvted input in Fortran, actually !!

  pos1 = 0;
  while (pos1 == 0)
  { // continue until we find the keyword
    for (size_t i = 0; i < tokens.size(); i++)
    {
      if(tokens[i] == "items")
      {
        pos1 = i;
      }
    }
    // Read the next line of the header if we did not find the keyword
    // in the line
    if(pos1 == 0)
    {
      tokens.clear();
      getline(m_InStream, line, '\n');
      tokenize(line, tokens, delimeters);
      if(tokens.size() == 20)
      {
        ss.clear();
        ss << "ERROR: Unable to locate the last header line" << std::endl;
        addErrorMessage(getHumanLabel(), ss.str(), -8);
        setErrorCondition(-496);
        m_InStream.close();
        return -496;
      }
    }
  } // when we get here, we are looking at data
  int points = 0;
  if(pos1 != 0)
  {
    error = 0;
    error += sscanf(tokens[pos1 + 1].c_str(), "%d", &points);
    tokens.clear();
  }
  m->setDimensions(nx, ny, nz);
//  std::cout << "Compare no. points " << points << " with x*y*z: " << nx * ny * nz << std::endl;
  return error;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int DxReader::readFile()
{
  std::stringstream ss;
  VoxelDataContainer* m = getVoxelDataContainer();
  if (NULL == m)
  {
    ss.clear();
    ss << "DataContainer Pointer was NULL and Must be valid." << __FILE__ << "("<<__LINE__<<")";
    addErrorMessage(getHumanLabel(), ss.str(), -5);
    setErrorCondition(-5);
    return -1;
  }

  std::string line;
  std::string delimeters(", ;\t"); /* delimeters to split the data */
  std::vector<std::string> tokens; /* vector to store the split data */

  int error, spin; /* dummy variables */

  bool finished_header, finished_data;
  finished_header = true;
  finished_data = false;
  size_t index = 0;

  size_t totalPoints = m->getTotalPoints();

  // Remove the array that we are about to create first as a 'datacheck()' was called from the super class's 'execute'
  // method which is performed before this function. This will cause an error -501 because the array with the name
  // m_GrainIdsArrayName already exists but of size 1, not the size we are going to read. So we get rid of the array
  m->removeCellData(m_GrainIdsArrayName);
  // Rerun the data check in order to allocate the array to store the data from the .dx file.
//  dataCheck(false, totalPoints, m->getNumFieldTuples(), m->getNumEnsembleTuples());
  CREATE_NON_PREREQ_DATA(m, DREAM3D, CellData, GrainIds, ss, int32_t, Int32ArrayType, 0, totalPoints, 1)


  if (getErrorCondition() < 0)
  {
    m_InStream.close();
    return -1;
  }


  while (getline(m_InStream, line, '\n') != NULL)
  {

    // Get the remaining lines of the header and ignore
    tokens.clear();
    error = 0;
    tokenize(line, tokens, delimeters);

    size_t total = m->getTotalPoints();
    if( index == total || ( finished_header && tokens.size() != 0 && tokens[0] == "attribute") )
    {
      finished_data = true;
    }

    // Allocate the DataArray at this point:
    if(finished_header && !finished_data)
    {
      for (size_t in_spins = 0; in_spins < tokens.size(); in_spins++)
      {
        error += sscanf(tokens[in_spins].c_str(), "%d", &spin);
        m_GrainIds[index] =  spin;
        ++index;
      }
    }
  }

  if(index != static_cast<size_t>(m->getTotalPoints()))
  {
    ss.clear();
    ss << "ERROR: data size does not match header dimensions" << std::endl;
    ss << "\t" << index << "\t" << m->getTotalPoints() << std::endl;
    setErrorCondition(-495);
    addErrorMessage(getHumanLabel(), ss.str(), getErrorCondition());
    m_InStream.close();
    return getErrorCondition();
  }

  tokens.clear();
  m_InStream.close();


  // Find the unique set of grain ids
//  std::set<int32_t> grainIdSet;
//  for (int64_t i = 0; i < totalPoints; ++i)
//  {
//    grainIdSet.insert(m_GrainIds[i]);
//  }
//  for (std::set<int32_t>::iterator iter = grainIdSet.begin(); iter != grainIdSet.end(); ++iter )
//  {
//    std::cout << "Grain ID: " << (*iter) << std::endl;
//  }

  notifyStatusMessage("Complete");
  return 0;
}

