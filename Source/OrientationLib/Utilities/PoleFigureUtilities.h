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

#ifndef _PoleFigureUtilities_H_
#define _PoleFigureUtilities_H_

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/DataArrays/DataArray.hpp"

#include "OrientationLib/OrientationLib.h"

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
class OrientationLib_EXPORT PoleFigureUtilities
{
  public:
    PoleFigureUtilities();
    virtual ~PoleFigureUtilities();

    /**
     * @brief CreateColorImage
     * @param data
     * @param width
     * @param height
     * @param nColors
     * @param name
     * @param min
     * @param max
     * @return
     */
    static UInt8ArrayType::Pointer CreateColorImage(DoubleArrayType* data, int width, int height, int nColors, const QString& name, double min, double max);

    /**
     * @brief CreateColorImage
     * @param data
     * @param config
     * @param image
     */
    static void CreateColorImage(DoubleArrayType* data, PoleFigureConfiguration_t& config, UInt8ArrayType* image);

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

/**
 * @brief The GeneratePoleFigureRgbaImageImpl class is a wrapper around generating the RGBA image (2D UChar Array with 4 Components) from the
 * intensity image. This should be called from a TBB Task
 */
class GeneratePoleFigureRgbaImageImpl
{
  public:
    GeneratePoleFigureRgbaImageImpl(DoubleArrayType* intensity, PoleFigureConfiguration_t* config, UInt8ArrayType* rgba) :
      m_Intensity(intensity),
      m_Config(config),
      m_Rgba(rgba)
    {}
    virtual ~GeneratePoleFigureRgbaImageImpl() {}

    void operator()() const
    {
      PoleFigureUtilities::CreateColorImage(m_Intensity, *m_Config, m_Rgba);
    }
  protected:
    GeneratePoleFigureRgbaImageImpl() {}

  private:
    DoubleArrayType*    m_Intensity;
    PoleFigureConfiguration_t* m_Config;
    UInt8ArrayType* m_Rgba;
};



#endif /* _PoleFigureUtilities_H_ */

