/* ============================================================================
 *
 *
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#ifndef _ITKUtilities_H_
#define _ITKUtilities_H_

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"

#include "Plugins/ImageProcessing/ImageProcessingConstants.h"
#include "DREAM3DLib/DataContainers/VoxelDataContainer.h"
#include "itkExtractImageFilter.h"

/*
 * @class ITKUtilities ITKUtilities.h DREAM3DLib/Plugins/ImageProcessing/ImageProcessingFilters/ITKUtilities.h
 * @brief This class performs conversions between dream3d and ITK formats and other utilities
 * @author Will Lenthe (UCSB)
 * @date Feb 26, 2014
 * @version 1.0
 */

class DREAM3DLib_EXPORT ITKUtilities
{
  public:
    DREAM3D_SHARED_POINTERS(ITKUtilities)
    DREAM3D_TYPE_MACRO(ITKUtilities)

    virtual ~ITKUtilities();

    //conversion from dream3d arrays to importfilter
    template<typename T>
    static typename itk::ImportImageFilter<T, ImageProcessing::ImageDimension>::Pointer Dream3DtoITKImportFilter(VoxelDataContainer* m, T* data)
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
      int64_t totalPoints = m->getTotalPoints();

      //create and setup import filter
      typedef typename itk::ImportImageFilter<T, ImageProcessing::ImageDimension> ImportFilterType;
      typename ImportFilterType::Pointer importFilter = ImportFilterType::New();

      typename ImportFilterType::SizeType  size;
      size[0]  = dims[0];  // size along X
      size[1]  = dims[1];  // size along Y
      size[2]  = dims[2];  // size along Z

      typename ImportFilterType::IndexType start;
      start.Fill( 0 );

      typename ImportFilterType::RegionType region;
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

    //conversion from dream3d arrays to itk images
    template<typename T>
    static typename itk::Image<T,ImageProcessing::ImageDimension>::Pointer Dream3DtoITK(VoxelDataContainer* m, T* data)
    {
      return Dream3DtoITKImportFilter(m, data)->GetOutput();
    }

    //set itk image buffer to dream3d array
    template<typename T>
    static void SetITKOutput(typename itk::Image<T, ImageProcessing::ImageDimension>::Pointer image, typename T* output, const unsigned int totalPoints)
    {
      const bool filterWillDeleteTheInputBuffer = false;
      image->GetPixelContainer()->SetImportPointer(output, totalPoints, filterWillDeleteTheInputBuffer);
    }

    //copy itk image to dream3d array
    template<typename T>
    static void CopyITKtoDream3D(typename itk::Image<T, ImageProcessing::ImageDimension>::Pointer image, T* buffer)
    {
      typename itk::Image<T, ImageProcessing::ImageDimension>::RegionType filterRegion = image->GetBufferedRegion();
      typedef typename itk::ImageRegionConstIterator<itk::Image<T, ImageProcessing::ImageDimension> > ConstIteratorType;
      ConstIteratorType it(image, filterRegion);
      it.GoToBegin();
      int index=0;
      while(!it.IsAtEnd())
      {
        buffer[index]=it.Get();
        ++it;
        ++index;
      }
    }

    //extract a slice
    template<typename T>
    static typename itk::Image<T,ImageProcessing::SliceDimension>::Pointer ExtractSlice(typename itk::Image<T,ImageProcessing::ImageDimension>::Pointer image, int sliceType, int sliceNum)
    {
      typedef typename itk::ExtractImageFilter< itk::Image<T,ImageProcessing::ImageDimension>, itk::Image<T,ImageProcessing::SliceDimension> > SliceExtractFilter;
      typename SliceExtractFilter::Pointer extractSlice = SliceExtractFilter::New();
      typename itk::Image<T,ImageProcessing::ImageDimension>::RegionType inputRegion = image->GetLargestPossibleRegion();
      typename itk::Image<T,ImageProcessing::ImageDimension>::SizeType size = inputRegion.GetSize();
      //if(ImageProcessing::XSlice==sliceType) size[0]=0;
      size[sliceType]=0;
      typename itk::Image<T,ImageProcessing::ImageDimension>::IndexType start = inputRegion.GetIndex();
      start[sliceType]=sliceNum;
      typename itk::Image<T,ImageProcessing::ImageDimension>::RegionType extractedRegion;
      extractedRegion.SetSize(size);
      extractedRegion.SetIndex(start);
      extractSlice->SetInput(image);
      extractSlice->SetExtractionRegion(extractedRegion);
      extractSlice->SetDirectionCollapseToIdentity();
      extractSlice->Update();
      return extractSlice->GetOutput();
    }

    ITKUtilities();

  private:
    ITKUtilities(const ITKUtilities&); // Copy Constructor Not Implemented
    void operator=(const ITKUtilities&); // Operator '=' Not Implemented
};

#endif /* _ITKUtilities_H_ */
