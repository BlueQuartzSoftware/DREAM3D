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

#include "MXA/Common/MXASetGetMacros.h"
#include "AIM/Common/AIMCommonConfiguration.h"
#include "AIM/ANG/AngConstants.h"


/**
 * @class HKLFamily HKLFamily.h AIM/ANG/HKLFamily.h
 * @brief
 * @author Michael A. Jackson for BlueQuartz Software
 * @date Mar 23, 2011
 * @version 1.0
 */
class AIMCOMMON_EXPORT HKLFamily
{
  public:
    MXA_SHARED_POINTERS(HKLFamily);
    MXA_STATIC_NEW_MACRO(HKLFamily);
    MXA_TYPE_MACRO(HKLFamily)

    virtual ~HKLFamily() {};

    int v1;
    int v2;
    int v3;
    int v4;
    float v5;
    int v6;

    void printSelf(std::ostream &stream)
    {
      stream << TSL::OIM::HKLFamilies;
      std::cout << " " << v1 << " " << v2 << " " << v3 << " " << v4 << " " << v5 << " " << v6 << std::endl;
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
class AIMCOMMON_EXPORT AngPhase
{
  public:
    MXA_SHARED_POINTERS(AngPhase)
    MXA_STATIC_NEW_MACRO(AngPhase)
    MXA_TYPE_MACRO(AngPhase)

    virtual ~AngPhase();


    MXA_INSTANCE_PROPERTY(int, Phase)
    MXA_INSTANCE_STRING_PROPERTY(MaterialName)
    MXA_INSTANCE_STRING_PROPERTY(Formula)
    MXA_INSTANCE_STRING_PROPERTY(Info)
    MXA_INSTANCE_PROPERTY(int, Symmetry)
    MXA_INSTANCE_PROPERTY(std::vector<float>, LatticeConstants)
    MXA_INSTANCE_PROPERTY(int, NumberFamilies)
    MXA_INSTANCE_PROPERTY(std::vector<HKLFamily::Pointer>, HKLFamilies)
    MXA_INSTANCE_PROPERTY(std::vector<int>, Categories)


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
