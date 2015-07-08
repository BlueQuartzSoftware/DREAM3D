/* ============================================================================
* Copyright (c) 2009-2015 BlueQuartz Software, LLC
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





#include "CtfReader.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

#include "CtfPhase.h"
#include "EbsdLib/EbsdMacros.h"
#include "EbsdLib/EbsdMath.h"



//#define PI_OVER_2f       90.0f
//#define THREE_PI_OVER_2f 270.0f
//#define TWO_PIf          360.0f
//#define ONE_PIf          180.0f

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CtfReader::CtfReader() :
  EbsdReader(),
  m_SingleSliceRead(-1)
{

  // Initialize the map of header key to header value
  m_HeaderMap[Ebsd::Ctf::ChannelTextFile] = CtfStringHeaderEntry::NewEbsdHeaderEntry(Ebsd::Ctf::ChannelTextFile);
  m_HeaderMap[Ebsd::Ctf::Prj] = CtfStringHeaderEntry::NewEbsdHeaderEntry(Ebsd::Ctf::Prj);
  m_HeaderMap[Ebsd::Ctf::Author] = CtfStringHeaderEntry::NewEbsdHeaderEntry(Ebsd::Ctf::Author);
  m_HeaderMap[Ebsd::Ctf::JobMode] = CtfStringHeaderEntry::NewEbsdHeaderEntry(Ebsd::Ctf::JobMode);
  m_HeaderMap[Ebsd::Ctf::XCells] = CtfHeaderEntry<int>::NewEbsdHeaderEntry(Ebsd::Ctf::XCells);
  m_HeaderMap[Ebsd::Ctf::YCells] = CtfHeaderEntry<int>::NewEbsdHeaderEntry(Ebsd::Ctf::YCells);
  m_HeaderMap[Ebsd::Ctf::ZCells] = CtfHeaderEntry<int>::NewEbsdHeaderEntry(Ebsd::Ctf::ZCells);
  m_HeaderMap[Ebsd::Ctf::XStep] = CtfHeaderEntry<float>::NewEbsdHeaderEntry(Ebsd::Ctf::XStep);
  m_HeaderMap[Ebsd::Ctf::YStep] = CtfHeaderEntry<float>::NewEbsdHeaderEntry(Ebsd::Ctf::YStep);
  m_HeaderMap[Ebsd::Ctf::ZStep] = CtfHeaderEntry<float>::NewEbsdHeaderEntry(Ebsd::Ctf::ZStep);
  m_HeaderMap[Ebsd::Ctf::AcqE1] = CtfHeaderEntry<float>::NewEbsdHeaderEntry(Ebsd::Ctf::AcqE1);
  m_HeaderMap[Ebsd::Ctf::AcqE2] = CtfHeaderEntry<float>::NewEbsdHeaderEntry(Ebsd::Ctf::AcqE2);
  m_HeaderMap[Ebsd::Ctf::AcqE3] = CtfHeaderEntry<float>::NewEbsdHeaderEntry(Ebsd::Ctf::AcqE3);
  m_HeaderMap[Ebsd::Ctf::Euler] = CtfStringHeaderEntry::NewEbsdHeaderEntry(Ebsd::Ctf::Euler);
  m_HeaderMap[Ebsd::Ctf::Mag] = CtfHeaderEntry<int>::NewEbsdHeaderEntry(Ebsd::Ctf::Mag);
  m_HeaderMap[Ebsd::Ctf::Coverage] = CtfHeaderEntry<int>::NewEbsdHeaderEntry(Ebsd::Ctf::Coverage);
  m_HeaderMap[Ebsd::Ctf::Device] = CtfHeaderEntry<int>::NewEbsdHeaderEntry(Ebsd::Ctf::Device);
  m_HeaderMap[Ebsd::Ctf::KV] = CtfHeaderEntry<int>::NewEbsdHeaderEntry(Ebsd::Ctf::KV);
  m_HeaderMap[Ebsd::Ctf::TiltAngle] = CtfHeaderEntry<float>::NewEbsdHeaderEntry(Ebsd::Ctf::TiltAngle);
  m_HeaderMap[Ebsd::Ctf::TiltAxis] = CtfHeaderEntry<float>::NewEbsdHeaderEntry(Ebsd::Ctf::TiltAxis);
  m_HeaderMap[Ebsd::Ctf::NumPhases] = CtfHeaderEntry<int>::NewEbsdHeaderEntry(Ebsd::Ctf::NumPhases);

  setXCells(0);
  setYCells(0);
  setZCells(1);

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CtfReader::~CtfReader()
{
  deletePointers();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CtfReader::initPointers(size_t numElements)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CtfReader::deletePointers()
{
  // Clear out all the maps and vectors
//  m_NameIndexMap.clear();
//  m_DataParsers.clear();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CtfReader::setPointerByName(const QString& name, void* p)
{
  // First we need to see if the pointer already exists
  QMap<QString, DataParser::Pointer>::iterator iter = m_NamePointerMap.find(name);
  if (iter == m_NamePointerMap.end())
  {
    // Data does not exist in Map
    DataParser::Pointer dparser = getParser(name, NULL, getXCells() * getYCells());
    dparser->setVoidPointer(p);
    m_NamePointerMap[name] = dparser;
  }
  else
  {
    DataParser::Pointer dparser = m_NamePointerMap[name];
    void* ptr = dparser->getVoidPointer();
    deallocateArrayData(ptr);
    dparser->setVoidPointer(p);
  }

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void* CtfReader::getPointerByName(const QString& featureName)
{
  void* ptr = NULL;
  if(m_NamePointerMap.contains(featureName) == true)
  {
    ptr = m_NamePointerMap.value(featureName)->getVoidPointer();
  }
  return ptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Ebsd::NumType CtfReader::getPointerType(const QString& featureName)
{
  // std::cout << "featureName: " << featureName << std::endl;
  if (featureName.compare(Ebsd::Ctf::Phase) == 0) { return Ebsd::Int32;}
  if (featureName.compare(Ebsd::Ctf::X) == 0) { return Ebsd::Float;}
  if (featureName.compare(Ebsd::Ctf::Y) == 0) { return Ebsd::Float;}
  if (featureName.compare(Ebsd::Ctf::Z) == 0) { return Ebsd::Float;}
  if (featureName.compare(Ebsd::Ctf::Bands) == 0) { return Ebsd::Int32;}
  if (featureName.compare(Ebsd::Ctf::Error) == 0) { return Ebsd::Int32;}
  if (featureName.compare(Ebsd::Ctf::Euler1) == 0) { return Ebsd::Float;}
  if (featureName.compare(Ebsd::Ctf::Euler2) == 0) { return Ebsd::Float;}
  if (featureName.compare(Ebsd::Ctf::Euler3) == 0) { return Ebsd::Float;}
  if (featureName.compare(Ebsd::Ctf::MAD) == 0) { return Ebsd::Float;}
  if (featureName.compare(Ebsd::Ctf::BC) == 0) { return Ebsd::Int32;}
  if (featureName.compare(Ebsd::Ctf::BS) == 0) { return Ebsd::Int32;}
  if (featureName.compare(Ebsd::Ctf::GrainIndex) == 0) { return Ebsd::Int32;}
  if (featureName.compare(Ebsd::Ctf::GrainRandomColourR) == 0) { return Ebsd::Int32;}
  if (featureName.compare(Ebsd::Ctf::GrainRandomColourG) == 0) { return Ebsd::Int32;}
  if (featureName.compare(Ebsd::Ctf::GrainRandomColourB) == 0) { return Ebsd::Int32;}
  // std::cout << "THIS IS NOT GOOD. Featurename: " << featureName << " was not found in the list" << std::endl;
  return Ebsd::UnknownNumType;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int CtfReader::getTypeSize(const QString& featureName)
{
  if (featureName.compare(Ebsd::Ctf::Phase) == 0) { return 4;}
  if (featureName.compare(Ebsd::Ctf::X) == 0) { return 4;}
  if (featureName.compare(Ebsd::Ctf::Y) == 0) { return 4;}
  if (featureName.compare(Ebsd::Ctf::Z) == 0) { return 4;}
  if (featureName.compare(Ebsd::Ctf::Bands) == 0) { return 4;}
  if (featureName.compare(Ebsd::Ctf::Error) == 0) { return 4;}
  if (featureName.compare(Ebsd::Ctf::Euler1) == 0) { return 4;}
  if (featureName.compare(Ebsd::Ctf::Euler2) == 0) { return 4;}
  if (featureName.compare(Ebsd::Ctf::Euler3) == 0) { return 4;}
  if (featureName.compare(Ebsd::Ctf::MAD) == 0) { return 4;}
  if (featureName.compare(Ebsd::Ctf::BC) == 0) { return 4;}
  if (featureName.compare(Ebsd::Ctf::BS) == 0) { return 4;}
  if (featureName.compare(Ebsd::Ctf::GrainIndex) == 0) { return 4;}
  if (featureName.compare(Ebsd::Ctf::GrainRandomColourR) == 0) { return 4;}
  if (featureName.compare(Ebsd::Ctf::GrainRandomColourG) == 0) { return 4;}
  if (featureName.compare(Ebsd::Ctf::GrainRandomColourB) == 0) { return 4;}
  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataParser::Pointer CtfReader::getParser(const QString& featureName, void* ptr, size_t size)
{
  // These are defaulted to a "3D" CTF file with Feature IDS already determined and their colors
  if (featureName.compare(Ebsd::Ctf::Phase) == 0) { return Int32Parser::New(static_cast<int32_t*>(ptr), size, featureName, 0);}
  if (featureName.compare(Ebsd::Ctf::X) == 0) { return FloatParser::New(static_cast<float*>(ptr), size, featureName, 1);}
  if (featureName.compare(Ebsd::Ctf::Y) == 0) { return FloatParser::New(static_cast<float*>(ptr), size, featureName, 2);}
  if (featureName.compare(Ebsd::Ctf::Z) == 0) { return FloatParser::New(static_cast<float*>(ptr), size, featureName, 3);}
  if (featureName.compare(Ebsd::Ctf::Bands) == 0) { return Int32Parser::New(static_cast<int32_t*>(ptr), size, featureName, 4);}
  if (featureName.compare(Ebsd::Ctf::Error) == 0) { return Int32Parser::New(static_cast<int32_t*>(ptr), size, featureName, 5);}
  if (featureName.compare(Ebsd::Ctf::Euler1) == 0) { return FloatParser::New(static_cast<float*>(ptr), size, featureName, 6);}
  if (featureName.compare(Ebsd::Ctf::Euler2) == 0) { return FloatParser::New(static_cast<float*>(ptr), size, featureName, 7);}
  if (featureName.compare(Ebsd::Ctf::Euler3) == 0) { return FloatParser::New(static_cast<float*>(ptr), size, featureName, 8);}
  if (featureName.compare(Ebsd::Ctf::MAD) == 0) { return FloatParser::New(static_cast<float*>(ptr), size, featureName, 9);}
  if (featureName.compare(Ebsd::Ctf::BC) == 0) { return Int32Parser::New(static_cast<int32_t*>(ptr), size, featureName, 10);}
  if (featureName.compare(Ebsd::Ctf::BS) == 0) { return Int32Parser::New(static_cast<int32_t*>(ptr), size, featureName, 11);}
  if (featureName.compare(Ebsd::Ctf::GrainIndex) == 0) { return Int32Parser::New(static_cast<int32_t*>(ptr), size, featureName, 12);}
  if (featureName.compare(Ebsd::Ctf::GrainRandomColourR) == 0) { return Int32Parser::New(static_cast<int32_t*>(ptr), size, featureName, 13);}
  if (featureName.compare(Ebsd::Ctf::GrainRandomColourG) == 0) { return Int32Parser::New(static_cast<int32_t*>(ptr), size, featureName, 14);}
  if (featureName.compare(Ebsd::Ctf::GrainRandomColourB) == 0) { return Int32Parser::New(static_cast<int32_t*>(ptr), size, featureName, 15);}
  return DataParser::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int CtfReader::readHeaderOnly()
{
  int err = 1;
  QByteArray buf;
  QFile in(getFileName());
  setHeaderIsComplete(false);
  if (!in.open(QIODevice::ReadOnly | QIODevice::Text))
  {
    QString msg = QString("Ctf file could not be opened: ") + getFileName();
    setErrorCode(-100);
    setErrorMessage(msg);
    return -100;
  }

  QString origHeader;
  setOriginalHeader(origHeader);
  m_PhaseVector.clear();

  // Parse the header
  QList<QByteArray> headerLines;
  err = getHeaderLines(in, headerLines);
  err = parseHeaderLines(headerLines);
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int CtfReader::readFile()
{
  int err = 1;
  setErrorCode(0);
  setErrorMessage("");
  QByteArray buf;
  QFile in(getFileName());
  setHeaderIsComplete(false);
  if (!in.open(QIODevice::ReadOnly | QIODevice::Text))
  {
    QString msg = QString("Ctf file could not be opened: ") + getFileName();
    setErrorCode(-100);
    setErrorMessage(msg);
    return -100;
  }
  QString origHeader;
  setOriginalHeader(origHeader);
  m_PhaseVector.clear();

  // Parse the header
  QList<QByteArray> headerLines;
  err = getHeaderLines(in, headerLines);
  if (err < 0) { return err;}
  err = parseHeaderLines(headerLines);
  if (err < 0) { return err;}


  if (getXStep() == 0.0 || getYStep() == 0.0f )
  {
    setErrorMessage("Either the X Step or Y Step was Zero (0.0) which is NOT allowed. Please update the CTF file header with appropriate values.");
    return -102;
  }

  if (getXCells() == 0 || getYCells() == 0 )
  {
    setErrorMessage("Either the X Cells or Y Cells was Zero (0) which is NOT allowed. Please update the CTF file header with appropriate values.");
    return -103;
  }

  err = readData(in);

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CtfReader::readOnlySliceIndex(int slice)
{
  m_SingleSliceRead = slice;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int CtfReader::readData(QFile& in)
{
  // Delete any currently existing pointers
  deletePointers();
  QString sBuf;
  QTextStream ss(&sBuf);
  // Initialize new pointers
  size_t yCells = getYCells();
  size_t xCells = getXCells();
  int zCells = getZCells();
  int zStart = 0;
  int zEnd = zCells;
  if(zCells < 0 || m_SingleSliceRead >= 0)
  {
    zCells = 1;
  }
  size_t totalScanPoints = yCells * xCells * zCells;

  setNumberOfElements(totalScanPoints);

  QByteArray buf;

  // Read the column Headers and allocate the necessary arrays
  buf = in.readLine();
  buf = buf.trimmed(); // Remove leading and trailing whitespace

  QList<QByteArray> tokens = buf.split('\t'); // Tokenize the array with a tab

  Ebsd::NumType pType = Ebsd::UnknownNumType;
  qint32 size = tokens.size();
  bool didAllocate = false;
  for (qint32 i = 0; i < size; ++i)
  {
    QString name = QString::fromLatin1(tokens[i]);
    pType = getPointerType(name);
    if(Ebsd::Int32 == pType)
    {
      Int32Parser::Pointer dparser = Int32Parser::New(NULL, totalScanPoints, name, i);
      didAllocate = dparser->allocateArray(totalScanPoints);
      //Q_ASSERT_X(dparser->getVoidPointer() != NULL, __FILE__, "Could not allocate memory for Integer data in CTF File.");
      if( didAllocate == true)
      {
        ::memset(dparser->getVoidPointer(), 0xAB, sizeof(int32_t) * totalScanPoints);
        m_NamePointerMap.insert(name, dparser);
      }
    }
    else if(Ebsd::Float == pType)
    {
      FloatParser::Pointer dparser = FloatParser::New(NULL, totalScanPoints, name, i);
      didAllocate = dparser->allocateArray(totalScanPoints);
      //Q_ASSERT_X(dparser->getVoidPointer() != NULL, __FILE__, "Could not allocate memory for Integer data in CTF File.");
      if( didAllocate == true)
      {
        ::memset(dparser->getVoidPointer(), 0xAB, sizeof(float) * totalScanPoints);
        m_NamePointerMap.insert(name, dparser);
      }
    }
    else
    {
      ss.string()->clear();
      ss << "Column Header '" << tokens[i] << "' is not a recognized column for CTF Files. Please recheck your .ctf file and report this error to the DREAM3D developers.";
      setErrorMessage(ss.string());
      deletePointers();
      return -107;
    }


    if(didAllocate == false)
    {
      setErrorCode(-106);
      QString msg;
      QTextStream ss(&msg);
      ss << "The CTF reader could not allocate memory for the data. Check the header for the number of X, Y and Z Cells.";
      ss << "\n X Cells: " << getXCells();
      ss << "\n Y Cells: " << getYCells();
      ss << "\n Z Cells: " << getZCells();
      ss << "\n Total Scan Points: " << totalScanPoints;
      setErrorMessage(msg);
      return -106; // Could not allocate the memory
    }

  }

  // Now start reading the data line by line
  int err = 0;
  size_t counter = 0;
  for (int slice = zStart; slice < zEnd; ++slice)
  {
    for (size_t row = 0; row < yCells; ++row)
    {
      for (size_t col = 0; col < xCells; ++col)
      {
        buf = in.readLine(); // Read the line into a QByteArray including the newline
        buf = buf.trimmed(); // Remove leading and trailing whitespace

        if ( (m_SingleSliceRead < 0) || (m_SingleSliceRead >= 0 && slice == m_SingleSliceRead) )
        {

          if(in.atEnd() == true && buf.isEmpty() == true) // We have to have read to the end of the file AND the buffer is empty
            // otherwise we read EXACTLY the last line and we still need to parse the line.
          {
            //  ++counter; // We need to make sure this gets incremented before leaving
            break;
          }
          err = parseDataLine(buf, row, col, counter, xCells, yCells);
          if (err < 0) { return err; }
          ++counter;
        }

      }
      if(in.atEnd() == true)
      {
        break;
      }
    }
    //   std::cout << ".ctf Z Slice " << slice << " Reading complete." << std::endl;
    if(m_SingleSliceRead >= 0 && slice == m_SingleSliceRead)
    {
      break;
    }
  }

  if(counter != getNumberOfElements() && in.atEnd() == true)
  {
    ss.string()->clear();
    ss << "Premature End Of File reached.\n" << getFileName() << "\nNumRows=" << getNumberOfElements() << "\ncounter=" << counter
       << "\nTotal Data Points Read=" << counter << "\n";
    setErrorMessage(ss.string());
    setErrorCode(-105);
    return -105;
  }
  return 0;
}

#if 0
#define PRINT_HTML_TABLE_ROW(p)\
  std::cout << "<tr>\n    <td>" << p->getKey() << "</td>\n    <td>" << p->getHDFType() << "</td>\n";\
  std::cout << "    <td colspan=\"2\"> Contains value for the header entry " << p->getKey() << "</td>\n</tr>" << std::endl;
#else
#define PRINT_HTML_TABLE_ROW(p)
#endif

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int CtfReader::parseHeaderLines(QList<QByteArray>& headerLines)
{
  int err = 0;
  qint32 size = headerLines.size();
  for (qint32 i = 0; i < size; ++i)
  {
    QByteArray line = headerLines[i];
    QString sLine = line; // Turn the line into a QString
    QList<QByteArray> spcTokens = line.split(' '); // Space Delimit the line
    QList<QByteArray> tabTokens = line.split('\t'); // Tab Delimit the line

    if(line.startsWith("Prj"))  // This is a special case/bug in HKL's writing code. This line is space delimited
    {
      line = line.trimmed();
      line = line.simplified();

      QByteArray value = line.mid(4); // Get the value part of the line
      EbsdHeaderEntry::Pointer p = m_HeaderMap["Prj"];
      if(NULL == p.get())
      {
        std::cout << "---------------------------" << std::endl;
        std::cout << "Could not find header entry for key 'Prj'" << std::endl;
      }
      else
      {
        p->parseValue(value);
        PRINT_HTML_TABLE_ROW(p)
      }
    }
    else if(sLine.startsWith(Ebsd::Ctf::NumPhases) == true)
    {
      QByteArray key = tabTokens[0];
      EbsdHeaderEntry::Pointer p = m_HeaderMap[key];
      p->parseValue(tabTokens[1]);
      int nPhases = getNumPhases();
      // We start the Phase Index at "1" instead of Zero by convention
      for (int p = 1; p <= nPhases; ++p)
      {
        ++i; // Increment the outer loop
        line = headerLines[i];
        CtfPhase::Pointer phase = CtfPhase::New();
        phase->setPhaseIndex(p);
        phase->parsePhase(line); // All the phase information is on a single line

        m_PhaseVector.push_back(phase);
      }
      ++i;
    }
    else if(sLine.startsWith("Euler angles refer to Sample Coordinate system (CS0)!") == true)
    {
      // We parse out lots of stuff from this one line
      //Mag
      EbsdHeaderEntry::Pointer p0 = m_HeaderMap[tabTokens[1]];
      p0->parseValue(tabTokens[2]);
      PRINT_HTML_TABLE_ROW(p0);
      //Coverage
      EbsdHeaderEntry::Pointer p1 = m_HeaderMap[tabTokens[3]];
      p1->parseValue(tabTokens[4]);
      PRINT_HTML_TABLE_ROW(p1);
      //Device
      EbsdHeaderEntry::Pointer p2 = m_HeaderMap[tabTokens[5]];
      p2->parseValue(tabTokens[6]);
      PRINT_HTML_TABLE_ROW(p2);
      //KV
      EbsdHeaderEntry::Pointer p3 = m_HeaderMap[tabTokens[7]];
      p3->parseValue(tabTokens[8]);
      PRINT_HTML_TABLE_ROW(p3);
      //TiltAngle
      EbsdHeaderEntry::Pointer p4 = m_HeaderMap[tabTokens[9]];
      p4->parseValue(tabTokens[10]);
      PRINT_HTML_TABLE_ROW(p4);
      //TiltAxis
      EbsdHeaderEntry::Pointer p5 = m_HeaderMap[tabTokens[11]];
      p5->parseValue(tabTokens[12]);
      PRINT_HTML_TABLE_ROW(p5);
    }
    else if(sLine.startsWith("Channel Text File") == true || sLine.startsWith(":Channel Text File") == true)
    {
      // We do not really do anything with this entry
    }
    else // This is the generic Catch all
    {
      EbsdHeaderEntry::Pointer p = m_HeaderMap[tabTokens[0]];
      if(NULL == p.get())
      {
        std::cout << "---------------------------" << std::endl;
        std::cout << "Could not find header entry for key '" << line[0] << "'" << std::endl;
        //        QString upper(line[0]);
        //        std::transform(upper.begin(), upper.end(), upper.begin(), ::toupper);
        //        std::cout << "#define ANG_" << upper << "     \"" << line[0] << "\"" << std::endl;
        //        std::cout << "const QString " << line[0] << "(ANG_" << upper << ");" << std::endl;
        //        std::cout << "angInstanceProperty(AngHeaderEntry<float>. float, " << line[0] << "Ebsd::Ctf::" << line[0] << std::endl;
        //        std::cout << "m_Headermap[Ebsd::Ctf::" << line[0] << "] = AngHeaderEntry<float>::NewEbsdHeaderEntry(Ebsd::Ctf::" << line[0] << ");" << std::endl;
      }
      else
      {
        if (line.size() > 1)
        {
          p->parseValue(tabTokens[1]);
          PRINT_HTML_TABLE_ROW(p)
        }
      }
    }

  }
  return err;
}



// -----------------------------------------------------------------------------
//  Read the data part of the .ctf file
// -----------------------------------------------------------------------------
int CtfReader::parseDataLine(QByteArray& line, size_t row, size_t col, size_t offset, size_t xCells, size_t yCells )
{
  /* When reading the data there should be at least 11 cols of data.
   */
  //  float x, y,  p1, p, p2, mad;
  //  int phase, bCount, error, bc, bs;
  //  size_t offset = i;

  // Filter the line to convert European command style decimals to US/UK style points
  //  QVector<char> cLine(line.size()+1);
  //  ::memcpy( &(cLine.front()), line.c_str(), line.size() + 1);
  for (int c = 0; c < line.size(); ++c)
  {
    if(line.at(c) == ',')
    {
      line[c] = '.';
    }
  }

  QList<QByteArray> tokens = line.split('\t');
  if(tokens.size() != m_NamePointerMap.size())
  {
    setErrorCode(-107);
    QString msg;
    QTextStream ss(&msg);
    ss << "The number of tab delimited data columns (" << tokens.size() << ") does not match the number of tab delimited header columns (";
    ss << m_NamePointerMap.size() << "). Please check the CTF file for mistakes.";
    ss << "The error occurred at data row " << row << " which is " << row << " past ";
    ss << "the column header row.";
    ss << "\nThe CTF Reader will now abort reading any further in the file.";

    setErrorMessage(msg);
    return -106; // Could not allocate the memory
  }
  QMapIterator<QString, DataParser::Pointer> iter(m_NamePointerMap);
  while (iter.hasNext())
  {
    iter.next();

    DataParser::Pointer dparser = iter.value();
    dparser->parse(tokens[dparser->getColumnIndex()], offset);
  }
  return 0;
}

#if 0
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<QString> CtfReader::tokenize(char* buf, char delimiter)
{
  QVector<QString> output;
  QString values(buf);
  QString::size_type start = 0;
  QString::size_type pos = 0;
  //  std::cout << "-----------------------------" << std::endl;
  while(pos != QString::npos && pos != values.size() - 1)
  {
    pos = values.find(delimiter, start);
    output.push_back(values.substr(start, pos - start));
    //   std::cout << "Adding: " << output.back() << std::endl;
    if (pos != QString::npos)
    {
      start = pos + 1;
    }
  }
  return output;
}
#endif


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int CtfReader::getHeaderLines(QFile& reader, QList<QByteArray>& headerLines)
{
  int err = 0;
  QByteArray buf;
  bool ok = false;
  int numPhases = -1;
  while (!reader.atEnd() && false == getHeaderIsComplete())
  {
    buf = reader.readLine();
    // Append the line to the complete header
    appendOriginalHeader(QString(buf));

    // remove the newline at the end of the line
    buf.chop(1);
    headerLines.push_back(buf);
    if (buf.startsWith("Phases") == true)
    {
      QList<QByteArray> tokens = buf.split('\t');
      numPhases = tokens.at(1).toInt(&ok, 10);
      break; //
    }
  }
  // Now read the phases line
  for(int p = 0; p < numPhases; ++p)
  {
    buf = reader.readLine();
    appendOriginalHeader(QString(buf));

    // remove the newline at the end of the line
    buf.chop(1);
    headerLines.push_back(buf);
  }
  setHeaderIsComplete(true);
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool CtfReader::isDataHeaderLine(QVector<QString>& columns)
{
  if (columns.size() != 11)
  { return false; }
  if (columns[0].compare("Phase") != 0)
  { return false; }
  if (columns[9].compare("BC") != 0)
  { return false; }

  return true;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int CtfReader::getXDimension()
{
  return getXCells();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CtfReader::setXDimension(int xdim)
{
  setXCells(xdim);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int CtfReader::getYDimension()
{
  return getYCells();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CtfReader::setYDimension(int ydim)
{
  setYCells(ydim);
}

#define CTF_SHUFFLE_ARRAY(tempPtr, var, m_msgType, numRows)\
  for (size_t i = 0; i < numRows; ++i) {\
    size_t nIdx = shuffleTable[i];\
    tempPtr[nIdx] = var[i];\
  }\
  /* Copy the values back into the array over writing the original values*/\
  ::memcpy(var, tempPtr, numRows * sizeof(m_msgType));


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QList<QString> CtfReader::getColumnNames()
{
  return m_NamePointerMap.keys();
}

#define CTF_PRINT_QSTRING(var, out)\
  out << #var << ": " << get##var().toStdString() << std::endl;

#define CTF_PRINT_HEADER_VALUE(var, out)\
  out << #var << ": " << get##var() << std::endl;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CtfReader::printHeader(std::ostream& out)
{
  std::cout << "-------------------- CtfReader Header Values --------------------" << std::endl;
  CTF_PRINT_QSTRING(Channel, out);
  CTF_PRINT_QSTRING(Prj, out);
  CTF_PRINT_QSTRING(Author, out);
  CTF_PRINT_QSTRING(JobMode, out);
  CTF_PRINT_HEADER_VALUE(XCells, out);
  CTF_PRINT_HEADER_VALUE(YCells, out);
  CTF_PRINT_HEADER_VALUE(XStep, out);
  CTF_PRINT_HEADER_VALUE(YStep, out);
  CTF_PRINT_HEADER_VALUE(AcqE1, out);
  CTF_PRINT_HEADER_VALUE(AcqE2, out);
  CTF_PRINT_HEADER_VALUE(AcqE3, out);
  CTF_PRINT_QSTRING(Euler, out);
  CTF_PRINT_HEADER_VALUE(Mag, out);
  CTF_PRINT_HEADER_VALUE(Coverage, out);
  CTF_PRINT_HEADER_VALUE(Device, out);
  CTF_PRINT_HEADER_VALUE(KV, out);
  CTF_PRINT_HEADER_VALUE(TiltAngle, out);
  CTF_PRINT_HEADER_VALUE(TiltAxis, out);
  CTF_PRINT_HEADER_VALUE(NumPhases, out);
  int nPhases = getNumPhases();
  for (int p = 0; p < nPhases; ++p)
  {
    out << "### Phase " << p << std::endl;
    m_PhaseVector[p]->printSelf(out);
  }

  std::cout << "----------------------------------------" << std::endl;
}

