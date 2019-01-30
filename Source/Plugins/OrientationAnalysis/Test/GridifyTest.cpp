/* ============================================================================
 * Copyright (c) 2019 BlueQuartz Software, LLC
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
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS “AS IS”
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

#pragma once

#include <QtCore/QCoreApplication>
#include <QtCore/QFile>
#include <unordered_map>

#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/Filtering/FilterFactory.hpp"
#include "SIMPLib/Filtering/FilterManager.h"
#include "SIMPLib/Filtering/FilterPipeline.h"
#include "SIMPLib/Plugin/ISIMPLibPlugin.h"
#include "SIMPLib/Plugin/SIMPLibPluginLoader.h"
#include "SIMPLib/SIMPLib.h"

#include "SIMPLib/Filtering/QMetaObjectUtilities.h"
#include "SIMPLib/Geometry/ImageGeom.h"
#include "SIMPLib/Geometry/TriangleGeom.h"

#include "UnitTestSupport.hpp"
//#include "OrientationAnalysisTestFileLocations.h"

class GridifyTest
{
  const QString m_filterName{"Gridify"};
  const QString m_amName{"Grid"};
  const QString m_aaName{"Layout"};
  const DataContainerArray::Pointer m_dca;
  AbstractFilter::Pointer m_filter{nullptr};

  // Test validation objects
  std::unordered_map<std::string, std::vector<uint16_t>> checkValues{
      {"I",   {0, 0, 0}},
      {"II",  {0, 2, 0}},
      {"III", {3, 2, 0}},
      {"IV",  {2, 0, 0}},
      {"V",   {1, 3, 0}},
      {"VI",  {1, 1, 1}},
      {"VII", {3, 3, 2}},
  };

  int ValidateData()
  {
    int wrongData{0};
    for(const auto& eachDC : m_dca->getDataContainers())
    {
      if (!eachDC->doesAttributeMatrixExist(m_amName))
      {
        continue;
      }
      uint16_t xTestComp{eachDC->getAttributeMatrix(m_amName)
            ->getAttributeArrayAs<UInt16ArrayType>(m_aaName)
            ->getValue(0)};
      uint16_t yTestComp{eachDC->getAttributeMatrix(m_amName)
            ->getAttributeArrayAs<UInt16ArrayType>(m_aaName)
            ->getValue(1)};
      uint16_t zTestComp{eachDC->getAttributeMatrix(m_amName)
            ->getAttributeArrayAs<UInt16ArrayType>(m_aaName)
            ->getValue(2)};
      uint16_t xCheckComp{checkValues[eachDC->getName().toStdString()][0]};
      uint16_t yCheckComp{checkValues[eachDC->getName().toStdString()][1]};
      uint16_t zCheckComp{checkValues[eachDC->getName().toStdString()][2]};
      if (!((xCheckComp == xTestComp) && (yCheckComp == yTestComp) && (zCheckComp == zTestComp)))
      {
        wrongData++;
      }
    }

    return wrongData;
  }

  int RemoveTestFiles()
  {
#if REMOVE_TEST_FILES
    //    QFile::remove(UnitTest::GridifyTest::TestFile1);
    //    QFile::remove(UnitTest::GridifyTest::TestFile2);
#endif
    return 0;
  }

  int SetUp()
  {
    return 0;
  }

  int RunTest()
  {
    m_filter->execute();
    int erred{m_filter->getErrorCondition()};
    DREAM3D_REQUIRE_EQUAL(erred, 0)
    int dataInvalidated{ValidateData()};
    DREAM3D_REQUIRE_EQUAL(dataInvalidated, 0)

    return erred + dataInvalidated;
  }

  int TearDown()
  {
    DREAM3D_REQUIRE_EQUAL(RemoveTestFiles(), 0)
    return 0;
  }

public:
  GridifyTest() : m_dca{DataContainerArray::New()}
  {
    IFilterFactory::Pointer filterFactory{FilterManager::Instance()->getFactoryFromClassName(m_filterName)};
    DREAM3D_REQUIRE(filterFactory.get() != nullptr);

    m_filter = filterFactory->create();
    DREAM3D_REQUIRE(m_filter.get() != nullptr);

    // Create geometries
    ImageGeom::Pointer dcR0C0Geometry{ImageGeom::New()};
    ImageGeom::Pointer dcR0C2Geometry{ImageGeom::New()};
    ImageGeom::Pointer dcR1C1Geometry{ImageGeom::New()};
    ImageGeom::Pointer dcR1C3Geometry{ImageGeom::New()};
    ImageGeom::Pointer dcR2C0Geometry{ImageGeom::New()};
    ImageGeom::Pointer dcR3C2Geometry{ImageGeom::New()};
    ImageGeom::Pointer dcR3C3Geometry{ImageGeom::New()};
    TriangleGeom::Pointer dcTriangleGeometry{TriangleGeom::New()};

    // Assign layouts to each image
    dcR0C0Geometry->setOrigin(0.0f, 0.0f, 0.0f);
    dcR0C0Geometry->setDimensions(410, 410, 0);
    dcR0C0Geometry->setResolution(1.0f, 1.0f, 1.0f);

    dcR0C2Geometry->setOrigin(430.0f, 0.0f, 0.0f);
    dcR0C2Geometry->setDimensions(400, 930, 0);
    dcR0C2Geometry->setResolution(1.0f, 1.0f, 1.0f);

    dcR1C1Geometry->setOrigin(400.0f, 400.0f, 100.0f);
    dcR1C1Geometry->setDimensions(40, 40, 0);
    dcR1C1Geometry->setResolution(1.0f, 1.0f, 1.0f);

    dcR1C3Geometry->setOrigin(830.0f, 400.0f, 0.0f);
    dcR1C3Geometry->setDimensions(500, 400, 0);
    dcR1C3Geometry->setResolution(1.0f, 1.0f, 1.0f);

    dcR2C0Geometry->setOrigin(0.0f, 430.0f, 0.0f);
    dcR2C0Geometry->setDimensions(400, 400, 0);
    dcR2C0Geometry->setResolution(1.0f, 1.0f, 1.0f);

    dcR3C2Geometry->setOrigin(430.0f, 830.0f, 0.0f);
    dcR3C2Geometry->setDimensions(1200, 400, 0);
    dcR3C2Geometry->setResolution(1.0f, 1.0f, 1.0f);

    dcR3C3Geometry->setOrigin(830.0f, 830.0f, 200.0f);
    dcR3C3Geometry->setDimensions(400, 400, 0);
    dcR3C3Geometry->setResolution(1.0f, 1.0f, 1.0f);

    // Assign image geometry to each data container
    DataContainer::Pointer dcI{DataContainer::New("I")};
    DataContainer::Pointer dcII{DataContainer::New("II")};
    DataContainer::Pointer dcIII{DataContainer::New("III")};
    DataContainer::Pointer dcIV{DataContainer::New("IV")};
    DataContainer::Pointer dcV{DataContainer::New("V")};
    DataContainer::Pointer dcVI{DataContainer::New("VI")};
    DataContainer::Pointer dcVII{DataContainer::New("VII")};
    DataContainer::Pointer dcTriangle{DataContainer::New("Triangle")};
    dcI->setGeometry(dcR0C0Geometry);
    dcII->setGeometry(dcR0C2Geometry);
    dcIII->setGeometry(dcR3C2Geometry);
    dcIV->setGeometry(dcR2C0Geometry);
    dcV->setGeometry(dcR1C3Geometry);
    dcVI->setGeometry(dcR1C1Geometry);
    dcVII->setGeometry(dcR3C3Geometry);
    dcTriangle->setGeometry(dcTriangleGeometry);

    // Assign data containers to dca
    m_dca->addDataContainer(dcI);
    m_dca->addDataContainer(dcII);
    m_dca->addDataContainer(dcIII);
    m_dca->addDataContainer(dcIV);
    m_dca->addDataContainer(dcV);
    m_dca->addDataContainer(dcVI);
    m_dca->addDataContainer(dcVII);
    m_dca->addDataContainer(dcTriangle);

    // Assign data container array to the filter
    m_filter->setDataContainerArray(m_dca);
  }
  GridifyTest(const GridifyTest&) = delete;            // Copy Constructor
  GridifyTest(GridifyTest&&) = delete;                 // Move Constructor
  GridifyTest& operator=(const GridifyTest&) = delete; // Copy Assignment
  GridifyTest& operator=(GridifyTest&&) = delete;      // Move Assignment
  ~GridifyTest() = default;

  void operator()()
  {
    int err = EXIT_SUCCESS;
    DREAM3D_REGISTER_TEST(SetUp());
    DREAM3D_REGISTER_TEST(RunTest())
    DREAM3D_REGISTER_TEST(TearDown())
  }
};
