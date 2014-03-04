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
ImageProcessing::ImportInt8FilterType::Pointer ITKUtilities::Dream3DInt8toITK(VoxelDataContainer* m, int8_t* data)
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
  ImageProcessing::ImportInt8FilterType::Pointer importFilter = ImageProcessing::ImportInt8FilterType::New();

  ImageProcessing::ImportInt8FilterType::SizeType  size;
  size[0]  = dims[0];  // size along X
  size[1]  = dims[1];  // size along Y
  size[2]  = dims[2];  // size along Z

  ImageProcessing::ImportInt8FilterType::IndexType start;
  start.Fill( 0 );

  ImageProcessing::ImportInt8FilterType::RegionType region;
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

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ImageProcessing::ImportUInt8FilterType::Pointer ITKUtilities::Dream3DUInt8toITK(VoxelDataContainer* m, uint8_t* data)
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
  ImageProcessing::ImportUInt8FilterType::Pointer importFilter = ImageProcessing::ImportUInt8FilterType::New();

  ImageProcessing::ImportUInt8FilterType::SizeType  size;
  size[0]  = dims[0];  // size along X
  size[1]  = dims[1];  // size along Y
  size[2]  = dims[2];  // size along Z

  ImageProcessing::ImportUInt8FilterType::IndexType start;
  start.Fill( 0 );

  ImageProcessing::ImportUInt8FilterType::RegionType region;
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

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ImageProcessing::ImportInt16FilterType::Pointer ITKUtilities::Dream3DInt16toITK(VoxelDataContainer* m, int16_t* data)
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
  ImageProcessing::ImportInt16FilterType::Pointer importFilter = ImageProcessing::ImportInt16FilterType::New();

  ImageProcessing::ImportInt16FilterType::SizeType  size;
  size[0]  = dims[0];  // size along X
  size[1]  = dims[1];  // size along Y
  size[2]  = dims[2];  // size along Z

  ImageProcessing::ImportInt16FilterType::IndexType start;
  start.Fill( 0 );

  ImageProcessing::ImportInt16FilterType::RegionType region;
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

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ImageProcessing::ImportUInt16FilterType::Pointer ITKUtilities::Dream3DUInt16toITK(VoxelDataContainer* m, uint16_t* data)
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
  ImageProcessing::ImportUInt16FilterType::Pointer importFilter = ImageProcessing::ImportUInt16FilterType::New();

  ImageProcessing::ImportUInt16FilterType::SizeType  size;
  size[0]  = dims[0];  // size along X
  size[1]  = dims[1];  // size along Y
  size[2]  = dims[2];  // size along Z

  ImageProcessing::ImportUInt16FilterType::IndexType start;
  start.Fill( 0 );

  ImageProcessing::ImportUInt16FilterType::RegionType region;
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

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ImageProcessing::ImportInt32FilterType::Pointer ITKUtilities::Dream3DInt32toITK(VoxelDataContainer* m, int32_t* data)
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
  ImageProcessing::ImportInt32FilterType::Pointer importFilter = ImageProcessing::ImportInt32FilterType::New();

  ImageProcessing::ImportInt32FilterType::SizeType  size;
  size[0]  = dims[0];  // size along X
  size[1]  = dims[1];  // size along Y
  size[2]  = dims[2];  // size along Z

  ImageProcessing::ImportInt32FilterType::IndexType start;
  start.Fill( 0 );

  ImageProcessing::ImportInt32FilterType::RegionType region;
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

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ImageProcessing::ImportUInt32FilterType::Pointer ITKUtilities::Dream3DUInt32toITK(VoxelDataContainer* m, uint32_t* data)
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
  ImageProcessing::ImportUInt32FilterType::Pointer importFilter = ImageProcessing::ImportUInt32FilterType::New();

  ImageProcessing::ImportUInt32FilterType::SizeType  size;
  size[0]  = dims[0];  // size along X
  size[1]  = dims[1];  // size along Y
  size[2]  = dims[2];  // size along Z

  ImageProcessing::ImportUInt32FilterType::IndexType start;
  start.Fill( 0 );

  ImageProcessing::ImportUInt32FilterType::RegionType region;
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

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ImageProcessing::ImportInt64FilterType::Pointer ITKUtilities::Dream3DInt64toITK(VoxelDataContainer* m, int64_t* data)
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
  ImageProcessing::ImportInt64FilterType::Pointer importFilter = ImageProcessing::ImportInt64FilterType::New();

  ImageProcessing::ImportInt64FilterType::SizeType  size;
  size[0]  = dims[0];  // size along X
  size[1]  = dims[1];  // size along Y
  size[2]  = dims[2];  // size along Z

  ImageProcessing::ImportInt64FilterType::IndexType start;
  start.Fill( 0 );

  ImageProcessing::ImportInt64FilterType::RegionType region;
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

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ImageProcessing::ImportUInt64FilterType::Pointer ITKUtilities::Dream3DUInt64toITK(VoxelDataContainer* m, uint64_t* data)
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
  ImageProcessing::ImportUInt64FilterType::Pointer importFilter = ImageProcessing::ImportUInt64FilterType::New();

  ImageProcessing::ImportUInt64FilterType::SizeType  size;
  size[0]  = dims[0];  // size along X
  size[1]  = dims[1];  // size along Y
  size[2]  = dims[2];  // size along Z

  ImageProcessing::ImportUInt64FilterType::IndexType start;
  start.Fill( 0 );

  ImageProcessing::ImportUInt64FilterType::RegionType region;
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

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ImageProcessing::ImportDoubleFilterType::Pointer ITKUtilities::Dream3DDoubletoITK(VoxelDataContainer* m, double* data)
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
  ImageProcessing::ImportDoubleFilterType::Pointer importFilter = ImageProcessing::ImportDoubleFilterType::New();

  ImageProcessing::ImportDoubleFilterType::SizeType  size;
  size[0]  = dims[0];  // size along X
  size[1]  = dims[1];  // size along Y
  size[2]  = dims[2];  // size along Z

  ImageProcessing::ImportDoubleFilterType::IndexType start;
  start.Fill( 0 );

  ImageProcessing::ImportDoubleFilterType::RegionType region;
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

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ITKUtilities::SetITKInt8Output(ImageProcessing::Int8ImageType::Pointer image, int8_t* output, const unsigned int totalPoints)
{
  const bool filterWillDeleteTheInputBuffer = false;
  ImageProcessing::Int8PixelType * pixelData = static_cast<ImageProcessing::Int8PixelType*>(output);
  image->GetPixelContainer()->SetImportPointer(pixelData, totalPoints, filterWillDeleteTheInputBuffer);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ITKUtilities::SetITKUInt8Output(ImageProcessing::UInt8ImageType::Pointer image, uint8_t* output, const unsigned int totalPoints)
{
  const bool filterWillDeleteTheInputBuffer = false;
  ImageProcessing::UInt8PixelType * pixelData = static_cast<ImageProcessing::UInt8PixelType*>(output);
  image->GetPixelContainer()->SetImportPointer(pixelData, totalPoints, filterWillDeleteTheInputBuffer);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ITKUtilities::SetITKInt16Output(ImageProcessing::Int16ImageType::Pointer image, int16_t* output, const unsigned int totalPoints)
{
  const bool filterWillDeleteTheInputBuffer = false;
  ImageProcessing::Int16PixelType * pixelData = static_cast<ImageProcessing::Int16PixelType*>(output);
  image->GetPixelContainer()->SetImportPointer(pixelData, totalPoints, filterWillDeleteTheInputBuffer);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ITKUtilities::SetITKUInt16Output(ImageProcessing::UInt16ImageType::Pointer image, uint16_t* output, const unsigned int totalPoints)
{
  const bool filterWillDeleteTheInputBuffer = false;
  ImageProcessing::UInt16PixelType * pixelData = static_cast<ImageProcessing::UInt16PixelType*>(output);
  image->GetPixelContainer()->SetImportPointer(pixelData, totalPoints, filterWillDeleteTheInputBuffer);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ITKUtilities::SetITKInt32Output(ImageProcessing::Int32ImageType::Pointer image, int32_t* output, const unsigned int totalPoints)
{
  const bool filterWillDeleteTheInputBuffer = false;
  ImageProcessing::Int32PixelType * pixelData = static_cast<ImageProcessing::Int32PixelType*>(output);
  image->GetPixelContainer()->SetImportPointer(pixelData, totalPoints, filterWillDeleteTheInputBuffer);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ITKUtilities::SetITKUInt32Output(ImageProcessing::UInt32ImageType::Pointer image, uint32_t* output, const unsigned int totalPoints)
{
  const bool filterWillDeleteTheInputBuffer = false;
  ImageProcessing::UInt32PixelType * pixelData = static_cast<ImageProcessing::UInt32PixelType*>(output);
  image->GetPixelContainer()->SetImportPointer(pixelData, totalPoints, filterWillDeleteTheInputBuffer);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ITKUtilities::SetITKInt64Output(ImageProcessing::Int64ImageType::Pointer image, int64_t* output, const unsigned int totalPoints)
{
  const bool filterWillDeleteTheInputBuffer = false;
  ImageProcessing::Int64PixelType * pixelData = static_cast<ImageProcessing::Int64PixelType*>(output);
  image->GetPixelContainer()->SetImportPointer(pixelData, totalPoints, filterWillDeleteTheInputBuffer);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ITKUtilities::SetITKUInt64Output(ImageProcessing::UInt64ImageType::Pointer image, uint64_t* output, const unsigned int totalPoints)
{
  const bool filterWillDeleteTheInputBuffer = false;
  ImageProcessing::UInt64PixelType * pixelData = static_cast<ImageProcessing::UInt64PixelType*>(output);
  image->GetPixelContainer()->SetImportPointer(pixelData, totalPoints, filterWillDeleteTheInputBuffer);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ITKUtilities::SetITKFloatOutput(ImageProcessing::FloatImageType::Pointer image, float* output, const unsigned int totalPoints)
{
  const bool filterWillDeleteTheInputBuffer = false;
  ImageProcessing::FloatPixelType * pixelData = static_cast<ImageProcessing::FloatPixelType*>(output);
  image->GetPixelContainer()->SetImportPointer(pixelData, totalPoints, filterWillDeleteTheInputBuffer);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ITKUtilities::SetITKDoubleOutput(ImageProcessing::DoubleImageType::Pointer image, double* output, const unsigned int totalPoints)
{
  const bool filterWillDeleteTheInputBuffer = false;
  ImageProcessing::DoublePixelType * pixelData = static_cast<ImageProcessing::DoublePixelType*>(output);
  image->GetPixelContainer()->SetImportPointer(pixelData, totalPoints, filterWillDeleteTheInputBuffer);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ITKUtilities::ITKInt8toDream3D(ImageProcessing::Int8ImageType::Pointer image, int8_t* buffer)
{
  ImageProcessing::Int8ImageType::RegionType filterRegion = image->GetBufferedRegion();
  ImageProcessing::Int8ConstIteratorType it(image, filterRegion);
  it.GoToBegin();
  int index=0;
  while(!it.IsAtEnd())
  {
    buffer[index]=it.Get();
    ++it;
    ++index;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ITKUtilities::ITKUInt8toDream3D(ImageProcessing::UInt8ImageType::Pointer image, uint8_t* buffer)
{
  ImageProcessing::UInt8ImageType::RegionType filterRegion = image->GetBufferedRegion();
  ImageProcessing::UInt8ConstIteratorType it(image, filterRegion);
  it.GoToBegin();
  int index=0;
  while(!it.IsAtEnd())
  {
    buffer[index]=it.Get();
    ++it;
    ++index;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ITKUtilities::ITKInt16toDream3D(ImageProcessing::Int16ImageType::Pointer image, int16_t* buffer)
{
  ImageProcessing::Int16ImageType::RegionType filterRegion = image->GetBufferedRegion();
  ImageProcessing::Int16ConstIteratorType it(image, filterRegion);
  it.GoToBegin();
  int index=0;
  while(!it.IsAtEnd())
  {
    buffer[index]=it.Get();
    ++it;
    ++index;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ITKUtilities::ITKUInt16toDream3D(ImageProcessing::UInt16ImageType::Pointer image, uint16_t* buffer)
{
  ImageProcessing::UInt16ImageType::RegionType filterRegion = image->GetBufferedRegion();
  ImageProcessing::UInt16ConstIteratorType it(image, filterRegion);
  it.GoToBegin();
  int index=0;
  while(!it.IsAtEnd())
  {
    buffer[index]=it.Get();
    ++it;
    ++index;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ITKUtilities::ITKInt32toDream3D(ImageProcessing::Int32ImageType::Pointer image, int32_t* buffer)
{
  ImageProcessing::Int32ImageType::RegionType filterRegion = image->GetBufferedRegion();
  ImageProcessing::Int32ConstIteratorType it(image, filterRegion);
  it.GoToBegin();
  int index=0;
  while(!it.IsAtEnd())
  {
    buffer[index]=it.Get();
    ++it;
    ++index;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ITKUtilities::ITKUInt32toDream3D(ImageProcessing::UInt32ImageType::Pointer image, uint32_t* buffer)
{
  ImageProcessing::UInt32ImageType::RegionType filterRegion = image->GetBufferedRegion();
  ImageProcessing::UInt32ConstIteratorType it(image, filterRegion);
  it.GoToBegin();
  int index=0;
  while(!it.IsAtEnd())
  {
    buffer[index]=it.Get();
    ++it;
    ++index;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ITKUtilities::ITKInt64toDream3D(ImageProcessing::Int64ImageType::Pointer image, int64_t* buffer)
{
  ImageProcessing::Int64ImageType::RegionType filterRegion = image->GetBufferedRegion();
  ImageProcessing::Int64ConstIteratorType it(image, filterRegion);
  it.GoToBegin();
  int index=0;
  while(!it.IsAtEnd())
  {
    buffer[index]=it.Get();
    ++it;
    ++index;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ITKUtilities::ITKUInt64toDream3D(ImageProcessing::UInt64ImageType::Pointer image, uint64_t* buffer)
{
  ImageProcessing::UInt64ImageType::RegionType filterRegion = image->GetBufferedRegion();
  ImageProcessing::UInt64ConstIteratorType it(image, filterRegion);
  it.GoToBegin();
  int index=0;
  while(!it.IsAtEnd())
  {
    buffer[index]=it.Get();
    ++it;
    ++index;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ITKUtilities::ITKFloattoDream3D(ImageProcessing::FloatImageType::Pointer image, float* buffer)
{
  ImageProcessing::FloatImageType::RegionType filterRegion = image->GetBufferedRegion();
  ImageProcessing::FloatConstIteratorType it(image, filterRegion);
  it.GoToBegin();
  int index=0;
  while(!it.IsAtEnd())
  {
    buffer[index]=it.Get();
    ++it;
    ++index;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ITKUtilities::ITKDoubletoDream3D(ImageProcessing::DoubleImageType::Pointer image, double* buffer)
{
  ImageProcessing::DoubleImageType::RegionType filterRegion = image->GetBufferedRegion();
  ImageProcessing::DoubleConstIteratorType it(image, filterRegion);
  it.GoToBegin();
  int index=0;
  while(!it.IsAtEnd())
  {
    buffer[index]=it.Get();
    ++it;
    ++index;
  }
}
