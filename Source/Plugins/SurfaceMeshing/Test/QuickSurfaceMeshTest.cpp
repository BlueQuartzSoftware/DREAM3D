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

#define SET_FILTER_PROPERTY_WITH_CHECK(filter, key, value, errVal)                                                                                                                                     \
  var.setValue(value);                                                                                                                                                                                 \
  propWasSet = filter->setProperty(key, var);                                                                                                                                                          \
  if(!propWasSet)                                                                                                                                                                                      \
  {                                                                                                                                                                                                    \
    qDebug() << "Unable to set property " << key;                                                                                                                                                      \
  }                                                                                                                                                                                                    \
  DREAM3D_REQUIRE_EQUAL(propWasSet, true)

/**
 * @brief The QuickSurfaceMeshTest class
 */
class QuickSurfaceMeshTest
{
public:
  QuickSurfaceMeshTest() = default;
  ~QuickSurfaceMeshTest() = default;

  SIMPL_TYPE_MACRO(QuickSurfaceMeshTest)

  QuickSurfaceMeshTest(const QuickSurfaceMeshTest&) = delete;            // Copy Constructor Not Implemented
  QuickSurfaceMeshTest(QuickSurfaceMeshTest&&) = delete;                 // Move Constructor Not Implemented
  QuickSurfaceMeshTest& operator=(const QuickSurfaceMeshTest&) = delete; // Copy Assignment Not Implemented
  QuickSurfaceMeshTest& operator=(QuickSurfaceMeshTest&&) = delete;      // Move Assignment Not Implemented

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void RemoveTestFiles()
  {
#if REMOVE_TEST_FILES
    QFile::remove(UnitTest::QuickSurfaceMeshTest::TestFile1);
    QFile::remove(UnitTest::QuickSurfaceMeshTest::TestFile1Xdmf);
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
    IFilterFactory::Pointer filterFactory = fm->getFactoryFromClassName(filtName);
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
    size_t dims[3] = {2, 2, 1};
    image3D->setDimensions(dims);
    image3D_DC->setGeometry(image3D);

    RectGridGeom::Pointer rectGrid = RectGridGeom::CreateGeometry(SIMPL::Geometry::RectGridGeometry);
    rectGrid->setDimensions(dims);
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
  void validateQuickSurfaceMesh(const AbstractFilter::Pointer& filter, const DataContainerArray::Pointer& dca)
  {
    QVariant var;
    bool propWasSet;
    int err = 0;

    DataArrayPath imageGeom3D_featureIds("ImageGeom3D", "Image3DData", "FeatureIds");
    QString imageSurfMesh = "ImageSurfMesh";
    QString imageSurfMeshTripleLineDCName = "SurfaceMesh TripleLines";

    SET_FILTER_PROPERTY_WITH_CHECK(filter, "FeatureIdsArrayPath", imageGeom3D_featureIds, err)
    SET_FILTER_PROPERTY_WITH_CHECK(filter, "SurfaceDataContainerName", imageSurfMesh, err)
    SET_FILTER_PROPERTY_WITH_CHECK(filter, "TripleLineDataContainerName", imageSurfMeshTripleLineDCName, err)
    // Run the filter NOW to create the ImageGeom based surface mesh
    filter->execute();
    err = filter->getErrorCondition();
    DREAM3D_REQUIRE_EQUAL(err, 0);

    // Now setup to create a surface mesh from a RectilinearGrid Geometry
    DataArrayPath rectGrid_featureIds("RectGrid", "RectGridData", "FeatureIds");
    QString rectGridSurfMesh = "RectGridSurfMesh";
    QString rectGridSurfMeshTripleLineDCName = "RectGrid TripleLines";

    SET_FILTER_PROPERTY_WITH_CHECK(filter, "FeatureIdsArrayPath", rectGrid_featureIds, err)
    SET_FILTER_PROPERTY_WITH_CHECK(filter, "SurfaceDataContainerName", rectGridSurfMesh, err)
    SET_FILTER_PROPERTY_WITH_CHECK(filter, "TripleLineDataContainerName", rectGridSurfMeshTripleLineDCName, err)

    filter->execute();
    err = filter->getErrorCondition();
    DREAM3D_REQUIRE_EQUAL(err, 0);

    //##################################################################################################################
    FilterManager* fm = FilterManager::Instance();
    QString filtName = "DataContainerWriter";
    IFilterFactory::Pointer factory = fm->getFactoryFromClassName(filtName);
    DREAM3D_REQUIRE(factory.get() != nullptr);
    AbstractFilter::Pointer writer = factory->create();
    DREAM3D_REQUIRE(writer.get() != nullptr);
    writer->setDataContainerArray(dca);
    var.setValue(UnitTest::QuickSurfaceMeshTest::TestFile1);
    propWasSet = writer->setProperty("OutputFile", var);
    if(!propWasSet)
    {
      qDebug() << "Unable to set property OutputFile";
    }
    writer->execute();
    err = writer->getErrorCondition();
    DREAM3D_REQUIRED(err, >=, 0)

    //##################################################################################################################
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

    int64_t tri[3] = {0, 0, 0};

    /**
     * This next bit of code uses a fixed array based that was manually verified by using ParaView to look at the mesh
     * and then verifiy that the winding is correct. The bit of code that _should_ be used is the constant 2D array. If
     * the QuickSurfaceMesh is changed so that this test starts failing the developer will need to go back through the
     * generated mesh manually and verify the output is correct. Then they can flip the #define from 1 to 0 (look a bit
     * farther down for another one), run the test, and then copy the proper output from the test which will generate the
     * proper 2D array again. Once you paste that code in here, flip the #define back to 1 from 0 and run the test. The
     * UnitTest should now pass.
     */
#if 1
    float k_TriangleIndices[36][3] = {
        /* 0 */ {0, 2, 1},
        /* 1 */ {1, 2, 3},
        /* 2 */ {0, 4, 2},
        /* 3 */ {4, 5, 2},
        /* 4 */ {0, 1, 4},
        /* 5 */ {4, 1, 6},
        /* 6 */ {6, 1, 7},
        /* 7 */ {1, 3, 7},
        /* 8 */ {5, 7, 2},
        /* 9 */ {2, 7, 3},
        /* 10 */ {4, 8, 5},
        /* 11 */ {8, 9, 5},
        /* 12 */ {4, 6, 8},
        /* 13 */ {8, 6, 10},
        /* 14 */ {8, 10, 9},
        /* 15 */ {10, 11, 9},
        /* 16 */ {10, 6, 11},
        /* 17 */ {6, 7, 11},
        /* 18 */ {9, 11, 5},
        /* 19 */ {5, 11, 7},
        /* 20 */ {1, 3, 12},
        /* 21 */ {12, 3, 13},
        /* 22 */ {1, 12, 6},
        /* 23 */ {6, 12, 14},
        /* 24 */ {14, 12, 15},
        /* 25 */ {12, 13, 15},
        /* 26 */ {7, 15, 3},
        /* 27 */ {3, 15, 13},
        /* 28 */ {6, 14, 10},
        /* 29 */ {10, 14, 16},
        /* 30 */ {10, 16, 11},
        /* 31 */ {16, 17, 11},
        /* 32 */ {16, 14, 17},
        /* 33 */ {14, 15, 17},
        /* 34 */ {11, 17, 7},
        /* 35 */ {7, 17, 15},
    };
#else
    std::cout << "float k_TriangleIndices[" << imageSurfMeshGeom->getNumberOfTris() << "][3] = {" << std::endl;
    for(size_t t = 0; t < imageSurfMeshGeom->getNumberOfTris(); t++)
    {
      imageSurfMeshGeom->getVertsAtTri(t, tri);
      std::cout << "  /* " << t << "*/  {" << tri[0] << ", " << tri[1] << ", " << tri[2] << "}, " << std::endl;
    }
    std::cout << "};" << std::endl;
#endif
    for(size_t i = 0; i < imageMeshTris->getSize(); i++)
    {
      DREAM3D_REQUIRE_EQUAL(imageTriPtr[i], rectGridTriPtr[i]);
    }

    for(size_t t = 0; t < imageSurfMeshGeom->getNumberOfTris(); t++)
    {
      imageSurfMeshGeom->getVertsAtTri(t, tri);
      DREAM3D_REQUIRE_EQUAL(tri[0], k_TriangleIndices[t][0]);
      DREAM3D_REQUIRE_EQUAL(tri[1], k_TriangleIndices[t][1]);
      DREAM3D_REQUIRE_EQUAL(tri[2], k_TriangleIndices[t][2]);
    }

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
#if 1
    float k_VertexCoords[18][3] = {
        /* 0 */ {0.00000f, 0.00000f, 0.00000f},
        /* 1 */ {0.00000f, 1.00000f, 0.00000f},
        /* 2 */ {0.00000f, 0.00000f, 1.00000f},
        /* 3 */ {0.00000f, 1.00000f, 1.00000f},
        /* 4 */ {1.00000f, 0.00000f, 0.00000f},
        /* 5 */ {1.00000f, 0.00000f, 1.00000f},
        /* 6 */ {1.00000f, 1.00000f, 0.00000f},
        /* 7 */ {1.00000f, 1.00000f, 1.00000f},
        /* 8 */ {2.00000f, 0.00000f, 0.00000f},
        /* 9 */ {2.00000f, 0.00000f, 1.00000f},
        /* 10 */ {2.00000f, 1.00000f, 0.00000f},
        /* 11 */ {2.00000f, 1.00000f, 1.00000f},
        /* 12 */ {0.00000f, 2.00000f, 0.00000f},
        /* 13 */ {0.00000f, 2.00000f, 1.00000f},
        /* 14 */ {1.00000f, 2.00000f, 0.00000f},
        /* 15 */ {1.00000f, 2.00000f, 1.00000f},
        /* 16 */ {2.00000f, 2.00000f, 0.00000f},
        /* 17 */ {2.00000f, 2.00000f, 1.00000f},
    };
#else
    std::cout.precision(5);
    std::cout << "float k_VertexCoords[" << imageSurfMeshGeom->getNumberOfVertices() << "][3] = {" << std::endl;
    std::cout << std::fixed;
    for(size_t v = 0; v < imageSurfMeshGeom->getNumberOfVertices(); v++)
    {
      imageSurfMeshGeom->getCoords(v, coords);
      std::cout << "  /* " << v << "*/  {" << coords[0] << ", " << coords[1] << ", " << coords[2] << "}, " << std::endl;
    }
    std::cout << "};" << std::endl;
#endif

    for(size_t v = 0; v < imageSurfMeshGeom->getNumberOfVertices(); v++)
    {
      imageSurfMeshGeom->getCoords(v, coords);
      DREAM3D_REQUIRE_EQUAL(coords[0], k_VertexCoords[v][0]);
      DREAM3D_REQUIRE_EQUAL(coords[1], k_VertexCoords[v][1]);
      DREAM3D_REQUIRE_EQUAL(coords[2], k_VertexCoords[v][2]);
    }
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  int RunTest()
  {
    DataContainerArray::Pointer dca = initializeDataContainerArray();

    QString filtName = "QuickSurfaceMesh";
    FilterManager* fm = FilterManager::Instance();
    IFilterFactory::Pointer factory = fm->getFactoryFromClassName(filtName);
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
    std::cout << "---- " << getNameOfClass().toStdString() << " ----" << std::endl;

    DREAM3D_REGISTER_TEST(TestFilterAvailability());

    DREAM3D_REGISTER_TEST(RunTest())

    DREAM3D_REGISTER_TEST(RemoveTestFiles())
  }
};
