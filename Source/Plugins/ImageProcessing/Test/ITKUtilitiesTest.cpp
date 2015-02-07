/* ============================================================================
 * Copyright (c) 2014 DREAM3D Consortium
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
 * Neither the names of any of the DREAM3D Consortium contributors
 * may be used to endorse or promote products derived from this software without
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
 *
 *  This code was partially written under United States Air Force Contract number
 *                              FA8650-10-D-5210
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include "ImageProcessing/ImageProcessingConstants.h"
#include "ImageProcessingFilters/ItkBridge.h"
#include "DREAM3DLib/DataContainers/DataContainer.h"


/**
* These functions are just here to make sure the templated static functions in the ItkBridge class
*  will properly compile. If these are actually executed the program would surely crash
* due to the use of the NULL pointers every where.
*/

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TestDream3DtoITKImportFilter()
{

  ImageProcessing::DefaultImageType::Pointer ptr;


  DataContainer::Pointer m = DataContainer::New();
  ImageGeom::Pointer image = ImageGeom::CreateGeometry(DREAM3D::Geometry::ImageGeometry);
  m->setGeometry(image);
  QString attrMatName("CellData");
  ImageProcessing::DefaultPixelType* data = NULL;

  ImageProcessing::ImportUInt8FilterType::Pointer importFilter = ItkBridge<ImageProcessing::DefaultPixelType>::Dream3DtoITKImportFilter<ImageProcessing::DefaultPixelType>(m, attrMatName, data);

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TestCreateItkWrapperForDataPointer()
{
  ImageProcessing::DefaultImageType::Pointer imagePtr;
  DataContainer::Pointer m = DataContainer::New();
  ImageGeom::Pointer image = ImageGeom::CreateGeometry(DREAM3D::Geometry::ImageGeometry);
  m->setGeometry(image);
  QString attrMatName("CellData");

  ImageProcessing::DefaultPixelType* data = NULL;

  imagePtr = (ImageProcessing::DefaultImageType::Pointer)ItkBridge<ImageProcessing::DefaultPixelType>::CreateItkWrapperForDataPointer(m, attrMatName, data);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TestSetITKOutput()
{

  ImageProcessing::DefaultImageType::Pointer imagePtr;
// ImageProcessing::DefaultPixelType* output = NULL;
// unsigned int totalPoints = 0;
  ImageProcessing::DefaultArrayType::Pointer array;
  ItkBridge<ImageProcessing::DefaultPixelType>::SetITKFilterOutput(imagePtr, array);

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TestCopyITKtoDream3D()
{

  ImageProcessing::DefaultImageType::Pointer imagePtr;
  ImageProcessing::DefaultPixelType* data = NULL;
  ItkBridge<ImageProcessing::DefaultPixelType>::CopyITKtoDream3D(imagePtr, data);

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TestExtractSlice()
{
  //itk::Image<ImageProcessing::DefaultPixelType,ImageProcessing::SliceDimension>::Pointer slicePtr;
  ImageProcessing::DefaultSliceType::Pointer slicePtr;
  ImageProcessing::DefaultImageType::Pointer image;
  slicePtr = ItkBridge<ImageProcessing::DefaultPixelType>::ExtractSlice(image, 0, 0);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TestSetSlice()
{
  // Lets use a typedef to shorten up the "type" that is used in the templates
  //typedef ImageProcessing::DefaultPixelType PixelType;
  // Now declare our Image and Slice variables
  ImageProcessing::DefaultImageType::Pointer imagePtr;
  ImageProcessing::DefaultSliceType::Pointer slicePtr;
  ItkBridge<ImageProcessing::DefaultPixelType>::SetSlice(imagePtr, slicePtr, 0, 0);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int main(int argc, char* argv[])
{
  return EXIT_SUCCESS;
}


