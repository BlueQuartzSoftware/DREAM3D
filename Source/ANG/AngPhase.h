/* ============================================================================
 * Copyright (c) 2011, Michael A. Jackson (BlueQuartz Software)
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
 * Neither the name of Michael A. Jackson nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
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
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#ifndef ANGPHASE_H_
#define ANGPHASE_H_

#include <string>
#include <vector>

#include "ANG/AngSetGetMacros.h"
#include "ANG/TSLAngLibConfiguration.h"
#include "AngConstants.h"

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
 * @class HKLFamily HKLFamily.h AIM/ANG/HKLFamily.h
 * @brief
 * @author Michael A. Jackson for BlueQuartz Software
 * @date Mar 23, 2011
 * @version 1.0
 */
class TSLANG_EXPORT HKLFamily
{
  public:
    ANG_SHARED_POINTERS(HKLFamily);
    ANG_STATIC_NEW_MACRO(HKLFamily);
    ANG_TYPE_MACRO(HKLFamily)

    virtual ~HKLFamily() {};

    int h;
    int k;
    int l;
    int s1;
    float diffractionIntensity;
    int s2;

    void printSelf(std::ostream &stream)
    {
      stream << TSL::OIM::HKLFamilies;
      std::cout << " " << h << " " << k << " " << l << " " << s1 << " " << diffractionIntensity << " " << s2 << std::endl;
    }

    void copyToStruct(HKLFamily_t* ptr)
    {
      ptr->h = h;
      ptr->k = k;
      ptr->l = l;
      ptr->s1 = s1;
      ptr->diffractionIntensity = diffractionIntensity;
      ptr->s2 = s2;
    }

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
    HKLFamily() {};

  private:
    HKLFamily(const HKLFamily&); // Copy Constructor Not Implemented
    void operator=(const HKLFamily&); // Operator '=' Not Implemented

};



/**
 * @class AngPhase AngPhase.h AIM/ANG/AngPhase.h
 * @brief
 * @author Michael A. Jackson for BlueQuartz Software
 * @date Mar 23, 2011
 * @version 1.0
 */
class TSLANG_EXPORT AngPhase
{
  public:
    ANG_SHARED_POINTERS(AngPhase)
    ANG_STATIC_NEW_MACRO(AngPhase)
    ANG_TYPE_MACRO(AngPhase)

    virtual ~AngPhase();


    ANG_INSTANCE_PROPERTY(int, Phase)
    ANG_INSTANCE_STRING_PROPERTY(MaterialName)
    ANG_INSTANCE_STRING_PROPERTY(Formula)
    ANG_INSTANCE_STRING_PROPERTY(Info)
    ANG_INSTANCE_PROPERTY(int, Symmetry)
    ANG_INSTANCE_PROPERTY(std::vector<float>, LatticeConstants)
    ANG_INSTANCE_PROPERTY(int, NumberFamilies)
    ANG_INSTANCE_PROPERTY(std::vector<HKLFamily::Pointer>, HKLFamilies)
    ANG_INSTANCE_PROPERTY(std::vector<int>, Categories)


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

  protected:
    AngPhase();

  private:
    AngPhase(const AngPhase&); // Copy Constructor Not Implemented
    void operator=(const AngPhase&); // Operator '=' Not Implemented
};

#endif /* ANGPHASE_H_ */
