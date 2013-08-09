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

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DMath.h"


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


    /************************************************
     * PUT CRYSTAL SYSTEM SPECIFIC COLOR GENERATORS IN THEIR RESPECTIVE *Ops CLASSES. DO NOT PROPAGATE THIS
     * ANY MORE IN THIS CLASS.
     **********************************************/


    /**
     * @brief Generates an RGB color based on the Rodrigues Space coloring
     * for a Cubic Crystal Structure
     * @param rod - Rodrigues Vector (3 component rotation description)
     * @param rgb Output - A pointer to store the RGB value into a unsigned char[3] array.
     */
    static void GenerateCubicRodriguesColor(float r1, float r2, float r3, unsigned char* rgb)
    {

    /************************************************
     * THIS SHOULD BE MOVED INTO THE VARIOUS *Ops CLASSES INSTEAD OF IN HERE.
     **********************************************/
      float range = static_cast<float>(2.0*(sqrt(2.0) - 1.0)+0.000001);
      float max = range/2.0f;
      float red = (r1+max)/range;
      float green = (r2+max)/range;
      float blue = (r3+max)/range;

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
    static void GenerateHexRodriguesColor(float r1, float r2, float r3, unsigned char* rgb)
    {

     /************************************************
     * THIS SHOULD BE MOVED INTO THE VARIOUS *Ops CLASSES INSTEAD OF IN HERE.
     **********************************************/
      float range1 = 2.0f*(tanf(DREAM3D::Constants::k_PiOver4));
      float range2 = 2.0f*(tanf(DREAM3D::Constants::k_PiOver12));
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



  protected:
    EbsdColoring() {}
    ~EbsdColoring() {}


  private:
    EbsdColoring(const EbsdColoring&);    // Copy Constructor Not Implemented
    void operator=(const EbsdColoring&);  // Operator '=' Not Implemented

};




#endif /* _EbsdColoring_HPP_ */
