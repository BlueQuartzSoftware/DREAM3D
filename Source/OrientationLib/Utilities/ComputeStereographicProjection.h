/* ============================================================================
 * Copyright (c) 2017 BlueQuartz Softwae, LLC
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
#ifndef _computeStereographicProjection_H_
#define _computeStereographicProjection_H_

#include "SIMPLib/DataArrays/DataArray.hpp"


#include "OrientationLib/OrientationLib.h"
#include "OrientationLib/Utilities/PoleFigureUtilities.h"

/**
* @class ComputeStereographicProjection This class is a wrapper around simply generating a stereo graphically projected intensity "image" (2D Array) based
* off the intended final size of an image and a modified Lambert projection for a set of XYZ coordinates that represent
* the Coords generated from Euler Angles. This all feeds into generating a pole figure.
*/
class OrientationLib_EXPORT ComputeStereographicProjection
{
  public:
    /**
     * @brief ComputeStereographicProjection
     * @param xyzCoords
     * @param config
     * @param intensity
     */
    ComputeStereographicProjection(FloatArrayType* xyzCoords, PoleFigureConfiguration_t* config, DoubleArrayType* intensity);

    virtual ~ComputeStereographicProjection();

    /**
     * @brief operator ()
     */
    void operator()() const;

  protected:

    /**
     * @brief ComputeStereographicProjection
     */
    ComputeStereographicProjection();

  private:
    FloatArrayType*     m_XYZCoords = nullptr;
    PoleFigureConfiguration_t* m_Config = nullptr;
    DoubleArrayType*    m_Intensity = nullptr;
};


#endif /* _computeStereographicProjection_H_ */
