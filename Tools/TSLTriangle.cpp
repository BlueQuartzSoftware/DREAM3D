///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2010, Michael A. Jackson. BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include <math.h>

#include <iostream>
#include <sstream>
#include <limits>

//-- TIFF Headers
#include <tif_config.h>
#include <tiffio.h>


namespace Detail
{
  static int TRI_WIDTH = 2000;
  static int TRI_HEIGHT = 2000;
  // static float ReferenceDirection[3] = { 0.0f, 0.0f, 1.0f};
}
#define RGB_NUM_COMP 3
#define OR ||
#define LT <
#define GT >
#define IF if
#define THEN {
#define BEGIN
#define endif }

#define MXA_PI          3.141592653589793
#define MXA_PI_OVER_4   0.785398163397448
const float GammaConstant = 5.27183797467291f;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int writeOutputAsTiff(unsigned char* segData,
                          int width,
                          int height,
                      const std::string &outputFile,
                      const std::string &imageDescription)
{


  std::stringstream ss;
  ss.setf(std::ios::fixed);
  ss.fill('0');
  ss.width(4);

  std::string fileName = outputFile;
  TIFF *image;
  image = TIFFOpen(fileName.c_str(), "w");
  if (image == NULL)
  {
    std::cout << "Could not open output tiff for writing" << std::endl;
    return -1;
  }

  int err = 0;
  int imageDims[2] = {width, height};


  // set the basic values
  err = TIFFSetField(image, TIFFTAG_IMAGEWIDTH, (int) imageDims[0]);
  err = TIFFSetField(image, TIFFTAG_IMAGELENGTH, (int) imageDims[1]);
  err = TIFFSetField(image, TIFFTAG_BITSPERSAMPLE, 8);
  err = TIFFSetField(image, TIFFTAG_SAMPLESPERPIXEL, 3);
  err = TIFFSetField(image, TIFFTAG_ROWSPERSTRIP, (int) imageDims[1]); // 1 strip
#if USE_LZW_COMPRESSION
  err = TIFFSetField(image, TIFFTAG_COMPRESSION, COMPRESSION_LZW);
  err = TIFFSetField(image, TIFFTAG_PREDICTOR, PREDICTOR_HORIZONTAL);
#else
  err = TIFFSetField(image, TIFFTAG_COMPRESSION, COMPRESSION_NONE);
#endif
//  std::string datetime = tifDateTime();
//  err = TIFFSetField(image, TIFFTAG_DATETIME, datetime.c_str());
  // String based tags
  if (fileName.length() > 0)
  {
    err = TIFFSetField(image, TIFFTAG_DOCUMENTNAME, fileName.c_str());
  }
  if (imageDescription.length() > 0)
  {

    err = TIFFSetField(image, TIFFTAG_IMAGEDESCRIPTION, imageDescription.c_str());
  }

  err = TIFFSetField(image, TIFFTAG_ORIENTATION, ORIENTATION_BOTLEFT);
  err = TIFFSetField(image, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);
  err = TIFFSetField(image, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);

  // Insert Resolution Units here if possible

  std::string software("LibTiff");

  err = TIFFSetField(image, TIFFTAG_SOFTWARE, software.c_str());

 // err = TIFFSetField(image, TIFFTAG_HOSTCOMPUTER, MXADATAMODEL_SYSTEM);

  // Write the information to the file
  tsize_t area = static_cast<tsize_t>(imageDims[0]* imageDims[1] * RGB_NUM_COMP);
  err = TIFFWriteEncodedStrip(image, 0, segData, area);

  (void) TIFFClose(image);

  return err;

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void tripletSort(float a, float b, float c, float* sorted)
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


  //std::cout << sorted[0] << " " << sorted[1] << " " << sorted[2] << std::endl;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void findColor(float phi1, float phi, float phi2, unsigned char* rgb)
{
  float sd[3];
  float d[3];

  // IDL Code to here.

  //3) move that direction to a single standard triangle - using the 001-011-111 triangle)
  sd[0] = fabs(phi1);
  sd[1] = fabs(phi);
  sd[2] = fabs(phi2);

  // Sort the sd array from smallest to largest
  tripletSort(sd[0], sd[1], sd[2], sd);

  float theta = (sd[0]*0)+(sd[1]*-sqrt(2.0)/2.0)+(sd[2]*sqrt(2.0)/2.0);
  theta = (180.0/MXA_PI)*acos(theta);
  float red = (90.0-theta)/45.0;
  d[0] = (sd[1]*1)-(sd[2]*0);
  d[1] = (sd[2]*0)-(sd[0]*1);
  d[2] = (sd[0]*0)-(sd[1]*0);
  d[0] = -(d[1]+d[2])/d[0];
  d[1] = 1;
  d[2] = 1;
  float norm = powf(((d[0]*d[0])+(d[1]*d[1])+(d[2]*d[2])),0.5);
  d[0] = d[0]/norm;
  d[1] = d[1]/norm;
  d[2] = d[2]/norm;
  float phi_local = (d[0]*0)+(d[1]*sqrt(2.0)/2.0)+(d[2]*sqrt(2.0)/2.0);
  phi_local = (180.0/MXA_PI)*acos(phi_local);
  float green = (1-red)*((35.26-phi_local)/35.26);
  float blue = (1-red)-green;
  float max = red;
  if(green > max) max = green;
  if(blue > max) max = blue;

  // Scale values from 0 to 1.0
  red = red/max;
  green = green/max;
  blue = blue/max;

  // Add in some correction factors
//  red = (0.75*red)+0.25;
//  green = (0.75*green)+0.25;
//  blue = (0.75*blue)+0.25;

  // Multiply by 255 to get an R/G/B value
  red = red * 255.0f;
  green = green * 255.0f;
  blue = blue * 255.0f;

  rgb[0] = static_cast<unsigned int> (red);
  rgb[1] = static_cast<unsigned int> (green);
  rgb[2] = static_cast<unsigned int> (blue);

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void freeMemory(void* ptr)
{
  free(ptr);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
float MAX(float* array, size_t size)
{
  float max = std::numeric_limits<float>::min();
  for (size_t i = 0; i < size; ++i)
  {
    if (array[i] > max) max = array[i];
  }
  return max;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
float* FLTARR(int size)
{
  return static_cast<float*> (malloc(size * sizeof(float)));
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
unsigned char* BYTARR(int numComp, int width, int height)
{
  return static_cast<unsigned char*> (malloc(numComp * width * height * sizeof(unsigned char)));
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
unsigned char* createTriangle(size_t width, size_t height)
{
  unsigned char* arr = BYTARR(RGB_NUM_COMP, width, height);
  float indexConst1 = 0.414 / width;
  float indexConst2 = 0.207 / width;
  float tslConst1 = (90.0f * MXA_PI) / 180.0f;
 // size_t index = 0;
  float temp = 0.0f;
  float red1 = 0.0f;
  float green1 = 0.0f;
  float blue1 = 0.0f;
  float red2 = 0.0f;
  float green2 = 0.0f;
  float blue2 = 0.0f;
  float x = 0.0f;
  float y = 0.0f;
  float z = 0.0f;
  float a = 0.0f;
  float b = 0.0f;
  float c = 0.0f;
  float check1 = 0.0f;
  float check2 = 0.0f;
  float val = 0.0f;
  float x1 = 0.0f;
  float y1 = 0.0f;
  float z1 = 0.0f;
  float denom = 0.0f;
  float phi = 0.0f;
  float x1alt = 0.0f;
  float theta = 0.0f;

  for (size_t yIndex = 0; yIndex < width; ++yIndex)
  {
    for (size_t xIndex = 0; xIndex < height; ++xIndex)
    {

      temp = 0;
      red1 = 0;
      green1 = 0;
      blue1 = 0;
      red2 = 0;
      green2 = 0;
      blue2 = 0;
      x = xIndex * indexConst1 + indexConst2;
      y = yIndex * indexConst1 + indexConst2;
      z = -1.0;
      a = (x * x + y * y + 1);
      b = (2 * x * x + 2 * y * y);
      c = (x * x + y * y - 1);
      check1 = b * b;
      check2 = 4 * a * c;
      val = (-b + sqrtf(b * b - 4 * a * c)) / (2 * a);
      x1 = (1 + val) * x;
      y1 = (1 + val) * y;
      z1 = val;
      denom = (x1 * x1) + (y1 * y1) + (z1 * z1);
      denom = sqrtf(denom);
      x1 = x1 / denom;
      y1 = y1 / denom;
      z1 = z1 / denom;

      red1 = x1 * (-0.707f) + z1 * 0.707f;
      phi = acos(red1);
      x1alt = x1 / 0.707;
      x1alt = x1alt / sqrt((x1alt * x1alt) + (y1 * y1));
      // float check = cos((tslConst1)-phi);
      theta = acos(x1alt / cos((tslConst1) - phi));
      theta = acos(x1alt);
      size_t index = (yIndex * width * RGB_NUM_COMP) + xIndex * RGB_NUM_COMP;

      if (phi LT (45 * 3.1415926535897 / 180.0) OR
          phi GT (90 * 3.1415926535897 / 180.0) OR
          theta GT (35.26 * 3.1415926535897 / 180.0))
      {
        arr[index] = 255;
        arr[index + 1] = 255;
        arr[index + 2] = 255;
      }
      else
      {
        findColor(x1, y1, z1, &(arr[index]));
      }

    }
  }

  return arr;

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int main(int argc, char **argv)
{

  if (argc != 2)
  {
    std::cout << "Nees 1 argument which is an output filename." << std::endl;
  }

  std::string outputFile(argv[1]);
  size_t width = Detail::TRI_WIDTH;
  size_t height = Detail::TRI_HEIGHT;

  unsigned char* image = createTriangle(width, height);
  int err = writeOutputAsTiff(image, width, height, outputFile, std::string("Crystal Orientation Color Legend"));
  freeMemory(image);

  return err;
}
