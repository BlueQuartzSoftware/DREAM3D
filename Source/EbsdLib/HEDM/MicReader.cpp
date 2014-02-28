/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2010, Dr. Michael A. Groeber (US Air Force Research Laboratories
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




#include "MicReader.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

#include "MXA/Utilities/MXAFileInfo.h"
#include "MicConstants.h"
#include "EbsdLib/EbsdMacros.h"
#include "EbsdLib/EbsdMath.h"

#ifdef _MSC_VER

#if _MSC_VER < 1400
#define snprintf _snprintf
#else
#define snprintf sprintf_s
#endif
#endif


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MicReader::MicReader() :
  EbsdReader()
{

  // Init all the arrays to NULL
  m_Euler1 = NULL;
  m_Euler2 = NULL;
  m_Euler3 = NULL;
  m_Conf = NULL;
  m_Phase = NULL;
  m_Level = NULL;
  m_Up = NULL;
  m_X = NULL;
  m_Y = NULL;

  m_HeaderMap[Ebsd::Mic::InfileBasename] = MicStringHeaderEntry::NewEbsdHeaderEntry(Ebsd::Mic::InfileBasename);
  m_HeaderMap[Ebsd::Mic::InfileSerialLength] = MicHeaderEntry<int>::NewEbsdHeaderEntry(Ebsd::Mic::InfileSerialLength);
  m_HeaderMap[Ebsd::Mic::OutfileBasename] = MicStringHeaderEntry::NewEbsdHeaderEntry(Ebsd::Mic::OutfileBasename);
  m_HeaderMap[Ebsd::Mic::OutfileSerialLength] = MicHeaderEntry<int>::NewEbsdHeaderEntry(Ebsd::Mic::OutfileSerialLength);
  m_HeaderMap[Ebsd::Mic::OutStructureBasename] = MicStringHeaderEntry::NewEbsdHeaderEntry(Ebsd::Mic::OutStructureBasename);
  m_HeaderMap[Ebsd::Mic::BCPeakDetectorOffset] = MicHeaderEntry<int>::NewEbsdHeaderEntry(Ebsd::Mic::BCPeakDetectorOffset);
  m_HeaderMap[Ebsd::Mic::InFileType] = MicStringHeaderEntry::NewEbsdHeaderEntry(Ebsd::Mic::InFileType);
  m_HeaderMap[Ebsd::Mic::OutfileExtension] = MicStringHeaderEntry::NewEbsdHeaderEntry(Ebsd::Mic::OutfileExtension);
  m_HeaderMap[Ebsd::Mic::InfileExtesnion] = MicStringHeaderEntry::NewEbsdHeaderEntry(Ebsd::Mic::InfileExtesnion);
  m_HeaderMap[Ebsd::Mic::BeamEnergyWidth] = MicHeaderEntry<float>::NewEbsdHeaderEntry(Ebsd::Mic::BeamEnergyWidth);
  m_HeaderMap[Ebsd::Mic::BeamDirection] = MicStringHeaderEntry::NewEbsdHeaderEntry(Ebsd::Mic::BeamDirection);
  m_HeaderMap[Ebsd::Mic::BeamDeflectionChiLaue] = MicHeaderEntry<int>::NewEbsdHeaderEntry(Ebsd::Mic::BeamDeflectionChiLaue);
  m_HeaderMap[Ebsd::Mic::BeamHeight] = MicHeaderEntry<float>::NewEbsdHeaderEntry(Ebsd::Mic::BeamHeight);
  m_HeaderMap[Ebsd::Mic::BeamEnergy] = MicHeaderEntry<float>::NewEbsdHeaderEntry(Ebsd::Mic::BeamEnergy);
  m_HeaderMap[Ebsd::Mic::DetectorFilename] = MicStringHeaderEntry::NewEbsdHeaderEntry(Ebsd::Mic::DetectorFilename);
  m_HeaderMap[Ebsd::Mic::OptimizationConstrainFilename] = MicStringHeaderEntry::NewEbsdHeaderEntry(Ebsd::Mic::OptimizationConstrainFilename);
  m_HeaderMap[Ebsd::Mic::EtaLimit] = MicHeaderEntry<int>::NewEbsdHeaderEntry(Ebsd::Mic::EtaLimit);
  m_HeaderMap[Ebsd::Mic::SampleFilename] = MicStringHeaderEntry::NewEbsdHeaderEntry(Ebsd::Mic::SampleFilename);
  m_HeaderMap[Ebsd::Mic::StructureFilename] = MicStringHeaderEntry::NewEbsdHeaderEntry(Ebsd::Mic::StructureFilename);
  m_HeaderMap[Ebsd::Mic::RotationRangeFilename] = MicStringHeaderEntry::NewEbsdHeaderEntry(Ebsd::Mic::RotationRangeFilename);
  m_HeaderMap[Ebsd::Mic::FundamentalZoneFilename] = MicStringHeaderEntry::NewEbsdHeaderEntry(Ebsd::Mic::FundamentalZoneFilename);
  m_HeaderMap[Ebsd::Mic::SampleSymmetry] = MicStringHeaderEntry::NewEbsdHeaderEntry(Ebsd::Mic::SampleSymmetry);
  m_HeaderMap[Ebsd::Mic::MinAmplitudeFraction] = MicHeaderEntry<float>::NewEbsdHeaderEntry(Ebsd::Mic::MinAmplitudeFraction);
  m_HeaderMap[Ebsd::Mic::MaxQ] = MicHeaderEntry<int>::NewEbsdHeaderEntry(Ebsd::Mic::MaxQ);
  m_HeaderMap[Ebsd::Mic::MaxInitSideLength] = MicHeaderEntry<float>::NewEbsdHeaderEntry(Ebsd::Mic::MaxInitSideLength);
  m_HeaderMap[Ebsd::Mic::MinSideLength] = MicHeaderEntry<float>::NewEbsdHeaderEntry(Ebsd::Mic::MinSideLength);
  m_HeaderMap[Ebsd::Mic::LocalOrientationGridRadius] = MicHeaderEntry<float>::NewEbsdHeaderEntry(Ebsd::Mic::LocalOrientationGridRadius);
  m_HeaderMap[Ebsd::Mic::MinLocalResolution] = MicHeaderEntry<float>::NewEbsdHeaderEntry(Ebsd::Mic::MinLocalResolution);
  m_HeaderMap[Ebsd::Mic::MaxLocalResolution] = MicHeaderEntry<float>::NewEbsdHeaderEntry(Ebsd::Mic::MaxLocalResolution);
  m_HeaderMap[Ebsd::Mic::MaxAcceptedCost] = MicHeaderEntry<float>::NewEbsdHeaderEntry(Ebsd::Mic::MaxAcceptedCost);
  m_HeaderMap[Ebsd::Mic::MaxConvergenceCost] = MicHeaderEntry<float>::NewEbsdHeaderEntry(Ebsd::Mic::MaxConvergenceCost);
  m_HeaderMap[Ebsd::Mic::MaxMCSteps] = MicHeaderEntry<int>::NewEbsdHeaderEntry(Ebsd::Mic::MaxMCSteps);
  m_HeaderMap[Ebsd::Mic::MCRadiusScaleFactor] = MicHeaderEntry<float>::NewEbsdHeaderEntry(Ebsd::Mic::MCRadiusScaleFactor);
  m_HeaderMap[Ebsd::Mic::SuccessiveRestarts] = MicHeaderEntry<int>::NewEbsdHeaderEntry(Ebsd::Mic::SuccessiveRestarts);
  m_HeaderMap[Ebsd::Mic::SecondsBetweenSave] = MicHeaderEntry<int>::NewEbsdHeaderEntry(Ebsd::Mic::SecondsBetweenSave);
  m_HeaderMap[Ebsd::Mic::NumParameterOptimizationSteps] = MicHeaderEntry<int>::NewEbsdHeaderEntry(Ebsd::Mic::NumParameterOptimizationSteps);
  m_HeaderMap[Ebsd::Mic::NumElementToOptimizePerPE] = MicHeaderEntry<int>::NewEbsdHeaderEntry(Ebsd::Mic::NumElementToOptimizePerPE);
  m_HeaderMap[Ebsd::Mic::OptimizationFilename] = MicStringHeaderEntry::NewEbsdHeaderEntry(Ebsd::Mic::OptimizationFilename);
  m_HeaderMap[Ebsd::Mic::DetectionLimitFilename] = MicStringHeaderEntry::NewEbsdHeaderEntry(Ebsd::Mic::DetectionLimitFilename);
  m_HeaderMap[Ebsd::Mic::ParameterMCInitTemperature] = MicHeaderEntry<float>::NewEbsdHeaderEntry(Ebsd::Mic::ParameterMCInitTemperature);
  m_HeaderMap[Ebsd::Mic::OrientationSearchMethod] = MicStringHeaderEntry::NewEbsdHeaderEntry(Ebsd::Mic::OrientationSearchMethod);
  m_HeaderMap[Ebsd::Mic::CoolingFraction] = MicHeaderEntry<float>::NewEbsdHeaderEntry(Ebsd::Mic::CoolingFraction);
  m_HeaderMap[Ebsd::Mic::ThermalizeFraction] = MicHeaderEntry<float>::NewEbsdHeaderEntry(Ebsd::Mic::ThermalizeFraction);
  m_HeaderMap[Ebsd::Mic::ParameterRefinements] = MicHeaderEntry<int>::NewEbsdHeaderEntry(Ebsd::Mic::ParameterRefinements);
  m_HeaderMap[Ebsd::Mic::NumDetectors] = MicHeaderEntry<int>::NewEbsdHeaderEntry(Ebsd::Mic::NumDetectors);
  m_HeaderMap[Ebsd::Mic::DetectorSpacing] = MicStringHeaderEntry::NewEbsdHeaderEntry(Ebsd::Mic::DetectorSpacing);
  m_HeaderMap[Ebsd::Mic::DetectorSpacingDeviation] = MicHeaderEntry<float>::NewEbsdHeaderEntry(Ebsd::Mic::DetectorSpacingDeviation);
  m_HeaderMap[Ebsd::Mic::DetectorOrientationDeviationInEuler] = MicStringHeaderEntry::NewEbsdHeaderEntry(Ebsd::Mic::DetectorOrientationDeviationInEuler);
  m_HeaderMap[Ebsd::Mic::DetectorOrientationDeviationInSO3] = MicHeaderEntry<float>::NewEbsdHeaderEntry(Ebsd::Mic::DetectorOrientationDeviationInSO3);
  m_HeaderMap[Ebsd::Mic::ParamMCMaxLocalRestarts] = MicHeaderEntry<int>::NewEbsdHeaderEntry(Ebsd::Mic::ParamMCMaxLocalRestarts);
  m_HeaderMap[Ebsd::Mic::ParamMCMaxGlobalRestarts] = MicHeaderEntry<int>::NewEbsdHeaderEntry(Ebsd::Mic::ParamMCMaxGlobalRestarts);
  m_HeaderMap[Ebsd::Mic::ParamMCNumGlobalSearchElements] = MicHeaderEntry<int>::NewEbsdHeaderEntry(Ebsd::Mic::ParamMCNumGlobalSearchElements);
  m_HeaderMap[Ebsd::Mic::ConstrainedOptimization] = MicHeaderEntry<int>::NewEbsdHeaderEntry(Ebsd::Mic::ConstrainedOptimization);
  m_HeaderMap[Ebsd::Mic::SearchVolumeReductionFactor] = MicHeaderEntry<int>::NewEbsdHeaderEntry(Ebsd::Mic::SearchVolumeReductionFactor);
  m_HeaderMap[Ebsd::Mic::FileNumStart] = MicHeaderEntry<int>::NewEbsdHeaderEntry(Ebsd::Mic::FileNumStart);
  m_HeaderMap[Ebsd::Mic::FileNumEnd] = MicHeaderEntry<int>::NewEbsdHeaderEntry(Ebsd::Mic::FileNumEnd);
  m_HeaderMap[Ebsd::Mic::SampleLocation] = MicStringHeaderEntry::NewEbsdHeaderEntry(Ebsd::Mic::SampleLocation);
  m_HeaderMap[Ebsd::Mic::SampleOrientation] = MicStringHeaderEntry::NewEbsdHeaderEntry(Ebsd::Mic::SampleOrientation);
  m_HeaderMap[Ebsd::Mic::EnableStrain] = MicHeaderEntry<int>::NewEbsdHeaderEntry(Ebsd::Mic::EnableStrain);
  m_HeaderMap[Ebsd::Mic::SampleCenter] = MicStringHeaderEntry::NewEbsdHeaderEntry(Ebsd::Mic::SampleCenter);
  m_HeaderMap[Ebsd::Mic::SampleRadius] = MicHeaderEntry<float>::NewEbsdHeaderEntry(Ebsd::Mic::SampleRadius);
  m_HeaderMap[Ebsd::Mic::MaxDeepeningHitRatio] = MicHeaderEntry<float>::NewEbsdHeaderEntry(Ebsd::Mic::MaxDeepeningHitRatio);
  m_HeaderMap[Ebsd::Mic::ConsistencyError] = MicHeaderEntry<float>::NewEbsdHeaderEntry(Ebsd::Mic::ConsistencyError);
  m_HeaderMap[Ebsd::Mic::BraggFilterTolerance] = MicHeaderEntry<float>::NewEbsdHeaderEntry(Ebsd::Mic::BraggFilterTolerance);
  m_HeaderMap[Ebsd::Mic::MinAccelerationThreshold] = MicHeaderEntry<float>::NewEbsdHeaderEntry(Ebsd::Mic::MinAccelerationThreshold);
  m_HeaderMap[Ebsd::Mic::MaxDiscreteCandidates] = MicHeaderEntry<int>::NewEbsdHeaderEntry(Ebsd::Mic::MaxDiscreteCandidates);
  m_HeaderMap[Ebsd::Mic::XDim] = MicHeaderEntry<int>::NewEbsdHeaderEntry(Ebsd::Mic::XDim);
  m_HeaderMap[Ebsd::Mic::YDim] = MicHeaderEntry<int>::NewEbsdHeaderEntry(Ebsd::Mic::YDim);
  m_HeaderMap[Ebsd::Mic::XRes] = MicHeaderEntry<float>::NewEbsdHeaderEntry(Ebsd::Mic::XRes);
  m_HeaderMap[Ebsd::Mic::YRes] = MicHeaderEntry<float>::NewEbsdHeaderEntry(Ebsd::Mic::YRes);

  setNumFields(8);
}

// -----------------------------------------------------------------------------
//  Clean up any Memory that was allocated for this class
// -----------------------------------------------------------------------------
MicReader::~MicReader()
{
  deletePointers();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MicReader::initPointers(size_t numElements)
{
  setNumberOfElements(numElements);
  size_t numBytes = numElements * sizeof(float);
  m_Euler1 = allocateArray<float > (numElements);
  m_Euler2 = allocateArray<float > (numElements);
  m_Euler3 = allocateArray<float > (numElements);
  m_Conf = allocateArray<float > (numElements);
  m_Phase = allocateArray<int> (numElements);
  m_Level = allocateArray<int> (numElements);
  m_Up = allocateArray<int> (numElements);
  m_X = allocateArray<float > (numElements);
  m_Y = allocateArray<float > (numElements);

  ::memset(m_Euler1, 0, numBytes);
  ::memset(m_Euler2, 0, numBytes);
  ::memset(m_Euler3, 0, numBytes);
  ::memset(m_Conf, 0, numBytes);
  ::memset(m_Phase, 0, numBytes);
  ::memset(m_Level, 0, numBytes);
  ::memset(m_Up, 0, numBytes);
  ::memset(m_X, 0, numBytes);
  ::memset(m_Y, 0, numBytes);

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MicReader::deletePointers()
{
  this->deallocateArrayData<float > (m_Euler1);
  this->deallocateArrayData<float > (m_Euler2);
  this->deallocateArrayData<float > (m_Euler3);
  this->deallocateArrayData<float > (m_Conf);
  this->deallocateArrayData<int > (m_Phase);
  this->deallocateArrayData<int > (m_Level);
  this->deallocateArrayData<int > (m_Up);
  this->deallocateArrayData<float > (m_X);
  this->deallocateArrayData<float > (m_Y);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void* MicReader::getPointerByName(const std::string &fieldName)
{
  if (fieldName.compare(Ebsd::Mic::Euler1) == 0) { return static_cast<void*>(m_Euler1);}
  if (fieldName.compare(Ebsd::Mic::Euler2) == 0) { return static_cast<void*>(m_Euler2);}
  if (fieldName.compare(Ebsd::Mic::Euler3) == 0) { return static_cast<void*>(m_Euler3);}
  if (fieldName.compare(Ebsd::Mic::Confidence) == 0) { return static_cast<void*>(m_Conf);}
  if (fieldName.compare(Ebsd::Mic::Phase) == 0) { return static_cast<void*>(m_Phase);}
  if (fieldName.compare(Ebsd::Mic::Level) == 0) { return static_cast<void*>(m_Level);}
  if (fieldName.compare(Ebsd::Mic::Up) == 0) { return static_cast<void*>(m_Up);}
  if (fieldName.compare(Ebsd::Mic::X) == 0) { return static_cast<void*>(m_X);}
  if (fieldName.compare(Ebsd::Mic::Y) == 0) { return static_cast<void*>(m_Y);}
  return NULL;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Ebsd::NumType MicReader::getPointerType(const std::string &fieldName)
{
  if (fieldName.compare(Ebsd::Mic::Euler1) == 0) { return Ebsd::Float;}
  if (fieldName.compare(Ebsd::Mic::Euler2) == 0) { return Ebsd::Float;}
  if (fieldName.compare(Ebsd::Mic::Euler3) == 0) { return Ebsd::Float;}
  if (fieldName.compare(Ebsd::Mic::Confidence) == 0) { return Ebsd::Float;}
  if (fieldName.compare(Ebsd::Mic::Phase) == 0) { return Ebsd::Int32;}
  if (fieldName.compare(Ebsd::Mic::Level) == 0) { return Ebsd::Int32;}
  if (fieldName.compare(Ebsd::Mic::Up) == 0) { return Ebsd::Int32;}
  if (fieldName.compare(Ebsd::Mic::X) == 0) { return Ebsd::Float;}
  if (fieldName.compare(Ebsd::Mic::Y) == 0) { return Ebsd::Float;}
  return Ebsd::UnknownNumType;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int MicReader::readHeaderOnly()
{
  int err = 1;
  char buf[kBufferSize];

  std::string origHeader;
  setOriginalHeader(origHeader);
  m_PhaseVector.clear();

  m_CurrentPhase = MicPhase::New();
  m_PhaseVector.push_back(m_CurrentPhase);

  std::string parentPath = MXAFileInfo::parentPath(getFileName());
  std::string name = MXAFileInfo::fileNameWithOutExtension(getFileName());
  if(parentPath.empty() == true) name = name + ".config";
  else name = parentPath + MXAFileInfo::Separator + name + ".config";
  std::ifstream inHeader(name.c_str());
  if (!inHeader.is_open())
  {
    std::cout << "Config file could not be opened: " << name << std::endl;
    return -100;
  }

  // 'name' now contains the complete path to the file with the new extension

  while (!inHeader.eof())
  {
    ::memset(buf, 0, kBufferSize);
    inHeader.getline(buf, kBufferSize);
    parseHeaderLine(buf, kBufferSize);
    int i = 0;
    while (buf[i] != 0) { ++i; }
    buf[i] = 10; //Add back in the \n character
    origHeader.append(buf);
  }
  // Update the Original Header variable
  setOriginalHeader(origHeader);


  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int MicReader::readFile()
{
  std::stringstream ss;
  int err = 1;
  char buf[kBufferSize];
  std::ifstream in(getFileName().c_str());
  if (!in.is_open())
  {
    ss << "Mic file could not be opened: " << getFileName();
    setErrorMessage(ss.str());
    setErrorCode(-100);
    return -100;
  }

  std::string origHeader;
  setOriginalHeader(origHeader);
  m_PhaseVector.clear();


  std::string parentPath = MXAFileInfo::parentPath(getFileName());
  std::string name = MXAFileInfo::fileNameWithOutExtension(getFileName());
  if(parentPath.empty() == true) name = name + ".config";
  else name = parentPath + MXAFileInfo::Separator + name + ".config";
  std::ifstream inHeader(name.c_str());
  if (!inHeader.is_open())
  {
    ss.str("");
    ss << "matching config file could not be opened: " << name;
    setErrorMessage(ss.str());
    setErrorCode(-101);
    return -101;
  }

  // 'name' now contains the complete path to the file with the new extension

  while (!inHeader.eof())
  {
    ::memset(buf, 0, kBufferSize);
    inHeader.getline(buf, kBufferSize);
    parseHeaderLine(buf, kBufferSize);
    int i = 0;
    while (buf[i] != 0) { ++i; }
    buf[i] = 10; //Add back in the \n character

    origHeader.append(buf);
  }
  // Update the Original Header variable
  setOriginalHeader(origHeader);

  parentPath = MXAFileInfo::parentPath(getFileName());
  name = MXAFileInfo::fileNameWithOutExtension(getFileName());
  if(parentPath.empty() == true) name = name + ".dat";
  else name = parentPath + MXAFileInfo::Separator + name + ".dat";
  std::ifstream inHeader2(name.c_str());
  if (!inHeader2.is_open())
  {
    ss.str("");
    ss << "Matching .dat file could not be opened: " << name << std::endl;
    setErrorMessage(ss.str());
    setErrorCode(-102);
    return -102;
  }

  // 'name' now contains the complete path to the file with the new extension

  m_CurrentPhase = MicPhase::New();

  //hard-coded dat file read
  ::memset(buf, 0, kBufferSize);
  inHeader2.getline(buf, kBufferSize);
  m_CurrentPhase->parseLatticeConstants(buf,0,kBufferSize);
  ::memset(buf, 0, kBufferSize);
  inHeader2.getline(buf, kBufferSize);
  m_CurrentPhase->parseLatticeAngles(buf,0,kBufferSize);
  ::memset(buf, 0, kBufferSize);
  inHeader2.getline(buf, kBufferSize);
  m_CurrentPhase->parseBasisAtoms(buf,0,kBufferSize);
  int numAtoms;
  sscanf(buf, "%d", &numAtoms);
  for(int iter=0;iter<numAtoms;iter++)
  {
    ::memset(buf, 0, kBufferSize);
    inHeader2.getline(buf, kBufferSize);
    m_CurrentPhase->parseZandCoordinates(buf,0,kBufferSize);
  }
  m_CurrentPhase->setPhaseIndex(1);
  std::string symm = getSampleSymmetry();
  m_CurrentPhase->setSymmetry(symm);
  m_PhaseVector.push_back(m_CurrentPhase);


  // We need to pass in the buffer because it has the first line of data
  err = readData(in, buf, kBufferSize);
  if (err < 0) {
      ss.str("");
    ss << "Error Reading the HEDM Data File." << std::endl;
    setErrorMessage(ss.str());
    setErrorCode(-103);
    return -103;

  }

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int MicReader::readData(std::ifstream &in, char* buf, size_t bufSize)
{
  // Delete any currently existing pointers
  deletePointers();
  // Initialize new pointers
  size_t totalDataRows = 0;
  size_t totalPossibleDataRows = 0;
  size_t fieldsRead = 0;
  float origEdgeLength;
  float xMax = 0, yMax = 0;
  float xMin = 1000000000, yMin = 1000000000;
  size_t counter = 0;

  initPointers(1);
  if (NULL == m_Euler1 || NULL == m_Euler2 || NULL == m_Euler3
      || NULL == m_Conf || NULL == m_Phase || NULL == m_Level  || m_X == NULL || m_Y == NULL)
  {
    std::cout << "Internal pointers were NULL at " __FILE__ << "(" << __LINE__ << ")" << std::endl;
    return -1;
  }

  ::memset(buf, 0, bufSize); // Clear the buffer
  in.getline(buf, kBufferSize);// Read the next line of data
  fieldsRead = sscanf(buf, "%f", &origEdgeLength);
  if(fieldsRead != 1) {}
  ::memset(buf, 0, bufSize); // Clear the buffer
  in.getline(buf, kBufferSize);// Read the next line of data
  this->parseDataLine(buf, 0);
  int level = m_Level[0];
  float newEdgeLength = origEdgeLength/powf(2.0,float(level));
  totalPossibleDataRows = static_cast<size_t>(6.0f*powf(4.0f,float(level)));
  initPointers(totalPossibleDataRows);
  this->parseDataLine(buf, 0);
  ::memset(buf, 0, bufSize); // Clear the buffer
  in.getline(buf, kBufferSize);// Read the next line of data
  ++counter;
  for(size_t i = 1; i < totalPossibleDataRows; ++i)
  {
    this->parseDataLine(buf, i);
    ::memset(buf, 0, bufSize); // Clear the buffer
    in.getline(buf, kBufferSize);// Read the next line of data
    ++counter;
    if (in.eof() == true) break;
  }
  totalDataRows = counter;

  std::vector<float> EA1(totalDataRows,0.0);
  std::vector<float> EA2(totalDataRows,0.0);
  std::vector<float> EA3(totalDataRows,0.0);
  std::vector<float> confidence(totalDataRows,0.0);
  std::vector<int> phase(totalDataRows,0);
  std::vector<int> up(totalDataRows,0);
  std::vector<float> xVal(totalDataRows,0.0);
  std::vector<float> yVal(totalDataRows,0.0);
  float constant = static_cast<float>(1.0f/(2.0*sqrt(3.0)));
  float x = 0, y = 0;
  for(size_t i = 0; i < totalDataRows; ++i)
  {
    if(m_Up[i] == 1)
    {
      x = m_X[i] + (newEdgeLength/2.0f);
      y = m_Y[i] + (constant*newEdgeLength);
    } 
	else if(m_Up[i] == 2)
    {
      x = m_X[i] + (newEdgeLength/2.0f);
      y = m_Y[i] - (constant*newEdgeLength);
    }
    if(x > xMax) xMax = x;
    if(y > yMax) yMax = y;
    if(x < xMin) xMin = x;
    if(y < yMin) yMin = y;
    EA1[i] = m_Euler1[i];
    EA2[i] = m_Euler2[i];
    EA3[i] = m_Euler3[i];
    confidence[i] = m_Conf[i];
    phase[i] = m_Phase[i];
    up[i] = m_Up[i];
    xVal[i] = m_X[i];
    yVal[i] = m_Y[i];
  }
  xDim = int((xMax-xMin)/newEdgeLength)+1;
  yDim = int((yMax-yMin)/newEdgeLength)+1;
  xRes = newEdgeLength*1000.0f;
  yRes = newEdgeLength*1000.0f;

  char buf_2[16];
  ::memset(buf_2, 0, 16);
  snprintf(buf_2, 16, "%d", xDim);
  m_HeaderMap[Ebsd::Mic::XDim]->parseValue(buf_2,0,16);
  ::memset(buf_2, 0, 16);
  snprintf(buf_2, 16, "%d", yDim);
  m_HeaderMap[Ebsd::Mic::YDim]->parseValue(buf_2,0,16);
  ::memset(buf_2, 0, 16);
  snprintf(buf_2, 16, "%f", xRes);
  m_HeaderMap[Ebsd::Mic::XRes]->parseValue(buf_2,0,16);
  ::memset(buf_2, 0, 16);
  snprintf(buf_2, 16, "%f", yRes);
  m_HeaderMap[Ebsd::Mic::YRes]->parseValue(buf_2,0,16);


  // Delete any currently existing pointers
  deletePointers();
  // Resize pointers
  initPointers(xDim*yDim);


  float xA = 0.0f, xB = 0.0f, xC = 0.0f, yA = 0.0f, yB=0.0f, yC=0.0f;
  int point;
  float root3over2 = sqrtf(3.0f)/2.0f;
  int check1, check2, check3;
  for(size_t i = 0; i < totalDataRows; ++i)
  {
    xA = xVal[i]-xMin;
    xB = xA + newEdgeLength;
    xC = xA + (newEdgeLength/2.0f);
    if(up[i] == 1)
    {
      yA = yVal[i]-yMin;
      yB = yA;
      yC = yA+(root3over2*newEdgeLength);
    }
    if(up[i] == 2)
    {
      yB = yVal[i]-yMin;
      yC = yB;
      yA = yB-(root3over2*newEdgeLength);
    }
    for(int j = int(xA/newEdgeLength); j < int(xB/newEdgeLength)+1; j++)
    {
      for(int k = int(yA/newEdgeLength); k < int(yC/newEdgeLength)+1; k++)
      {
        x = float(j)*newEdgeLength;
        y = float(k)*newEdgeLength;
        check1 = static_cast<int>((x-xB)*(yA-yB)-(xA-xB)*(y-yB));
        check2 = static_cast<int>((x-xC)*(yB-yC)-(xB-xC)*(y-yC));
        check3 = static_cast<int>((x-xA)*(yC-yA)-(xC-xA)*(y-yA));
        if((check1<=0 && check2<=0 && check3<=0) || (check1>=0 && check2>=0 && check3>=0))
        {
          point = (k*xDim) + j;
          m_Euler1[point] = EA1[i];
          m_Euler2[point] = EA2[i];
          m_Euler3[point] = EA3[i];
          m_Conf[point] = confidence[i];
          m_Phase[point] = phase[i];
          m_X[point] = float(j)*xRes;
          m_Y[point] = float(k)*yRes;
        }
      }
    }
  }


  return 0;
}

// -----------------------------------------------------------------------------
//  Read the Header part of the Mic file
// -----------------------------------------------------------------------------
void MicReader::parseHeaderLine(char* buf, size_t length)
{
  if (buf[0] == '#')
  {
    return;
  }
  // Start at the first character and walk until you find another non-space character
  size_t i = 0;
  while(buf[i] == ' ')
  {
    ++i;
  }
  size_t wordStart = i;
  size_t wordEnd = i+1;
  while(1)
  {
    if (buf[i] == 45 || buf[i] == 95) { ++i; } // "-" or "_" character
    else if (buf[i] >= 65 && buf[i] <=90) { ++i; } // Upper case alpha character
    else if (buf[i] >= 97 && buf[i] <=122) {++i; } // Lower case alpha character
    else { break;}
  }
  wordEnd = i;

  std::string word( &(buf[wordStart]), wordEnd - wordStart);

  if (word.size() == 0)
  {
    return;
  }

  EbsdHeaderEntry::Pointer p = m_HeaderMap[word];
  if (NULL == p.get())
  {
    /*
      std::cout << "---------------------------" << std::endl;
      std::cout << "Could not find header entry for key'" << word << "'" << std::endl;
      std::string upper(word);
      std::transform(upper.begin(), upper.end(), upper.begin(), ::toupper);
      std::cout << "#define ANG_" << upper << "     \"" << word << "\"" << std::endl;
      std::cout << "const std::string " << word << "(ANG_" << upper << ");" << std::endl;

      std::cout << "angInstanceProperty(AngHeaderEntry<float>. float, " << word << "Ebsd::Ang::" << word << std::endl;
      std::cout << "m_Headermap[Ebsd::Ang::" << word << "] = AngHeaderEntry<float>::NewEbsdHeaderEntry(Ebsd::Ang::" << word << ");" << std::endl;
      */
#if 0
    std::cout << "<tr>\n    <td>" << word << "</td>\n    <td>" << "H5T_STRING" << "</td>\n";
    std::cout << "    <td colspan=\"2\"> Contains value for the header entry " << word << "</td>\n</tr>" << std::endl;
#endif
    return;
  }
  else
  {
    p->parseValue(buf, wordEnd, length);
#if 0
    std::cout << "<tr>\n    <td>" << p->getKey() << "</td>\n    <td>" << p->getHDFType() << "</td>\n";
    std::cout << "    <td colspan=\"2\"> Contains value for the header entry " << p->getKey() << "</td>\n</tr>" << std::endl;
#endif

  }
}

// -----------------------------------------------------------------------------
//  Read the data part of the Mic file
// -----------------------------------------------------------------------------
void MicReader::parseDataLine(const std::string &line, size_t i)
{
  /* When reading the data there should be at least 8 cols of data. There may even
   * be 10 columns of data. The column names should be the following:
   * phi1
   * phi
   * phi2
   * x pos
   * y pos
   * image quality
   * confidence index
   * phase
   * SEM Signal
   * Fit of Solution
   *
   * Some HEDM Mic files do NOT have all 10 columns. Assume these are lacking the last
   * 2 columns and all the other columns are the same as above.
   */
  float p1 = 0.0f, p=0.0f, p2=0.0f, x=-1.0f, y=-1.0f, z=-1.0f, conf=-1.0f;
  int up = 0, level = 0, good = 0;
  size_t offset = 0;
  size_t fieldsRead = 0;
  fieldsRead = sscanf(line.c_str(), "%f %f %f %d %d %d %f %f %f %f", &x, &y,&z, &up, &level, &good, &p1, &p, &p2, &conf);
    if(fieldsRead != 10) {}
  offset = i;

  m_Euler1[offset] = p1;
  m_Euler2[offset] = p;
  m_Euler3[offset] = p2;
  m_Conf[offset] = conf;
  m_Phase[offset] = good;
  m_Level[offset] = level;
  m_Up[offset] = up;
  m_X[offset] = x;
  m_Y[offset] = y;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int MicReader::getXDimension()
{
  return getXDim();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int MicReader::getYDimension()
{
  return getYDim();
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MicReader::setXDimension(int xD)
{
  setXDim(xD);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MicReader::setYDimension(int yD)
{
  setYDim(yD);
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
float MicReader::getXStep()
{
  return getXRes();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
float MicReader::getYStep()
{
  return getYRes();
}
