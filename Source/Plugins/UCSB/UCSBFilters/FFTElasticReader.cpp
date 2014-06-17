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
* This code was written under United States Air Force Contract number
* FA8650-07-D-5800
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */


#include "FFTElasticReader.h"

#include <iostream>
#include <cstring>

#include "MXA/Utilities/MXAFileInfo.h"
#include "DREAM3DLib/Math/DREAM3DMath.h"

#define ELASTIC_FIELD_COUNT 20

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FFTElasticReader::FFTElasticReader() :
  AbstractFilter(),
  m_FieldsFile(""),
  m_RStatsFile(""),
  m_Stress11ArrayName(DREAM3D::CellData::Stress11),
  m_Stress22ArrayName(DREAM3D::CellData::Stress22),
  m_Stress33ArrayName(DREAM3D::CellData::Stress33),
  m_Stress23ArrayName(DREAM3D::CellData::Stress23),
  m_Stress31ArrayName(DREAM3D::CellData::Stress31),
  m_Stress12ArrayName(DREAM3D::CellData::Stress12),
  m_Strain11ArrayName(DREAM3D::CellData::Strain11),
  m_Strain22ArrayName(DREAM3D::CellData::Strain22),
  m_Strain33ArrayName(DREAM3D::CellData::Strain33),
  m_Strain23ArrayName(DREAM3D::CellData::Strain23),
  m_Strain31ArrayName(DREAM3D::CellData::Strain31),
  m_Strain12ArrayName(DREAM3D::CellData::Strain12),
  m_StressArrayName(DREAM3D::CellData::Stress),
  m_StrainArrayName(DREAM3D::CellData::Strain),
  m_EEDArrayName(DREAM3D::CellData::EED),
  m_MaxPrincipalStressArrayName(DREAM3D::CellData::MaxPrincipalStress),
  m_MinPrincipalStressArrayName(DREAM3D::CellData::MinPrincipalStress),
  m_Stress11(NULL),
  m_Stress22(NULL),
  m_Stress33(NULL),
  m_Stress23(NULL),
  m_Stress31(NULL),
  m_Stress12(NULL),
  m_Strain11(NULL),
  m_Strain22(NULL),
  m_Strain33(NULL),
  m_Strain23(NULL),
  m_Strain31(NULL),
  m_Strain12(NULL),
  m_Stress(NULL),
  m_Strain(NULL),
  m_EED(NULL),
  m_MaxPrincipalStress(NULL),
  m_MinPrincipalStress(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FFTElasticReader::~FFTElasticReader()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FFTElasticReader::setupFilterParameters()
{
  std::vector<FilterParameter::Pointer> parameters;
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Fields File");
    option->setPropertyName("FieldsFile");
    option->setWidgetType(FilterParameter::InputFileWidget);
    option->setValueType("string");
    option->setFileExtension("*.out");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("R-Stats File");
    option->setPropertyName("RStatsFile");
    option->setWidgetType(FilterParameter::InputFileWidget);
    option->setValueType("string");
    option->setFileExtension("*.txt");
    parameters.push_back(option);
  }
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FFTElasticReader::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  /* Code to read the values goes between these statements */
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE BEGIN*/
  setFieldsFile( reader->readValue( "FieldsFile", getFieldsFile()) );
  setRStatsFile( reader->readValue( "RStatsFile", getRStatsFile()) );
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE END*/
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int FFTElasticReader::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("FieldsFile", getFieldsFile() );
  writer->writeValue("RStatsFile", getRStatsFile() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FFTElasticReader::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{

  setErrorCondition(0);
  std::stringstream ss;
  VoxelDataContainer* m = getVoxelDataContainer();

  if (getFieldsFile().empty() == true)
  {
    ss << ClassName() << " needs the Fields File Set and it was not.";
    setErrorCondition(-387);
    addErrorMessage(getHumanLabel(), ss.str(), getErrorCondition());
  }
  else if (MXAFileInfo::exists(getFieldsFile()) == false)
  {
    ss << "Fields input file does not exist.";
    setErrorCondition(-388);
    addErrorMessage(getHumanLabel(), ss.str(), getErrorCondition());
  }

  if (getRStatsFile().empty() == true)
  {
    ss << ClassName() << " needs the R-Stats File Set and it was not.";
    setErrorCondition(-387);
    addErrorMessage(getHumanLabel(), ss.str(), getErrorCondition());
  }
  else if (MXAFileInfo::exists(getRStatsFile()) == false)
  {
    ss << "The input file does not exist.";
    setErrorCondition(-388);
    addErrorMessage(getHumanLabel(), ss.str(), getErrorCondition());
  }

  CREATE_NON_PREREQ_DATA(m, DREAM3D, CellData, Stress11, ss, float, FloatArrayType, 0, voxels, 1)
  CREATE_NON_PREREQ_DATA(m, DREAM3D, CellData, Stress22, ss, float, FloatArrayType, 0, voxels, 1)
  CREATE_NON_PREREQ_DATA(m, DREAM3D, CellData, Stress33, ss, float, FloatArrayType, 0, voxels, 1)
  CREATE_NON_PREREQ_DATA(m, DREAM3D, CellData, Stress23, ss, float, FloatArrayType, 0, voxels, 1)
  CREATE_NON_PREREQ_DATA(m, DREAM3D, CellData, Stress31, ss, float, FloatArrayType, 0, voxels, 1)
  CREATE_NON_PREREQ_DATA(m, DREAM3D, CellData, Stress12, ss, float, FloatArrayType, 0, voxels, 1)
  CREATE_NON_PREREQ_DATA(m, DREAM3D, CellData, Strain11, ss, float, FloatArrayType, 0, voxels, 1)
  CREATE_NON_PREREQ_DATA(m, DREAM3D, CellData, Strain22, ss, float, FloatArrayType, 0, voxels, 1)
  CREATE_NON_PREREQ_DATA(m, DREAM3D, CellData, Strain33, ss, float, FloatArrayType, 0, voxels, 1)
  CREATE_NON_PREREQ_DATA(m, DREAM3D, CellData, Strain23, ss, float, FloatArrayType, 0, voxels, 1)
  CREATE_NON_PREREQ_DATA(m, DREAM3D, CellData, Strain31, ss, float, FloatArrayType, 0, voxels, 1)
  CREATE_NON_PREREQ_DATA(m, DREAM3D, CellData, Strain12, ss, float, FloatArrayType, 0, voxels, 1)
  CREATE_NON_PREREQ_DATA(m, DREAM3D, CellData, Stress, ss, float, FloatArrayType, 0, voxels, 1)
  CREATE_NON_PREREQ_DATA(m, DREAM3D, CellData, Strain, ss, float, FloatArrayType, 0, voxels, 1)
  CREATE_NON_PREREQ_DATA(m, DREAM3D, CellData, EED, ss, float, FloatArrayType, 0, voxels, 1)
  CREATE_NON_PREREQ_DATA(m, DREAM3D, CellData, MaxPrincipalStress, ss, float, FloatArrayType, 0, voxels, 1)
  CREATE_NON_PREREQ_DATA(m, DREAM3D, CellData, MinPrincipalStress, ss, float, FloatArrayType, 0, voxels, 1)
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FFTElasticReader::preflight()
{
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FFTElasticReader::execute()
{
  int err = 0;
  setErrorCondition(err);
  VoxelDataContainer* m = getVoxelDataContainer();
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The DataContainer Object was NULL", getErrorCondition());
    return;
  }

  readFieldsFile();
  if (getErrorCondition() < 0)
  {
    return;
  }

  readRStatsFile();
    if (getErrorCondition() < 0)
  {
    return;
  }


// If there is an error set this to something negative and also set a message
  notifyStatusMessage("Complete");
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FFTElasticReader::readFieldsFile()
{
  VoxelDataContainer* m = getVoxelDataContainer();

  // Start reading the file so we can get the total number of voxels to read and get the sizes of our arrays

  const int kBufferSize=512;
  char buf[kBufferSize];
  std::stringstream ss;

  ///read fields file
  std::ifstream in(getFieldsFile().c_str());
  if (!in.is_open())
  {
    notifyErrorMessage("FFT Elastic Fields file could not be opened", -100);
    return;
  }
  int64_t fftside=0, fftpoints=0;
  bool endOfHeader=false;

  // For the Elastic Files there are 4 lines that are considered "header" lines. Lets just
  // read those 4 lines manually

  ::memset(buf, 0, kBufferSize);
  in.getline(buf, kBufferSize);
  int numFields = sscanf(buf, "%lld", &fftside);
  if(1 != numFields)
  {
    notifyErrorMessage("Error reading the FFT Side value from the Fields File", -102);
    return;
  }

  ::memset(buf, 0, kBufferSize);
  in.getline(buf, kBufferSize);
  numFields = sscanf(buf, "%lld", &fftpoints);
  if(1 != numFields)
  {
    notifyErrorMessage("Error reading the FFT Points value from the Fields File", -102);
    return;
  }

  // The next has the scaling of the voxel as a 3x3 array
  ::memset(buf, 0, kBufferSize);
  in.getline(buf, kBufferSize);
  float scaling[9];
  numFields = sscanf(buf, "%f %f %f %f %f %f %f %f %f", scaling, scaling+1, scaling+2, scaling+3, scaling+4, scaling+5, scaling+6, scaling+7, scaling+8);
  if(numFields != 9)
  {
    notifyErrorMessage("Error reading the FFT Tensor value from the Fields file. Aborting reading now", -101);
    return;
  }

  // Read and dump the Column Titles
  in.getline(buf, kBufferSize);
  endOfHeader=true;//2nd line after is last of header

  size_t volDims[3] = {fftside, fftside, fftside};
  m->setDimensions(volDims);
  float volScaling[3] = {scaling[0], scaling[4], scaling[8] };
  m->setResolution(volScaling);

  // Now tha the VoxelDataContainer is properly initialized with the sizes and resolutions, now run the dataCheck
  dataCheck(false, m->getTotalPoints(), m->getNumFieldTuples(), m->getNumEnsembleTuples());
  if (getErrorCondition() < 0)
  {
    return;
  }

  int64_t totalPoints = m->getTotalPoints();

  for (int64_t i=0; i<totalPoints; i++)
  {
    m_Stress11[i]=0.0f;
    m_Stress22[i]=0.0f;
    m_Stress33[i]=0.0f;
    m_Stress23[i]=0.0f;
    m_Stress31[i]=0.0f;
    m_Stress12[i]=0.0f;
    m_Strain11[i]=0.0f;
    m_Strain22[i]=0.0f;
    m_Strain33[i]=0.0f;
    m_Strain23[i]=0.0f;
    m_Strain31[i]=0.0f;
    m_Strain12[i]=0.0f;
    m_Stress[i]=0.0f;
    m_Strain[i]=0.0f;
    m_EED[i]=0.0f;
    m_MaxPrincipalStress[i]=0.0f;
    m_MinPrincipalStress[i]=0.0f;
  }

  if(totalPoints!=fftpoints)
  {
    ss.str("");
    ss << "Fields file " << getFieldsFile() << " contains " <<fftpoints<<" data points. Expected: "<<totalPoints;
    notifyErrorMessage(ss.str(), -200);
    return;
  }

  int64_t counter = 0;

  for(size_t i = 0; i < fftpoints; ++i)
  {
    if (in.eof() == true)
    {
      break;
    }
    ::memset(buf, 0, kBufferSize); // Clear the buffer
    in.getline(buf, kBufferSize);// Read the next line of data
    parseFieldsDataLine(buf, i);
    if(getErrorCondition() < 0) { break; }
    ++counter;
  }

  if (counter != fftpoints && in.eof() == true)
  {
    ss.str("");
    ss << "End of Fields file reached before all data was parsed. Expected Data Points: " << fftpoints
       << " Current Data Point Count: " << counter
       << std::endl;
    notifyErrorMessage(ss.str(), -600);
  }
  in.close();
}


// -----------------------------------------------------------------------------
// Read the data part of the fields.out file
// -----------------------------------------------------------------------------
void FFTElasticReader::parseFieldsDataLine(const std::string &line, size_t i)
{
  /* When reading the data there should be 20 cols of data.
* The column names should be the following:
* ?
* ?
* ?
* x
* y
* z
* phase?
* spin
* strain_11
* strain_22
* strain_33
* strain_23
* strain_31
* strain_12
* stress_11
* stress_22
* stress_33
* stress_23
* stress_31
* stress_12
*/
  float unk1=0.0f, unk2=0.0f, unk3=0.0f, e11=0.0f, e22=0.0f, e33=0.0f, e23=0.0f, e31=0.0f, e12=0.0f, s11=0.0f, s22=0.0f, s33=0.0f, s23=0.0f, s31=0.0f, s12=0.0f;
  int x=0, y=0, z=0, phase=0, spin=0;
  size_t offset = 0;
  size_t fieldsRead = 0;
  fieldsRead = sscanf(line.c_str(), "%f %f %f %i %i %i %i %i %f %f %f %f %f %f %f %f %f %f %f %f", &unk1, &unk2, &unk3, &x, &y, &z, &phase, &spin, &e11, &e22, &e33, &e23, &e31, &e12, &s11, &s22, &s33, &s23, &s31, &s12);
  if (ELASTIC_FIELD_COUNT != fieldsRead)
  {
    std::stringstream ss;
    ss << "Error reading proper number of columns in Field File at line " << (i+4);
    notifyErrorMessage(ss.str(), -120);
  }
  offset = i;

  m_Stress11[offset]=s11;
  m_Stress22[offset]=s22;
  m_Stress33[offset]=s33;
  m_Stress23[offset]=s23;
  m_Stress31[offset]=s31;
  m_Stress12[offset]=s12;
  m_Strain11[offset]=e11;
  m_Strain22[offset]=e22;
  m_Strain33[offset]=e33;
  m_Strain23[offset]=e23;
  m_Strain31[offset]=e31;
  m_Strain12[offset]=e12;

}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FFTElasticReader::readRStatsFile()
{
  VoxelDataContainer* m = getVoxelDataContainer();

  int64_t fftpoints = m->getTotalPoints();

   const int kBufferSize=512;
  char buf[kBufferSize];
  std::stringstream ss;

  ///read rstats file
  std::ifstream in2(getRStatsFile().c_str());
  if (!in2.is_open())
  {
    notifyErrorMessage("RStats file could not be opened", -100);
    return;
  }

  bool endOfHeader=false;

  ::memset(buf, 0, kBufferSize);
  in2.getline(buf, kBufferSize);
  endOfHeader=true;//only 1 header line


  size_t counter=0;
  for(size_t i = 0; i < fftpoints; ++i)
  {
    if (in2.eof() == true)
    {
      break;
    }
    ::memset(buf, 0, kBufferSize); // Clear the buffer
    in2.getline(buf, kBufferSize);// Read the next line of data
    parseRStatsDataLine(buf, i);
    ++counter;
  }

  if (counter != fftpoints && in2.eof() == true)
  {
    ss.str("");
    ss << "End of RStats file reached before all data was parsed. Expected Data Points: " << fftpoints
       << " Current Data Point Count: " << counter
       << std::endl;
    notifyErrorMessage(ss.str(), -600);
  }
  in2.close();
}

// -----------------------------------------------------------------------------
// Read the data part of the RStats.txt file
// -----------------------------------------------------------------------------
void FFTElasticReader::parseRStatsDataLine(const std::string &line, size_t i)
{
  /* When reading the data there should be 6 cols of data.
* The column names should be the following:
* strain
* stress
* EED
* maxPrincipalStress
* minPrincipalStress
* phase
*/
  float e=0.0f, s=0.0f, EED=0.0f, maxps=0.0f, minps=0.0f;
  int p=0;
  size_t offset = 0;
  size_t fieldsRead = 0;
  fieldsRead = sscanf(line.c_str(), "%f %f %f %f %f %i", &e, &s, &EED, &maxps, &minps, &p);

  offset = i;

  m_Strain[offset]=e;
  m_Stress[offset]=s;
  m_EED[offset]=EED;
  m_MaxPrincipalStress[offset]=maxps;
  m_MinPrincipalStress[offset]=minps;
}
