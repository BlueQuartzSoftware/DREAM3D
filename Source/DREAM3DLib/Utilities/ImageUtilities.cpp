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
#include "ImageUtilities.h"

#include "DREAM3DLib/Utilities/ColorTable.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ImageUtilities::ImageUtilities(){}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ImageUtilities::~ImageUtilities(){}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
UInt8ArrayType::Pointer ImageUtilities::CreateColorImage(DoubleArrayType* data, int width, int height, int nColors, const QString &name, double min, double max)
{
  UInt8ArrayType::Pointer image = UInt8ArrayType::CreateArray(width * height, 4, name);
  PoleFigureConfiguration_t config;
  config.imageDim = width;
  config.numColors = nColors;
  config.eulers = NULL;
  config.minScale = min;
  config.maxScale = max;
  config.sphereRadius = 1.0;
  ImageUtilities::CreateColorImage(data, config, image.get());
  return image;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImageUtilities::CreateColorImage(DoubleArrayType *data, PoleFigureConfiguration_t &config, UInt8ArrayType* image)
{
  int width = config.imageDim ;
  int height = config.imageDim ;

  int halfWidth = width / 2;
  int halfHeight = height / 2;

  float xres = 2.0 / (float)(width);
  float yres = 2.0 / (float)(height);
  float xtmp, ytmp;

  float max = static_cast<float>(config.maxScale);
  float min = static_cast<float>(config.minScale);

  // Initialize the image with all zeros
  image->initializeWithZeros();
  uint32_t* rgbaPtr = reinterpret_cast<uint32_t*>(image->getPointer(0));

  int numColors = config.numColors;
  QVector<float> colors(numColors*3, 0.0);
  ColorTable::GetColorTable(config.numColors, colors);

  float r=0.0, g=0.0, b=0.0;

  double* dataPtr = data->getPointer(0);
  size_t idx = 0;
  double value;
  int bin;
  for (int64_t y = 0; y < height; y++)
  {
    for (int64_t x = 0; x < width; x++)
    {
      xtmp = float(x-halfWidth)*xres+(xres * 0.5);
      ytmp = float(y-halfHeight)*yres+(yres * 0.5);
      idx = (width * y) + x;
      if( ( xtmp * xtmp + ytmp * ytmp) <= 1.0) // Inside the circle
      {
        value = dataPtr[y * width + x];
        value = (value-min)/(max-min);
        bin = int(value*numColors);
        if(bin > numColors-1) bin = numColors-1;
        r = colors[3*bin];
        g = colors[3*bin+1];
        b = colors[3*bin+2];
        rgbaPtr[idx] = RgbColor::dRgb(r*255, g*255, b*255, 255);
      }
      else // Outside the Circle - Set pixel to White
      {
        rgbaPtr[idx] = 0xFFFFFFFF; // White
      }
    }
  }

}
