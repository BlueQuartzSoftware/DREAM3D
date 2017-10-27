/* ============================================================================
* Copyright (c) 2009-2016 BlueQuartz Software, LLC
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
* The code contained herein was partially funded by the followig contracts:
*    United States Air Force Prime Contract FA8650-07-D-5800
*    United States Air Force Prime Contract FA8650-10-D-5210
*    United States Prime Contract Navy N00173-07-C-2068
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include <QtCore/QCoreApplication>
#include <QtCore/QFile>

#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/DataArrays/NeighborList.hpp"
#include "SIMPLib/Filtering/FilterFactory.hpp"
#include "SIMPLib/Filtering/FilterManager.h"
#include "SIMPLib/Filtering/FilterPipeline.h"
#include "SIMPLib/Filtering/QMetaObjectUtilities.h"
#include "SIMPLib/Geometry/TriangleGeom.h"
#include "SIMPLib/Plugin/ISIMPLibPlugin.h"
#include "SIMPLib/Plugin/SIMPLibPluginLoader.h"
#include "SIMPLib/SIMPLib.h"
#include "UnitTestSupport.hpp"

#include "SurfaceMeshingTestFileLocations.h"

class FindTriangleGeomNeighborsTest
{

public:
  FindTriangleGeomNeighborsTest()
  {
  }
  virtual ~FindTriangleGeomNeighborsTest()
  {
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void RemoveTestFiles()
  {
#if REMOVE_TEST_FILES
    QFile::remove(UnitTest::FindTriangleGeomNeighborsTest::TestFile1);
    QFile::remove(UnitTest::FindTriangleGeomNeighborsTest::TestFile2);
#endif
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  int TestFilterAvailability()
  {
    // Now instantiate the FindTriangleGeomNeighborsTest Filter from the FilterManager
    QString filtName = "FindTriangleGeomNeighbors";
    FilterManager* fm = FilterManager::Instance();
    IFilterFactory::Pointer filterFactory = fm->getFactoryForFilter(filtName);
    if(nullptr == filterFactory.get())
    {
      std::stringstream ss;
      ss << "The SurfaceMeshing Requires the use of the " << filtName.toStdString() << " filter which is found in the SurfaceMeshing Plugin";
      DREAM3D_TEST_THROW_EXCEPTION(ss.str())
    }
    return 0;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  int TestFindTriangleGeomNeighborsTest()
  {
    DataContainerArray::Pointer dca = DataContainerArray::New();

    DataContainer::Pointer tdc = DataContainer::New(SIMPL::Defaults::TriangleDataContainerName);
    dca->addDataContainer(tdc);

    // Basic idea is to create a surface mesh of a rectangular prism with edge lengths of 3x1x1, with
    // three total features (one voxel each)
    SharedVertexList::Pointer vertex = TriangleGeom::CreateSharedVertexList(16);
    TriangleGeom::Pointer triangle = TriangleGeom::CreateGeometry(32, vertex, SIMPL::Geometry::TriangleGeometry);
    tdc->setGeometry(triangle);
    float* vertices = triangle->getVertexPointer(0);
    int64_t* tris = triangle->getTriPointer(0);

    // Define 16 vertex coordinates
    vertices[3 * 0 + 0] = -1.0f;
    vertices[3 * 0 + 1] = 0.0f;
    vertices[3 * 0 + 2] = 0.0f;

    vertices[3 * 1 + 0] = -1.0f;
    vertices[3 * 1 + 1] = 0.0f;
    vertices[3 * 1 + 2] = 1.0f;

    vertices[3 * 2 + 0] = -1.0f;
    vertices[3 * 2 + 1] = 1.0f;
    vertices[3 * 2 + 2] = 1.0f;

    vertices[3 * 3 + 0] = -1.0f;
    vertices[3 * 3 + 1] = 1.0f;
    vertices[3 * 3 + 2] = 0.0f;

    vertices[3 * 4 + 0] = 2.0f;
    vertices[3 * 4 + 1] = 0.0f;
    vertices[3 * 4 + 2] = 0.0f;

    vertices[3 * 5 + 0] = 2.0f;
    vertices[3 * 5 + 1] = 0.0f;
    vertices[3 * 5 + 2] = 1.0f;

    vertices[3 * 6 + 0] = 2.0f;
    vertices[3 * 6 + 1] = 1.0f;
    vertices[3 * 6 + 2] = 1.0f;

    vertices[3 * 7 + 0] = 2.0f;
    vertices[3 * 7 + 1] = 1.0f;
    vertices[3 * 7 + 2] = 0.0f;

    vertices[3 * 8 + 0] = 0.0f;
    vertices[3 * 8 + 1] = 0.0f;
    vertices[3 * 8 + 2] = 0.0f;

    vertices[3 * 9 + 0] = 0.0f;
    vertices[3 * 9 + 1] = 0.0f;
    vertices[3 * 9 + 2] = 1.0f;

    vertices[3 * 10 + 0] = 0.0f;
    vertices[3 * 10 + 1] = 1.0f;
    vertices[3 * 10 + 2] = 1.0f;

    vertices[3 * 11 + 0] = 0.0f;
    vertices[3 * 11 + 1] = 1.0f;
    vertices[3 * 11 + 2] = 0.0f;

    vertices[3 * 12 + 0] = 1.0f;
    vertices[3 * 12 + 1] = 0.0f;
    vertices[3 * 12 + 2] = 0.0f;

    vertices[3 * 13 + 0] = 1.0f;
    vertices[3 * 13 + 1] = 0.0f;
    vertices[3 * 13 + 2] = 1.0f;

    vertices[3 * 14 + 0] = 1.0f;
    vertices[3 * 14 + 1] = 1.0f;
    vertices[3 * 14 + 2] = 1.0f;

    vertices[3 * 15 + 0] = 1.0f;
    vertices[3 * 15 + 1] = 1.0f;
    vertices[3 * 15 + 2] = 0.0f;

    // Define 32 triangles
    tris[3 * 0 + 0] = 0;
    tris[3 * 0 + 1] = 1;
    tris[3 * 0 + 2] = 3;

    tris[3 * 1 + 0] = 1;
    tris[3 * 1 + 1] = 2;
    tris[3 * 1 + 2] = 3;

    tris[3 * 2 + 0] = 0;
    tris[3 * 2 + 1] = 9;
    tris[3 * 2 + 2] = 1;

    tris[3 * 3 + 0] = 0;
    tris[3 * 3 + 1] = 8;
    tris[3 * 3 + 2] = 9;

    tris[3 * 4 + 0] = 8;
    tris[3 * 4 + 1] = 13;
    tris[3 * 4 + 2] = 9;

    tris[3 * 5 + 0] = 8;
    tris[3 * 5 + 1] = 12;
    tris[3 * 5 + 2] = 13;

    tris[3 * 6 + 0] = 12;
    tris[3 * 6 + 1] = 5;
    tris[3 * 6 + 2] = 13;

    tris[3 * 7 + 0] = 12;
    tris[3 * 7 + 1] = 4;
    tris[3 * 7 + 2] = 5;

    tris[3 * 8 + 0] = 4;
    tris[3 * 8 + 1] = 6;
    tris[3 * 8 + 2] = 5;

    tris[3 * 9 + 0] = 4;
    tris[3 * 9 + 1] = 7;
    tris[3 * 9 + 2] = 6;

    tris[3 * 10 + 0] = 7;
    tris[3 * 10 + 1] = 14;
    tris[3 * 10 + 2] = 6;

    tris[3 * 11 + 0] = 7;
    tris[3 * 11 + 1] = 15;
    tris[3 * 11 + 2] = 14;

    tris[3 * 12 + 0] = 15;
    tris[3 * 12 + 1] = 10;
    tris[3 * 12 + 2] = 14;

    tris[3 * 13 + 0] = 15;
    tris[3 * 13 + 1] = 11;
    tris[3 * 13 + 2] = 10;

    tris[3 * 14 + 0] = 11;
    tris[3 * 14 + 1] = 2;
    tris[3 * 14 + 2] = 10;

    tris[3 * 15 + 0] = 11;
    tris[3 * 15 + 1] = 3;
    tris[3 * 15 + 2] = 2;

    tris[3 * 16 + 0] = 1;
    tris[3 * 16 + 1] = 9;
    tris[3 * 16 + 2] = 10;

    tris[3 * 17 + 0] = 1;
    tris[3 * 17 + 1] = 10;
    tris[3 * 17 + 2] = 2;

    tris[3 * 18 + 0] = 9;
    tris[3 * 18 + 1] = 13;
    tris[3 * 18 + 2] = 14;

    tris[3 * 19 + 0] = 9;
    tris[3 * 19 + 1] = 14;
    tris[3 * 19 + 2] = 10;

    tris[3 * 20 + 0] = 13;
    tris[3 * 20 + 1] = 5;
    tris[3 * 20 + 2] = 6;

    tris[3 * 21 + 0] = 13;
    tris[3 * 21 + 1] = 6;
    tris[3 * 21 + 2] = 14;

    tris[3 * 22 + 0] = 0;
    tris[3 * 22 + 1] = 3;
    tris[3 * 22 + 2] = 11;

    tris[3 * 23 + 0] = 0;
    tris[3 * 23 + 1] = 11;
    tris[3 * 23 + 2] = 8;

    tris[3 * 24 + 0] = 8;
    tris[3 * 24 + 1] = 11;
    tris[3 * 24 + 2] = 15;

    tris[3 * 25 + 0] = 8;
    tris[3 * 25 + 1] = 15;
    tris[3 * 25 + 2] = 12;

    tris[3 * 26 + 0] = 12;
    tris[3 * 26 + 1] = 15;
    tris[3 * 26 + 2] = 7;

    tris[3 * 27 + 0] = 12;
    tris[3 * 27 + 1] = 7;
    tris[3 * 27 + 2] = 4;

    tris[3 * 28 + 0] = 8;
    tris[3 * 28 + 1] = 9;
    tris[3 * 28 + 2] = 11;

    tris[3 * 29 + 0] = 9;
    tris[3 * 29 + 1] = 10;
    tris[3 * 29 + 2] = 11;

    tris[3 * 30 + 0] = 12;
    tris[3 * 30 + 1] = 13;
    tris[3 * 30 + 2] = 15;

    tris[3 * 31 + 0] = 13;
    tris[3 * 31 + 1] = 14;
    tris[3 * 31 + 2] = 15;

    QVector<size_t> tDims(1, 32);
    AttributeMatrix::Pointer faceAttrMat = AttributeMatrix::New(tDims, SIMPL::Defaults::FaceAttributeMatrixName, AttributeMatrix::Type::Face);
    tdc->addAttributeMatrix(SIMPL::Defaults::FaceAttributeMatrixName, faceAttrMat);
    tDims[0] = 4;
    AttributeMatrix::Pointer featAttrMat = AttributeMatrix::New(tDims, SIMPL::Defaults::FaceFeatureAttributeMatrixName, AttributeMatrix::Type::FaceFeature);
    tdc->addAttributeMatrix(SIMPL::Defaults::FaceFeatureAttributeMatrixName, featAttrMat);
    QVector<size_t> cDims(1, 2);
    Int32ArrayType::Pointer faceLabels = Int32ArrayType::CreateArray(32, cDims, SIMPL::FaceData::SurfaceMeshFaceLabels);
    faceAttrMat->addAttributeArray(SIMPL::FaceData::SurfaceMeshFaceLabels, faceLabels);
    int32_t* faceLabelsPtr = faceLabels->getPointer(0);

    faceLabelsPtr[2 * 0 + 0] = -1;
    faceLabelsPtr[2 * 0 + 1] = 1;

    faceLabelsPtr[2 * 1 + 0] = -1;
    faceLabelsPtr[2 * 1 + 1] = 1;

    faceLabelsPtr[2 * 2 + 0] = -1;
    faceLabelsPtr[2 * 2 + 1] = 1;

    faceLabelsPtr[2 * 3 + 0] = -1;
    faceLabelsPtr[2 * 3 + 1] = 1;

    faceLabelsPtr[2 * 4 + 0] = -1;
    faceLabelsPtr[2 * 4 + 1] = 2;

    faceLabelsPtr[2 * 5 + 0] = -1;
    faceLabelsPtr[2 * 5 + 1] = 2;

    faceLabelsPtr[2 * 6 + 0] = -1;
    faceLabelsPtr[2 * 6 + 1] = 3;

    faceLabelsPtr[2 * 7 + 0] = -1;
    faceLabelsPtr[2 * 7 + 1] = 3;

    faceLabelsPtr[2 * 8 + 0] = -1;
    faceLabelsPtr[2 * 8 + 1] = 3;

    faceLabelsPtr[2 * 9 + 0] = -1;
    faceLabelsPtr[2 * 9 + 1] = 3;

    faceLabelsPtr[2 * 10 + 0] = -1;
    faceLabelsPtr[2 * 10 + 1] = 3;

    faceLabelsPtr[2 * 11 + 0] = -1;
    faceLabelsPtr[2 * 11 + 1] = 3;

    faceLabelsPtr[2 * 12 + 0] = -1;
    faceLabelsPtr[2 * 12 + 1] = 2;

    faceLabelsPtr[2 * 13 + 0] = -1;
    faceLabelsPtr[2 * 13 + 1] = 2;

    faceLabelsPtr[2 * 14 + 0] = -1;
    faceLabelsPtr[2 * 14 + 1] = 1;

    faceLabelsPtr[2 * 15 + 0] = -1;
    faceLabelsPtr[2 * 15 + 1] = 1;

    faceLabelsPtr[2 * 16 + 0] = -1;
    faceLabelsPtr[2 * 16 + 1] = 1;

    faceLabelsPtr[2 * 17 + 0] = -1;
    faceLabelsPtr[2 * 17 + 1] = 1;

    faceLabelsPtr[2 * 18 + 0] = -1;
    faceLabelsPtr[2 * 18 + 1] = 2;

    faceLabelsPtr[2 * 19 + 0] = -1;
    faceLabelsPtr[2 * 19 + 1] = 2;

    faceLabelsPtr[2 * 20 + 0] = -1;
    faceLabelsPtr[2 * 20 + 1] = 3;

    faceLabelsPtr[2 * 21 + 0] = -1;
    faceLabelsPtr[2 * 21 + 1] = 3;

    faceLabelsPtr[2 * 22 + 0] = -1;
    faceLabelsPtr[2 * 22 + 1] = 1;

    faceLabelsPtr[2 * 23 + 0] = -1;
    faceLabelsPtr[2 * 23 + 1] = 1;

    faceLabelsPtr[2 * 24 + 0] = -1;
    faceLabelsPtr[2 * 24 + 1] = 2;

    faceLabelsPtr[2 * 25 + 0] = -1;
    faceLabelsPtr[2 * 25 + 1] = 2;

    faceLabelsPtr[2 * 26 + 0] = -1;
    faceLabelsPtr[2 * 26 + 1] = 3;

    faceLabelsPtr[2 * 27 + 0] = -1;
    faceLabelsPtr[2 * 27 + 1] = 3;

    faceLabelsPtr[2 * 28 + 0] = 1;
    faceLabelsPtr[2 * 28 + 1] = 2;

    faceLabelsPtr[2 * 29 + 0] = 1;
    faceLabelsPtr[2 * 29 + 1] = 2;

    faceLabelsPtr[2 * 30 + 0] = 2;
    faceLabelsPtr[2 * 30 + 1] = 3;

    faceLabelsPtr[2 * 31 + 0] = 2;
    faceLabelsPtr[2 * 31 + 1] = 3;

    QString filtName = "FindTriangleGeomNeighbors";
    FilterManager* fm = FilterManager::Instance();
    IFilterFactory::Pointer factory = fm->getFactoryForFilter(filtName);
    DREAM3D_REQUIRE(factory.get() != nullptr)

    AbstractFilter::Pointer neighborsFilter = factory->create();
    DREAM3D_REQUIRE(neighborsFilter.get() != nullptr)

    neighborsFilter->setDataContainerArray(dca);

    bool propWasSet = true;
    QVariant var;

    DataArrayPath path(SIMPL::Defaults::TriangleDataContainerName, SIMPL::Defaults::FaceAttributeMatrixName, SIMPL::FaceData::SurfaceMeshFaceLabels);
    var.setValue(path);
    propWasSet = neighborsFilter->setProperty("FaceLabelsArrayPath", var);
    if(!propWasSet)
    {
      qDebug() << "Unable to set property FaceLabelsArrayPath";
    }

    path.update(SIMPL::Defaults::TriangleDataContainerName, SIMPL::Defaults::FaceFeatureAttributeMatrixName, "");
    var.setValue(path);
    propWasSet = neighborsFilter->setProperty("FeatureAttributeMatrixPath", var);
    if(!propWasSet)
    {
      qDebug() << "Unable to set property FeatureAttributeMatrixName";
    }

    neighborsFilter->execute();
    int32_t err = neighborsFilter->getErrorCondition();
    DREAM3D_REQUIRE_EQUAL(err, 0);

    AttributeMatrix::Pointer faceFeatAttrMat = tdc->getAttributeMatrix(SIMPL::Defaults::FaceFeatureAttributeMatrixName);
    Int32ArrayType::Pointer numNeighbors = faceFeatAttrMat->getAttributeArrayAs<Int32ArrayType>(SIMPL::FeatureData::NumNeighbors);
    NeighborList<int32_t>::Pointer neighborList = faceFeatAttrMat->getAttributeArrayAs<NeighborList<int32_t>>(SIMPL::FeatureData::NeighborList);

    DREAM3D_REQUIRE_EQUAL(numNeighbors->getNumberOfTuples(), 4);
    DREAM3D_REQUIRE_EQUAL(numNeighbors->getValue(1), 1);
    DREAM3D_REQUIRE_EQUAL(numNeighbors->getValue(2), 2);
    DREAM3D_REQUIRE_EQUAL(numNeighbors->getValue(3), 1);

    std::vector<int32_t> neigh1 = neighborList->getListReference(1);
    std::vector<int32_t> neigh2 = neighborList->getListReference(2);
    std::vector<int32_t> neigh3 = neighborList->getListReference(3);

    DREAM3D_REQUIRE_EQUAL(neigh1.size(), 1);
    DREAM3D_REQUIRE_EQUAL(neigh2.size(), 2);
    DREAM3D_REQUIRE_EQUAL(neigh3.size(), 1);

    DREAM3D_REQUIRE_EQUAL(neigh1[0], 2);
    DREAM3D_REQUIRE_EQUAL(neigh2[0], 1);
    DREAM3D_REQUIRE_EQUAL(neigh2[1], 3);
    DREAM3D_REQUIRE_EQUAL(neigh3[0], 2);

    return EXIT_SUCCESS;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void operator()()
  {
    int err = EXIT_SUCCESS;

    DREAM3D_REGISTER_TEST(TestFilterAvailability());

    DREAM3D_REGISTER_TEST(TestFindTriangleGeomNeighborsTest())

    DREAM3D_REGISTER_TEST(RemoveTestFiles())
  }

private:
  FindTriangleGeomNeighborsTest(const FindTriangleGeomNeighborsTest&); // Copy Constructor Not Implemented
  void operator=(const FindTriangleGeomNeighborsTest&);                // Operator '=' Not Implemented
};
