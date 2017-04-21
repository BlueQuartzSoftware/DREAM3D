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

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/Common/FilterPipeline.h"
#include "SIMPLib/Common/FilterManager.h"
#include "SIMPLib/Common/FilterFactory.hpp"
#include "SIMPLib/Geometry/ImageGeom.h"
#include "SIMPLib/Math/SIMPLibMath.h"
#include "SIMPLib/Plugin/ISIMPLibPlugin.h"
#include "SIMPLib/Plugin/SIMPLibPluginLoader.h"
#include "SIMPLib/Utilities/UnitTestSupport.hpp"
#include "SIMPLib/Utilities/QMetaObjectUtilities.h"

#include "StatisticsTestFileLocations.h"

class FindShapesTest
{

  public:
    FindShapesTest() {}
    virtual ~FindShapesTest() {}

#define DREAM3D_CLOSE_ENOUGH(L, R, eps)\
  if (false == SIMPLibMath::closeEnough<>(L, R, eps) ) {  \
    QString buf;\
    QTextStream ss(&buf);\
    ss << "Your test required the following\n            '";\
    ss << "SIMPLibMath::closeEnough<>(" << #L << ", " << #R << ", " << #eps << "'\n             but this condition was not met with eps=" << eps << "\n";\
    ss << "             " << L << "==" << R;\
    DREAM3D_TEST_THROW_EXCEPTION( buf.toStdString() ) }


  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void RemoveTestFiles()
  {
  #if REMOVE_TEST_FILES
    QFile::remove(UnitTest::FindShapesTest::TestFile1);
    QFile::remove(UnitTest::FindShapesTest::TestFile2);
  #endif
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  int TestFilterAvailability()
  {
    // Now instantiate the FindShapesTest Filter from the FilterManager
    // Also test for other filters that will be needed for the test
    FilterManager* fm = FilterManager::Instance();
    QString filtName = "FindShapes";
    IFilterFactory::Pointer filterFactory = fm->getFactoryForFilter(filtName);
    if (nullptr == filterFactory.get())
    {
      std::stringstream ss;
      ss << "The FindShapesTest Requires the use of the " << filtName.toStdString() << " filter which is found in the Statistics Plugin";
      DREAM3D_TEST_THROW_EXCEPTION(ss.str())
    }
    
    filtName = "FindFeatureCentroids";
    filterFactory = fm->getFactoryForFilter(filtName);
    if(nullptr == filterFactory.get())
    {
      std::stringstream ss;
      ss << "The FindShapesTest Requires the use of the " << filtName.toStdString() << " filter which is found in the Generic Plugin";
      DREAM3D_TEST_THROW_EXCEPTION(ss.str())
    }

    return 0;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  int TestFindShapesTest()
  {
    DataContainerArray::Pointer dca = DataContainerArray::New();

    DataContainer::Pointer idc = DataContainer::New(SIMPL::Defaults::ImageDataContainerName);
    dca->addDataContainer(idc);

    DataContainer::Pointer idc2 = DataContainer::New("ImageDataContainer2");
    dca->addDataContainer(idc2);

    ImageGeom::Pointer image = ImageGeom::CreateGeometry(SIMPL::Geometry::ImageGeometry);
    size_t dims[3] = { 256, 128, 64 };
    float res[3] = {0.75f, 0.5f, 0.25f};
    image->setDimensions(dims);
    image->setResolution(res);
    idc->setGeometry(image);

    ImageGeom::Pointer image2 = ImageGeom::CreateGeometry(SIMPL::Geometry::ImageGeometry);
    dims[0] = 128;
    dims[1] = 256;
    dims[2] = 1;
    res[2] = 1.0f;
    image2->setDimensions(dims);
    image2->setResolution(res);
    idc2->setGeometry(image2);

    QVector<size_t> tDims = { 256, 128, 64 };
    AttributeMatrix::Pointer attrMat = AttributeMatrix::New(tDims, SIMPL::Defaults::CellAttributeMatrixName, AttributeMatrix::Type::Cell);
    idc->addAttributeMatrix(SIMPL::Defaults::CellAttributeMatrixName, attrMat);

    tDims.resize(1);
    tDims[0] = 2;
    AttributeMatrix::Pointer featAttrMat = AttributeMatrix::New(tDims, SIMPL::Defaults::CellFeatureAttributeMatrixName, AttributeMatrix::Type::CellFeature);
    idc->addAttributeMatrix(SIMPL::Defaults::CellFeatureAttributeMatrixName, featAttrMat);

    QVector<size_t> cDims(1, 1);
    Int32ArrayType::Pointer featureIds = Int32ArrayType::CreateArray(256 * 128 * 64, cDims, SIMPL::CellData::FeatureIds);
    featureIds->initializeWithValue(1);
    attrMat->addAttributeArray(SIMPL::CellData::FeatureIds, featureIds);

    tDims.resize(3);
    tDims[0] = 256;
    tDims[1] = 128;
    tDims[2] = 1;
    attrMat = AttributeMatrix::New(tDims, SIMPL::Defaults::CellAttributeMatrixName, AttributeMatrix::Type::Cell);
    idc2->addAttributeMatrix(SIMPL::Defaults::CellAttributeMatrixName, attrMat);

    tDims.resize(1);
    tDims[0] = 2;
    featAttrMat = AttributeMatrix::New(tDims, SIMPL::Defaults::CellFeatureAttributeMatrixName, AttributeMatrix::Type::CellFeature);
    idc2->addAttributeMatrix(SIMPL::Defaults::CellFeatureAttributeMatrixName, featAttrMat);

    featureIds = Int32ArrayType::CreateArray(256 * 128 * 1, cDims, SIMPL::CellData::FeatureIds);
    featureIds->initializeWithValue(1);
    attrMat->addAttributeArray(SIMPL::CellData::FeatureIds, featureIds);

    FilterManager* fm = FilterManager::Instance();
    bool propWasSet = true;
    QVariant var;

    QString filtName = "FindFeatureCentroids";
    IFilterFactory::Pointer factory = fm->getFactoryForFilter(filtName);
    DREAM3D_REQUIRE(factory.get() != nullptr);
    AbstractFilter::Pointer centroidsFilter = factory->create();
    DREAM3D_REQUIRE(centroidsFilter.get() != nullptr);
    centroidsFilter->setDataContainerArray(dca);

    DataArrayPath path(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellAttributeMatrixName, SIMPL::CellData::FeatureIds);
    var.setValue(path);
    propWasSet = centroidsFilter->setProperty("FeatureIdsArrayPath", var);
    if(!propWasSet)
    {
      qDebug() << "Unable to set property FeatureIdsArrayPath";
    }
    path.update(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellFeatureAttributeMatrixName, SIMPL::FeatureData::Centroids);
    var.setValue(path);
    propWasSet = centroidsFilter->setProperty("CentroidsArrayPath", var);
    if(!propWasSet)
    {
      qDebug() << "Unable to set property CentroidsArrayPath";
    }
    centroidsFilter->execute();
    int32_t err = centroidsFilter->getErrorCondition();
    DREAM3D_REQUIRE_EQUAL(err, 0);

    path.update("ImageDataContainer2", SIMPL::Defaults::CellAttributeMatrixName, SIMPL::CellData::FeatureIds);
    var.setValue(path);
    propWasSet = centroidsFilter->setProperty("FeatureIdsArrayPath", var);
    if(!propWasSet)
    {
      qDebug() << "Unable to set property FeatureIdsArrayPath";
    }
    path.update("ImageDataContainer2", SIMPL::Defaults::CellFeatureAttributeMatrixName, SIMPL::FeatureData::Centroids);
    var.setValue(path);
    propWasSet = centroidsFilter->setProperty("CentroidsArrayPath", var);
    if(!propWasSet)
    {
      qDebug() << "Unable to set property CentroidsArrayPath";
    }
    centroidsFilter->execute();
    err = centroidsFilter->getErrorCondition();
    DREAM3D_REQUIRE_EQUAL(err, 0);

    filtName = "FindShapes";
    factory = fm->getFactoryForFilter(filtName);
    DREAM3D_REQUIRE(factory.get() != nullptr);
    AbstractFilter::Pointer shapesFilter = factory->create();
    DREAM3D_REQUIRE(shapesFilter.get() != nullptr);
    shapesFilter->setDataContainerArray(dca);

    path.update(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellAttributeMatrixName, SIMPL::CellData::FeatureIds);
    var.setValue(path);
    propWasSet = shapesFilter->setProperty("FeatureIdsArrayPath", var);
    if(!propWasSet)
    {
      qDebug() << "Unable to set property FeatureIdsArrayPath";
    }
    path.update(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellFeatureAttributeMatrixName, "");
    var.setValue(path);
    propWasSet = shapesFilter->setProperty("CellFeatureAttributeMatrixName", var);
    if(!propWasSet)
    {
      qDebug() << "Unable to set property CellFeatureAttributeMatrixName";
    }
    path.update(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellFeatureAttributeMatrixName, SIMPL::FeatureData::Centroids);
    var.setValue(path);
    propWasSet = shapesFilter->setProperty("CentroidsArrayPath", var);
    if(!propWasSet)
    {
      qDebug() << "Unable to set property CentroidsArrayPath";
    }
    shapesFilter->execute();
    err = shapesFilter->getErrorCondition();
    DREAM3D_REQUIRE_EQUAL(err, 0);

    path.update("ImageDataContainer2", SIMPL::Defaults::CellAttributeMatrixName, SIMPL::CellData::FeatureIds);
    var.setValue(path);
    propWasSet = shapesFilter->setProperty("FeatureIdsArrayPath", var);
    if(!propWasSet)
    {
      qDebug() << "Unable to set property FeatureIdsArrayPath";
    }
    path.update("ImageDataContainer2", SIMPL::Defaults::CellFeatureAttributeMatrixName, "");
    var.setValue(path);
    propWasSet = shapesFilter->setProperty("CellFeatureAttributeMatrixName", var);
    if(!propWasSet)
    {
      qDebug() << "Unable to set property CellFeatureAttributeMatrixName";
    }
    path.update("ImageDataContainer2", SIMPL::Defaults::CellFeatureAttributeMatrixName, SIMPL::FeatureData::Centroids);
    var.setValue(path);
    propWasSet = shapesFilter->setProperty("CentroidsArrayPath", var);
    if(!propWasSet)
    {
      qDebug() << "Unable to set property CentroidsArrayPath";
    }
    shapesFilter->execute();
    err = shapesFilter->getErrorCondition();
    DREAM3D_REQUIRE_EQUAL(err, 0);

    featAttrMat = dca->getDataContainer(SIMPL::Defaults::ImageDataContainerName)->getAttributeMatrix(SIMPL::Defaults::CellFeatureAttributeMatrixName);
    FloatArrayType::Pointer omega3s = featAttrMat->getAttributeArrayAs<FloatArrayType>(SIMPL::FeatureData::Omega3s);
    FloatArrayType::Pointer axisLengths = featAttrMat->getAttributeArrayAs<FloatArrayType>(SIMPL::FeatureData::AxisLengths);
    FloatArrayType::Pointer axisEulerAngles = featAttrMat->getAttributeArrayAs<FloatArrayType>(SIMPL::FeatureData::AxisEulerAngles);
    FloatArrayType::Pointer aspectRatios = featAttrMat->getAttributeArrayAs<FloatArrayType>(SIMPL::FeatureData::AspectRatios);
    FloatArrayType::Pointer volumes = featAttrMat->getAttributeArrayAs<FloatArrayType>(SIMPL::FeatureData::Volumes);

    DREAM3D_REQUIRE_EQUAL(omega3s->getNumberOfTuples(), 2);
    DREAM3D_REQUIRE_EQUAL(axisLengths->getNumberOfTuples(), 2);
    DREAM3D_REQUIRE_EQUAL(axisEulerAngles->getNumberOfTuples(), 2);
    DREAM3D_REQUIRE_EQUAL(aspectRatios->getNumberOfTuples(), 2);

    DREAM3D_CLOSE_ENOUGH(omega3s->getValue(1), 0.7879f, 0.0001f);

    DREAM3D_CLOSE_ENOUGH(axisLengths->getValue(3), 120.0f, 1.5f);
    DREAM3D_CLOSE_ENOUGH(axisLengths->getValue(4), 40.0f, 1.5f);
    DREAM3D_CLOSE_ENOUGH(axisLengths->getValue(5), 10.0f, 1.5f);

    DREAM3D_CLOSE_ENOUGH(axisEulerAngles->getValue(3), 3.141f, 0.001f);
    DREAM3D_CLOSE_ENOUGH(axisEulerAngles->getValue(4), 0.0f, 0.00001f);
    DREAM3D_CLOSE_ENOUGH(axisEulerAngles->getValue(5), 0.0f, 0.00001f);

    DREAM3D_CLOSE_ENOUGH(aspectRatios->getValue(2), 0.3333f, 0.0001f);
    DREAM3D_CLOSE_ENOUGH(aspectRatios->getValue(3), 0.0833f, 0.0001f);

    float scalarRes = 0.75f * 0.5f * 0.25f;
    float volume = static_cast<float>(image->getNumberOfElements()) * scalarRes;

    DREAM3D_CLOSE_ENOUGH(volumes->getValue(1), volume, 0.0001f);

    featAttrMat = dca->getDataContainer("ImageDataContainer2")->getAttributeMatrix(SIMPL::Defaults::CellFeatureAttributeMatrixName);
    omega3s = featAttrMat->getAttributeArrayAs<FloatArrayType>(SIMPL::FeatureData::Omega3s);
    axisLengths = featAttrMat->getAttributeArrayAs<FloatArrayType>(SIMPL::FeatureData::AxisLengths);
    axisEulerAngles = featAttrMat->getAttributeArrayAs<FloatArrayType>(SIMPL::FeatureData::AxisEulerAngles);
    aspectRatios = featAttrMat->getAttributeArrayAs<FloatArrayType>(SIMPL::FeatureData::AspectRatios);
    volumes = featAttrMat->getAttributeArrayAs<FloatArrayType>(SIMPL::FeatureData::Volumes);

    DREAM3D_REQUIRE_EQUAL(omega3s->getNumberOfTuples(), 2);
    DREAM3D_REQUIRE_EQUAL(axisLengths->getNumberOfTuples(), 2);
    DREAM3D_REQUIRE_EQUAL(axisEulerAngles->getNumberOfTuples(), 2);
    DREAM3D_REQUIRE_EQUAL(aspectRatios->getNumberOfTuples(), 2);

    DREAM3D_CLOSE_ENOUGH(omega3s->getValue(1), 0.0f, 0.00001f);

    DREAM3D_CLOSE_ENOUGH(axisLengths->getValue(3), 73.0f, 1.5f);
    DREAM3D_CLOSE_ENOUGH(axisLengths->getValue(4), 54.0f, 1.5f);
    DREAM3D_CLOSE_ENOUGH(axisLengths->getValue(5), 0.0f, 1.5f);

    DREAM3D_CLOSE_ENOUGH(axisEulerAngles->getValue(3), 1.5707f, 0.001f);
    DREAM3D_CLOSE_ENOUGH(axisEulerAngles->getValue(4), 0.0f, 0.00001f);
    DREAM3D_CLOSE_ENOUGH(axisEulerAngles->getValue(5), 0.0f, 0.00001f);

    DREAM3D_CLOSE_ENOUGH(aspectRatios->getValue(2), 0.75f, 0.0001f);
    DREAM3D_CLOSE_ENOUGH(aspectRatios->getValue(3), 0.0f, 0.00001f);

    scalarRes = 0.75f * 0.5f * 1.0f;
    volume = static_cast<float>(image2->getNumberOfElements()) * scalarRes;

    DREAM3D_CLOSE_ENOUGH(volumes->getValue(1), volume, 0.0001f);

    return EXIT_SUCCESS;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void operator()()
  {
    int err = EXIT_SUCCESS;

    DREAM3D_REGISTER_TEST( TestFilterAvailability() );

    DREAM3D_REGISTER_TEST( TestFindShapesTest() )

    DREAM3D_REGISTER_TEST( RemoveTestFiles() )
  }

  private:
    FindShapesTest(const FindShapesTest&); // Copy Constructor Not Implemented
    void operator=(const FindShapesTest&); // Operator '=' Not Implemented


};

