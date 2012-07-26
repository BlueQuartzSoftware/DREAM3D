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

#include <iostream>
#include <fstream>
#include <sstream>

#include "DREAM3DLib/Common/DataArray.hpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PhReader::PhReader() :
FileReader(),
m_GrainIdsArrayName(DREAM3D::CellData::GrainIds),
m_CellPhasesArrayName(DREAM3D::CellData::Phases),
m_CellEulerAnglesArrayName(DREAM3D::CellData::EulerAngles),
m_FieldPhasesArrayName(DREAM3D::FieldData::Phases),
m_FieldEulerAnglesArrayName(DREAM3D::FieldData::EulerAngles),
m_GrainIds(NULL),
m_CellPhases(NULL),
m_CellEulerAngles(NULL),
m_FieldPhases(NULL),
m_FieldEulerAngles(NULL),
m_InputInfoFile(""),
m_XRes(1.0f),
m_YRes(1.0f),
m_ZRes(1.0f)
{
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
    option->setHumanLabel("Input PH File");
    option->setPropertyName("InputFile");
    option->setWidgetType(FilterParameter::InputFileWidget);
    option->setValueType("string");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Input Grain Info File");
    option->setPropertyName("InputInfoFile");
    option->setWidgetType(FilterParameter::InputFileWidget);
    option->setValueType("string");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("X Res");
    option->setPropertyName("XRes");
    option->setWidgetType(FilterParameter::DoubleWidget);
    option->setValueType("float");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Y Res");
    option->setPropertyName("YRes");
    option->setWidgetType(FilterParameter::DoubleWidget);
    option->setValueType("float");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Z Res");
    option->setPropertyName("ZRes");
    option->setWidgetType(FilterParameter::DoubleWidget);
    option->setValueType("float");
    parameters.push_back(option);
  }
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PhReader::writeFilterParameters(AbstractFilterParametersWriter* writer)
{
  writer->writeValue("InputFile", getInputFile() );
  writer->writeValue("InputInfoFile", getInputInfoFile() );
  writer->writeValue("XRes", getXRes() );
  writer->writeValue("YRes", getYRes() );
  writer->writeValue("ZRes", getZRes() );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PhReader::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{

  setErrorCondition(0);
  std::stringstream ss;
  DataContainer* m = getDataContainer();

  if (getInputFile().empty() == true)
  {
    std::stringstream ss;
    ss << ClassName() << " needs the Input File Set and it was not.";
    addErrorMessage(getHumanLabel(), ss.str(), -4);
    setErrorCondition(-387);
  }
  if (getInputInfoFile().empty() == true)
  {
    std::stringstream ss;
    ss << ClassName() << " needs the Input Grain Info File Set and it was not.";
    addErrorMessage(getHumanLabel(), ss.str(), -4);
    setErrorCondition(-387);
  }
  CREATE_NON_PREREQ_DATA(m, DREAM3D, CellData, GrainIds, ss, int32_t, Int32ArrayType, 0, voxels, 1)
  CREATE_NON_PREREQ_DATA(m, DREAM3D, CellData, CellEulerAngles, ss, float, FloatArrayType, 0, voxels, 1)
  CREATE_NON_PREREQ_DATA(m, DREAM3D, CellData, CellPhases, ss, int32_t, Int32ArrayType, 0, voxels, 1)

  CREATE_NON_PREREQ_DATA(m, DREAM3D, FieldData, FieldPhases, ss, int32_t, Int32ArrayType, 0, fields, 1)
  CREATE_NON_PREREQ_DATA(m, DREAM3D, FieldData, FieldEulerAngles, ss, float, FloatArrayType, 0, fields, 1)

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
int PhReader::readHeader()
{
  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int  PhReader::readFile()
{

  if (NULL == getDataContainer())
  {
    std::stringstream ss;
    ss << "DataContainer Pointer was NULL and Must be valid." << __FILE__ << "("<<__LINE__<<")";
    addErrorMessage(getHumanLabel(), ss.str(), -1);
    setErrorCondition(-1);
    return -1;
  }

  std::string line;
  std::string delimeters(", ;\t"); /* delimeters to split the data */
  std::vector<std::string> tokens; /* vector to store the split data */

  int error, spin; /* dummy variables */
  int nx = 0;
  int ny = 0;
  int nz = 0;

  std::ifstream inFile;
  inFile.open(getInputFile().c_str(), std::ios_base::binary);
  if(!inFile)
  {
    std::stringstream ss;
    ss << "Failed to open: " << getInputFile();
    setErrorCondition(-1);
    addErrorMessage(getHumanLabel(), ss.str(), -1);
    return -1;
  }

  getline(inFile, line, '\n');
  tokenize(line, tokens, delimeters);

  // Process the header information from the PH file.
  error = 0;
  error += sscanf(tokens[0].c_str(), "%d", &nx);
  error += sscanf(tokens[1].c_str(), "%d", &ny);
  error += sscanf(tokens[2].c_str(), "%d", &nz);
  tokens.clear();

  //  cout << "INFO: PH file grid size: " << nx << "\t" << ny << "\t" << nz << endl;;

  // Get the remaining two lines of the header and ignore
  getline(inFile, line, '\n');
  getline(inFile, line, '\n');

  //The PH file has a unique format of 20 entries on each line. I have
  //now idea who initiated this insanity but I am about to propetuate
  //it.
  //
  //The most simple thing todo is to read the entire dataset into one
  //long vector and then read that vector to assign values to the grid
  size_t index = 0;
  Int32ArrayType::Pointer m_GrainIdData = Int32ArrayType::CreateArray(nx * ny * nz, DREAM3D::CellData::GrainIds);

  while (getline(inFile, line, '\n') != NULL)
  {
    tokens.clear();
    error = 0;
    tokenize(line, tokens, delimeters);
    //        cout << line << endl;
    //        for(int i=0; i < tokens.size(); i++ )
    //              cout << setw(6) << tokens[i];
    //        cout << endl;

    for (size_t in_spins = 0; in_spins < tokens.size(); in_spins++)
    {
      error += sscanf(tokens[in_spins].c_str(), "%d", &spin);
      m_GrainIdData->SetValue(index, spin);
      ++index;
    }
    //        if(error != 20)
    //              {
    //                cout << "ERROR: Invalid number of line entries in PH file" << endl;
    //              }
  }

  if(index != static_cast<size_t>(nz * ny * nx))
  {
    std::stringstream ss;
    ss << "ERROR: data size does not match header dimensions. ";
    ss << "\t" << index << "\t" << nz * nx * ny;
    setErrorCondition(-1);
    addErrorMessage(getHumanLabel(), ss.str(), 1);
    return -1;
    inFile.close();
  }

  // Read the data and stick it in the data Container
  getDataContainer()->addCellData(DREAM3D::CellData::GrainIds, m_GrainIdData);
  getDataContainer()->setDimensions(nx, ny, nz);
  getDataContainer()->setResolution(m_XRes, m_YRes, m_ZRes);
  getDataContainer()->setOrigin(0.0f, 0.0f, 0.0f);

  tokens.clear();
  inFile.close();

  std::ifstream inFile2;
  inFile2.open(getInputInfoFile().c_str(), std::ios_base::binary);
  if(!inFile2)
  {
    std::stringstream ss;
    ss << "Failed to open: " << getInputInfoFile();
    setErrorCondition(-1);
    addErrorMessage(getHumanLabel(), ss.str(), -1);
    return -1;
  }
  int numgrains;
  int gnum, phase;
  float ea1, ea2, ea3;
  inFile2 >> numgrains;
  FloatArrayType::Pointer m_FieldEulerData = FloatArrayType::CreateArray(3*numgrains, DREAM3D::FieldData::EulerAngles);
  Int32ArrayType::Pointer m_FieldPhaseData = Int32ArrayType::CreateArray(numgrains, DREAM3D::FieldData::Phases);
  for(int i=0;i<numgrains;i++)
  {
	inFile2 >> gnum >> phase >> ea1 >> ea2 >> ea3;
	m_FieldEulerData->SetValue(3*gnum, ea1);
	m_FieldEulerData->SetValue(3*gnum+1, ea2);
	m_FieldEulerData->SetValue(3*gnum+2, ea3);
	m_FieldPhaseData->SetValue(gnum, phase);
  }
  getDataContainer()->addFieldData(DREAM3D::FieldData::EulerAngles, m_FieldEulerData);
  getDataContainer()->addFieldData(DREAM3D::FieldData::Phases, m_FieldPhaseData);

  FloatArrayType::Pointer m_CellEulerData = FloatArrayType::CreateArray(3*nx*ny*nz, DREAM3D::FieldData::EulerAngles);
  Int32ArrayType::Pointer m_CellPhaseData = Int32ArrayType::CreateArray(nx*ny*nz, DREAM3D::FieldData::Phases);
  for(int i=0;i<nx*ny*nz;i++)
  {
	gnum = m_GrainIdData->GetValue(i);    
	m_CellEulerData->SetValue(3*i, m_FieldEulerData->GetValue(3*gnum));
	m_CellEulerData->SetValue(3*i+1, m_FieldEulerData->GetValue(3*gnum+1));
	m_CellEulerData->SetValue(3*i+2, m_FieldEulerData->GetValue(3*gnum+2));
	m_CellPhaseData->SetValue(i, m_FieldPhaseData->GetValue(gnum));
  }
  getDataContainer()->addCellData(DREAM3D::CellData::EulerAngles, m_CellEulerData);
  getDataContainer()->addCellData(DREAM3D::CellData::Phases, m_CellPhaseData);

  DataContainer* m = getDataContainer();

  notifyStatusMessage("Complete");
  return 0;
}
