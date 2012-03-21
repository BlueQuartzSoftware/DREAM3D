///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2010, Michael A. Jackson. BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////
//-- C Includes
#include <stdio.h>
#include <math.h>
#include <memory.h>
#include <string.h>

//-- C++ includes
#include <iostream>

//-- MXA Includes
#include <MXA/Common/LogTime.h>
#include <MXA/Common/MXAEndian.h>
#include <MXA/Common/IO/MXAFileReader64.h>

//-- our own includes
#include "MXABmpIO.h"


#define PIXEL8_TO_GREYVALUE(pal, palIndex, out)\
    r = pal[0][palIndex]; g = pal[1][palIndex]; b = pal[2][palIndex];\
    if (r == b && r == g)\
    { bitmapData[temp] = r; }\
    else {\
      R = r * 0.299; G = g * 0.587; B = b * 0.114;\
      out = static_cast<unsigned char>(R + G + B);\
    }

#define PIXEL24_TO_GREYVALUE(in, out)\
  r = in[0]; g = in[1]; b = in[2];\
  if (r == g && r == b) \
    out = r;\
  else {\
    R = r * 0.299; G = g * 0.587; B = b * 0.144;\
    out = static_cast<unsigned char>(R + G + B);\
  }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MXABmpIO::MXABmpIO() :
  width(0),
  height(0),
  numChannels(0),
  isGrayscale(false),
  _imageFlipped(false),
  _imageConvertedToGrayScale(false),
  _convertToGrayScale(false),
  bytesRead(0)
{
  bitmapDataVec = NULL;
  m_Reader64 = NULL;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MXABmpIO::~MXABmpIO()
{
  if (NULL != bitmapDataVec) { delete[] bitmapDataVec; }
  if (NULL != m_Reader64) { delete m_Reader64; }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
LOAD_TEXTUREBMP_RESULT MXABmpIO::loadBMPData( const char* fName, bool readAsGrayScale )
{
  this->_convertToGrayScale = readAsGrayScale;
  // Open file for buffered read.
  LOAD_TEXTUREBMP_RESULT res = LOAD_TEXTUREBMP_SUCCESS;
  m_Reader64 = new MXAFILEREADER_CLASS_NAME(fName);
  if ( false == m_Reader64->initReader() )
  {
    return LOAD_TEXTUREBMP_COULD_NOT_FIND_OR_READ_FILE;
  }

  bytesRead=0;

// Read File Header
  res=readFileHeader();

// Read Info Header
  res=readInfoHeader();

// Read Palette
  res=readPalette();

  // Read Data
  res = readBitmapData();

  // Only clean up bitmapData if there was an error.
  if (res == LOAD_TEXTUREBMP_SUCCESS)
  {
    if (this->_imageConvertedToGrayScale == true)
    {
      numChannels = 1;
    }
    else {
      numChannels = 3;
    }
  }
  // Reset the Shared Pointer forces the wrapped pointer to be deleted
  //  and the underlying file closed
  delete m_Reader64;
  m_Reader64 = NULL;

  // Flip the y values of the image since the (0,0) is in the lower left of a bitmap file
  if (false == this->_imageFlipped)
  {
    this->flipBitmap();
  }

  // Convert the image to a gray scale image if asked for
  if (true == this->_convertToGrayScale && false == this->_imageConvertedToGrayScale )
  {
    this->convertToGrayscale();
  }
  return res;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
LOAD_TEXTUREBMP_RESULT MXABmpIO::readBitmapData()
{

  // Make sure we have a large enough buffer
  this->bitmapDataVec = new uint8_t[width*height*3];
  // Pad until byteoffset. Most images will need no padding
  // since they already are made to use as little space as
  // possible.
  while(bytesRead < this->fileHeader.dataOffset)
  {
    read8BitValue();
  }

  // The data reading procedure depends on the bit depth.
  switch(this->dibHeader.bitsPerPixel)
  {
  case 1:
    return readBitmapData1Bit();
  case 4:
    return readBitmapData4Bit();
  case 8:
    return readBitmapData8Bit();
  default: // 24 bit.
    return readBitmapData24Bit();
  }
}


// -----------------------------------------------------------------------------
// Load a 24-bit image. Cannot be encoded.
LOAD_TEXTUREBMP_RESULT MXABmpIO::readBitmapData24Bit()
{

  int32_t temp = 0;
  int32_t index = 0;
  uint8_t red, green, blue = 0;
  int32_t componentNumBytes = 3;
  std::streamsize numBytes = width * componentNumBytes;
  int32_t offset = 0;
  // 24-bit bitmaps cannot be encoded. Verify this.
  if (this->dibHeader.compressionMethod != BMP_BI_RGB)
    return LOAD_TEXTUREBMP_ILLEGAL_FILE_FORMAT;


  if (true == this->_convertToGrayScale) {
    if (bitmapDataVec != NULL) { delete bitmapDataVec; bitmapDataVec = NULL; }
    this->bitmapDataVec = new uint8_t[width*height];
    componentNumBytes = 1;
  }
  uint8_t* bitmapData =  (this->bitmapDataVec);
  int32_t widthByComponentNumBytes = componentNumBytes * width;

  uint8_t* buffer = new uint8_t[numBytes];// Create a buffer large enough to hold a row of rgb
  // For each scan line
  int targetRow = 0;
  char* buffPtr = (char*)buffer;
  float fTmp;
  uint8_t r,g,b;
  float R, G, B;
  for (int i = 0; i < height; i++)
  {
    //read a row of bytes
    m_Reader64->rawRead( (char*)buffer, numBytes);
    bytesRead += numBytes;
    offset = 0;  //Reset the offset to start of the buffer
    char* buf = (char*)((buffer));
    buffPtr = (char*)((buffer));

    targetRow = height - i - 1;
    index = targetRow * widthByComponentNumBytes;
    for (int j=0;j<width;j++)
    {

      if (true == this->_convertToGrayScale)
      {
        temp = index + j;
        PIXEL24_TO_GREYVALUE(buffPtr, bitmapData[temp])
      }
      else
      {
        blue = buf[offset++];
        green = buf[offset++];
        red = buf[offset++];
		    temp = index + j * 3;
        bitmapData[temp++] = red;
        bitmapData[temp++] = green;
        bitmapData[temp] = blue;
      }
    }

    for (int k=(width*3)%4; k!=0 && k<4;k++)
    {
        read8BitValue();
    }
  }
  this->_imageFlipped = true;
  if (true == this->_convertToGrayScale) {
    this->_imageConvertedToGrayScale = true;
    this->numChannels = 1;
  }
  delete buffer;
  return LOAD_TEXTUREBMP_SUCCESS;
}


// -----------------------------------------------------------------------------
// Read 8-bit image. Can be uncompressed or RLE-8 compressed.
LOAD_TEXTUREBMP_RESULT MXABmpIO::readBitmapData8Bit()
{
  int32_t temp = 0;
  int32_t index = 0;
  uint8_t palIndex = 0;
  int32_t componentNumBytes = 3;
  std::streamsize numBytes = width;
  int32_t offset = 0;
  if (this->dibHeader.compressionMethod != BMP_BI_RGB &&
    this->dibHeader.compressionMethod != BMP_BI_RLE8) {
      return LOAD_TEXTUREBMP_ILLEGAL_FILE_FORMAT;
   }

  //RoboMet Images fall into this category
  // We are going to flip the image as it gets read from the file
  // We are going to convert to grayscale on the fly if needed
  if (this->dibHeader.compressionMethod == BMP_BI_RGB)
  {
    uint8_t* buffer = new uint8_t[numBytes];// Create a buffer large enough to hold a row of rgb
    char* buffPtr = (char*)((buffer));
    if (true == this->_convertToGrayScale) {
      if (NULL != bitmapDataVec) { delete[] bitmapDataVec; bitmapDataVec = NULL;}
      this->bitmapDataVec = new uint8_t[width*height];
      componentNumBytes = 1;
    }
    uint8_t* bitmapData = (this->bitmapDataVec);
    int32_t widthByComponentNumBytes = componentNumBytes * width;
    uint8_t r,g,b;
    float R, G, B;
    // For each scan line
    int targetRow = 0;
    for (int i = 0;i < height; ++i)
    {
      m_Reader64->rawRead( (char*)((buffer)), numBytes);
      bytesRead += numBytes;
      offset = 0;  //Reset the offset to start of the buffer
      buffPtr = (char*)((buffer));
      targetRow = height - i - 1;
      index = targetRow * widthByComponentNumBytes;
      for (int j=0;j<width;j++)
      {
        palIndex = *buffPtr++;
        if (true == this->_convertToGrayScale)
        {
          temp = index + j;
          PIXEL8_TO_GREYVALUE(palette, palIndex, bitmapData[temp]);
        }
        else
        {
		      temp = index + j * 3;
          bitmapData[temp++] = palette[0][palIndex];
          bitmapData[temp++] = palette[1][palIndex];
          bitmapData[temp] = palette[2][palIndex];
        }
      }

      // go to next alignment of 4 bytes.
      int32_t kEnd = width%4;
      for (int k = 0; k < kEnd; ++k) {
         char value;
         this->m_Reader64->readValue(value);
         bytesRead+=1;
      }
    }

    this->_imageFlipped = true;
    if (true == this->_convertToGrayScale) {
      this->_imageConvertedToGrayScale = true;
      this->numChannels = 1;
    }
    delete[] buffer;
  }

  if (this->dibHeader.compressionMethod == BMP_BI_RLE8)
  {
    // Drawing cursor pos
    int32_t x=0;
    int32_t y=0;

    bytesRead=0;

    // Clear bitmap data since it is legal not to
    // fill it all out.
    uint8_t* bitmapData = (this->bitmapDataVec);
    memset(bitmapData,0,sizeof(unsigned char)*width*height*3);

    while(true)
    {
      unsigned char firstByte = read8BitValue();
      unsigned char secondByte = read8BitValue();

      // Is this an escape code or absolute encoding?
      if (firstByte==0)
      {
        // Is this an escape code
        if (secondByte<=0x02)
        {
          LOAD_TEXTUREBMP_RESULT res;
          if (handleEscapeCode(secondByte,&x,&y,&res))
            return res;
        }
        else
        {
          // Absolute encoding
          index = y*width*3;
          for (int i=0; i<secondByte; i++)
          {
            if (x>=width)
              return LOAD_TEXTUREBMP_ILLEGAL_FILE_FORMAT;
            palIndex = read8BitValue();
            bitmapData[index+x*3] = palette[0][palIndex];
            bitmapData[index+x*3+1] = palette[1][palIndex];
            bitmapData[index+x*3+2] = palette[2][palIndex];
            x++;
          } // end for (int i=0; i<secondByte; i++)

          // Pad to 16-bit word boundery
          if (secondByte%2 == 1)
            read8BitValue();
        }
      }
      else
      {
        // If not, perform data decode for next length of colors
        for (int i=0;i<firstByte;i++)
        {
          if (x>=width)
            return LOAD_TEXTUREBMP_ILLEGAL_FILE_FORMAT;
          int32_t idx = y*width*3+x*3;
          bitmapData[idx] = palette[0][secondByte];
          bitmapData[idx+1] = palette[1][secondByte];
          bitmapData[idx+2] = palette[2][secondByte];
          x++;
        }        // end for (int i=0;i<firstByte;i++)
      }
    }
  }

  return LOAD_TEXTUREBMP_SUCCESS;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
LOAD_TEXTUREBMP_RESULT MXABmpIO::readFileHeader()
{
  // Read "BM" tag in ASCII.
  if (read8BitValue()!=66 || read8BitValue()!=77)
    return LOAD_TEXTUREBMP_ILLEGAL_FILE_FORMAT;

  // Read file size. We do not need this. Ignore.
  this->fileHeader.fileSize = read32BitValue();

  // Skip the two reserved areas
  this->fileHeader.reserved1 = read16BitValue();
  this->fileHeader.reserved2 = read16BitValue();

  // Read the byte offset
  this->fileHeader.dataOffset = read32BitValue();

  return LOAD_TEXTUREBMP_SUCCESS;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
LOAD_TEXTUREBMP_RESULT MXABmpIO::readInfoHeader()
{
  // We expect this to be at least 40. If it is larger we read
  // some more bytes in the end to be forward compatible.
  uint32_t sizeOfInfoHeader = (uint32_t)read32BitValue();

  if (sizeOfInfoHeader<40)
    return LOAD_TEXTUREBMP_ILLEGAL_FILE_FORMAT;

  width = dibHeader.width = read32BitValue();

  height = dibHeader.height = read32BitValue();

  // Read number of planes. According to the specification this
  // must be 1.
  if (read16BitValue()!=1)
    return LOAD_TEXTUREBMP_ILLEGAL_FILE_FORMAT;

  dibHeader.bitsPerPixel = read16BitValue();

  if (this->dibHeader.bitsPerPixel != 1 &&
      this->dibHeader.bitsPerPixel != 4 &&
      this->dibHeader.bitsPerPixel != 8 &&
      this->dibHeader.bitsPerPixel != 24)
    return LOAD_TEXTUREBMP_ILLEGAL_FILE_FORMAT;

  dibHeader.compressionMethod = (CompressionType)read32BitValue();

  if (dibHeader.compressionMethod != BMP_BI_RGB &&
      dibHeader.compressionMethod != BMP_BI_RLE8 &&
      dibHeader.compressionMethod != BMP_BI_RLE4)
    return LOAD_TEXTUREBMP_ILLEGAL_FILE_FORMAT;

  // Read image size. We do not need this since we have the
  // image size.
  dibHeader.imageSize = read32BitValue();

  // Pixel to device mapping. This is irrelevant since we simply
  // want the bitmap.
  dibHeader.horizontalResolution = read32BitValue();
  dibHeader.verticalResolution = read32BitValue();

  // Read colors used. We do not need this, so it is ignored.
  dibHeader.numPaletteColors = read32BitValue();

  // Read the number of important colors. This will not be needed
  // in OpenGL so we will ignore this.
  dibHeader.numImportantColors = read32BitValue();

  // Apply padding in end of header to be forward compatible.
  sizeOfInfoHeader -= 40;
  while (sizeOfInfoHeader>0)
  {
    read8BitValue();
    sizeOfInfoHeader--;
  }

  return LOAD_TEXTUREBMP_SUCCESS;
}

// The palette follows directly after the
// info header
//
//typedef struct tagRGBQUAD {     /* rgbq */
//  BYTE    rgbBlue;
//  BYTE    rgbGreen;
//  BYTE    rgbRed;
//  BYTE    rgbReserved;
// } RGBQUAD;
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
LOAD_TEXTUREBMP_RESULT MXABmpIO::readPalette()
{
  // 24-bit images are not paletted.
  if (this->dibHeader.bitsPerPixel == 24)
    return LOAD_TEXTUREBMP_SUCCESS;

  int32_t numColors = 1<<this->dibHeader.bitsPerPixel;
  for (int i=0;i<numColors;i++)
  {
    // Read RGB.
    for (int j=2;j>=0;j--)
      this->palette[j][i] = read8BitValue();

    // Skip reversed byte.
    read8BitValue();
  }

  return LOAD_TEXTUREBMP_SUCCESS;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
LOAD_TEXTUREBMP_RESULT MXABmpIO::readBitmapData1Bit()
{
  uint8_t* bitmapData = (this->bitmapDataVec);
  // 1-bit format cannot be compressed
  if (this->dibHeader.compressionMethod != BMP_BI_RGB)
    return LOAD_TEXTUREBMP_ILLEGAL_FILE_FORMAT;

  uint8_t byteRead;
  for (int y=0;y<height;y++)
  {
    int32_t index = y*width*3;
    for (int x=0;x<width;x++)
    {
      if (x%8==0)
      {
        byteRead = read8BitValue();
      }

      uint8_t color = (byteRead >> (7-(x%8))) & 1;

      bitmapData[index+x*3] = palette[0][color];
      bitmapData[index+x*3+1] = palette[1][color];
      bitmapData[index+x*3+2] = palette[2][color];
    }

    // Pad to 32-bit boundery.
    while(bytesRead%4 != 0)
      read8BitValue();
  }

  return LOAD_TEXTUREBMP_SUCCESS;
}

// -----------------------------------------------------------------------------
// This is called after the first byte has been found to be 0
// and the second to be 0-2. This is only used in RLE-4 and RLE-8
// encoding.
bool MXABmpIO::handleEscapeCode(int secondByte, int* x, int* y,
                             LOAD_TEXTUREBMP_RESULT* res)
{
  if (secondByte==0x00)
  {
    // End of line
    (*x)=0;
    if ((*y)>=height)
    {
      *res = LOAD_TEXTUREBMP_ILLEGAL_FILE_FORMAT;
      return true;
    }
    (*y)++;
  }
  else if (secondByte==0x01)
  {
    // End of bitmap
    *res = LOAD_TEXTUREBMP_SUCCESS;
    return true;
  }
  else // secondByte=0x02
  {
    // Delta. Move drawing cursor.
    *x += read8BitValue();
    *y += read8BitValue();
    if (*x>=width || *x<0 || *y>=height || *y<0)
    {
      *res = LOAD_TEXTUREBMP_ILLEGAL_FILE_FORMAT;
      return true;
    }
  }

  return false;
}

// -----------------------------------------------------------------------------
// Draw a 4-bit image. Can be uncompressed or RLE-4 compressed.
LOAD_TEXTUREBMP_RESULT MXABmpIO::readBitmapData4Bit()
{
  uint8_t* bitmapData = (this->bitmapDataVec);
  if (this->dibHeader.compressionMethod != BMP_BI_RGB &&
      this->dibHeader.compressionMethod != BMP_BI_RLE4)
    return LOAD_TEXTUREBMP_ILLEGAL_FILE_FORMAT;

  // Uncompressed 4 bit encoding
  if (this->dibHeader.compressionMethod == BMP_BI_RGB)
  {
    for (int j=0;j<height;j++)
    {
      int32_t index = j*width*3;
      unsigned char byteValue = 0;
      unsigned char color = 0;
      for (int i=0;i<width;i++)
      {
        if (i%2==0)
        {
          byteValue = read8BitValue();
          color = byteValue>>4;
        }
        else
        {
          color = byteValue & 0x0F;
        }

        bitmapData[index+i*3] = palette[0][color];
        bitmapData[index+i*3+1] = palette[1][color];
        bitmapData[index+i*3+2] = palette[2][color];
      }

      // Pad to 32-bit boundery.
      for (int k=0; k<(((width+1)/2)%4);k++)
        read8BitValue();
    }
  }

  // RLE encoded 4-bit compression
  if (this->dibHeader.compressionMethod == BMP_BI_RLE4)
  {
    // Drawing cursor pos
    int32_t x=0;
    int32_t y=0;

    // Clear bitmap data since it is legal not to
    // fill it all out.
    memset(bitmapData,0,sizeof(unsigned char)*width*height*3);

    bytesRead=0;

    while(true)
    {
      unsigned char firstByte = read8BitValue();
      unsigned char secondByte = read8BitValue();

      // Is this an escape code or absolute encoding?
      if (firstByte==0)
      {
        // Is this an escape code
        if (secondByte<=0x02)
        {
          LOAD_TEXTUREBMP_RESULT res;
          if (handleEscapeCode(secondByte,&x,&y,&res))
            return res;
        }
        else
        {
          // Absolute encoding
          int32_t index = y*width*3;
          int32_t color;
          unsigned char databyte;
          for (int i=0; i<secondByte; i++)
          {
            if (x>=width)
              return LOAD_TEXTUREBMP_ILLEGAL_FILE_FORMAT;

            if (i%2==0)
            {
              databyte = read8BitValue();
              color = databyte >> 4;
            }
            else
            {
              color = databyte & 0x0F;
            }

            bitmapData[index+x*3] = palette[0][color];
            bitmapData[index+x*3+1] = palette[1][color];
            bitmapData[index+x*3+2] = palette[2][color];
            x++;
          }

          // Pad to 16-bit word boundery
          while (bytesRead%2 != 0)
            read8BitValue();
        }
      }
      else
      {
        // If not absolute or escape code, perform data decode for next
        // length of colors
        int32_t color1 = secondByte >> 4;
        int32_t color2 = secondByte & 0x0F;

        for (int i=0;i<firstByte;i++)
        {
          if (x>=width)
            return LOAD_TEXTUREBMP_ILLEGAL_FILE_FORMAT;

          int32_t color;
          if (i%2==0)
            color = color1;
          else
            color = color2;

          int32_t index = y*width*3+x*3;
          bitmapData[index] = palette[0][color];
          bitmapData[index+1] = palette[1][color];
          bitmapData[index+2] = palette[2][color];
          x++;
        }
      }
    }
  }

  return LOAD_TEXTUREBMP_SUCCESS;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int32_t MXABmpIO::getHeight()
{
  return this->height;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int32_t MXABmpIO::getWidth()
{
  return this->width;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int32_t MXABmpIO::getNumberOfChannels()
{
  return this->numChannels;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MXABmpIO::flipBitmap()
{
  uint8_t* bitmapData = (this->bitmapDataVec);
  uint8_t* flippedVec = NULL;
  uint8_t* flippedImage = NULL;
//  uint8_t* temp;
  int32_t element1, element2, width3, el1, el2;

	width3 = width * 3;

  if ( !isGrayscale )
  {
  	flippedVec = new uint8_t[height*width3];
  	flippedImage = (flippedVec);
  	for ( int32_t row1 = 0, row2 = (height - 1); row1 < height || row2 > 0; row1++, row2-- )
  	{
		el1 = (row1 * width3); // Beginning of scan line in source
		el2 = (row2 * width3); // Beginning of scan line in result
  	  for ( int32_t col = 0; col < width3; col+=3 )
  	  {
		  // Calculate beginning of this pixel's data for both images
  	    element1 = el1 + col;
  	    element2 = el2 + col;
  	    flippedImage[element2++] = bitmapData[element1++];
  	    flippedImage[element2++] = bitmapData[element1++];
  	    flippedImage[element2] = bitmapData[element1];
  	  }
  	}
  }
  else
  {
    flippedVec = new uint8_t[height*width];
    flippedImage = flippedVec;
  	for ( int32_t row1 = 0, row2 = (height - 1); row1 < height || row2 > 0; row1++, row2-- )
  	{
  	  for ( int32_t col = 0; col < width; col++ )
  	  {
  	    element1 = (row1 * width) + col;
  	    element2 = (row2 * width) + col;
  	    flippedImage[element2] = bitmapData[element1];
  	  }
  	}
  }
 if (bitmapDataVec != NULL)
 {
   delete[] bitmapDataVec;
   bitmapDataVec = NULL;
 }
  this->bitmapDataVec = flippedVec;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MXABmpIO::convertToGrayscale()
{
  uint8_t* bitmapData = (this->bitmapDataVec);
  uint8_t* ptr = NULL;
  //Uses the function Y = 0.3R + 0.59G + 0.11B
  if ( this->isGrayscale )
  {
    return;
  }
  uint8_t r, g, b;
  double R, B, G;
  uint8_t* grayscaleImage;
  int32_t element1, element2;
  uint8_t* grayscaleVec = new uint8_t[this->height * this->width];
  grayscaleImage = (grayscaleVec);
  for ( int32_t row = 0; row < height; row++ )
  {
    for ( int32_t col = 0; col < width; col++ )
    {
      element1 = (row * width) + col;
      element2 = element1 * 3;
      ptr = bitmapData + element2;
      PIXEL24_TO_GREYVALUE(ptr, grayscaleImage[element1]);
    }
  }
  isGrayscale = true;
  this->_imageConvertedToGrayScale = true;
  if (bitmapDataVec != NULL)
  {
    delete[] bitmapDataVec;
    bitmapDataVec = NULL;
  }
  this->bitmapDataVec = grayscaleVec;
  this->numChannels = 1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool MXABmpIO::isGrayscaleImage()
{
  return isGrayscale;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
uint8_t* MXABmpIO::getImageData()
{
#if 0
  if (makeCopy)
  {
    size_t m_length = this->width * this->height * this->numChannels;
    uint8_t* copy = new uint8_t[m_length];
    ::memcpy(copy, this->bitmapDataVec, m_length);
    return copy;
  }
#endif
  return this->bitmapDataVec;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int32_t MXABmpIO::copyImageData(uint8_t* destinationBuffer)
{
  size_t m_length = this->width * this->height * this->numChannels;
  uint8_t* b = static_cast<uint8_t*>(::memcpy(destinationBuffer, this->bitmapDataVec, m_length));
  return (b == destinationBuffer) ? 1 : -1;
}

// -----------------------------------------------------------------------------
// Reads and returns a 32-bit value from the file.
// -----------------------------------------------------------------------------
int32_t MXABmpIO::read32BitValue()
{
  int32_t value;
  this->m_Reader64->read<MXA::Endian::FromLittleToSystem>(value);
  bytesRead+=4;
  return value;
}

// -----------------------------------------------------------------------------
// Reads and returns a 16-bit value from the file
// -----------------------------------------------------------------------------
int16_t MXABmpIO::read16BitValue()
{
  int16_t value;
  this->m_Reader64->read<MXA::Endian::FromLittleToSystem>(value);
  bytesRead+=2;
  return value;
}

// -----------------------------------------------------------------------------
// Reads and returns a 8-bit value from the file
// -----------------------------------------------------------------------------
uint8_t MXABmpIO::read8BitValue()
{
  uint8_t value;
  this->m_Reader64->readValue(value);
  bytesRead+=1;
  return value;
}
