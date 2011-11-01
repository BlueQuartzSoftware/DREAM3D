/* ============================================================================
 * Copyright (c) 2011 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2011 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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
 *                           FA8650-07-D-5800
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include <stdlib.h>

#include <string>

#include <boost/shared_ptr.hpp>

#include "MXA/MXA.h"
#include "MXA/Utilities/MXAFileInfo.h"
#include "MXA/Common/LogTime.h"

#include "DREAM3DLib/Common/AIMArray.hpp"
#include "DREAM3DLib/Common/OIMColoring.hpp"

#include "EbsdLib/EbsdLib.h"
#include "EbsdLib/EbsdReader.h"
#include "EbsdLib/EbsdLibVersion.h"

#include "EbsdLib/TSL/AngConstants.h"
#include "EbsdLib/TSL/AngReader.h"
#include "EbsdLib/HKL/CtfConstants.h"
#include "EbsdLib/HKL/CtfReader.h"

#define _TIFF_DATA_TYPEDEFS_ 1
#include <tiffio.h>

int writeColorTiff(const std::string filename, AIMArray<uint8_t>::Pointer image, int width, int height,
                   const std::string imageDescription, int orientation)
{

  int err;
   TIFF *out;
   std::string dateTime;
   char software[1024];
   tsize_t area;

   if (NULL == image)
   {
     return -1;
   }
   out = TIFFOpen(filename.c_str(), "w");
   if (out == NULL)
   {
     printf("Could not open output file '%s' for writing.\n", filename.c_str());
     return -1;
   }

   err = 0;
   // set the basic values
   err = TIFFSetField(out, TIFFTAG_IMAGEWIDTH, width);
   err = TIFFSetField(out, TIFFTAG_IMAGELENGTH, height);
   err = TIFFSetField(out, TIFFTAG_BITSPERSAMPLE, 8);
   err = TIFFSetField(out, TIFFTAG_SAMPLESPERPIXEL, 3);
   err = TIFFSetField(out, TIFFTAG_ROWSPERSTRIP, height); // 1 strip
   err = TIFFSetField(out, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG); // single image plane

   dateTime = tifDateTime();
   err = TIFFSetField(out, TIFFTAG_DATETIME, dateTime.c_str());
   // String based tags
   if (filename.empty() == false)
   {
     err = TIFFSetField(out, TIFFTAG_DOCUMENTNAME, filename.c_str());
   }
   if (imageDescription.empty() == false)
   {
     err = TIFFSetField(out, TIFFTAG_IMAGEDESCRIPTION, imageDescription.c_str());
   }

   err = TIFFSetField(out, TIFFTAG_ORIENTATION, orientation);
   err = TIFFSetField(out, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);


 #if USE_LZW_COMPRESSION
   err = TIFFSetField(image, TIFFTAG_COMPRESSION, COMPRESSION_LZW);
   err = TIFFSetField(image, TIFFTAG_PREDICTOR, PREDICTOR_HORIZONTAL);
 #else
   err = TIFFSetField(out, TIFFTAG_COMPRESSION, COMPRESSION_NONE);
 #endif

   // Insert Resolution Units here if possible


   memset(software, 0, 1024);
   snprintf(software, 1024, "%s using libTif", EbsdLib_PACKAGE_COMPLETE);

   err = TIFFSetField(out, TIFFTAG_SOFTWARE, software);

   err = TIFFSetField(out, TIFFTAG_HOSTCOMPUTER, MXADATAMODEL_SYSTEM);

   // Write the information to the file
   area = (tsize_t)( width *  height * 3);
   err = TIFFWriteEncodedStrip(out, 0, image->GetVoidPointer(0), area);
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




int main(int argc, char **argv)
{

  std::string ebsdFile = argv[1];
  std::string imageFile = argv[2];
  int orientation = atoi(argv[3]);

  std::cout << "Generating IPF Color map [0,-1,0] for EBSD data" << std::endl;
  std::cout << "  " << ebsdFile << std::endl;

  boost::shared_ptr<EbsdReader> ebsdReader(static_cast<EbsdReader*>(NULL));
  bool degToRads = false;

  std::string ext = MXAFileInfo::extension(ebsdFile);
  if (ext.compare(Ebsd::Ang::FileExt) == 0)
  {
    AngReader* reader = new AngReader;
    ebsdReader.reset(static_cast<EbsdReader*>(reader));
  }
  else if (ext.compare(Ebsd::Ctf::FileExt) == 0)
  {
    CtfReader* reader = new CtfReader;
    ebsdReader.reset(static_cast<EbsdReader*>(reader));
    degToRads = true;
  }

  ebsdReader->setFileName(ebsdFile);
  int err = ebsdReader->readFile();
  if (err < 0)
  {
    std::cout << "Error reading the Ebsd File '" << ebsdFile << "'" << std::endl;
    return  EXIT_FAILURE;
  }


  //unsigned char* rgb;
  unsigned char hkl[3] = { 0, 0, 0 };
  float RefDirection[3] = { 0.0, 1.0, 0.0 };

  int width = ebsdReader->getXDimension();
  int height = ebsdReader->getYDimension();
  int total = width *height;
  AIMArray<uint8_t>::Pointer rgbArray = AIMArray<uint8_t>::CreateArray(total * 3);
  rgbArray->SetNumberOfComponents(3);
  rgbArray->WritePointer(0, total*3);
  // Splat 0xFF across all the data
  ::memset(rgbArray->GetPointer(0), 255, total*3);

  float* e0 = reinterpret_cast<float*>(ebsdReader->getPointerByName("Phi1"));
  float* e1 = reinterpret_cast<float*>(ebsdReader->getPointerByName("Phi"));
  float* e2 = reinterpret_cast<float*>(ebsdReader->getPointerByName("Phi2"));

  for (int i = 0; i < total; ++i) {
    uint8_t* rgb = rgbArray->GetPointer(i*3);
    OIMColoring::GenerateIPFColor(e0[i], e1[i], e2[i],
                                  RefDirection[0], RefDirection[1], RefDirection[2],
                                  rgb, hkl, degToRads);
  }

  err = writeColorTiff(imageFile, rgbArray, width, height, "IPF Color Map", orientation);
  if (err < 0)
  {
    std::cout << "Error writing tiff file" << std::endl;
  }

  std::cout << "Generation complete" << std::endl;
  return err;
}



