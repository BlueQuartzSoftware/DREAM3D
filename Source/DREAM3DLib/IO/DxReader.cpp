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
 *  This code was written under United States Air Force Contract number
 *                           FA8650-07-D-5800
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include "DxReader.h"

#include <iostream>
#include <fstream>

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DxReader::DxReader()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DxReader::~DxReader()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int DxReader::readFile()
{
  std::string line;
  std::string delimeters(", ;\t"); /* delimeters to split the data */
  std::vector<std::string> tokens; /* vector to store the split data */
  //std::vector<int> data; /* vector to store the data */

  int error, spin; /* dummy variables */
  //int nx, ny, nz;

  std::ifstream inFile;
  inFile.open(getFileName().c_str());
  if(!inFile)
  {
    std::cout << "Failed to open: " << getFileName() << std::endl;
    exit(0);
  }

  getline(inFile, line, '\n');
  tokenize(line, tokens, delimeters);

  // Process the header information and look for the std::string "counts"
  // Then read the data size after that
  int pos1 = 0;
  while (pos1 == 0)
  { // continue until we find the keyword
    for (size_t i = 0; i < tokens.size(); i++)
    {
      if(tokens[i] == "counts")
      {
        pos1 = i;
      }
    }
    // Read the next line of the header if we did not find the keyword
    // in the line
    if(pos1 == 0)
    {
      tokens.clear();
      getline(inFile, line, '\n');
      tokenize(line, tokens, delimeters);
      if(tokens.size() == 20)
      {
        std::cout << "ERROR: Unable to read data dimensions from the header" << std::endl;
        inFile.close();
        return -1;
      }
    }

  }

  int nx = 0;
  int ny = 0;
  int nz = 0;

  if(pos1 != 0)
  {
    error = 0;
    error += sscanf(tokens[pos1 + 1].c_str(), "%d", &nz);
    error += sscanf(tokens[pos1 + 2].c_str(), "%d", &ny);
    error += sscanf(tokens[pos1 + 3].c_str(), "%d", &nx);
    tokens.clear();
    // The dimensions listed in the DX file are always one greater
    // than the actual dimensions
    nx--;
    ny--;
    nz--;
  }

  std::cout << "INFO: DX data dimensions: " << std::endl;
  std::cout << "nz= " << nz << std::endl;
  std::cout << "ny= " << ny << std::endl;
  std::cout << "nx= " << nx << std::endl;

  //The DX file has a unique format of 20 entries on each line. I have
  //no idea who initiated this insanity but I am about to perpetuate
  //it.
  //
  //The most simple thing to do is to read the entire dataset into one
  //long vector and then read that vector to assign values to the grid

  //  ADR:  6 Sep 08; time to make the input much more general!
  //  equivalent to list-direcvted input in Fortran, actually !!

  pos1 = 0;
  while (pos1 == 0)
  { // continue until we find the keyword
    for (size_t i = 0; i < tokens.size(); i++)
    {
      if(tokens[i] == "items")
      {
        pos1 = i;
      }
    }
    // Read the next line of the header if we did not find the keyword
    // in the line
    if(pos1 == 0)
    {
      tokens.clear();
      getline(inFile, line, '\n');
      tokenize(line, tokens, delimeters);
      if(tokens.size() == 20)
      {
        std::cout << "ERROR: Unable to locate the last header line" << std::endl;
        inFile.close();
        return -1;
      }
    }
  } // when we get here, we are looking at data
  int points = 0;
  if(pos1 != 0)
  {
    error = 0;
    error += sscanf(tokens[pos1 + 1].c_str(), "%d", &points);
    tokens.clear();
  }
  std::cout << "Compare no. points " << points << " with x*y*z: " << nx * ny * nz << std::endl;

  bool finished_header, finished_data;
  finished_header = true;
  //  finished_header = false;
  finished_data = false;
  size_t index = 0;
  setDimensions(nx, ny, nz);
  m_Data = AIMArray<int>::CreateArray(nx * ny * nz);
  while (getline(inFile, line, '\n') != NULL)
  {

    // Get the remaining lines of the header and ignore
    tokens.clear();
    error = 0;
    tokenize(line, tokens, delimeters);

    //    if(tokens.size()==20)
    //      finished_header = true;

    if( static_cast<int>(index) == nz * ny * nx || finished_header && tokens.size() != 0 && tokens[0] == "attribute" )
    {
      finished_data = true;
    }

    // Allocate the AIMArray at this point:



    if(finished_header && !finished_data)
    {
      for (size_t in_spins = 0; in_spins < tokens.size(); in_spins++)
      {
        error += sscanf(tokens[in_spins].c_str(), "%d", &spin);
        m_Data->SetValue(index, spin);
        ++index;
      }
    }
  }

  if(index != static_cast<size_t>(nz * ny * nx))
  {
    std::cout << "ERROR: data size does not match header dimensions" << std::endl;
    std::cout << "\t" << index << "\t" << nz * nx * ny << std::endl;
    return -1;
    inFile.close();
  }

  tokens.clear();
  inFile.close();
  return 0;
}

