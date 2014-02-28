/* ============================================================================
 *
 *
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#ifndef _ITKUtilities_H_
#define _ITKUtilities_H_

#include "itkImage.h"
#include "itkImportImageFilter.h"
#include "itkImageRegionIterator.h"
#include "DREAM3DLib/DataContainers/VoxelDataContainer.h"

/*
 * @class ITKUtilities ITKUtilities.h DREAM3DLib/Plugins/ITK/ITKFilters/ITKUtilities.h
 * @brief This class performs conversions between dream3d and ITK formats and other utilities
 * @author Will Lenthe (UCSB)
 * @date Feb 26, 2014
 * @version 1.0
 */

 //define commonly used pixel types
typedef uint8_t ITK_CharPixelType;//8 bit images (dream3d ImageData)
typedef float ITK_FloatPixelType;//32 bit images (normal itk output)

const unsigned int ITK_ImageDimension = 3;

//define commonly used image types
typedef itk::Image< ITK_CharPixelType, ITK_ImageDimension > ITK_CharImageType;
typedef itk::Image< ITK_FloatPixelType, ITK_ImageDimension > ITK_FloatImageType;

//define import filters
typedef itk::ImportImageFilter<ITK_FloatPixelType, ITK_ImageDimension> ImportFloatFilterType;
typedef itk::ImportImageFilter<ITK_CharPixelType, ITK_ImageDimension> ImportCharFilterType;

//define iterators
typedef itk::ImageRegionConstIterator< ITK_FloatImageType > FloatConstIteratorType;
typedef itk::ImageRegionConstIterator< ITK_CharImageType > CharConstIteratorType;

//functions
ImportFloatFilterType::Pointer Dream3DFloatoITK(VoxelDataContainer* m, float* data);
ImportCharFilterType::Pointer Dream3DChartoITK(VoxelDataContainer* m, uint8_t* data);


#endif /* _OrientationMath_H_ */
