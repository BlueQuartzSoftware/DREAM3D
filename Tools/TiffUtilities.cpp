/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
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
 * Neither the name of Michael A. Jackson nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.
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
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */


#include "TiffUtilities.h"

#include <stdio.h>

#include <iostream>

#include "MXA/Common/LogTime.h"
#include "MXA/Common/Win32Defines.h"
#include "MXA/MXAVersion.h"


#define PIXEL8_TO_GREYVALUE(pal, palIndex, out)\
    r = pal[0][palIndex]; g = pal[1][palIndex]; b = pal[2][palIndex];\
    if (r == b && r == g)\
    { bitmapData[temp] = r; }\
    else {\
      R = r * 0.299f; G = g * 0.587f; B = b * 0.114f;\
      out = (unsigned char)(R + G + B);\
    }

#define PIXEL24_TO_GREYVALUE(in, out)\
  r = in[0]; g = in[1]; b = in[2];\
  if (r == g && r == b) \
    out = r;\
  else {\
    R = r * 0.299f; G = g * 0.587f; B = b * 0.144f;\
    out = (unsigned char)(R + G + B);\
  }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TiffUtilities::TiffUtilities()
{

}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TiffUtilities::~TiffUtilities()
{

}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int TiffUtilities::writeOutputImage(TiffImage* data, const char* outpuFileName)
{
  int err = 1;

  if (data->imageData == NULL)
  {
    printf("Error: inputs->output_image was NULL. Can not write output image\n");
    return -1;
  }

  err = writeGrayScaleTiff(data, "Segmented with EM/MPM", outpuFileName);
 // err = writePalettedImage(data, "", outpuFileName);
  if (err < 0)
  {
    printf("Error writing Tiff file %s\n", outpuFileName);
    return -1;
  }

  return err;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int TiffUtilities::readInputImage(TiffImage* data, const char* inputFileName)
{
  int err = 0;

  TIFF* in = (TIFF*)(NULL);
  in = TIFFOpen(inputFileName, "r");
  if (in == NULL)
  {
   printf("Error Opening Tiff file with Absolute Path:\n %s\n", inputFileName);
   return 0;
  }
  int width, height;
  unsigned short samplesperpixel;
  unsigned short bitspersample;
  unsigned short photometric;
  unsigned short orientation;
  unsigned short xRes = 0;
  unsigned short yRes = 0;
  unsigned short resUnits = 1;

  err = TIFFGetField(in, TIFFTAG_IMAGEWIDTH, &width);
  data->width = width;
  err = TIFFGetField(in, TIFFTAG_IMAGELENGTH, &height);
  data->height = height;
  err = TIFFGetField(in, TIFFTAG_SAMPLESPERPIXEL, &samplesperpixel);
  data->samplesPerPixel = samplesperpixel;
  err = TIFFGetField(in, TIFFTAG_BITSPERSAMPLE, &bitspersample);
  data->bitsPerSample = bitspersample;
  err = TIFFGetField(in, TIFFTAG_PHOTOMETRIC, &photometric);
  data->photometric = photometric;
  err = TIFFGetField(in, TIFFTAG_ORIENTATION, &orientation);
  data->orientation = orientation;
  err = TIFFGetField(in, TIFFTAG_XRESOLUTION, &xRes);
  data->xResolution = xRes;
  err = TIFFGetField(in, TIFFTAG_YRESOLUTION, &yRes);
  data->yResolution = yRes;
  err = TIFFGetField(in, TIFFTAG_RESOLUTIONUNIT, &resUnits);
  data->resolutionUnits = resUnits;



  if (bitspersample == 8) {
    err = readTiffAsGrayScale(in, data);
  }
  else if (bitspersample == 16)
  {
    err = read16BitImage(in, data);
  }
  if (data->imageData == NULL)
  {
    return -1;
  }

  if (NULL ==  data->imageData)
  {
    std::cout << "Error Reading tiff image '" << inputFileName << "'" << std::endl;
    return -1;
  }
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int TiffUtilities::read16BitImage(TIFF* in, TiffImage* data)
{
  int err = 0;
  int bytesPerScanLine = TIFFScanlineSize(in);
  // Allocate a buffer to read each scan line into
  tdata_t scanBuffer = (unsigned char*)(malloc(bytesPerScanLine));

  // Allocate the image buffer
  data->imageData = malloc(data->width * data->height * 2);
  unsigned short* dest = static_cast<unsigned short*>(data->imageData);


  //Loop over each row (scanline) and read the data, then copy the data into the
  // image buffer. We do it this way because of alignment issues that crop up in
  // some tiff images. If the scanline size is an odd number tiff will round up
  // to the next multiple of 4 bytes.
  for (int i = 0; i < data->height; ++i)
  {
    err = TIFFReadScanline(in, scanBuffer, i);
    ::memcpy(dest, scanBuffer, bytesPerScanLine);
    dest = dest + data->width;
  }

  // Free the memory that was allocated
  free(scanBuffer);
  return err;

}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int TiffUtilities::readTiffAsGrayScale(TIFF* in, TiffImage* data)
{
  unsigned char* raster; /* retrieve RGBA image */
  int32_t pixel_count;
  unsigned char* src = static_cast<unsigned char*>(NULL);
  unsigned char* dst = static_cast<unsigned char*>(NULL);
  tsize_t totalBytes;

  unsigned char r, g, b;
  float R,G,B;


  // This will eventually be TOO much data but we need it to read the initial data
  totalBytes = data->width * data->height * 4;
  raster = (unsigned char*)_TIFFmalloc( totalBytes );
  if (raster == NULL)
  {
    TIFFError(TIFFFileName(in), "No space for raster buffer");
    return 0;
  }

  // TIFFReadRGBAImageOriented converts non 8bit images including:
  //  Grayscale, bilevel, CMYK, and YCbCR transparently into 32bit RGBA
  //  samples

  /* Read the image in one chunk into an RGBA array */
  if (!TIFFReadRGBAImageOriented(in, (data->width), (data->height), (unsigned int*)(raster), ORIENTATION_TOPLEFT, 0))
  {
    _TIFFfree(raster);
    return 0;
  }

  // Collapse the data down to a single channel, that will end up
  //  being the grayscale values
  pixel_count = (data->height) * (data->width);

  // The collapse is done IN PLACE
  src = raster;
  dst = raster;

  while (pixel_count > 0)
  {
    PIXEL24_TO_GREYVALUE(src, *(dst));
    dst++;
    src += 4; //skip ahead by 4 bytes because we read the raw array into an RGBA array.
    pixel_count--;
  }
  (void) TIFFClose(in); // Close the tiff structures
  data->imageData = raster;
  return 1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int TiffUtilities::writeGrayScaleImage(const char* filename, int rows, int columns,
                        const char* imageDescription, unsigned char* image)
{

  int err;
   TIFF *out;

   char software[1024];
   tsize_t area;

   if (NULL == image)
   {
     return -1;
   }
   out = TIFFOpen(filename, "w");
   if (out == NULL)
   {
     printf("Could not open output file '%s' for writing.\n", filename);
     return -1;
   }

   err = 0;
   // set the basic values
   err = TIFFSetField(out, TIFFTAG_IMAGEWIDTH, (int)columns);
   err = TIFFSetField(out, TIFFTAG_IMAGELENGTH, (int)rows);
   err = TIFFSetField(out, TIFFTAG_BITSPERSAMPLE, 8);
   err = TIFFSetField(out, TIFFTAG_SAMPLESPERPIXEL, 1);
   err = TIFFSetField(out, TIFFTAG_ROWSPERSTRIP, (int)rows); // 1 strip


   QString dateTime = tifDateTime();
   err = TIFFSetField(out, TIFFTAG_DATETIME, dateTime.c_str());


   // String based tags
   if (NULL != filename)
   {
     err = TIFFSetField(out, TIFFTAG_DOCUMENTNAME, filename);
   }
   if (NULL != imageDescription)
   {
     err = TIFFSetField(out, TIFFTAG_IMAGEDESCRIPTION, imageDescription);
   }

   err = TIFFSetField(out, TIFFTAG_ORIENTATION, ORIENTATION_BOTLEFT);
   err = TIFFSetField(out, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);


 #if 0
   err = TIFFSetField(image, TIFFTAG_COMPRESSION, COMPRESSION_LZW);
   err = TIFFSetField(image, TIFFTAG_PREDICTOR, PREDICTOR_HORIZONTAL);
 #else
   err = TIFFSetField(out, TIFFTAG_COMPRESSION, COMPRESSION_NONE);
 #endif

   // Insert Resolution Units here if possible


   ::memset(software, 0, 1024);
   snprintf(software, 1024, "%s using libTif", MXA::Version::Complete().c_str() );


   err = TIFFSetField(out, TIFFTAG_SOFTWARE, software);

 //  err = TIFFSetField(out, TIFFTAG_HOSTCOMPUTER, EMMPM_SYSTEM);

   // Write the information to the file
   area = (tsize_t)( columns *  rows);
   err = TIFFWriteEncodedStrip(out, 0, image, area);
   if (err != area)
   {
     err = -1;
   }
   else
   {
     err = 1;
   }

   (void)TIFFClose(out);
   return err;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int TiffUtilities::writeGrayScaleTiff(TiffImage* data,
                                      const char* imageDescription,
                                      const char* outputFileName)
{
  return  writeGrayScaleImage(outputFileName,
                              data->height, data->width,
                              imageDescription, (unsigned char*)data->imageData);
}

#if 0
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int TiffUtilities::writePalettedImage(TiffImage* data,
                                      char* imageDescription,
                                      char* outputFileName)
{
  int err;
  unsigned char* raster;
 // size_t i;
  TIFF *out;
  char* dateTime;
  char software[1024];
  tsize_t area;
  size_t totalPixels = 0;
  uint16 *r, *g, *b;
  int bitsPerSample = 8;
  uint16 nColors = 1<<bitsPerSample;
  totalPixels = data->width * data->height;


  if (NULL == data->imageData)
  {
    return -1;
  }
  out = TIFFOpen(outputFileName, "w");
  if (out == NULL)
  {
    printf("Could not open output file '%s' for writing.\n", outputFileName);
    return -1;
  }

  err = 0;
  // set the basic values
  err = TIFFSetField(out, TIFFTAG_IMAGEWIDTH, (int)data->width);
  err = TIFFSetField(out, TIFFTAG_IMAGELENGTH, (int)data->height);
  err = TIFFSetField(out, TIFFTAG_BITSPERSAMPLE, bitsPerSample);
  err = TIFFSetField(out, TIFFTAG_SAMPLESPERPIXEL, 1);
  err = TIFFSetField(out, TIFFTAG_ROWSPERSTRIP, (int)data->height); // 1 strip

  dateTime = EIMTOMO_TiffDateTime();
  err = TIFFSetField(out, TIFFTAG_DATETIME, dateTime);
  // String based tags
  if (NULL != outputFileName)
  {
    err = TIFFSetField(out, TIFFTAG_DOCUMENTNAME, outputFileName);
  }
  if (NULL != imageDescription)
  {
    err = TIFFSetField(out, TIFFTAG_IMAGEDESCRIPTION, imageDescription);
  }

  err = TIFFSetField(out, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
  err = TIFFSetField(out, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_PALETTE);
 // err = TIFFSetField(out, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG); // single image plane

  r = (uint16 *) _TIFFmalloc(sizeof(uint16) * nColors);
  g = (uint16 *) _TIFFmalloc(sizeof(uint16) * nColors);
  b = (uint16 *) _TIFFmalloc(sizeof(uint16) * nColors);
  // Set the entire color table to Zeros
  memset(r,0, sizeof(uint16) * nColors);
  memset(g,0, sizeof(uint16) * nColors);
  memset(b,0, sizeof(uint16) * nColors);

  // Copy in the Gray_Table for the segmentation
  for (int32_t i = 0; i < 1; i++) {
    r[i] = UINT16_MAX * ( (float)data->grayTable[i]/(float)UINT8_MAX);
    g[i] = UINT16_MAX * ( (float)data->grayTable[i]/(float)UINT8_MAX);
    b[i] = UINT16_MAX * ( (float)data->grayTable[i]/(float)UINT8_MAX);
  }

  TIFFSetField(out, TIFFTAG_COLORMAP, r, g, b);

#if USE_LZW_COMPRESSION
  err = TIFFSetField(image, TIFFTAG_COMPRESSION, COMPRESSION_LZW);
  err = TIFFSetField(image, TIFFTAG_PREDICTOR, PREDICTOR_HORIZONTAL);
#else
  err = TIFFSetField(out, TIFFTAG_COMPRESSION, COMPRESSION_NONE);
#endif

  // Insert Resolution Units here if possible


  memset(software, 0, 1024);
  snprintf(software, 1024, "%s using libTIFF", TomoEngine_PACKAGE_COMPLETE);

  err = TIFFSetField(out, TIFFTAG_SOFTWARE, software);


  {
    err = TIFFSetField(out, TIFFTAG_XRESOLUTION, data->xResolution);
    err = TIFFSetField(out, TIFFTAG_YRESOLUTION, data->yResolution);
    err = TIFFSetField(out, TIFFTAG_RESOLUTIONUNIT, data->resolutionUnits);
  }

  // Write the information to the file
  area = (tsize_t)( data->width *  data->height);
  err = TIFFWriteEncodedStrip(out, 0, raster, area);
  if (err != area)
  {
    err = -1;
  }
  else
  {
    err = 1;
  }

  (void)TIFFClose(out);

  // Release the color table
  _TIFFfree(r);
  _TIFFfree(g);
  _TIFFfree(b);

  return 1;
}
#endif

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TiffUtilities::freeTiffImageBuffer(unsigned char* buffer)
{
  _TIFFfree(buffer);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
unsigned char* TiffUtilities::allocateTiffImageBuffer(int width, int height, int samplesPerPixel)
{
  return (unsigned char*)_TIFFmalloc( width * height * samplesPerPixel);
}
