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
  //return importFilter->GetOutput();
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
  //return importFilter->GetOutput();
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
  //return importFilter->GetOutput();
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
  //return importFilter->GetOutput();
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
  //return importFilter->GetOutput();
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
  //return importFilter->GetOutput();
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
  //return importFilter->GetOutput();
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
  //return importFilter->GetOutput();
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
  //return importFilter->GetOutput();
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
  //return importFilter->GetOutput();
}
