#pragma once

#include <memory>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/DataContainers/AttributeMatrix.h"
#include "SIMPLib/DataContainers/DataContainer.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/Geometry/ImageGeom.h"

#include "ImportExportTestFileLocations.h"

int generateFeatureIds(DataContainerArray& dca, const DataArrayPath& dap)
{
  DataContainer::Pointer m = dca.getDataContainer(dap.getDataContainerName());
  if(m == nullptr)
  {
    return -1;
  }
  AttributeMatrix::Pointer cellAttrMat = m->getAttributeMatrix(dap.getAttributeMatrixName());
  if(cellAttrMat == nullptr)
  {
    return -2;
  }

  std::vector<size_t> tDims = {UnitTest::FeatureIdsTest::XSize, UnitTest::FeatureIdsTest::YSize, UnitTest::FeatureIdsTest::ZSize};
  cellAttrMat->resizeAttributeArrays(tDims);

  std::vector<size_t> dims = {1};
  auto featureIds = cellAttrMat->createNonPrereqArray<DataArray<int32_t>>(nullptr, dap.getDataArrayName(), 0, dims);
  if(featureIds == nullptr)
  {
    return -3;
  }

  size_t size = UnitTest::FeatureIdsTest::XSize * UnitTest::FeatureIdsTest::YSize * UnitTest::FeatureIdsTest::ZSize;

  for(size_t i = 0; i < size; ++i)
  {
    (*featureIds)[i] = i + UnitTest::FeatureIdsTest::Offset;
  }

  return 0;
}

void createImageGeomDataContainer(DataContainerArray& dca, size_t xDim, size_t yDim, size_t zDim)
{
  DataContainer::Pointer m = DataContainer::New(SIMPL::Defaults::DataContainerName);
  ImageGeom::Pointer image = ImageGeom::CreateGeometry(SIMPL::Geometry::ImageGeometry);
  image->setDimensions(xDim, yDim, zDim);
  m->setGeometry(image);
  dca.addOrReplaceDataContainer(m);
  std::vector<size_t> tDims = {xDim, yDim, zDim};
  AttributeMatrix::Pointer attrMat = AttributeMatrix::New(tDims, SIMPL::Defaults::CellAttributeMatrixName, AttributeMatrix::Type::Cell);
  m->addOrReplaceAttributeMatrix(attrMat);
}
