/* ============================================================================
 * Copyright (c) 2019-2019 BlueQuartz Software, LLC
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

#include <QtCore/QFile>

#include "SIMPLib/CoreFilters/DataContainerReader.h"
#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/Filtering/FilterFactory.hpp"
#include "SIMPLib/Filtering/FilterManager.h"
#include "SIMPLib/Filtering/FilterPipeline.h"
#include "SIMPLib/Filtering/QMetaObjectUtilities.h"
#include "SIMPLib/Plugin/ISIMPLibPlugin.h"
#include "SIMPLib/Plugin/SIMPLibPluginLoader.h"
#include "SIMPLib/SIMPLib.h"
#include "UnitTestSupport.hpp"

#include "SamplingTestFileLocations.h"

#include "SIMPLib/FilterParameters/DynamicTableData.h"
#include "SIMPLib/FilterParameters/FloatVec3FilterParameter.h"

class RotateSampleRefFrameTest
{
private:
  const QString m_FilterName = "RotateSampleRefFrame";

  static void resetGeometry(AttributeMatrix::Pointer matrix, ImageGeom::Pointer imageGeom, const std::vector<size_t>& tDims)
  {
    matrix->resizeAttributeArrays(tDims);
    imageGeom->setDimensions(tDims);
    imageGeom->setOrigin(0.0f, 0.0f, 0.0f);
    imageGeom->setSpacing(1.0f, 1.0f, 1.0f);
  }

public:
  RotateSampleRefFrameTest() = default;
  ~RotateSampleRefFrameTest() = default;
  RotateSampleRefFrameTest(const RotateSampleRefFrameTest&) = delete;            // Copy Constructor Not Implemented
  RotateSampleRefFrameTest(RotateSampleRefFrameTest&&) = delete;                 // Move Constructor Not Implemented
  RotateSampleRefFrameTest& operator=(const RotateSampleRefFrameTest&) = delete; // Copy Assignment Not Implemented
  RotateSampleRefFrameTest& operator=(RotateSampleRefFrameTest&&) = delete;      // Move Assignment Not Implemented

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void RemoveTestFiles()
  {
#if REMOVE_TEST_FILES
    QFile::remove(UnitTest::RotateSampleRefFrameTest::TestFile1);
    QFile::remove(UnitTest::RotateSampleRefFrameTest::TestFile2);
#endif
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  int TestFilterAvailability()
  {
    // Now instantiate the SampleSurfaceMeshSpecifiedPointsTest Filter from the FilterManager
    FilterManager* filterManager = FilterManager::Instance();
    IFilterFactory::Pointer filterFactory = filterManager->getFactoryFromClassName(m_FilterName);
    if(filterFactory == nullptr)
    {
      std::stringstream ss;
      ss << "The RotateSampleRefFrameTest Requires the use of the " << m_FilterName.toStdString() << " filter which is found in the Sampling Plugin";
      DREAM3D_TEST_THROW_EXCEPTION(ss.str())
    }
    return 0;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestFilterParameters()
  {
    const std::vector<size_t> tDims{10, 20, 30};
    const DataArrayPath path("DataContainer", "AttributeMatrix", "DataArray");

    DataContainerArray::Pointer dca = DataContainerArray::New();

    DataContainer::Pointer dc = DataContainer::New(path.getDataContainerName());

    ImageGeom::Pointer imageGeom = ImageGeom::New();

    imageGeom->setDimensions(tDims);

    dc->setGeometry(imageGeom);

    dca->addOrReplaceDataContainer(dc);

    AttributeMatrix::Pointer matrix = dc->createNonPrereqAttributeMatrix(nullptr, path.getAttributeMatrixName(), tDims, AttributeMatrix::Type::Cell);

    FilterManager* fm = FilterManager::Instance();
    IFilterFactory::Pointer filterFactory = fm->getFactoryFromClassName(m_FilterName);
    DREAM3D_REQUIRE_VALID_POINTER(filterFactory)

    AbstractFilter::Pointer rotateFilter = filterFactory->create();
    DREAM3D_REQUIRE_VALID_POINTER(rotateFilter)
    rotateFilter->setDataContainerArray(dca);

    QVariant value;
    value.setValue(path);
    bool propWasSet = rotateFilter->setProperty("CellAttributeMatrixPath", value);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    value.setValue(0);
    propWasSet = rotateFilter->setProperty("RotationRepresentation", value);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    // Correct axis angle inputs

    value.setValue(FloatVec3Type(0.0f, 1.0f, 0.0f));
    propWasSet = rotateFilter->setProperty("RotationAxis", value);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    value.setValue(90.0f);
    propWasSet = rotateFilter->setProperty("RotationAngle", value);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    rotateFilter->preflight();
    int error = rotateFilter->getErrorCode();
    DREAM3D_REQUIRED(error, >=, 0)

    resetGeometry(matrix, imageGeom, tDims);

    // Non-normalized rotation axis should generate a warning

    value.setValue(FloatVec3Type(1.0f, 1.0f, 0.0f));
    propWasSet = rotateFilter->setProperty("RotationAxis", value);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    rotateFilter->preflight();
    int warning = rotateFilter->getWarningCode();
    DREAM3D_REQUIRED(warning, <, 0)

    resetGeometry(matrix, imageGeom, tDims);

    // Correct rotation matrix inputs

    value.setValue(1);
    propWasSet = rotateFilter->setProperty("RotationRepresentation", value);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    //  0 0 1
    //  0 1 0
    // -1 0 0

    std::vector<std::vector<double>> table = {{0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}};

    DynamicTableData tableData(table);

    value.setValue(tableData);
    propWasSet = rotateFilter->setProperty("RotationTable", value);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    rotateFilter->preflight();
    error = rotateFilter->getErrorCode();
    DREAM3D_REQUIRED(error, >=, 0)

    resetGeometry(matrix, imageGeom, tDims);

    // Inverse != Transpose error

    // 1 0 1
    // 0 1 0
    // 0 0 1

    table = {{1.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}};

    tableData.setTableData(table);

    value.setValue(tableData);
    propWasSet = rotateFilter->setProperty("RotationTable", value);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    rotateFilter->preflight();
    error = rotateFilter->getErrorCode();
    DREAM3D_REQUIRED(error, <, 0)

    resetGeometry(matrix, imageGeom, tDims);

    // Determinant != 1 error

    // 1 0 1
    // 0 1 0
    // 1 0 1

    table = {{1.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 1.0f}};

    tableData.setTableData(table);

    value.setValue(tableData);
    propWasSet = rotateFilter->setProperty("RotationTable", value);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    rotateFilter->preflight();
    error = rotateFilter->getErrorCode();
    DREAM3D_REQUIRED(error, <, 0)

    resetGeometry(matrix, imageGeom, tDims);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestRotateSampleRefFrameTest()
  {
    // DataContainerArray::Pointer dca = DataContainerArray::New();

    // DataContainerReader::Pointer dataContainerReader = DataContainerReader::New();
    // dataContainerReader->setDataContainerArray(dca);
    // dataContainerReader->setInputFile(UnitTest::RotateSampleRefFrameTest::TestFile1);
    // DataContainerArrayProxy proxy = dataContainerReader->readDataContainerArrayStructure(UnitTest::RotateSampleRefFrameTest::TestFile1);
    // dataContainerReader->setInputFileDataContainerArrayProxy(proxy);

    // dataContainerReader->execute();
    // int error = dataContainerReader->getErrorCode();
    // DREAM3D_REQUIRED(error, >=, 0)
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void operator()()
  {
    std::cout << "----Start RotateSampleRefFrameTest----\n";

    int err = EXIT_SUCCESS;
    DREAM3D_REGISTER_TEST(TestFilterAvailability());

    DREAM3D_REGISTER_TEST(TestFilterParameters())
    DREAM3D_REGISTER_TEST(TestRotateSampleRefFrameTest())

    DREAM3D_REGISTER_TEST(RemoveTestFiles())

    std::cout << "----End RotateSampleRefFrameTest----\n";
  }
};
