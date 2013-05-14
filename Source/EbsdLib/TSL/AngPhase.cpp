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





#include "AngPhase.h"
#include <string.h>

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AngPhase::AngPhase() :
m_PhaseIndex(-1)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AngPhase::~AngPhase()
{
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AngPhase::parsePhase(char* value, size_t start, size_t length)
{
  if (value[start] == ':')
  {
    ++start;
  } // move past the ":" character
  std::string data(&(value[start]), strlen(value) - start);
  std::stringstream ss(data);
  ss >> m_PhaseIndex;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AngPhase::parseMaterialName(char* value, size_t start, size_t length)
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
  std::string data(&(value[start]), len - start);
  m_MaterialName = data;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AngPhase::parseFormula(char* value, size_t start, size_t length)
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
  std::string data(&(value[start]), len - start);
  m_Formula = data;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AngPhase::parseInfo(char* value, size_t start, size_t length)
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
  std::string data(&(value[start]), len - start);
  m_Info = data;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AngPhase::parseSymmetry(char* value, size_t start, size_t length)
{
  if (value[start] == ':')
  {
    ++start;
  } // move past the ":" character
  std::string data(&(value[start]), strlen(value) - start);
  std::stringstream ss(data);
  unsigned int symm;
  ss >> symm;
  m_Symmetry = symm;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AngPhase::parseLatticeConstants(char* value, size_t start, size_t length)
{
  m_LatticeConstants.clear();

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
  std::string data(&(value[start]), len - start);
  std::stringstream ss(data);
  float lc;
  while(ss.good() )
  {
    ss >> lc;
    m_LatticeConstants.push_back(lc);
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AngPhase::parseNumberFamilies(char* value, size_t start, size_t length)
{
  if (value[start] == ':')
  {
    ++start;
  } // move past the ":" character
  std::string data(&(value[start]), strlen(value) - start);
  std::stringstream ss(data);
  ss >> m_NumberFamilies;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AngPhase::parseHKLFamilies(char* value, size_t start, size_t length)
{
  HKLFamily::Pointer family = HKLFamily::New();
  if (value[start] == ':')
  {
    ++start;
  } // move past the ":" character
  std::string data(&(value[start]), strlen(value) - start);
  std::stringstream ss(data);

  while(ss.good() )
  {
    ss >> family->h;
    ss >> family->k;
    ss >> family->l;
    ss >> family->s1;
    ss >> family->diffractionIntensity;
    ss >> family->s2;
  }
  if (family->s1 > 1) { family->s1 = 1; }
  if (family->s2 > 1) { family->s2 = 1; }
  m_HKLFamilies.push_back(family);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AngPhase::parseCategories(char* value, size_t start, size_t length)
{
  m_Categories.clear();

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
  std::string data(&(value[start]), len - start);
  std::stringstream ss(data);
  int cat;
  while(ss.good() )
  {
    ss >> cat;
    m_Categories.push_back(cat);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AngPhase::printSelf(std::ostream &stream)
{
  stream << Ebsd::Ang::Phase << ": " << m_PhaseIndex << std::endl;
  stream << Ebsd::Ang::MaterialName << ": " << m_MaterialName << std::endl;
  stream << Ebsd::Ang::Formula << ": " << m_Formula << std::endl;
  stream << Ebsd::Ang::Info << ": " << m_Info << std::endl;
  stream << Ebsd::Ang::Symmetry << ": " << m_Symmetry << std::endl;

  stream << Ebsd::Ang::LatticeConstants;
  for (std::vector<float>::iterator iter = m_LatticeConstants.begin(); iter != m_LatticeConstants.end(); ++iter )
  {
    stream << " " << *iter;
  }
  stream << std::endl;

  stream << Ebsd::Ang::NumberFamilies << ": " << m_NumberFamilies << std::endl;


  for (std::vector<HKLFamily::Pointer>::iterator iter = m_HKLFamilies.begin(); iter != m_HKLFamilies.end(); ++iter )
  {
   (*iter)->printSelf(stream);
  }


  stream << Ebsd::Ang::Categories;
  for (std::vector<int>::iterator iter = m_Categories.begin(); iter != m_Categories.end(); ++iter )
  {
    stream << " " << *iter;
  }
  stream << std::endl;

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
unsigned int AngPhase::determineCrystalStructure()
{
  uint32_t symmetry = getSymmetry();
  unsigned int crystal_structure = Ebsd::CrystalStructure::UnknownCrystalStructure;

  switch(symmetry)
  {
    case Ebsd::Ang::PhaseSymmetry::Cubic:
      crystal_structure = Ebsd::CrystalStructure::Cubic;
      break;
    case Ebsd::Ang::PhaseSymmetry::Hexagonal:
    case Ebsd::Ang::PhaseSymmetry::DiHexagonal:
      crystal_structure = Ebsd::CrystalStructure::Hexagonal;
      break;
    case Ebsd::Ang::PhaseSymmetry::Trigonal:
    case Ebsd::Ang::PhaseSymmetry::DiTrigonal:
      crystal_structure = Ebsd::CrystalStructure::Trigonal;
      break;
    default:
      crystal_structure = Ebsd::CrystalStructure::UnknownCrystalStructure;

  }
  return crystal_structure;
}
