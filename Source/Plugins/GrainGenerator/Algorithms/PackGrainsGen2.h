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

#ifndef PACKGRAINSGEN2_H_
#define PACKGRAINSGEN2_H_

#include <vector>
#include <string>

#include "MXA/Common/MXASetGetMacros.h"
#include "DREAM3D/Common/Observable.h"
#include "GrainGenerator/GrainGeneratorFunc.h"


class PackGrainsGen2 : public Observable
{
  public:
    MXA_SHARED_POINTERS(PackGrainsGen2);
    MXA_STATIC_NEW_MACRO(PackGrainsGen2);
    MXA_TYPE_MACRO(PackGrainsGen2);

    virtual ~PackGrainsGen2();


    MXA_INSTANCE_PROPERTY(int, ErrorCondition);
    MXA_INSTANCE_STRING_PROPERTY(ErrorMessage);
    void setGrainGenFunc(GrainGeneratorFunc* gg) { m = gg; }
    GrainGeneratorFunc*getGrainGenFunc() { return m; }


    virtual void execute();

    void initialize_packinggrid();
    void generate_grain(int, int);
    void pack_grains();
    void insert_grain(size_t grainNum);

    void add_grain(size_t grainNum);
    void move_grain(size_t grainNum, float xc, float yc, float zc);

    void remove_grain(size_t grainNum);
    void determine_neighbors(size_t grainNum, int add);
    float check_neighborhooderror(int gadd, int gremove);
    float check_sizedisterror(int gadd, int gremove);
    float check_fillingerror(int gadd, int gremove);


    void compare_1Ddistributions(std::vector<float>, std::vector<float>, float &sqrerror);
    void compare_2Ddistributions(std::vector<std::vector<float> >, std::vector<std::vector<float> >, float &sqrerror);

    void compare_3Ddistributions(std::vector<std::vector<std::vector<float> > >, std::vector<std::vector<std::vector<float> > >, float &sqrerror);


  protected:
    PackGrainsGen2();
    GrainGeneratorFunc* m;

  private:


    PackGrainsGen2(const PackGrainsGen2&); // Copy Constructor Not Implemented
    void operator=(const PackGrainsGen2&); // Operator '=' Not Implemented
};

#endif /* PACKGRAINSGEN2_H_ */
