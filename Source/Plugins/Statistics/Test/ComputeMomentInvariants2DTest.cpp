/* ============================================================================
 * Copyright (c) 2017 BlueQuartz Softwae, LLC
 * All rights reserved.
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
 * Neither the names of any of the BlueQuartz Software contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
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
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include <QtCore/QCoreApplication>
#include <QtCore/QFile>

#include "SIMPLib/Common/FilterFactory.hpp"
#include "SIMPLib/Common/FilterManager.h"
#include "SIMPLib/Common/FilterPipeline.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/Geometry/ImageGeom.h"
#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/Plugin/ISIMPLibPlugin.h"
#include "SIMPLib/Plugin/SIMPLibPluginLoader.h"
#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Utilities/QMetaObjectUtilities.h"
#include "SIMPLib/Utilities/UnitTestSupport.hpp"

#include "StatisticsTestFileLocations.h"

#include <Eigen/Dense>

// Directly include the .cpp file instead of the header because of the way the unit
// tests are compiled.
#include "StatisticsFilters/util/MomentInvariants2D.cpp"

class ComputeMomentInvariants2DTest
{

  public:
    ComputeMomentInvariants2DTest()
    {
    }
    virtual ~ComputeMomentInvariants2DTest()
    {
    }

    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    void RemoveTestFiles()
    {
#if REMOVE_TEST_FILES
      QFile::remove(UnitTest::ComputeMomentInvariants2DTest::TestFile1);
      QFile::remove(UnitTest::ComputeMomentInvariants2DTest::TestFile2);
#endif
    }

    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    int TestFilterAvailability()
    {
      // Now instantiate the ComputeMomentInvariants2DTest Filter from the FilterManager
      QString filtName = "ComputeMomentInvariants2D";
      FilterManager* fm = FilterManager::Instance();
      IFilterFactory::Pointer filterFactory = fm->getFactoryForFilter(filtName);
      if(nullptr == filterFactory.get())
      {
        std::stringstream ss;
        ss << "The Statistics Requires the use of the " << filtName.toStdString() << " filter which is found in the Statistics Plugin";
        DREAM3D_TEST_THROW_EXCEPTION(ss.str())
      }
      return 0;
    }

    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    DataContainerArray::Pointer CreateTestData()
    {
      DataContainerArray::Pointer dca = DataContainerArray::New();
      DataContainer::Pointer dc = DataContainer::New("Test");
      dca->addDataContainer(dc);

      ImageGeom::Pointer igeom = ImageGeom::New();
      igeom->setDimensions(5, 5, 1);
      dc->setGeometry(igeom);
      QVector<size_t> dims(3, 0);
      dims[0] = 5;
      dims[1] = 5;
      dims[2] = 1;
      AttributeMatrix::Pointer cellAM = AttributeMatrix::New(dims, "CellData", AttributeMatrix::Type::Cell);
      dc->addAttributeMatrix(cellAM->getName(), cellAM);

      Int32ArrayType::Pointer featureIds = Int32ArrayType::CreateArray(25, "FeatureIds", true);
      featureIds->initializeWithZeros();
      featureIds->setValue(6, 1);
      featureIds->setValue(7, 1);
      featureIds->setValue(8, 1);

      featureIds->setValue(11, 1);
      featureIds->setValue(12, 1);
      featureIds->setValue(13, 1);

      featureIds->setValue(16, 1);
      featureIds->setValue(17, 1);
      featureIds->setValue(18, 1);

      cellAM->addAttributeArray(featureIds->getName(), featureIds);

      dims.resize(1);
      dims[0] = 2;
      AttributeMatrix::Pointer featureAM = AttributeMatrix::New(dims, "FeatureData", AttributeMatrix::Type::CellFeature);
      dc->addAttributeMatrix(featureAM->getName(), featureAM);

#if 0
      0, 0, 0, 0, 0,
      0, 1, 1, 1, 0,
      0, 1, 1, 1, 0,
      0, 1, 1, 1, 0,
      0, 0, 0, 0, 0;
#endif
      QVector<size_t> compDims(1, 6);
      UInt32ArrayType::Pointer rect = UInt32ArrayType::CreateArray(2, compDims, "Rect Coords", true);
      rect->initializeWithZeros();
      rect->setValue(6, 1);
      rect->setValue(7, 1);
      rect->setValue(8, 0);
      rect->setValue(9, 3);
      rect->setValue(10, 3);
      rect->setValue(11, 0);
      featureAM->addAttributeArray(rect->getName(), rect);

      return dca;
    }

    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    int TestComputeMomentInvariants2DTest()
    {

      DataContainerArray::Pointer dca = CreateTestData();

      // Now instantiate the ComputeMomentInvariants2DTest Filter from the FilterManager
      QString filtName = "ComputeMomentInvariants2D";
      FilterManager* fm = FilterManager::Instance();
      IFilterFactory::Pointer filterFactory = fm->getFactoryForFilter(filtName);

      AbstractFilter::Pointer filter = filterFactory->create();
      filter->setDataContainerArray(dca);

      Observer obs;
      obs.connect(filter.get(), SIGNAL(filterGeneratedMessage(const PipelineMessage&)),
                  &obs, SLOT(processPipelineMessage(const PipelineMessage&)));


      DataArrayPath path("Test", "CellData", "FeatureIds");
      QVariant variant;
      variant.setValue(path);

      bool ok = filter->setProperty("FeatureIdsArrayPath", variant);
      DREAM3D_REQUIRE_EQUAL(ok, true)

          path = DataArrayPath("Test", "FeatureData", "Rect Coords");
      variant.setValue(path);
      ok = filter->setProperty("FeatureRectArrayPath", variant);
      DREAM3D_REQUIRE_EQUAL(ok, true)

          bool b = true;
      variant.setValue(b);
      ok = filter->setProperty("NormalizeMomentInvariants", variant);
      DREAM3D_REQUIRE_EQUAL(ok, true)

          path = DataArrayPath("Test", "FeatureData", "Omega1");
      variant.setValue(path);
      ok = filter->setProperty("Omega1ArrayPath", variant);
      DREAM3D_REQUIRE_EQUAL(ok, true)

          path = DataArrayPath("Test", "FeatureData", "Omega2");
      variant.setValue(path);
      ok = filter->setProperty("Omega2ArrayPath", variant);
      DREAM3D_REQUIRE_EQUAL(ok, true)

          filter->preflight();
      int err = filter->getErrorCondition();
      DREAM3D_REQUIRE( err >= 0)

      dca = CreateTestData();
      filter->setDataContainerArray(dca);

      filter->execute();
      err = filter->getErrorCondition();
      DREAM3D_REQUIRE(err >= 0)

      AttributeMatrix::Pointer featureAM = dca->getAttributeMatrix(DataArrayPath("Test", "FeatureData", ""));
      FloatArrayType::Pointer omega1 = featureAM->getAttributeArrayAs<FloatArrayType>("Omega1");
      FloatArrayType::Pointer omega2 = featureAM->getAttributeArrayAs<FloatArrayType>("Omega2");

      float value = omega1->getValue(1);
      DREAM3D_REQUIRE(std::abs(value - 0.95493) < 0.00001)

          value = omega2->getValue(1);
      DREAM3D_REQUIRE(std::abs(value - 0.911891) < 0.00001)

          return EXIT_SUCCESS;
    }

    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    void TestBinomial()
    {
      MomentInvariants2D moments;
      size_t max_order = 2;

      MomentInvariants2D::DoubleMatrixType binomial = moments.binomial(max_order);

      DREAM3D_REQUIRE_EQUAL(binomial(0,0), 1.0)
          DREAM3D_REQUIRE_EQUAL(binomial(0,1), 1.0)
          DREAM3D_REQUIRE_EQUAL(binomial(0,2), 1.0)

          DREAM3D_REQUIRE_EQUAL(binomial(1,0), 1.0)
          DREAM3D_REQUIRE_EQUAL(binomial(1,1), 1.0)
          DREAM3D_REQUIRE_EQUAL(binomial(1,2), 2.0)

          DREAM3D_REQUIRE_EQUAL(binomial(2,0), 1.0)
          DREAM3D_REQUIRE_EQUAL(binomial(2,1), 2.0)
          DREAM3D_REQUIRE_EQUAL(binomial(2,2), 1.0)
    }

    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    void TestBigX()
    {
      MomentInvariants2D moments;
      size_t max_order = 2;
      size_t dim = 16;
      MomentInvariants2D::DoubleMatrixType bigX = moments.getBigX(max_order, dim);
      //    std::cout << "bigX=" << bigX.rows() << "," << bigX.cols() << std::endl;
      //    std::cout << bigX << std::endl;

      MomentInvariants2D::DoubleMatrixType ideal(16, 3);
      ideal << 0.133333, -0.142222, 0.151901,
          0.133333, -0.124444, 0.116346,
          0.133333, -0.106667, 0.0855309,
          0.133333, -0.0888889, 0.0594568,
          0.133333, -0.0711111, 0.0381235,
          0.133333, -0.0533333, 0.0215309,
          0.133333, -0.0355556, 0.00967901,
          0.133333, -0.0177778, 0.0025679,
          0.133333, 0.0000000, 0.000197531,
          0.133333, 0.0177778, 0.0025679,
          0.133333, 0.0355556, 0.00967901,
          0.133333, 0.0533333, 0.0215309,
          0.133333, 0.0711111, 0.0381235,
          0.133333, 0.0888889, 0.0594568,
          0.133333, 0.106667, 0.0855309,
          0.133333, 0.124444, 0.116346;

      MomentInvariants2D::DoubleMatrixType diff = bigX - ideal;

      double maxCoeff = diff.maxCoeff();
      DREAM3D_REQUIRE(maxCoeff < 0.000001)

    }

    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    void TestComputeMoments2D()
    {
      MomentInvariants2D moments;
      size_t max_order = 2;
      //size_t mDim = max_order + 1;
      size_t imageDim = 5; // The algorithm takes a square image
      //  std::vector<double> bigx;
      //  moments.getBigX(max_order, imageDim, bigx);

      size_t inputDims[2] = {imageDim, imageDim};


      // std::cout << "Eigen  Methods +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;

      MomentInvariants2D::DoubleMatrixType input2D(5, 5);
      input2D <<  0, 0, 0, 0, 0,
          /*Row*/ 0, 1, 1, 1, 0,
          /*Row*/ 0, 1, 1, 1, 0,
          /*Row*/ 0, 1, 1, 1, 0,
          /*Row*/ 0, 0, 0, 0, 0;
      MomentInvariants2D::DoubleMatrixType centralMoments = moments.computeMomentInvariants(input2D, inputDims, max_order);
      //std::cout << "Central Moments=\n" << m2D << std::endl;
      // compute the second order moment invariants
      MomentInvariants2D::DoubleMatrixType idealCentralMoments(3,3);
      idealCentralMoments <<  9.0, 0.0, 6.75,
          0.0,      0.0,      0.0,
          6.75,      0.0, 5.0625;
      MomentInvariants2D::DoubleMatrixType diff = centralMoments - idealCentralMoments;
      double maxCoeff = diff.maxCoeff();
      DREAM3D_REQUIRE(maxCoeff < 0.000001)

          double omega1 = 2.0 * (centralMoments(0, 0) * centralMoments(0, 0)) / (centralMoments(0, 2) + centralMoments(2, 0));
      double omega2 = std::pow(centralMoments(0, 0), 4) / (centralMoments(2, 0) * centralMoments(0, 2) - std::pow(centralMoments(1, 1), 2));
      //std::cout << ",'2D moment invariants : " << omega1 << "\t" << omega2 << " (should be 12 and 144)" << std::endl;

      DREAM3D_REQUIRE_EQUAL(omega1, 12.0)
          DREAM3D_REQUIRE_EQUAL(omega2, 144.0)

          // normalize the invariants by those of the circle
          double circle_omega[2] = {4.0 * M_PI, 16.0 * M_PI * M_PI};
      omega1 /= circle_omega[0];
      omega2 /= circle_omega[1];

      DREAM3D_REQUIRE(std::abs(omega1 - 0.95493) < 0.00001)
          DREAM3D_REQUIRE(std::abs(omega2 - 0.911891) < 0.00001)

          // std::cout << "normalized moment invariants: " << omega1 << "\t" << omega2 << std::endl;

    }

    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    void operator()()
    {
      int err = EXIT_SUCCESS;

      DREAM3D_REGISTER_TEST(TestBinomial());
      DREAM3D_REGISTER_TEST(TestBigX());
      DREAM3D_REGISTER_TEST(TestComputeMoments2D());

      DREAM3D_REGISTER_TEST(TestFilterAvailability());

      DREAM3D_REGISTER_TEST(TestComputeMomentInvariants2DTest());

      DREAM3D_REGISTER_TEST(RemoveTestFiles());
    }

  private:
    ComputeMomentInvariants2DTest(const ComputeMomentInvariants2DTest&); // Copy Constructor Not Implemented
    void operator=(const ComputeMomentInvariants2DTest&);       // Operator '=' Not Implemented
};
