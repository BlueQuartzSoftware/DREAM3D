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


#include <algorithm>

#include <QtCore/QObject>
#include <QtCore/QFile>
#include <QtCore/QtDebug>
#include <QtCore/QTextStream>
#include <QtCore/QFileInfo>
#include <QtCore/QDir>

#include "EbsdLib/EbsdMacros.h"
#include "EbsdLib/EbsdMath.h"

#include "MicConstants.h"

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

  setNumFeatures(8);
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
void* MicReader::getPointerByName(const QString& featureName)
{
  if (featureName.compare(Ebsd::Mic::Euler1) == 0) { return static_cast<void*>(m_Euler1);}
  if (featureName.compare(Ebsd::Mic::Euler2) == 0) { return static_cast<void*>(m_Euler2);}
  if (featureName.compare(Ebsd::Mic::Euler3) == 0) { return static_cast<void*>(m_Euler3);}
  if (featureName.compare(Ebsd::Mic::Confidence) == 0) { return static_cast<void*>(m_Conf);}
  if (featureName.compare(Ebsd::Mic::Phase) == 0) { return static_cast<void*>(m_Phase);}
  if (featureName.compare(Ebsd::Mic::Level) == 0) { return static_cast<void*>(m_Level);}
  if (featureName.compare(Ebsd::Mic::Up) == 0) { return static_cast<void*>(m_Up);}
  if (featureName.compare(Ebsd::Mic::X) == 0) { return static_cast<void*>(m_X);}
  if (featureName.compare(Ebsd::Mic::Y) == 0) { return static_cast<void*>(m_Y);}
  return NULL;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Ebsd::NumType MicReader::getPointerType(const QString& featureName)
{
  if (featureName.compare(Ebsd::Mic::Euler1) == 0) { return Ebsd::Float;}
  if (featureName.compare(Ebsd::Mic::Euler2) == 0) { return Ebsd::Float;}
  if (featureName.compare(Ebsd::Mic::Euler3) == 0) { return Ebsd::Float;}
  if (featureName.compare(Ebsd::Mic::Confidence) == 0) { return Ebsd::Float;}
  if (featureName.compare(Ebsd::Mic::Phase) == 0) { return Ebsd::Int32;}
  if (featureName.compare(Ebsd::Mic::Level) == 0) { return Ebsd::Int32;}
  if (featureName.compare(Ebsd::Mic::Up) == 0) { return Ebsd::Int32;}
  if (featureName.compare(Ebsd::Mic::X) == 0) { return Ebsd::Float;}
  if (featureName.compare(Ebsd::Mic::Y) == 0) { return Ebsd::Float;}
  return Ebsd::UnknownNumType;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int MicReader::readHeaderOnly()
{
  int err = 1;
  QByteArray buf;

  QString origHeader;
  setOriginalHeader(origHeader);
  m_PhaseVector.clear();

  m_CurrentPhase = MicPhase::New();
  m_PhaseVector.push_back(m_CurrentPhase);

  QFileInfo fi(getFileName());

  QString parentPath(fi.path());
  QString name = fi.baseName();
  if(QDir::toNativeSeparators(parentPath).isEmpty() == true) { name = name + ".config"; }
  else { name = parentPath + QDir::separator() + name + ".config"; }

  QFile inHeader(getFileName());
  if (!inHeader.open(QIODevice::ReadOnly | QIODevice::Text))
  {
    QString msg = QString("HEDM .config file could not be opened: ") + name;
    setErrorCode(-100);
    setErrorMessage(msg);
    return -100;
  }

  // 'name' now contains the complete path to the file with the new extension

  while (!inHeader.atEnd())
  {
    buf = inHeader.readLine();
    origHeader.append(buf); // Append the line NOW to the header as we are going to modify it next

    parseHeaderLine(buf);
  }
  // Update the Original Header variable
  setOriginalHeader(origHeader);

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int MicReader::readDatFile()
{

  QFileInfo fi(getFileName());
  QString parentPath(fi.path());
  QString name = fi.baseName();
  if(QDir::toNativeSeparators(parentPath).isEmpty() == true) { name = name + ".dat"; }
  else { name = parentPath + QDir::separator() + name + ".dat"; }

  QFile inHeader(name);
  if (!inHeader.open(QIODevice::ReadOnly | QIODevice::Text))
  {
    QString msg = QString("HEDM .dat file could not be opened: ") + name;
    setErrorCode(-111);
    setErrorMessage(msg);
    return -111;
  }

  QByteArray buf;
  m_CurrentPhase = MicPhase::New();

  //hard-coded dat file read
  buf = inHeader.readLine();
  m_CurrentPhase->parseLatticeConstants(buf);
  buf = inHeader.readLine();
  m_CurrentPhase->parseLatticeAngles(buf);
  buf = inHeader.readLine();

  m_CurrentPhase->parseBasisAtoms(buf);
  int numAtoms;
  sscanf(buf, "%d", &numAtoms);
  for(int iter = 0; iter < numAtoms; iter++)
  {
    buf = inHeader.readLine();

    m_CurrentPhase->parseZandCoordinates(buf);
  }
  m_CurrentPhase->setPhaseIndex(1);
  QString symm = getSampleSymmetry();
  m_CurrentPhase->setSymmetry(symm);
  m_PhaseVector.push_back(m_CurrentPhase);
  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int MicReader::readFile()
{
  int err = 1;

  QString origHeader;
  setOriginalHeader(origHeader);
  m_PhaseVector.clear();

  // Read the .config file
  err = readHeaderOnly();
  if (err < 0) { return err; }

  // Read the .dat file
  err = readDatFile();
  if (err < 0) { return err; }

  // We need to pass in the buffer because it has the first line of data
  err = readMicFile();
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int MicReader::readMicFile()
{

  QFile in(getFileName());
  if (!in.open(QIODevice::ReadOnly | QIODevice::Text))
  {
    QString msg = QObject::tr("Mic file could not be opened: %1").arg(getFileName());
    setErrorMessage(msg);
    setErrorCode(-113);
    return -113;
  }

  // Delete any currently existing pointers
  deletePointers();
  // Initialize new pointers
  int32_t totalDataRows = 0;
  int32_t totalPossibleDataRows = 0;
//  size_t featuresRead = 0;
  float origEdgeLength;
  float xMax = 0, yMax = 0;
  float xMin = 1000000000, yMin = 1000000000;
  float xMinUM, yMinUM;
  int32_t counter = 0;
  bool ok = false;
  QByteArray buf;

  // Read the First line in the file which is the edge length
  buf = in.readLine();
  origEdgeLength = buf.toFloat(&ok);


  // Initialize our pointers to 1 element
  initPointers(1);
  if (NULL == m_Euler1 || NULL == m_Euler2 || NULL == m_Euler3
      || NULL == m_Conf || NULL == m_Phase || NULL == m_Level  || m_X == NULL || m_Y == NULL)
  {
    qDebug() << "Internal pointers were NULL at " __FILE__ << "(" << __LINE__ << ")";
    return -1;
  }

  // Read the first line of actual data which we then derive the actual number of data rows
  buf = in.readLine();
  parseDataLine(buf, 0);

  int level = m_Level[0];
  float newEdgeLength = origEdgeLength / powf(2.0, float(level));
  totalPossibleDataRows = static_cast<size_t>(6.0f * powf(4.0f, float(level)));

  // Reinitialize the pointers that hold the data to the proper size
  initPointers(totalPossibleDataRows);
  // Reparse the first data line to get those values into the new arrays
  parseDataLine(buf, 0);

  buf = in.readLine();
  ++counter;
  for(size_t i = 1; i < totalPossibleDataRows; ++i)
  {
    parseDataLine(buf, i);
    buf = in.readLine();

    ++counter;
    if (in.atEnd() == true) { break; }
  }
  totalDataRows = counter;

  std::vector<float> EA1(totalDataRows, 0.0);
  std::vector<float> EA2(totalDataRows, 0.0);
  std::vector<float> EA3(totalDataRows, 0.0);
  std::vector<float> confidence(totalDataRows, 0.0);
  std::vector<int> phase(totalDataRows, 0);
  std::vector<int> up(totalDataRows, 0);
  std::vector<float> xVal(totalDataRows, 0.0);
  std::vector<float> yVal(totalDataRows, 0.0);
  float constant = static_cast<float>(1.0f / (2.0 * sqrt(3.0)));
  float x, y;
  for(size_t i = 0; i < totalDataRows; ++i)
  {
    if(m_Up[i] == 1)
    {
      x = m_X[i] + (newEdgeLength / 2.0f);
      y = m_Y[i] + (constant * newEdgeLength);
    }
    if(m_Up[i] == 2)
    {
      x = m_X[i] + (newEdgeLength / 2.0f);
      y = m_Y[i] - (constant * newEdgeLength);
    }
    if(x > xMax) { xMax = x; }
    if(y > yMax) { yMax = y; }
    if(x < xMin) { xMin = x; }
    if(y < yMin) { yMin = y; }
    EA1[i] = m_Euler1[i];
    EA2[i] = m_Euler2[i];
    EA3[i] = m_Euler3[i];
    confidence[i] = m_Conf[i];
    phase[i] = m_Phase[i];
    up[i] = m_Up[i];
    xVal[i] = m_X[i];
    yVal[i] = m_Y[i];
  }
  xMin = xMin - (2.0 * newEdgeLength);
  xMax = xMax + (2.0 * newEdgeLength);
  yMin = yMin - (2.0 * newEdgeLength);
  yMax = yMax + (2.0 * newEdgeLength);
  xDim = int((xMax - xMin) / newEdgeLength) + 1;
  yDim = int((yMax - yMin) / newEdgeLength) + 1;
  xRes = newEdgeLength * 1000.0f;
  yRes = newEdgeLength * 1000.0f;
  xMinUM = xMin * 1000.0f;
  yMinUM = xMin * 1000.0f;

  EbsdHeaderEntry::Pointer xDimHeader = MicHeaderEntry<int>::NewEbsdHeaderEntry(Ebsd::Mic::XDim, xDim);
  m_HeaderMap[Ebsd::Mic::XDim] = xDimHeader;

  EbsdHeaderEntry::Pointer yDimHeader = MicHeaderEntry<int>::NewEbsdHeaderEntry(Ebsd::Mic::YDim, yDim);
  m_HeaderMap[Ebsd::Mic::YDim] = yDimHeader;

  EbsdHeaderEntry::Pointer xResHeader = MicHeaderEntry<float>::NewEbsdHeaderEntry(Ebsd::Mic::XRes, xRes);
  m_HeaderMap[Ebsd::Mic::XRes] = xResHeader;

  EbsdHeaderEntry::Pointer yResHeader = MicHeaderEntry<float>::NewEbsdHeaderEntry(Ebsd::Mic::YRes, yRes);
  m_HeaderMap[Ebsd::Mic::YRes] = yResHeader;


  // Delete any currently existing pointers
  deletePointers();
  // Resize pointers
  initPointers(xDim * yDim);


  float xA, xB, xC, yA, yB, yC;
  int point;
  float root3over2 = sqrtf(3.0f) / 2.0f;
  int check1, check2, check3;
  for(size_t i = 0; i < totalDataRows; ++i)
  {
    xA = xVal[i] - xMin;
    xB = xA + newEdgeLength;
    xC = xA + (newEdgeLength / 2.0f);
    if(up[i] == 1)
    {
      yA = yVal[i] - yMin;
      yB = yA;
      yC = yA + (root3over2 * newEdgeLength);
    }
    if(up[i] == 2)
    {
      yB = yVal[i] - yMin;
      yC = yB;
      yA = yB - (root3over2 * newEdgeLength);
    }
    for(int j = int(xA / newEdgeLength); j < int(xB / newEdgeLength) + 1; j++)
    {
      for(int k = int(yA / newEdgeLength); k < int(yC / newEdgeLength) + 1; k++)
      {
        x = float(j) * newEdgeLength;
        y = float(k) * newEdgeLength;
        check1 = static_cast<int>((x - xB) * (yA - yB) - (xA - xB) * (y - yB));
        check2 = static_cast<int>((x - xC) * (yB - yC) - (xB - xC) * (y - yC));
        check3 = static_cast<int>((x - xA) * (yC - yA) - (xC - xA) * (y - yA));
        if((check1 <= 0 && check2 <= 0 && check3 <= 0) || (check1 >= 0 && check2 >= 0 && check3 >= 0))
        {
          point = (k * xDim) + j;
          m_Euler1[point] = EA1[i];
          m_Euler2[point] = EA2[i];
          m_Euler3[point] = EA3[i];
          m_Conf[point] = confidence[i];
          m_Phase[point] = phase[i];
          m_X[point] = float(j) * xRes + xMinUM;
          m_Y[point] = float(k) * yRes + yMinUM;
        }
      }
    }
  }


  return 0;
}

// -----------------------------------------------------------------------------
//  Read the Matching Config file to the .mic file
// -----------------------------------------------------------------------------
void MicReader::parseHeaderLine(QByteArray& line)
{
  if (line[0] == '#')
  {
    return;
  }
  line = line.trimmed();
  line = line.simplified();

  // Split the lines based on 'space' delimiter
  QList<QByteArray> tokens = line.split(' ');
  QString key(tokens[0]);

  EbsdHeaderEntry::Pointer p = m_HeaderMap[key];
  if (NULL == p.get())
  {
    /*
      std::cout << "---------------------------" << std::endl;
      std::cout << "Could not find header entry for key'" << word << "'" << std::endl;
      QString upper(word);
      std::transform(upper.begin(), upper.end(), upper.begin(), ::toupper);
      std::cout << "#define ANG_" << upper << "     \"" << word << "\"" << std::endl;
      std::cout << "const QString " << word << "(ANG_" << upper << ");" << std::endl;

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
    int size = tokens[0].count();
    line = line.mid(size);
    p->parseValue(line); // Send the second part of the QByteArray, just the value part
#if 0
    std::cout << "<tr>\n    <td>" << p->getKey() << "</td>\n    <td>" << p->getHDFType() << "</td>\n";
    std::cout << "    <td colspan=\"2\"> Contains value for the header entry " << p->getKey() << "</td>\n</tr>" << std::endl;
#endif

  }
}

// -----------------------------------------------------------------------------
//  Read the data part of the Mic file
// -----------------------------------------------------------------------------
void MicReader::parseDataLine(QByteArray& line, size_t i)
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
  float p1 = 0.0f, p = 0.0f, p2 = 0.0f, x = -1.0f, y = -1.0f, z = -1.0f, conf = -1.0f, junk1, junk2, junk3;
  int up = 0, level = 0, good = 0, junk4, junk5, junk6, junk7, junk8, junk9;
  size_t offset = 0;
  size_t featuresRead = 0;
  featuresRead = sscanf(line.data(), "%f %f %f %d %d %d %f %f %f %f %f %f %f %d %d %d %d %d %d", &x, &y, &z, &up, &level, &good, &p1, &p, &p2, &conf, &junk1, &junk2, &junk3, &junk4, &junk5, &junk6, &junk7, &junk8, &junk9);

  offset = i;

  if(good > 0) { good = 1; }
  else { good = 0; }

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
