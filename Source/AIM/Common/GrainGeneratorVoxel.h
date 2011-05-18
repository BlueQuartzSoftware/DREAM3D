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

#ifndef _GRAINGENERATORVOXEL_H_
#define _GRAINGENERATORVOXEL_H_

#if defined (_MSC_VER)
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#endif


#include <vector>

#include "MXA/Common/MXASetGetMacros.h"

#include "AIM/Common/AIMCommonConfiguration.h"
#include "AIM/Common/Constants.h"

/**
* @class GrainGeneratorVoxel GrainGeneratorVoxel.h AIM/Common/GrainGeneratorVoxel.h
* @brief Support class for the MicroGen3D class
* @author Michael A. Jackson for BlueQuartz Software, Dr. Michael Groeber for USAFRL
* @date Nov 4, 2009
* @version 1.0
*/
class AIMCOMMON_EXPORT GrainGeneratorVoxel
{
  public:
    MXA_SHARED_POINTERS(GrainGeneratorVoxel);
    MXA_STATIC_NEW_MACRO(GrainGeneratorVoxel);
    MXA_TYPE_MACRO(GrainGeneratorVoxel);
    GrainGeneratorVoxel();
    virtual ~GrainGeneratorVoxel();

    /**
     * @brief Performs a Depp copy of the Grain object by making copies of the contained
     * data and not just copying the pointers to the data
     * @param grain Another Grain object to copy from.
     */
    void deepCopy(GrainGeneratorVoxel* graingeneratorvoxel);

    int grainname;
   	float ellipfunc;
    int alreadychecked;
    int nearestneighbor[3];
    float nearestneighbordistance[3];
//    float euler1;
//    float euler2;
//    float euler3;
    int phase;
    int neighbor;
    int numowners;
    int surfacevoxel;
    int unassigned;
//    float quat[5];

    IntVectorType grainlist;
    FloatVectorType ellipfunclist;
    IntVectorType neighborlist;

  protected:


  private:
    GrainGeneratorVoxel(const GrainGeneratorVoxel&); // Copy Constructor Not Implemented
    void operator=(const GrainGeneratorVoxel&); // Operator '=' Not Implemented
};

#endif /* GRAINGENERATORVOXEL_H_ */
