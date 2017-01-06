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





#ifndef _ctfconstants_h_
#define _ctfconstants_h_

#include <QtCore/QString>
#include <QtCore/QVector>

#define DECLARE_STRING_CONST(var)\
  const QString var(#var);


namespace Ebsd
{

  namespace Ctf
  {

    enum LaueGroupTable
    {
      LG_Triclinic = 1,
      LG_Monoclinic = 2,
      LG_Orthorhombic = 3,
      LG_Tetragonal_Low = 4,
      LG_Tetragonal_High = 5,
      LG_Trigonal_Low = 6,
      LG_Trigonal_High = 7,
      LG_Hexagonal_Low = 8,
      LG_Hexagonal_High = 9,
      LG_Cubic_Low = 10,
      LG_Cubic_High = 11,
      UnknownSymmetry = 12
    };

    class LaueGroupStrings
    {
      public:
        LaueGroupStrings()
        {
          m_Values.push_back("Unknown(0)");
          m_Values.push_back("Triclinic -1");
          m_Values.push_back("Monoclinic 2/m");
          m_Values.push_back("Orthorhombic mmm");
          m_Values.push_back("Tetragonal-Low 4/m");
          m_Values.push_back("Tetragonal-High 4/mmm");
          m_Values.push_back("Trigonal-Low -3");
          m_Values.push_back("Trigonal-High -3m");
          m_Values.push_back("Hexagonal-Low 6/m");
          m_Values.push_back("Hexagonal-High 6/mmm");
          m_Values.push_back("Cubic-Low m3");
          m_Values.push_back("Cubic-High m3m");
          m_Values.push_back("Unknown(12)");
        }
        virtual ~LaueGroupStrings() {}

        QString getString(LaueGroupTable i)
        {
          return m_Values[static_cast<size_t>(i)];
        }
      private:

        QVector<QString> m_Values;

    };

    class SpaceGroupQuery
    {
      public:
        SpaceGroupQuery() {}
        virtual ~SpaceGroupQuery() {}

        static bool isCubic(int spaceGroup)
        {
          return (spaceGroup >= 195 && spaceGroup <= 230);
        }
        static bool isHexagonal(int spaceGroup)
        {
          return (spaceGroup >= 168 && spaceGroup <= 194);
        }

    };

    const QString FileExt("ctf");

    const QString Manufacturer("HKL");

// These are Header related
    const QString ChannelTextFile("Channel Text File");
    DECLARE_STRING_CONST(Prj)
    DECLARE_STRING_CONST(Author)
    DECLARE_STRING_CONST(JobMode)
    DECLARE_STRING_CONST(XCells)
    DECLARE_STRING_CONST(YCells)
    DECLARE_STRING_CONST(ZCells)
    DECLARE_STRING_CONST(XStep)
    DECLARE_STRING_CONST(YStep)
    DECLARE_STRING_CONST(ZStep)
    DECLARE_STRING_CONST(AcqE1)
    DECLARE_STRING_CONST(AcqE2)
    DECLARE_STRING_CONST(AcqE3)
    DECLARE_STRING_CONST(Euler)
    DECLARE_STRING_CONST(Mag)
    DECLARE_STRING_CONST(Coverage)
    DECLARE_STRING_CONST(Device)
    DECLARE_STRING_CONST(KV)
    DECLARE_STRING_CONST(TiltAngle)
    DECLARE_STRING_CONST(TiltAxis)
    const QString NumPhases("Phases");

    // These are phase related
    DECLARE_STRING_CONST(LatticeConstants)
    DECLARE_STRING_CONST(PhaseName)
    DECLARE_STRING_CONST(LaueGroup)
    DECLARE_STRING_CONST(SpaceGroup)
    DECLARE_STRING_CONST(Internal1)
    DECLARE_STRING_CONST(Internal2)
    DECLARE_STRING_CONST(Comment)

    /* ******************************************************************************************************** */
    /* ******************************************************************************************************** */
    // These are the names of the Data Columns
    // DO NOT CHANGE ANY OF THESE CONSTANTS, IT WILL MESS UP THE CTF Parser
    DECLARE_STRING_CONST(Phase)
    DECLARE_STRING_CONST(X)
    DECLARE_STRING_CONST(Y)
    DECLARE_STRING_CONST(Z)
    DECLARE_STRING_CONST(Bands)
    DECLARE_STRING_CONST(Error)
    DECLARE_STRING_CONST(Euler1)
    DECLARE_STRING_CONST(Euler2)
    DECLARE_STRING_CONST(Euler3)
    DECLARE_STRING_CONST(MAD)
    DECLARE_STRING_CONST(BC) // BC
    DECLARE_STRING_CONST(BS) // BS
    DECLARE_STRING_CONST(GrainIndex) // FeatureIndex
    DECLARE_STRING_CONST(GrainRandomColourR)
    DECLARE_STRING_CONST(GrainRandomColourG)
    DECLARE_STRING_CONST(GrainRandomColourB)
    /* ******************************************************************************************************** */
    /* ******************************************************************************************************** */
    /* ******************************************************************************************************** */

  } // End namespace Ctf

  namespace CtfFile
  {
    const QString Phases("Phases");
    const QString EulerAngles("EulerAngles");
    const QString CrystalStructures("CrystalStructures");
    const QString PhaseName("PhaseName");
    const QString LatticeConstants("LatticeConstants");
    const QString BravaisLattice("BravaisLattice");
  }
} // End Namespace Ebsd

#endif /* _HKLConstants.h_  */
