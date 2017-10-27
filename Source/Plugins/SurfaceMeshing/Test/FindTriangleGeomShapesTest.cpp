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
#include "SIMPLib/Geometry/TriangleGeom.h"
#include "SIMPLib/Math/SIMPLibMath.h"
#include "SIMPLib/Plugin/ISIMPLibPlugin.h"
#include "SIMPLib/Plugin/SIMPLibPluginLoader.h"
#include "SIMPLib/SIMPLib.h"
#include "UnitTestSupport.hpp"

#include "SurfaceMeshingTestFileLocations.h"

class FindTriangleGeomShapesTest
{

public:
  FindTriangleGeomShapesTest()
  {
  }
  virtual ~FindTriangleGeomShapesTest()
  {
  }

#define DREAM3D_CLOSE_ENOUGH(L, R, eps)                                                                                                                                                                \
  if(false == SIMPLibMath::closeEnough<>(L, R, eps))                                                                                                                                                   \
  {                                                                                                                                                                                                    \
    QString buf;                                                                                                                                                                                       \
    QTextStream ss(&buf);                                                                                                                                                                              \
    ss << "Your test required the following\n            '";                                                                                                                                           \
    ss << "SIMPLibMath::closeEnough<>(" << #L << ", " << #R << ", " << #eps << "'\n             but this condition was not met with eps=" << eps << "\n";                                              \
    ss << "             " << L << "==" << R;                                                                                                                                                           \
    DREAM3D_TEST_THROW_EXCEPTION(buf.toStdString())                                                                                                                                                    \
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void RemoveTestFiles()
  {
#if REMOVE_TEST_FILES
    QFile::remove(UnitTest::FindTriangleGeomShapesTest::TestFile1);
    QFile::remove(UnitTest::FindTriangleGeomShapesTest::TestFile2);
#endif
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  int TestFilterAvailability()
  {
    // Now instantiate the FindTriangleGeomShapesTest Filter from the FilterManager
    // Also test for other filters that will be needed for the test
    FilterManager* fm = FilterManager::Instance();
    QString filtName = "FindTriangleGeomShapes";
    IFilterFactory::Pointer filterFactory = fm->getFactoryForFilter(filtName);
    if(nullptr == filterFactory.get())
    {
      std::stringstream ss;
      ss << "The FindTriangleGeomShapesTest Requires the use of the " << filtName.toStdString() << " filter which is found in the SurfaceMeshing Plugin";
      DREAM3D_TEST_THROW_EXCEPTION(ss.str())
    }

    filtName = "QuickSurfaceMesh";
    filterFactory = fm->getFactoryForFilter(filtName);
    if(nullptr == filterFactory.get())
    {
      std::stringstream ss;
      ss << "The FindTriangleGeomShapesTest Requires the use of the " << filtName.toStdString() << " filter which is found in the SurfaceMeshing Plugin";
      DREAM3D_TEST_THROW_EXCEPTION(ss.str())
    }

    filtName = "FindTriangleGeomCentroids";
    filterFactory = fm->getFactoryForFilter(filtName);
    if(nullptr == filterFactory.get())
    {
      std::stringstream ss;
      ss << "The FindTriangleGeomShapesTest Requires the use of the " << filtName.toStdString() << " filter which is found in the SurfaceMeshing Plugin";
      DREAM3D_TEST_THROW_EXCEPTION(ss.str())
    }

    filtName = "FindTriangleGeomSizes";
    filterFactory = fm->getFactoryForFilter(filtName);
    if(nullptr == filterFactory.get())
    {
      std::stringstream ss;
      ss << "The FindTriangleGeomShapesTest Requires the use of the " << filtName.toStdString() << " filter which is found in the SurfaceMeshing Plugin";
      DREAM3D_TEST_THROW_EXCEPTION(ss.str())
    }
    return 0;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  int TestFindTriangleGeomShapesTest()
  {
    DataContainerArray::Pointer dca = DataContainerArray::New();

    DataContainer::Pointer idc = DataContainer::New(SIMPL::Defaults::ImageDataContainerName);
    dca->addDataContainer(idc);

    ImageGeom::Pointer image = ImageGeom::CreateGeometry(SIMPL::Geometry::ImageGeometry);
    size_t dims[3] = {256, 128, 64};
    image->setDimensions(dims);
    idc->setGeometry(image);

    QVector<size_t> tDims = {256, 128, 64};
    AttributeMatrix::Pointer attrMat = AttributeMatrix::New(tDims, SIMPL::Defaults::CellAttributeMatrixName, AttributeMatrix::Type::Cell);
    idc->addAttributeMatrix(SIMPL::Defaults::CellAttributeMatrixName, attrMat);

    QVector<size_t> cDims(1, 1);
    Int32ArrayType::Pointer featureIds = Int32ArrayType::CreateArray(tDims, cDims, SIMPL::CellData::FeatureIds);
    featureIds->initializeWithValue(1);
    attrMat->addAttributeArray(SIMPL::CellData::FeatureIds, featureIds);

    FilterManager* fm = FilterManager::Instance();
    bool propWasSet = true;
    QVariant var;

    QString filtName = "QuickSurfaceMesh";
    IFilterFactory::Pointer factory = fm->getFactoryForFilter(filtName);
    DREAM3D_REQUIRE(factory.get() != nullptr);
    AbstractFilter::Pointer meshFilter = factory->create();
    DREAM3D_REQUIRE(meshFilter.get() != nullptr);
    meshFilter->setDataContainerArray(dca);

    DataArrayPath path(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellAttributeMatrixName, SIMPL::CellData::FeatureIds);
    var.setValue(path);
    propWasSet = meshFilter->setProperty("FeatureIdsArrayPath", var);
    if(!propWasSet)
    {
      qDebug() << "Unable to set property FeatureIdsArrayPath";
    }
    meshFilter->execute();
    int32_t err = meshFilter->getErrorCondition();
    DREAM3D_REQUIRE_EQUAL(err, 0);

    filtName = "FindTriangleGeomCentroids";
    factory = fm->getFactoryForFilter(filtName);
    DREAM3D_REQUIRE(factory.get() != nullptr);
    AbstractFilter::Pointer centroidsFilter = factory->create();
    DREAM3D_REQUIRE(centroidsFilter.get() != nullptr);
    centroidsFilter->setDataContainerArray(dca);

    path.update(SIMPL::Defaults::TriangleDataContainerName, SIMPL::Defaults::FaceAttributeMatrixName, SIMPL::FaceData::SurfaceMeshFaceLabels);
    var.setValue(path);
    propWasSet = centroidsFilter->setProperty("FaceLabelsArrayPath", var);
    if(!propWasSet)
    {
      qDebug() << "Unable to set property FaceLabelsArrayPath";
    }
    path.update(SIMPL::Defaults::TriangleDataContainerName, SIMPL::Defaults::FaceFeatureAttributeMatrixName, "");
    var.setValue(path);
    propWasSet = centroidsFilter->setProperty("FeatureAttributeMatrixName", var);
    if(!propWasSet)
    {
      qDebug() << "Unable to set property FeatureAttributeMatrixName";
    }
    centroidsFilter->execute();
    err = centroidsFilter->getErrorCondition();
    DREAM3D_REQUIRE_EQUAL(err, 0);

    filtName = "FindTriangleGeomSizes";
    factory = fm->getFactoryForFilter(filtName);
    DREAM3D_REQUIRE(factory.get() != nullptr);
    AbstractFilter::Pointer sizesFilter = factory->create();
    DREAM3D_REQUIRE(sizesFilter.get() != nullptr);
    sizesFilter->setDataContainerArray(dca);

    path.update(SIMPL::Defaults::TriangleDataContainerName, SIMPL::Defaults::FaceAttributeMatrixName, SIMPL::FaceData::SurfaceMeshFaceLabels);
    var.setValue(path);
    propWasSet = sizesFilter->setProperty("FaceLabelsArrayPath", var);
    if(!propWasSet)
    {
      qDebug() << "Unable to set property FaceLabelsArrayPath";
    }
    path.update(SIMPL::Defaults::TriangleDataContainerName, SIMPL::Defaults::FaceFeatureAttributeMatrixName, "");
    var.setValue(path);
    propWasSet = sizesFilter->setProperty("FeatureAttributeMatrixName", var);
    if(!propWasSet)
    {
      qDebug() << "Unable to set property FeatureAttributeMatrixName";
    }
    sizesFilter->execute();
    err = sizesFilter->getErrorCondition();
    DREAM3D_REQUIRE_EQUAL(err, 0);

    filtName = "FindTriangleGeomShapes";
    factory = fm->getFactoryForFilter(filtName);
    DREAM3D_REQUIRE(factory.get() != nullptr);
    AbstractFilter::Pointer shapesFilter = factory->create();
    DREAM3D_REQUIRE(shapesFilter.get() != nullptr);
    shapesFilter->setDataContainerArray(dca);

    path.update(SIMPL::Defaults::TriangleDataContainerName, SIMPL::Defaults::FaceAttributeMatrixName, SIMPL::FaceData::SurfaceMeshFaceLabels);
    var.setValue(path);
    propWasSet = shapesFilter->setProperty("FaceLabelsArrayPath", var);
    if(!propWasSet)
    {
      qDebug() << "Unable to set property FaceLabelsArrayPath";
    }
    path.update(SIMPL::Defaults::TriangleDataContainerName, SIMPL::Defaults::FaceFeatureAttributeMatrixName, "");
    var.setValue(path);
    propWasSet = shapesFilter->setProperty("FeatureAttributeMatrixName", var);
    if(!propWasSet)
    {
      qDebug() << "Unable to set property FeatureAttributeMatrixName";
    }
    path.update(SIMPL::Defaults::TriangleDataContainerName, SIMPL::Defaults::FaceFeatureAttributeMatrixName, SIMPL::FeatureData::Centroids);
    var.setValue(path);
    propWasSet = shapesFilter->setProperty("CentroidsArrayPath", var);
    if(!propWasSet)
    {
      qDebug() << "Unable to set property CentroidsArrayPath";
    }
    path.update(SIMPL::Defaults::TriangleDataContainerName, SIMPL::Defaults::FaceFeatureAttributeMatrixName, SIMPL::FeatureData::Volumes);
    var.setValue(path);
    propWasSet = shapesFilter->setProperty("VolumesArrayPath", var);
    if(!propWasSet)
    {
      qDebug() << "Unable to set property VolumesArrayPath";
    }
    shapesFilter->execute();
    err = shapesFilter->getErrorCondition();
    DREAM3D_REQUIRE_EQUAL(err, 0);

    AttributeMatrix::Pointer faceFeatAttrMat = dca->getDataContainer(SIMPL::Defaults::TriangleDataContainerName)->getAttributeMatrix(SIMPL::Defaults::FaceFeatureAttributeMatrixName);
    FloatArrayType::Pointer omega3s = faceFeatAttrMat->getAttributeArrayAs<FloatArrayType>(SIMPL::FeatureData::Omega3s);
    FloatArrayType::Pointer axisLengths = faceFeatAttrMat->getAttributeArrayAs<FloatArrayType>(SIMPL::FeatureData::AxisLengths);
    FloatArrayType::Pointer axisEulerAngles = faceFeatAttrMat->getAttributeArrayAs<FloatArrayType>(SIMPL::FeatureData::AxisEulerAngles);
    FloatArrayType::Pointer aspectRatios = faceFeatAttrMat->getAttributeArrayAs<FloatArrayType>(SIMPL::FeatureData::AspectRatios);

    DREAM3D_REQUIRE_EQUAL(omega3s->getNumberOfTuples(), 2);
    DREAM3D_REQUIRE_EQUAL(axisLengths->getNumberOfTuples(), 2);
    DREAM3D_REQUIRE_EQUAL(axisEulerAngles->getNumberOfTuples(), 2);
    DREAM3D_REQUIRE_EQUAL(aspectRatios->getNumberOfTuples(), 2);

    DREAM3D_CLOSE_ENOUGH(omega3s->getValue(1), 0.824f, 0.0001f);

    DREAM3D_CLOSE_ENOUGH(axisLengths->getValue(3), 160.0f, 1.0f);
    DREAM3D_CLOSE_ENOUGH(axisLengths->getValue(4), 80.0f, 1.0f);
    DREAM3D_CLOSE_ENOUGH(axisLengths->getValue(5), 40.0f, 1.0f);

    DREAM3D_CLOSE_ENOUGH(axisEulerAngles->getValue(3), 3.141f, 0.001f);
    DREAM3D_CLOSE_ENOUGH(axisEulerAngles->getValue(4), 0.0f, 0.001f);
    DREAM3D_CLOSE_ENOUGH(axisEulerAngles->getValue(5), 0.0f, 0.001f);

    DREAM3D_CLOSE_ENOUGH(aspectRatios->getValue(2), 0.5f, 0.0001f);
    DREAM3D_CLOSE_ENOUGH(aspectRatios->getValue(3), 0.25f, 0.0001f);

    return EXIT_SUCCESS;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void operator()()
  {
    int err = EXIT_SUCCESS;

    DREAM3D_REGISTER_TEST(TestFilterAvailability());

    DREAM3D_REGISTER_TEST(TestFindTriangleGeomShapesTest())

    DREAM3D_REGISTER_TEST(RemoveTestFiles())
  }

private:
  FindTriangleGeomShapesTest(const FindTriangleGeomShapesTest&); // Copy Constructor Not Implemented
  void operator=(const FindTriangleGeomShapesTest&);             // Operator '=' Not Implemented
};
