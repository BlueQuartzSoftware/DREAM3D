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
#include <QtCore/QDir>
#include <QtCore/QFile>

#include "SIMPLib/Common/FilterFactory.hpp"
#include "SIMPLib/Common/FilterManager.h"
#include "SIMPLib/Common/FilterPipeline.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/Common/TemplateHelpers.hpp"
#include "SIMPLib/CoreFilters/DataContainerWriter.h"
#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/Geometry/ImageGeom.h"
#include "SIMPLib/Plugin/ISIMPLibPlugin.h"
#include "SIMPLib/Plugin/SIMPLibPluginLoader.h"
#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Utilities/QMetaObjectUtilities.h"
#include "SIMPLib/Utilities/UnitTestSupport.hpp"

#include "StatisticsTestFileLocations.h"


const DataArrayPath k_FeatureIdsArrayPath = DataArrayPath("TestDataContainer1", "TestAttributeMatrix1", "FeatureIds");

class FindEuclideanDistMapTest
{
public:
  FindEuclideanDistMapTest()
  {
  }
  virtual ~FindEuclideanDistMapTest()
  {
  }
  SIMPL_TYPE_MACRO(FindEuclideanDistMapTest)

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void RemoveTestFiles()
  {
#if REMOVE_TEST_FILES
//    QFile::remove(UnitTest::FindEuclideanDistMapTest::TestFile1);
//    QFile::remove(UnitTest::FindEuclideanDistMapTest::TestFile2);
#endif
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  int TestFilterAvailability()
  {
    // Now instantiate the FindEuclideanDistMap Filter from the FilterManager
    QString filtName = "FindEuclideanDistMap";
    FilterManager* fm = FilterManager::Instance();
    IFilterFactory::Pointer filterFactory = fm->getFactoryForFilter(filtName);
    if(nullptr == filterFactory.get())
    {
      std::stringstream ss;
      ss << "The FindEuclideanDistMapTest requires the use of the " << filtName.toStdString() << " filter which is found in Core Filters";
      DREAM3D_TEST_THROW_EXCEPTION(ss.str())
    }

    return 0;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  DataContainerArray::Pointer initializeDataContainerArray(QVector<size_t> tDims)
  {
    DataContainerArray::Pointer dca = DataContainerArray::New();

    DataContainer::Pointer m = DataContainer::New();
    m->setName(k_FeatureIdsArrayPath.getDataContainerName());
    ImageGeom::Pointer geom = ImageGeom::CreateGeometry("ImageGeometry");
    m->setGeometry(geom);
    geom->setDimensions(tDims.data());
    geom->setResolution(1.0f, 2.0f, 1.0f);

    // Create Attribute Matrices with different tDims to test validation of tuple compatibility

    AttributeMatrix::Pointer attrMat1 = AttributeMatrix::New(tDims, k_FeatureIdsArrayPath.getAttributeMatrixName(), AttributeMatrix::Type::Cell);

    m->addAttributeMatrix(k_FeatureIdsArrayPath.getAttributeMatrixName(), attrMat1);
    dca->addDataContainer(m);

    QVector<size_t> cDims(1, 1);

    Int32ArrayType::Pointer featureIds = Int32ArrayType::CreateArray(tDims, cDims, k_FeatureIdsArrayPath.getDataArrayName());
    int err = attrMat1->addAttributeArray(k_FeatureIdsArrayPath.getDataArrayName(), featureIds);
    DREAM3D_REQUIRE(err >= 0);
    featureIds->initializeWithValue(1);


    std::vector<int32_t> features = {
      1,1,1,3,3,3,4,4,4,0,
      1,1,1,3,3,3,4,4,4,0,
      1,1,1,3,3,3,4,4,4,0,
      2,2,2,3,3,3,5,5,5,0,
      2,2,2,3,3,3,5,5,5,0,
      2,2,2,3,3,3,5,5,5,0,
    };


    for(size_t i = 0; i < featureIds->getNumberOfTuples(); i++)
    {
      featureIds->setValue(i, features[i]);
    }
    return dca;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  int validateResults(DataContainerArray::Pointer dca)
  {

    AttributeMatrix::Pointer am = dca->getAttributeMatrix(k_FeatureIdsArrayPath);

    Int32ArrayType::Pointer int32Array = am->getAttributeArrayAs<Int32ArrayType>("GBManhattanDistance");

    std::vector<int32_t> GBManhattan = {
    2, 1, 0, 0, 1, 0, 0, 1, 0, -1,
    1, 1, 0, 0, 1, 0, 0, 1, 0, -1,
    0, 0, 0, 0, 1, 0, 0, 0, 0, -1,
    0, 0, 0, 0, 1, 0, 0, 0, 0, -1,
    1, 1, 0, 0, 1, 0, 0, 1, 0, -1,
    2, 1, 0, 0, 1, 0, 0, 1, 0, -1};

    for(size_t i = 0; i < int32Array->getNumberOfTuples(); i++)
    {
       int32_t computedValue = int32Array->getValue(i);
       int32_t refValue = GBManhattan[i];
       DREAM3D_REQUIRE_EQUAL(computedValue, refValue);
    }

    int32Array = am->getAttributeArrayAs<Int32ArrayType>("TJManhattanDistance");
    std::vector<int32_t> TJManhattan = {
    4, 3, 2, 3, 4, 3, 2, 3, 2, -1,
    3, 2, 1, 2, 3, 2, 1, 2, 1, -1,
    2, 1, 0, 1, 2, 1, 0, 1, 0, -1,
    2, 1, 0, 1, 2, 1, 0, 1, 0, -1,
    3, 2, 1, 2, 3, 2, 1, 2, 1, -1,
    4, 3, 2, 3, 4, 3, 2, 3, 2, -1};

    for(size_t i = 0; i < int32Array->getNumberOfTuples(); i++)
    {
      int32_t computedValue = int32Array->getValue(i);
      int32_t refValue = TJManhattan[i];
      DREAM3D_REQUIRE_EQUAL(computedValue, refValue);
    }

    int32Array = am->getAttributeArrayAs<Int32ArrayType>("QPManhattanDistance");
    std::vector<int32_t> QPManhattan = {
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1};
    for(size_t i = 0; i < int32Array->getNumberOfTuples(); i++)
    {
      int32_t computedValue = int32Array->getValue(i);
      int32_t refValue = QPManhattan[i];
      DREAM3D_REQUIRE_EQUAL(computedValue, refValue);
    }



    FloatArrayType::Pointer floatArray = am->getAttributeArrayAs<FloatArrayType>("GBEuclideanDistance");

    std::vector<float> GBEuclidean = {
    4.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
    2.0f, 2.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 2.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
    2.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
    2.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0};

    for(size_t i = 0; i < floatArray->getNumberOfTuples(); i++)
    {
      float computedValue = floatArray->getValue(i);
      float refValue = GBEuclidean[i];
      DREAM3D_COMPARE_FLOATS(&computedValue, &refValue, 1);
    }

    floatArray = am->getAttributeArrayAs<FloatArrayType>("TJEuclideanDistance");
    std::vector<float> TJEuclidean = {
    4.472136f,  4.1231055f, 4.0f, 4.1231055f, 4.472136f,  4.1231055f, 4.0f, 4.1231055f, 4.0f, 0.0f,
    2.828427f,  2.236068f,  2.0f, 2.236068f,  2.828427f,  2.236068f,  2.0f, 2.236068f,  2.0f, 0.0f,
    2.0f, 1.0f, 0.0f, 1.0f, 2.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
    2.0f, 1.0f, 0.0f, 1.0f, 2.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
    2.828427f,  2.236068f,  2.0f, 2.236068f,  2.828427f,  2.236068f,  2.0f, 2.236068f,  2.0f, 0.0f,
    4.472136f,  4.1231055f, 4.0f, 4.1231055f, 4.472136f,  4.1231055f, 4.0f, 4.1231055f, 4.0f, 0.0};

    for(size_t i = 0; i < floatArray->getNumberOfTuples(); i++)
    {
      float computedValue = floatArray->getValue(i);
      float refValue = TJEuclidean[i];
      DREAM3D_COMPARE_FLOATS(&computedValue, &refValue, 1);
    }

    floatArray = am->getAttributeArrayAs<FloatArrayType>("QPEuclideanDistance");
    std::vector<float> QPEuclidean = {
    -1.0f,  -1.0f,  -1.0f,  -1.0f,  -1.0f,  -1.0f,  -1.0f,  -1.0f,  -1.0f,  0.0f,
    -1.0f,  -1.0f,  -1.0f,  -1.0f,  -1.0f,  -1.0f,  -1.0f,  -1.0f,  -1.0f,  0.0f,
    -1.0f,  -1.0f,  -1.0f,  -1.0f,  -1.0f,  -1.0f,  -1.0f,  -1.0f,  -1.0f,  0.0f,
    -1.0f,  -1.0f,  -1.0f,  -1.0f,  -1.0f,  -1.0f,  -1.0f,  -1.0f,  -1.0f,  0.0f,
    -1.0f,  -1.0f,  -1.0f,  -1.0f,  -1.0f,  -1.0f,  -1.0f,  -1.0f,  -1.0f,  0.0f,
    -1.0f,  -1.0f,  -1.0f,  -1.0f,  -1.0f,  -1.0f,  -1.0f,  -1.0f,  -1.0f,  0.0};

    for(size_t i = 0; i < floatArray->getNumberOfTuples(); i++)
    {
      float computedValue = floatArray->getValue(i);
      float refValue = QPEuclidean[i];
      DREAM3D_COMPARE_FLOATS(&computedValue, &refValue, 1);
    }

    return 0;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  int RunTest()
  {
    QVector<size_t> tDims = {10, 6, 1};
    DataContainerArray::Pointer dca = initializeDataContainerArray(tDims);

    QString filtName = "FindEuclideanDistMap";
    FilterManager* fm = FilterManager::Instance();
    IFilterFactory::Pointer factory = fm->getFactoryForFilter(filtName);
    DREAM3D_REQUIRE(factory.get() != nullptr)

    AbstractFilter::Pointer filter = factory->create();
    DREAM3D_REQUIRE(filter.get() != nullptr)

    filter->setDataContainerArray(dca);

    QVariant var;
    var.setValue(k_FeatureIdsArrayPath);
    int err = filter->setProperty("FeatureIdsArrayPath", var);
    DREAM3D_REQUIRE(err >= 0);

    QString boundaryArrayName = "GBEuclideanDistance";
    var.setValue(boundaryArrayName);
    err = filter->setProperty("GBDistancesArrayName", var);
    DREAM3D_REQUIRE(err >= 0);

    bool calcManhattan = false;
    var.setValue(calcManhattan);
    err = filter->setProperty("CalcManhattanDist", var);
    DREAM3D_REQUIRE(err >= 0);


    var.setValue(true);
    err = filter->setProperty("DoTripleLines", var);
    DREAM3D_REQUIRE(err >= 0);
    var.setValue(QString("TJEuclideanDistance"));
    err = filter->setProperty("TJDistancesArrayName", var);
    DREAM3D_REQUIRE(err >= 0);

    var.setValue(true);
    err = filter->setProperty("DoQuadPoints", var);
    DREAM3D_REQUIRE(err >= 0);
    var.setValue(QString("QPEuclideanDistance"));
    err = filter->setProperty("QPDistancesArrayName", var);
    DREAM3D_REQUIRE(err >= 0);

    filter->execute();
    DREAM3D_REQUIRE(filter->getErrorCondition() >= 0);


    //-------------------------------------------
    boundaryArrayName = "GBManhattanDistance";
    var.setValue(boundaryArrayName);
    err = filter->setProperty("GBDistancesArrayName", var);
    DREAM3D_REQUIRE(err >= 0);

    calcManhattan = true;
    var.setValue(calcManhattan);
    err = filter->setProperty("CalcManhattanDist", var);

    var.setValue(true);
    err = filter->setProperty("DoTripleLines", var);
    DREAM3D_REQUIRE(err >= 0);
    var.setValue(QString("TJManhattanDistance"));
    err = filter->setProperty("TJDistancesArrayName", var);
    DREAM3D_REQUIRE(err >= 0);

    var.setValue(true);
    err = filter->setProperty("DoQuadPoints", var);
    DREAM3D_REQUIRE(err >= 0);
    var.setValue(QString("QPManhattanDistance"));
    err = filter->setProperty("QPDistancesArrayName", var);
    DREAM3D_REQUIRE(err >= 0);


    filter->execute();
    DREAM3D_REQUIRE(filter->getErrorCondition() >= 0);


    DataContainerWriter::Pointer writer = DataContainerWriter::New();
    writer->setOutputFile(UnitTest::StatisticsTempDir + QDir::separator() + "FindEuclideanDistMap.dream3d");
    writer->setDataContainerArray(dca);

    writer->execute();
    DREAM3D_REQUIRE(writer->getErrorCondition() >= 0);


    err = validateResults(dca);

    return EXIT_SUCCESS;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void operator()()
  {
    std::cout << "#### FindEuclideanDistMapTest Starting ####" << std::endl;

    int err = EXIT_SUCCESS;
    DREAM3D_REGISTER_TEST(TestFilterAvailability());

    DREAM3D_REGISTER_TEST(RunTest())

    DREAM3D_REGISTER_TEST(RemoveTestFiles())
  }

private:
  FindEuclideanDistMapTest(const FindEuclideanDistMapTest&); // Copy Constructor Not Implemented
  void operator=(const FindEuclideanDistMapTest&);           // Operator '=' Not Implemented
};
