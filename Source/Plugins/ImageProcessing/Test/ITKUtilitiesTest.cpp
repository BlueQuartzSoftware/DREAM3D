
#include "ImageProcessing/ImageProcessingConstants.h"
#include "ImageProcessingFilters/ItkBridge.h"


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


  VolumeDataContainer::Pointer m = VolumeDataContainer::New();
  QString attrMatName("CellData");
  ImageProcessing::DefaultPixelType* data = NULL;

  ImageProcessing::ImportUInt8FilterType::Pointer importFilter = ItkBridge<ImageProcessing::DefaultPixelType>::Dream3DtoITKImportFilter<ImageProcessing::DefaultPixelType>(m.get(), attrMatName, data);

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TestCreateItkWrapperForDataPointer()
{
  ImageProcessing::DefaultImageType::Pointer imagePtr;
  VolumeDataContainer::Pointer m = VolumeDataContainer::New();
  QString attrMatName("CellData");

  ImageProcessing::DefaultPixelType* data = NULL;

  imagePtr = (ImageProcessing::DefaultImageType::Pointer)ItkBridge<ImageProcessing::DefaultPixelType>::CreateItkWrapperForDataPointer(m.get(), attrMatName, data);
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
/*
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TestDream3DRGBtoITK()
{
  ItkBridge<ImageProcessing::DefaultPixelType>::RGBImageType::Pointer rgbPtr;

  VolumeDataContainer::Pointer m = VolumeDataContainer::New();
  QString attrMatName("CellData");
  ImageProcessing::DefaultPixelType* data = NULL;
  rgbPtr = ItkBridge<ImageProcessing::DefaultPixelType>::Dream3DRGBtoITK(m.get(), attrMatName, data);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TestCopyRGBITKtoDream3D()
{
  ImageProcessing::RGBImageType::Pointer imagePtr;
  ImageProcessing::DefaultPixelType* data = NULL;
  ItkBridge<ImageProcessing::DefaultPixelType>::CopyRGBITKtoDream3D(imagePtr, data);
}
*/
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
  typedef ImageProcessing::DefaultPixelType PixelType;
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


