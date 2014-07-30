/* ============================================================================
 * Copyright (c) 2011 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2011 Dr. Michael A. Groeber (US Air Force Research Laboratories)
 * Copyright (c) 2014 Dr. Joseph C. Tucker (UES, Inc.)
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
 * Neither the name of Joseph C. Tucker, Michael A. Groeber, Michael A. Jackson,
 * UES, Inc., the US Air Force, BlueQuartz Software nor the names of its contributors
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
 *  This code was written under United States Air Force Contract number
 *                   FA8650-07-D-5800 and FA8650-10-D-5226
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#ifndef _OrientationMath_H_
#define _OrientationMath_H_

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Math/QuaternionMath.hpp"


// Here the order of multiplication is q1*q2 not q2*q1
#if 0
#define MULT_QUAT(q1, q2, out)\
  out[1] = q2[1] * q1[4] + q2[4] * q1[1] + q2[3] * q1[2] - q2[2] * q1[3];\
  out[2] = q2[2] * q1[4] + q2[4] * q1[2] + q2[1] * q1[3] - q2[3] * q1[1];\
  out[3] = q2[3] * q1[4] + q2[4] * q1[3] + q2[2] * q1[1] - q2[1] * q1[2];\
  out[4] = q2[4] * q1[4] - q2[1] * q1[1] - q2[2] * q1[2] - q2[3] * q1[3];\
   
#endif

/*
 * @class OrientationMath OrientationMath.h DREAM3DLib/Common/OrientationMath.h
 * @brief This class performs Crystallographic Misorientation Calculations
 * @author Michael A. Jackson (BlueQuartz Software)
 * @author Michael A. Groeber (US Air Force Research Laboratory)
 * @author Joseph C. Tucker (UES, Inc.)
 * @date Jul 30, 2014
 * @version 5.1
 */
class DREAM3DLib_EXPORT OrientationMath
{
  public:

    virtual ~OrientationMath();

    /**
     * @brief AxisAngletoHomochoric
     * @param w Angle (Radians)
     * @param n1 Axis
     * @param n2 Axis
     * @param n3 Axis
     * @param r1 Output
     * @param r2 Output
     * @param r3 Output
     */
    static void AxisAngletoHomochoric(float w, float n1, float n2, float n3, float& r1, float& r2, float& r3);

    /**
     * @brief AxisAngletoRod
     * @param w Angle (Radians)
     * @param n1 Axis
     * @param n2 Axis
     * @param n3 Axis
     * @param r1 Output
     * @param r2 Output
     * @param r3 Output
     */
    static void AxisAngletoRod(float w, float n1, float n2, float n3, float& r1, float& r2, float& r3);

    /**
     * @brief AxisAngletoQuat
     * @param w Angle (Radians)
     * @param n1 Axis
     * @param n2 Axis
     * @param n3 Axis
     * @param q Output
     */
    static void AxisAngletoQuat(float w, float n1, float n2, float n3, QuatF& q);


    /**
     * @brief AxisAngletoMat
     * @param w Angle (Radians)
     * @param n1 Axis
     * @param n2 Axis
     * @param n3 Axis
     * @param g
     */
    static void AxisAngletoMat(float w, float n1, float n2, float n3, float g[3][3]);

    static void ChangeAxisReferenceFrame(QuatF& q, float& n1, float& n2, float& n3);

    static void HomochorictoRod(float& r1, float& r2, float& r3);

    static void RodtoAxisAngle(float r1, float r2, float r3, float& w, float& n1, float& n2, float& n3);
    static void RodtoQuat(QuatF& q, float r1, float r2, float r3);
    static void RodtoHomochoric(float& r1, float& r2, float& r3);
    static void RodtoEuler(float r1, float r2, float r3, float& ea1, float& ea2, float& ea3);


    static void QuattoAxisAngle(QuatF& q, float& w, float& n1, float& n2, float& n3);
    static void QuattoMat(QuatF& q, float g[3][3]);
    static void QuattoRod(QuatF& q, float& r1, float& r2, float& r3);
    static void QuattoEuler(QuatF& q, float& ea1, float& ea2, float& ea3);

    /**
     * @brief EulertoQuat Passive Quaternion
     * @param ea1
     * @param ea2
     * @param ea3
     * @param q
     */
    static void EulertoQuat(float ea1, float ea2, float ea3, QuatF& q);
    static QuatF EulertoQuat(float ea1, float ea2, float ea3);
    static void EulertoQuat(const float* euler, QuatF& q);
    static QuatF EulertoQuat(const float* ea);

    /**
     * @brief EulertoMat This function converts an Euler Angle triplet (Bunge) into a <b>PASSIVE</b> Orientation Matrix. This
     * is taking a Sample Coordinate system and transforming it to the Crystal Coordinate System (S->C)
     * @param ea1
     * @param ea2
     * @param ea3
     * @param g Output 3x3 Matrix where the values are ordered Row X Column
     */
    static void EulertoMat(float ea1, float ea2, float ea3, float g[3][3]);
    static void EulertoRod(float ea1, float ea2, float ea3, float& r1, float& r2, float& r3);

    static void EulerToAxisAngle(float ea1, float ea2, float ea3, float& w, float& n1, float& n2, float& n3);

    /**
     * @brief EulertoMatActive This function converts an Euler Angle triplet (Bunge) into a <b>ACTIVE</b> Orientation Matrix. This
     * is taking a Crystal Coordinate system and transforming it to the Sample Coordinate System (C->S). Note that to convert
     * from an ACTIVE to PASSIVE or PASSIVE to ACTIVE is just the transpose of the Orienation Maxtrix
     * @param ea1
     * @param ea2
     * @param ea3
     * @param g
     */
    static void EulertoMatActive(float ea1, float ea2, float ea3, float g[3][3]);


    static void MattoEuler(float g[3][3], float& ea1, float& ea2, float& ea3);

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

    /**
    * @brief Converts 4 parameter Miller-Bravais lattice coordinates direction to 3 parameter Miller lattice 
	* coordinates direction
    * @param millerBravais Input Vector (UVTW)
    * @param miller Output Vector (UVW)
    */
	static void MillerBravaisToMillerDirection(float millerBravais[4], float miller[3]);

    /**
    * @brief Converts 3 parameter Miller lattice coordinates direction to 4 parameter Miller-Bravais lattice 
	* coordinates direction
    * @param miller Input Vector (UVW)
    * @param millerBravais Output Vector (UVTW)
    */
	static void MillerToMillerBravaisDirection(float miller[3], float millerBravais[4]);
	
    /**
    * @brief Converts 4 parameter Miller-Bravais lattice coordinates plane to 3 parameter Miller lattice 
	* coordinates plane
    * @param millerBravais Input Vector (UVTW)
    * @param miller Output Vector (UVW)
    */
	static void MillerBravaisToMillerPlane(float millerBravais[4], float miller[3]);
	
    /**
    * @brief Converts 3 parameter Miller lattice coordinates plane to 4 parameter Miller-Bravais lattice 
	* coordinates plane
    * @param miller Input Vector (UVW)
    * @param millerBravais Output Vector (UVTW)
    */
	static void MillerToMillerBravaisPlane(float miller[3], float millerBravais[4]);
    
  protected:
    OrientationMath();


  private:
    OrientationMath(const OrientationMath&); // Copy Constructor Not Implemented
    void operator=(const OrientationMath&); // Operator '=' Not Implemented
};

#endif /* _OrientationMath_H_ */

