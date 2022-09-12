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
#include "SIMPLib/CoreFilters/DataContainerWriter.h"
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
  void TestGeometry(PartitionGeometry::Pointer filter, const QString& inputFile, const DataArrayPath& arrayPath, const QString& exemplaryArrayName, bool dumpOutput = false)
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

    filter->setDataContainerArray(dca);
    filter->execute();
    int err = filter->getErrorCode();
    DREAM3D_REQUIRE(err >= 0)

    if(dumpOutput)
    {
      DataContainerWriter::Pointer filter = DataContainerWriter::New();
      filter->setDataContainerArray(dca);
      filter->setOutputFile("/tmp/test.dream3d");
      filter->setWriteXdmfFile(true);
      filter->execute();
      int err = filter->getErrorCode();
      DREAM3D_REQUIRE(err >= 0)
    }

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
      if(partitionId != exemplaryId)
      {
        int a = 0;
      }
      DREAM3D_REQUIRE_EQUAL(partitionId, exemplaryId)
    }
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestBasicGeometry(const QString& inputFile, const DataArrayPath& arrayPath, const IntVec3Type& numOfPartitionsPerAxis, const QString& exemplaryArrayName, bool dumpOutput = false)
  {
    PartitionGeometry::Pointer filter = PartitionGeometry::New();
    filter->setPartitioningMode(static_cast<int>(PartitionGeometry::PartitioningMode::Basic));
    filter->setNumberOfPartitionsPerAxis(numOfPartitionsPerAxis);
    filter->setAttributeMatrixPath(arrayPath);
    filter->setPartitionIdsArrayName(arrayPath.getDataArrayName());

    TestGeometry(filter, inputFile, arrayPath, exemplaryArrayName, dumpOutput);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestAdvancedGeometry(const QString& inputFile, const DataArrayPath& arrayPath, const IntVec3Type& numOfPartitionsPerAxis, const FloatVec3Type& partitioningSchemeOrigin,
                            const FloatVec3Type& lengthPerPartition, const QString& exemplaryArrayName, bool dumpOutput = false)
  {
    PartitionGeometry::Pointer filter = PartitionGeometry::New();
    filter->setPartitioningMode(static_cast<int>(PartitionGeometry::PartitioningMode::Advanced));
    filter->setNumberOfPartitionsPerAxis(numOfPartitionsPerAxis);
    filter->setPartitioningSchemeOrigin(partitioningSchemeOrigin);
    filter->setLengthPerPartition(lengthPerPartition);
    filter->setAttributeMatrixPath(arrayPath);
    filter->setPartitionIdsArrayName(arrayPath.getDataArrayName());

    TestGeometry(filter, inputFile, arrayPath, exemplaryArrayName, dumpOutput);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestBoundingBoxGeometry(const QString& inputFile, const DataArrayPath& arrayPath, const IntVec3Type& numOfPartitionsPerAxis, const FloatVec3Type& lowerLeftCoord,
                               const FloatVec3Type& upperRightCoord, const QString& exemplaryArrayName, bool dumpOutput = false)
  {
    PartitionGeometry::Pointer filter = PartitionGeometry::New();
    filter->setPartitioningMode(static_cast<int>(PartitionGeometry::PartitioningMode::BoundingBox));
    filter->setNumberOfPartitionsPerAxis(numOfPartitionsPerAxis);
    filter->setLowerLeftCoord(lowerLeftCoord);
    filter->setUpperRightCoord(upperRightCoord);
    filter->setAttributeMatrixPath(arrayPath);
    filter->setPartitionIdsArrayName(arrayPath.getDataArrayName());

    TestGeometry(filter, inputFile, arrayPath, exemplaryArrayName, dumpOutput);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestBasicImageGeometry()
  {
    QString inputFile = UnitTest::PartitionGeometryTest::ExemplaryImageGeomIdsPath;
    IntVec3Type numOfPartitionsPerAxis = {5, 5, 5};
    DataArrayPath arrayPath = {"DataContainer", "CellData", "PartitionIds"};
    QString exemplaryArrayName = "ExemplaryPartitionIds";

    TestBasicGeometry(inputFile, arrayPath, numOfPartitionsPerAxis, exemplaryArrayName);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestAdvancedImageGeometry()
  {
    QString inputFile = UnitTest::PartitionGeometryTest::ExemplaryImageGeomIdsPath;
    IntVec3Type numOfPartitionsPerAxis = {5, 5, 5};
    FloatVec3Type partitioningSchemeOrigin = {-10, 5, 2};
    FloatVec3Type lengthPerPartition = {5, 5, 5};
    DataArrayPath arrayPath = {"DataContainer", "CellData", "PartitionIds"};
    QString exemplaryArrayName = "ExemplaryPartitionIds";

    TestAdvancedGeometry(inputFile, arrayPath, numOfPartitionsPerAxis, partitioningSchemeOrigin, lengthPerPartition, exemplaryArrayName);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestBoundingBoxImageGeometry()
  {
    QString inputFile = UnitTest::PartitionGeometryTest::ExemplaryImageGeomIdsPath;
    IntVec3Type numOfPartitionsPerAxis = {5, 5, 5};
    FloatVec3Type lowerLeftCoord = {-10, 5, 2};
    FloatVec3Type upperRightCoord = {15, 30, 27};
    DataArrayPath arrayPath = {"DataContainer", "CellData", "PartitionIds"};
    QString exemplaryArrayName = "ExemplaryPartitionIds";

    TestBoundingBoxGeometry(inputFile, arrayPath, numOfPartitionsPerAxis, lowerLeftCoord, upperRightCoord, exemplaryArrayName);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestBasicRectGridGeometry()
  {
    QString inputFile = UnitTest::PartitionGeometryTest::ExemplaryRectGridGeomIdsPath;
    IntVec3Type numOfPartitionsPerAxis = {5, 5, 5};
    DataArrayPath arrayPath = {"DataContainer", "CellData", "PartitionIds"};
    QString exemplaryArrayName = "ExemplaryPartitionIds";

    TestBasicGeometry(inputFile, arrayPath, numOfPartitionsPerAxis, exemplaryArrayName);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestAdvancedRectGridGeometry()
  {
    QString inputFile = UnitTest::PartitionGeometryTest::ExemplaryRectGridGeomIdsPath;
    IntVec3Type numOfPartitionsPerAxis = {5, 5, 5};
    FloatVec3Type partitioningSchemeOrigin = {0, 0, 0};
    FloatVec3Type lengthPerPartition = {6, 6, 6};
    DataArrayPath arrayPath = {"DataContainer", "CellData", "PartitionIds"};
    QString exemplaryArrayName = "ExemplaryPartitionIds";

    TestAdvancedGeometry(inputFile, arrayPath, numOfPartitionsPerAxis, partitioningSchemeOrigin, lengthPerPartition, exemplaryArrayName);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestBoundingBoxRectGridGeometry()
  {
    QString inputFile = UnitTest::PartitionGeometryTest::ExemplaryRectGridGeomIdsPath;
    IntVec3Type numOfPartitionsPerAxis = {5, 5, 5};
    FloatVec3Type lowerLeftCoord = {0, 0, 0};
    FloatVec3Type upperRightCoord = {30, 30, 30};
    DataArrayPath arrayPath = {"DataContainer", "CellData", "PartitionIds"};
    QString exemplaryArrayName = "ExemplaryPartitionIds";

    TestBoundingBoxGeometry(inputFile, arrayPath, numOfPartitionsPerAxis, lowerLeftCoord, upperRightCoord, exemplaryArrayName);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestBasicTriangleGeometry()
  {
    QString inputFile = UnitTest::PartitionGeometryTest::ExemplaryTriangleGeomIdsPath;
    IntVec3Type numOfPartitionsPerAxis = {5, 4, 4};
    DataArrayPath arrayPath = {"DataContainer", "VertexData", "PartitioningSchemeIds"};
    QString exemplaryArrayName = "ExemplaryPartitioningSchemeIds";

    TestBasicGeometry(inputFile, arrayPath, numOfPartitionsPerAxis, exemplaryArrayName);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestAdvancedTriangleGeometry()
  {
    QString inputFile = UnitTest::PartitionGeometryTest::ExemplaryTriangleGeomIdsPath;
    IntVec3Type numOfPartitionsPerAxis = {5, 4, 4};
    FloatVec3Type partitioningSchemeOrigin = {-0.997462, -0.997462, -0.00001};
    FloatVec3Type lengthPerPartition = {0.398984, 0.49873, 0.247939};
    DataArrayPath arrayPath = {"DataContainer", "VertexData", "PartitioningSchemeIds"};
    QString exemplaryArrayName = "ExemplaryPartitioningSchemeIds";

    TestAdvancedGeometry(inputFile, arrayPath, numOfPartitionsPerAxis, partitioningSchemeOrigin, lengthPerPartition, exemplaryArrayName, true);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestBoundingBoxTriangleGeometry()
  {
    QString inputFile = UnitTest::PartitionGeometryTest::ExemplaryTriangleGeomIdsPath;
    IntVec3Type numOfPartitionsPerAxis = {5, 4, 4};
    FloatVec3Type lowerLeftCoord = {-0.997462, -0.997462, -0.00001};
    FloatVec3Type upperRightCoord = {0.997463, 0.997462, 0.991746};
    DataArrayPath arrayPath = {"DataContainer", "VertexData", "PartitioningSchemeIds"};
    QString exemplaryArrayName = "ExemplaryPartitioningSchemeIds";

    TestBoundingBoxGeometry(inputFile, arrayPath, numOfPartitionsPerAxis, lowerLeftCoord, upperRightCoord, exemplaryArrayName);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestBasicEdgeGeometry()
  {
    QString inputFile = UnitTest::PartitionGeometryTest::ExemplaryEdgeGeomIdsPath;
    IntVec3Type numOfPartitionsPerAxis = {4, 4, 4};
    DataArrayPath arrayPath = {"DataContainer", "VertexData", "PartitioningSchemeIds"};
    QString exemplaryArrayName = "ExemplaryPartitioningSchemeIds";

    TestBasicGeometry(inputFile, arrayPath, numOfPartitionsPerAxis, exemplaryArrayName);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestAdvancedEdgeGeometry()
  {
    QString inputFile = UnitTest::PartitionGeometryTest::ExemplaryEdgeGeomIdsPath;
    IntVec3Type numOfPartitionsPerAxis = {4, 4, 4};
    FloatVec3Type partitioningSchemeOrigin = {-0.997462, -0.997462, -0.00001};
    FloatVec3Type lengthPerPartition = {0.49873, 0.49873, 0.247939};
    DataArrayPath arrayPath = {"DataContainer", "VertexData", "PartitioningSchemeIds"};
    QString exemplaryArrayName = "ExemplaryPartitioningSchemeIds";

    TestAdvancedGeometry(inputFile, arrayPath, numOfPartitionsPerAxis, partitioningSchemeOrigin, lengthPerPartition, exemplaryArrayName, true);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestBoundingBoxEdgeGeometry()
  {
    QString inputFile = UnitTest::PartitionGeometryTest::ExemplaryEdgeGeomIdsPath;
    IntVec3Type numOfPartitionsPerAxis = {4, 4, 4};
    FloatVec3Type lowerLeftCoord = {-0.997462, -0.997462, -0.00001};
    FloatVec3Type upperRightCoord = {0.997462, 0.997462, 0.991746};
    DataArrayPath arrayPath = {"DataContainer", "VertexData", "PartitioningSchemeIds"};
    QString exemplaryArrayName = "ExemplaryPartitioningSchemeIds";

    TestBoundingBoxGeometry(inputFile, arrayPath, numOfPartitionsPerAxis, lowerLeftCoord, upperRightCoord, exemplaryArrayName);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestBasicVertexGeometry()
  {
    QString inputFile = UnitTest::PartitionGeometryTest::ExemplaryVertexGeomIdsPath;
    IntVec3Type numOfPartitionsPerAxis = {20, 10, 5};
    DataArrayPath arrayPath = {"DataContainer", "VertexData", "PartitioningSchemeIds"};
    QString exemplaryArrayName = "ExemplaryPartitioningSchemeIds";

    TestBasicGeometry(inputFile, arrayPath, numOfPartitionsPerAxis, exemplaryArrayName);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestAdvancedVertexGeometry()
  {
    QString inputFile = UnitTest::PartitionGeometryTest::ExemplaryVertexGeomIdsPath;
    IntVec3Type numOfPartitionsPerAxis = {20, 10, 5};
    FloatVec3Type partitioningSchemeOrigin = {-0.997462, -0.997462, -0.00001};
    FloatVec3Type lengthPerPartition = {0.099746, 0.199492, 0.198351};
    DataArrayPath arrayPath = {"DataContainer", "VertexData", "PartitioningSchemeIds"};
    QString exemplaryArrayName = "ExemplaryPartitioningSchemeIds";

    TestAdvancedGeometry(inputFile, arrayPath, numOfPartitionsPerAxis, partitioningSchemeOrigin, lengthPerPartition, exemplaryArrayName, true);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestBoundingBoxVertexGeometry()
  {
    QString inputFile = UnitTest::PartitionGeometryTest::ExemplaryVertexGeomIdsPath;
    IntVec3Type numOfPartitionsPerAxis = {20, 10, 5};
    FloatVec3Type lowerLeftCoord = {-0.997462, -0.997462, -0.00001};
    FloatVec3Type upperRightCoord = {0.997462, 0.997458, 0.991745};
    DataArrayPath arrayPath = {"DataContainer", "VertexData", "PartitioningSchemeIds"};
    QString exemplaryArrayName = "ExemplaryPartitioningSchemeIds";

    TestBoundingBoxGeometry(inputFile, arrayPath, numOfPartitionsPerAxis, lowerLeftCoord, upperRightCoord, exemplaryArrayName);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestBasicQuadGeometry()
  {
    QString inputFile = UnitTest::PartitionGeometryTest::ExemplaryQuadGeomIdsPath;
    IntVec3Type numOfPartitionsPerAxis = {10, 5, 3};
    DataArrayPath arrayPath = {"DataContainer", "VertexData", "PartitioningSchemeIds"};
    QString exemplaryArrayName = "ExemplaryPartitioningSchemeIds";

    TestBasicGeometry(inputFile, arrayPath, numOfPartitionsPerAxis, exemplaryArrayName);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestAdvancedQuadGeometry()
  {
    QString inputFile = UnitTest::PartitionGeometryTest::ExemplaryQuadGeomIdsPath;
    IntVec3Type numOfPartitionsPerAxis = {10, 5, 3};
    FloatVec3Type partitioningSchemeOrigin = {-0.997462, -0.997462, -0.00001};
    FloatVec3Type lengthPerPartition = {0.199492, 0.398984, 0.330585333333333};
    DataArrayPath arrayPath = {"DataContainer", "VertexData", "PartitioningSchemeIds"};
    QString exemplaryArrayName = "ExemplaryPartitioningSchemeIds";

    TestAdvancedGeometry(inputFile, arrayPath, numOfPartitionsPerAxis, partitioningSchemeOrigin, lengthPerPartition, exemplaryArrayName, true);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestBoundingBoxQuadGeometry()
  {
    QString inputFile = UnitTest::PartitionGeometryTest::ExemplaryQuadGeomIdsPath;
    IntVec3Type numOfPartitionsPerAxis = {10, 5, 3};
    FloatVec3Type lowerLeftCoord = {-0.997462, -0.997462, -0.00001};
    FloatVec3Type upperRightCoord = {0.997462, 0.997462, 0.991746};
    DataArrayPath arrayPath = {"DataContainer", "VertexData", "PartitioningSchemeIds"};
    QString exemplaryArrayName = "ExemplaryPartitioningSchemeIds";

    TestBoundingBoxGeometry(inputFile, arrayPath, numOfPartitionsPerAxis, lowerLeftCoord, upperRightCoord, exemplaryArrayName);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestBasicTetrahedralGeometry()
  {
    QString inputFile = UnitTest::PartitionGeometryTest::ExemplaryTetrahedralGeomIdsPath;
    IntVec3Type numOfPartitionsPerAxis = {100, 45, 8};
    DataArrayPath arrayPath = {"DataContainer", "VertexData", "PartitioningSchemeIds"};
    QString exemplaryArrayName = "ExemplaryPartitioningSchemeIds";

    TestBasicGeometry(inputFile, arrayPath, numOfPartitionsPerAxis, exemplaryArrayName);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestAdvancedTetrahedralGeometry()
  {
    QString inputFile = UnitTest::PartitionGeometryTest::ExemplaryTetrahedralGeomIdsPath;
    IntVec3Type numOfPartitionsPerAxis = {100, 45, 8};
    FloatVec3Type partitioningSchemeOrigin = {-0.997462, -0.997462, -0.00001};
    FloatVec3Type lengthPerPartition = {0.0199492, 0.044331555555556, 0.12397};
    DataArrayPath arrayPath = {"DataContainer", "VertexData", "PartitioningSchemeIds"};
    QString exemplaryArrayName = "ExemplaryPartitioningSchemeIds";

    TestAdvancedGeometry(inputFile, arrayPath, numOfPartitionsPerAxis, partitioningSchemeOrigin, lengthPerPartition, exemplaryArrayName, true);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestBoundingBoxTetrahedralGeometry()
  {
    QString inputFile = UnitTest::PartitionGeometryTest::ExemplaryTetrahedralGeomIdsPath;
    IntVec3Type numOfPartitionsPerAxis = {100, 45, 8};
    FloatVec3Type lowerLeftCoord = {-0.997462, -0.997462, -0.00001};
    FloatVec3Type upperRightCoord = {0.997458, 0.99746, 0.99175};
    DataArrayPath arrayPath = {"DataContainer", "VertexData", "PartitioningSchemeIds"};
    QString exemplaryArrayName = "ExemplaryPartitioningSchemeIds";

    TestBoundingBoxGeometry(inputFile, arrayPath, numOfPartitionsPerAxis, lowerLeftCoord, upperRightCoord, exemplaryArrayName);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestBasicHexahedralGeometry()
  {
    QString inputFile = UnitTest::PartitionGeometryTest::ExemplaryHexahedralGeomIdsPath;
    IntVec3Type numOfPartitionsPerAxis = {6, 7, 8};
    DataArrayPath arrayPath = {"DataContainer", "VertexData", "PartitioningSchemeIds"};
    QString exemplaryArrayName = "ExemplaryPartitioningSchemeIds";

    TestBasicGeometry(inputFile, arrayPath, numOfPartitionsPerAxis, exemplaryArrayName);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestAdvancedHexahedralGeometry()
  {
    QString inputFile = UnitTest::PartitionGeometryTest::ExemplaryHexahedralGeomIdsPath;
    IntVec3Type numOfPartitionsPerAxis = {6, 7, 8};
    FloatVec3Type partitioningSchemeOrigin = {0.99999, 0.99999, 1.54999};
    FloatVec3Type lengthPerPartition = {1.1055, 0.285717142857143, 0.2500025};
    DataArrayPath arrayPath = {"DataContainer", "VertexData", "PartitioningSchemeIds"};
    QString exemplaryArrayName = "ExemplaryPartitioningSchemeIds";

    TestAdvancedGeometry(inputFile, arrayPath, numOfPartitionsPerAxis, partitioningSchemeOrigin, lengthPerPartition, exemplaryArrayName, true);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestBoundingBoxHexahedralGeometry()
  {
    QString inputFile = UnitTest::PartitionGeometryTest::ExemplaryHexahedralGeomIdsPath;
    IntVec3Type numOfPartitionsPerAxis = {6, 7, 8};
    FloatVec3Type lowerLeftCoord = {0.99999, 0.99999, 1.54999};
    FloatVec3Type upperRightCoord = {7.631, 3.00001, 3.55001};
    DataArrayPath arrayPath = {"DataContainer", "VertexData", "PartitioningSchemeIds"};
    QString exemplaryArrayName = "ExemplaryPartitioningSchemeIds";

    TestBoundingBoxGeometry(inputFile, arrayPath, numOfPartitionsPerAxis, lowerLeftCoord, upperRightCoord, exemplaryArrayName);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void operator()()
  {
    int err = EXIT_SUCCESS;
    Q_UNUSED(err)

    DREAM3D_REGISTER_TEST(TestBasicImageGeometry())
    DREAM3D_REGISTER_TEST(TestAdvancedImageGeometry())
    DREAM3D_REGISTER_TEST(TestBoundingBoxImageGeometry())

    DREAM3D_REGISTER_TEST(TestBasicRectGridGeometry())
    DREAM3D_REGISTER_TEST(TestAdvancedRectGridGeometry())
    DREAM3D_REGISTER_TEST(TestBoundingBoxRectGridGeometry())

    DREAM3D_REGISTER_TEST(TestBasicTriangleGeometry())
    DREAM3D_REGISTER_TEST(TestAdvancedTriangleGeometry())
    DREAM3D_REGISTER_TEST(TestBoundingBoxTriangleGeometry())

    DREAM3D_REGISTER_TEST(TestBasicEdgeGeometry())
    DREAM3D_REGISTER_TEST(TestAdvancedEdgeGeometry())
    DREAM3D_REGISTER_TEST(TestBoundingBoxEdgeGeometry())

    DREAM3D_REGISTER_TEST(TestBasicVertexGeometry())
    DREAM3D_REGISTER_TEST(TestAdvancedVertexGeometry())
    DREAM3D_REGISTER_TEST(TestBoundingBoxVertexGeometry())

    DREAM3D_REGISTER_TEST(TestBasicQuadGeometry())
    DREAM3D_REGISTER_TEST(TestAdvancedQuadGeometry())
    DREAM3D_REGISTER_TEST(TestBoundingBoxQuadGeometry())

    DREAM3D_REGISTER_TEST(TestBasicTetrahedralGeometry())
    DREAM3D_REGISTER_TEST(TestAdvancedTetrahedralGeometry())
    DREAM3D_REGISTER_TEST(TestBoundingBoxTetrahedralGeometry())

    DREAM3D_REGISTER_TEST(TestBasicHexahedralGeometry())
    DREAM3D_REGISTER_TEST(TestAdvancedHexahedralGeometry())
    DREAM3D_REGISTER_TEST(TestBoundingBoxHexahedralGeometry())
  }
};
