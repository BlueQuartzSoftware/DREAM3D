#pragma once
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
 * SERVICES; LOSS OF USE, Data, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
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

#include <QTextStream>
#include <QtCore/QCoreApplication>
#include <QtCore/QFile>

#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/FilterParameters/FloatVec3FilterParameter.h"
#include "SIMPLib/Filtering/FilterFactory.hpp"
#include "SIMPLib/Filtering/FilterManager.h"
#include "SIMPLib/Filtering/FilterPipeline.h"
#include "SIMPLib/Geometry/EdgeGeom.h"
#include "SIMPLib/Geometry/ImageGeom.h"
#include "SIMPLib/Geometry/TriangleGeom.h"
#include "SIMPLib/Geometry/VertexGeom.h"
#include "SIMPLib/Plugin/ISIMPLibPlugin.h"
#include "SIMPLib/Plugin/SIMPLibPluginLoader.h"
#include "SIMPLib/SIMPLib.h"

#include "SIMPLib/Filtering/QMetaObjectUtilities.h"
#include <tbb/parallel_for.h>

#include "ProcessingTestFileLocations.h"
#include "UnitTestSupport.hpp"

class ConvertColorToGrayScaleTest
{
  const FloatVec3_t m_defaultWeights{0.2125f, 0.7154f, 0.0721f};
  const bool m_createNewAM{false};
  const QString m_filtName{"ConvertColorToGrayScale"};
  const QString m_outputArrayPrefix{"grayTestImage"};
  const QString m_outputAMName{"grayTestAM"};
  AbstractFilter::Pointer m_colorToGrayscaleFilter;
  DataContainerArray::Pointer m_dca;

  enum Algorithms
  {
    LUMINOSITY,
    AVERAGE,
    LIGHTNESS,
    SINGLE_CHANNEL
  };

  const std::vector<std::vector<uint8_t>> testColors{
      {0, 0, 0},       // black
      {0, 0, 128},     // navy
      {0, 0, 255},     // blue
      {128, 0, 0},     // maroon
      {128, 0, 128},   // purple
      {255, 0, 0},     // red
      {255, 0, 255},   // fuchsia
      {0, 128, 0},     // green
      {0, 128, 128},   // teal
      {128, 128, 0},   // olive
      {128, 128, 128}, // gray
      {0, 255, 0},     // lime
      {192, 192, 192}, // silver
      {0, 255, 255},   // aqua
      {255, 255, 0},   // yellow
      {255, 255, 255}, // white
  };
  const std::vector<uint8_t> checkDefaultLuminosityColors{
      0,   // black
      9,   // navy
      18,  // blue
      27,  // maroon
      36,  // purple
      54,  // red
      73,  // fuchsia
      92,  // green
      101, // teal
      119, // olive
      128, // gray
      182, // lime
      192, // silver
      201, // aqua
      237, // yellow
      255, // white
  };
  const std::vector<uint8_t> checkCustomLuminosityColors{
      0,   // black
      96,  // navy
      191, // blue
      96,  // maroon
      192, // purple
      191, // red
      127, // fuchsia
      96,  // green
      192, // teal
      192, // olive
      32,  // gray
      191, // lime
      176, // silver
      127, // aqua
      127, // yellow
      62,  // white
  };
  const std::vector<uint8_t> checkLessZeroLuminosityColors{
      0,   // black
      160, // navy
      65,  // blue
      160, // maroon
      64,  // purple
      65,  // red
      129, // fuchsia
      160, // green
      64,  // teal
      64,  // olive
      224, // gray
      65,  // lime
      80,  // silver
      129, // aqua
      129, // yellow
      194, // white
  };
  const std::vector<uint8_t> checkGreaterOneLuminosityColors{
      0,   // black
      224, // navy
      190, // blue
      224, // maroon
      192, // purple
      190, // red
      125, // fuchsia
      224, // green
      192, // teal
      192, // olive
      160, // gray
      190, // lime
      240, // silver
      125, // aqua
      125, // yellow
      59,  // white
  };
  const std::vector<uint8_t> checkLessNegativeOneLuminosityColors{
      0,   // black
      32,  // navy
      66,  // blue
      32,  // maroon
      64,  // purple
      66,  // red
      131, // fuchsia
      32,  // green
      64,  // teal
      64,  // olive
      96,  // gray
      66,  // lime
      16,  // silver
      131, // aqua
      131, // yellow
      197, // white
  };
  const std::vector<uint8_t> checkAverageColors{
      0,   // black
      43,  // navy
      85,  // blue
      43,  // maroon
      85,  // purple
      85,  // red
      170, // fuchsia
      43,  // green
      85,  // teal
      85,  // olive
      128, // gray
      85,  // lime
      192, // silver
      170, // aqua
      170, // yellow
      255, // white
  };
  const std::vector<uint8_t> checkLightnessColors{
      0,   // black
      64,  // navy
      128, // blue
      64,  // maroon
      64,  // purple
      128, // red
      128, // fuchsia
      64,  // green
      64,  // teal
      64,  // olive
      128, // gray
      128, // lime
      192, // silver
      128, // aqua
      128, // yellow
      255, // white
  };
  const std::vector<uint8_t> checkRChannelColors{
      0,   // black
      0,   // navy
      0,   // blue
      128, // maroon
      128, // purple
      255, // red
      255, // fuchsia
      0,   // green
      0,   // teal
      128, // olive
      128, // gray
      0,   // lime
      192, // silver
      0,   // aqua
      255, // yellow
      255, // white
  };
  const std::vector<uint8_t> checkGChannelColors{
      0,   // black
      0,   // navy
      0,   // blue
      0,   // maroon
      0,   // purple
      0,   // red
      0,   // fuchsia
      128, // green
      128, // teal
      128, // olive
      128, // gray
      255, // lime
      192, // silver
      255, // aqua
      255, // yellow
      255, // white
  };
  const std::vector<uint8_t> checkBChannelColors{
      0,   // black
      128, // navy
      255, // blue
      0,   // maroon
      128, // purple
      0,   // red
      255, // fuchsia
      0,   // green
      128, // teal
      0,   // olive
      128, // gray
      0,   // lime
      192, // silver
      255, // aqua
      0,   // yellow
      255, // white
  };

  const std::vector<std::vector<uint8_t>> algorithmMap{checkDefaultLuminosityColors,
                                                       checkCustomLuminosityColors,
                                                       checkLessZeroLuminosityColors,
                                                       checkGreaterOneLuminosityColors,
                                                       checkLessNegativeOneLuminosityColors,
                                                       checkAverageColors,
                                                       checkLightnessColors,
                                                       checkRChannelColors,
                                                       checkGChannelColors,
                                                       checkBChannelColors};

  void SetDataArrayTestValues(DataArray<uint8_t>::Pointer& aa)
  {
    aa->initializeWithValue(0);
    size_t index{0};
    for(const auto& eachColor : testColors)
    {
      aa->setTuple(index++, eachColor);
    }
  }

  static DataContainer::Pointer createVertexGeometryDataContainer(const DataArray<uint8_t>::Pointer& aa, const QVector<size_t>& tDims)
  {
    AttributeMatrix::Pointer am{AttributeMatrix::New(tDims, SIMPL::Defaults::VertexAttributeMatrixName, AttributeMatrix::Type::Vertex)};
    am->addAttributeArray(aa->getName(), aa);

    DataContainer::Pointer dc = DataContainer::New(SIMPL::Defaults::VertexDataContainerName);
    dc->addAttributeMatrix(SIMPL::Defaults::VertexAttributeMatrixName, am);
    dc->setGeometry(VertexGeom::CreateGeometry(static_cast<int64_t>(aa->getNumberOfTuples()), SIMPL::Geometry::VertexGeometry));
    return dc;
  }

  int CheckFilterParameters(const QVariant& algorithm, const FloatVec3_t& cws, const uint8_t& cc)
  {
    QVariant conversionAlgorithm{m_colorToGrayscaleFilter->property("ConversionAlgorithm")};
    FloatVec3_t colorWeights{(m_colorToGrayscaleFilter->property("ColorWeights").value<FloatVec3_t>())};
    QVariant outputArrayPrefix{m_colorToGrayscaleFilter->property("OutputArrayPrefix")};
    QVariant colorChannel{m_colorToGrayscaleFilter->property("ColorChannel")};
    QVariant createNewAM{m_colorToGrayscaleFilter->property("CreateNewAttributeMatrix")};
    QVariant outputAMName{m_colorToGrayscaleFilter->property("OutputAttributeMatrixName")};

    int wrongParameters{0};
    wrongParameters += (conversionAlgorithm == algorithm) ? 0 : 1;
    float epsilon{0.000000001f};
    wrongParameters += (abs(colorWeights.x - cws.x) < epsilon && abs(colorWeights.y - cws.y) < epsilon && abs(colorWeights.z - cws.z) < epsilon) ? 0 : 1;
    wrongParameters += (outputArrayPrefix == m_outputArrayPrefix) ? 0 : 1;
    wrongParameters += (colorChannel == cc) ? 0 : 1;
    wrongParameters += (createNewAM == m_createNewAM) ? 0 : 1;
    wrongParameters += (outputAMName == m_outputAMName) ? 0 : 1;

    return wrongParameters;
  }

  int CompareResults(const uint8_t& algoMapIndex) const
  {
    QString amName{m_outputAMName};
    DataContainer::Pointer dc{m_dca->getDataContainers().first()};
    if(!m_createNewAM)
      amName = dc->getAttributeMatrices().first()->getName();
    AttributeMatrix::Pointer am{dc->getAttributeMatrix(amName)};

    UCharArrayType::Pointer testArray{};
    for(const auto& eachAAName : am->getAttributeArrayNames())
    {
      if(eachAAName.contains(m_outputArrayPrefix))
        testArray = am->getAttributeArrayAs<UCharArrayType>(eachAAName);
    }

    std::vector<uint8_t> colorArray{algorithmMap[algoMapIndex]};

    int wrongValues{0};
    tbb::parallel_for(tbb::blocked_range<size_t>(0, colorArray.size()), [&](const tbb::blocked_range<size_t>& r) {
      for(size_t index = r.begin(); index < r.end(); ++index)
      {
        uint8_t testValue{testArray->getValue(index)};
        uint8_t checkValue{colorArray[index]};
        wrongValues += (testValue == checkValue) ? 0 : 1;
      }
    });
    return wrongValues;
  }

  /**
   @brief Creates the input data array path from the data container and
   assigns it, the passed data container, and any other relevant parameters
   to the filter
   @details The DCA can hold multiple geometries; as long as the data container
   of the desired geometry is passed in here this method will sort out the input
   data array path and should work as expected
   */
  void SetUp(const QVariant& algorithm, const DataContainer::Pointer& dc, const FloatVec3_t& colorWeights = {0.2125f, 0.7154f, 0.0721f}, const uint8_t& colorChannel = 0)
  {
    m_dca->addDataContainer(dc);
    QVector<DataArrayPath> daps{};
    for(const AttributeMatrix::Pointer& eachAM : dc->getAttributeMatrices())
    {
      QString amName{eachAM->getName()};
      for(const QString& eachAAName : eachAM->getAttributeArrayNames())
      {
        // Make sure not to add an output AA
        if(!eachAAName.contains(m_outputArrayPrefix))
          daps.append(DataArrayPath(dc->getName(), amName, eachAAName));
      }
    }

    QVariant dataArrayVector{};
    dataArrayVector.setValue(daps);

    QVariant cw;
    cw.setValue(colorWeights);

    m_colorToGrayscaleFilter->setProperty("ConversionAlgorithm", algorithm);
    m_colorToGrayscaleFilter->setProperty("OutputArrayPrefix", static_cast<QVariant>(m_outputArrayPrefix));
    m_colorToGrayscaleFilter->setProperty("ColorWeights", cw);
    m_colorToGrayscaleFilter->setProperty("ColorChannel", colorChannel);
    m_colorToGrayscaleFilter->setProperty("CreateNewAttributeMatrix", m_createNewAM);
    m_colorToGrayscaleFilter->setProperty("OutputAttributeMatrixName", static_cast<QVariant>(m_outputAMName));
    m_colorToGrayscaleFilter->setProperty("InputDataArrayVector", dataArrayVector);
    m_colorToGrayscaleFilter->setDataContainerArray(m_dca);

    int wrongParameters{CheckFilterParameters(algorithm, colorWeights, colorChannel)};
    DREAM3D_REQUIRE_EQUAL(0, wrongParameters)
  }

  void SetUp(const QVariant& algorithm, const DataContainer::Pointer& dc, const uint8_t& colorChannel)
  {
    SetUp(algorithm, dc, m_defaultWeights, colorChannel);
  }

  int RunTest(const uint8_t& algoMapIndex)
  {
    m_colorToGrayscaleFilter->execute();
    int erred{m_colorToGrayscaleFilter->getErrorCondition()};
    DREAM3D_REQUIRE_EQUAL(erred, 0);
    int wrongValues{CompareResults(algoMapIndex)};
    DREAM3D_REQUIRE_EQUAL(wrongValues, 0)
    return erred + wrongValues;
  }

  /**
   * @brief Resets the environment for another test
   */
  void TearDown(const DataContainer::Pointer& dc) const
  {
    for(const auto& eachDC : m_dca->getDataContainers())
    {
      for(const auto& eachAM : eachDC->getAttributeMatrices())
      {
        for(const auto& eachAAName : eachAM->getAttributeArrayNames())
        {
          if(eachAAName.contains(m_outputArrayPrefix))
          {
            eachAM->getAttributeArray(eachAAName)->releaseOwnership();
            eachAM->removeAttributeArray(eachAAName);
            bool doesAAStillExist{m_dca->doesAttributeArrayExist(DataArrayPath(eachDC->getName(), eachAM->getName(), eachAAName))};
            DREAM3D_REQUIRE_EQUAL(false, doesAAStillExist)
          }
        }
        eachDC->removeAttributeMatrix(m_outputAMName);
        bool doesAMStillExist{eachDC->doesAttributeMatrixExist(m_outputAMName)};
        DREAM3D_REQUIRE_EQUAL(false, doesAMStillExist)
      }
    }

    m_dca->removeDataContainer(dc->getName());

#if REMOVE_TEST_FILES
//    QFile::remove(UnitTest::ConvertColorToGrayScale::TestOutputPath);
#endif
  }

public:
  ConvertColorToGrayScaleTest(const ConvertColorToGrayScaleTest&) = delete;            // Copy Constructor
  ConvertColorToGrayScaleTest(ConvertColorToGrayScaleTest&&) = delete;                 // Move Constructor
  ConvertColorToGrayScaleTest& operator=(const ConvertColorToGrayScaleTest&) = delete; // Copy Assignment
  ConvertColorToGrayScaleTest& operator=(ConvertColorToGrayScaleTest&&) = delete;      // Move Assignment

  ConvertColorToGrayScaleTest()
  : m_dca{DataContainerArray::New()}
  {
    IFilterFactory::Pointer colorToGrayscaleFactory{FilterManager::Instance()->getFactoryFromClassName(m_filtName)};
    DREAM3D_REQUIRE(colorToGrayscaleFactory.get() != nullptr);

    m_colorToGrayscaleFilter = colorToGrayscaleFactory->create();
    DREAM3D_REQUIRE(m_colorToGrayscaleFilter.get() != nullptr);
  }
  ~ConvertColorToGrayScaleTest() = default;

  void operator()()
  {
    int err = 0;

    const QString aaName{SIMPL::VertexData::SurfaceMeshNodes};
    const QVector<size_t> tDims{16};
    const QVector<size_t> cDims{3, 1, 1};

    DataArray<uint8_t>::Pointer testAA{DataArray<uint8_t>::CreateArray(tDims, cDims, aaName)};
    SetDataArrayTestValues(testAA);

    // NOTE: This filter has no geometry requirements
    DataContainer::Pointer vertexDC{createVertexGeometryDataContainer(testAA, tDims)};

    // Luminosity Algorithm testing
    // Test defaults
    qDebug() << "Testing luminosity algorithm...";
    qDebug() << "Default weights (0.2125, 0.7154, 0.0721)...";
    SetUp(Algorithms::LUMINOSITY, vertexDC);
    DREAM3D_REGISTER_TEST(RunTest(0))
    TearDown(vertexDC);

    // Test custom
    FloatVec3_t colorWeights{0.75, 0.75, 0.75};
    qDebug() << "Custom weights (0.75, 0.75, 0.75)...";
    SetUp(Algorithms::LUMINOSITY, vertexDC, colorWeights);
    DREAM3D_REGISTER_TEST(RunTest(1))
    TearDown(vertexDC);

    // Test <0
    colorWeights = {-0.75, -0.75, -0.75};
    qDebug() << "Testing weights < 0 (-0.75, -0.75, -0.75)...";
    SetUp(Algorithms::LUMINOSITY, vertexDC, colorWeights);
    DREAM3D_REGISTER_TEST(RunTest(2))
    TearDown(vertexDC);

    // Test >1
    colorWeights = {1.75, 1.75, 1.75};
    qDebug() << "Testing weights > 1 (1.75, 1.75, 1.75)...";
    SetUp(Algorithms::LUMINOSITY, vertexDC, colorWeights);
    DREAM3D_REGISTER_TEST(RunTest(3))
    TearDown(vertexDC);

    // Test <-1
    colorWeights = {-1.75, -1.75, -1.75};
    qDebug() << "Testing weights < -1 (-1.75, -1.75, -1.75)...";
    SetUp(Algorithms::LUMINOSITY, vertexDC, colorWeights);
    DREAM3D_REGISTER_TEST(RunTest(4))
    TearDown(vertexDC);

    // Average Algorithm testing
    qDebug() << "Testing average algorithm...";
    SetUp(Algorithms::AVERAGE, vertexDC);
    DREAM3D_REGISTER_TEST(RunTest(5))
    TearDown(vertexDC);

    // Lightness Algorithm testing
    qDebug() << "Testing lightness algorithm...";
    SetUp(Algorithms::LIGHTNESS, vertexDC);
    DREAM3D_REGISTER_TEST(RunTest(6))
    TearDown(vertexDC);

    // Single Channel Algorithm testing
    // Red channel
    qDebug() << "Testing red channel algorithm...";
    SetUp(Algorithms::SINGLE_CHANNEL, vertexDC);
    DREAM3D_REGISTER_TEST(RunTest(7))
    TearDown(vertexDC);

    // Green channel
    qDebug() << "Testing green channel algorithm...";
    SetUp(Algorithms::SINGLE_CHANNEL, vertexDC, 1);
    DREAM3D_REGISTER_TEST(RunTest(8))
    TearDown(vertexDC);

    // Blue channel
    qDebug() << "Testing blue channel algorithm...";
    SetUp(Algorithms::SINGLE_CHANNEL, vertexDC, 2);
    DREAM3D_REGISTER_TEST(RunTest(9))
    TearDown(vertexDC);
  }
};
