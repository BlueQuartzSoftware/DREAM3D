/* ============================================================================
 * Copyright (c) 2015 BlueQuartz Software, LLC
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
 * Neither the names of any of the BlueQuartz Software contributors
 * may be used to endorse or promote products derived from this software without
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
 *
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#ifndef _so3sampler_h_
#define _so3sampler_h_



#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/Math/SIMPLibMath.h"

#include "OrientationLib/OrientationLib.h"
#include "OrientationLib/OrientationMath/OrientationArray.hpp"


/**
 * @brief The SO3Sampler class
 */
class OrientationLib_EXPORT SO3Sampler
{
  public:
    SIMPL_SHARED_POINTERS(SO3Sampler)
    SIMPL_STATIC_NEW_MACRO(SO3Sampler)
    SIMPL_TYPE_MACRO(SO3Sampler)

    virtual ~SO3Sampler();

    /**
     * @brief OrientationListArrayType
     */
    typedef std::list<DOrientArrayType> OrientationListArrayType;

    // sampler routine
    OrientationListArrayType SampleRFZ(int nsteps,int pgnum);

    /**
     * @brief IsinsideFZ
     * @param rod
     * @param FZtype
     * @param FZorder
     * @return
     */
    bool IsinsideFZ(double* rod, int FZtype, int FZorder);

    /**
     * @brief insideCubicFZ
     * @param rod
     * @param symType
     * @return
     */
    bool insideCubicFZ(double* rod, int symType);

    /**
     * @brief insideCyclicFZ
     * @param rod
     * @param order
     * @return
     */
    bool insideCyclicFZ(double* rod, int order);

    /**
     * @brief insideDihedralFZ
     * @param rod
     * @param order
     * @return
     */
    bool insideDihedralFZ(double* rod, int order);

  private:

  protected:
    SO3Sampler();

  private:

    SO3Sampler(const SO3Sampler&); // Copy Constructor Not Implemented
    void operator=(const SO3Sampler&); // Operator '=' Not Implemented
};


#endif /* _SO3Sampler_H_ */

