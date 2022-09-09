/* ============================================================================
 * Copyright (c) 2022-2022 BlueQuartz Software, LLC
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
 * Neither the name of BlueQuartz Software, the US Air Force, nor the names of its
 * contributors may be used to endorse or promote products derived from this software
 * without specific prior written permission.
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
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#pragma once

#include <QtCore/QFile>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/CoreFilters/DataContainerReader.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/Geometry/ImageGeom.h"

#include "Reconstruction/ReconstructionFilters/PartitionGeometry.h"
#include "Reconstruction/Test/ReconstructionTestFileLocations.h"
#include "Reconstruction/Test/UnitTestSupport.hpp"

class PartitionGeometryTest
{

public:
  PartitionGeometryTest() = default;
  ~PartitionGeometryTest() = default;
  PartitionGeometryTest(const PartitionGeometryTest&) = delete;            // Copy Constructor
  PartitionGeometryTest(PartitionGeometryTest&&) = delete;                 // Move Constructor
  PartitionGeometryTest& operator=(const PartitionGeometryTest&) = delete; // Copy Assignment
  PartitionGeometryTest& operator=(PartitionGeometryTest&&) = delete;      // Move Assignment

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestGeometry(const QString& inputFile, const DataArrayPath& arrayPath, const IntVec3Type& numOfPartitionsPerAxis, const FloatVec3Type& partitioningSchemeOrigin,
                    const FloatVec3Type& lengthPerPartition, const QString& exemplaryArrayName, PartitionGeometry::PartitioningMode mode)
  {
    DataContainerArray::Pointer dca = DataContainerArray::New();
    {
      DataContainerReader::Pointer filter = DataContainerReader::New();
      DataContainerArrayProxy dcaProxy = filter->readDataContainerArrayStructure(inputFile);
      filter->setInputFileDataContainerArrayProxy(dcaProxy);
      filter->setInputFile(inputFile);
      filter->setDataContainerArray(dca);
      filter->execute();
      int err = filter->getErrorCode();
      DREAM3D_REQUIRE(err >= 0)
    }

    PartitionGeometry::Pointer filter = PartitionGeometry::New();
    filter->setDataContainerArray(dca);
    filter->setPartitioningMode(static_cast<int>(mode));
    filter->setNumberOfPartitionsPerAxis(numOfPartitionsPerAxis);
    filter->setPartitioningSchemeOrigin(partitioningSchemeOrigin);
    filter->setLengthPerPartition(lengthPerPartition);
    filter->setAttributeMatrixPath(arrayPath);
    filter->setPartitionIdsArrayName(arrayPath.getDataArrayName());

    filter->execute();
    int err = filter->getErrorCode();
    DREAM3D_REQUIRE(err >= 0)

    AttributeMatrix::Pointer am = dca->getAttributeMatrix(arrayPath);
    DREAM3D_REQUIRE(am != AttributeMatrix::NullPointer())

    Int32ArrayType::Pointer partitionIds = am->getAttributeArrayAs<Int32ArrayType>(arrayPath.getDataArrayName());
    DREAM3D_REQUIRE(partitionIds != Int32ArrayType::NullPointer())

    Int32ArrayType::Pointer exemplaryPartitionIds = am->getAttributeArrayAs<Int32ArrayType>(exemplaryArrayName);
    DREAM3D_REQUIRE(exemplaryPartitionIds != Int32ArrayType::NullPointer())

    DREAM3D_REQUIRE_EQUAL(partitionIds->getSize(), exemplaryPartitionIds->getSize())

    int32_t* partitionIdsPtr = partitionIds->getPointer(0);
    int32_t* exemplaryPartitionIdsPtr = exemplaryPartitionIds->getPointer(0);
    for(size_t i = 0; i < partitionIds->getSize(); i++)
    {
      const int32_t partitionId = partitionIdsPtr[i];
      const int32_t exemplaryId = exemplaryPartitionIdsPtr[i];
      DREAM3D_REQUIRE_EQUAL(partitionId, exemplaryId)
    }
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestImageGeometry()
  {
    QString inputFile = UnitTest::PartitionGeometryTest::ExemplaryImageGeomIdsPath;
    IntVec3Type numOfPartitionsPerAxis = {5, 5, 5};
    FloatVec3Type partitioningSchemeOrigin = {-10, 5, 2};
    FloatVec3Type lengthPerPartition = {5, 5, 5};
    DataArrayPath arrayPath = {"DataContainer", "CellData", "PartitionIds"};
    QString exemplaryArrayName = "ExemplaryPartitionIds";

    TestGeometry(inputFile, arrayPath, numOfPartitionsPerAxis, partitioningSchemeOrigin, lengthPerPartition, exemplaryArrayName, PartitionGeometry::PartitioningMode::Advanced);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestRectGridGeometry()
  {
    QString inputFile = UnitTest::PartitionGeometryTest::ExemplaryRectGridGeomIdsPath;
    IntVec3Type numOfPartitionsPerAxis = {5, 5, 5};
    FloatVec3Type partitioningSchemeOrigin = {0, 0, 0};
    FloatVec3Type lengthPerPartition = {6, 6, 6};
    DataArrayPath arrayPath = {"DataContainer", "CellData", "PartitionIds"};
    QString exemplaryArrayName = "ExemplaryPartitionIds";

    TestGeometry(inputFile, arrayPath, numOfPartitionsPerAxis, partitioningSchemeOrigin, lengthPerPartition, exemplaryArrayName, PartitionGeometry::PartitioningMode::Advanced);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestTriangleGeometry()
  {
    QString inputFile = UnitTest::PartitionGeometryTest::ExemplaryTriangleGeomIdsPath;
    IntVec3Type numOfPartitionsPerAxis = {4, 4, 3};
    FloatVec3Type partitioningSchemeOrigin = {0, 0, 0};
    FloatVec3Type lengthPerPartition = {5.01, 5.01, 10.01};
    DataArrayPath arrayPath = {"TriangleDataContainer", "VertexData", "PartitionIds"};
    QString exemplaryArrayName = "ExemplaryPartitionIds";

    TestGeometry(inputFile, arrayPath, numOfPartitionsPerAxis, partitioningSchemeOrigin, lengthPerPartition, exemplaryArrayName, PartitionGeometry::PartitioningMode::Advanced);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestEdgeGeometry()
  {
    QString inputFile = UnitTest::PartitionGeometryTest::ExemplaryEdgeGeomIdsPath;
    IntVec3Type numOfPartitionsPerAxis = {4, 4, 4};
    FloatVec3Type partitioningSchemeOrigin = {-1, -1, -0.05};
    FloatVec3Type lengthPerPartition = {0.5, 0.5, 0.25};
    DataArrayPath arrayPath = {"DataContainer", "VertexData", "PartitionIds"};
    QString exemplaryArrayName = "ExemplaryPartitionIds";

    TestGeometry(inputFile, arrayPath, numOfPartitionsPerAxis, partitioningSchemeOrigin, lengthPerPartition, exemplaryArrayName, PartitionGeometry::PartitioningMode::Advanced);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestVertexGeometry()
  {
    QString inputFile = UnitTest::PartitionGeometryTest::ExemplaryVertexGeomIdsPath;
    IntVec3Type numOfPartitionsPerAxis = {4, 4, 4};
    FloatVec3Type partitioningSchemeOrigin = {-1, -1, -0.1};
    FloatVec3Type lengthPerPartition = {0.25, 0.25, 0.25};
    DataArrayPath arrayPath = {"DataContainer", "VertexData", "PartitionIds"};
    QString exemplaryArrayName = "ExemplaryPartitionIds";

    TestGeometry(inputFile, arrayPath, numOfPartitionsPerAxis, partitioningSchemeOrigin, lengthPerPartition, exemplaryArrayName, PartitionGeometry::PartitioningMode::Advanced);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestQuadGeometry()
  {
    QString inputFile = UnitTest::PartitionGeometryTest::ExemplaryQuadGeomIdsPath;
    IntVec3Type numOfPartitionsPerAxis = {10, 10, 10};
    FloatVec3Type partitioningSchemeOrigin = {-1, -1, -0.05};
    FloatVec3Type lengthPerPartition = {0.2, 0.2, 0.1};
    DataArrayPath arrayPath = {"DataContainer", "VertexData", "PartitionIds"};
    QString exemplaryArrayName = "ExemplaryPartitionIds";

    TestGeometry(inputFile, arrayPath, numOfPartitionsPerAxis, partitioningSchemeOrigin, lengthPerPartition, exemplaryArrayName, PartitionGeometry::PartitioningMode::Advanced);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestTetrahedralGeometry()
  {
    QString inputFile = UnitTest::PartitionGeometryTest::ExemplaryTetrahedralGeomIdsPath;
    IntVec3Type numOfPartitionsPerAxis = {4, 4, 4};
    FloatVec3Type partitioningSchemeOrigin = {-1, -1, -0.05};
    FloatVec3Type lengthPerPartition = {0.5, 0.5, 0.25};
    DataArrayPath arrayPath = {"DataContainer", "VertexData", "PartitionIds"};
    QString exemplaryArrayName = "ExemplaryPartitionIds";

    TestGeometry(inputFile, arrayPath, numOfPartitionsPerAxis, partitioningSchemeOrigin, lengthPerPartition, exemplaryArrayName, PartitionGeometry::PartitioningMode::Advanced);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestHexahedralGeometry()
  {
    QString inputFile = UnitTest::PartitionGeometryTest::ExemplaryHexahedralGeomIdsPath;
    IntVec3Type numOfPartitionsPerAxis = {4, 4, 4};
    FloatVec3Type partitioningSchemeOrigin = {1, 1, 1};
    FloatVec3Type lengthPerPartition = {0.75, 0.75, 0.75};
    DataArrayPath arrayPath = {"DataContainer", "VertexData", "PartitionIds"};
    QString exemplaryArrayName = "ExemplaryPartitionIds";

    TestGeometry(inputFile, arrayPath, numOfPartitionsPerAxis, partitioningSchemeOrigin, lengthPerPartition, exemplaryArrayName, PartitionGeometry::PartitioningMode::Advanced);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void operator()()
  {
    int err = EXIT_SUCCESS;
    Q_UNUSED(err)

    DREAM3D_REGISTER_TEST(TestImageGeometry())
    DREAM3D_REGISTER_TEST(TestRectGridGeometry())
    DREAM3D_REGISTER_TEST(TestTriangleGeometry())
    DREAM3D_REGISTER_TEST(TestEdgeGeometry())
    DREAM3D_REGISTER_TEST(TestVertexGeometry())
    DREAM3D_REGISTER_TEST(TestQuadGeometry())
    DREAM3D_REGISTER_TEST(TestTetrahedralGeometry())
    DREAM3D_REGISTER_TEST(TestHexahedralGeometry())
  }
};
