/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2010, Dr. Michael A. Groeber (US Air Force Research Laboratories)
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

#ifndef _FIELDS_H_
#define _FIELDS_H_

#if defined (_MSC_VER)
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#endif

#include <vector>

#include "MXA/Common/MXASetGetMacros.h"

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/Constants.h"

#define CORRUPT_TEST 0

/**
 * @class Grain Grain.h AIM/Common/Grain.h
 * @brief Support class for the MicroGen3D class
 * @author Michael A. Jackson for BlueQuartz Software, Dr. Michael Groeber for USAFRL
 * @date Nov 4, 2009
 * @version 1.0
 */
class DREAM3DLib_EXPORT Field
{

  public:
    MXA_SHARED_POINTERS(Field);
    MXA_STATIC_NEW_MACRO(Field);
    MXA_TYPE_MACRO(Field);
    virtual ~Field();

    /**
     * @brief Performs a Depp copy of the Field object by making copies of the contained
     * data and not just copying the pointers to the data
     * @param grain Another Field object to copy from.
     */
    void deepCopy(Field::Pointer field);

    std::vector<int>* neighborlist;
#if CORRUPT_TEST
    unsigned long long int* test0;
#endif
    std::vector<float>* neighborsurfacealist;
#if CORRUPT_TEST
    unsigned long long int* test1;
#endif
    std::vector<float>* misorientationlist;


    /*
     * IF YOU ADD NEW VARIABLES MAKE SURE TO UPDATE THE "DeepCopy()" METHOD WITH
     * THE NEW VARIABLES OTHERWISE BAD THINGS CAN HAPPEN.
     */
    int nucleus;
    bool active;
    int numvoxels;
    int numneighbors;
    int newfieldname;
    bool gotcontainedmerged;
    bool gottwinmerged;
    bool gotcolonymerged;
    uint8_t surfacefield;
    bool outsideboundbox;
    int twinnewnumber;
    int colonynewnumber;
    int slipsystem;
    int phase;
    float centroidx;
    float centroidy;
    float centroidz;
    float aspectratio1;
    float aspectratio2;
    float Ixx;
    float Iyy;
    float Izz;
    float Ixy;
    float Iyz;
    float Ixz;
    float omega3;
  	float averageimagequality;
    float averagemisorientation;
    float kernelmisorientation;
    float red;
    float green;
    float blue;
    float IPF[3];
    float schmidfactor;
    float euler1;
    float euler2;
    float euler3;
    float axiseuler1;
    float axiseuler2;
    float axiseuler3;
    float volume;
    float equivdiameter;
    float radius1;
    float radius2;
    float radius3;
    float packquality;
    float avg_quat[5];
#if CORRUPT_TEST
    unsigned long long int* test5;
#endif
    int neighbordistfunc[3];
#if CORRUPT_TEST
    unsigned long long int* test6;
#endif

  protected:
    Field();

 //   Field(const Field&);
 //   Field(Field& field);

  private:
    Field(const Field&); // Copy Constructor Not Implemented
    void operator=(const Field&); // Operator '=' Not Implemented
};

#endif /* FIELDS_H_ */
