/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2010, Dr. Michael A. Grober (US Air Force Research Laboratories
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

#ifndef _GRAINS_H_
#define _GRAINS_H_

#if defined (_MSC_VER)
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#endif

#include <vector>

#include "MXA/Common/MXASetGetMacros.h"

#include "AIM/Common/AIMCommonConfiguration.h"
#include "AIM/Common/Constants.h"



/**
 * @class Grain Grain.h AIM/Common/Grain.h
 * @brief Support class for the MicroGen3D class
 * @author Michael A. Jackson for BlueQuartz Software, Dr. Michael Groeber for USAFRL
 * @date Nov 4, 2009
 * @version 1.0
 */
class AIMCOMMON_EXPORT Grain
{

  public:
    MXA_SHARED_POINTERS(Grain);
    MXA_STATIC_NEW_MACRO(Grain);
    MXA_TYPE_MACRO(Grain);
    virtual ~Grain();

    /**
     * @brief Performs a Depp copy of the Grain object by making copies of the contained
     * data and not just copying the pointers to the data
     * @param grain Another Grain object to copy from.
     */
    void deepCopy(Grain::Pointer grain);

    IntVectorType     neighborlist;
    DoubleVectorType  neighborsurfarealist;

    std::vector<int>* voxellist;
    std::vector<double>* ellipfunclist;
    std::vector<double>* misorientationlist;
    std::vector<std::vector<int> > neighbordistfunclist;

    /*
     * IF YOU ADD NEW VARIABLES MAKE SURE TO UPDATE THE "DeepCopy()" METHOD WITH
     * THE NEW VARIABLES OTHERWISE BAD THINGS CAN HAPPEN.
     */
    int nucleus;
    int active;
    int numvoxels;
    int numneighbors;
    int newgrainname;
    int gotcontainedmerged;
    int gottwinmerged;
    int gotcolonymerged;
    int surfacegrain;
    int twinnewnumber;
    int colonynewnumber;
    int slipsystem;
    double packquality;
    int phase;
    double centroidx;
    double centroidy;
    double centroidz;
    double Ixx;
    double Iyy;
    double Izz;
    double Ixy;
    double Iyz;
    double Ixz;
    double omega3;
  	double averageimagequality;
    double averagemisorientation;
    double kernelmisorientation;
    double red;
    double green;
    double blue;
    double IPF[3];
    double schmidfactor;
    double euler1;
    double euler2;
    double euler3;
    double axiseuler1;
    double axiseuler2;
    double axiseuler3;
    double volume;
    double equivdiameter;
    double neigherror;
    double radius1;
    double radius2;
    double radius3;
    double lowanglefraction;
    double avg_quat[5];
    int neighbordistfunc[3];

  protected:
    Grain();

 //   Grain(const Grain&);
 //   Grain(Grain& grain);

  private:
    Grain(const Grain&); // Copy Constructor Not Implemented
    void operator=(const Grain&); // Operator '=' Not Implemented
};

#endif /* GRAINS_H_ */
