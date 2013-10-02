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
#ifndef _ImageUtilites_H_
#define _ImageUtilites_H_

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/DataArrays/DataArray.hpp"
#include "DREAM3DLib/Utilities/PoleFigureUtilities.h"

/**
 * @class ImageUtilities ImageUtilities.h /Utilities/ImageUtilities.h
 * @brief This class has functions that help create and manipulate images.
 * @author Mike Jackson for BlueQuartz Software
 * @date Aug 13 2013
 * @version 1.0
 */
class DREAM3DLib_EXPORT ImageUtilities
{
public:
  ImageUtilities();
  virtual ~ImageUtilities();

  static UInt8ArrayType::Pointer CreateColorImage(DoubleArrayType *data, int width, int height, int nColors, const std::string &name, double min, double max);

  static void CreateColorImage(DoubleArrayType *data, PoleFigureConfiguration_t &config, UInt8ArrayType* image);

private:
  ImageUtilities(const ImageUtilities&); // Copy Constructor Not Implemented
  void operator=(const ImageUtilities&); // Operator '=' Not Implemented

  };



  /**
   * @brief The GenerateRgbaImageImpl class is a wrapper around generating the RGBA image (2D UChar Array with 4 Components) from the
   * intensity image. This should be called from a TBB Task
   */
  class GenerateRgbaImageImpl
  {
    public:
      GenerateRgbaImageImpl(DoubleArrayType* intensity, PoleFigureConfiguration_t* config, UInt8ArrayType* rgba) :
       m_Intensity(intensity),
       m_Config(config),
       m_Rgba(rgba)
      {}
      virtual ~GenerateRgbaImageImpl(){}

      void operator()() const
      {
         ImageUtilities::CreateColorImage(m_Intensity, *m_Config, m_Rgba);
      }
    protected:
      GenerateRgbaImageImpl(){}

    private:
      DoubleArrayType*    m_Intensity;
      PoleFigureConfiguration_t* m_Config;
      UInt8ArrayType* m_Rgba;
  };


#endif /* _ImageUtilites_H_ */
