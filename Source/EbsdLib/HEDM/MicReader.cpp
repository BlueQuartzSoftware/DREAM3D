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


#include "MicConstants.h"
#include "EbsdLib/EbsdMacros.h"
#include "EbsdLib/EbsdMath.h"



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
  std::ifstream in(getFileName().c_str());
  setHeaderIsComplete(false);
  if (!in.is_open())
  {
    std::cout << "Mic file could not be opened: " << getFileName() << std::endl;
    return -100;
  }
  std::string origHeader;
  setOriginalHeader(origHeader);
  m_PhaseVector.clear();

  while (!in.eof() && false == getHeaderIsComplete())
  {
    ::memset(buf, 0, kBufferSize);
    in.getline(buf, kBufferSize);
    parseHeaderLine(buf, kBufferSize);
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
  int err = 1;
  char buf[kBufferSize];
  std::ifstream in(getFileName().c_str());
  setHeaderIsComplete(false);
  if (!in.is_open())
  {
    std::cout << "Mic file could not be opened: " << getFileName() << std::endl;
    return -100;
  }

  std::string origHeader;
  setOriginalHeader(origHeader);
  m_PhaseVector.clear();

  m_CurrentPhase = MicPhase::New();
  m_PhaseVector.push_back(m_CurrentPhase);

  while (!in.eof() && false == getHeaderIsComplete())
  {
    ::memset(buf, 0, kBufferSize);
    in.getline(buf, kBufferSize);
    int i = 0;
    while (buf[i] != 0) { ++i; }
    buf[i] = 10; //Add back in the \n character
    parseHeaderLine(buf, kBufferSize);
    if (getHeaderIsComplete() == false) {
      origHeader.append(buf);
    }
  }
  // Update the Original Header variable
  setOriginalHeader(origHeader);

  // We need to pass in the buffer because it has the first line of data
  err = readData(in, buf, kBufferSize);
  if (err < 0) { return err;}

  if(getRotateSlice() == true || getReorderArray() == true || getAlignEulers() == true)
  {
    transformData();
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
  size_t fieldsRead = 0;
  float origEdgeLength;
  float xMax = 0, yMax = 0;
  float xMin = 1000000000, yMin = 1000000000;
  size_t counter = 0;

  // The buf variable already has the first line of data in it
  fieldsRead = sscanf(buf, "%d", &totalDataRows);

  initPointers(totalDataRows);
  if (NULL == m_Euler1 || NULL == m_Euler2 || NULL == m_Euler3
      || NULL == m_Conf || NULL == m_Phase || NULL == m_Level  || m_X == NULL || m_Y == NULL)
  {
    std::cout << "Internal pointers were NULL at " __FILE__ << "(" << __LINE__ << ")" << std::endl;
    return -1;
  }

  ::memset(buf, 0, bufSize); // Clear the buffer
  in.getline(buf, kBufferSize);// Read the next line of data
  fieldsRead = sscanf(buf, "%f", &origEdgeLength);
  ::memset(buf, 0, bufSize); // Clear the buffer
  in.getline(buf, kBufferSize);// Read the next line of data
  this->parseDataLine(buf, 0);
  ::memset(buf, 0, bufSize); // Clear the buffer
  in.getline(buf, kBufferSize);// Read the next line of data
  int level = m_Level[0];
  float newEdgeLength = origEdgeLength/powf(2,float(level));
  for(size_t i = 1; i < totalDataRows; ++i)
  {
      this->parseDataLine(buf, i);
      ::memset(buf, 0, bufSize); // Clear the buffer
      in.getline(buf, kBufferSize);// Read the next line of data
      ++counter;
      if (in.eof() == true) break;
  }

  if (counter != totalDataRows && in.eof() == true)
  {

    std::cout << "Premature End Of File reached.\n"
        << getFileName()
        << "\ncounter=" << counter << " totalDataRows=" << totalDataRows
        << "\nTotal Data Points Read=" << counter << std::endl;
  }

  std::vector<float> EA1(totalDataRows,0.0);
  std::vector<float> EA2(totalDataRows,0.0);
  std::vector<float> EA3(totalDataRows,0.0);
  std::vector<float> confidence(totalDataRows,0.0);
  std::vector<int> phase(totalDataRows,0);
  std::vector<float> xVal(totalDataRows,0.0);
  std::vector<float> yVal(totalDataRows,0.0);
  float constant = 1.0/(2.0*sqrt(3.0));
  for(size_t i = 0; i < totalDataRows; ++i)
  {
	if(m_Up[i] == 1)
	{
		m_X[i] = m_X[i] + (newEdgeLength/2.0);
		m_Y[i] = m_Y[i] + (constant*newEdgeLength);
	}
	if(m_Up[i] == 2)
	{
		m_X[i] = m_X[i] + (newEdgeLength/2.0);
		m_Y[i] = m_Y[i] - (constant*newEdgeLength);
	}
	if(m_X[i] > xMax) xMax = m_X[i];
	if(m_Y[i] > yMax) yMax = m_Y[i];
	if(m_X[i] < xMin) xMin = m_X[i];
	if(m_Y[i] < yMin) yMin = m_Y[i];
	EA1[i] = m_Euler1[i];
	EA2[i] = m_Euler2[i];
	EA3[i] = m_Euler3[i];
	confidence[i] = m_Conf[i];
	phase[i] = m_Phase[i];
	xVal[i] = m_X[i];
	yVal[i] = m_Y[i];
  }
  xDim = int((xMax-xMin)/newEdgeLength)+1;
  yDim = int((yMax-yMin)/newEdgeLength)+1;
  xRes = newEdgeLength;
  yRes = newEdgeLength;

    // Delete any currently existing pointers 
  deletePointers();
    // Resize pointers 
  initPointers(xDim*yDim);


  float x, y;
  int col, row, point;
  for(size_t i = 0; i < totalDataRows; ++i)
  {
     x = xVal[i]-xMin;
     y = yVal[i]-yMin;
	 col = int(x/newEdgeLength);
	 row = int(y/newEdgeLength);
	 point = (row*xDim) + col;
	 m_Euler1[point] = EA1[i];
	 m_Euler2[point] = EA2[i];
	 m_Euler3[point] = EA3[i];
	 m_Conf[point] = confidence[i];
	 m_Phase[point] = phase[i];
	 m_X[point] = col*newEdgeLength;
	 m_Y[point] = row*newEdgeLength;
  }


  return 0;
}

// -----------------------------------------------------------------------------
//  Read the Header part of the Mic file
// -----------------------------------------------------------------------------
void MicReader::parseHeaderLine(char* buf, size_t length)
{
  if (buf[0] != '#')
  {
    setHeaderIsComplete(true);
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
  int ph = 1, up = 0, level = 0, good = 0;
  size_t offset = 0;
  size_t fieldsRead = 0;
  fieldsRead = sscanf(line.c_str(), "%f %f %f %d %d %d %f %f %f %f", &x, &y,&z, &up, &level, &good, &p1, &p, &p2, &conf);

  offset = i;

  m_Euler1[offset] = p1;
  m_Euler2[offset] = p;
  m_Euler3[offset] = p2;
  m_Conf[offset] = conf;
  m_Phase[offset] = ph;
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
  return xDim;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int MicReader::getYDimension()
{
  return yDim;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MicReader::setXDimension(int xD)
{
  xDim = xD;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MicReader::setYDimension(int yD)
{
  yDim = yD;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
float MicReader::getXStep()
{
  return xRes;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
float MicReader::getYStep()
{
  return yRes;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MicReader::transformData()
{
  float* p1 = getEuler1Pointer();
  float* p = getEuler2Pointer();
  float* p2 = getEuler3Pointer();

  int* ph = getPhasePointer();
  float* conf = getConfidencePointer();

  size_t offset = 0;

  size_t yCells = getYDimension();
  size_t xCells = getYDimension();
  yCells = 429;
  xCells = 431;
  size_t totalDataRows = yCells * xCells;

  std::vector<size_t> shuffleTable(totalDataRows, 0);

  size_t i = 0;
  size_t adjustedcol, adjustedrow;
  for(size_t row = 0; row < yCells; ++row)
   {
     for(size_t col = 0; col < xCells; ++col)
     {
     adjustedcol = col;
     adjustedrow = row;
     if(getRotateSlice() == true) adjustedcol = (xCells-1)-adjustedcol, adjustedrow = (yCells-1)-adjustedrow;
     if(getReorderArray() == true) adjustedrow = (yCells-1)-adjustedrow;
       offset = (adjustedrow*xCells)+(adjustedcol);
       if(getAlignEulers() == true)
     {
       if (p1[i] - M_PI_2 < 0.0f)
       {
        p1[i] = static_cast<float>( p1[i] + M_PI_2*3.0f );
       }
       else
       {
        p1[i] = static_cast<float>( p1[i] - M_PI_2 );
       }
     }
       shuffleTable[(row*xCells)+col] = offset;
       ++i;
      }
    }

  SHUFFLE_ARRAY(Euler1, p1, float)
  SHUFFLE_ARRAY(Euler2, p, float)
  SHUFFLE_ARRAY(Euler3, p2, float)
  SHUFFLE_ARRAY(Confidence, conf, float)
  SHUFFLE_ARRAY(Phase, ph, int)

}
