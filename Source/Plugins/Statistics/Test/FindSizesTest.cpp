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

#include "SIMPLib/Common/FilterFactory.hpp"
#include "SIMPLib/Common/FilterManager.h"
#include "SIMPLib/Common/FilterPipeline.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/Common/TemplateHelpers.hpp"
#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/Geometry/ImageGeom.h"
#include "SIMPLib/Geometry/RectGridGeom.h"
#include "SIMPLib/Geometry/VertexGeom.h"
#include "SIMPLib/Geometry/EdgeGeom.h"
#include "SIMPLib/Geometry/TriangleGeom.h"
#include "SIMPLib/Geometry/QuadGeom.h"
#include "SIMPLib/Geometry/TetrahedralGeom.h"
#include "SIMPLib/Plugin/ISIMPLibPlugin.h"
#include "SIMPLib/Plugin/SIMPLibPluginLoader.h"
#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Utilities/QMetaObjectUtilities.h"
#include "SIMPLib/Utilities/UnitTestSupport.hpp"

#include "StatisticsTestFileLocations.h"

#define SET_PROPERTIES_AND_CHECK(filter, featureIdsPath, featureAttrMatPath, errVal)\
  var.setValue(featureIdsPath);\
  propWasSet = filter->setProperty("FeatureIdsArrayPath", var);\
  if(false == propWasSet)\
  {\
    qDebug() << "Unable to set property FeatureIdsArrayPath";\
  }\
  var.setValue(featureAttrMatPath);\
  propWasSet = filter->setProperty("FeatureAttributeMatrixName", var);\
  if(false == propWasSet)\
  {\
    qDebug() << "Unable to set property FeatureAttributeMatrixName";\
  }\
  filter->execute();\
  err = filter->getErrorCondition();\
  DREAM3D_REQUIRE_EQUAL(err, 0);

class FindSizesTest
{
public:
  FindSizesTest()
  {
  }
  virtual ~FindSizesTest()
  {
  }
  SIMPL_TYPE_MACRO(FindSizesTest)

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void RemoveTestFiles()
  {
#if REMOVE_TEST_FILES
    QFile::remove(UnitTest::FindSizesTest::TestFile1);
    QFile::remove(UnitTest::FindSizesTest::TestFile2);
#endif
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  int TestFilterAvailability()
  {
    // Now instantiate the FindSizesTest Filter from the FilterManager
    QString filtName = "FindSizes";
    FilterManager* fm = FilterManager::Instance();
    IFilterFactory::Pointer filterFactory = fm->getFactoryForFilter(filtName);
    if(nullptr == filterFactory.get())
    {
      std::stringstream ss;
      ss << "The FindSizesTest Requires the use of the " << filtName.toStdString() << " filter which is found in the Statistics Plugin";
      DREAM3D_TEST_THROW_EXCEPTION(ss.str())
    }
    return 0;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  DataContainerArray::Pointer initializeDataContainerArray()
  {
    int err = 0;

    DataContainerArray::Pointer dca = DataContainerArray::New();

    // Create a DataContainer for each geometry
    DataContainer::Pointer image2D_DC = DataContainer::New("ImageGeom2D");
    dca->addDataContainer(image2D_DC);

    DataContainer::Pointer image3D_DC = DataContainer::New("ImageGeom3D");
    dca->addDataContainer(image3D_DC);

    DataContainer::Pointer rectGrid_DC = DataContainer::New("RectGrid");
    dca->addDataContainer(rectGrid_DC);

    DataContainer::Pointer vertex_DC = DataContainer::New("VertexGeom");
    dca->addDataContainer(vertex_DC);

    DataContainer::Pointer edge_DC = DataContainer::New("EdgeGeom");
    dca->addDataContainer(edge_DC);

    DataContainer::Pointer triangle_DC = DataContainer::New("TriangleGeom");
    dca->addDataContainer(triangle_DC);

    DataContainer::Pointer quad_DC = DataContainer::New("QuadGeom");
    dca->addDataContainer(quad_DC);

    DataContainer::Pointer tet_DC = DataContainer::New("TetrahedralGeom");
    dca->addDataContainer(tet_DC);

    // Create an instance of each geometry for testing
    ImageGeom::Pointer image2D = ImageGeom::CreateGeometry(SIMPL::Geometry::ImageGeometry);
    image2D->setDimensions(2, 2, 1);
    image2D_DC->setGeometry(image2D);

    ImageGeom::Pointer image3D = ImageGeom::CreateGeometry(SIMPL::Geometry::ImageGeometry);
    image3D->setDimensions(2, 2, 2);
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

    VertexGeom::Pointer vertex = VertexGeom::CreateGeometry(2, SIMPL::Geometry::VertexGeometry);
    vertex_DC->setGeometry(vertex);

    SharedVertexList::Pointer edgeVerts = EdgeGeom::CreateSharedVertexList(3);
    EdgeGeom::Pointer edge = EdgeGeom::CreateGeometry(2, edgeVerts, SIMPL::Geometry::EdgeGeometry);
    float coords[3] = {0.0f, 0.0f, 0.0f};
    int64_t vertsAtEdge[2] = {0, 1};
    edge->setVertsAtEdge(0, vertsAtEdge);
    vertsAtEdge[1] = 2;
    edge->setVertsAtEdge(1, vertsAtEdge);
    edge->setCoords(0, coords);
    coords[0] = 4.0f;
    edge->setCoords(1, coords);
    coords[0] = 9.0f;
    edge->setCoords(2, coords);
    edge_DC->setGeometry(edge);

    SharedVertexList::Pointer triVerts = TriangleGeom::CreateSharedVertexList(4);
    TriangleGeom::Pointer tris = TriangleGeom::CreateGeometry(2, triVerts, SIMPL::Geometry::TriangleGeometry);
    int64_t vertsAtTri[3] = {0, 1, 2};
    tris->setVertsAtTri(0, vertsAtTri);
    vertsAtTri[0] = 1;
    vertsAtTri[1] = 3;
    vertsAtTri[2] = 2;
    tris->setVertsAtTri(1, vertsAtTri);
    coords[0] = 0.0f;
    tris->setCoords(0, coords);
    coords[0] = 1.0f;
    tris->setCoords(1, coords);
    coords[0] = 2.0f;
    tris->setCoords(3, coords);
    coords[0] = 1.0f;
    coords[1] = 1.0f;
    tris->setCoords(2, coords);
    triangle_DC->setGeometry(tris);

    SharedVertexList::Pointer quadVerts = TriangleGeom::CreateSharedVertexList(6);
    QuadGeom::Pointer quads = QuadGeom::CreateGeometry(2, quadVerts, SIMPL::Geometry::QuadGeometry);
    int64_t vertsAtQuad[4] = {0, 1, 2, 3};
    quads->setVertsAtQuad(0, vertsAtQuad);
    vertsAtQuad[0] = 1;
    vertsAtQuad[1] = 4;
    vertsAtQuad[2] = 5;
    vertsAtQuad[3] = 2;
    quads->setVertsAtQuad(1, vertsAtQuad);
    coords[0] = 0.0f;
    coords[1] = 0.0f;
    coords[2] = 0.0f;
    quads->setCoords(0, coords);
    coords[0] = 1.0f;
    quads->setCoords(1, coords);
    coords[1] = 1.0f;
    quads->setCoords(2, coords);
    coords[0] = 0.0f;
    quads->setCoords(3, coords);
    coords[0] = 2.0f;
    coords[1] = 0.0f;
    quads->setCoords(4, coords);
    coords[1] = 1.0f;
    quads->setCoords(5, coords);
    quad_DC->setGeometry(quads);

    SharedVertexList::Pointer tetVerts = TetrahedralGeom::CreateSharedVertexList(5);
    TetrahedralGeom::Pointer tets = TetrahedralGeom::CreateGeometry(2, tetVerts, SIMPL::Geometry::TetrahedralGeometry);
    int64_t vertsAtTet[4] = {0, 1, 2, 3};
    tets->setVertsAtTet(0, vertsAtTet);
    vertsAtTet[0] = 0;
    vertsAtTet[1] = 2;
    vertsAtTet[2] = 1;
    vertsAtTet[3] = 4;
    tets->setVertsAtTet(1, vertsAtTet);
    coords[0] = 0.0f;
    coords[1] = 0.0f;
    coords[2] = 0.0f;
    tets->setCoords(0, coords);
    coords[0] = 1.0f;
    tets->setCoords(1, coords);
    coords[0] = 0.0f;
    coords[1] = 1.0f;
    tets->setCoords(2, coords);
    coords[1] = 0.0f;
    coords[2] = 1.0f;
    tets->setCoords(3, coords);
    coords[2] = -1.0f;
    tets->setCoords(4, coords);
    tet_DC->setGeometry(tets);

    // Create an element AttributeMatrix and FeatureIds array for each geometry
    QVector<size_t> tDims(1, 4);
    AttributeMatrix::Pointer image2D_AttrMat = AttributeMatrix::New(tDims, "Image2DData", SIMPL::AttributeMatrixType::Cell);
    Int32ArrayType::Pointer image2D_fIDs = Int32ArrayType::CreateArray(4, SIMPL::CellData::FeatureIds);
    image2D_fIDs->initializeWithValue(1);
    image2D_fIDs->setValue(2, 2);
    image2D_fIDs->setValue(3, 2);
    image2D_AttrMat->addAttributeArray(SIMPL::CellData::FeatureIds, image2D_fIDs);
    image2D_DC->addAttributeMatrix("Image2DData", image2D_AttrMat);

    tDims[0] = 8;
    AttributeMatrix::Pointer image3D_AttrMat = AttributeMatrix::New(tDims, "Image3DData", SIMPL::AttributeMatrixType::Cell);
    Int32ArrayType::Pointer image3D_fIDs = Int32ArrayType::CreateArray(8, SIMPL::CellData::FeatureIds);
    image3D_fIDs->initializeWithValue(1);
    image3D_fIDs->setValue(4, 2);
    image3D_fIDs->setValue(5, 2);
    image3D_fIDs->setValue(6, 2);
    image3D_fIDs->setValue(7, 2);
    image3D_AttrMat->addAttributeArray(SIMPL::CellData::FeatureIds, image3D_fIDs);
    image3D_DC->addAttributeMatrix("Image3DData", image3D_AttrMat);

    tDims[0] = 4;
    AttributeMatrix::Pointer rectGrid_AttrMat = AttributeMatrix::New(tDims, "RectGridData", SIMPL::AttributeMatrixType::Cell);
    Int32ArrayType::Pointer rectGrid_fIDs = Int32ArrayType::CreateArray(4, SIMPL::CellData::FeatureIds);
    rectGrid_fIDs->initializeWithValue(1);
    rectGrid_fIDs->setValue(2, 2);
    rectGrid_fIDs->setValue(3, 2);
    rectGrid_AttrMat->addAttributeArray(SIMPL::CellData::FeatureIds, rectGrid_fIDs);
    rectGrid_DC->addAttributeMatrix("RectGridData", rectGrid_AttrMat);

    tDims[0] = 2;
    AttributeMatrix::Pointer vertex_AttrMat = AttributeMatrix::New(tDims, "VertexData", SIMPL::AttributeMatrixType::Vertex);
    Int32ArrayType::Pointer vertex_fIDs = Int32ArrayType::CreateArray(2, SIMPL::CellData::FeatureIds);
    vertex_fIDs->initializeWithValue(1);;
    vertex_fIDs->setValue(1, 2);
    vertex_AttrMat->addAttributeArray(SIMPL::CellData::FeatureIds, vertex_fIDs);
    vertex_DC->addAttributeMatrix("VertexData", vertex_AttrMat);

    AttributeMatrix::Pointer edge_AttrMat = AttributeMatrix::New(tDims, "EdgeData", SIMPL::AttributeMatrixType::Edge);
    Int32ArrayType::Pointer edge_fIDs = Int32ArrayType::CreateArray(2, SIMPL::CellData::FeatureIds);
    edge_fIDs->initializeWithValue(1);;
    edge_fIDs->setValue(1, 2);
    edge_AttrMat->addAttributeArray(SIMPL::CellData::FeatureIds, edge_fIDs);
    edge_DC->addAttributeMatrix("EdgeData", edge_AttrMat);

    AttributeMatrix::Pointer tri_AttrMat = AttributeMatrix::New(tDims, "TriData", SIMPL::AttributeMatrixType::Face);
    Int32ArrayType::Pointer tri_fIDs = Int32ArrayType::CreateArray(2, SIMPL::CellData::FeatureIds);
    tri_fIDs->initializeWithValue(1);;
    tri_fIDs->setValue(1, 2);
    tri_AttrMat->addAttributeArray(SIMPL::CellData::FeatureIds, tri_fIDs);
    triangle_DC->addAttributeMatrix("TriData", tri_AttrMat);

    AttributeMatrix::Pointer quad_AttrMat = AttributeMatrix::New(tDims, "QuadData", SIMPL::AttributeMatrixType::Face);
    Int32ArrayType::Pointer quad_fIDs = Int32ArrayType::CreateArray(2, SIMPL::CellData::FeatureIds);
    quad_fIDs->initializeWithValue(1);;
    quad_fIDs->setValue(1, 2);
    quad_AttrMat->addAttributeArray(SIMPL::CellData::FeatureIds, quad_fIDs);
    quad_DC->addAttributeMatrix("QuadData", quad_AttrMat);

    AttributeMatrix::Pointer tet_AttrMat = AttributeMatrix::New(tDims, "TetData", SIMPL::AttributeMatrixType::Face);
    Int32ArrayType::Pointer tet_fIDs = Int32ArrayType::CreateArray(2, SIMPL::CellData::FeatureIds);
    tet_fIDs->initializeWithValue(1);;
    tet_fIDs->setValue(1, 2);
    tet_AttrMat->addAttributeArray(SIMPL::CellData::FeatureIds, tet_fIDs);
    tet_DC->addAttributeMatrix("TetData", tet_AttrMat);

    // Create a feature AttributeMatrix for each geometry, which all have 2 features
    tDims[0] = 3;
    AttributeMatrix::Pointer image2D_featureAttrMat = AttributeMatrix::New(tDims, "Image2DFeatureData", SIMPL::AttributeMatrixType::CellFeature);
    image2D_DC->addAttributeMatrix("Image2DFeatureData", image2D_featureAttrMat);

    AttributeMatrix::Pointer image3D_featureAttrMat = AttributeMatrix::New(tDims, "Image3DFeatureData", SIMPL::AttributeMatrixType::CellFeature);
    image3D_DC->addAttributeMatrix("Image3DFeatureData", image3D_featureAttrMat);

    AttributeMatrix::Pointer rectGrid_featureAttrMat = AttributeMatrix::New(tDims, "RectGridFeatureData", SIMPL::AttributeMatrixType::CellFeature);
    rectGrid_DC->addAttributeMatrix("RectGridFeatureData", rectGrid_featureAttrMat);

    AttributeMatrix::Pointer vertex_featureAttrMat = AttributeMatrix::New(tDims, "VertexFeatureData", SIMPL::AttributeMatrixType::VertexFeature);
    vertex_DC->addAttributeMatrix("VertexFeatureData", vertex_featureAttrMat);

    AttributeMatrix::Pointer edge_featureAttrMat = AttributeMatrix::New(tDims, "EdgeFeatureData", SIMPL::AttributeMatrixType::EdgeFeature);
    edge_DC->addAttributeMatrix("EdgeFeatureData", edge_featureAttrMat);

    AttributeMatrix::Pointer tri_featureAttrMat = AttributeMatrix::New(tDims, "TriFeatureData", SIMPL::AttributeMatrixType::FaceFeature);
    triangle_DC->addAttributeMatrix("TriFeatureData", tri_featureAttrMat);

    AttributeMatrix::Pointer quad_featureAttrMat = AttributeMatrix::New(tDims, "QuadFeatureData", SIMPL::AttributeMatrixType::FaceFeature);
    quad_DC->addAttributeMatrix("QuadFeatureData", quad_featureAttrMat);

    AttributeMatrix::Pointer tet_featureAttrMat = AttributeMatrix::New(tDims, "TetFeatureData", SIMPL::AttributeMatrixType::CellFeature);
    tet_DC->addAttributeMatrix("TetFeatureData", tet_featureAttrMat);

    return dca;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void validateFindSizes(AbstractFilter::Pointer filter, DataContainerArray::Pointer dca)
  {
    QVariant var;
    bool propWasSet;
    int err = 0;

    DataArrayPath imageGeom2D_featureIds("ImageGeom2D", "Image2DData", "FeatureIds");
    DataArrayPath imageGeom3D_featureIds("ImageGeom3D", "Image3DData", "FeatureIds");
    DataArrayPath rectGrid_featureIds("RectGrid", "RectGridData", "FeatureIds");
    DataArrayPath vertex_featureIds("VertexGeom", "VertexData", "FeatureIds");
    DataArrayPath edge_featureIds("EdgeGeom", "EdgeData", "FeatureIds");
    DataArrayPath tri_featureIds("TriangleGeom", "TriData", "FeatureIds");
    DataArrayPath quad_featureIds("QuadGeom", "QuadData", "FeatureIds");
    DataArrayPath tet_featureIds("TetrahedralGeom", "TetData", "FeatureIds");

    DataArrayPath imageGeom2D_featureAttrMat("ImageGeom2D", "Image2DFeatureData", "");
    DataArrayPath imageGeom3D_featureAttrMat("ImageGeom3D", "Image3DFeatureData", "");
    DataArrayPath rectGrid_featureAttrMat("RectGrid", "RectGridFeatureData", "");
    DataArrayPath vertex_featureAttrMat("VertexGeom", "VertexFeatureData", "");
    DataArrayPath edge_featureAttrMat("EdgeGeom", "EdgeFeatureData", "");
    DataArrayPath tri_featureAttrMat("TriangleGeom", "TriFeatureData", "");
    DataArrayPath quad_featureAttrMat("QuadGeom", "QuadFeatureData", "");
    DataArrayPath tet_featureAttrMat("TetrahedralGeom", "TetFeatureData", "");

    SET_PROPERTIES_AND_CHECK(filter, imageGeom2D_featureIds, imageGeom2D_featureAttrMat, err);
    SET_PROPERTIES_AND_CHECK(filter, imageGeom3D_featureIds, imageGeom3D_featureAttrMat, err);
    SET_PROPERTIES_AND_CHECK(filter, rectGrid_featureIds, rectGrid_featureAttrMat, err);
    SET_PROPERTIES_AND_CHECK(filter, vertex_featureIds, vertex_featureAttrMat, err);
    SET_PROPERTIES_AND_CHECK(filter, edge_featureIds, edge_featureAttrMat, err);
    SET_PROPERTIES_AND_CHECK(filter, tri_featureIds, tri_featureAttrMat, err);
    SET_PROPERTIES_AND_CHECK(filter, quad_featureIds, quad_featureAttrMat, err);
    SET_PROPERTIES_AND_CHECK(filter, tet_featureIds, tet_featureAttrMat, err);

    imageGeom2D_featureAttrMat.setDataArrayName("Volumes");
    imageGeom3D_featureAttrMat.setDataArrayName("Volumes");
    rectGrid_featureAttrMat.setDataArrayName("Volumes");
    vertex_featureAttrMat.setDataArrayName("Volumes");
    edge_featureAttrMat.setDataArrayName("Volumes");
    tri_featureAttrMat.setDataArrayName("Volumes");
    quad_featureAttrMat.setDataArrayName("Volumes");
    tet_featureAttrMat.setDataArrayName("Volumes");

    QVector<size_t> cDims(1, 1);

    FloatArrayType::Pointer image2D_volumes = dca->getPrereqArrayFromPath<FloatArrayType, AbstractFilter>(NULL, imageGeom2D_featureAttrMat, cDims);
    FloatArrayType::Pointer image3D_volumes = dca->getPrereqArrayFromPath<FloatArrayType, AbstractFilter>(NULL, imageGeom3D_featureAttrMat, cDims);
    FloatArrayType::Pointer rectGrid_volumes = dca->getPrereqArrayFromPath<FloatArrayType, AbstractFilter>(NULL, rectGrid_featureAttrMat, cDims);
    FloatArrayType::Pointer vertex_volumes = dca->getPrereqArrayFromPath<FloatArrayType, AbstractFilter>(NULL, vertex_featureAttrMat, cDims);
    FloatArrayType::Pointer edge_volumes = dca->getPrereqArrayFromPath<FloatArrayType, AbstractFilter>(NULL, edge_featureAttrMat, cDims);
    FloatArrayType::Pointer tri_volumes = dca->getPrereqArrayFromPath<FloatArrayType, AbstractFilter>(NULL, tri_featureAttrMat, cDims);
    FloatArrayType::Pointer quad_volumes = dca->getPrereqArrayFromPath<FloatArrayType, AbstractFilter>(NULL, quad_featureAttrMat, cDims);
    FloatArrayType::Pointer tet_volumes = dca->getPrereqArrayFromPath<FloatArrayType, AbstractFilter>(NULL, tet_featureAttrMat, cDims);

    DREAM3D_REQUIRE_EQUAL(image2D_volumes->getValue(1), 2.0f);
    DREAM3D_REQUIRE_EQUAL(image2D_volumes->getValue(2), 2.0f);

    DREAM3D_REQUIRE_EQUAL(image3D_volumes->getValue(1), 4.0f);
    DREAM3D_REQUIRE_EQUAL(image3D_volumes->getValue(2), 4.0f);

    DREAM3D_REQUIRE_EQUAL(rectGrid_volumes->getValue(1), 4.0f);
    DREAM3D_REQUIRE_EQUAL(rectGrid_volumes->getValue(2), 4.0f);

    DREAM3D_REQUIRE_EQUAL(vertex_volumes->getValue(1), 0.0f);
    DREAM3D_REQUIRE_EQUAL(vertex_volumes->getValue(2), 0.0f);

    DREAM3D_REQUIRE_EQUAL(edge_volumes->getValue(1), 4.0f);
    DREAM3D_REQUIRE_EQUAL(edge_volumes->getValue(2), 9.0f);

    DREAM3D_REQUIRE_EQUAL(tri_volumes->getValue(1), 0.5f);
    DREAM3D_REQUIRE_EQUAL(tri_volumes->getValue(2), 0.5f);

    DREAM3D_REQUIRE_EQUAL(quad_volumes->getValue(1), 1.0f);
    DREAM3D_REQUIRE_EQUAL(quad_volumes->getValue(2), 1.0f);

    DREAM3D_REQUIRE_EQUAL(tet_volumes->getValue(1), 1.0f / 6.0f);
    DREAM3D_REQUIRE_EQUAL(tet_volumes->getValue(2), 1.0f / 6.0f);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  int RunTest()
  {
    DataContainerArray::Pointer dca = initializeDataContainerArray();

    QString filtName = "FindSizes";
    FilterManager* fm = FilterManager::Instance();
    IFilterFactory::Pointer factory = fm->getFactoryForFilter(filtName);
    DREAM3D_REQUIRE(factory.get() != nullptr)

    AbstractFilter::Pointer sizeFilter = factory->create();
    DREAM3D_REQUIRE(sizeFilter.get() != nullptr)

    sizeFilter->setDataContainerArray(dca);

    validateFindSizes(sizeFilter, dca);

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
  FindSizesTest(const FindSizesTest&); // Copy Constructor Not Implemented
  void operator=(const FindSizesTest&);        // Operator '=' Not Implemented
};
