///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2010, Michael A. Jackson. BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
//
///////////////////////////////////////////////////////////////////////////////


#include "AngReader.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>


#include "AngConstants.h"


#define PI_OVER_2f       1.57079632679489661f
#define THREE_PI_OVER_2f 4.71238898038468985f
#define TWO_PIf          6.28318530717958647f
#define ONE_PIf          3.14159265358979323f

#define kBufferSize 1024


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AngReader::AngReader() :
m_UserOrigin(Ang::NoOrientation),
m_FileName(""),
m_NumberOfElements(0),
m_ManageMemory(true)
{
  // Init all the arrays to NULL
  m_Phi1 = NULL;
  m_Phi = NULL;
  m_Phi2 = NULL;
  m_Iq = NULL;
  m_Ci = NULL;
  m_PhaseData = NULL;
  m_X = NULL;
  m_Y = NULL;
  m_D1 = NULL;
  m_D2 = NULL;

  m_NumFields = 8;
  // Initialize the map of header key to header value
  m_Headermap[TSL::OIM::TEMPIXPerUM] = AngHeaderEntry<float>::NewHeaderEntry(TSL::OIM::TEMPIXPerUM);
  m_Headermap[TSL::OIM::XStar] = AngHeaderEntry<float>::NewHeaderEntry(TSL::OIM::XStar);
  m_Headermap[TSL::OIM::YStar] = AngHeaderEntry<float>::NewHeaderEntry(TSL::OIM::YStar);
  m_Headermap[TSL::OIM::ZStar] = AngHeaderEntry<float>::NewHeaderEntry(TSL::OIM::ZStar);
  m_Headermap[TSL::OIM::WorkingDistance] = AngHeaderEntry<float>::NewHeaderEntry(TSL::OIM::WorkingDistance);
  m_Headermap[TSL::OIM::Grid] = AngStringHeaderEntry::NewHeaderEntry(TSL::OIM::Grid);
  m_Headermap[TSL::OIM::XStep] = AngHeaderEntry<float>::NewHeaderEntry(TSL::OIM::XStep);
  m_Headermap[TSL::OIM::YStep] = AngHeaderEntry<float>::NewHeaderEntry(TSL::OIM::YStep);
  m_Headermap[TSL::OIM::ZStep] = AngHeaderEntry<float>::NewHeaderEntry(TSL::OIM::ZStep); // NOT actually in the file>::NewHeaderEntry(); , but may be needed
  m_Headermap[TSL::OIM::ZPos] = AngHeaderEntry<float>::NewHeaderEntry(TSL::OIM::ZPos); // NOT actually in the file>::NewHeaderEntry(); , but may be needed
  m_Headermap[TSL::OIM::ZMax] = AngHeaderEntry<float>::NewHeaderEntry(TSL::OIM::ZMax); // NOT actually in the file>::NewHeaderEntry(); , but may be needed
  m_Headermap[TSL::OIM::NColsOdd] = AngHeaderEntry<int>::NewHeaderEntry(TSL::OIM::NColsOdd);
  m_Headermap[TSL::OIM::NColsEven] = AngHeaderEntry<int>::NewHeaderEntry(TSL::OIM::NColsEven);
  //  m_Headermap[TSL::OIM::NCols] = AngHeaderEntry<int>::NewHeaderEntry(TSL::OIM::NCols);
  m_Headermap[TSL::OIM::NRows] = AngHeaderEntry<int>::NewHeaderEntry(TSL::OIM::NRows);
  m_Headermap[TSL::OIM::Operator] = AngStringHeaderEntry::NewHeaderEntry(TSL::OIM::Operator);
  m_Headermap[TSL::OIM::SampleId] = AngStringHeaderEntry::NewHeaderEntry(TSL::OIM::SampleId);
  m_Headermap[TSL::OIM::ScanId] = AngStringHeaderEntry::NewHeaderEntry(TSL::OIM::ScanId);


  // Give these values some defaults
//  setNumCols(-1);
  setNumOddCols(-1);
  setNumEvenCols(-1);
  setNumRows(-1);
//  setZStep(1.0);
//  setZMax(1.0);
//  setZPos(0.0);
}

// -----------------------------------------------------------------------------
//  Clean up any Memory that was allocated for this class
// -----------------------------------------------------------------------------
AngReader::~AngReader()
{
  deletePointers();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AngReader::initPointers(size_t numElements)
{
  size_t numBytes = numElements * sizeof(float);
  m_Phi1 = allocateArray<float > (numElements);
  m_Phi = allocateArray<float > (numElements);
  m_Phi2 = allocateArray<float > (numElements);
  m_Iq = allocateArray<float > (numElements);
  m_Ci = allocateArray<float > (numElements);
  m_PhaseData = allocateArray<float > (numElements);
  m_X = allocateArray<float > (numElements);
  m_Y = allocateArray<float > (numElements);
  m_D1 = allocateArray<float > (numElements);
  m_D2 = allocateArray<float > (numElements);

  ::memset(m_Phi1, 0, numBytes);
  ::memset(m_Phi, 0, numBytes);
  ::memset(m_Phi2, 0, numBytes);
  ::memset(m_Iq, 0, numBytes);
  ::memset(m_Ci, 0, numBytes);
  ::memset(m_PhaseData, 0, numBytes);
  ::memset(m_X, 0, numBytes);
  ::memset(m_Y, 0, numBytes);
  ::memset(m_D1, 0, numBytes);
  ::memset(m_D2, 0, numBytes);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AngReader::deletePointers()
{
  this->deallocateArrayData<float > (m_Phi1);
  this->deallocateArrayData<float > (m_Phi);
  this->deallocateArrayData<float > (m_Phi2);
  this->deallocateArrayData<float > (m_Iq);
  this->deallocateArrayData<float > (m_Ci);
  this->deallocateArrayData<float > (m_PhaseData);
  this->deallocateArrayData<float > (m_X);
  this->deallocateArrayData<float > (m_Y);
  this->deallocateArrayData<float > (m_D1);
  this->deallocateArrayData<float > (m_D2);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int AngReader::readHeaderOnly()
{
  int err = 1;
  char buf[kBufferSize];
  std::ifstream in(m_FileName.c_str());
  m_headerComplete = false;
  if (!in.is_open())
  {
    std::cout << "Ang file could not be opened: " << m_FileName << std::endl;
    return -100;
  }

  m_CompleteHeader.clear();
  m_Phases.clear();

  while (!in.eof() && !m_headerComplete)
  {
    ::memset(buf, 0, kBufferSize);
    in.getline(buf, kBufferSize);
    parseHeaderLine(buf, kBufferSize);
    m_CompleteHeader.append(buf);
  }
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int AngReader::readFile()
{
  int err = 1;
  char buf[kBufferSize];
  m_headerComplete = false;
  std::ifstream in(m_FileName.c_str());

  if (!in.is_open())
  {
    std::cout << "Ang file could not be opened: " << m_FileName << std::endl;
    return -100;
  }

  m_CompleteHeader.clear();
  m_Phases.clear();

  while (!in.eof() && !m_headerComplete)
  {
    ::memset(buf, 0, kBufferSize);
    in.getline(buf, kBufferSize);
    parseHeaderLine(buf, kBufferSize);
    if (m_headerComplete == false) {
      m_CompleteHeader.append(buf);
    }
  }

  // Delete any currently existing pointers
  deletePointers();
  // Initialize new pointers
  size_t numElements = 0;

  std::string grid = getGrid();

  int nOddCols = getNumOddCols();
  int nEvenCols = getNumEvenCols();
  int nRows = getNumRows();

  if (nRows < 1)
  {
    return -200;
  }
  else if (grid.find(TSL::OIM::SquareGrid) == 0)
  {
   // if (nCols > 0) { numElements = nRows * nCols; }
    if (nOddCols > 0) { numElements = nRows * nOddCols;/* nCols = nOddCols;*/ }
    else if (nEvenCols > 0) { numElements = nRows * nEvenCols; /* nCols = nEvenCols; */ }
    else { numElements = 0; }
  }
  else if (grid.find(TSL::OIM::HexGrid) == 0)
  {
    std::cout << "Ang Files with Hex Grids Are NOT currently supported." << std::endl;
    return -400;
  }
  else // Grid was not set
  {
    return -300;
  }

  initPointers(numElements);

  if (NULL == m_Phi1 || NULL == m_Phi || NULL == m_Phi2 || NULL == m_Iq || NULL == m_D1 || NULL == m_Ci || NULL == m_PhaseData || m_X == NULL || m_Y == NULL)
  {
    return -1;
  }

  float xstep = getXStep();
  float ystep = getYStep();
  float xMaxValue = static_cast<float > ((nEvenCols - 1) * xstep );
  float yMaxValue = static_cast<float > ((nRows - 1) * ystep );

  //double progress = 0.0;
  int totalDataRows = nRows * nEvenCols;
  int counter = 0;
  while (in.eof() == false)
  {

    this->readData(buf, xMaxValue, yMaxValue, nEvenCols, xstep, ystep, counter);
    // Read the next line of data
    in.getline(buf, kBufferSize);
    ++counter;
  }
  if (counter != totalDataRows && in.eof() == true)
  {
    std::cout << "Premature End Of File reached while reading the .ang file.\n NumRows=" << nRows << "nEvenCols=" << nEvenCols << " Total Data Points Read="
        << counter << std::endl;
  }
  if (m_NumFields < 10)
  {
    this->deallocateArrayData<float > (m_D2);
  }
  if (m_NumFields < 9)
  {
    this->deallocateArrayData<float > (m_D1);
  }
  return err;
}

// -----------------------------------------------------------------------------
//  Read the Header part of the ANG file
// -----------------------------------------------------------------------------
void AngReader::parseHeaderLine(char* buf, size_t length)
{
  if (buf[0] != '#')
  {
    m_headerComplete = true;
    return;
  }
  // Start at the first character and walk until you find another non-space character
  size_t i = 1;
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

  //TODO: If the word is "Phase" then we need to construct a "Phase" class and
  //  store all the meta data for the phase into that class. When we are done
  //  parsing data for the phase then stick the Phase instance into the header
  //  map or stick it into a vector<Phase::Pointer> and stick the vector into
  //  the map under the "Phase" key
  if (word.compare(TSL::OIM::Phase) == 0)
  {
    m_CurrentPhase = AngPhase::New();
    m_CurrentPhase->parsePhase(buf, wordEnd, length);
    // Parsing the phase is complete, now add it to the vector of Phases
    m_Phases.push_back(m_CurrentPhase);
  }
  else if (word.compare(TSL::OIM::MaterialName) == 0 && m_CurrentPhase.get() != NULL)
  {
    m_CurrentPhase->parseMaterialName(buf, wordEnd, length);
  }
  else if (word.compare(TSL::OIM::Formula) == 0 && m_CurrentPhase.get() != NULL)
  {
    m_CurrentPhase->parseFormula(buf, wordEnd, length);
  }
  else if (word.compare(TSL::OIM::Info) == 0 && m_CurrentPhase.get() != NULL)
  {
    m_CurrentPhase->parseInfo(buf, wordEnd, length);
  }
  else if (word.compare(TSL::OIM::Symmetry) == 0 && m_CurrentPhase.get() != NULL)
  {
    m_CurrentPhase->parseSymmetry(buf, wordEnd, length);
  }
  else if (word.compare(TSL::OIM::LatticeConstants) == 0 && m_CurrentPhase.get() != NULL)
  {
    m_CurrentPhase->parseLatticeConstants(buf, wordEnd, length);
  }
  else if (word.compare(TSL::OIM::NumberFamilies) == 0 && m_CurrentPhase.get() != NULL)
  {
    m_CurrentPhase->parseNumberFamilies(buf, wordEnd, length);
  }
  else if (word.compare(TSL::OIM::HKLFamilies) == 0 && m_CurrentPhase.get() != NULL)
  {
    m_CurrentPhase->parseHKLFamilies(buf, wordEnd, length);
  }
  else if (word.compare(TSL::OIM::Categories) == 0 && m_CurrentPhase.get() != NULL)
  {
    m_CurrentPhase->parseCategories(buf, wordEnd, length);
  }
  else
  {
    HeaderEntry::Pointer p = m_Headermap[word];
    if (NULL == p.get())
    {
      std::cout << "---------------------------" << std::endl;
      std::cout << "Could not find header entry for key'" << word << "'" << std::endl;
      std::string upper(word);
      std::transform(upper.begin(), upper.end(), upper.begin(), ::toupper);
      std::cout << "#define ANG_" << upper << "     \"" << word << "\"" << std::endl;
      std::cout << "const std::string " << word << "(ANG_" << upper << ");" << std::endl;

      std::cout << "angInstanceProperty(AngHeaderEntry<float>. float, " << word << "TSL::OIM::" << word << std::endl;
      std::cout << "m_Headermap[TSL::OIM::" << word << "] = AngHeaderEntry<float>::NewHeaderEntry(TSL::OIM::" << word << ");" << std::endl;
      return;
    }
    else
    {
      p->parseValue(buf, wordEnd, length);
    }
  }

}

// -----------------------------------------------------------------------------
//  Read the data part of the ANG file
// -----------------------------------------------------------------------------
void AngReader::readData(const std::string &line, float xMaxValue, float yMaxValue,
                        int nCols, float xstep, float ystep, size_t counter)
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
   * unknown 1
   * unknown 2
   *
   * Some TSL ang files do NOT have all 10 columns. Assume these are lacking the last
   * 2 columns and all the other columns are the same as above.
   */
  float p1, p, p2, x, y, iqual, conf, ph, d1, d2;

  m_NumFields = sscanf(line.c_str(), "%f %f %f %f %f %f %f %f %f %f", &p1, &p,&p2, &x, &y, &iqual, &conf, &ph, &d1, &d2);

  // Do we transform the data
  if (m_UserOrigin == Ang::UpperRightOrigin)
  {
    x = xMaxValue - x;
    if (p1 - PI_OVER_2f < 0.0)
    {
      p1 = p1 + THREE_PI_OVER_2f;
    }
    else
    {
      p1 = p1 - PI_OVER_2f;
    }
  }
  else if (m_UserOrigin == Ang::UpperLeftOrigin)
  {
    if (p1 + PI_OVER_2f > TWO_PIf)
    {
      p1 = p1 - THREE_PI_OVER_2f;
    }
    else
    {
      p1 = p1 + PI_OVER_2f;
    }
    if (p + ONE_PIf > TWO_PIf)
    {
      p = p - ONE_PIf;
    }
    else
    {
      p = p + ONE_PIf;
    }
  }
  else if (m_UserOrigin == Ang::LowerLeftOrigin)
  {
    y = yMaxValue - y;
    if (p1 + PI_OVER_2f > TWO_PIf)
    {
      p1 = p1 - THREE_PI_OVER_2f;
    }
    else
    {
      p1 = p1 + PI_OVER_2f;
    }
  }
  else if (m_UserOrigin == Ang::LowerRightOrigin)
  {
    x = xMaxValue - x;
    y = yMaxValue - y;
  }

  size_t offset  = 0;

  if (m_UserOrigin == Ang::NoOrientation)
  {
    // If the user/programmer sets "NoOrientation" then we simply read the data
    // from the file and copy the values into the arrays without any regard for
    // the true X and Y positions in the grid. We are simply trying to keep the
    // data as close to the original as possible.
    offset = counter;
  }
  else
  {
    // The next set of calculations figures out where to place the data
    // in the arrays, ie, which offset based on the array really being a
    // 2D array that is laid out with the X Axis moving the fastest and
    // the Y Axis moving the slowest. On Visual Studio there seems to be some
    // sort of round off error when the floats are converted to size_t types.
    // In order for Visual Studio compilers to get the conversion correct
    // it seems that we need to break up the calculations, at least on 32 bit
    // compiles. 64 Bit compiles did not show this issue. If the user/programmer
    // sets any type of Orientation preference then we have to calculate
    // the offsets based on the x, y, xStep and yStep values. Changes to the
    // x and y values from the "if" statements above will effect what the final
    // offset is calculated to be.
    float xTemp = x / xstep;
    float yTemp = y / ystep;
    size_t xTempSizeT = static_cast<size_t>(xTemp);
    size_t yTempSizeT = static_cast<size_t>(yTemp);
    offset = yTempSizeT * nCols + xTempSizeT;
  }
  m_Phi1[offset] = p1;
  m_Phi[offset] = p;
  m_Phi2[offset] = p2;
  m_Iq[offset] = iqual;
  m_Ci[offset] = conf;
  m_PhaseData[offset] = ph;
  m_X[offset] = x;
  m_Y[offset] = y;
  if (m_NumFields > 8) {
    m_D1[offset] = d1;
  }
  if (m_NumFields > 9)
  {
    m_D2[offset] = d2;
  }
}

