/* ============================================================================
* Copyright (c) 2009-2015 BlueQuartz Software, LLC
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
* Neither the name of BlueQuartz Software, the US Air Force, nor the names of its
* contributors may be used to endorse or promote products derived from this software
* without specific prior written permission.
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
* The code contained herein was partially funded by the followig contracts:
*    United States Air Force Prime Contract FA8650-07-D-5800
*    United States Air Force Prime Contract FA8650-10-D-5210
*    United States Prime Contract Navy N00173-07-C-2068
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */


#ifndef _OrientationMath_H_
#define _OrientationMath_H_

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Math/QuaternionMath.hpp"

#include "OrientationLib/OrientationLib.h"


/**
 * @class OrientationMath OrientationMath.h DREAM3DLib/Common/OrientationMath.h
 * @brief This class performs Crystallographic Misorientation Calculations
 * @author Michael A. Jackson (BlueQuartz Software)
 * @author Michael A. Groeber (US Air Force Research Laboratory)
 * @author Joseph C. Tucker (UES, Inc.)
 * @date Jul 30, 2014
 * @version 5.1
 */
class OrientationLib_EXPORT OrientationMath
{
  public:

    virtual ~OrientationMath();

#if 0
    /**
     * @brief EulertoMatActive This function converts an Euler Angle triplet (Bunge)
     * into a <b>ACTIVE</b> Orientation Matrix. This is taking a Crystal Coordinate
     * system and transforming it to the Sample Coordinate System (C->S). Note that to convert
     * from an ACTIVE to PASSIVE or PASSIVE to ACTIVE is just the transpose of the Orientation Maxtrix
     * @param ea1
     * @param ea2
     * @param ea3
     * @param g
     */
    static void EulertoMatActive(float ea1, float ea2, float ea3, float g[3][3]);

    /**
     * @brief ChangeAxisReferenceFrame
     * @param q
     * @param n1
     * @param n2
     * @param n3
     */
    static void ChangeAxisReferenceFrame(QuatF& q, float& n1, float& n2, float& n3);

    /**
     * @brief MatrixMisorientation
     * @param g1
     * @param g2
     * @return
     */
    static float MatrixMisorientation(float g1[3][3], float g2[3][3]);

    /**
     * @brief MultiplyQuaternionVector Multiplies a Vector by a quaternion putting the result into 'outVec'
     * @param inQuat Input Quaternion
     * @param inVec Input Vector (XYZ)
     * @param outVec Resulting vector
     */
    static void MultiplyQuaternionVector(QuatF& inQuat, float inVec[3], float outVec[3]);

    /**
    * @brief Does a passive Rotation of the coordinate system defined by w,n1,n2,n3 of the point xyz using Quaternion
    * math to perform the rotation
    * @param angle
    * @param xAxis
    * @param yAxis
    * @param zAxis
    * @param x
    * @param y
    * @param z
    * @return The returned Quaternion is a "pure" Vector quaterion with a Zero value for the scalar (w) portion
    */
    static QuatF PassiveRotation(float angle, float xAxis, float yAxis, float zAxis, float x, float y, float z);

    /**
    * @brief Does an active Rotation of (xyz) in using the Axis-Angle w,n1,n2,n3 using Quaternion math to perform
    * the rotation
    * @param angle
    * @param xAxis
    * @param yAxis
    * @param zAxis
    * @param x
    * @param y
    * @param z
    * @return The returned Quaternion is a "pure" Vector quaterion with a Zero value for the scalar (w) portion
    */
    static QuatF ActiveRotation(float angle, float xAxis, float yAxis, float zAxis, float x, float y, float z);

#endif
    /**
    * @brief Converts lattice parameters to metric tensor
    * @param lattive parameters as a, b, c, alpha, beta, gamma
    * @param Output metric tensor
    */
    static void MetricTensorFromLatticeParameters(float a, float b, float c, float alpha, float beta, float gamma, float mt[3][3]);

    /**
    * @brief Converts lattice parameters to metric tensor
    * @param lattive parameters as a, b, c, alpha, beta, gamma
    * @param Output metric tensor
    */
    static void RootTensorFromLatticeParameters(float a, float b, float c, float alpha, float beta, float gamma, float rt[3][3]);

    /**
    * @brief Converts 4 parameter Miller-Bravais lattice coordinates direction to 3 parameter Miller lattice
    * coordinates direction
    * @param millerBravais Input Vector (UVTW)
    * @param miller Output Vector (UVW)
    */
    static void MillerBravaisToMillerDirection(int millerBravais[4], int miller[3]);

    /**
    * @brief Converts 3 parameter Miller lattice coordinates direction to 4 parameter Miller-Bravais lattice
    * coordinates direction
    * @param miller Input Vector (UVW)
    * @param millerBravais Output Vector (UVTW)
    */
    static void MillerToMillerBravaisDirection(int miller[3], int millerBravais[4]);

    /**
    * @brief Converts 4 parameter Miller-Bravais lattice coordinates plane to 3 parameter Miller lattice
    * coordinates plane
    * @param millerBravais Input Vector (HKIL)
    * @param miller Output Vector (HKL)
    */
    static void MillerBravaisToMillerPlane(int millerBravais[4], int miller[3]);

    /**
    * @brief Converts 3 parameter Miller lattice coordinates plane to 4 parameter Miller-Bravais lattice
    * coordinates plane
    * @param miller Input Vector (HKL)
    * @param millerBravais Output Vector (HKIL)
    */
    static void MillerToMillerBravaisPlane(int miller[3], int millerBravais[4]);

  protected:
    OrientationMath();


  private:
    OrientationMath(const OrientationMath&); // Copy Constructor Not Implemented
    void operator=(const OrientationMath&); // Operator '=' Not Implemented
};

#endif /* _OrientationMath_H_ */

