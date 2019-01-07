/* ============================================================================
 * Copyright (c) 2019 BlueQuartz Software, LLC
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
 * Neither the names of any of the BlueQuartz Software contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
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
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "EspritPhase.h"

#include <cstring>

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EspritPhase::EspritPhase()
: m_PhaseIndex(-1)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EspritPhase::~EspritPhase() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString EspritPhase::getMaterialName()
{
  return m_Name;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
unsigned int EspritPhase::determineLaueGroup()
{
  int sg = getIT();

  if(sg >= 1 && sg <= 2)
  {
    return Ebsd::CrystalStructure::Triclinic;
  }

  if(sg >= 3 && sg <= 15)
  {
    return Ebsd::CrystalStructure::Monoclinic;
  }

  if(sg >= 16 && sg <= 74)
  {
    return Ebsd::CrystalStructure::OrthoRhombic;
  }

  if(sg >= 75 && sg <= 89)
  {
    return Ebsd::CrystalStructure::Tetragonal_Low;
  }
  if(sg >= 90 && sg <= 142)
  {
    return Ebsd::CrystalStructure::Tetragonal_High;
  }

  if(sg >= 143 && sg <= 148)
  {
    return Ebsd::CrystalStructure::Trigonal_Low;
  }
  if(sg >= 149 && sg <= 167)
  {
    return Ebsd::CrystalStructure::Trigonal_High;
  }

  if(sg >= 168 && sg <= 176)
  {
    return Ebsd::CrystalStructure::Hexagonal_Low;
  }
  if(sg >= 177 && sg <= 194)
  {
    return Ebsd::CrystalStructure::Hexagonal_High;
  }

  if(sg >= 195 && sg <= 206)
  {
    return Ebsd::CrystalStructure::Cubic_Low;
  }
  if(sg >= 207 && sg <= 230)
  {
    return Ebsd::CrystalStructure::Cubic_High;
  }

  return Ebsd::CrystalStructure::UnknownCrystalStructure;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EspritPhase::parseFormula(QList<QByteArray>& tokens)
{
  m_Formula.clear();
  for(int i = 1; i < tokens.size(); ++i)
  {
    m_Formula.append(tokens.at(i)).append(" ");
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EspritPhase::parseName(QList<QByteArray>& tokens)
{
  m_Name.clear();
  for(int i = 1; i < tokens.size(); ++i)
  {
    m_Name.append(tokens.at(i)).append(" ");
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EspritPhase::parseSpaceGroup(QList<QByteArray>& tokens)
{
  m_SpaceGroup.clear();
  for(int i = 1; i < tokens.size(); ++i)
  {
    m_SpaceGroup.append(tokens.at(i)).append(" ");
  }
}
