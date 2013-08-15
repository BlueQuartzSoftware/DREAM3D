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
UInt8ArrayType::Pointer ImageUtilities::CreateColorImage(DoubleArrayType* data, int width, int height, int nColors, const std::string &name, double min, double max)
{

//  size_t npoints = data->GetNumberOfTuples();
//  max = std::numeric_limits<double>::min();
//  min = std::numeric_limits<double>::max();
//  double value = 0.0;
//  for(size_t i = 0; i < npoints; ++i)
//  {
//    value = data->GetValue(i);
//    if (value < 0) {continue;}
//    if (value > max) { max = value;}
//    if (value < min) { min = value;}
//  }

  //std::cout << "Min: " << min << "   Max: " << max << std::endl;

  int xpoints = width;
  int ypoints = height;

  int xpointshalf = xpoints / 2;
  int ypointshalf = ypoints / 2;

  float xres = 2.0 / (float)(xpoints);
  float yres = 2.0 / (float)(ypoints);
  float xtmp, ytmp;

  //if (max < 14) { max = 14; }
  UInt8ArrayType::Pointer image = UInt8ArrayType::CreateArray(ypoints * xpoints, 4, name);
  image->initializeWithZeros();
  uint32_t* rgbaPtr = reinterpret_cast<uint32_t*>(image->GetPointer(0));

  int32_t numColors = nColors + 1;

  std::vector<ColorTable::Rgba> colorTable(numColors);

  float r=0.0, g=0.0, b=0.0;

  for (int i = 0; i < nColors; i++)
  {
    ColorTable::GetColorCorrespondingToValue(i, r, g, b, nColors, 0);
    colorTable[i] = ColorTable::makeRgba(r*255, g*255, b*255, 255);
  }
  // Index 0 is all white which is every pixel outside of the Pole Figure circle
  colorTable[nColors] = ColorTable::makeRgba(255, 255, 255, 255);

  //*********************** NOTE ************************************
  // In the below loop over the Pole Figure Image we are swapping the
  // X & Y coordinates when we place the RGBA value into the image. This
  // is because for some reason the data is rotated 90 degrees. Since
  // the image is square we can easily do this swap and effectively
  // rotate the image 90 degrees.
  size_t idx = 0;
  for (int64_t y = 0; y < ypoints; y++)
  {
    for (int64_t x = 0; x < xpoints; x++)
    {
      xtmp = float(x-xpointshalf)*xres+(xres * 0.5);
      ytmp = float(y-ypointshalf)*yres+(yres * 0.5);
      idx = (xpoints * y) + x;
      if( ( xtmp * xtmp + ytmp * ytmp) <= 1.0) // Inside the circle
      {
        int32_t cindex = int32_t(data->GetValue(y * xpoints + x));
        int32_t colorIndex = (cindex-min) / (max - min) * nColors;
        rgbaPtr[idx] = colorTable[colorIndex];
      }
      else // Outside the Circle - Set pixel to White
      {
        rgbaPtr[idx] = colorTable[nColors];
      }
    }
  }

  return image;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
UInt8ArrayType::Pointer ImageUtilities::CreateColorImage(DoubleArrayType* data, PoleFigureConfiguration_t &config, const std::string &name)
{
  return ImageUtilities::CreateColorImage(data, config.imageDim, config.imageDim, config.numColors, name, config.minScale, config.maxScale);
}


