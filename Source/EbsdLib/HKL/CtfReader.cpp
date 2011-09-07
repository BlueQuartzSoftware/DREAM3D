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

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CtfReader::CtfReader() :
EbsdReader()
{
  m_Phase = NULL;
  m_X = NULL;
  m_Y = NULL;
  m_BandCount = NULL;
  m_Error = NULL;
  m_Euler1 = NULL;
  m_Euler2 = NULL;
  m_Euler3 = NULL;
  m_MAD = NULL;
  m_BC = NULL;
  m_BS = NULL;

  setNumFields(11);


  // Initialize the map of header key to header value
  m_Headermap[Ebsd::Ctf::ChannelTextFile] = CtfStringHeaderEntry::NewEbsdHeaderEntry(Ebsd::Ctf::ChannelTextFile);
  m_Headermap[Ebsd::Ctf::Prj] = CtfStringHeaderEntry::NewEbsdHeaderEntry(Ebsd::Ctf::Prj);
  m_Headermap[Ebsd::Ctf::Author] = CtfStringHeaderEntry::NewEbsdHeaderEntry(Ebsd::Ctf::Author);
  m_Headermap[Ebsd::Ctf::JobMode] = CtfStringHeaderEntry::NewEbsdHeaderEntry(Ebsd::Ctf::JobMode);
  m_Headermap[Ebsd::Ctf::XCells] = CtfHeaderEntry<int>::NewEbsdHeaderEntry(Ebsd::Ctf::XCells);
  m_Headermap[Ebsd::Ctf::YCells] = CtfHeaderEntry<int>::NewEbsdHeaderEntry(Ebsd::Ctf::YCells);
  m_Headermap[Ebsd::Ctf::XStep] = CtfHeaderEntry<float>::NewEbsdHeaderEntry(Ebsd::Ctf::XStep);
  m_Headermap[Ebsd::Ctf::YStep] = CtfHeaderEntry<float>::NewEbsdHeaderEntry(Ebsd::Ctf::YStep);
  m_Headermap[Ebsd::Ctf::AcqE1] = CtfHeaderEntry<float>::NewEbsdHeaderEntry(Ebsd::Ctf::AcqE1);
  m_Headermap[Ebsd::Ctf::AcqE2] = CtfHeaderEntry<float>::NewEbsdHeaderEntry(Ebsd::Ctf::AcqE2);
  m_Headermap[Ebsd::Ctf::AcqE3] = CtfHeaderEntry<float>::NewEbsdHeaderEntry(Ebsd::Ctf::AcqE3);
  m_Headermap[Ebsd::Ctf::Euler] = CtfStringHeaderEntry::NewEbsdHeaderEntry(Ebsd::Ctf::Euler);
  m_Headermap[Ebsd::Ctf::Mag] = CtfHeaderEntry<int>::NewEbsdHeaderEntry(Ebsd::Ctf::Mag);
  m_Headermap[Ebsd::Ctf::Coverage] = CtfHeaderEntry<int>::NewEbsdHeaderEntry(Ebsd::Ctf::Coverage);
  m_Headermap[Ebsd::Ctf::Device] = CtfHeaderEntry<int>::NewEbsdHeaderEntry(Ebsd::Ctf::Device);
  m_Headermap[Ebsd::Ctf::KV] = CtfHeaderEntry<int>::NewEbsdHeaderEntry(Ebsd::Ctf::KV);
  m_Headermap[Ebsd::Ctf::TiltAngle] = CtfHeaderEntry<float>::NewEbsdHeaderEntry(Ebsd::Ctf::TiltAngle);
  m_Headermap[Ebsd::Ctf::TiltAxis] = CtfHeaderEntry<float>::NewEbsdHeaderEntry(Ebsd::Ctf::TiltAxis);
  m_Headermap[Ebsd::Ctf::NumPhases] = CtfHeaderEntry<int>::NewEbsdHeaderEntry(Ebsd::Ctf::NumPhases);

  setXCells(-1);
  setYCells(-1);

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
  setNumberOfElements(numElements);
  if (numElements == 0) { return; }
  size_t numBytes = numElements * sizeof(float);
  m_Phase = allocateArray<int > (numElements);
  m_X = allocateArray<float > (numElements);
  m_Y = allocateArray<float > (numElements);
  m_BandCount = allocateArray<int > (numElements);
  m_Error = allocateArray<int > (numElements);
  m_Euler1 = allocateArray<float> (numElements);
  m_Euler2 = allocateArray<float > (numElements);
  m_Euler3 = allocateArray<float > (numElements);
  m_MAD = allocateArray<float > (numElements);
  m_BC = allocateArray<int > (numElements);
  m_BS = allocateArray<int > (numElements);

  ::memset(m_Phase, 0, numBytes);
  ::memset(m_X, 0, numBytes);
  ::memset(m_Y, 0, numBytes);
  ::memset(m_BandCount, 0, numBytes);
  ::memset(m_Error, 0, numBytes);
  ::memset(m_Euler1, 0, numBytes);
  ::memset(m_Euler2, 0, numBytes);
  ::memset(m_Euler3, 0, numBytes);
  ::memset(m_MAD, 0, numBytes);
  ::memset(m_BC, 0, numBytes);
  ::memset(m_BS, 0, numBytes);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CtfReader::deletePointers()
{
  this->deallocateArrayData<int > (m_Phase);
  this->deallocateArrayData<float > (m_X);
  this->deallocateArrayData<float > (m_Y);
  this->deallocateArrayData<int > (m_BandCount);
  this->deallocateArrayData<int > (m_Error);
  this->deallocateArrayData<float > (m_Euler1);
  this->deallocateArrayData<float > (m_Euler2);
  this->deallocateArrayData<float > (m_Euler3);
  this->deallocateArrayData<float > (m_MAD);
  this->deallocateArrayData<int > (m_BC);
  this->deallocateArrayData<int > (m_BS);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void* CtfReader::getPointerByName(const std::string &fieldName)
{
  if (fieldName.compare(Ebsd::Ctf::Phase) == 0) { return static_cast<void*>(m_Phase);}
  if (fieldName.compare(Ebsd::Ctf::X) == 0) { return static_cast<void*>(m_X);}
  if (fieldName.compare(Ebsd::Ctf::Y) == 0) { return static_cast<void*>(m_Y);}
  if (fieldName.compare(Ebsd::Ctf::BandCount) == 0) { return static_cast<void*>(m_BandCount);}
  if (fieldName.compare(Ebsd::Ctf::Error) == 0) { return static_cast<void*>(m_Error);}
  if (fieldName.compare(Ebsd::Ctf::Euler1) == 0) { return static_cast<void*>(m_Euler1);}
  if (fieldName.compare(Ebsd::Ctf::Euler2) == 0) { return static_cast<void*>(m_Euler2);}
  if (fieldName.compare(Ebsd::Ctf::Euler3) == 0) { return static_cast<void*>(m_Euler3);}
  if (fieldName.compare(Ebsd::Ctf::MeanAngularDeviation) == 0) { return static_cast<void*>(m_MAD);}
  if (fieldName.compare(Ebsd::Ctf::BandContrast) == 0) { return static_cast<void*>(m_BC);}
  if (fieldName.compare(Ebsd::Ctf::BandSlope) == 0) { return static_cast<void*>(m_BS);}
  return NULL;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Ebsd::NumType CtfReader::getPointerType(const std::string &fieldName)
  {
  if (fieldName.compare(Ebsd::Ctf::Phase) == 0) { return Ebsd::Int32;}
  if (fieldName.compare(Ebsd::Ctf::X) == 0) { return Ebsd::Float;}
  if (fieldName.compare(Ebsd::Ctf::Y) == 0) { return Ebsd::Float;}
  if (fieldName.compare(Ebsd::Ctf::BandCount) == 0) { return Ebsd::Int32;}
  if (fieldName.compare(Ebsd::Ctf::Error) == 0) { return Ebsd::Int32;}
  if (fieldName.compare(Ebsd::Ctf::Euler1) == 0) { return Ebsd::Float;}
  if (fieldName.compare(Ebsd::Ctf::Euler2) == 0) { return Ebsd::Float;}
  if (fieldName.compare(Ebsd::Ctf::Euler3) == 0) { return Ebsd::Float;}
  if (fieldName.compare(Ebsd::Ctf::MeanAngularDeviation) == 0) { return Ebsd::Float;}
  if (fieldName.compare(Ebsd::Ctf::BandContrast) == 0) { return Ebsd::Int32;}
  if (fieldName.compare(Ebsd::Ctf::BandSlope) == 0) { return Ebsd::Int32;}
  return Ebsd::UnknownNumType;
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
    std::cout << "Ang file could not be opened: " << getFileName() << std::endl;
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

  err = readData(in);
  if (err < 0) { return err;}

  transformData();

  checkAndFlipAxisDimensions();

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int CtfReader::readData(std::ifstream &in)
{
  // Delete any currently existing pointers
   deletePointers();

   // Initialize new pointers
   size_t yCells = getYCells();
   size_t xCells = getXCells();
   size_t totalDataRows = yCells * xCells;


   initPointers(totalDataRows);
   if (NULL == getPhasePointer() || NULL == getXPointer() || NULL == getYPointer()
       || NULL == getBandCountPointer()  || NULL == getErrorPointer()
       || NULL == getEuler1Pointer() || getEuler2Pointer() == NULL || getEuler3Pointer() == NULL
       || NULL == getMeanAngularDeviationPointer() || NULL == getBandContrastPointer() || NULL == getBandSlopePointer())
   {
     return -1;
   }


   size_t counter = 0;
   char buf[kBufferSize];

   for (size_t row = 0; row < yCells ; ++row)
   {
     for (size_t col = 0; col < xCells; ++col)
     {
       in.getline(buf, kBufferSize);
       if (in.eof() == true) break;
       parseDataLine(buf, row, col, counter, xCells, yCells);
       ++counter;
     }
     if (in.eof() == true) break;
   }


   if (counter != getNumberOfElements() && in.eof() == true)
   {
     std::cout << "Premature End Of File reached.\n"
         << getFileName()
         << "\nNumRows=" << getNumberOfElements()
         << "\ncounter=" << counter
         << "\nTotal Data Points Read=" << counter << std::endl;
   }
   return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int CtfReader::parseHeaderLines(std::vector<std::vector<std::string> > &headerLines)
{
  int err = 0;
  size_t size = headerLines.size();
  for(size_t i = 0; i < size; ++i) {
    std::vector<std::string> line = headerLines[i];
  //  std::cout << "Parsing Header Line: " << line[0] << std::endl;
    if (line[0].compare(Ebsd::Ctf::NumPhases) == 0)
    {
    //  std::cout << "Parsing Phases" << std::endl;
      EbsdHeaderEntry::Pointer p = m_Headermap[line[0]];
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
    else if (line[0].compare("Euler angles refer to Sample Coordinate system (CS0)!") == 0)
    {
      // We parse out lots of stuff from this one line
      //Mag
      EbsdHeaderEntry::Pointer p0 = m_Headermap[line[1]];
      p0->parseValue(const_cast<char*>(line[2].c_str()), 0, line[2].length());
      //Coverage
      EbsdHeaderEntry::Pointer p1 = m_Headermap[line[3]];
      p1->parseValue(const_cast<char*>(line[4].c_str()), 0, line[4].length());
      //Device
      EbsdHeaderEntry::Pointer p2 = m_Headermap[line[5]];
      p2->parseValue(const_cast<char*>(line[6].c_str()), 0, line[6].length());
      //KV
      EbsdHeaderEntry::Pointer p3 = m_Headermap[line[7]];
      p3->parseValue(const_cast<char*>(line[8].c_str()), 0, line[8].length());
      //TiltAngle
      EbsdHeaderEntry::Pointer p4 = m_Headermap[line[9]];
      p4->parseValue(const_cast<char*>(line[10].c_str()), 0, line[10].length());
      //TiltAxis
      EbsdHeaderEntry::Pointer p5 = m_Headermap[line[11]];
      p5->parseValue(const_cast<char*>(line[12].c_str()), 0, line[12].length());
    }
    else if (line[0].compare("Channel Text File") == 0)
    {
      // We do not really do anything with this entry
      //  EbsdHeaderEntry::Pointer p = m_Headermap[line[0]];
    }
    else  // This is the generic Catch all
    {
      EbsdHeaderEntry::Pointer p = m_Headermap[line[0]];
      if (NULL == p.get())
      {
        std::cout << "---------------------------" << std::endl;
        std::cout << "Could not find header entry for key'" << line[0] << "'" << std::endl;
        std::string upper(line[0]);
        std::transform(upper.begin(), upper.end(), upper.begin(), ::toupper);
        std::cout << "#define ANG_" << upper << "     \"" << line[0] << "\"" << std::endl;
        std::cout << "const std::string " << line[0] << "(ANG_" << upper << ");" << std::endl;
        std::cout << "angInstanceProperty(AngHeaderEntry<float>. float, " << line[0] << "Ebsd::Ctf::" << line[0] << std::endl;
        std::cout << "m_Headermap[Ebsd::Ctf::" << line[0] << "] = AngHeaderEntry<float>::NewEbsdHeaderEntry(Ebsd::Ctf::" << line[0] << ");" << std::endl;
      }
      else
      {
        p->parseValue(const_cast<char*>(line[1].c_str()), 0, line[1].length());
      }
    }

  }
  return err;
}

// -----------------------------------------------------------------------------
//  Read the data part of the .ctf file
// -----------------------------------------------------------------------------
void CtfReader::parseDataLine(const std::string &line, int row, int col, size_t i, int xCells, int yCells )
{
  /* When reading the data there should be at least 11 cols of data.
   */
  float x, y,  p1, p, p2, mad;
  int phase, bCount, error, bc, bs;
  size_t offset = i;
  int fields = sscanf(line.c_str(), "%d\t%f\t%f\t%d\t%d\t%f\t%f\t%f\t%f\t%d\t%d",
                       &phase, &x,&y, &bCount, &error, &p1, &p, &p2, &mad, &bc, &bs);
  assert(fields == getNumFields());

#if 0

  // Do we transform the data
		  if (getUserOrigin() == Ebsd::UpperRightOrigin)
		  {
			  if (getUserZDir() == Ebsd::IntoSlice)
			  {
				offset = (((xCells-1)-col)*yCells)+(row);
				if (p - ONE_PIf < 0.0)
				{
				  p = p + ONE_PIf;
				}
				else
				{
				  p = p - ONE_PIf;
				}
				if (p2 - PI_OVER_2f < 0.0)
				{
				  p2 = p2 + THREE_PI_OVER_2f;
				}
				else
				{
				  p2 = p2 - PI_OVER_2f;
				}
			  }
			  if (getUserZDir() == Ebsd::OutofSlice)
			  {
				offset = (col*yCells)+(row);
				if (p1 - ONE_PIf < 0.0)
				{
				  p1 = p1 + ONE_PIf;
				}
				else
				{
				  p1 = p1 - ONE_PIf;
				}
			  }
		  }
		  else if (getUserOrigin() == Ebsd::UpperLeftOrigin)
		  {
			  if (getUserZDir() == Ebsd::IntoSlice)
			  {
				offset = (row*xCells)+(col);
				if (p - ONE_PIf < 0.0)
				{
				  p = p + ONE_PIf;
				}
				else
				{
				  p = p - ONE_PIf;
				}
			  }
			  if (getUserZDir() == Ebsd::OutofSlice)
			  {
				offset = (row*xCells)+((xCells-1)-col);
				if (p1 - THREE_PI_OVER_2f < 0.0)
				{
				  p1 = p1 + PI_OVER_2f;
				}
				else
				{
				  p1 = p1 - THREE_PI_OVER_2f;
				}
			  }
		  }
		  else if (getUserOrigin() == Ebsd::LowerLeftOrigin)
		  {
			  if (getUserZDir() == Ebsd::IntoSlice)
			  {
				offset = (col*yCells)+((yCells-1)-row);
				if (p - ONE_PIf < 0.0)
				{
				  p = p + ONE_PIf;
				}
				else
				{
				  p = p - ONE_PIf;
				}
				if (p2 - THREE_PI_OVER_2f < 0.0)
				{
				  p2 = p2 + PI_OVER_2f;
				}
				else
				{
				  p2 = p2 - THREE_PI_OVER_2f;
				}
			  }
			  if (getUserZDir() == Ebsd::OutofSlice)
			  {
				offset = (((xCells-1)-col)*yCells)+((yCells-1)-row);
			  }
		  }
		  else if (getUserOrigin() == Ebsd::LowerRightOrigin)
		  {
			  if (getUserZDir() == Ebsd::IntoSlice)
			  {
				offset = (((yCells-1)-row)*xCells)+((xCells-1)-col);
				if (p - ONE_PIf < 0.0)
				{
				  p = p + ONE_PIf;
				}
				else
				{
				  p = p - ONE_PIf;
				}
				if (p2 - ONE_PIf < 0.0)
				{
				  p2 = p2 + ONE_PIf;
				}
				else
				{
				  p2 = p2 - ONE_PIf;
				}
			  }
			  if (getUserZDir() == Ebsd::OutofSlice)
			  {
				offset = (((yCells-1)-row)*xCells)+(col);
				if (p1 - PI_OVER_2f < 0.0)
				{
				  p1 = p1 + THREE_PI_OVER_2f;
				}
				else
				{
				  p1 = p1 - PI_OVER_2f;
				}
			  }
		  }

	  if (getUserOrigin() == Ebsd::NoOrientation)
	  {
		// If the user/programmer sets "NoOrientation" then we simply read the data
		// from the file and copy the values into the arrays without any regard for
		// the true X and Y positions in the grid. We are simply trying to keep the
		// data as close to the original as possible.
		offset = i;
	  }
#else
	  offset = i;
#endif
  m_Phase[offset] = phase;
  m_X[offset] = x;
  m_Y[offset] = y;
  m_BandCount[offset] = bCount;
  m_Error[offset] = error;
  m_Euler1[offset] = p1;
  m_Euler2[offset] = p;
  m_Euler3[offset] = p2;
  m_MAD[offset] = mad;
  m_BC[offset] = bc;
  m_BS[offset] = bs;

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
  while (!reader.eof() && false == getHeaderIsComplete())
  {
    ::memset(buf, 0, kBufferSize);
    reader.getline(buf, kBufferSize);
    // Append the line to the complete header
    appendOriginalHeader(std::string(buf));

    // Replace the newline at the end of the line with a NULL character
    int i = 0;
    while(buf[i] != 0 ) {++i;}
    if (buf[i-1] < 32) {
      buf[i-1] = 0;
    }

    std::vector<std::string> tokens = tokenize(buf, '\t');

    // Remove empty lines
    if (buf[0] == 0) continue;

    // End when column header line is read
    if (isDataHeaderLine(tokens)) {
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

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CtfReader::transformData()
{
  float* p1 = getEuler1Pointer();
  float* p = getEuler2Pointer();
  float* p2 = getEuler3Pointer();

  int* ph = getPhasePointer();
  int* bCount = getBandCountPointer();
  int* error = getErrorPointer();
  float* mad = getMeanAngularDeviationPointer();
  int* bc = getBandContrastPointer();
  int* bs = getBandSlopePointer();

  size_t offset = 0;

  size_t yCells = getYCells();
  size_t xCells = getXCells();
  size_t totalDataRows = yCells * xCells;

  std::vector<size_t> shuffleTable(totalDataRows, 0);

  size_t i = 0;
  for(size_t row = 0; row < yCells; ++row)
   {
     for(size_t col = 0; col < xCells; ++col)
     {
     // Do we transform the data
       if (getUserOrigin() == Ebsd::UpperRightOrigin)
       {
         if (getUserZDir() == Ebsd::IntoSlice)
         {
         offset = (((xCells-1)-col)*yCells)+(row);
         if (p[i] - ONE_PIf < 0.0)
         {
           p[i] = p[i] + ONE_PIf;
         }
         else
         {
           p[i] = p[i] - ONE_PIf;
         }
         if (p2[i] - PI_OVER_2f < 0.0)
         {
           p2[i] = p2[i] + THREE_PI_OVER_2f;
         }
         else
         {
           p2[i] = p2[i] - PI_OVER_2f;
         }
         }
         if (getUserZDir() == Ebsd::OutofSlice)
         {
         offset = (col*yCells)+(row);
         if (p1[i] - ONE_PIf < 0.0)
         {
           p1[i] = p1[i] + ONE_PIf;
         }
         else
         {
           p1[i] = p1[i] - ONE_PIf;
         }
         }
       }
       else if (getUserOrigin() == Ebsd::UpperLeftOrigin)
       {
         if (getUserZDir() == Ebsd::IntoSlice)
         {
         offset = (row*xCells)+(col);
         if (p[i] - ONE_PIf < 0.0)
         {
           p[i] = p[i] + ONE_PIf;
         }
         else
         {
           p[i] = p[i] - ONE_PIf;
         }
         }
         if (getUserZDir() == Ebsd::OutofSlice)
         {
         offset = (row*xCells)+((xCells-1)-col);
         if (p1[i] - THREE_PI_OVER_2f < 0.0)
         {
           p1[i] = p1[i] + PI_OVER_2f;
         }
         else
         {
           p1[i] = p1[i] - THREE_PI_OVER_2f;
         }
         }
       }
       else if (getUserOrigin() == Ebsd::LowerLeftOrigin)
       {
         if (getUserZDir() == Ebsd::IntoSlice)
         {
         offset = (col*yCells)+((yCells-1)-row);
         if (p[i] - ONE_PIf < 0.0)
         {
           p[i] = p[i] + ONE_PIf;
         }
         else
         {
           p[i] = p[i] - ONE_PIf;
         }
         if (p2[i] - THREE_PI_OVER_2f < 0.0)
         {
           p2[i] = p2[i] + PI_OVER_2f;
         }
         else
         {
           p2[i] = p2[i] - THREE_PI_OVER_2f;
         }
         }
         if (getUserZDir() == Ebsd::OutofSlice)
         {
         offset = (((xCells-1)-col)*yCells)+((yCells-1)-row);
         }
       }
       else if (getUserOrigin() == Ebsd::LowerRightOrigin)
       {
         if (getUserZDir() == Ebsd::IntoSlice)
         {
         offset = (((yCells-1)-row)*xCells)+((xCells-1)-col);
         if (p[i] - ONE_PIf < 0.0)
         {
           p[i] = p[i] + ONE_PIf;
         }
         else
         {
           p[i] = p[i] - ONE_PIf;
         }
         if (p2[i] - ONE_PIf < 0.0)
         {
           p2[i] = p2[i] + ONE_PIf;
         }
         else
         {
           p2[i] = p2[i] - ONE_PIf;
         }
         }
         if (getUserZDir() == Ebsd::OutofSlice)
         {
         offset = (((yCells-1)-row)*xCells)+(col);
         if (p1[i] - PI_OVER_2f < 0.0)
         {
           p1[i] = p1[i] + THREE_PI_OVER_2f;
         }
         else
         {
           p1[i] = p1[i] - PI_OVER_2f;
         }
         }
       }

       if (getUserOrigin() == Ebsd::NoOrientation)
       {
       // If the user/programmer sets "NoOrientation" then we simply read the data
       // from the file and copy the values into the arrays without any regard for
       // the true X and Y positions in the grid. We are simply trying to keep the
       // data as close to the original as possible.
       offset = i;
       }
       shuffleTable[(row*xCells)+col] = offset;
       ++i;
     }
   }

  SHUFFLE_ARRAY(Phase, ph, int)
  SHUFFLE_ARRAY(BandCount, bCount, int)
  SHUFFLE_ARRAY(Error, error, int)
  SHUFFLE_ARRAY(Euler1, p1, float)
  SHUFFLE_ARRAY(Euler2, p, float)
  SHUFFLE_ARRAY(Euler3, p2, float)
  SHUFFLE_ARRAY(MeanAngularDeviation, mad, float)
  SHUFFLE_ARRAY(BandContrast, bc, int)
  SHUFFLE_ARRAY(BandSlope, bs, int)

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

