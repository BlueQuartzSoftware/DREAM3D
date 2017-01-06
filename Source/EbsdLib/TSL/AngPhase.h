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






#ifndef _angphase_h_
#define _angphase_h_

#include <QtCore/QString>
#include <QtCore/QVector>
#include <QtCore/QTextStream>

#include "EbsdLib/EbsdSetGetMacros.h"
#include "EbsdLib/EbsdLib.h"
#include "EbsdLib/EbsdConstants.h"

#include "AngConstants.h"

#pragma pack(push)  /* push current alignment to stack */
#pragma pack(1)     /* set alignment to 1 byte boundary */
/*!
 * @struct HKLFamily_t is used to write the HKL Family to an HDF5 file using a
 * compound data type.
 */
typedef struct
{
  int h;
  int k;
  int l;
  float diffractionIntensity;
  char s1;
  char s2;
} HKLFamily_t;

#pragma pack(pop)

/**
 * @class HKLFamily HKLFamily.h EbsdLib/TSL/HKLFamily.h
 * @brief Class to hold the information associated with a HKL Family value
 *
 * @date Mar 23, 2011
 * @version 1.0
 */
class EbsdLib_EXPORT HKLFamily
{
  public:
    EBSD_SHARED_POINTERS(HKLFamily)
    EBSD_STATIC_NEW_MACRO(HKLFamily)
    EBSD_TYPE_MACRO(HKLFamily)

    virtual ~HKLFamily() {}

    int h;
    int k;
    int l;
    float diffractionIntensity;
    char s1;
    char s2;

    /**
     * @brief Prints this class to the output stream. Useful for debuggin
     * @param stream The stream to print to
     */
    void printSelf(QTextStream& stream)
    {
      stream << Ebsd::Ang::HKLFamilies;
      stream << " " << h << " " << k << " " << l << " " << diffractionIntensity << " " << (int)(s1) << " " << (int)(s2) << "\n";
    }

    /**
     * @brief Copies the content of this instance to another class instance
     * @param ptr The destination of the copied contents
     */
    void copyToStruct(HKLFamily_t* ptr)
    {
      ptr->h = h;
      ptr->k = k;
      ptr->l = l;
      ptr->s1 = s1;
      ptr->diffractionIntensity = diffractionIntensity;
      ptr->s2 = s2;
    }

    /**
     * @brief Copies the content <b>from</b> another structure into this structure
     * @param ptr The source of the copy, ie, the values from <i>ptr</i> will be copied
     * into this instance.
     */
    void copyFromStruct(HKLFamily_t* ptr)
    {
      h = ptr->h;
      k = ptr->k;
      l = ptr->l;
      s1 = ptr->s1;
      diffractionIntensity = ptr->diffractionIntensity;
      s2 = ptr->s2;
    }

  protected:
    HKLFamily() {}

  private:
    HKLFamily(const HKLFamily&); // Copy Constructor Not Implemented
    void operator=(const HKLFamily&); // Operator '=' Not Implemented

};



/**
 * @class AngPhase AngPhase.h EbsdLib/TSL/AngPhase.h
 * @brief This class holds all the values for a "Phase" header block in a TSL file
 *
 * @date Mar 23, 2011
 * @version 1.0
 */
class EbsdLib_EXPORT AngPhase
{
  public:
    EBSD_SHARED_POINTERS(AngPhase)
    EBSD_STATIC_NEW_MACRO(AngPhase)
    EBSD_TYPE_MACRO(AngPhase)

    virtual ~AngPhase();


    EBSD_INSTANCE_PROPERTY(int, PhaseIndex)
    EBSD_INSTANCE_STRING_PROPERTY(PhaseName)
    EBSD_INSTANCE_STRING_PROPERTY(Formula)
    EBSD_INSTANCE_STRING_PROPERTY(Info)
    EBSD_INSTANCE_PROPERTY(uint32_t, Symmetry)
    EBSD_INSTANCE_PROPERTY(QVector<float>, LatticeConstants)
    EBSD_INSTANCE_PROPERTY(int, NumberFamilies)
    EBSD_INSTANCE_PROPERTY(QVector<HKLFamily::Pointer>, HKLFamilies)
    EBSD_INSTANCE_PROPERTY(QVector<int>, Categories)

    void setLatticeConstantA(float a);
    void setLatticeConstantB(float a);
    void setLatticeConstantC(float a);
    void setLatticeConstantAlpha(float a);
    void setLatticeConstantBeta(float a);
    void setLatticeConstantGamma(float a);

    //  void parsePhase(char* value, size_t start, size_t length);
    void parseMaterialName(QList<QByteArray>& tokens);
    void parseFormula(QList<QByteArray>& tokens);
    void parseInfo(QList<QByteArray>& tokens);
    //  void parseSymmetry(char* value, size_t start, size_t length);
    void parseLatticeConstants(QList<QByteArray>& tokens);
    //  void parseNumberFamilies(char* value, size_t start, size_t length);
    void parseHKLFamilies(QList<QByteArray>& tokens);
    void parseCategories(QList<QByteArray>& tokens);

    void printSelf(QTextStream& stream);

    /**
     * @brief Returns the type of crystal structure for this phase.
     */
    unsigned int determineCrystalStructure();



  protected:
    AngPhase();

  private:
    AngPhase(const AngPhase&); // Copy Constructor Not Implemented
    void operator=(const AngPhase&); // Operator '=' Not Implemented
};


struct Ang_Private_Data
{
  QVector<size_t> dims;
  QVector<float> resolution;
  QVector<float> origin;
  QVector<AngPhase::Pointer> phases;
};

enum ANG_READ_FLAG
{
  ANG_FULL_FILE,
  ANG_HEADER_ONLY
};

Q_DECLARE_METATYPE(Ang_Private_Data)

#endif /* ANGPHASE_H_ */

