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
#ifndef _PoleFigureUtilities_H_
#define _PoleFigureUtilities_H_

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/DataArrays/DataArray.hpp"

typedef struct
{
  FloatArrayType* eulers;
  int imageDim;
  int lambertDim;
  int numColors;
  double minScale;
  double maxScale;
  float sphereRadius;
} PoleFigureConfiguration_t;

/**
 * @class PoleFigureUtilities PoleFigureUtilities.h /Utilities/PoleFigureUtilities.h
 * @brief This class has functions that help create pole figures.
 * @author Mike Jackson for BlueQuartz Software
 * @date Aug 1 2013
 * @version 1.0
 */
class DREAM3DLib_EXPORT PoleFigureUtilities
{
  public:
    PoleFigureUtilities();
    virtual ~PoleFigureUtilities();


    /**
     * @brief GenerateHexPoleFigures
     * @param eulers
     * @param lambertDimension
     * @param poleFigureDim
     * @param intensity0001 [output]
     * @param intensity1010 [output]
     * @param intensity1120 [output]
     */
    static void GenerateHexPoleFigures(FloatArrayType* eulers, int lambertDimension, int poleFigureDim,
                                       DoubleArrayType::Pointer& intensity0001,
                                       DoubleArrayType::Pointer& intensity1010,
                                       DoubleArrayType::Pointer& intensity1120);


    /**
     * @brief GenerateHexPoleFigures
     * @param eulers
     * @param lambertDimension
     * @param poleFigureDim
     * @param intensity100 [output]
     * @param intensity010 [output]
     * @param intensity001 [output]
     */
    static void GenerateOrthoPoleFigures(FloatArrayType* eulers, int lambertDimension, int poleFigureDim,
                                         DoubleArrayType::Pointer& intensity100,
                                         DoubleArrayType::Pointer& intensity010,
                                         DoubleArrayType::Pointer& intensity001);



  private:
    PoleFigureUtilities(const PoleFigureUtilities&); // Copy Constructor Not Implemented
    void operator=(const PoleFigureUtilities&); // Operator '=' Not Implemented


};








#endif /* _PoleFigureUtilities_H_ */

