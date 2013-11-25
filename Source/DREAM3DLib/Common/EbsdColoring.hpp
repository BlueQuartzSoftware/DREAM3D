/* ============================================================================
 * Copyright (c) 2011, Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2011, Dr. Michael A. Groeber (US Air Force Research Laboratories)
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

#ifndef _EbsdColoring_HPP_
#define _EbsdColoring_HPP_

#include <string>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Math/DREAM3DMath.h"
#include "DREAM3DLib/Math/OrientationMath.h"
#include "DREAM3DLib/Math/MatrixMath.h"
#include "DREAM3DLib/OrientationOps/HexagonalOps.h"
#include "DREAM3DLib/OrientationOps/TrigonalOps.h"
#include "DREAM3DLib/OrientationOps/TetragonalOps.h"

namespace EbsdColor
{
  namespace Detail
  {
    static const float DegToRads = static_cast<float>(M_PI/180.0f);
    static const float HalfSqrt2 = sqrt(2.0f) / 2.0f;
    static const float RadToDegs = static_cast<float>(180.0f/M_PI);
    static const float PiOver4  = static_cast<float>(M_PI_4);
    static const float Pi_f = static_cast<float>(M_PI);
    static const float PiOver12 = static_cast<float>(M_PI/12.0);
  }
}

/**
* @class EbsdColoring EbsdColoring.hpp DREAM3DLib/Common/EbsdColoring.hpp
* @brief This class helps generate an RGB color based on a Euler Angle and a
* Reference Direction.
* @author Michael A. Jackson for BlueQuartz Software
* @author Michael Groeber for the US Air Force
* @date Feb 24, 2010
* @version 1.0
*/
class EbsdColoring
{

  public:
    //static std::string  EulerAngles() { return std::string("Euler_Angles"); }

    /**
   * @brief Sorts the 3 values from low to high
   * @param a
   * @param b
   * @param c
   * @param sorted The array to store the sorted values.
   */
    template<typename T>
    static void TripletSort(T a, T b, T c, T* sorted)
    {
      if ( a > b && a > c)
      {
        sorted[2] = a;
        if (b > c) { sorted[1] = b; sorted[0] = c; }
        else { sorted[1] = c; sorted[0] = b; }
      }
      else if ( b > a && b > c)
      {
        sorted[2] = b;
        if (a > c) { sorted[1] = a; sorted[0] = c; }
        else { sorted[1] = c; sorted[0] = a; }
      }
      else if ( a > b )
      {
        sorted[1] = a; sorted[0] = b; sorted[2] = c;
      }
      else if (a >= c && b >=c)
      {
        sorted[0] = c; sorted[1] = a; sorted[2] = b;
      }
      else
      { sorted[0] = a; sorted[1] = b; sorted[2] = c;}
    }

    /**
   * @brief Returns the maximum value of the three values
   * @param red
   * @param green
   * @param blue
   * @return
   */
    template<typename T>
    static T TripletMax(T red, T green, T blue)
    {
      if ( red > green && red > blue)
      {
        return red;
      }
      else if ( green > red && green > blue)
      {
        return green;
      }
      return blue;
    }


    /**
     * @brief returns the minimum of the three values
     * @param red
     * @param green
     * @param blue
     * @return
     */
    template<typename T>
    static T TripletMin(T red, T green, T blue)
    {
      if ( red < green && red < blue)
      {
        return red;
      }
      else if ( green < red && green < blue)
      {
        return green;
      }
      return blue;
    }



    /**
     * @brief Wrapper for convenience - Generates an RGB color based on the Inverse
     * Pole Figure coloring for a Cubic Crystal Structure.  <b>The Reference Direction MUST be a Normalized vector for this algorithm to work correctly</b>
     * @param eulers The euler angles which MUST be encode into the array in the following order:
     * phi1, Phi, phi2
     * @param refDir The Reference direction. Usually either the ND (001), RD(100), or TD(010). <b></b>
     * @param rgb A pointer to store the RGB value into
     * @param hkl Stores the hkl values
     */
    template<typename T, typename K>
    static void GenerateCubicIPFColor(T* eulers, K* refDir, unsigned char* rgb, unsigned char* hkl)
    {
      EbsdColoring::GenerateCubicIPFColor<T>(eulers[0], eulers[1], eulers[2],
          refDir[0], refDir[1], refDir[2], rgb, hkl);
    }

    /**
     * @brief Generates an RGB color based on the Inverse Pole Figure coloring
     * for a Cubic Crystal Structure. <b>The Reference Direction MUST be a Normalized vector for this algorithm to work correctly</b>
     * @param phi1 The phi1 euler Angle
     * @param phi The Phi euler Angle
     * @param phi2 The phi2 euler Angle
     * @param refDirZ The first component of the Reference direction vector
     * @param refDirY The Second component of the Reference direction vector
     * @param refDirZ The third component of the Reference direction vector
     * @param rgb Output - A pointer to store the RGB value into a unsigned char[3] array.
     * @param hkl Output
     * @param degToRad Convert the euler angles to radians
     */
    template <typename T, typename K>
    static void GenerateCubicIPFColor(T phi1, T phi, T phi2,
                                 K refDir0, K refDir1, K refDir2,
                                 unsigned char* rgb, unsigned char* hkl, bool degToRad = false)
    {

      if (degToRad == true)
      {
        phi1 = phi1 * EbsdColor::Detail::DegToRads;
        phi = phi * EbsdColor::Detail::DegToRads;
        phi2 = phi2 * EbsdColor::Detail::DegToRads;
      }
      float g[3][3]; // Rotation Matrix?
      float cd[3];
      float d[3];

      // 1) find rotation matrix from Euler angles
      OrientationMath::BungeEulerToRotationMatrix(phi1, phi, phi2, g);

      // 2) use rotation matrix to find which crystal direction is aligned with 001
      float refDirection[3];
      refDirection[0] = refDir0;
      refDirection[1] = refDir1;
      refDirection[2] = refDir2;
      MatrixMath::Multiply3x3with3x1(g, refDirection, cd);

      //3) move that direction to a single standard triangle - using the 001-011-111 triangle)
      cd[0] = fabs(cd[0]);
      cd[1] = fabs(cd[1]);
      cd[2] = fabs(cd[2]);

      // Sort the cd array from smallest to largest
      EbsdColoring::TripletSort<T>(cd[0], cd[1], cd[2], cd);

      float h = cd[0];
      float k = cd[1];
      float l = cd[2];
      hkl[0] = static_cast<unsigned char> (h * 100);
      hkl[1] = static_cast<unsigned char> (k * 100);
      hkl[2] = static_cast<unsigned char> (l * 100);
      float theta = (cd[0] * 0) + (cd[1] * -EbsdColor::Detail::HalfSqrt2) + (cd[2] * EbsdColor::Detail::HalfSqrt2);
      theta = (EbsdColor::Detail::RadToDegs) * acos(theta);
      float red = (90.0f - theta) / 45.0f;
      d[0] = (cd[1] * 1) - (cd[2] * 0);
      d[1] = (cd[2] * 0) - (cd[0] * 1);
      d[2] = (cd[0] * 0) - (cd[1] * 0);
      d[0] = -(d[1] + d[2]) / d[0];
      d[1] = 1;
      d[2] = 1;
      float norm = powf(((d[0] * d[0]) + (d[1] * d[1]) + (d[2] * d[2])), 0.5);
      d[0] = d[0] / norm;
      d[1] = d[1] / norm;
      d[2] = d[2] / norm;
      float phi_local = (d[0] * 0) + (d[1] * EbsdColor::Detail::HalfSqrt2) + (d[2] * EbsdColor::Detail::HalfSqrt2);
      phi_local = (EbsdColor::Detail::RadToDegs) * acos(phi_local);
      float green = (1 - red) * ((35.26f - phi_local) / 35.26f);
      float blue = (1 - red) - green;
      float max = red;
      if (green > max) max = green;
      if (blue > max) max = blue;

      // Scale values from 0 to 1.0
      red = red / max;
      green = green / max;
      blue = blue / max;
#if 0
      // Add in some correction factors
      red = (0.85f * red) + 0.15f;
      green = (0.85f * green) + 0.15f;
      blue = (0.85f * blue) + 0.15f;
#endif
      // Multiply by 255 to get an R/G/B value
      red = red * 255.0f;
      green = green * 255.0f;
      blue = blue * 255.0f;


      rgb[0] = static_cast<unsigned char> (red);
      rgb[1] = static_cast<unsigned char> (green);
      rgb[2] = static_cast<unsigned char> (blue);
    }

    /**
     * @brief Generates an RGB color based on the Rodrigues Space coloring
     * for a Cubic Crystal Structure
     * @param rod - Rodrigues Vector (3 component rotation description)
     * @param rgb Output - A pointer to store the RGB value into a unsigned char[3] array.
     */
    static void GenerateCubicRodriguesColor(float r1, float r2, float r3, unsigned char* rgb)
    {
      float range = static_cast<float>(2.0*(sqrt(2.0) - 1.0)+0.000001);
      float max = range/2.0f;
      float red = (r1+max)/range;
      float green = (r2+max)/range;
      float blue = (r3+max)/range;

      // Scale values from 0 to 1.0
      //      red = red / max;
      //      green = green / max;
      //      blue = blue / max;

      // Multiply by 255 to get an R/G/B value
      red = red * 255.0f;
      green = green * 255.0f;
      blue = blue * 255.0f;

      rgb[0] = static_cast<unsigned char> (red);
      rgb[1] = static_cast<unsigned char> (green);
      rgb[2] = static_cast<unsigned char> (blue);
    }


    /**
     * @brief Wrapper for convenience - Generates an RGB color based on the Inverse
     * Pole Figure coloring for a Hex Crystal Structure.  <b>The Reference Direction MUST be a Normalized vector for this algorithm to work correctly</b>
     * @param eulers The euler angles which MUST be encode into the array in the following order:
     * phi1, Phi, phi2
     * @param refDir The Reference direction. Usually either the ND (001), RD(100), or TD(010)
     * @param rgb A pointer to store the RGB value into
     */
    template<typename T, typename K>
    static void GenerateHexIPFColor(T* eulers, K* refDir, unsigned char* rgb)
    {
      EbsdColoring::GenerateHexIPFColor<T>(eulers[0], eulers[1], eulers[2],
          refDir[0], refDir[1], refDir[2],rgb);
    }

    /**
     * @brief Generates an RGB color based on the Inverse Pole Figure coloring
     * for a Hex Crystal Structure.  <b>The Reference Direction MUST be a Normalized vector for this algorithm to work correctly</b>
     * @param phi1 The phi1 euler Angle
     * @param phi The Phi euler Angle
     * @param phi2 The phi2 euler Angle
     * @param refDir0 The first component of the Reference direction vector
     * @param refDir1 The Second component of the Reference direction vector
     * @param refDir2 The third component of the Reference direction vector
     * @param rgb Output - A pointer to store the RGB value into a unsigned char[3] array.
     */
    template <typename T, typename K>
    static void GenerateHexIPFColor(T phi1, T phi, T phi2,
                                     K refDir0, K refDir1, K refDir2,
                                     unsigned char* rgb)
    {
      QuatF qc;
      QuatF q1;
      float g[3][3];
      float p[3];
      float refDirection[3];
      float d[3];
      float theta, phi_local;
      float _rgb[3] = { 0.0, 0.0, 0.0 };

      OrientationMath::EulertoQuat(q1, phi1, phi, phi2);

      for (int j = 0; j < 12; j++)
      {
        //        q2 =  const_HexagonalMath::Detail::HexQuatSym[j];
        QuaternionMathF::Multiply(q1, HexagonalMath::Detail::HexQuatSym[j], qc);

        OrientationMath::QuattoMat(qc, g);

        refDirection[0] = refDir0;
        refDirection[1] = refDir1;
        refDirection[2] = refDir2;
        MatrixMath::Multiply3x3with3x1(g, refDirection, p);
        MatrixMath::Normalize3x1(p);

        if (p[2] < 0)
        {
          p[0] = -p[0];
          p[1] = -p[1];
          p[2] = -p[2];
        }
        d[0] = p[0];
        d[1] = p[1];
        d[2] = 0;
        MatrixMath::Normalize3x1(d);
        if (atan2(d[1], d[0]) >= 0 && atan2(d[1], d[0]) < (30.0 * EbsdColor::Detail::DegToRads))
        {
          theta = (p[0] * 0) + (p[1] * 0) + (p[2] * 1);
          if (theta > 1) theta = 1;

          if (theta < -1) theta = -1;

          theta = (EbsdColor::Detail::RadToDegs) * acos(theta);
          _rgb[0] = (90.0f - theta) / 90.0f;
          phi_local = (d[0] * 1) + (d[1] * 0) + (d[2] * 0);
          if (phi_local > 1) phi_local = 1;

          if (phi_local < -1) phi_local = -1;

          phi_local = (EbsdColor::Detail::RadToDegs) * acos(phi_local);
          _rgb[1] = (1 - _rgb[0]) * ((30.0f - phi_local) / 30.0f);
          _rgb[2] = (1 - _rgb[0]) - _rgb[1];
          break;
        }
      }

      float max = _rgb[0];
      if (_rgb[1] > max) max = _rgb[1];
      if (_rgb[2] > max) max = _rgb[2];

      _rgb[0] = _rgb[0] / max;
      _rgb[1] = _rgb[1] / max;
      _rgb[2] = _rgb[2] / max;
      _rgb[0] = (0.85f * _rgb[0]) + 0.15f;
      _rgb[1] = (0.85f * _rgb[1]) + 0.15f;
      _rgb[2] = (0.85f * _rgb[2]) + 0.15f;

      // Multiply by 255 to get an R/G/B value
      _rgb[0] = _rgb[0] * 255.0f;
      _rgb[1] = _rgb[1] * 255.0f;
      _rgb[2] = _rgb[2] * 255.0f;

      rgb[0] = static_cast<unsigned char>(_rgb[0]);
      rgb[1] = static_cast<unsigned char>(_rgb[1]);
      rgb[2] = static_cast<unsigned char>(_rgb[2]);
    }

    /**
     * @brief Generates an RGB color based on the Rodrigues Space coloring
     * for a Cubic Crystal Structure
     * @param rod - Rodrigues Vector (3 component rotation description)
     * @param rgb Output - A pointer to store the RGB value into a unsigned char[3] array.
     */
    static void GenerateHexRodriguesColor(float r1, float r2, float r3, unsigned char* rgb)
    {
      float range1 = 2.0f*(tanf(EbsdColor::Detail::PiOver4));
      float range2 = 2.0f*(tanf(EbsdColor::Detail::PiOver12));
      float max1 = range1/2.0f;
      float max2 = range2/2.0f;
      float red = (r1+max1)/range1;
      float green = (r2+max1)/range1;
      float blue = (r3+max2)/range2;

      // Scale values from 0 to 1.0
      red = red / max1;
      green = green / max1;
      blue = blue / max2;

      // Multiply by 255 to get an R/G/B value
      red = red * 255.0f;
      green = green * 255.0f;
      blue = blue * 255.0f;

      rgb[0] = static_cast<unsigned char> (red);
      rgb[1] = static_cast<unsigned char> (green);
      rgb[2] = static_cast<unsigned char> (blue);
    }


    /**
     * @brief Wrapper for convenience - Generates an RGB color based on the Inverse
     * Pole Figure coloring for a Trig Crystal Structure.  <b>The Reference Direction MUST be a Normalized vector for this algorithm to work correctly</b>
     * @param eulers The euler angles which MUST be encode into the array in the following order:
     * phi1, Phi, phi2
     * @param refDir The Reference direction. Usually either the ND (001), RD(100), or TD(010)
     * @param rgb A pointer to store the RGB value into
     */
    template<typename T, typename K>
    static void GenerateTrigIPFColor(T* eulers, K* refDir, unsigned char* rgb)
    {
      EbsdColoring::GenerateTrigIPFColor<T>(eulers[0], eulers[1], eulers[2],
          refDir[0], refDir[1], refDir[2],rgb);
    }

    /**
     * @brief Generates an RGB color based on the Inverse Pole Figure coloring
     * for a Trig Crystal Structure.  <b>The Reference Direction MUST be a Normalized vector for this algorithm to work correctly</b>
     * @param phi1 The phi1 euler Angle
     * @param phi The Phi euler Angle
     * @param phi2 The phi2 euler Angle
     * @param refDir0 The first component of the Reference direction vector
     * @param refDir1 The Second component of the Reference direction vector
     * @param refDir2 The third component of the Reference direction vector
     * @param rgb Output - A pointer to store the RGB value into a unsigned char[3] array.
     */
    template <typename T, typename K>
    static void GenerateTrigIPFColor(T phi1, T phi, T phi2,
                                     K refDir0, K refDir1, K refDir2,
                                     unsigned char* rgb)
    {
      QuatF qc;
      QuatF q1;
      float g[3][3];
      float p[3];
      float refDirection[3];
      float d[3];
      float theta, phi_local;
      float _rgb[3] = { 0.0, 0.0, 0.0 };

      OrientationMath::EulertoQuat(q1, phi1, phi, phi2);

      for (int j = 0; j < 6; j++)
      {
        //        q2 =  const_TrigagonalMath::Detail::TrigQuatSym[j];
        QuaternionMathF::Multiply(q1, TrigonalMath::Detail::TrigQuatSym[j], qc);

        OrientationMath::QuattoMat(qc, g);

        refDirection[0] = refDir0;
        refDirection[1] = refDir1;
        refDirection[2] = refDir2;
        MatrixMath::Multiply3x3with3x1(g, refDirection, p);
        MatrixMath::Normalize3x1(p);

        if (p[2] < 0)
        {
          p[0] = -p[0];
          p[1] = -p[1];
          p[2] = -p[2];
        }
        d[0] = p[0];
        d[1] = p[1];
        d[2] = 0;
        MatrixMath::Normalize3x1(d);
        if (atan2(d[1], d[0]) >= (-90.0 * EbsdColor::Detail::DegToRads) && atan2(d[1], d[0]) < (-30.0 * EbsdColor::Detail::DegToRads))
        {
          theta = (p[0] * 0) + (p[1] * 0) + (p[2] * 1);
          if (theta > 1) theta = 1;

          if (theta < -1) theta = -1;

          theta = (EbsdColor::Detail::RadToDegs) * acos(theta);
          _rgb[0] = (90.0f - theta) / 90.0f;
          phi_local = (d[0] * 0) + (d[1] * -1) + (d[2] * 0);
          if (phi_local > 1) phi_local = 1;

          if (phi_local < -1) phi_local = -1;

          phi_local = (EbsdColor::Detail::RadToDegs) * acos(phi_local);
          _rgb[1] = (1 - _rgb[0]) * ((60.0f - phi_local) / 60.0f);
          _rgb[2] = (1 - _rgb[0]) - _rgb[1];
        }
      }

      float max = _rgb[0];
      if (_rgb[1] > max) max = _rgb[1];
      if (_rgb[2] > max) max = _rgb[2];

      _rgb[0] = _rgb[0] / max;
      _rgb[1] = _rgb[1] / max;
      _rgb[2] = _rgb[2] / max;
      _rgb[0] = (0.85f * _rgb[0]) + 0.15f;
      _rgb[1] = (0.85f * _rgb[1]) + 0.15f;
      _rgb[2] = (0.85f * _rgb[2]) + 0.15f;

      // Multiply by 255 to get an R/G/B value
      _rgb[0] = _rgb[0] * 255.0f;
      _rgb[1] = _rgb[1] * 255.0f;
      _rgb[2] = _rgb[2] * 255.0f;

      rgb[0] = static_cast<unsigned char>(_rgb[0]);
      rgb[1] = static_cast<unsigned char>(_rgb[1]);
      rgb[2] = static_cast<unsigned char>(_rgb[2]);
    }

    /**
     * @brief Wrapper for convenience - Generates an RGB color based on the Inverse
     * Pole Figure coloring for a Tetra Crystal Structure.  <b>The Reference Direction MUST be a Normalized vector for this algorithm to work correctly</b>
     * @param eulers The euler angles which MUST be encode into the array in the following order:
     * phi1, Phi, phi2
     * @param refDir The Reference direction. Usually either the ND (001), RD(100), or TD(010)
     * @param rgb A pointer to store the RGB value into
     */
    template<typename T, typename K>
    static void GenerateTetraIPFColor(T* eulers, K* refDir, unsigned char* rgb)
    {
      EbsdColoring::GenerateTetraIPFColor<T>(eulers[0], eulers[1], eulers[2],
          refDir[0], refDir[1], refDir[2],rgb);
    }

    /**
     * @brief Generates an RGB color based on the Inverse Pole Figure coloring
     * for a Tetra Crystal Structure.  <b>The Reference Direction MUST be a Normalized vector for this algorithm to work correctly</b>
     * @param phi1 The phi1 euler Angle
     * @param phi The Phi euler Angle
     * @param phi2 The phi2 euler Angle
     * @param refDir0 The first component of the Reference direction vector
     * @param refDir1 The Second component of the Reference direction vector
     * @param refDir2 The third component of the Reference direction vector
     * @param rgb Output - A pointer to store the RGB value into a unsigned char[3] array.
     */
    template <typename T, typename K>
    static void GenerateTetraIPFColor(T phi1, T phi, T phi2,
                                     K refDir0, K refDir1, K refDir2,
                                     unsigned char* rgb)
    {
      QuatF qc;
      QuatF q1;
      float g[3][3];
      float p[3];
      float refDirection[3];
      float d[3];
      float theta, phi_local;
      float _rgb[3] = { 0.0, 0.0, 0.0 };

      OrientationMath::EulertoQuat(q1, phi1, phi, phi2);

      for (int j = 0; j < 12; j++)
      {
        QuaternionMathF::Multiply(q1, TetragonalMath::Detail::TetraQuatSym[j], qc);

        OrientationMath::QuattoMat(qc, g);

        refDirection[0] = refDir0;
        refDirection[1] = refDir1;
        refDirection[2] = refDir2;
        MatrixMath::Multiply3x3with3x1(g, refDirection, p);
        MatrixMath::Normalize3x1(p);


        if (p[2] < 0)
        {
          p[0] = -p[0];
          p[1] = -p[1];
          p[2] = -p[2];
        }
        d[0] = p[0];
        d[1] = p[1];
        d[2] = 0;
        MatrixMath::Normalize3x1(d);
        if (atan2(d[1], d[0]) >= 0 && atan2(d[1], d[0]) < (45.0 * EbsdColor::Detail::DegToRads))
        {
          theta = (p[0] * 0) + (p[1] * 0) + (p[2] * 1);
          if (theta > 1) theta = 1;

          if (theta < -1) theta = -1;

          theta = (EbsdColor::Detail::RadToDegs) * acos(theta);
          _rgb[0] = (90.0f - theta) / 90.0f;
          phi_local = (d[0] * 1) + (d[1] * 0) + (d[2] * 0);
          if (phi_local > 1) phi_local = 1;

          if (phi_local < -1) phi_local = -1;

          phi_local = (EbsdColor::Detail::RadToDegs) * acos(phi_local);
          _rgb[1] = (1 - _rgb[0]) * ((45.0f - phi_local) / 45.0f);
          _rgb[2] = (1 - _rgb[0]) - _rgb[1];
          break;
        }
      }

      float max = _rgb[0];
      if (_rgb[1] > max) max = _rgb[1];
      if (_rgb[2] > max) max = _rgb[2];

      _rgb[0] = _rgb[0] / max;
      _rgb[1] = _rgb[1] / max;
      _rgb[2] = _rgb[2] / max;
      _rgb[0] = (0.85f * _rgb[0]) + 0.15f;
      _rgb[1] = (0.85f * _rgb[1]) + 0.15f;
      _rgb[2] = (0.85f * _rgb[2]) + 0.15f;

      // Multiply by 255 to get an R/G/B value
      _rgb[0] = _rgb[0] * 255.0f;
      _rgb[1] = _rgb[1] * 255.0f;
      _rgb[2] = _rgb[2] * 255.0f;

      rgb[0] = static_cast<unsigned char>(_rgb[0]);
      rgb[1] = static_cast<unsigned char>(_rgb[1]);
      rgb[2] = static_cast<unsigned char>(_rgb[2]);
    }


  protected:
    EbsdColoring() {}
    ~EbsdColoring() {}


  private:
    EbsdColoring(const EbsdColoring&);    // Copy Constructor Not Implemented
    void operator=(const EbsdColoring&);  // Operator '=' Not Implemented

};




#endif /* _EbsdColoring_HPP_ */
