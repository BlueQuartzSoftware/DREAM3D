///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2010, Michael A. Jackson. BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <string>


#include <MXA/Common/LogTime.h>
#include <MXA/BMPIO/MXABmpIO.h>
#define _TIFF_DATA_TYPEDEFS_ 1
#include <tiffio.h>
#include <tiffvers.h>

int WriteGrayScaleTiff(unsigned char* raster, char* filename,
                       int width, int height,
                       char* documentName, char* imageDescription)
{
  int err;
  TIFF *out;
  std::string dateTime;
  char software[1024];
  tsize_t area;

  out = TIFFOpen(filename, "w");
  if (out == NULL)
  {
    printf("Could not open output file '%s' for writing.\n", filename);
    return -1;
  }

  err = 0;
  // set the basic values
  err = TIFFSetField(out, TIFFTAG_IMAGEWIDTH, (int)width);
  err = TIFFSetField(out, TIFFTAG_IMAGELENGTH, (int)height);
  err = TIFFSetField(out, TIFFTAG_BITSPERSAMPLE, 8);
  err = TIFFSetField(out, TIFFTAG_SAMPLESPERPIXEL, 1);
  err = TIFFSetField(out, TIFFTAG_ROWSPERSTRIP, (int)height); // 1 strip


  dateTime = tifDateTime();
  err = TIFFSetField(out, TIFFTAG_DATETIME, dateTime.c_str());
  // String based tags
  if (NULL != documentName)
  {
    err = TIFFSetField(out, TIFFTAG_DOCUMENTNAME, documentName);
  }
  if (NULL != imageDescription)
  {
    err = TIFFSetField(out, TIFFTAG_IMAGEDESCRIPTION, imageDescription);
  }

  err = TIFFSetField(out, TIFFTAG_ORIENTATION, 1);
  err = TIFFSetField(out, TIFFTAG_PHOTOMETRIC, 1);

#if USE_LZW_COMPRESSION
  err = TIFFSetField(image, TIFFTAG_COMPRESSION, COMPRESSION_LZW);
  err = TIFFSetField(image, TIFFTAG_PREDICTOR, PREDICTOR_HORIZONTAL);
#else
  err = TIFFSetField(out, TIFFTAG_COMPRESSION, COMPRESSION_NONE);
#endif

  // Insert Resolution Units here if possible


  memset(software, 0, 1024);
  snprintf(software, 1024, "IPHelper BMP2Tiff: %s", TIFFLIB_VERSION_STR);

  err = TIFFSetField(out, TIFFTAG_SOFTWARE, software);

  err = TIFFSetField(out, TIFFTAG_HOSTCOMPUTER, MXADATAMODEL_SYSTEM);

  // Write the information to the file
  area = (tsize_t)(width * height);
  err = TIFFWriteEncodedStrip(out, 0, raster, area);
  if (err != area)
  {
    err = -1;
  }

  (void)TIFFClose(out);
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int main(int argc, char **argv)
{
  if (argc != 3)
  {
    std::cout << "This program takes 2 arguments. The input bmp file and the output tiff file." << std::endl;
    return EXIT_FAILURE;
  }
  MXABmpIO bmp;
  LOAD_TEXTUREBMP_RESULT err = bmp.loadBMPData(argv[1], false);
  bmp.convertToGrayscale();
  uint8_t* image = bmp.getImageData();
  int32_t width = bmp.getWidth();
  int32_t height = bmp.getHeight();

  int writeErr = WriteGrayScaleTiff(image, argv[2], width, height, argv[1], "Converted From Original BMP file");
  if (writeErr < 0)
  {
    std::cout << "Error Converting BMP to Tiff. See other output for possible details." << std::endl;
  }

  return EXIT_SUCCESS;
}
