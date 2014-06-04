/* ============================================================================
 *
 *
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#ifndef _ITKUtilities_H_
#define _ITKUtilities_H_

// ITK Includes first
#include "itkExtractImageFilter.h"


// DREAM3D Includes next
#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/DataArrays/DataArray.hpp"
#include "DREAM3DLib/DataContainers/VolumeDataContainer.h"
#include "DREAM3DLib/DataContainers/AttributeMatrix.h"

// Our Plugin Includes next
#include "ImageProcessing/ImageProcessingConstants.h"




/*
 * @class ITKUtilities ITKUtilities.h DREAM3DLib/Plugins/ImageProcessing/ImageProcessingFilters/ITKUtilities.h
 * @brief This class performs conversions between DREAM3D and ITK formats and other utilities
 * @author Will Lenthe (UCSB)
 * @date Feb 26, 2014
 * @version 1.0
 */
template<typename PixelType>
class DREAM3DLib_EXPORT ITKUtilities
{
  public:
    DREAM3D_SHARED_POINTERS(ITKUtilities<PixelType>)
    DREAM3D_TYPE_MACRO(ITKUtilities<PixelType>)

    virtual ~ITKUtilities();

    //define image and iterator types

    typedef typename itk::Image<PixelType, ImageProcessing::ImageDimension>                       ItkImageType;  // 3D Image
    typedef typename itk::ImageRegionConstIterator<ItkImageType>                                  ItkImageRegionConstIteratorType;
    typedef typename itk::Image<PixelType, ImageProcessing::SliceDimension>                       ItkSliceImageType;  //2D Image
    typedef typename itk::ImageRegionConstIterator<ItkSliceImageType>                             SliceConstIteratorType;
    typedef typename itk::ImageRegionIterator<ItkImageType>                                       VolumeIteratorType;

    typedef typename itk::ImportImageFilter<PixelType, ImageProcessing::ImageDimension>          ImportImageFilterType;


    typedef itk::RGBPixel<PixelType>                                  RGBPixelType;
    typedef itk::Image<RGBPixelType, ImageProcessing::ImageDimension> RGBImageType;
    typedef itk::ImageRegionIterator<RGBImageType>                    IteratorType;
    typedef typename itk::ImageRegionConstIterator<RGBImageType>      ItkRGBImageRegionConstIteratorType;
    typedef typename itk::ExtractImageFilter<ItkImageType, ItkSliceImageType>      SliceExtractFilter;

    /**
     * @brief Dream3DtoITKImportFilter Grayscale conversion / copying & conversion from dream3d arrays to importfilter
     * @param m
     * @param attrMatName
     * @param data
     * @return
     */
    static typename ImportImageFilterType::Pointer Dream3DtoITKImportFilter(VolumeDataContainer* m, QString attrMatName, PixelType* data)
    {
      AttributeMatrix::Pointer attrMat = m->getAttributeMatrix(attrMatName);

      //get size+dimensions of dataset
      //  size_t udims[3] = { 0, 0, 0 };
      QVector<size_t> udims = attrMat->getTupleDimensions();

      //  DimType dims[3] = { static_cast<DimType>(udims[0]), static_cast<DimType>(udims[1]), static_cast<DimType>(udims[2]), };
      size_t totalPoints = attrMat->getNumTuples();

      //create and setup import filter

      typename ImportImageFilterType::Pointer importFilter = ImportImageFilterType::New();

      typename ImportImageFilterType::SizeType  size;
      size[0]  = udims[0];  // size along X
      size[1]  = udims[1];  // size along Y
      size[2]  = udims[2];  // size along Z

      typename ImportImageFilterType::IndexType start;
      start.Fill( 0 );

      typename ImportImageFilterType::RegionType region;
      region.SetIndex( start );
      region.SetSize(  size  );
      importFilter->SetRegion( region );

      float sampleOrigin[3] = {0.0f, 0.0f, 0.0f};
      m->getOrigin(sampleOrigin);
      double origin[ ImageProcessing::ImageDimension ];
      origin[0] = sampleOrigin[0];    // X coordinate
      origin[1] = sampleOrigin[1];    // Y coordinate
      origin[2] = sampleOrigin[2];    // Z coordinate
      importFilter->SetOrigin( origin );

      float voxelResolution[3] = {0.0f, 0.0f, 0.0f};
      m->getResolution(voxelResolution);
      double spacing[ ImageProcessing::ImageDimension ];
      spacing[0] = voxelResolution[0];    // along X direction
      spacing[1] = voxelResolution[1];    // along Y direction
      spacing[2] = voxelResolution[2];    // along Z direction
      importFilter->SetSpacing( spacing );

      const bool importImageFilterWillOwnTheBuffer = false;
      importFilter->SetImportPointer( data, totalPoints, importImageFilterWillOwnTheBuffer );
      importFilter->Update();
      return importFilter;
    }

    /**
     * @brief Dream3DtoITK conversion from dream3d arrays to itk images
     * @param m
     * @param attrMatName
     * @param data
     * @return
     */
    static typename ItkImageType::Pointer Dream3DtoITK(VolumeDataContainer* m, QString attrMatName, PixelType* data)
    {
      return Dream3DtoITKImportFilter(m, attrMatName, data)->GetOutput();
    }

    /**
     * @brief SetITKOutput set buffer of image to dream3d array
     * @param image
     * @param output
     * @param totalPoints
     */
    static void SetITKOutput(typename ItkImageType::Pointer image, typename DataArray<PixelType>::Pointer array) // * output, const unsigned int totalPoints)
    {
      const bool filterWillDeleteTheInputBuffer = false;
      image->GetPixelContainer()->SetImportPointer(array->getPointer(0), array->getNumberOfTuples(), filterWillDeleteTheInputBuffer);
    }


    /**
     * @brief CopyITKtoDream3D copy itk image to dream3d array
     * @param image
     * @param buffer
     */
    static void CopyITKtoDream3D(typename ItkImageType::Pointer image, PixelType* buffer)
    {
      typename ItkImageType::RegionType filterRegion = image->GetBufferedRegion();

      ItkImageRegionConstIteratorType it(image, filterRegion);
      it.GoToBegin();
      int index=0;
      while(!it.IsAtEnd())
      {
        buffer[index]=it.Get();
        ++it;
        ++index;
      }
    }



    /**
     * @brief Dream3DRGBtoITK Converts DREAM3D DataArray into an RGB Pixel for use in ITK. While the method will work
     * for any "DataArray" based class the individual values will be converted to 8 bit unsigned chars so if the values
     * of each component are outside the range of 0 255 then unpredictable results will occur.
     * @param m
     * @param data
     * @return
     */
    static typename RGBImageType::Pointer Dream3DRGBtoITK(VolumeDataContainer* m, QString attrMatName, PixelType* data)
    {
      //get size+dimensions of dataset
      size_t udims[3] =
      { 0, 0, 0 };
      m->getDimensions(udims);
#if (CMP_SIZEOF_SIZE_T == 4)
      typedef int32_t DimType;
#else
      typedef int64_t DimType;
#endif
      DimType dims[3] = { static_cast<DimType>(udims[0]), static_cast<DimType>(udims[1]), static_cast<DimType>(udims[2]), };
      //int64_t totalPoints = m->getTotalPoints();

      //copy dataset dimensions
      typename RGBImageType::SizeType size;
      size[0]=dims[0];
      size[1]=dims[1];
      size[2]=dims[2];

      //create image region
      typename RGBImageType::IndexType start;
      start.Fill(0);
      typename RGBImageType::RegionType region(start,size);

      //create and allocate image
      typename RGBImageType::Pointer image = RGBImageType::New();
      image->SetRegions(region);
      image->Allocate();

      //iterate over image copying data
      IteratorType it(image, region);
      it.GoToBegin();
      int index=-1;
      while(!it.IsAtEnd())
      {
        RGBPixelType pixel;
        pixel[0]=static_cast<unsigned char>(data[++index]);
        pixel[1]=static_cast<unsigned char>(data[++index]);
        pixel[2]=static_cast<unsigned char>(data[++index]);
        it.Set(pixel);
        ++it;
      }
      return image;
    }


    /**
     * @brief CopyRGBITKtoDream3D copy itk image to dream3d array
     * @param image
     * @param buffer
     */
    static void CopyRGBITKtoDream3D(typename RGBImageType::Pointer image, PixelType* buffer)
    {
      typename RGBImageType::RegionType filterRegion = image->GetBufferedRegion();

      ItkRGBImageRegionConstIteratorType it(image, filterRegion);
      it.GoToBegin();
      int index=-1;
      while(!it.IsAtEnd())
      {
        typename itk::RGBPixel<PixelType> pixel = it.Get();
        buffer[++index]=pixel.GetRed();
        buffer[++index]=pixel.GetGreen();
        buffer[++index]=pixel.GetBlue();
        ++it;
      }
    }


    /**
     * @brief ExtractSlice extract a slice
     * @param image
     * @param sliceType
     * @param sliceNum
     * @return
     */
    static typename ItkSliceImageType::Pointer ExtractSlice(typename ItkImageType::Pointer image, int sliceType, int sliceNum)
    {
      typename SliceExtractFilter::Pointer extractSlice = SliceExtractFilter::New();
      typename ItkImageType::RegionType inputRegion = image->GetLargestPossibleRegion();
      typename ItkImageType::SizeType size = inputRegion.GetSize();
      //if(ImageProcessing::XSlice==sliceType) size[0]=0;
      size[sliceType]=0;
      typename ItkImageType::IndexType start = inputRegion.GetIndex();
      start[sliceType]=sliceNum;
      typename ItkImageType::RegionType extractedRegion;
      extractedRegion.SetSize(size);
      extractedRegion.SetIndex(start);
      extractSlice->SetInput(image);
      extractSlice->SetExtractionRegion(extractedRegion);
      extractSlice->SetDirectionCollapseToIdentity();
      extractSlice->Update();
      return extractSlice->GetOutput();
    }

    /**
     * @brief SetSlice update a slice
     * @param image
     * @param slice
     * @param sliceType
     * @param sliceNum
     */
    static void SetSlice(typename ItkImageType::Pointer image, typename ItkSliceImageType::Pointer slice, int sliceType, int sliceNum)
    {
      //get region to replace
      typename ItkImageType::RegionType volumeRegion = image->GetLargestPossibleRegion();
      typename ItkImageType::SizeType size = volumeRegion.GetSize();
      size[sliceType]=1;
      volumeRegion.SetSize(size);
      typename ItkImageType::IndexType start = volumeRegion.GetIndex();
      start.Fill(0);
      start[sliceType]=sliceNum;
      volumeRegion.SetIndex(start);

      //get iterator for slice and image

      SliceConstIteratorType sliceIt(slice, slice->GetLargestPossibleRegion());
      VolumeIteratorType volIt(image, volumeRegion);

      //copy slice into volume
      volIt.GoToBegin();
      sliceIt.GoToBegin();
      while(!volIt.IsAtEnd())
      {
        volIt.Set(sliceIt.Get());
        ++volIt;
        ++sliceIt;
      }
    }

    /*
    static typename itk::Image<typename PixelType,ImageProcessing::ImageDimension>::Pointer ConvertDataArray(int* dims, float* resolution, float* origin, typename DataArray<T>::Pointer inData)
    {
      //create import filter
      typedef typename itk::ImportImageFilter<typename PixelType, ImageProcessing::ImageDimension> ImportFilterType;
      typename ImportFilterType::Pointer importFilter = ImportFilterType::New();
      typename ImportFilterType::IndexType start;
      start.Fill( 0 );

      //get size+dimensions of dataset
      int totalPoints = dims[0]*dims[1]*dims[2];
      typename ImportFilterType::SizeType  size;
      size[0]  = dims[0];  // size along X
      size[1]  = dims[1];  // size along Y
      size[2]  = dims[2];  // size along Z

      //configure import filter
      typename ImportFilterType::RegionType region;
      region.SetIndex( start );
      region.SetSize(  size  );
      importFilter->SetRegion( region );

      importFilter->SetOrigin( origin );
      importFilter->SetSpacing( resolution );

      const bool importImageFilterWillOwnTheBuffer = false;
      importFilter->SetImportPointer( inData, totalPoints, importImageFilterWillOwnTheBuffer );
      importFilter->Update();
      return importFilter->GetOutput();
    }
    */

  protected:
    ITKUtilities();

  private:
    ITKUtilities(const ITKUtilities&); // Copy Constructor Not Implemented
    void operator=(const ITKUtilities&); // Operator '=' Not Implemented
};

#endif /* _ITKUtilities_H_ */
