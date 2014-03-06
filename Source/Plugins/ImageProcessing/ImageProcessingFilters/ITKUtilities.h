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

    //conversion from dream3d arrays to itk images
    static ImageProcessing::ImportInt8FilterType::Pointer Dream3DInt8toITK(VoxelDataContainer* m, int8_t* data);
    static ImageProcessing::ImportUInt8FilterType::Pointer Dream3DUInt8toITK(VoxelDataContainer* m, uint8_t* data);
    static ImageProcessing::ImportInt16FilterType::Pointer Dream3DInt16toITK(VoxelDataContainer* m, int16_t* data);
    static ImageProcessing::ImportUInt16FilterType::Pointer Dream3DUInt16toITK(VoxelDataContainer* m, uint16_t* data);
    static ImageProcessing::ImportInt32FilterType::Pointer Dream3DInt32toITK(VoxelDataContainer* m, int32_t* data);
    static ImageProcessing::ImportUInt32FilterType::Pointer Dream3DUInt32toITK(VoxelDataContainer* m, uint32_t* data);
    static ImageProcessing::ImportInt64FilterType::Pointer Dream3DInt64toITK(VoxelDataContainer* m, int64_t* data);
    static ImageProcessing::ImportUInt64FilterType::Pointer Dream3DUInt64toITK(VoxelDataContainer* m, uint64_t* data);
    static ImageProcessing::ImportFloatFilterType::Pointer Dream3DFloattoITK(VoxelDataContainer* m, float* data);
    static ImageProcessing::ImportDoubleFilterType::Pointer Dream3DDoubletoITK(VoxelDataContainer* m, double* data);

    //static ImageProcessing::ImportRGBUint8FilterType::Pointer Dream3DRGBUInt8toITK(VoxelDataContainer* m, uint8_t* data);
    //static ImageProcessing::ImportRGBAFloatFilterType::Pointer Dream3DRGBAFloattoITK(VoxelDataContainer* m, float* data);

    template<typename TImage>
    static typename TImage::Pointer Dream3DtoITK(VoxelDataContainer* m, typename TImage::InternalPixelType* data)
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
      typedef typename itk::ImportImageFilter<typename TImage::PixelType, ImageProcessing::ImageDimension> ImportFilterType;
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
      return importFilter->GetOutput();
    }

    //set itk ImageToImageFilter output buffer to dream3d buffer
    static void SetITKInt8Output(ImageProcessing::Int8ImageType::Pointer image, int8_t* output, const unsigned int totalPoints);
    static void SetITKUInt8Output(ImageProcessing::UInt8ImageType::Pointer image, uint8_t* output, const unsigned int totalPoints);
    static void SetITKInt16Output(ImageProcessing::Int16ImageType::Pointer image, int16_t* output, const unsigned int totalPoints);
    static void SetITKUInt16Output(ImageProcessing::UInt16ImageType::Pointer image, uint16_t* output, const unsigned int totalPoints);
    static void SetITKInt32Output(ImageProcessing::Int32ImageType::Pointer image, int32_t* output, const unsigned int totalPoints);
    static void SetITKUInt32Output(ImageProcessing::UInt32ImageType::Pointer image, uint32_t* output, const unsigned int totalPoints);
    static void SetITKInt64Output(ImageProcessing::Int64ImageType::Pointer image, int64_t* output, const unsigned int totalPoints);
    static void SetITKUInt64Output(ImageProcessing::UInt64ImageType::Pointer image, uint64_t* output, const unsigned int totalPoints);
    static void SetITKFloatOutput(ImageProcessing::FloatImageType::Pointer image, float* output, const unsigned int totalPoints);
    static void SetITKDoubleOutput(ImageProcessing::DoubleImageType::Pointer image, double* output, const unsigned int totalPoints);

    template<typename TImageFilter>
    static void SetITKOutput(typename TImageFilter::Pointer filter, typename TImageFilter::OutputImageType::InternalPixelType* output, const unsigned int totalPoints)
    {
      const bool filterWillDeleteTheInputBuffer = false;
      filter->GetOutput()->GetPixelContainer()->SetImportPointer(output, totalPoints, filterWillDeleteTheInputBuffer);
    }

    //copy itk image to dream3d buffer
    static void ITKInt8toDream3D(ImageProcessing::Int8ImageType::Pointer image, int8_t* buffer);
    static void ITKUInt8toDream3D(ImageProcessing::UInt8ImageType::Pointer image, uint8_t* buffer);
    static void ITKInt16toDream3D(ImageProcessing::Int16ImageType::Pointer image, int16_t* buffer);
    static void ITKUInt16toDream3D(ImageProcessing::UInt16ImageType::Pointer image, uint16_t* buffer);
    static void ITKInt32toDream3D(ImageProcessing::Int32ImageType::Pointer image, int32_t* buffer);
    static void ITKUInt32toDream3D(ImageProcessing::UInt32ImageType::Pointer image, uint32_t* buffer);
    static void ITKInt64toDream3D(ImageProcessing::Int64ImageType::Pointer image, int64_t* buffer);
    static void ITKUInt64toDream3D(ImageProcessing::UInt64ImageType::Pointer image, uint64_t* buffer);
    static void ITKFloattoDream3D(ImageProcessing::FloatImageType::Pointer image, float* buffer);
    static void ITKDoubletoDream3D(ImageProcessing::DoubleImageType::Pointer image, double* buffer);

    template<typename TImage>
    static void CopyITKtoDream3D(typename TImage::Pointer image, typename TImage::InternalPixelType* buffer)
    {
      typename TImage::RegionType filterRegion = image->GetBufferedRegion();
      typedef itk::ImageRegionConstIterator<TImage> ConstIteratorType;
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

    ITKUtilities();

  private:
    ITKUtilities(const ITKUtilities&); // Copy Constructor Not Implemented
    void operator=(const ITKUtilities&); // Operator '=' Not Implemented
};

#endif /* _ITKUtilities_H_ */
