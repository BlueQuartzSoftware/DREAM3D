/* ============================================================================
* Copyright (c) 2009-2016 BlueQuartz Software, LLC
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
* Neither the name of BlueQuartz Software, the US Air Force, nor the names of its
* contributors may be used to endorse or promote products derived from this software
* without specific prior written permission.
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
* The code contained herein was partially funded by the followig contracts:
*    United States Air Force Prime Contract FA8650-07-D-5800
*    United States Air Force Prime Contract FA8650-10-D-5210
*    United States Prime Contract Navy N00173-07-C-2068
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */






#include "AngPhase.h"
#include <string.h>

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AngPhase::AngPhase() :
  m_PhaseIndex(-1),
  m_Symmetry(0),
  m_NumberFamilies(0)
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
//void AngPhase::parsePhase(char* value, size_t start, size_t length)
//{
//  if (value[start] == ':')
//  {
//    ++start;
//  } // move past the ":" character
//  QByteArray buf(&(value[start]), strlen(value) - start);
//  bool ok = false;
//  m_PhaseIndex = buf.toInt(&ok, 10);
//}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AngPhase::parseMaterialName(QList<QByteArray>& tokens)
{
  m_MaterialName.clear();
  for(int i = 1; i < tokens.size(); ++i)
  {
    m_MaterialName.append(tokens.at(i)).append(" ");
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AngPhase::parseFormula(QList<QByteArray>& tokens)
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
void AngPhase::parseInfo(QList<QByteArray>& tokens)
{
  m_Info.clear();
  for(int i = 1; i < tokens.size(); ++i)
  {
    m_Info.append(tokens.at(i)).append(" ");
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//void AngPhase::parseSymmetry(char* value, size_t start, size_t length)
//{
//  if (value[start] == ':')
//  {
//    ++start;
//  } // move past the ":" character
//  QByteArray data(&(value[start]), strlen(value) - start);
//  bool ok = false;
//  m_Symmetry = data.toUInt(&ok, 10);
//}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AngPhase::parseLatticeConstants(QList<QByteArray>& tokens)
{
  m_LatticeConstants.clear();

  bool ok = false;
  m_LatticeConstants.push_back(tokens[1].toFloat(&ok)); //A
  m_LatticeConstants.push_back(tokens[2].toFloat(&ok)); //B
  m_LatticeConstants.push_back(tokens[3].toFloat(&ok)); //C
  m_LatticeConstants.push_back(tokens[4].toFloat(&ok)); // Alpha
  m_LatticeConstants.push_back(tokens[5].toFloat(&ok)); // Beta
  m_LatticeConstants.push_back(tokens[6].toFloat(&ok)); // Gamma

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//void AngPhase::parseNumberFamilies(char* value, size_t start, size_t length)
//{
//  if (value[start] == ':')
//  {
//    ++start;
//  } // move past the ":" character
//  QByteArray data(&(value[start]), strlen(value) - start);
//  bool ok = false;
//  m_NumberFamilies = data.toInt(&ok, 10);
//}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AngPhase::parseHKLFamilies(QList<QByteArray>& tokens)
{
  HKLFamily::Pointer family = HKLFamily::New();

  bool ok = false;
  family->h = tokens[1].toInt(&ok, 10);
  family->k = tokens[2].toInt(&ok, 10);
  family->l = tokens[3].toInt(&ok, 10);
  family->s1 = tokens[4].toInt(&ok, 10);
  family->diffractionIntensity = tokens[5].toFloat(&ok);
  if (tokens.size() > 6)
  {
    family->s2 = tokens[6].toInt(&ok, 10);
  }
  if (family->s1 > 1) { family->s1 = 1; }
  if (family->s2 > 1) { family->s2 = 1; }
  m_HKLFamilies.push_back(family);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AngPhase::parseCategories(QList<QByteArray>& tokens)
{
  m_Categories.clear();
  bool ok = false;
  if(tokens[0].size() != Ebsd::Ang::Categories.size())
  {
    tokens[0].replace(Ebsd::Ang::Categories, "");
    m_Categories.push_back(tokens.at(0).toInt(&ok, 10));
  }
  for(int i = 1; i < tokens.size(); ++i)
  {
    m_Categories.push_back(tokens.at(i).toInt(&ok, 10));
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AngPhase::printSelf(QTextStream& stream)
{
  stream << Ebsd::Ang::Phase << ": " << m_PhaseIndex << QString("\n");
  stream << Ebsd::Ang::MaterialName << ": " << m_MaterialName << QString("\n");
  stream << Ebsd::Ang::Formula << ": " << m_Formula << QString("\n");
  stream << Ebsd::Ang::Info << ": " << m_Info << QString("\n");
  stream << Ebsd::Ang::Symmetry << ": " << m_Symmetry << QString("\n");

  stream << Ebsd::Ang::LatticeConstants;
  for (QVector<float>::iterator iter = m_LatticeConstants.begin(); iter != m_LatticeConstants.end(); ++iter )
  {
    stream << " " << *iter;
  }
  stream << QString("\n");

  stream << Ebsd::Ang::NumberFamilies << ": " << m_NumberFamilies << QString("\n");


  for (QVector<HKLFamily::Pointer>::iterator iter = m_HKLFamilies.begin(); iter != m_HKLFamilies.end(); ++iter )
  {
    (*iter)->printSelf(stream);
  }


  stream << Ebsd::Ang::Categories;
  for (QVector<int>::iterator iter = m_Categories.begin(); iter != m_Categories.end(); ++iter )
  {
    stream << " " << *iter;
  }
  stream << QString("\n");

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
      crystal_structure = Ebsd::CrystalStructure::Cubic_High;
      break;
    case Ebsd::Ang::PhaseSymmetry::Tetrahedral:
      crystal_structure = Ebsd::CrystalStructure::Cubic_Low;
      break;
    case Ebsd::Ang::PhaseSymmetry::DiTetragonal:
      crystal_structure = Ebsd::CrystalStructure::Tetragonal_High;
      break;
    case Ebsd::Ang::PhaseSymmetry::Tetragonal:
      crystal_structure = Ebsd::CrystalStructure::Tetragonal_Low;
      break;
    case Ebsd::Ang::PhaseSymmetry::Orthorhombic:
      crystal_structure = Ebsd::CrystalStructure::OrthoRhombic;
      break;
    case Ebsd::Ang::PhaseSymmetry::Monoclinic_c:
    case Ebsd::Ang::PhaseSymmetry::Monoclinic_b:
    case Ebsd::Ang::PhaseSymmetry::Monoclinic_a:
      crystal_structure = Ebsd::CrystalStructure::Monoclinic;
      break;
    case Ebsd::Ang::PhaseSymmetry::Triclinic:
      crystal_structure = Ebsd::CrystalStructure::Triclinic;
      break;
    case Ebsd::Ang::PhaseSymmetry::DiHexagonal:
      crystal_structure = Ebsd::CrystalStructure::Hexagonal_High;
      break;
    case Ebsd::Ang::PhaseSymmetry::Hexagonal:
      crystal_structure = Ebsd::CrystalStructure::Hexagonal_Low;
      break;
    case Ebsd::Ang::PhaseSymmetry::DiTrigonal:
      crystal_structure = Ebsd::CrystalStructure::Trigonal_High;
      break;
    case Ebsd::Ang::PhaseSymmetry::Trigonal:
      crystal_structure = Ebsd::CrystalStructure::Trigonal_Low;
      break;


    default:
      crystal_structure = Ebsd::CrystalStructure::UnknownCrystalStructure;

  }
  return crystal_structure;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AngPhase::setLatticeConstantA(float a)
{
  m_LatticeConstants[0] = a;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AngPhase::setLatticeConstantB(float a)
{
  m_LatticeConstants[1] = a;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AngPhase::setLatticeConstantC(float a)
{
  m_LatticeConstants[2] = a;

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AngPhase::setLatticeConstantAlpha(float a)
{
  m_LatticeConstants[3] = a;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AngPhase::setLatticeConstantBeta(float a)
{
  m_LatticeConstants[4] = a;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AngPhase::setLatticeConstantGamma(float a)
{
  m_LatticeConstants[5] = a;
}

