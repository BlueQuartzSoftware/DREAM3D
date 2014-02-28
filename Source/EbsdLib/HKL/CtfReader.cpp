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
  typedef std::map<std::string, void*> NamePointerMapType;

  for (NamePointerMapType::iterator iter = m_NamePointerMap.begin(); iter != m_NamePointerMap.end(); ++iter )
  {
    Ebsd::NumType pType = getPointerType( (*iter).first );
    if (Ebsd::Int32 == pType)
    {
      int32_t* ptr = static_cast<int32_t*>((*iter).second);
      this->deallocateArrayData<int32_t>(ptr);
    }
    else if (Ebsd::Float == pType)
    {
      float* ptr = static_cast<float*>((*iter).second);
      this->deallocateArrayData<float>(ptr);
    }
  }

  // Clear out all the maps and vectors
  m_NamePointerMap.clear();
  m_NameIndexMap.clear();
  m_ColumnData.clear();
  m_DataParsers.clear();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CtfReader::setPointerByName(const std::string &name, void* p)
{
  // First we need to see if the pointer already exists
  std::map<std::string, int>::iterator iter = m_NameIndexMap.find(name);
  if (iter == m_NameIndexMap.end())
  {
    // Data does not exist in Map
    size_t i = m_ColumnData.size();
    m_ColumnData.push_back(p);
    m_NameIndexMap[name] = static_cast<int>(i);
    m_NamePointerMap[name] = m_ColumnData[i];
    if (m_DataParsers.size() <= i)
    {
      m_DataParsers.resize(i+1);
    }
    m_DataParsers[i] = getParser(name, m_ColumnData[i], getXCells()*getYCells());
  }
  else
  {
    void* ptr = m_NamePointerMap[name];
    deallocateArrayData(ptr);
    m_NamePointerMap[name] = p;
    int index = m_NameIndexMap[name];
    m_ColumnData[index] = p;
    m_DataParsers[index] = getParser(name, m_ColumnData[index], getXCells()*getYCells());
  }

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void* CtfReader::getPointerByName(const std::string &fieldName)
{
  void* ptr = NULL;
  std::map<std::string, void*>::iterator iter = m_NamePointerMap.find(fieldName);
  if(iter != m_NamePointerMap.end())
  {
    ptr = (*iter).second;
    return ptr;
  }
  return ptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Ebsd::NumType CtfReader::getPointerType(const std::string &fieldName)
{
 // std::cout << "fieldName: " << fieldName << std::endl;
  if (fieldName.compare(Ebsd::Ctf::Phase) == 0) { return Ebsd::Int32;}
  if (fieldName.compare(Ebsd::Ctf::X) == 0) { return Ebsd::Float;}
  if (fieldName.compare(Ebsd::Ctf::Y) == 0) { return Ebsd::Float;}
  if (fieldName.compare(Ebsd::Ctf::Z) == 0) { return Ebsd::Float;}
  if (fieldName.compare(Ebsd::Ctf::Bands) == 0) { return Ebsd::Int32;}
  if (fieldName.compare(Ebsd::Ctf::Error) == 0) { return Ebsd::Int32;}
  if (fieldName.compare(Ebsd::Ctf::Euler1) == 0) { return Ebsd::Float;}
  if (fieldName.compare(Ebsd::Ctf::Euler2) == 0) { return Ebsd::Float;}
  if (fieldName.compare(Ebsd::Ctf::Euler3) == 0) { return Ebsd::Float;}
  if (fieldName.compare(Ebsd::Ctf::MAD) == 0) { return Ebsd::Float;}
  if (fieldName.compare(Ebsd::Ctf::BC) == 0) { return Ebsd::Int32;}
  if (fieldName.compare(Ebsd::Ctf::BS) == 0) { return Ebsd::Int32;}
  if (fieldName.compare(Ebsd::Ctf::GrainIndex) == 0) { return Ebsd::Int32;}
  if (fieldName.compare(Ebsd::Ctf::GrainRandomColourR) == 0) { return Ebsd::Int32;}
  if (fieldName.compare(Ebsd::Ctf::GrainRandomColourG) == 0) { return Ebsd::Int32;}
  if (fieldName.compare(Ebsd::Ctf::GrainRandomColourB) == 0) { return Ebsd::Int32;}
 // std::cout << "THIS IS NOT GOOD. Fieldname: " << fieldName << " was not found in the list" << std::endl;
  return Ebsd::UnknownNumType;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int CtfReader::getTypeSize(const std::string &fieldName)
{
  if (fieldName.compare(Ebsd::Ctf::Phase) == 0) { return 4;}
  if (fieldName.compare(Ebsd::Ctf::X) == 0) { return 4;}
  if (fieldName.compare(Ebsd::Ctf::Y) == 0) { return 4;}
  if (fieldName.compare(Ebsd::Ctf::Z) == 0) { return 4;}
  if (fieldName.compare(Ebsd::Ctf::Bands) == 0) { return 4;}
  if (fieldName.compare(Ebsd::Ctf::Error) == 0) { return 4;}
  if (fieldName.compare(Ebsd::Ctf::Euler1) == 0) { return 4;}
  if (fieldName.compare(Ebsd::Ctf::Euler2) == 0) { return 4;}
  if (fieldName.compare(Ebsd::Ctf::Euler3) == 0) { return 4;}
  if (fieldName.compare(Ebsd::Ctf::MAD) == 0) { return 4;}
  if (fieldName.compare(Ebsd::Ctf::BC) == 0) { return 4;}
  if (fieldName.compare(Ebsd::Ctf::BS) == 0) { return 4;}
  if (fieldName.compare(Ebsd::Ctf::GrainIndex) == 0) { return 4;}
  if (fieldName.compare(Ebsd::Ctf::GrainRandomColourR) == 0) { return 4;}
  if (fieldName.compare(Ebsd::Ctf::GrainRandomColourG) == 0) { return 4;}
  if (fieldName.compare(Ebsd::Ctf::GrainRandomColourB) == 0) { return 4;}
  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataParser::Pointer CtfReader::getParser(const std::string &fieldName, void* ptr, size_t size)
{
  if (fieldName.compare(Ebsd::Ctf::Phase) == 0) { return Int32Parser::New(static_cast<int32_t*>(ptr), size, fieldName);}
  if (fieldName.compare(Ebsd::Ctf::X) == 0) { return FloatParser::New(static_cast<float*>(ptr), size, fieldName);}
  if (fieldName.compare(Ebsd::Ctf::Y) == 0) { return FloatParser::New(static_cast<float*>(ptr), size, fieldName);}
  if (fieldName.compare(Ebsd::Ctf::Z) == 0) { return FloatParser::New(static_cast<float*>(ptr), size, fieldName);}
  if (fieldName.compare(Ebsd::Ctf::Bands) == 0) { return Int32Parser::New(static_cast<int32_t*>(ptr), size, fieldName);}
  if (fieldName.compare(Ebsd::Ctf::Error) == 0) { return Int32Parser::New(static_cast<int32_t*>(ptr), size, fieldName);}
  if (fieldName.compare(Ebsd::Ctf::Euler1) == 0) { return FloatParser::New(static_cast<float*>(ptr), size, fieldName);}
  if (fieldName.compare(Ebsd::Ctf::Euler2) == 0) { return FloatParser::New(static_cast<float*>(ptr), size, fieldName);}
  if (fieldName.compare(Ebsd::Ctf::Euler3) == 0) { return FloatParser::New(static_cast<float*>(ptr), size, fieldName);}
  if (fieldName.compare(Ebsd::Ctf::MAD) == 0) { return FloatParser::New(static_cast<float*>(ptr), size, fieldName);}
  if (fieldName.compare(Ebsd::Ctf::BC) == 0) { return Int32Parser::New(static_cast<int32_t*>(ptr), size, fieldName);}
  if (fieldName.compare(Ebsd::Ctf::BS) == 0) { return Int32Parser::New(static_cast<int32_t*>(ptr), size, fieldName);}
  if (fieldName.compare(Ebsd::Ctf::GrainIndex) == 0) { return Int32Parser::New(static_cast<int32_t*>(ptr), size, fieldName);}
  if (fieldName.compare(Ebsd::Ctf::GrainRandomColourR) == 0) { return Int32Parser::New(static_cast<int32_t*>(ptr), size, fieldName);}
  if (fieldName.compare(Ebsd::Ctf::GrainRandomColourG) == 0) { return Int32Parser::New(static_cast<int32_t*>(ptr), size, fieldName);}
  if (fieldName.compare(Ebsd::Ctf::GrainRandomColourB) == 0) { return Int32Parser::New(static_cast<int32_t*>(ptr), size, fieldName);}
  return DataParser::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int CtfReader::readHeaderOnly()
{
  int err = 1;

  setHeaderIsComplete(false);
  std::ifstream in(getFileName().c_str());

  if (!in.is_open())
  {
    std::cout << "ctf file could not be opened: " << getFileName() << std::endl;
    return -100;
  }

  std::string origHeader;
  setOriginalHeader(origHeader);
  m_PhaseVector.clear();

  // Parse the header
  std::vector<std::vector<std::string> > headerLines;
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

  setHeaderIsComplete(false);
  std::ifstream in(getFileName().c_str());

  if (!in.is_open())
  {
    std::cout << "ctf file could not be opened: " << getFileName() << std::endl;
    return -100;
  }

  std::string origHeader;
  setOriginalHeader(origHeader);
  m_PhaseVector.clear();

  // Parse the header
  std::vector<std::vector<std::string> > headerLines;
  err = getHeaderLines(in, headerLines);
  if (err < 0) { return err;}
  err = parseHeaderLines(headerLines);
  if (err < 0) { return err;}


  if (getXStep() == 0.0 || getYStep() == 0.0f )
  {
    setErrorMessage("Either the X Step or Y Step was Zero (0.0) which is NOT allowed. Please update the CTF file header with appropriate values.");
    return -102;
  }

  err = readData(in);
  if (err < 0) {
    return err;
  }

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
int CtfReader::readData(std::ifstream &in)
{
  // Delete any currently existing pointers
  deletePointers();
  std::stringstream ss;
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
  size_t totalDataRows = yCells * xCells * zCells;

  setNumberOfElements(totalDataRows);

  char buf[kBufferSize];
  ::memset(buf, 0, kBufferSize);
  in.getline(buf, kBufferSize);
  // over write the newline at the end of the line with a NULL character
  uint32_t idx = 0;
  while (idx < kBufferSize && buf[idx] != 0 ) { ++idx; }
  if(buf[idx - 1] < 32) { buf[idx - 1] = 0; }

  std::vector<std::string> tokens = tokenize(buf, '\t');

  m_ColumnData.resize(tokens.size());
  m_DataParsers.resize(tokens.size());

  Ebsd::NumType pType = Ebsd::UnknownNumType;
  size_t size = tokens.size();
  for (size_t i = 0; i < size; ++i)
  {
    pType = getPointerType(tokens[i]);
    if(Ebsd::Int32 == pType)
    {
      m_ColumnData[i] = static_cast<void*>(allocateArray<int>(totalDataRows));
      ::memset(m_ColumnData[i], 0xAB, sizeof(int) * totalDataRows);
    }
    else if(Ebsd::Float == pType)
    {
      m_ColumnData[i] = static_cast<void*>(allocateArray<float>(totalDataRows));
      ::memset(m_ColumnData[i], 0xFF, sizeof(float) * totalDataRows);
    }
    else
    {
      ss.str("");
      ss << "Column Header '" << tokens[i] << "' is not a recognized column for CTF Files. Please recheck your .ctf file and report this error to the DREAM3D developers.";
      setErrorMessage(ss.str());
      deletePointers();
      return -107;
    }

    if(m_ColumnData[i] == NULL)
    {
      setErrorCode(-106);
      setErrorMessage("The CTF reader could not allocate memory for the data.");
      return -106; // Could not allocate the memory
    }
    m_NameIndexMap[tokens[i]] = static_cast<int>(i);
    m_NamePointerMap[tokens[i]] = m_ColumnData[i];
    m_DataParsers[i] = getParser(tokens[i], m_ColumnData[i], totalDataRows);
  }

  size_t counter = 0;
  for (int slice = zStart; slice < zEnd; ++slice)
  {
    for (size_t row = 0; row < yCells; ++row)
    {
      for (size_t col = 0; col < xCells; ++col)
      {
        ::memset(buf, 0, kBufferSize);
        in.getline(buf, kBufferSize);

        if ( (m_SingleSliceRead < 0) || (m_SingleSliceRead >= 0 && slice == m_SingleSliceRead) )
        {
          idx = 0;
          while (buf[idx] != 0 && idx < kBufferSize) { ++idx; }
          if(buf[idx - 1] < 32) { buf[idx - 1] = 0; }
          if(in.eof() == true) {
            break;
          }
          parseDataLine(buf, row, col, counter, xCells, yCells);
          ++counter;
        }

      }
      if(in.eof() == true)
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

  if(counter != getNumberOfElements() && in.eof() == true)
  {
    ss.str("");
    ss << "Premature End Of File reached.\n" << getFileName() << "\nNumRows=" << getNumberOfElements() << "\ncounter=" << counter
        << "\nTotal Data Points Read=" << counter << std::endl;
    setErrorMessage(ss.str());
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
int CtfReader::parseHeaderLines(std::vector<std::vector<std::string> > &headerLines)
{
  int err = 0;
  size_t size = headerLines.size();
  for (size_t i = 0; i < size; ++i)
  {
    std::vector<std::string> line = headerLines[i];
    //  std::cout << "Parsing Header Line: " << line[0] << std::endl;
    if(line[0].compare(Ebsd::Ctf::NumPhases) == 0)
    {
      //  std::cout << "Parsing Phases" << std::endl;
      EbsdHeaderEntry::Pointer p = m_HeaderMap[line[0]];
      p->parseValue(const_cast<char*>(line[1].c_str()), 0, line[1].length());
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
    }
    else if(line[0].compare("Euler angles refer to Sample Coordinate system (CS0)!") == 0)
    {
      // We parse out lots of stuff from this one line
      //Mag
      EbsdHeaderEntry::Pointer p0 = m_HeaderMap[line[1]];
      p0->parseValue(const_cast<char*>(line[2].c_str()), 0, line[2].length());
        PRINT_HTML_TABLE_ROW(p0)
      //Coverage
      EbsdHeaderEntry::Pointer p1 = m_HeaderMap[line[3]];
      p1->parseValue(const_cast<char*>(line[4].c_str()), 0, line[4].length());
      PRINT_HTML_TABLE_ROW(p1)
      //Device
      EbsdHeaderEntry::Pointer p2 = m_HeaderMap[line[5]];
      p2->parseValue(const_cast<char*>(line[6].c_str()), 0, line[6].length());
      PRINT_HTML_TABLE_ROW(p2)
      //KV
      EbsdHeaderEntry::Pointer p3 = m_HeaderMap[line[7]];
      p3->parseValue(const_cast<char*>(line[8].c_str()), 0, line[8].length());
      PRINT_HTML_TABLE_ROW(p3)
      //TiltAngle
      EbsdHeaderEntry::Pointer p4 = m_HeaderMap[line[9]];
      p4->parseValue(const_cast<char*>(line[10].c_str()), 0, line[10].length());
      PRINT_HTML_TABLE_ROW(p4)
      //TiltAxis
      EbsdHeaderEntry::Pointer p5 = m_HeaderMap[line[11]];
      p5->parseValue(const_cast<char*>(line[12].c_str()), 0, line[12].length());
      PRINT_HTML_TABLE_ROW(p5)
    }
    else if(line[0].compare("Channel Text File") == 0 || line[0].compare(":Channel Text File") == 0)
    {
      // We do not really do anything with this entry
    }
    else // This is the generic Catch all
    {
      EbsdHeaderEntry::Pointer p = m_HeaderMap[line[0]];
      if(NULL == p.get())
      {
        std::cout << "---------------------------" << std::endl;
        std::cout << "Could not find header entry for key'" << line[0] << "'" << std::endl;
//        std::string upper(line[0]);
//        std::transform(upper.begin(), upper.end(), upper.begin(), ::toupper);
//        std::cout << "#define ANG_" << upper << "     \"" << line[0] << "\"" << std::endl;
//        std::cout << "const std::string " << line[0] << "(ANG_" << upper << ");" << std::endl;
//        std::cout << "angInstanceProperty(AngHeaderEntry<float>. float, " << line[0] << "Ebsd::Ctf::" << line[0] << std::endl;
//        std::cout << "m_Headermap[Ebsd::Ctf::" << line[0] << "] = AngHeaderEntry<float>::NewEbsdHeaderEntry(Ebsd::Ctf::" << line[0] << ");" << std::endl;
      }
      else
      {
        if (line.size() > 1) {
          p->parseValue(const_cast<char*>(line[1].c_str()), 0, line[1].length());
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
void CtfReader::parseDataLine(const std::string &line, size_t row, size_t col, size_t offset, size_t xCells, size_t yCells )
{
  /* When reading the data there should be at least 11 cols of data.
   */
//  float x, y,  p1, p, p2, mad;
//  int phase, bCount, error, bc, bs;
//  size_t offset = i;

  // Filter the line to convert European command style decimals to US/UK style points
  std::vector<char> cLine(line.size()+1);
  ::memcpy( &(cLine.front()), line.c_str(), line.size() + 1);
  for (size_t c = 0; c < cLine.size(); ++c)
  {
    if(cLine[c] == ',')
    {
      cLine[c] = '.';
    }
  }

  std::vector<std::string> tokens = tokenize( &(cLine.front()), '\t');
  BOOST_ASSERT(tokens.size() == m_DataParsers.size());

  for (unsigned int i = 0; i < m_DataParsers.size(); ++i)
  {
    m_DataParsers[i]->parse(tokens[i], offset);
  }

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::vector<std::string> CtfReader::tokenize(char* buf, char delimiter)
{
  std::vector<std::string> output;
  std::string values(buf);
  std::string::size_type start = 0;
  std::string::size_type pos = 0;
//  std::cout << "-----------------------------" << std::endl;
  while(pos != std::string::npos && pos != values.size() - 1)
  {
    pos = values.find(delimiter, start);
    output.push_back(values.substr(start, pos-start));
 //   std::cout << "Adding: " << output.back() << std::endl;
    if (pos != std::string::npos)
    {
      start = pos + 1;
    }
  }
  return output;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int CtfReader::getHeaderLines(std::ifstream &reader, std::vector<std::vector<std::string> > &headerLines)
{
  int err = 0;
  char buf[kBufferSize];
  int index = 0;
  int numPhases = 0;
  while (!reader.eof() && false == getHeaderIsComplete())
  {
    ::memset(buf, 0, kBufferSize);
    reader.getline(buf, kBufferSize);
    // Append the line to the complete header
    appendOriginalHeader(std::string(buf));

    // Replace the newline at the end of the line with a NULL character
    int i = 0;
	while (i < kBufferSize && buf[i] != 0) { ++i; }
    if(buf[i - 1] < 32) { buf[i - 1] = 0; }

    std::vector<std::string> tokens = tokenize(buf, '\t');

    // Once we hit the "Phases" header line we know exactly how many lines we have left to read
    if (tokens[0].compare("Phases") == 0)
    {
      numPhases = atoi(tokens[1].c_str());
      headerLines.push_back(tokens);
      for(int n = 0; n < numPhases; ++n)
      {
        ::memset(buf, 0, kBufferSize);
        reader.getline(buf, kBufferSize);
        appendOriginalHeader(std::string(buf));
        i = 0;
        while (buf[i] != 0 && i < kBufferSize) { ++i; }
        if(buf[i - 1] < 32) { buf[i - 1] = 0; }
        tokens = tokenize(buf, '\t');
        headerLines.push_back(tokens);
      }
      setHeaderIsComplete(true);
      break;
    }


    ++index;

    // Remove empty lines
    if (buf[0] == 0) continue;

    // End when column header line is read
    if (isDataHeaderLine(tokens))
    {
      setHeaderIsComplete(true);
      break;
    }
    headerLines.push_back(tokens);
  }
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool CtfReader::isDataHeaderLine(std::vector<std::string> &columns)
{
    if (columns.size() != 11)
        return false;
    if (columns[0].compare("Phase") != 0)
        return false;
    if (columns[9].compare("BC") != 0)
        return false;

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
std::vector<std::string> CtfReader::getColumnNames()
{
  std::vector<std::string> names;
  for (std::map<std::string, int>::iterator iter = m_NameIndexMap.begin(); iter != m_NameIndexMap.end(); ++iter )
  {
    names.push_back((*iter).first);
  }

  return names;
}

#define CTF_PRINT_HEADER_VALUE(var, out)\
 out << #var << ": " << get##var () << std::endl;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CtfReader::printHeader(std::ostream &out)
{
  std::cout << "-------------------- CtfReader Header Values --------------------" << std::endl;
  CTF_PRINT_HEADER_VALUE(Channel, out);
  CTF_PRINT_HEADER_VALUE(Prj, out);
  CTF_PRINT_HEADER_VALUE(Author, out);
  CTF_PRINT_HEADER_VALUE(JobMode, out);
  CTF_PRINT_HEADER_VALUE(XCells, out);
  CTF_PRINT_HEADER_VALUE(YCells, out);
  CTF_PRINT_HEADER_VALUE(XStep, out);
  CTF_PRINT_HEADER_VALUE(YStep, out);
  CTF_PRINT_HEADER_VALUE(AcqE1, out);
  CTF_PRINT_HEADER_VALUE(AcqE2, out);
  CTF_PRINT_HEADER_VALUE(AcqE3, out);
  CTF_PRINT_HEADER_VALUE(Euler, out);
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

