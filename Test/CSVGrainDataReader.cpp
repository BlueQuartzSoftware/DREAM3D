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

#include "CSVGrainDataReader.h"

#include <string.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>



#define PI_OVER_2f       1.57079632679489661f
#define THREE_PI_OVER_2f 4.71238898038468985f
#define TWO_PIf          6.28318530717958647f
#define ONE_PIf          3.14159265358979323f

#define kBufferSize 1024


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CSVGrainDataReader::CSVGrainDataReader() :
m_FileName(""),
m_NumberOfElements(0),
m_ManageMemory(true)
{
  // Init all the arrays to NULL
  m_GrainId = NULL;
  m_Phi1 = NULL;
  m_Phi = NULL;
  m_Phi2 = NULL;
  m_EquivDiam = NULL;
  m_B_Over_A = NULL;
  m_C_Over_A = NULL;
  m_Omega3 = NULL;
  m_OutsideBoundingBox = NULL;
  m_NumNeighbors = NULL;
  m_SurfaceGrain = NULL;

  m_NumFields = 11;

}

// -----------------------------------------------------------------------------
//  Clean up any Memory that was allocated for this class
// -----------------------------------------------------------------------------
CSVGrainDataReader::~CSVGrainDataReader()
{
  deletePointers();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CSVGrainDataReader::initPointers(size_t numElements)
{
  size_t numBytes = numElements * sizeof(float);
  m_GrainId = allocateArray<int > (numElements);
  m_Phi1 = allocateArray<float > (numElements);
  m_Phi = allocateArray<float > (numElements);
  m_Phi2 = allocateArray<float > (numElements);
  m_EquivDiam = allocateArray<float > (numElements);
  m_B_Over_A = allocateArray<float > (numElements);
  m_C_Over_A = allocateArray<float> (numElements);
  m_Omega3 = allocateArray<float > (numElements);
  m_OutsideBoundingBox = allocateArray<int > (numElements);
  m_NumNeighbors = allocateArray<int > (numElements);
  m_SurfaceGrain = allocateArray<int > (numElements);

  ::memset(m_GrainId, 0, numBytes);
  ::memset(m_Phi1, 0, numBytes);
  ::memset(m_Phi, 0, numBytes);
  ::memset(m_Phi2, 0, numBytes);
  ::memset(m_EquivDiam, 0, numBytes);
  ::memset(m_B_Over_A, 0, numBytes);
  ::memset(m_C_Over_A, 0, numBytes);
  ::memset(m_Omega3, 0, numBytes);
  ::memset(m_OutsideBoundingBox, 0, numBytes);
  ::memset(m_NumNeighbors, 0, numBytes);
  ::memset(m_SurfaceGrain, 0, numBytes);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CSVGrainDataReader::deletePointers()
{
  this->deallocateArrayData<int > (m_GrainId);
  this->deallocateArrayData<float > (m_Phi1);
  this->deallocateArrayData<float > (m_Phi);
  this->deallocateArrayData<float > (m_Phi2);
  this->deallocateArrayData<float > (m_EquivDiam);
  this->deallocateArrayData<float > (m_B_Over_A);
  this->deallocateArrayData<float > (m_C_Over_A);
  this->deallocateArrayData<float > (m_Omega3);
  this->deallocateArrayData<int > (m_OutsideBoundingBox);
  this->deallocateArrayData<int > (m_NumNeighbors);
  this->deallocateArrayData<int > (m_SurfaceGrain);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int CSVGrainDataReader::readHeaderOnly()
{
  int err = 1;
  char buf[kBufferSize];
  std::ifstream in(m_FileName.c_str());
  m_headerComplete = false;
  if (!in.is_open())
  {
    std::cout << "CSV file could not be opened: " << m_FileName << std::endl;
    return -100;
  }

  while (!in.eof() && !m_headerComplete)
  {
    ::memset(buf, 0, kBufferSize);
    in.getline(buf, kBufferSize);
    parseHeaderLine(buf, kBufferSize);
  }
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int CSVGrainDataReader::readFile()
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

    ::memset(buf, 0, kBufferSize);
    in.getline(buf, kBufferSize);
    if (sscanf(buf, "%lld", &m_NumberOfElements) != 1)
    {
      std::cout << "First Line of file not parsed." << std::endl;
    }

    ::memset(buf, 0, kBufferSize);
    in.getline(buf, kBufferSize);
    parseHeaderLine(buf, kBufferSize);

  // Delete any currently existing pointers
  deletePointers();

  initPointers(m_NumberOfElements);

  if (NULL == m_GrainId ||
      NULL == m_Phi1 ||
      NULL == m_Phi ||
      NULL == m_Phi2 ||
      NULL == m_EquivDiam ||
      NULL == m_NumNeighbors ||
      NULL == m_B_Over_A ||
      NULL == m_C_Over_A ||
      m_Omega3 == NULL ||
      m_OutsideBoundingBox == NULL)
  {
    return -1;
  }

  size_t counter = 0;
  for(size_t row = 0; row < m_NumberOfElements && in.eof() == false; ++row)
  {
    in.getline(buf, kBufferSize);
    this->readData(buf, static_cast<int>(row), counter);
    // Read the next line of data
    ++counter;
  }


  if (counter != m_NumberOfElements && in.eof() == true)
  {

    std::cout << "Premature End Of File reached.\n" << m_FileName
        << "\nNumRows=" << m_NumberOfElements
        << "\ncounter=" << counter << " m_NumberOfElements=" << m_NumberOfElements
        << "\nTotal Data Points Read=" << counter << std::endl;
  }

  return err;
}

// -----------------------------------------------------------------------------
//  Read the Header part of the ANG file
// -----------------------------------------------------------------------------
void CSVGrainDataReader::parseHeaderLine(char* buf, size_t length)
{
  if (buf[0] != '#')
  {
    m_headerComplete = true;
  }

}

// -----------------------------------------------------------------------------
//  Read the data part of the ANG file
// -----------------------------------------------------------------------------
void CSVGrainDataReader::readData(const std::string &line, int row, size_t i)
{
  /* When reading the data there should be at least 8 cols of data. There may even
   * be 11 columns of data. The column names should be the following:
   * Grain ID
   * phi1
   * phi
   * phi2
   *
   *
   */
  float  p1, p, p2, eq, ba, ca, o3;

  int gid, obb, nn, sg;

  m_NumFields = sscanf(line.c_str(), "%d,%f,%f,%f,%f,%f,%f,%f,%d,%d,%d", &gid, &p1, &p,&p2, &eq, &ba, &ca, &o3, &obb, &nn, &sg);


  m_GrainId[row] = gid;
  m_Phi1[row] = p1;
  m_Phi[row] = p;
  m_Phi2[row] = p2;
  m_EquivDiam[row] = eq;
  m_B_Over_A[row] = ba;
  m_C_Over_A[row] = ca;
  m_Omega3[row] = o3;
  m_OutsideBoundingBox[row] = obb;
  m_NumNeighbors[row] = nn;
  m_SurfaceGrain[row] = sg;
}

