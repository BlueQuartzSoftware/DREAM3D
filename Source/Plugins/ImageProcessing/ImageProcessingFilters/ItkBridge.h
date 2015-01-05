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

#ifndef _ItkBridge_H_
#define _ItkBridge_H_

// ITK Includes first
#include "itkExtractImageFilter.h"
#include "itkRGBPixel.h"
#include "itkRGBAPixel.h"

// DREAM3D Includes next
#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/DataArrays/DataArray.hpp"
#include "DREAM3DLib/DataContainers/VolumeDataContainer.h"
#include "DREAM3DLib/DataContainers/AttributeMatrix.h"

// Our Plugin Includes next
#include "ImageProcessing/ImageProcessingConstants.h"


template<typename T, typename K, typename ItkOutPixelType>
class Dream3DToItkImageConversion
{
  public:
    Dream3DToItkImageConversion() {}
    virtual ~Dream3DToItkImageConversion() {}

    /// Itk ImportImage Filter Types
    typedef itk::ImportImageFilter<K, ImageProcessing::ImageDimension>                   ItkImportImageFilterType;
    typedef typename ItkImportImageFilterType::Pointer                                   ItkImportImageFilterPointerType;


    typedef typename itk::Image<ItkOutPixelType, ImageProcessing::ImageDimension>        ItkImageOutType;  // 3D RGB Image
    typedef typename ItkImageOutType::Pointer                                            ItkImageOutPointerType;

    /**
     * @brief operator ()
     * @param m
     * @param attrMatName
     * @param data
     * @return
     */
    ItkImportImageFilterPointerType operator()(VolumeDataContainer* m, QString attrMatName, T* data)
    {
      AttributeMatrix::Pointer attrMat = m->getAttributeMatrix(attrMatName);

      //get size+dimensions of dataset
      QVector<size_t> udims = attrMat->getTupleDimensions();
      size_t totalPoints = attrMat->getNumTuples();

      //create and setup import filter

      ItkImportImageFilterPointerType importFilter = ItkImportImageFilterType::New();

      typename ItkImportImageFilterType::SizeType  size;
      size[0]  = udims[0];  // size along X
      size[1]  = udims[1];  // size along Y
      size[2]  = udims[2];  // size along Z

      typename ItkImportImageFilterType::IndexType start;
      start.Fill( 0 );

      typename ItkImportImageFilterType::RegionType region;
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
      importFilter->SetImportPointer( reinterpret_cast<K*>(data), totalPoints, importImageFilterWillOwnTheBuffer );

      try
      {
        importFilter->Update();
      }
      catch( itk::ExceptionObject& err )
      {
        ItkImportImageFilterPointerType nullPointer;
        importFilter = nullPointer;
      }

      return importFilter;
    }

  private:
    Dream3DToItkImageConversion(const Dream3DToItkImageConversion&); // Copy Constructor Not Implemented
    void operator=(const Dream3DToItkImageConversion&); // Operator '=' Not Implemented
};

#if 0
template<typename InputPixelType, typename OutputPixelType>
class ItkBridge2
{
  public:
    virtual ~ItkBridge2() {}

    typedef itk::ImportImageFilter<OutputPixelType, ImageProcessing::ImageDimension> ItkImportImageFilterType;
    typedef typename ItkImportImageFilterType::Pointer ItkImportImageFilterPointerType;

    static ItkImportImageFilterPointerType Dream3DtoITKImportFilter(VolumeDataContainer* m, QString attrMatName, InputPixelType* data)
    {
      AttributeMatrix::Pointer attrMat = m->getAttributeMatrix(attrMatName);

      //get size+dimensions of dataset
      QVector<size_t> udims = attrMat->getTupleDimensions();
      size_t totalPoints = attrMat->getNumTuples();

      //create and setup import filter

      ItkImportImageFilterPointerType importFilter = ItkImportImageFilterType::New();

      typename ItkImportImageFilterType::SizeType  size;
      size[0]  = udims[0];  // size along X
      size[1]  = udims[1];  // size along Y
      size[2]  = udims[2];  // size along Z

      typename ItkImportImageFilterType::IndexType start;
      start.Fill( 0 );

      typename ItkImportImageFilterType::RegionType region;
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
      importFilter->SetImportPointer( reinterpret_cast<OutputPixelType*>(data), totalPoints, importImageFilterWillOwnTheBuffer );
      importFilter->Update();
      return importFilter;
    }

  protected:
    ItkBridge2() {}

    ItkBridge2(const ItkBridge2&); // Copy Constructor Not Implemented
    void operator=(const ItkBridge2&); // Operator '=' Not Implemented
};
#endif

template<typename ComponentType>
class CreateItkWrapperForDataPointer
{
  public:
    CreateItkWrapperForDataPointer() {}
    virtual ~CreateItkWrapperForDataPointer() {}

    typedef typename itk::Image<ComponentType, ImageProcessing::ImageDimension>                       ScalarImageType;  // 3D Scalar Image
    typedef typename ScalarImageType::Pointer                                                         ScalarImagePointerType;
    typedef itk::ImportImageFilter<ComponentType, ImageProcessing::ImageDimension>                    ImportImageFilterType;
    typedef typename ImportImageFilterType::Pointer                                                   ImportImageFilterPointerType;

    ScalarImagePointerType operator()(VolumeDataContainer* m, QString attrMatName, ComponentType* data)
    {
      AttributeMatrix::Pointer attrMat = m->getAttributeMatrix(attrMatName);

      //get size+dimensions of dataset
      QVector<size_t> udims = attrMat->getTupleDimensions();
      size_t totalPoints = attrMat->getNumTuples();

      //create and setup import filter
      typedef itk::ImportImageFilter<ComponentType, ImageProcessing::ImageDimension> ImportImageFilterType;
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
      importFilter->SetImportPointer(data, totalPoints, importImageFilterWillOwnTheBuffer );
      importFilter->Update();


      // ImportImageFilterPointerType importer = Dream3DtoITKImportFilter<ComponentType>(m, attrMatName, data);
      const ScalarImageType* constImage = importFilter->GetOutput();
      typename ScalarImageType::Pointer image = ScalarImageType::New();
      image->Graft(constImage);
      return image;
    }

  private:
    CreateItkWrapperForDataPointer(const CreateItkWrapperForDataPointer&); // Copy Constructor Not Implemented
    void operator=(const CreateItkWrapperForDataPointer&); // Operator '=' Not Implemented
};



/*
 * @class ItkBridge ItkBridge.h DREAM3DLib/Plugins/ImageProcessing/ImageProcessingFilters/ItkBridge.h
 * @brief This class performs conversions between DREAM3D and ITK formats and other utilities
 * @author Will Lenthe (UCSB)
 * @date Feb 26, 2014
 * @version 1.0
 */
template<typename ComponentType>
class ItkBridge
{
  public:
    DREAM3D_SHARED_POINTERS(ItkBridge<ComponentType>)
    DREAM3D_TYPE_MACRO(ItkBridge<ComponentType>)

    virtual ~ItkBridge() {}


    //*! Define all the Typedefs for this class
    typedef DataArray<ComponentType>                                                                  DataArrayType;
    typedef typename DataArrayType::Pointer                                                           DataArrayPointerType;

    typedef typename itk::Image<ComponentType, ImageProcessing::ImageDimension>                       ScalarImageType;  // 3D Scalar Image
    typedef typename itk::Image<itk::RGBPixel<ComponentType>, ImageProcessing::ImageDimension>        RGBImageType;  // 3D RGB Image
    typedef typename itk::Image<itk::RGBAPixel<ComponentType>, ImageProcessing::ImageDimension>       RGBAImageType;  // 3D RGBA Image

    typedef typename itk::Image<ComponentType, ImageProcessing::SliceDimension>                       ScalarSliceImageType;  // 2D Scalar Image
    typedef typename itk::Image<itk::RGBPixel<ComponentType>, ImageProcessing::SliceDimension>        RGBSliceImageType;  // 2D RGB Image
    typedef typename itk::Image<itk::RGBAPixel<ComponentType>, ImageProcessing::SliceDimension>       RGBASliceImageType;  // 2D RGBA Image

    typedef itk::ImportImageFilter<ComponentType, ImageProcessing::ImageDimension>                    ImportImageFilterType;
    typedef typename ImportImageFilterType::Pointer                                                   ImportImageFilterPointerType;


    /**
     * @brief Dream3DtoITKImportFilter Grayscale conversion / copying & conversion from dream3d arrays to importfilter
     * @param m
     * @param attrMatName
     * @param data
     * @return
     */
    template<typename TPixel>
    static typename itk::ImportImageFilter<TPixel, ImageProcessing::ImageDimension>::Pointer Dream3DtoITKImportFilter(VolumeDataContainer* m, QString attrMatName, ComponentType* data)
    {
      AttributeMatrix::Pointer attrMat = m->getAttributeMatrix(attrMatName);

      //get size+dimensions of dataset
      QVector<size_t> udims = attrMat->getTupleDimensions();
      size_t totalPoints = attrMat->getNumTuples();

      //create and setup import filter
      typedef itk::ImportImageFilter<TPixel, ImageProcessing::ImageDimension> ImportImageFilterType;
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
      importFilter->SetImportPointer( reinterpret_cast<TPixel*>(data), totalPoints, importImageFilterWillOwnTheBuffer );
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
    static typename ScalarImageType::Pointer CreateItkWrapperForDataPointer(VolumeDataContainer* m, QString attrMatName, ComponentType* data)
    {
      ImportImageFilterPointerType importer = Dream3DtoITKImportFilter<ComponentType>(m, attrMatName, data);
      const ScalarImageType* constImage = importer->GetOutput();
      typename ScalarImageType::Pointer image = ScalarImageType::New();
      image->Graft(constImage);
      return image;
    }

    /**
     * @brief Dream3DtoITKTemplate
     * @param m
     * @param attrMatName
     * @param data
     * @return
     */
#if 1
    template <typename TImage>
    static typename TImage::Pointer CreateItkWrapperForDataPointer(VolumeDataContainer* m, QString attrMatName, ComponentType* data)
    {
      return Dream3DtoITKImportFilter<typename TImage::PixelType>(m, attrMatName, data)->GetOutput();
    }
#endif
    /**
     * @brief SetITKOutput set buffer of image to dream3d array
     * @param image
     * @param output
     * @param totalPoints
     */
    static void SetITKFilterOutput(typename ScalarImageType::Pointer image, DataArrayPointerType d3dData) // * output, const unsigned int totalPoints)
    {
      const bool filterDeletesData = false;
      ComponentType* data = d3dData->getPointer(0);
      size_t numTuples = d3dData->getNumberOfTuples();
      image->GetPixelContainer()->SetImportPointer(data, numTuples, filterDeletesData);
    }


    /**
     * @brief CopyITKtoDream3D copy itk image to dream3d array
     * @param image
     * @param buffer
     */
    static void CopyITKtoDream3D(typename ScalarImageType::Pointer image, ComponentType* buffer)
    {
      typename ScalarImageType::RegionType filterRegion = image->GetBufferedRegion();
      itk::ImageRegionConstIterator<ScalarImageType> it(image, filterRegion);
      it.GoToBegin();
      int index = 0;
      while(!it.IsAtEnd())
      {
        buffer[index] = it.Get();
        ++it;
        ++index;
      }
    }


    /**
     * @brief ExtractSlice extract a slice
     * @param image
     * @param sliceType
     * @param sliceNum
     * @return
     */
    static typename ScalarSliceImageType::Pointer ExtractSlice(typename ScalarImageType::Pointer image, int sliceType, int sliceNum)
    {
      typedef typename itk::ExtractImageFilter<ScalarImageType, ScalarSliceImageType> SliceExtractFilter;
      typename SliceExtractFilter::Pointer extractSlice = SliceExtractFilter::New();
      typename ScalarImageType::RegionType inputRegion = image->GetLargestPossibleRegion();
      typename ScalarImageType::SizeType size = inputRegion.GetSize();
      //if(ImageProcessing::XSlice==sliceType) size[0]=0;
      size[sliceType] = 0;
      typename ScalarImageType::IndexType start = inputRegion.GetIndex();
      start[sliceType] = sliceNum;
      typename ScalarImageType::RegionType extractedRegion;
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
    static void SetSlice(typename ScalarImageType::Pointer image, typename ScalarSliceImageType::Pointer slice, int sliceType, int sliceNum)
    {
      //get region to replace
      typename ScalarImageType::RegionType volumeRegion = image->GetLargestPossibleRegion();
      typename ScalarImageType::SizeType size = volumeRegion.GetSize();
      size[sliceType] = 1;
      volumeRegion.SetSize(size);
      typename ScalarImageType::IndexType start = volumeRegion.GetIndex();
      start.Fill(0);
      start[sliceType] = sliceNum;
      volumeRegion.SetIndex(start);

      //get iterator for slice and image

      itk::ImageRegionConstIterator<ScalarSliceImageType> sliceIt(slice, slice->GetLargestPossibleRegion());
      itk::ImageRegionIterator<ScalarImageType> volIt(image, volumeRegion);

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

  protected:
    ItkBridge() {}

  private:
    ItkBridge(const ItkBridge&); // Copy Constructor Not Implemented
    void operator=(const ItkBridge&); // Operator '=' Not Implemented
};


//// Create some typedefs for our convenience
typedef ItkBridge<ImageProcessing::DefaultPixelType>   ITKUtilitiesType;



#endif /* _ItkBridge_H_ */
