/* ============================================================================
* Copyright (c) 2009-2016 BlueQuartz Software, LLC
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
* Neither the name of BlueQuartz Software, the US Air Force, nor the names of its
* contributors may be used to endorse or promote products derived from this software
* without specific prior written permission.
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
* The code contained herein was partially funded by the followig contracts:
*    United States Air Force Prime Contract FA8650-07-D-5800
*    United States Air Force Prime Contract FA8650-10-D-5210
*    United States Prime Contract Navy N00173-07-C-2068
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "AngReader.h"

#include <algorithm>

#include <QtCore/QFile>
#include <QtCore/QObject>
#include <QtCore/QTextStream>

#include "AngConstants.h"
#include "EbsdLib/EbsdMacros.h"
#include "EbsdLib/EbsdMath.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AngReader::AngReader()
{
  // Init all the arrays to nullptr
  m_Phi1 = nullptr;
  m_Phi = nullptr;
  m_Phi2 = nullptr;
  m_Iq = nullptr;
  m_Ci = nullptr;
  m_PhaseData = nullptr;
  m_X = nullptr;
  m_Y = nullptr;
  m_SEMSignal = nullptr;
  m_Fit = nullptr;

  setNumFeatures(10);

  m_ReadHexGrid = false;

  // Initialize the map of header key to header value
  m_HeaderMap[Ebsd::Ang::TEMPIXPerUM] = AngHeaderEntry<float>::NewEbsdHeaderEntry(Ebsd::Ang::TEMPIXPerUM);
  m_HeaderMap[Ebsd::Ang::XStar] = AngHeaderEntry<float>::NewEbsdHeaderEntry(Ebsd::Ang::XStar);
  m_HeaderMap[Ebsd::Ang::YStar] = AngHeaderEntry<float>::NewEbsdHeaderEntry(Ebsd::Ang::YStar);
  m_HeaderMap[Ebsd::Ang::ZStar] = AngHeaderEntry<float>::NewEbsdHeaderEntry(Ebsd::Ang::ZStar);
  m_HeaderMap[Ebsd::Ang::WorkingDistance] = AngHeaderEntry<float>::NewEbsdHeaderEntry(Ebsd::Ang::WorkingDistance);
  m_HeaderMap[Ebsd::Ang::Grid] = AngStringHeaderEntry::NewEbsdHeaderEntry(Ebsd::Ang::Grid);
  m_HeaderMap[Ebsd::Ang::XStep] = AngHeaderEntry<float>::NewEbsdHeaderEntry(Ebsd::Ang::XStep);
  m_HeaderMap[Ebsd::Ang::YStep] = AngHeaderEntry<float>::NewEbsdHeaderEntry(Ebsd::Ang::YStep);
  m_HeaderMap[Ebsd::Ang::ZStep] = AngHeaderEntry<float>::NewEbsdHeaderEntry(Ebsd::Ang::ZStep); // NOT actually in the file>::NewEbsdHeaderEntry(); , but may be needed
  m_HeaderMap[Ebsd::Ang::ZPos] = AngHeaderEntry<float>::NewEbsdHeaderEntry(Ebsd::Ang::ZPos);   // NOT actually in the file>::NewEbsdHeaderEntry(); , but may be needed
  m_HeaderMap[Ebsd::Ang::ZMax] = AngHeaderEntry<float>::NewEbsdHeaderEntry(Ebsd::Ang::ZMax);   // NOT actually in the file>::NewEbsdHeaderEntry(); , but may be needed
  m_HeaderMap[Ebsd::Ang::NColsOdd] = AngHeaderEntry<int>::NewEbsdHeaderEntry(Ebsd::Ang::NColsOdd);
  m_HeaderMap[Ebsd::Ang::NColsEven] = AngHeaderEntry<int>::NewEbsdHeaderEntry(Ebsd::Ang::NColsEven);
  m_HeaderMap[Ebsd::Ang::NRows] = AngHeaderEntry<int>::NewEbsdHeaderEntry(Ebsd::Ang::NRows);
  m_HeaderMap[Ebsd::Ang::OPERATOR] = AngStringHeaderEntry::NewEbsdHeaderEntry(Ebsd::Ang::OPERATOR);
  m_HeaderMap[Ebsd::Ang::SAMPLEID] = AngStringHeaderEntry::NewEbsdHeaderEntry(Ebsd::Ang::SAMPLEID);
  m_HeaderMap[Ebsd::Ang::SCANID] = AngStringHeaderEntry::NewEbsdHeaderEntry(Ebsd::Ang::SCANID);

  // Give these values some defaults
  setNumOddCols(-1);
  setNumEvenCols(-1);
  setNumRows(-1);
  setXStep(0.0f);
  setYStep(0.0f);
}

// -----------------------------------------------------------------------------
//  Clean up any Memory that was allocated for this class
// -----------------------------------------------------------------------------
AngReader::~AngReader()
{
  if(m_Phi1Cleanup)
  {
    deallocateArrayData<float>(m_Phi1);
    m_Phi1 = nullptr;
  }
  if(m_PhiCleanup)
  {
    deallocateArrayData<float>(m_Phi);
    m_Phi = nullptr;
  }
  if(m_Phi2Cleanup)
  {
    deallocateArrayData<float>(m_Phi2);
    m_Phi2 = nullptr;
  }
  if(m_IqCleanup)
  {
    deallocateArrayData<float>(m_Iq);
    m_Iq = nullptr;
  }
  if(m_CiCleanup)
  {
    deallocateArrayData<float>(m_Ci);
    m_Ci = nullptr;
  }
  if(m_PhaseDataCleanup)
  {
    deallocateArrayData<int32_t>(m_PhaseData);
    m_PhaseData = nullptr;
  }
  if(m_XCleanup)
  {
    deallocateArrayData<float>(m_X);
    m_X = nullptr;
  }
  if(m_YCleanup)
  {
    deallocateArrayData<float>(m_Y);
    m_Y = nullptr;
  }
  if(m_SEMSignalCleanup)
  {
    deallocateArrayData<float>(m_SEMSignal);
    m_SEMSignal = nullptr;
  }
  if(m_FitCleanup)
  {
    deallocateArrayData<float>(m_Fit);
    m_Fit = nullptr;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void* AngReader::getPointerByName(const QString& featureName)
{
  if(featureName.compare(Ebsd::Ang::Phi1) == 0)
  {
    return static_cast<void*>(m_Phi1);
  }
  if(featureName.compare(Ebsd::Ang::Phi) == 0)
  {
    return static_cast<void*>(m_Phi);
  }
  if(featureName.compare(Ebsd::Ang::Phi2) == 0)
  {
    return static_cast<void*>(m_Phi2);
  }
  if(featureName.compare(Ebsd::Ang::ImageQuality) == 0)
  {
    return static_cast<void*>(m_Iq);
  }
  if(featureName.compare(Ebsd::Ang::ConfidenceIndex) == 0)
  {
    return static_cast<void*>(m_Ci);
  }
  if(featureName.compare(Ebsd::Ang::PhaseData) == 0)
  {
    return static_cast<void*>(m_PhaseData);
  }
  if(featureName.compare(Ebsd::Ang::XPosition) == 0)
  {
    return static_cast<void*>(m_X);
  }
  if(featureName.compare(Ebsd::Ang::YPosition) == 0)
  {
    return static_cast<void*>(m_Y);
  }
  if(featureName.compare(Ebsd::Ang::SEMSignal) == 0)
  {
    return static_cast<void*>(m_SEMSignal);
  }
  if(featureName.compare(Ebsd::Ang::Fit) == 0)
  {
    return static_cast<void*>(m_Fit);
  }
  return nullptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Ebsd::NumType AngReader::getPointerType(const QString& featureName)
{
  if(featureName.compare(Ebsd::Ang::Phi1) == 0)
  {
    return Ebsd::Float;
  }
  if(featureName.compare(Ebsd::Ang::Phi) == 0)
  {
    return Ebsd::Float;
  }
  if(featureName.compare(Ebsd::Ang::Phi2) == 0)
  {
    return Ebsd::Float;
  }
  if(featureName.compare(Ebsd::Ang::ImageQuality) == 0)
  {
    return Ebsd::Float;
  }
  if(featureName.compare(Ebsd::Ang::ConfidenceIndex) == 0)
  {
    return Ebsd::Float;
  }
  if(featureName.compare(Ebsd::Ang::PhaseData) == 0)
  {
    return Ebsd::Int32;
  }
  if(featureName.compare(Ebsd::Ang::XPosition) == 0)
  {
    return Ebsd::Float;
  }
  if(featureName.compare(Ebsd::Ang::YPosition) == 0)
  {
    return Ebsd::Float;
  }
  if(featureName.compare(Ebsd::Ang::SEMSignal) == 0)
  {
    return Ebsd::Float;
  }
  if(featureName.compare(Ebsd::Ang::Fit) == 0)
  {
    return Ebsd::Float;
  }
  return Ebsd::UnknownNumType;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int AngReader::readHeaderOnly()
{
  int err = 1;
  QByteArray buf;
  QFile in(getFileName());
  setHeaderIsComplete(false);
  if(!in.open(QIODevice::ReadOnly | QIODevice::Text))
  {
    QString msg = QString("Ang file could not be opened: ") + getFileName();
    setErrorCode(-100);
    setErrorMessage(msg);
    return -100;
  }
  QString origHeader;
  setOriginalHeader(origHeader);
  QTextStream ostr(&origHeader);
  m_PhaseVector.clear();

  while(!in.atEnd() && !getHeaderIsComplete())
  {
    buf = in.readLine();
    parseHeaderLine(buf);
    if(!getHeaderIsComplete())
    {
      ostr << buf << "\n";
    }
  }
  // Update the Original Header variable
  setOriginalHeader(origHeader);
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int AngReader::readFile()
{
  setErrorCode(0);
  setErrorMessage("");
  QByteArray buf;
  setHeaderIsComplete(false);

  QFile in(getFileName());
  if(!in.open(QIODevice::ReadOnly | QIODevice::Text))
  {
    QString msg = QObject::tr("Ang file could not be opened: %1").arg(getFileName());
    setErrorCode(-100);
    setErrorMessage(msg);
    return -100;
  }

  QString origHeader;
  setOriginalHeader(origHeader);
  m_PhaseVector.clear();

  while(!in.atEnd() && !getHeaderIsComplete())
  {
    buf = in.readLine();
    if(buf.at(0) != '#')
    {
      setHeaderIsComplete(true);
    }
    else
    {
      origHeader.append(buf);
      parseHeaderLine(buf);
    }
  }
  // Update the Original Header variable
  setOriginalHeader(origHeader);
  if(getErrorCode() < 0)
  {
    return getErrorCode();
  }

  if(getXStep() == 0.0 || getYStep() == 0.0f)
  {
    QString msg = QString("Either the X Step or Y Step was Zero (0.0) and this is not allowed");
    setErrorCode(-110);
    setErrorMessage(msg);
    return -110;
  }
  if(m_PhaseVector.empty())
  {
    setErrorCode(-150);
    setErrorMessage("No phase was parsed in the header portion of the file. This possibly means that part of the header is missing.");
    return -150;
  }
  // We need to pass in the buffer because it has the first line of data
  readData(in, buf);
  if(getErrorCode() < 0)
  {
    return getErrorCode();
  }

  return getErrorCode();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AngReader::readData(QFile& in, QByteArray& buf)
{
  QString streamBuf;
  QTextStream ss(&streamBuf);

  size_t totalDataPoints = 0;

  QString grid = getGrid();

  int nOddCols = getNumOddCols();
  int nEvenCols = getNumEvenCols();
  int numRows = getNumRows();

  if(numRows < 1)
  {
    setErrorCode(-200);
    setErrorMessage("NumRows Sanity Check not correct. Check the entry for NROWS in the .ang file");
    return;
  }
  if(grid.startsWith(Ebsd::Ang::SquareGrid))
  {
    if(nOddCols > 0)
    {
      totalDataPoints = numRows * nOddCols; /* xCells = nOddCols;*/
    }
    else if(nEvenCols > 0)
    {
      totalDataPoints = numRows * nEvenCols; /* xCells = nEvexCells; */
    }
    else
    {
      totalDataPoints = 0;
    }
  }
  else if(grid.startsWith(Ebsd::Ang::HexGrid) && !m_ReadHexGrid)
  {
    setErrorCode(-400);
    setErrorMessage("Ang Files with Hex Grids Are NOT currently supported - Try converting them to Square Grid with the Hex2Sqr Converter filter.");
    return;
  }
  else if(grid.startsWith(Ebsd::Ang::HexGrid) && m_ReadHexGrid)
  {
    bool evenRow = false;
    totalDataPoints = 0;
    for(int r = 0; r < numRows; r++)
    {
      if(evenRow)
      {
        totalDataPoints = totalDataPoints + nEvenCols;
        evenRow = false;
      }
      else
      {
        totalDataPoints = totalDataPoints + nOddCols;
        evenRow = true;
      }
    }
  }
  else // Grid was not set
  {
    setErrorMessage("Ang file is missing the 'GRID' header entry.");
    setErrorCode(-300);
    return;
  }

  // Initialize all the pointers and allocate memory
  setNumberOfElements(totalDataPoints);
  size_t numBytes = totalDataPoints * sizeof(float);
  m_Phi1 = allocateArray<float>(totalDataPoints);
  m_Phi = allocateArray<float>(totalDataPoints);
  m_Phi2 = allocateArray<float>(totalDataPoints);
  m_Iq = allocateArray<float>(totalDataPoints);
  m_Ci = allocateArray<float>(totalDataPoints);
  m_PhaseData = allocateArray<int>(totalDataPoints);
  m_X = allocateArray<float>(totalDataPoints);
  m_Y = allocateArray<float>(totalDataPoints);
  m_SEMSignal = allocateArray<float>(totalDataPoints);
  m_Fit = allocateArray<float>(totalDataPoints);

  ::memset(m_Phi1, 0, numBytes);
  ::memset(m_Phi, 0, numBytes);
  ::memset(m_Phi2, 0, numBytes);
  ::memset(m_Iq, 0, numBytes);
  ::memset(m_Ci, 0, numBytes);
  ::memset(m_PhaseData, 0, numBytes);
  ::memset(m_X, 0, numBytes);
  ::memset(m_Y, 0, numBytes);
  ::memset(m_SEMSignal, 0, numBytes);
  ::memset(m_Fit, 0, numBytes);

  if(nullptr == m_Phi1 || nullptr == m_Phi || nullptr == m_Phi2 || nullptr == m_Iq || nullptr == m_SEMSignal || nullptr == m_Ci || nullptr == m_PhaseData || m_X == nullptr || m_Y == nullptr)
  {
    ss.string()->clear();
    ss << "Internal pointers were nullptr at " << __FILE__ << "(" << __LINE__ << ")\n";
    setErrorMessage(*(ss.string()));
    setErrorCode(-2500);
    return;
  }

  size_t counter = 1; // Because we are on the first line now.

  bool onEvenRow = false;
  int col = 0;

  int yChange = 0;
  float oldY = m_Y[0];
  int nxOdd = 0;
  int nxEven = 0;
  // int nRows = 0;

  for(size_t i = 0; i < totalDataPoints; ++i)
  {
    if(i > 0)
    {
      //  ::memset(buf, 0, bufSize); // Clear the buffer
      buf = in.readLine();
      ++counter;
    }
    parseDataLine(buf, i);
    if(getErrorCode() < 0)
    {
      ss.string()->clear();

      ss << "Error parsing the data line (Numeric conversion). Error code is " << getErrorCode() << " and occurred at data column " << m_ErrorColumn << " (Zero Based)\n"
         << buf << "\n*** Header information ***\nRows=" << numRows << " EvenCols=" << nEvenCols << " OddCols=" << nOddCols << "  Calculated Data Points: " << totalDataPoints
         << "\n***Parsing Position ***\nCurrent Row: " << yChange << "  Current Column Index: " << col << "  Current Data Point Count: " << counter << "\n";
      setErrorMessage(*(ss.string()));
      break;
    }

    if(fabs(m_Y[i] - oldY) > 1e-6)
    {
      ++yChange;
      oldY = m_Y[i];
      onEvenRow = !onEvenRow;
      col = 0;
    }
    else
    {
      col++;
    }
    if(yChange == 0)
    {
      ++nxOdd;
    }
    if(yChange == 1)
    {
      ++nxEven;
    }
    if(in.atEnd())
    {
      break;
    }
  }

#if 0
  nRows = yChange + 1;

  std::cout << "Header: nRows: " << numRows << " Odd Cols: " << nOddCols << "  Even Cols: " << nEvenCols << std::endl;
  std::cout << "File:   nRows: " << nRows << " Odd Cols: " << nxOdd << "  Even Cols: " << nxEven << std::endl;
#endif

  if(getNumFeatures() < 10)
  {
    deallocateArrayData<float>(m_Fit);
  }
  if(getNumFeatures() < 9)
  {
    deallocateArrayData<float>(m_SEMSignal);
  }
  if(getErrorCode() < 0)
  {
    return;
  }

  if(counter != totalDataPoints && in.atEnd())
  {
    ss.string()->clear();

    ss << "End of ANG file reached before all data was parsed.\n"
       << getFileName() << "\n*** Header information ***\nRows=" << numRows << " EvenCols=" << nEvenCols << " OddCols=" << nOddCols << "  Calculated Data Points: " << totalDataPoints
       << "\n***Parsing Position ***\nCurrent Row: " << yChange << "  Current Column Index: " << col << "  Current Data Point Count: " << counter << "\n";
    setErrorMessage(*(ss.string()));
    setErrorCode(-600);
  }
}

// -----------------------------------------------------------------------------
//  Read the Header part of the ANG file
// -----------------------------------------------------------------------------
void AngReader::parseHeaderLine(QByteArray& buf)
{
  bool ok = false;

  // Check to see if we are reading a header or data line.
  if(buf[0] != '#')
  {
    setHeaderIsComplete(true);
    return;
  }

  buf = buf.mid(1);       // remove the '#' charater
  buf = buf.trimmed();    // remove leading/trailing white space
  buf = buf.simplified(); // remove multiple white space characters internal to the array

  // now split the array based on spaces
  QList<QByteArray> tokens = buf.split(' ');
  QString word(tokens.at(0));
  if(word.lastIndexOf(':') > 0)
  {
    word.chop(1);
  }

  // If the word is "Phase" then we need to construct a "Phase" class and
  // store all the meta data for the phase into that class. When we are done
  // parsing data for the phase then stick the Phase instance into the header
  // map or stick it into a vector<Phase::Pointer> and stick the vector into
  // the map under the "Phase" key
  if(word.compare(Ebsd::Ang::Phase) == 0)
  {
    m_CurrentPhase = AngPhase::New();
    m_CurrentPhase->setPhaseIndex(tokens.at(1).toInt(&ok, 10));
    // Parsing the phase is complete, now add it to the vector of Phases
    m_PhaseVector.push_back(m_CurrentPhase);
  }
  else if(word.compare(Ebsd::Ang::MaterialName) == 0 && m_CurrentPhase.get() != nullptr)
  {
    if(tokens.size() > 1)
    {
      m_CurrentPhase->parseMaterialName(tokens);
    }
  }
  else if(word.compare(Ebsd::Ang::Formula) == 0 && m_CurrentPhase.get() != nullptr)
  {
    if(tokens.size() > 1)
    {
      m_CurrentPhase->parseFormula(tokens);
    }
  }
  else if(word.compare(Ebsd::Ang::Info) == 0 && m_CurrentPhase.get() != nullptr)
  {
    if(tokens.size() > 1)
    {
      m_CurrentPhase->parseInfo(tokens);
    }
  }
  else if(word.compare(Ebsd::Ang::Symmetry) == 0 && m_CurrentPhase.get() != nullptr)
  {
    if(tokens.size() > 1)
    {
      m_CurrentPhase->setSymmetry(tokens.at(1).toUInt(&ok, 10));
    }
  }
  else if(word.compare(Ebsd::Ang::LatticeConstants) == 0 && m_CurrentPhase.get() != nullptr)
  {
    if(tokens.size() > 1)
    {
      m_CurrentPhase->parseLatticeConstants(tokens);
    }
  }
  else if(word.compare(Ebsd::Ang::NumberFamilies) == 0 && m_CurrentPhase.get() != nullptr)
  {
    if(tokens.size() > 1)
    {
      m_CurrentPhase->setNumberFamilies(tokens.at(1).toInt(&ok, 10));
    }
  }
  else if(word.compare(Ebsd::Ang::HKLFamilies) == 0 && m_CurrentPhase.get() != nullptr)
  {
    if(tokens.size() > 1)
    {
      m_CurrentPhase->parseHKLFamilies(tokens);
    }
  }
  else if(word.startsWith(Ebsd::Ang::Categories) && m_CurrentPhase.get() != nullptr)
  {
    if(tokens.size() > 1)
    {
      m_CurrentPhase->parseCategories(tokens);
    }
  }
  else
  {
    EbsdHeaderEntry::Pointer p = m_HeaderMap[word];
    if(nullptr == p.get())
    {
/*
std::cout << "---------------------------" << std::endl;
std::cout << "Could not find header entry for key'" << word << "'" << std::endl;
QString upper(word);
std::transform(upper.begin(), upper.end(), upper.begin(), ::toupper);
std::cout << "#define ANG_" << upper << "     \"" << word << "\"" << std::endl;
std::cout << "const QString " << word << "(ANG_" << upper << ");" << std::endl;

std::cout << "angInstanceProperty(AngHeaderEntry<float>. float, " << word << "Ebsd::Ang::" << word << std::endl;
std::cout << "m_HeaderMap[Ebsd::Ang::" << word << "] = AngHeaderEntry<float>::NewEbsdHeaderEntry(Ebsd::Ang::" << word << ");" << std::endl;
*/
#if 0
      std::cout << "<tr>\n    <td>" << word << "</td>\n    <td>" << "H5T_STRING" << "</td>\n";
      std::cout << "    <td colspan=\"2\"> Contains value for the header entry " << word << "</td>\n</tr>" << std::endl;
#endif
      return;
    }
    if(tokens.size() > 1)
    {
      p->parseValue(tokens[1]);
#if 0
      std::cout << "<tr>\n    <td>" << p->getKey() << "</td>\n    <td>" << p->getHDFType() << "</td>\n";
      std::cout << "    <td colspan=\"2\"> Contains value for the header entry " << p->getKey() << "</td>\n</tr>" << std::endl;
#endif
    }
  }
}

// -----------------------------------------------------------------------------
//  Read the data part of the ANG file
// -----------------------------------------------------------------------------
void AngReader::parseDataLine(QByteArray& line, size_t i)
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
   * Some TSL ang files do NOT have all 10 columns. Assume these are lacking the last
   * 2 columns and all the other columns are the same as above.
   */
  m_ErrorColumn = 0;
  float p1 = 0.0f, p = 0.0f, p2 = 0.0f, x = -1.0f, y = -1.0f, iqual = -1.0f, conf = -1.0f, semSignal = -1.0f, fit = -1.0f;
  int ph = 0;
  size_t offset = 0;
  QList<QByteArray> tokens = line.trimmed().simplified().split(' ');
  bool ok = true;
  offset = i;
  if(!tokens.empty())
  {
    p1 = tokens[0].toFloat(&ok);
    if(!ok)
    {
      setErrorCode(-2501);
      m_ErrorColumn = 0;
    }
    m_Phi1[offset] = p1;
  }
  if(tokens.size() >= 2)
  {
    p = tokens[1].toFloat(&ok);
    if(!ok)
    {
      setErrorCode(-2502);
      m_ErrorColumn = 1;
    }
    m_Phi[offset] = p;
  }
  if(tokens.size() >= 3)
  {
    p2 = tokens[2].toFloat(&ok);
    if(!ok)
    {
      setErrorCode(-2503);
      m_ErrorColumn = 2;
    }
    m_Phi2[offset] = p2;
  }
  if(tokens.size() >= 4)
  {
    x = tokens[3].toFloat(&ok);
    if(!ok)
    {
      setErrorCode(-2504);
      m_ErrorColumn = 3;
    }
    m_X[offset] = x;
  }
  if(tokens.size() >= 5)
  {
    y = tokens[4].toFloat(&ok);
    if(!ok)
    {
      setErrorCode(-2505);
      m_ErrorColumn = 4;
    }
    m_Y[offset] = y;
  }
  if(tokens.size() >= 6)
  {
    iqual = tokens[5].toFloat(&ok);
    if(!ok)
    {
      setErrorCode(-2506);
      m_ErrorColumn = 5;
    }
    m_Iq[offset] = iqual;
  }
  if(tokens.size() >= 7)
  {
    conf = tokens[6].toFloat(&ok);
    if(!ok)
    {
      setErrorCode(-2507);
      m_ErrorColumn = 6;
    }
    m_Ci[offset] = conf;
  }
  if(tokens.size() >= 8)
  {
    ph = tokens[7].toInt(&ok);
    if(!ok)
    { 
      setErrorCode(-2508);
      m_ErrorColumn = 7;
      // Some have floats instead of integers so lets try that.
      float f = tokens[7].toFloat(&ok);
      if(!ok)
      {
        setErrorCode(-2588);
        m_ErrorColumn = 7;
      }
      else
      {
        setErrorCode(0);
        ph = static_cast<int32_t>(f);
      }
    }
    m_PhaseData[offset] = ph;
  }

  if(tokens.size() >= 9)
  {
    semSignal = tokens[8].toFloat(&ok);
    if(!ok)
    {
      setErrorCode(-2509);
      m_ErrorColumn = 8;
    }
    m_SEMSignal[offset] = semSignal;
  }
  if(tokens.size() >= 10)
  {
    fit = tokens[9].toFloat(&ok);
    if(!ok)
    {
      setErrorCode(-2510);
      m_ErrorColumn = 9;
    }
    m_Fit[offset] = fit;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int AngReader::getXDimension()
{
  return getNumEvenCols();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AngReader::setXDimension(int xdim)
{
  setNumEvenCols(xdim);
  setNumOddCols(xdim);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int AngReader::getYDimension()
{
  return getNumRows();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AngReader::setYDimension(int ydim)
{
  setNumRows(ydim);
}
