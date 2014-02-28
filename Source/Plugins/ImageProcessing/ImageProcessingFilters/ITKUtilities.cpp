/* ============================================================================
 *
 *
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "ITKUtilities.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ITKUtilities::ITKUtilities()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ITKUtilities::~ITKUtilities()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ImageProcessing::ImportFloatFilterType::Pointer ITKUtilities::Dream3DFloattoITK(VoxelDataContainer* m, float* data)
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
  ImageProcessing::ImportFloatFilterType::Pointer importFilter = ImageProcessing::ImportFloatFilterType::New();

  ImageProcessing::ImportFloatFilterType::SizeType  size;
  size[0]  = dims[0];  // size along X
  size[1]  = dims[1];  // size along Y
  size[2]  = dims[2];  // size along Z

  ImageProcessing::ImportFloatFilterType::IndexType start;
  start.Fill( 0 );

  ImageProcessing::ImportFloatFilterType::RegionType region;
  region.SetIndex( start );
  region.SetSize(  size  );
  importFilter->SetRegion( region );

  double origin[ ImageProcessing::ImageDimension ];///this may not be zero if cropped w/o updating origin
  origin[0] = 0.0;    // X coordinate
  origin[1] = 0.0;    // Y coordinate
  origin[2] = 0.0;    // Z coordinate
  importFilter->SetOrigin( origin );

  double spacing[ ImageProcessing::ImageDimension ];///should get actual spacing here too
  spacing[0] = 1.0;    // along X direction
  spacing[1] = 1.0;    // along Y direction
  spacing[2] = 1.0;    // along Z direction
  importFilter->SetSpacing( spacing );

  const bool importImageFilterWillOwnTheBuffer = false;
  importFilter->SetImportPointer( data, totalPoints, importImageFilterWillOwnTheBuffer );
  importFilter->Update();
  return importFilter;
  //return importFilter->GetOutput();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ImageProcessing::ImportCharFilterType::Pointer ITKUtilities::Dream3DChartoITK(VoxelDataContainer* m, uint8_t* data)
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
  ImageProcessing::ImportCharFilterType::Pointer importFilter = ImageProcessing::ImportCharFilterType::New();

  ImageProcessing::ImportCharFilterType::SizeType  size;
  size[0]  = dims[0];  // size along X
  size[1]  = dims[1];  // size along Y
  size[2]  = dims[2];  // size along Z

  ImageProcessing::ImportCharFilterType::IndexType start;
  start.Fill( 0 );

  ImageProcessing::ImportCharFilterType::RegionType region;
  region.SetIndex( start );
  region.SetSize(  size  );
  importFilter->SetRegion( region );

  double origin[ ImageProcessing::ImageDimension ];///this may not be zero if cropped w/o updating origin
  origin[0] = 0.0;    // X coordinate
  origin[1] = 0.0;    // Y coordinate
  origin[2] = 0.0;    // Z coordinate
  importFilter->SetOrigin( origin );

  double spacing[ ImageProcessing::ImageDimension ];///should get actual spacing here too
  spacing[0] = 1.0;    // along X direction
  spacing[1] = 1.0;    // along Y direction
  spacing[2] = 1.0;    // along Z direction
  importFilter->SetSpacing( spacing );

  const bool importImageFilterWillOwnTheBuffer = false;
  importFilter->SetImportPointer( data, totalPoints, importImageFilterWillOwnTheBuffer );
  importFilter->Update();
  return importFilter;
  //return importFilter->GetOutput();
}

