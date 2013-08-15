/* ============================================================================
 * Copyright (c) 2012 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2012 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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
 *                           FA8650-10-D-5210
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#ifndef _ColorTable_H_
#define _ColorTable_H_

#include "DREAM3DLib/DREAM3DLib.h"

/**
 * @class ColorTable ColorTable.h /Utilities/ColorTable.h
 * @brief This class has functions that help create color tables.
 * @author Mike Jackson for BlueQuartz Software
 * @date Aug 1 2013
 * @version 1.0
 */
class DREAM3DLib_EXPORT ColorTable
{
  public:
    ColorTable();
    virtual ~ColorTable();


    typedef uint32_t Rgba;

    static int red(Rgba rgb)
    {
      return ((rgb >> 16) & 0xff);
    }

    static int green(Rgba rgb)
    {
      return ((rgb >> 8) & 0xff);
    }

    static int blue(Rgba rgb)
    {
      return (rgb & 0xff);
    }

    static int alpha(Rgba rgb)
    {
      return rgb >> 24;
    }

    static Rgba makeRgba(int r, int g, int b, int a)
    {
      return ((a & 0xff) << 24) | ((r & 0xff) << 16) | ((g & 0xff) << 8) | (b & 0xff);
    }



    /**
    * @brief Assumes you've already generated min and max -- the extrema for the data
    * to which you're applying the color map. Then define the number of colorNodes
    * and make sure there's a row of three float values (representing r, g, and b
    * in a 0.0-1.0 range) for each node. Then call this method for with parameter
    * val some float value between min and max inclusive. The corresponding rgb
    * values will be returned in the reference-to-float parameters r, g, and b.
     * @param val
     * @param r [output] The Red Value (0 <= 1.0)
     * @param g [output] The Green Value (0 <= 1.0)
     * @param b [output] The Blue Value (0 <= 1.0)
     * @param max
     * @param min
    */
    static void GetColorCorrespondingToValue(float val,
                                             float &r, float &g, float &b,
                                             float max, float min);

  private:
    ColorTable(const ColorTable&); // Copy Constructor Not Implemented
    void operator=(const ColorTable&); // Operator '=' Not Implemented

};
#endif /* _ColorTable_H_ */
