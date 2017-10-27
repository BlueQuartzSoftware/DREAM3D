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
#include "SIMPLib/Common/TemplateHelpers.hpp"
#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/Filtering/FilterFactory.hpp"
#include "SIMPLib/Filtering/FilterManager.h"
#include "SIMPLib/Filtering/FilterPipeline.h"
#include "SIMPLib/Filtering/QMetaObjectUtilities.h"
#include "SIMPLib/Geometry/ImageGeom.h"
#include "SIMPLib/Geometry/RectGridGeom.h"
#include "SIMPLib/Geometry/TriangleGeom.h"
#include "SIMPLib/Plugin/ISIMPLibPlugin.h"
#include "SIMPLib/Plugin/SIMPLibPluginLoader.h"
#include "SIMPLib/SIMPLib.h"
#include "UnitTestSupport.hpp"

#include "SurfaceMeshingTestFileLocations.h"

#define SET_PROPERTIES_AND_CHECK(filter, featureIdsPath, surfMeshName, errVal)                                                                                                                         \
  var.setValue(featureIdsPath);                                                                                                                                                                        \
  propWasSet = filter->setProperty("FeatureIdsArrayPath", var);                                                                                                                                        \
  if(false == propWasSet)                                                                                                                                                                              \
  {                                                                                                                                                                                                    \
    qDebug() << "Unable to set property FeatureIdsArrayPath";                                                                                                                                          \
  }                                                                                                                                                                                                    \
  var.setValue(surfMeshName);                                                                                                                                                                          \
  propWasSet = filter->setProperty("SurfaceDataContainerName", var);                                                                                                                                   \
  if(false == propWasSet)                                                                                                                                                                              \
  {                                                                                                                                                                                                    \
    qDebug() << "Unable to set property SurfaceDataContainerName";                                                                                                                                     \
  }                                                                                                                                                                                                    \
  filter->execute();                                                                                                                                                                                   \
  err = filter->getErrorCondition();                                                                                                                                                                   \
  DREAM3D_REQUIRE_EQUAL(err, 0);

class QuickSurfaceMeshTest
{
public:
  QuickSurfaceMeshTest()
  {
  }
  virtual ~QuickSurfaceMeshTest()
  {
  }
  SIMPL_TYPE_MACRO(QuickSurfaceMeshTest)

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void RemoveTestFiles()
  {
#if REMOVE_TEST_FILES
    QFile::remove(UnitTest::QuickSurfaceMeshTest::TestFile1);
    QFile::remove(UnitTest::QuickSurfaceMeshTest::TestFile2);
#endif
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  int TestFilterAvailability()
  {
    // Now instantiate the QuickSurfaceMeshTest Filter from the FilterManager
    QString filtName = "QuickSurfaceMesh";
    FilterManager* fm = FilterManager::Instance();
    IFilterFactory::Pointer filterFactory = fm->getFactoryForFilter(filtName);
    if(nullptr == filterFactory.get())
    {
      std::stringstream ss;
      ss << "The QuickSurfaceMeshTest Requires the use of the " << filtName.toStdString() << " filter which is found in the SurfaceMeshing Plugin";
      DREAM3D_TEST_THROW_EXCEPTION(ss.str())
    }
    return 0;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  DataContainerArray::Pointer initializeDataContainerArray()
  {
    DataContainerArray::Pointer dca = DataContainerArray::New();

    // Create a DataContainer for each geometry
    DataContainer::Pointer image3D_DC = DataContainer::New("ImageGeom3D");
    dca->addDataContainer(image3D_DC);

    DataContainer::Pointer rectGrid_DC = DataContainer::New("RectGrid");
    dca->addDataContainer(rectGrid_DC);

    // Create an instance of each geometry for testing
    ImageGeom::Pointer image3D = ImageGeom::CreateGeometry(SIMPL::Geometry::ImageGeometry);
    image3D->setDimensions(2, 2, 1);
    image3D_DC->setGeometry(image3D);

    RectGridGeom::Pointer rectGrid = RectGridGeom::CreateGeometry(SIMPL::Geometry::RectGridGeometry);
    rectGrid->setDimensions(2, 2, 1);
    FloatArrayType::Pointer xBounds = FloatArrayType::CreateArray(3, SIMPL::Geometry::xBoundsList);
    xBounds->setValue(0, 0.0f);
    xBounds->setValue(1, 1.0f);
    xBounds->setValue(2, 2.0f);
    rectGrid->setXBounds(xBounds);
    FloatArrayType::Pointer yBounds = FloatArrayType::CreateArray(3, SIMPL::Geometry::yBoundsList);
    yBounds->setValue(0, 0.0f);
    yBounds->setValue(1, 1.0f);
    yBounds->setValue(2, 2.0f);
    rectGrid->setYBounds(yBounds);
    FloatArrayType::Pointer zBounds = FloatArrayType::CreateArray(2, SIMPL::Geometry::zBoundsList);
    zBounds->setValue(0, 0.0f);
    zBounds->setValue(1, 2.0f);
    rectGrid->setZBounds(zBounds);
    rectGrid_DC->setGeometry(rectGrid);

    // Create an element AttributeMatrix and FeatureIds array for each geometry
    QVector<size_t> tDims(1, 4);

    AttributeMatrix::Pointer image3D_AttrMat = AttributeMatrix::New(tDims, "Image3DData", AttributeMatrix::Type::Cell);
    Int32ArrayType::Pointer image3D_fIDs = Int32ArrayType::CreateArray(4, SIMPL::CellData::FeatureIds);
    image3D_fIDs->initializeWithValue(1);
    image3D_fIDs->setValue(2, 2);
    image3D_fIDs->setValue(3, 2);
    image3D_AttrMat->addAttributeArray(SIMPL::CellData::FeatureIds, image3D_fIDs);
    image3D_DC->addAttributeMatrix("Image3DData", image3D_AttrMat);

    AttributeMatrix::Pointer rectGrid_AttrMat = AttributeMatrix::New(tDims, "RectGridData", AttributeMatrix::Type::Cell);
    Int32ArrayType::Pointer rectGrid_fIDs = Int32ArrayType::CreateArray(4, SIMPL::CellData::FeatureIds);
    rectGrid_fIDs->initializeWithValue(1);
    rectGrid_fIDs->setValue(2, 2);
    rectGrid_fIDs->setValue(3, 2);
    rectGrid_AttrMat->addAttributeArray(SIMPL::CellData::FeatureIds, rectGrid_fIDs);
    rectGrid_DC->addAttributeMatrix("RectGridData", rectGrid_AttrMat);

    // Create a feature AttributeMatrix for each geometry, which all have 2 features
    tDims[0] = 3;

    AttributeMatrix::Pointer image3D_featureAttrMat = AttributeMatrix::New(tDims, "Image3DFeatureData", AttributeMatrix::Type::CellFeature);
    image3D_DC->addAttributeMatrix("Image3DFeatureData", image3D_featureAttrMat);

    AttributeMatrix::Pointer rectGrid_featureAttrMat = AttributeMatrix::New(tDims, "RectGridFeatureData", AttributeMatrix::Type::CellFeature);
    rectGrid_DC->addAttributeMatrix("RectGridFeatureData", rectGrid_featureAttrMat);

    return dca;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void validateQuickSurfaceMesh(AbstractFilter::Pointer filter, DataContainerArray::Pointer dca)
  {
    QVariant var;
    bool propWasSet;
    int err = 0;

    DataArrayPath imageGeom3D_featureIds("ImageGeom3D", "Image3DData", "FeatureIds");
    DataArrayPath rectGrid_featureIds("RectGrid", "RectGridData", "FeatureIds");

    QString imageSurfMesh = "ImageSurfMesh";
    QString rectGridSurfMesh = "RectGridSurfMesh";

    SET_PROPERTIES_AND_CHECK(filter, imageGeom3D_featureIds, imageSurfMesh, err);
    SET_PROPERTIES_AND_CHECK(filter, rectGrid_featureIds, rectGridSurfMesh, err);

    TriangleGeom::Pointer imageSurfMeshGeom = dca->getDataContainer(imageSurfMesh)->getGeometryAs<TriangleGeom>();
    TriangleGeom::Pointer rectGridSurfMeshGeom = dca->getDataContainer(rectGridSurfMesh)->getGeometryAs<TriangleGeom>();

    DREAM3D_REQUIRE_EQUAL(imageSurfMeshGeom->getNumberOfTris(), 36);
    DREAM3D_REQUIRE_EQUAL(rectGridSurfMeshGeom->getNumberOfTris(), 36);
    DREAM3D_REQUIRE_EQUAL(imageSurfMeshGeom->getNumberOfVertices(), 18);
    DREAM3D_REQUIRE_EQUAL(rectGridSurfMeshGeom->getNumberOfVertices(), 18);

    SharedTriList::Pointer imageMeshTris = imageSurfMeshGeom->getTriangles();
    SharedTriList::Pointer rectGridMeshTris = rectGridSurfMeshGeom->getTriangles();
    int64_t* imageTriPtr = imageMeshTris->getPointer(0);
    int64_t* rectGridTriPtr = rectGridMeshTris->getPointer(0);

    DREAM3D_REQUIRE_EQUAL(imageMeshTris->getNumberOfTuples(), rectGridMeshTris->getNumberOfTuples());

    for(size_t i = 0; i < imageMeshTris->getSize(); i++)
    {
      DREAM3D_REQUIRE_EQUAL(imageTriPtr[i], rectGridTriPtr[i]);
    }

    int64_t tri[3] = {0, 0, 0};

    imageSurfMeshGeom->getVertsAtTri(0, tri);
    DREAM3D_REQUIRE_EQUAL(tri[0], 0);
    DREAM3D_REQUIRE_EQUAL(tri[1], 1);
    DREAM3D_REQUIRE_EQUAL(tri[2], 2);

    imageSurfMeshGeom->getVertsAtTri(1, tri);
    DREAM3D_REQUIRE_EQUAL(tri[0], 1);
    DREAM3D_REQUIRE_EQUAL(tri[1], 3);
    DREAM3D_REQUIRE_EQUAL(tri[2], 2);

    imageSurfMeshGeom->getVertsAtTri(2, tri);
    DREAM3D_REQUIRE_EQUAL(tri[0], 0);
    DREAM3D_REQUIRE_EQUAL(tri[1], 2);
    DREAM3D_REQUIRE_EQUAL(tri[2], 4);

    imageSurfMeshGeom->getVertsAtTri(3, tri);
    DREAM3D_REQUIRE_EQUAL(tri[0], 4);
    DREAM3D_REQUIRE_EQUAL(tri[1], 2);
    DREAM3D_REQUIRE_EQUAL(tri[2], 5);

    imageSurfMeshGeom->getVertsAtTri(4, tri);
    DREAM3D_REQUIRE_EQUAL(tri[0], 0);
    DREAM3D_REQUIRE_EQUAL(tri[1], 4);
    DREAM3D_REQUIRE_EQUAL(tri[2], 1);

    imageSurfMeshGeom->getVertsAtTri(5, tri);
    DREAM3D_REQUIRE_EQUAL(tri[0], 4);
    DREAM3D_REQUIRE_EQUAL(tri[1], 6);
    DREAM3D_REQUIRE_EQUAL(tri[2], 1);

    imageSurfMeshGeom->getVertsAtTri(6, tri);
    DREAM3D_REQUIRE_EQUAL(tri[0], 6);
    DREAM3D_REQUIRE_EQUAL(tri[1], 1);
    DREAM3D_REQUIRE_EQUAL(tri[2], 7);

    imageSurfMeshGeom->getVertsAtTri(7, tri);
    DREAM3D_REQUIRE_EQUAL(tri[0], 1);
    DREAM3D_REQUIRE_EQUAL(tri[1], 3);
    DREAM3D_REQUIRE_EQUAL(tri[2], 7);

    imageSurfMeshGeom->getVertsAtTri(8, tri);
    DREAM3D_REQUIRE_EQUAL(tri[0], 2);
    DREAM3D_REQUIRE_EQUAL(tri[1], 7);
    DREAM3D_REQUIRE_EQUAL(tri[2], 5);

    imageSurfMeshGeom->getVertsAtTri(9, tri);
    DREAM3D_REQUIRE_EQUAL(tri[0], 3);
    DREAM3D_REQUIRE_EQUAL(tri[1], 7);
    DREAM3D_REQUIRE_EQUAL(tri[2], 2);

    imageSurfMeshGeom->getVertsAtTri(10, tri);
    DREAM3D_REQUIRE_EQUAL(tri[0], 4);
    DREAM3D_REQUIRE_EQUAL(tri[1], 5);
    DREAM3D_REQUIRE_EQUAL(tri[2], 8);

    imageSurfMeshGeom->getVertsAtTri(11, tri);
    DREAM3D_REQUIRE_EQUAL(tri[0], 8);
    DREAM3D_REQUIRE_EQUAL(tri[1], 5);
    DREAM3D_REQUIRE_EQUAL(tri[2], 9);

    imageSurfMeshGeom->getVertsAtTri(12, tri);
    DREAM3D_REQUIRE_EQUAL(tri[0], 4);
    DREAM3D_REQUIRE_EQUAL(tri[1], 8);
    DREAM3D_REQUIRE_EQUAL(tri[2], 6);

    imageSurfMeshGeom->getVertsAtTri(13, tri);
    DREAM3D_REQUIRE_EQUAL(tri[0], 8);
    DREAM3D_REQUIRE_EQUAL(tri[1], 10);
    DREAM3D_REQUIRE_EQUAL(tri[2], 6);

    imageSurfMeshGeom->getVertsAtTri(14, tri);
    DREAM3D_REQUIRE_EQUAL(tri[0], 9);
    DREAM3D_REQUIRE_EQUAL(tri[1], 10);
    DREAM3D_REQUIRE_EQUAL(tri[2], 8);

    imageSurfMeshGeom->getVertsAtTri(15, tri);
    DREAM3D_REQUIRE_EQUAL(tri[0], 9);
    DREAM3D_REQUIRE_EQUAL(tri[1], 11);
    DREAM3D_REQUIRE_EQUAL(tri[2], 10);

    imageSurfMeshGeom->getVertsAtTri(16, tri);
    DREAM3D_REQUIRE_EQUAL(tri[0], 10);
    DREAM3D_REQUIRE_EQUAL(tri[1], 6);
    DREAM3D_REQUIRE_EQUAL(tri[2], 11);

    imageSurfMeshGeom->getVertsAtTri(17, tri);
    DREAM3D_REQUIRE_EQUAL(tri[0], 6);
    DREAM3D_REQUIRE_EQUAL(tri[1], 7);
    DREAM3D_REQUIRE_EQUAL(tri[2], 11);

    imageSurfMeshGeom->getVertsAtTri(18, tri);
    DREAM3D_REQUIRE_EQUAL(tri[0], 5);
    DREAM3D_REQUIRE_EQUAL(tri[1], 11);
    DREAM3D_REQUIRE_EQUAL(tri[2], 9);

    imageSurfMeshGeom->getVertsAtTri(19, tri);
    DREAM3D_REQUIRE_EQUAL(tri[0], 7);
    DREAM3D_REQUIRE_EQUAL(tri[1], 11);
    DREAM3D_REQUIRE_EQUAL(tri[2], 5);

    imageSurfMeshGeom->getVertsAtTri(20, tri);
    DREAM3D_REQUIRE_EQUAL(tri[0], 1);
    DREAM3D_REQUIRE_EQUAL(tri[1], 12);
    DREAM3D_REQUIRE_EQUAL(tri[2], 3);

    imageSurfMeshGeom->getVertsAtTri(21, tri);
    DREAM3D_REQUIRE_EQUAL(tri[0], 12);
    DREAM3D_REQUIRE_EQUAL(tri[1], 13);
    DREAM3D_REQUIRE_EQUAL(tri[2], 3);

    imageSurfMeshGeom->getVertsAtTri(22, tri);
    DREAM3D_REQUIRE_EQUAL(tri[0], 1);
    DREAM3D_REQUIRE_EQUAL(tri[1], 6);
    DREAM3D_REQUIRE_EQUAL(tri[2], 12);

    imageSurfMeshGeom->getVertsAtTri(23, tri);
    DREAM3D_REQUIRE_EQUAL(tri[0], 6);
    DREAM3D_REQUIRE_EQUAL(tri[1], 14);
    DREAM3D_REQUIRE_EQUAL(tri[2], 12);

    imageSurfMeshGeom->getVertsAtTri(24, tri);
    DREAM3D_REQUIRE_EQUAL(tri[0], 15);
    DREAM3D_REQUIRE_EQUAL(tri[1], 12);
    DREAM3D_REQUIRE_EQUAL(tri[2], 14);

    imageSurfMeshGeom->getVertsAtTri(25, tri);
    DREAM3D_REQUIRE_EQUAL(tri[0], 15);
    DREAM3D_REQUIRE_EQUAL(tri[1], 13);
    DREAM3D_REQUIRE_EQUAL(tri[2], 12);

    imageSurfMeshGeom->getVertsAtTri(26, tri);
    DREAM3D_REQUIRE_EQUAL(tri[0], 3);
    DREAM3D_REQUIRE_EQUAL(tri[1], 15);
    DREAM3D_REQUIRE_EQUAL(tri[2], 7);

    imageSurfMeshGeom->getVertsAtTri(27, tri);
    DREAM3D_REQUIRE_EQUAL(tri[0], 13);
    DREAM3D_REQUIRE_EQUAL(tri[1], 15);
    DREAM3D_REQUIRE_EQUAL(tri[2], 3);

    imageSurfMeshGeom->getVertsAtTri(28, tri);
    DREAM3D_REQUIRE_EQUAL(tri[0], 6);
    DREAM3D_REQUIRE_EQUAL(tri[1], 10);
    DREAM3D_REQUIRE_EQUAL(tri[2], 14);

    imageSurfMeshGeom->getVertsAtTri(29, tri);
    DREAM3D_REQUIRE_EQUAL(tri[0], 10);
    DREAM3D_REQUIRE_EQUAL(tri[1], 16);
    DREAM3D_REQUIRE_EQUAL(tri[2], 14);

    imageSurfMeshGeom->getVertsAtTri(30, tri);
    DREAM3D_REQUIRE_EQUAL(tri[0], 11);
    DREAM3D_REQUIRE_EQUAL(tri[1], 16);
    DREAM3D_REQUIRE_EQUAL(tri[2], 10);

    imageSurfMeshGeom->getVertsAtTri(31, tri);
    DREAM3D_REQUIRE_EQUAL(tri[0], 11);
    DREAM3D_REQUIRE_EQUAL(tri[1], 17);
    DREAM3D_REQUIRE_EQUAL(tri[2], 16);

    imageSurfMeshGeom->getVertsAtTri(32, tri);
    DREAM3D_REQUIRE_EQUAL(tri[0], 17);
    DREAM3D_REQUIRE_EQUAL(tri[1], 14);
    DREAM3D_REQUIRE_EQUAL(tri[2], 16);

    imageSurfMeshGeom->getVertsAtTri(33, tri);
    DREAM3D_REQUIRE_EQUAL(tri[0], 17);
    DREAM3D_REQUIRE_EQUAL(tri[1], 15);
    DREAM3D_REQUIRE_EQUAL(tri[2], 14);

    imageSurfMeshGeom->getVertsAtTri(34, tri);
    DREAM3D_REQUIRE_EQUAL(tri[0], 7);
    DREAM3D_REQUIRE_EQUAL(tri[1], 17);
    DREAM3D_REQUIRE_EQUAL(tri[2], 11);

    imageSurfMeshGeom->getVertsAtTri(35, tri);
    DREAM3D_REQUIRE_EQUAL(tri[0], 15);
    DREAM3D_REQUIRE_EQUAL(tri[1], 17);
    DREAM3D_REQUIRE_EQUAL(tri[2], 7);

    SharedVertexList::Pointer imageMeshVerts = imageSurfMeshGeom->getVertices();
    SharedVertexList::Pointer rectGridMeshVerts = rectGridSurfMeshGeom->getVertices();
    float* imageVertPtr = imageMeshVerts->getPointer(0);
    float* rectGridVertPtr = rectGridMeshVerts->getPointer(0);

    DREAM3D_REQUIRE_EQUAL(imageMeshVerts->getNumberOfTuples(), rectGridMeshVerts->getNumberOfTuples());

    for(size_t i = 0; i < imageMeshVerts->getNumberOfTuples(); i++)
    {
      DREAM3D_REQUIRE_EQUAL(imageVertPtr[3 * i + 0], rectGridVertPtr[3 * i + 0]);
      DREAM3D_REQUIRE_EQUAL(imageVertPtr[3 * i + 1], rectGridVertPtr[3 * i + 1]);
      DREAM3D_REQUIRE_EQUAL(2 * imageVertPtr[3 * i + 2], rectGridVertPtr[3 * i + 2]);
    }

    float coords[3] = {0, 0, 0};

    imageSurfMeshGeom->getCoords(0, coords);
    DREAM3D_REQUIRE_EQUAL(coords[0], 0.0f);
    DREAM3D_REQUIRE_EQUAL(coords[1], 0.0f);
    DREAM3D_REQUIRE_EQUAL(coords[2], 0.0f);

    imageSurfMeshGeom->getCoords(1, coords);
    DREAM3D_REQUIRE_EQUAL(coords[0], 0.0f);
    DREAM3D_REQUIRE_EQUAL(coords[1], 1.0f);
    DREAM3D_REQUIRE_EQUAL(coords[2], 0.0f);

    imageSurfMeshGeom->getCoords(2, coords);
    DREAM3D_REQUIRE_EQUAL(coords[0], 0.0f);
    DREAM3D_REQUIRE_EQUAL(coords[1], 0.0f);
    DREAM3D_REQUIRE_EQUAL(coords[2], 1.0f);

    imageSurfMeshGeom->getCoords(3, coords);
    DREAM3D_REQUIRE_EQUAL(coords[0], 0.0f);
    DREAM3D_REQUIRE_EQUAL(coords[1], 1.0f);
    DREAM3D_REQUIRE_EQUAL(coords[2], 1.0f);

    imageSurfMeshGeom->getCoords(4, coords);
    DREAM3D_REQUIRE_EQUAL(coords[0], 1.0f);
    DREAM3D_REQUIRE_EQUAL(coords[1], 0.0f);
    DREAM3D_REQUIRE_EQUAL(coords[2], 0.0f);

    imageSurfMeshGeom->getCoords(5, coords);
    DREAM3D_REQUIRE_EQUAL(coords[0], 1.0f);
    DREAM3D_REQUIRE_EQUAL(coords[1], 0.0f);
    DREAM3D_REQUIRE_EQUAL(coords[2], 1.0f);

    imageSurfMeshGeom->getCoords(6, coords);
    DREAM3D_REQUIRE_EQUAL(coords[0], 1.0f);
    DREAM3D_REQUIRE_EQUAL(coords[1], 1.0f);
    DREAM3D_REQUIRE_EQUAL(coords[2], 0.0f);

    imageSurfMeshGeom->getCoords(7, coords);
    DREAM3D_REQUIRE_EQUAL(coords[0], 1.0f);
    DREAM3D_REQUIRE_EQUAL(coords[1], 1.0f);
    DREAM3D_REQUIRE_EQUAL(coords[2], 1.0f);

    imageSurfMeshGeom->getCoords(8, coords);
    DREAM3D_REQUIRE_EQUAL(coords[0], 2.0f);
    DREAM3D_REQUIRE_EQUAL(coords[1], 0.0f);
    DREAM3D_REQUIRE_EQUAL(coords[2], 0.0f);

    imageSurfMeshGeom->getCoords(9, coords);
    DREAM3D_REQUIRE_EQUAL(coords[0], 2.0f);
    DREAM3D_REQUIRE_EQUAL(coords[1], 0.0f);
    DREAM3D_REQUIRE_EQUAL(coords[2], 1.0f);

    imageSurfMeshGeom->getCoords(10, coords);
    DREAM3D_REQUIRE_EQUAL(coords[0], 2.0f);
    DREAM3D_REQUIRE_EQUAL(coords[1], 1.0f);
    DREAM3D_REQUIRE_EQUAL(coords[2], 0.0f);

    imageSurfMeshGeom->getCoords(11, coords);
    DREAM3D_REQUIRE_EQUAL(coords[0], 2.0f);
    DREAM3D_REQUIRE_EQUAL(coords[1], 1.0f);
    DREAM3D_REQUIRE_EQUAL(coords[2], 1.0f);

    imageSurfMeshGeom->getCoords(12, coords);
    DREAM3D_REQUIRE_EQUAL(coords[0], 0.0f);
    DREAM3D_REQUIRE_EQUAL(coords[1], 2.0f);
    DREAM3D_REQUIRE_EQUAL(coords[2], 0.0f);

    imageSurfMeshGeom->getCoords(13, coords);
    DREAM3D_REQUIRE_EQUAL(coords[0], 0.0f);
    DREAM3D_REQUIRE_EQUAL(coords[1], 2.0f);
    DREAM3D_REQUIRE_EQUAL(coords[2], 1.0f);

    imageSurfMeshGeom->getCoords(14, coords);
    DREAM3D_REQUIRE_EQUAL(coords[0], 1.0f);
    DREAM3D_REQUIRE_EQUAL(coords[1], 2.0f);
    DREAM3D_REQUIRE_EQUAL(coords[2], 0.0f);

    imageSurfMeshGeom->getCoords(15, coords);
    DREAM3D_REQUIRE_EQUAL(coords[0], 1.0f);
    DREAM3D_REQUIRE_EQUAL(coords[1], 2.0f);
    DREAM3D_REQUIRE_EQUAL(coords[2], 1.0f);

    imageSurfMeshGeom->getCoords(16, coords);
    DREAM3D_REQUIRE_EQUAL(coords[0], 2.0f);
    DREAM3D_REQUIRE_EQUAL(coords[1], 2.0f);
    DREAM3D_REQUIRE_EQUAL(coords[2], 0.0f);

    imageSurfMeshGeom->getCoords(17, coords);
    DREAM3D_REQUIRE_EQUAL(coords[0], 2.0f);
    DREAM3D_REQUIRE_EQUAL(coords[1], 2.0f);
    DREAM3D_REQUIRE_EQUAL(coords[2], 1.0f);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  int RunTest()
  {
    DataContainerArray::Pointer dca = initializeDataContainerArray();

    QString filtName = "QuickSurfaceMesh";
    FilterManager* fm = FilterManager::Instance();
    IFilterFactory::Pointer factory = fm->getFactoryForFilter(filtName);
    DREAM3D_REQUIRE(factory.get() != nullptr)

    AbstractFilter::Pointer meshFilter = factory->create();
    DREAM3D_REQUIRE(meshFilter.get() != nullptr)

    meshFilter->setDataContainerArray(dca);

    validateQuickSurfaceMesh(meshFilter, dca);

    return EXIT_SUCCESS;
  }
  /**
* @brief
*/
  void operator()()
  {
    int err = EXIT_SUCCESS;
    DREAM3D_REGISTER_TEST(TestFilterAvailability());

    DREAM3D_REGISTER_TEST(RunTest())

    DREAM3D_REGISTER_TEST(RemoveTestFiles())
  }

private:
  QuickSurfaceMeshTest(const QuickSurfaceMeshTest&); // Copy Constructor Not Implemented
  void operator=(const QuickSurfaceMeshTest&);       // Operator '=' Not Implemented
};
