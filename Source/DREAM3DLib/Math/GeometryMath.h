/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2010, Michael A. Groeber (US Air Force Research Laboratory)
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
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#ifndef _GeometryMath_H_
#define _GeometryMath_H_


#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"


/*
 * @class GeometryMath GeometryMath.h DREAM3DLib/Common/GeometryMath.h
 * @brief This class performs Crystallographic Misorientation Calculations
 * @author Michael A. Jackson (BlueQuartz Software)
 * @author Michael A. Groeber (US Air Force Research Laboratory)
 * @date Feb 19, 2011
 * @version 1.0
 */
class DREAM3DLib_EXPORT GeometryMath
{
  public:
    DREAM3D_SHARED_POINTERS(GeometryMath)
    DREAM3D_TYPE_MACRO(GeometryMath)

    virtual ~GeometryMath();

    /**
     * @brief Calculates the Cosine of the angle between 2 vectors. To get the actual angle the programmer should
     * use the following form: float radians = acos(GeometryMath::CosThetaBetweenVectors(a, b));
     * @param a 1x3 Vector
     * @param b 1x3 Vector
     * @return
     */
    static float CosThetaBetweenVectors(float a[3], float b[3]);

    /**
     * @brief Computes the angle in RADIANS between 2 vectors.
     * @param a
     * @param b
     * @return
     */
    static float AngleBetweenVectors(float a[3], float b[3]);

    /**
     * @brief Determines if a point is inside of a box defined by the lower left and upper right corners
     * @param p 1x3 Vector
     * @param lowerLeft 1x3 Vector
     * @param upperRight 1x3 Vector
     * @return
     */
    static bool PointInBox(float p[3], float lowerLeft[3], float upperRight[3]);

    /**
     * @brief Determines if a segment between two points intersects a box defined by the lower left and upper right corners
     * @param p 1x3 Vector (head of segment)
     * @param q 1x3 Vector (tail of segment)
     * @param lowerLeft 1x3 Vector
     * @param upperRight 1x3 Vector
     * @return
     */
    static bool RayIntersectsBox(float p[3], float q[3], float lowerLeft[3], float upperRight[3]);

    /**
     * @brief Creates a randomly oriented ray from point of given length
     * @param p 1x3 Vector 
     * @param length float
     * @param ray 1x3 Vector
     * @return
     */
    static float GenerateRandomRay(float p[3], float length, float ray[3]);

  protected:
    GeometryMath();

  private:
    GeometryMath(const GeometryMath&); // Copy Constructor Not Implemented
    void operator=(const GeometryMath&); // Operator '=' Not Implemented
};

#endif /* _GeometryMath_H_ */
