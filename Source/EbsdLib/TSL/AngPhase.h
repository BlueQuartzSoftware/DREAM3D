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





#ifndef ANGPHASE_H_
#define ANGPHASE_H_

#include <string>
#include <vector>

#include "EbsdLib/EbsdSetGetMacros.h"
#include "EbsdLib/EbsdLib.h"
#include "EbsdLib/EbsdConstants.h"

#include "AngConstants.h"

/*!
 * @struct HKLFamily_t is used to write the HKL Family to an HDF5 file using a
 * compound data type.
 */
typedef struct
{
    int h;
    int k;
    int l;
    int s1;
    float diffractionIntensity;
    int s2;
} HKLFamily_t;


/**
 * @class HKLFamily HKLFamily.h EbsdLib/TSL/HKLFamily.h
 * @brief Class to hold the information associated with a HKL Family value
 * @author Michael A. Jackson for BlueQuartz Software
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
    int s1;
    float diffractionIntensity;
    int s2;

    /**
     * @brief Prints this class to the output stream. Useful for debuggin
     * @param stream The stream to print to
     */
    void printSelf(std::ostream &stream)
    {
      stream << Ebsd::Ang::HKLFamilies;
      std::cout << " " << h << " " << k << " " << l << " " << s1 << " " << diffractionIntensity << " " << s2 << std::endl;
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
 * @author Michael A. Jackson for BlueQuartz Software
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
    EBSD_INSTANCE_STRING_PROPERTY(MaterialName)
    EBSD_INSTANCE_STRING_PROPERTY(Formula)
    EBSD_INSTANCE_STRING_PROPERTY(Info)
    EBSD_INSTANCE_PROPERTY(uint32_t, Symmetry)
    EBSD_INSTANCE_PROPERTY(std::vector<float>, LatticeConstants)
    EBSD_INSTANCE_PROPERTY(int, NumberFamilies)
    EBSD_INSTANCE_PROPERTY(std::vector<HKLFamily::Pointer>, HKLFamilies)
    EBSD_INSTANCE_PROPERTY(std::vector<int>, Categories)


    void parsePhase(char* value, size_t start, size_t length);
    void parseMaterialName(char* value, size_t start, size_t length);
    void parseFormula(char* value, size_t start, size_t length);
    void parseInfo(char* value, size_t start, size_t length);
    void parseSymmetry(char* value, size_t start, size_t length);
    void parseLatticeConstants(char* value, size_t start, size_t length);
    void parseNumberFamilies(char* value, size_t start, size_t length);
    void parseHKLFamilies(char* value, size_t start, size_t length);
    void parseCategories(char* value, size_t start, size_t length);

    void printSelf(std::ostream &stream);

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

#endif /* ANGPHASE_H_ */
