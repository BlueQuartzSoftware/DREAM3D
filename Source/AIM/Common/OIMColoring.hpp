///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2010, Michael A. Jackson. rgb[2]Quartz Software
//  Copyright (c) 2010, Michael Groeber. US Air Force
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef OIMCOLORING_HPP_
#define OIMCOLORING_HPP_

#include <string>

#include "AIM/Common/AIMMath.h"

#include "AIM/Common/OrientationMath.h"

#define MXA_PI          3.141592653589793
#define MXA_PI_OVER_4   0.785398163397448

/**
* @class OIMColoring OIMColoring.hpp Server/IPFFilter/OIMColoring.hpp
* @brief This class helps generate an RGB color based on a Euler Angle and a
* Reference Direction.
* @author Michael A. Jackson for BlueQuartz Software
* @author Michael Groeber for the US Air Force
* @date Feb 24, 2010
* @version 1.0
*/
class OIMColoring
{

  public:
   static std::string  EulerAngles() { return std::string("Euler_Angles"); }

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
     * Pole Figure coloring for a Cubic Crystal Structure
     * @param eulers The euler angles which MUST be encode into the array in the following order:
     * phi1, Phi, phi2
     * @param refDir The Reference direction. Usually either the ND (001), RD(100), or TD(010)
     * @param rgb A pointer to store the RGB value into
     */
    template<typename T, typename K>
    static void GenerateIPFColor(T* eulers, K* refDir, unsigned char* rgb, unsigned char* hkl)
    {
      OIMColoring::GenerateIPFColor<T>(eulers[0], eulers[1], eulers[2],
                                       refDir[0], refDir[1], refDir[2],rgb, hkl);
    }

    /**
     * @brief Generates an RGB color based on the Inverse Pole Figure coloring
     * for a Cubic Crystal Structure
     * @param phi1 The phi1 euler Angle
     * @param phi The Phi euler Angle
     * @param phi2 The phi2 euler Angle
     * @param refDir0 The first component of the Reference direction vector
     * @param refDir1 The Second component of the Reference direction vector
     * @param refDir2 The third component of the Reference direction vector
     * @param rgb Output - A pointer to store the RGB value into a unsigned char[3] array.
     */
    template <typename T, typename K>
    static void GenerateIPFColor(T phi1, T phi, T phi2,
                                 K refDir0, K refDir1, K refDir2,
                                 unsigned char* rgb, unsigned char* hkl)
    {

      T q1[3][3]; // Rotation Matrix?
      T cd[3];
      T d[3];

      // Calcuate all the values once
      T cos_phi1 = cosf(phi1);
      T sin_phi1 = sinf(phi1);
      T cos_phi = cosf(phi);
      T sin_phi = sinf(phi);
      T cos_phi2 = cosf(phi2);
      T sin_phi2 = sinf(phi2);

      // 1) find rotation matrix from Euler angles
      q1[0][0] = cos_phi1 * cos_phi2 - sin_phi1 * sin_phi2 * cos_phi;
      q1[0][1] = sin_phi1 * cos_phi2 + cos_phi1 * sin_phi2 * cos_phi;
      q1[0][2] = sin_phi2 * sin_phi;
      q1[1][0] = -cos_phi1 * sin_phi2 - sin_phi1 * cos_phi2 * cos_phi;
      q1[1][1] = -sin_phi1 * sin_phi2 + cos_phi1 * cos_phi2 * cos_phi;
      q1[1][2] = cos_phi2 * sin_phi;
      q1[2][0] = sin_phi1 * sin_phi;
      q1[2][1] = -cos_phi1 * sin_phi;
      q1[2][2] = cos_phi;

      // 2) use rotation matrix to find which crystal direction is aligned with 001
      cd[0] = q1[0][0] * refDir0 + q1[0][1] * refDir1 + q1[0][2] * refDir2;
      cd[1] = q1[1][0] * refDir0 + q1[1][1] * refDir1 + q1[1][2] * refDir2;
      cd[2] = q1[2][0] * refDir0 + q1[2][1] * refDir1 + q1[2][2] * refDir2;

	  //3) move that direction to a single standard triangle - using the 001-011-111 triangle)
      cd[0] = fabs(cd[0]);
      cd[1] = fabs(cd[1]);
      cd[2] = fabs(cd[2]);

      // Sort the cd array from smallest to largest
      OIMColoring::TripletSort<T>(cd[0], cd[1], cd[2], cd);

	  T h = cd[0];
	  T k = cd[1];
	  T l = cd[2];
	  hkl[0] = static_cast<unsigned char>(h*100);
	  hkl[1] = static_cast<unsigned char>(k*100);
	  hkl[2] = static_cast<unsigned char>(l*100);
      T theta = (cd[0]*0)+(cd[1]*-sqrt(2.0)/2.0)+(cd[2]*sqrt(2.0)/2.0);
      theta = (180.0/MXA_PI)*acos(theta);
      T red = (90.0-theta)/45.0;
      d[0] = (cd[1]*1)-(cd[2]*0);
      d[1] = (cd[2]*0)-(cd[0]*1);
      d[2] = (cd[0]*0)-(cd[1]*0);
      d[0] = -(d[1]+d[2])/d[0];
      d[1] = 1;
      d[2] = 1;
      T norm = powf(((d[0]*d[0])+(d[1]*d[1])+(d[2]*d[2])),0.5);
      d[0] = d[0]/norm;
      d[1] = d[1]/norm;
      d[2] = d[2]/norm;
      T phi_local = (d[0]*0)+(d[1]*sqrt(2.0)/2.0)+(d[2]*sqrt(2.0)/2.0);
      phi_local = (180.0/MXA_PI)*acos(phi_local);
      T green = (1-red)*((35.26-phi_local)/35.26);
      T blue = (1-red)-green;
      T max = red;
      if(green > max) max = green;
      if(blue > max) max = blue;

      // Scale values from 0 to 1.0
      red = red/max;
      green = green/max;
      blue = blue/max;

      // Add in some correction factors
      red = (0.85*red)+0.15;
      green = (0.85*green)+0.15;
      blue = (0.85*blue)+0.15;

      // Multiply by 255 to get an R/G/B value
      red = red * 255.0f;
      green = green * 255.0f;
      blue = blue * 255.0f;

      rgb[0] = static_cast<unsigned char>(red);
      rgb[1] = static_cast<unsigned char>(green);
      rgb[2] = static_cast<unsigned char>(blue);
    }

/**
 * @brief Calculates the IPF Color in RGB for a Hexagonal Crystal Structure.
 * @param q1 Quaternion to calculate the RGB value for
 * @param rgb Output - A pointer to store the RGB value into a unsigned char[3] array.
 */
    template <typename K>
    void static CalculateHexIPFColor(float q1[5], K refDir0, K refDir1, K refDir2,
                              unsigned char* rgb)
    {
      float qc[5];
      float g[3][3];
      float p[3];
      float d[3];
      float theta, phi;
      float _rgb[3] =
      { 0.0, 0.0, 0.0 };
      for (int j = 0; j < 12; j++)
      {
        OrientationMath::multiplyQuaternions(q1, q1, qc);
        g[0][0] = (1 - (2 * qc[2] * qc[2]) - (2 * qc[3] * qc[3]));
        g[1][0] = ((2 * qc[1] * qc[2]) - (2 * qc[3] * qc[4]));
        g[2][0] = ((2 * qc[1] * qc[3]) + (2 * qc[2] * qc[4]));
        g[0][1] = ((2 * qc[1] * qc[2]) + (2 * qc[3] * qc[4]));
        g[1][1] = (1 - (2 * qc[1] * qc[1]) - (2 * qc[3] * qc[3]));
        g[2][1] = ((2 * qc[2] * qc[3]) - (2 * qc[1] * qc[4]));
        g[0][2] = ((2 * qc[1] * qc[3]) - (2 * qc[1] * qc[4]));
        g[1][2] = ((2 * qc[2] * qc[3]) + (2 * qc[1] * qc[4]));
        g[2][2] = (1 - (2 * qc[1] * qc[1]) - (2 * qc[2] * qc[2]));

        p[0] = g[0][0] * refDir0 + g[0][1] * refDir1 + g[0][2] * refDir2;
        p[1] = g[1][0] * refDir0 + g[1][1] * refDir1 + g[1][2] * refDir2;
        p[2] = g[2][0] * refDir0 + g[2][1] * refDir1 + g[2][2] * refDir2;
        float denom = p[0] * p[0] + p[1] * p[1] + p[2] * p[2];
        denom = powf(denom, 0.5);
        p[0] = p[0] / denom;
        p[1] = p[1] / denom;
        p[2] = p[2] / denom;
        if (p[2] < 0)
        {
          p[0] = -p[0];
          p[1] = -p[1];
          p[2] = -p[2];
        }
        d[0] = (p[1] * 1) - (p[2] * 0);
        d[1] = (p[2] * 0) - (p[0] * 1);
        d[2] = (p[0] * 0) - (p[1] * 0);
        if (d[0] != 0) d[0] = -d[1] / d[0];

        if (d[0] == 0) d[0] = 0;

        d[1] = 1;
        d[2] = 0;
        float norm = powf(((d[0] * d[0]) + (d[1] * d[1]) + (d[2] * d[2])), 0.5);
        d[0] = d[0] / norm;
        d[1] = d[1] / norm;
        d[2] = d[2] / norm;
        if (atan2(d[1], d[0]) >= 0 && atan2(d[1], d[0]) < (30.0 * MXA_PI / 180.0))
        {
          theta = (p[0] * 0) + (p[1] * 0) + (p[2] * 1);
          if (theta > 1) theta = 1;

          if (theta < -1) theta = -1;

          theta = (180.0 / MXA_PI) * acos(theta);
          _rgb[0] = (90.0 - theta) / 90.0;
          phi = (d[0] * 1) + (d[1] * 0) + (d[2] * 0);
          if (phi > 1) phi = 1;

          if (phi < -1) phi = -1;

          phi = (180.0 / MXA_PI) * acos(phi);
          _rgb[1] = (1 - _rgb[0]) * ((30.0 - phi) / 30.0);
          _rgb[2] = (1 - _rgb[0]) - _rgb[1];
        }
      }

      float max = _rgb[0];
      if (_rgb[1] > max) max = _rgb[1];
      if (_rgb[2] > max) max = _rgb[2];

      _rgb[0] = _rgb[0] / max;
      _rgb[1] = _rgb[1] / max;
      _rgb[2] = _rgb[2] / max;
      _rgb[0] = (0.85 * _rgb[0]) + 0.15;
      _rgb[1] = (0.85 * _rgb[1]) + 0.15;
      _rgb[2] = (0.85 * _rgb[2]) + 0.15;

      // Multiply by 255 to get an R/G/B value
      _rgb[0] = _rgb[0] * 255.0f;
      _rgb[1] = _rgb[1] * 255.0f;
      _rgb[2] = _rgb[2] * 255.0f;

      rgb[0] = static_cast<unsigned char>(_rgb[0]);
      rgb[1] = static_cast<unsigned char>(_rgb[1]);
      rgb[2] = static_cast<unsigned char>(_rgb[2]);
    }


  protected:
    OIMColoring() {};
    ~OIMColoring() {};


  private:
    OIMColoring(const OIMColoring&);    // Copy Constructor Not Implemented
    void operator=(const OIMColoring&);  // Operator '=' Not Implemented

};




#endif /* OIMCOLORING_HPP_ */
