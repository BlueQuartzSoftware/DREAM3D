/* ============================================================================
 *
 *
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#ifndef _ImageProcessingConstants_H_
#define _ImageProcessingConstants_H_

#include "itkImage.h"
#include "itkImportImageFilter.h"
#include "itkImageRegionIterator.h"

namespace ImageProcessing
{
   //define commonly used pixel types
  typedef uint8_t CharPixelType;//8 bit images (dream3d ImageData)
  typedef float FloatPixelType;//32 bit images (normal itk output)

  const unsigned int ImageDimension = 3;

  //define commonly used image types
  typedef itk::Image< CharPixelType, ImageDimension > CharImageType;
  typedef itk::Image< FloatPixelType, ImageDimension > FloatImageType;

  //define import filters
  typedef itk::ImportImageFilter<FloatPixelType, ImageDimension> ImportFloatFilterType;
  typedef itk::ImportImageFilter<CharPixelType, ImageDimension> ImportCharFilterType;

  //define iterators
  typedef itk::ImageRegionConstIterator< FloatImageType > FloatConstIteratorType;
  typedef itk::ImageRegionConstIterator< CharImageType > CharConstIteratorType;
}

#endif /* _ImageProcessingConstants_H_ */
