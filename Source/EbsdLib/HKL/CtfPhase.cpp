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




#include "CtfPhase.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CtfPhase::CtfPhase() :
m_PhaseIndex(-1),
m_PhaseName("-1")
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CtfPhase::~CtfPhase()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CtfPhase::parsePhase(const std::vector<std::string> &tokens)
{
  m_LatticeDimensions = tokenize<float>(tokens[0], ';');
  m_LatticeAngles = tokenize<float>(tokens[1], ';');
  m_PhaseName = tokens[2];
  unsigned int sym = 999;
  stringToNum<unsigned int>(sym, tokens[3]);
  m_LaueGroup = static_cast<Ebsd::Ctf::LaueGroupTable>(sym);
  if(tokens.size() == 8)
  {
    stringToNum<int>(m_SpaceGroup, tokens[4]);
    m_Internal1 = tokens[5];
    m_Internal2 = tokens[6];
    m_Comment = tokens[7];
  }
  if(tokens.size() == 5)
  {
    m_Comment = tokens[4];
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CtfPhase::printSelf(std::ostream &stream)
{
  stream << Ebsd::Ctf::LatticeDimensions << " " << m_LatticeDimensions[0] << ", " << m_LatticeDimensions[1] << ", " << m_LatticeDimensions[2] << std::endl;
  stream << Ebsd::Ctf::LatticeAngles << " " << m_LatticeAngles[0] << ", " << m_LatticeAngles[1] << ", " << m_LatticeAngles[2] << std::endl;
  stream << Ebsd::Ctf::PhaseName << " " << m_PhaseName << std::endl;
  stream << Ebsd::Ctf::LaueGroup << " " << m_LaueGroup << std::endl;
  stream << Ebsd::Ctf::SpaceGroup << " " << m_SpaceGroup << std::endl;
  stream << Ebsd::Ctf::Internal1 << " " << m_Internal1 << std::endl;
  stream << Ebsd::Ctf::Internal2 << " " << m_Internal2 << std::endl;
  stream << Ebsd::Ctf::Comment << " " << m_Comment << std::endl;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
unsigned int CtfPhase::determineCrystalStructure()
{
  Ebsd::Ctf::LaueGroupTable symmetry = getLaueGroup();
  unsigned int crystal_structure = Ebsd::CrystalStructure::UnknownCrystalStructure;
  if (symmetry == Ebsd::Ctf::LG_Cubic_High || symmetry == Ebsd::Ctf::LG_Cubic_Low) crystal_structure = Ebsd::CrystalStructure::Cubic;
  else if (symmetry == Ebsd::Ctf::LG_Hexagonal_High || symmetry == Ebsd::Ctf::LG_Hexagonal_Low) crystal_structure = Ebsd::CrystalStructure::Hexagonal;
  return crystal_structure;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::string CtfPhase::getMaterialName()
{
  return m_PhaseName;
}
