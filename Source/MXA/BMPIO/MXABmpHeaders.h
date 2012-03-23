///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, mjackson
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
//  This code was written under United States Air Force Contract number 
//                           FA8650-04-C-5229
//
///////////////////////////////////////////////////////////////////////////////
#ifndef MXA_BMP_HEADERS_H
#define MXA_BMP_HEADERS_H

#include <MXA/MXATypes.h>

enum CompressionType {
  BMP_BI_RGB=0, 
  BMP_BI_RLE8, 
  BMP_BI_RLE4 
  };

/**
 * @brief This holds the bitmap file header values
 * @struct MXABMPFileHeader
 */
struct MXABMPFileHeader
{
	int16_t signature;		/* Must be 4D42 hex */
	int32_t fileSize;			/* size of file in bytes */
	int16_t reserved1;
	int16_t reserved2;
	int32_t dataOffset;		/* starting address of bitmap data */
};

/* Only supported header is the V3 header */
/**
 * @brief This holds the Bitmap info header values. Only V3 headers are supported.
 * @struct MXABMPDIBHeader
 */
struct MXABMPDIBHeader
{
	uint32_t headerSize;		/* Must be 40 */
	int32_t width;
	int32_t height;
	int16_t numColorPlanes;	/* Everything I see says this must be 1 */
	int16_t bitsPerPixel;		/* Typical values are 1, 4, 8, 16, 24 and 32. */
	CompressionType compressionMethod;
	uint32_t imageSize;
	int32_t horizontalResolution;
	int32_t verticalResolution;
	uint32_t numPaletteColors;
	uint32_t numImportantColors;
};



#endif


