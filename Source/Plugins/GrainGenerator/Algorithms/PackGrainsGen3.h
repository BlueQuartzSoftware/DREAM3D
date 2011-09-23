/* ============================================================================
 * Copyright (c) 2011 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2011 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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

#ifndef PACKGRAINSGEN3_H_
#define PACKGRAINSGEN3_H_


#include "MXA/Common/MXASetGetMacros.h"
#include "DREAM3D/Common/Observable.h"
#include "GrainGenerator/GrainGeneratorFunc.h"
#include "GrainGenerator/Algorithms/PackGrainsGen2.h"


class PackGrainsGen3 : public PackGrainsGen2
{
  public:
    MXA_SHARED_POINTERS(PackGrainsGen3);
    MXA_STATIC_NEW_MACRO(PackGrainsGen3);
    MXA_TYPE_MACRO(PackGrainsGen3);

    virtual ~PackGrainsGen3();

//
//    MXA_INSTANCE_PROPERTY(int, ErrorCondition);
//    MXA_INSTANCE_STRING_PROPERTY(ErrorMessage);
//    void setGrainGenFunc(GrainGeneratorFunc::Pointer gg) { m = gg; }
//    GrainGeneratorFunc::Pointer getGrainGenFunc() { return m; }


    void execute();


  protected:
    PackGrainsGen3();

  private:
    GrainGeneratorFunc::Pointer m;

    PackGrainsGen3(const PackGrainsGen3&); // Copy Constructor Not Implemented
    void operator=(const PackGrainsGen3&); // Operator '=' Not Implemented
};

#endif /* PACKGRAINSGEN3_H_ */
