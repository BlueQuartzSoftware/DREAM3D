/* ============================================================================
 *
 *
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#ifndef _ImageProcessingConstants_H_
#define _ImageProcessingConstants_H_

#include "itkImage.h"
#include "itkRGBPixel.h"
//#include "itkRGBAPixel.h"
#include "itkImportImageFilter.h"
#include "itkImageRegionIterator.h"
#include "itkImageToImageFilter.h"

namespace ImageProcessing
{
  //define pixels for dream3d variable types
  typedef int8_t Int8PixelType;
  typedef uint8_t UInt8PixelType;
  typedef int16_t Int16PixelType;
  typedef uint16_t UInt16PixelType;
  typedef int32_t Int32PixelType;
  typedef uint32_t UInt32PixelType;
  typedef int64_t Int64PixelType;
  typedef uint64_t UInt64PixelType;
  typedef float FloatPixelType;
  typedef double DoublePixelType;

  //multicomponent pixels
  typedef itk::RGBPixel <uint8_t> RGBUInt8PixelType; //ipf color etc
  //typedef itk::RGBAPixel <float> RGBAFloatPixelType; //may be able to handle quats with this?

  //define dimensionality
  const unsigned int SliceDimension = 2;
  const unsigned int ImageDimension = 3;

  //slice directions
  const unsigned int XSlice = 0;
  const unsigned int YSlice = 1;
  const unsigned int ZSlice = 2;

  //define image types
  typedef itk::Image< Int8PixelType, ImageDimension > Int8ImageType;
  typedef itk::Image< UInt8PixelType, ImageDimension > UInt8ImageType;
  typedef itk::Image< Int16PixelType, ImageDimension > Int16ImageType;
  typedef itk::Image< UInt16PixelType, ImageDimension > UInt16ImageType;
  typedef itk::Image< Int32PixelType, ImageDimension > Int32ImageType;
  typedef itk::Image< UInt32PixelType, ImageDimension > UInt32ImageType;
  typedef itk::Image< Int64PixelType, ImageDimension > Int64ImageType;
  typedef itk::Image< UInt64PixelType, ImageDimension > UInt64ImageType;
  typedef itk::Image< FloatPixelType, ImageDimension > FloatImageType;
  typedef itk::Image< DoublePixelType, ImageDimension > DoubleImageType;

  typedef itk::Image< ImageProcessing::RGBUInt8PixelType, ImageProcessing::ImageDimension > RGBUInt8ImageType;
  //typedef itk::Image< RGBAFloatPixelType, ImageDimension > RGBAFloatImageType;

  typedef itk::Image< Int8PixelType, SliceDimension > Int8SliceType;
  typedef itk::Image< UInt8PixelType, SliceDimension > UInt8SliceType;
  typedef itk::Image< Int16PixelType, SliceDimension > Int16SliceType;
  typedef itk::Image< UInt16PixelType, SliceDimension > UInt16SliceType;
  typedef itk::Image< Int32PixelType, SliceDimension > Int32SliceType;
  typedef itk::Image< UInt32PixelType, SliceDimension > UInt32SliceType;
  typedef itk::Image< Int64PixelType, SliceDimension > Int64SliceType;
  typedef itk::Image< UInt64PixelType, SliceDimension > UInt64SliceType;
  typedef itk::Image< FloatPixelType, SliceDimension > FloatSliceType;
  typedef itk::Image< DoublePixelType, SliceDimension > DoubleSliceType;


  //define import filters
  typedef itk::ImportImageFilter<Int8PixelType, ImageDimension> ImportInt8FilterType;
  typedef itk::ImportImageFilter<UInt8PixelType, ImageDimension> ImportUInt8FilterType;
  typedef itk::ImportImageFilter<Int16PixelType, ImageDimension> ImportInt16FilterType;
  typedef itk::ImportImageFilter<UInt16PixelType, ImageDimension> ImportUInt16FilterType;
  typedef itk::ImportImageFilter<Int32PixelType, ImageDimension> ImportInt32FilterType;
  typedef itk::ImportImageFilter<UInt32PixelType, ImageDimension> ImportUInt32FilterType;
  typedef itk::ImportImageFilter<Int64PixelType, ImageDimension> ImportInt64FilterType;
  typedef itk::ImportImageFilter<UInt64PixelType, ImageDimension> ImportUInt64FilterType;
  typedef itk::ImportImageFilter<FloatPixelType, ImageDimension> ImportFloatFilterType;
  typedef itk::ImportImageFilter<DoublePixelType, ImageDimension> ImportDoubleFilterType;

  typedef itk::ImportImageFilter<RGBUInt8PixelType, ImageDimension> ImportRGBUint8FilterType;
  //typedef itk::ImportImageFilter<RGBAFloatPixelType, ImageDimension> ImportRGBAFloatFilterType;

  //define constant iterators
  typedef itk::ImageRegionConstIterator< Int8ImageType > Int8ConstIteratorType;
  typedef itk::ImageRegionConstIterator< UInt8ImageType > UInt8ConstIteratorType;
  typedef itk::ImageRegionConstIterator< Int16ImageType > Int16ConstIteratorType;
  typedef itk::ImageRegionConstIterator< UInt16ImageType > UInt16ConstIteratorType;
  typedef itk::ImageRegionConstIterator< Int32ImageType > Int32ConstIteratorType;
  typedef itk::ImageRegionConstIterator< UInt32ImageType > UInt32ConstIteratorType;
  typedef itk::ImageRegionConstIterator< Int64ImageType > Int64ConstIteratorType;
  typedef itk::ImageRegionConstIterator< UInt64ImageType > UInt64ConstIteratorType;
  typedef itk::ImageRegionConstIterator< FloatImageType > FloatConstIteratorType;
  typedef itk::ImageRegionConstIterator< DoubleImageType > DoubleConstIteratorType;

  typedef itk::ImageRegionConstIterator< ImageProcessing::RGBUInt8PixelType > RGBUInt8ConstIteratorType;
  //typedef itk::ImageRegionConstIterator< RGBAFloatPixelType > RGBAFloatConstIteratorType;
}

#endif /* _ImageProcessingConstants_H_ */
