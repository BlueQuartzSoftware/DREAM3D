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
void CtfPhase::convertEuropeanDecimals(QByteArray& line)
{
  // Filter the line to convert European command style decimals to US/UK style points
  //  QVector<char> cLine(line.size()+1);
  //  ::memcpy( &(cLine.front()), line.c_str(), line.size() + 1);
  for (int c = 0; c < line.size(); ++c)
  {
    if(line.at(c) == ',')
    {
      line[c] = '.';
    }
  }

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CtfPhase::parsePhase(QByteArray& line)
{
  bool ok = false;
  QList<QByteArray> tokens = line.split('\t'); // the line is segmented by Tabs
  convertEuropeanDecimals(tokens[0]);
  convertEuropeanDecimals(tokens[1]);
  // The lattice constants and lattice angles are ';' delimited
  m_LatticeConstants.resize(6);
  // The first set of values are the lattice constants

  QList<QByteArray> latConst = tokens[0].split(';');
  m_LatticeConstants[0] = latConst[0].toFloat(&ok);
  m_LatticeConstants[1] = latConst[1].toFloat(&ok);
  m_LatticeConstants[2] = latConst[2].toFloat(&ok);
  // The second set of values are the lattice angles
  latConst = tokens[1].split(';');
  m_LatticeConstants[3] = latConst[0].toFloat(&ok);
  m_LatticeConstants[4] = latConst[1].toFloat(&ok);
  m_LatticeConstants[5] = latConst[2].toFloat(&ok);

  // 3rd set is the Name of the phase
  m_PhaseName = tokens[2];

  // 4th set is the Symmetry group
  m_LaueGroup = static_cast<Ebsd::Ctf::LaueGroupTable>(tokens[3].toInt(&ok, 10));

  if(tokens.size() == 5)
  {
    m_Comment = tokens[4];
  }
  else if(tokens.size() == 8)
  {
    m_SpaceGroup = tokens[4].toInt(&ok, 10);
    m_Internal1 = tokens[5];
    m_Internal2 = tokens[6];
    m_Comment = tokens[7];
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CtfPhase::printSelf(std::ostream& stream)
{
  stream << Ebsd::Ctf::LatticeConstants.toStdString() << " " << m_LatticeConstants[0] << ", " << m_LatticeConstants[1] << ", " << m_LatticeConstants[2] << " "
         << m_LatticeConstants[3] << ", " << m_LatticeConstants[4] << ", " << m_LatticeConstants[5] << std::endl;
  stream << Ebsd::Ctf::PhaseName.toStdString() << " " << m_PhaseName.toStdString() << std::endl;
  stream << Ebsd::Ctf::LaueGroup.toStdString() << " " << m_LaueGroup << std::endl;
  stream << Ebsd::Ctf::SpaceGroup.toStdString() << " " << m_SpaceGroup << std::endl;
  stream << Ebsd::Ctf::Internal1.toStdString() << " " << m_Internal1.toStdString() << std::endl;
  stream << Ebsd::Ctf::Internal2.toStdString() << " " << m_Internal2.toStdString() << std::endl;
  stream << Ebsd::Ctf::Comment.toStdString() << " " << m_Comment.toStdString() << std::endl;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
unsigned int CtfPhase::determineCrystalStructure()
{
  Ebsd::Ctf::LaueGroupTable symmetry = getLaueGroup();

  switch(symmetry)
  {
    case Ebsd::Ctf::LG_Triclinic:
      return Ebsd::CrystalStructure::Triclinic;
    case Ebsd::Ctf::LG_Monoclinic:
      return Ebsd::CrystalStructure::Monoclinic;
    case Ebsd::Ctf::LG_Orthorhombic:
      return Ebsd::CrystalStructure::OrthoRhombic;
    case Ebsd::Ctf::LG_Tetragonal_Low:
      return Ebsd::CrystalStructure::Tetragonal_Low;
    case Ebsd::Ctf::LG_Tetragonal_High:
      return Ebsd::CrystalStructure::Tetragonal_High;
    case Ebsd::Ctf::LG_Trigonal_Low:
      return Ebsd::CrystalStructure::Trigonal_Low;
    case Ebsd::Ctf::LG_Trigonal_High:
      return Ebsd::CrystalStructure::Trigonal_High;
    case Ebsd::Ctf::LG_Hexagonal_Low:
      return Ebsd::CrystalStructure::Hexagonal_Low;
    case Ebsd::Ctf::LG_Hexagonal_High:
      return Ebsd::CrystalStructure::Hexagonal_High;
    case Ebsd::Ctf::LG_Cubic_Low:
      return Ebsd::CrystalStructure::Cubic_Low;
    case Ebsd::Ctf::LG_Cubic_High:
      return Ebsd::CrystalStructure::Cubic_High;
    case Ebsd::Ctf::UnknownSymmetry:
      return Ebsd::CrystalStructure::UnknownCrystalStructure;
    default:
      break;

  }
  return Ebsd::CrystalStructure::UnknownCrystalStructure;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString CtfPhase::getMaterialName()
{
  return m_PhaseName;
}
