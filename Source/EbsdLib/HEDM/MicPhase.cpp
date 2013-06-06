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





#include "MicPhase.h"
#include <string.h>

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MicPhase::MicPhase() :
m_PhaseName(""),
m_PhaseIndex(1)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MicPhase::~MicPhase()
{
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MicPhase::printSelf(std::ostream &stream)
{


}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MicPhase::parseString(char* value, size_t start, size_t length, std::string &data)
{
  if (value[start] == ':')
  {
    ++start;
  } // move past the ":" character
  while (value[start] == ' ' || value[start] == '\t')
  {
    ++start;
  }

  size_t len = strlen(value);
  // Strip off training new line and carriage returns
  if (value[len-1] == '\r' || value[len-1] == '\n')
  {
    len--;
  }
  if (value[len-1] == '\r' || value[len-1] == '\n')
  {
    len--;
  }
  std::string data2(&(value[start]), len - start);
  data = data2;
}

/**
     * @brief Converts a string to a number
     */
template<typename T>
bool stringToNum(T &t, const std::string &s)
{
  // Filter the line to convert European comma style decimals to US/UK style points
  std::vector<char> cLine(s.size()+1);
  ::memcpy( &(cLine.front()), s.c_str(), s.size() + 1);
  for (size_t c = 0; c < cLine.size(); ++c)
  {
    if (cLine[c] == ',') { cLine[c] = '.';}
  }
  std::istringstream iss(std::string( &(cLine.front()) ) );
  return !(iss >> t).fail();
}

/**
* @brief Parses a header line into string "tokens"
*/
template<typename T>
std::vector<T> tokenize(const std::string &values, char delimiter)
{
  std::vector<T> output;
  std::string::size_type start = 0;
  std::string::size_type pos = 0;
  while(pos != std::string::npos && pos != values.size() - 1)
  {
    pos = values.find(delimiter, start);
    T value = 0;
    stringToNum(value, values.substr(start, pos-start));
    output.push_back(value);
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
void MicPhase::parseLatticeConstants(char* value, size_t start, size_t length)
{
  std::string data;
  parseString(value, start, length, data);
  std::vector<float> constants = tokenize<float>(data, ',');
  std::cout << "size: " << constants.size() << std::endl;
  std::cout << "data: " << data << std::endl;
  m_LatticeConstants.resize(6);
  m_LatticeConstants[0] = constants[0];
  m_LatticeConstants[1] = constants[1];
  m_LatticeConstants[2] = constants[2];

  std::cout << m_LatticeConstants[0] << ", " << m_LatticeConstants[2] <<  "," << m_LatticeConstants[2] << std::endl;

  //m_LatticeConstants = data;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MicPhase::parseLatticeAngles(char* value, size_t start, size_t length)
{
  std::string data;
  parseString(value, start, length, data);
  std::cout << "data: " << data << std::endl;
  std::vector<float> constants = tokenize<float>(data, ',');
    m_LatticeConstants[3] = constants[0];
  m_LatticeConstants[4] = constants[1];
  m_LatticeConstants[5] = constants[2];
std::cout << m_LatticeConstants[4] << ", " << m_LatticeConstants[5] <<  "," << m_LatticeConstants[6] << std::endl;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MicPhase::parseBasisAtoms(char* value, size_t start, size_t length)
{
  std::string data;
  parseString(value, start, length, data);
  m_BasisAtoms = data;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MicPhase::parseZandCoordinates(char* value, size_t start, size_t length)
{
  std::string data;
  parseString(value, start, length, data);
  m_ZandCoordinates.push_back(data);
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
unsigned int MicPhase::determineCrystalStructure()
{
  unsigned int crystal_structure;
  if(m_Symmetry.compare(Ebsd::Mic::Cubic) == 0) crystal_structure = Ebsd::CrystalStructure::Cubic_High;
  else if(m_Symmetry.compare(Ebsd::Mic::Hexagonal) == 0) crystal_structure = Ebsd::CrystalStructure::Hexagonal_High;
  else if(m_Symmetry.compare(Ebsd::Mic::OrthoRhombic) == 0) crystal_structure = Ebsd::CrystalStructure::OrthoRhombic;
  else if(m_Symmetry.compare(Ebsd::Mic::Tetragonal) == 0) crystal_structure = Ebsd::CrystalStructure::UnknownCrystalStructure;
  else crystal_structure = Ebsd::CrystalStructure::UnknownCrystalStructure;

  return crystal_structure;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::string MicPhase::getMaterialName()
{
  std::string name = "Nickel";
  m_PhaseName = name;

  return m_PhaseName;
}
