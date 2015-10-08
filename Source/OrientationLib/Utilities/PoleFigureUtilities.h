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

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/DataArrays/DataArray.hpp"

#include "OrientationLib/OrientationLib.h"

/**
 * @struct PoleFigureConfiguration_t
 * @brief This structure controls how Pole Figures are generated. The Order member
 * is the trickiest to understad. Each SpaceGroup has a default order for displaying
 * the 3 pole figures that are created. If you need a different order then the
 * developer needs to create a 3 element QVector and set the order index for the
 * Pole Figure. For example the OrthorhombicOps generates 3 Pole Figures in the order
 * of <001>, <100>, <010>. If the developer would like the Pole Figures to appear
 * in the order of <100>, <010>, <100> then the oder needs to be set as [2, 0, 1].
 *
 * The other interesting item is the Labels. Each SpaceGroupOps subclass uses a default
 * label for each Pole Figure. If the developer would like to over ride those labels
 * then this member can be set with a 3 Element QVector<QString> with the new labels.
 * Note that the new lables will REPLACE the default labels.
 */
typedef struct
{
  FloatArrayType* eulers; ///<* The Euler Angles (in Radians) to use for the pole figure
  int imageDim; ///<* The height/width of the generated pole figure
  int lambertDim; ///<* The dimensions in voxels of the Lambert Square used for interpolation
  int numColors; ///<* The number of colors to use in the Pole figure
  double minScale; ///<* The minimum scale of the Pole Figure
  double maxScale; ///<* The maximum scale of the Pole Figure
  float sphereRadius; ///<* The radius of the Sphere to compute XYZ coords. Should ALWAYS be 1.0
  QVector<QString> labels; ///<* The labels for each of the 3 Pole Figures
  QVector<unsigned int> order; ///<* The order that the pole figures should appear in.
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

